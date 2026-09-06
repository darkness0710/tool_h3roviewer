#include <QFile>
#include <QMouseEvent>
#include <QApplication>
#include <QStyle>
#include <QtMessageHandler>
#include <QJsonArray>
#include <QJsonDocument>
#include <QCryptographicHash>
#include <QMessageBox>

#include <iostream>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "memoryscanner.h"
#include "outlinelabel.h"

constexpr char SETTINGS_WINDOW_X[] = "windowX";
constexpr char SETTINGS_WINDOW_Y[] = "windowY";
constexpr char SETTINGS_WINDOW_WIDTH[] = "windowWidth";
constexpr char SETTINGS_WINDOW_HEIGHT[] = "windowHeight";
constexpr char SETTINGS_UI_LAYOUT[] = "uiLayout";
constexpr char SETTINGS_AUTO_HIDE_INTERACTIONS_ENABLED[] = "autoHideEnabled";
constexpr char SETTINGS_AUTO_HIDE_NEW_DAY_ENABLED[] = "autoHideNewDayEnabled";
constexpr char SETTINGS_AUTO_HIDE_TIME[] = "autoHideTime";
constexpr char SETTINGS_DEFAULT_LAYOUT_HASH[] = "defaultLayoutHash";

static AboutWindow *globalAboutWindow = {nullptr};
static bool debugActivated = {false};

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)
    QByteArray localMsg = msg.toLocal8Bit();
    if (globalAboutWindow == nullptr)
    {
        return;
    }

    switch (type) {
    case QtDebugMsg:
        if (debugActivated)
        {
            globalAboutWindow->addToLog("Debug: " + msg);
            fprintf(stdout, "Debug: %s\n", localMsg.constData());
        }
        break;
    case QtInfoMsg:
        globalAboutWindow->addToLog("Info: " + msg);
        fprintf(stdout, "Info: %s\n", localMsg.constData());
        break;
    case QtWarningMsg:
        globalAboutWindow->addToLog("Warning: " + msg);
        fprintf(stderr, "Warning: %s\n", localMsg.constData());
        break;
    case QtCriticalMsg:
        globalAboutWindow->addToLog("Critical: " + msg);
        fprintf(stderr, "Critical: %s\n", localMsg.constData());
        break;
    case QtFatalMsg:
        globalAboutWindow->addToLog("Fatal: " + msg);
        fprintf(stderr, "Fatal: %s\n", localMsg.constData());
        break;
    }
    std::cout << std::flush;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      borderless(true),
      holdingLeft(false),
      about(this),
      settings(this),
      storedSettings("H3roViewer","H3roViewer"),
      autoHideOnInteractionEnabled(true),
      autoHideOnNewDayEnabled(true),
      isWindowHidden(false)
{
    ui->setupUi(this);

    qInstallMessageHandler(myMessageOutput);
    globalAboutWindow = &(this->about);

    removeManifest();
    connectEvents();
    setupTrayIcon();

    configureUiElements();
    registerUiElements();

    connectProviders();

    connect(&(this->guiAutoHideTimer), &QTimer::timeout, this, &MainWindow::restoreGUI);
    this->guiAutoHideTimer.setSingleShot(true);
    this->guiAutoHideTimer.setInterval(1500);

    this->ui->centralwidget->setStyleSheet("#centralwidget{background-image: url(:/images/main/background.png)}");

    /**
     * In some rare occurence, the restore event after an auto-hide event does
     * not properly redraw the window. This timer will trigger another
     * delayed redraw event to work around that issue.
     */
    connect(&(this->refreshGuiTimer), &QTimer::timeout, this, qOverload<>(&MainWindow::update));
    this->refreshGuiTimer.setSingleShot(true);
    this->refreshGuiTimer.setInterval(100);
}


MainWindow::~MainWindow()
{
    saveSettings();
    this->trayIcon.hide();
    this->trayIcon.deleteLater();
    delete ui;
}

void MainWindow::removeManifest()
{
    QFile manifest(QCoreApplication::applicationDirPath() + "/h3roviewer.exe.manifest");
    if (manifest.exists())
    {
        manifest.remove();
    }
}

void MainWindow::connectEvents()
{
    connect(&this->scanner, &MemoryScanner::playerUpdated, this, &MainWindow::refreshPlayerUI);
    connect(&this->scanner, &MemoryScanner::heroUpdated, this, &MainWindow::refreshHeroUI);

    connect(this->ui->actionSettings, &QAction::triggered, &this->settings, &SettingsWindow::show);
    connect(this->ui->actionEditUI, &QAction::triggered, &this->editUI, &EditUIWindow::show);
    connect(this->ui->actionAbout, &QAction::triggered, &this->about, &AboutWindow::show);
    connect(this->ui->actionExit, &QAction::triggered, this, &MainWindow::exit);

    connect(&(this->about), &AboutWindow::activateDebugMessages, this, &MainWindow::updateDebugStatus);

    connect(&(this->settings), &SettingsWindow::hideTimeUpdated, this, &MainWindow::setGUIHideTime);
    connect(&(this->settings), &SettingsWindow::interactionAutoHideUpdated, this, &MainWindow::enableInteractionAutoHide);
    connect(&(this->settings), &SettingsWindow::newDayAutoHideUpdated, this, &MainWindow::enableNewDayAutoHide);
    connect(&(this->settings), &SettingsWindow::redetectOffsetsRequested, &this->scanner, &MemoryScanner::rescanMemoryOffsets);
    connect(&this->scanner, &MemoryScanner::offsetStatusChanged, &this->settings, &SettingsWindow::setOffsetStatus);

    connect(&(this->editUI), &EditUIWindow::saveUiPressed, this, &MainWindow::saveUI);
    connect(&(this->editUI), &EditUIWindow::importFile, this, &MainWindow::importUiFile);
    connect(&(this->editUI), &EditUIWindow::exportFile, this, &MainWindow::exportUI);
    connect(&(this->editUI), &EditUIWindow::editModeChanged, this, &MainWindow::hideTopLayer);
}

void MainWindow::setupTrayIcon()
{
    this->contextMenu.addAction(ui->actionSettings);
    this->contextMenu.addAction(ui->actionExit);
    this->contextMenu.setContextMenuPolicy(Qt::CustomContextMenu);

    this->trayIcon.setIcon(QIcon(":/images/items/Artifact_Power_of_the_Dragon_Father.gif"));
    this->trayIcon.setToolTip("H3ro Viewer");
    this->trayIcon.show();

    connect(&this->trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::openContextMenu);
}

void MainWindow::configureUiElements()
{
    ui->heroNameLabel->label.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    //ui->heroNameLabel->setBackgroundEnabled(false);
    ui->heroNameLabel->label.setFontColor("FFEE9D");
    //ui->heroNameLabel->label.setFontColor("FFFFFF");
    ui->levelLabel->label.setFontColor("FFEE9D");
    ui->levelLabel->setBackgroundEnabled(false);
    ui->levelProgressImage->setBackgroundEnabled(false);

    ui->moraleImage->setBackgroundEnabled(false);
    ui->luckImage->setBackgroundEnabled(false);

    ui->attackLabel->setBackgroundEnabled(false);
    ui->attackImage->setImage(":/images/main/AttackSkill.png");
    ui->attackModLabel->setBackgroundEnabled(false);

    ui->defLabel->setBackgroundEnabled(false);
    ui->defenceImage->setImage(":/images/main/DefenceSkill.png");
    ui->defModLabel->setBackgroundEnabled(false);

    ui->powerLabel->setBackgroundEnabled(false);
    ui->powerImage->setImage(":/images/main/PowerSkill.png");
    ui->powerModLabel->setBackgroundEnabled(false);

    ui->knowledgeLabel->setBackgroundEnabled(false);
    ui->knowledgeImage->setImage(":/images/main/KnowledgeSkill.png");
    ui->knowledgeModLabel->setBackgroundEnabled(false);

    ui->attackLabel->label.setAlignment(      Qt::AlignHCenter | Qt::AlignTop);
    ui->attackModLabel->label.setAlignment(   Qt::AlignHCenter | Qt::AlignTop);
    ui->defLabel->label.setAlignment(         Qt::AlignHCenter | Qt::AlignTop);
    ui->defModLabel->label.setAlignment(      Qt::AlignHCenter | Qt::AlignTop);
    ui->powerLabel->label.setAlignment(       Qt::AlignHCenter | Qt::AlignTop);
    ui->powerModLabel->label.setAlignment(    Qt::AlignHCenter | Qt::AlignTop);
    ui->knowledgeLabel->label.setAlignment(   Qt::AlignHCenter | Qt::AlignTop);
    ui->knowledgeModLabel->label.setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    ui->creature0Image->label.setTextAlignment(Qt::AlignBottom |  Qt::AlignRight);
    ui->creature1Image->label.setTextAlignment(Qt::AlignBottom |  Qt::AlignRight);
    ui->creature2Image->label.setTextAlignment(Qt::AlignBottom |  Qt::AlignRight);
    ui->creature3Image->label.setTextAlignment(Qt::AlignBottom |  Qt::AlignRight);
    ui->creature4Image->label.setTextAlignment(Qt::AlignBottom |  Qt::AlignRight);
    ui->creature5Image->label.setTextAlignment(Qt::AlignBottom |  Qt::AlignRight);
    ui->creature6Image->label.setTextAlignment(Qt::AlignBottom |  Qt::AlignRight);

    ui->creature0Image->label.enableOutline(true);
    ui->creature1Image->label.enableOutline(true);
    ui->creature2Image->label.enableOutline(true);
    ui->creature3Image->label.enableOutline(true);
    ui->creature4Image->label.enableOutline(true);
    ui->creature5Image->label.enableOutline(true);
    ui->creature6Image->label.enableOutline(true);

    ui->creature0LossImage->label.setTextAlignment(Qt::AlignBottom | Qt::AlignRight);
    ui->creature1LossImage->label.setTextAlignment(Qt::AlignBottom | Qt::AlignRight);
    ui->creature2LossImage->label.setTextAlignment(Qt::AlignBottom | Qt::AlignRight);
    ui->creature3LossImage->label.setTextAlignment(Qt::AlignBottom | Qt::AlignRight);
    ui->creature4LossImage->label.setTextAlignment(Qt::AlignBottom | Qt::AlignRight);
    ui->creature5LossImage->label.setTextAlignment(Qt::AlignBottom | Qt::AlignRight);
    ui->creature6LossImage->label.setTextAlignment(Qt::AlignBottom | Qt::AlignRight);

    ui->creature0LossImage->label.setStyleSheet("QLabel{color:#FF0000}");
    ui->creature1LossImage->label.setStyleSheet("QLabel{color:#FF0000}");
    ui->creature2LossImage->label.setStyleSheet("QLabel{color:#FF0000}");
    ui->creature3LossImage->label.setStyleSheet("QLabel{color:#FF0000}");
    ui->creature4LossImage->label.setStyleSheet("QLabel{color:#FF0000}");
    ui->creature5LossImage->label.setStyleSheet("QLabel{color:#FF0000}");
    ui->creature6LossImage->label.setStyleSheet("QLabel{color:#FF0000}");

    ui->creature0LossImage->label.enableOutline(true);
    ui->creature1LossImage->label.enableOutline(true);
    ui->creature2LossImage->label.enableOutline(true);
    ui->creature3LossImage->label.enableOutline(true);
    ui->creature4LossImage->label.enableOutline(true);
    ui->creature5LossImage->label.enableOutline(true);
    ui->creature6LossImage->label.enableOutline(true);

    ui->creature0LossImage->setBackgroundEnabled(false);
    ui->creature1LossImage->setBackgroundEnabled(false);
    ui->creature2LossImage->setBackgroundEnabled(false);
    ui->creature3LossImage->setBackgroundEnabled(false);
    ui->creature4LossImage->setBackgroundEnabled(false);
    ui->creature5LossImage->setBackgroundEnabled(false);
    ui->creature6LossImage->setBackgroundEnabled(false);

    ui->spAmountImage->label.setTextAlignment (Qt::AlignHCenter | Qt::AlignTop);
    ui->spAmountImage->setBackgroundEnabled(false);

    ui->movementPointsIcon->setImage(":/images/main/MovementsIcon.png");
    ui->movementPointsIcon->setBackgroundEnabled(false);
    ui->movementPointsImage->label.setFontColor("FFEE9D");
    ui->movementPointsImage->setBackgroundEnabled(false);

    ui->spIconImage->setImage(":/images/main/spellPoints.png");
    ui->spIconImage->setBackgroundEnabled(false);

    ui->stabledImage->setBackgroundEnabled(false);
    ui->trailblazerImage->setBackgroundEnabled(false);
}

void MainWindow::connectProviders()
{
    connect(&dataProviders.playerColor,
            &playerColorProvider::updated,
            this,
            &MainWindow::updatePlayerColor);

    connect(&dataProviders.status,
            &StatusProvider::updated,
            this,
            &MainWindow::updateWindowState);

    connectFrameWithProvider(this->ui->helmetImage,   &dataProviders.helmet);
    connectFrameWithProvider(this->ui->neckImage,     &dataProviders.neck);
    connectFrameWithProvider(this->ui->chestImage,    &dataProviders.chest);
    connectFrameWithProvider(this->ui->weaponImage,   &dataProviders.weapon);
    connectFrameWithProvider(this->ui->shieldImage,   &dataProviders.shield);
    connectFrameWithProvider(this->ui->feetImage,     &dataProviders.boots);
    connectFrameWithProvider(this->ui->capeImage,     &dataProviders.cape);
    connectFrameWithProvider(this->ui->catapultImage, &dataProviders.catapult);
    connectFrameWithProvider(this->ui->firstAidImage, &dataProviders.firstAidTent);
    connectFrameWithProvider(this->ui->ammoCartImage, &dataProviders.ammoCart);
    connectFrameWithProvider(this->ui->balistaImage,  &dataProviders.balista);

    connectFrameWithProvider(this->ui->rightRingImage, &dataProviders.rightRing);
    connectFrameWithProvider(this->ui->leftRingImage,  &dataProviders.leftRing);
    connectFrameWithProvider(this->ui->arti0Image,     &dataProviders.arti1);
    connectFrameWithProvider(this->ui->arti1Image,     &dataProviders.arti2);
    connectFrameWithProvider(this->ui->arti2Image,     &dataProviders.arti3);
    connectFrameWithProvider(this->ui->arti3Image,     &dataProviders.arti4);
    connectFrameWithProvider(this->ui->arti4Image,     &dataProviders.arti5);

    connectFrameWithProvider(this->ui->heroPotraitImage,       &dataProviders.heroPotrait);
    connectLabelWithProvider(this->ui->heroNameLabel,          &dataProviders.heroName);
    connectLabelWithProvider(this->ui->movementPointsImage,    &dataProviders.heroMovePoints);
    connectFrameWithProvider(this->ui->heroSpecialty,          &dataProviders.specialty);
    connectFrameWithProvider(this->ui->spellbookImage,         &dataProviders.spellBook);
    connectFrameWithProvider(this->ui->moraleImage,            &dataProviders.morale);
    connectFrameWithProvider(this->ui->luckImage,              &dataProviders.luck);


    connectFrameWithProvider(this->ui->skill0Image, &dataProviders.skillSlot0);
    connectFrameWithProvider(this->ui->skill1Image, &dataProviders.skillSlot1);
    connectFrameWithProvider(this->ui->skill2Image, &dataProviders.skillSlot2);
    connectFrameWithProvider(this->ui->skill3Image, &dataProviders.skillSlot3);
    connectFrameWithProvider(this->ui->skill4Image, &dataProviders.skillSlot4);
    connectFrameWithProvider(this->ui->skill5Image, &dataProviders.skillSlot5);
    connectFrameWithProvider(this->ui->skill6Image, &dataProviders.skillSlot6);
    connectFrameWithProvider(this->ui->skill7Image, &dataProviders.skillSlot7);


    connectLabelWithProvider(this->ui->attackLabel,    &dataProviders.attackSkill);
    connectLabelWithProvider(this->ui->attackModLabel, &dataProviders.attackModifiedSkill);
    connect(&this->dataProviders.attackModifiedSkill,
            &DeltaStatsProvider::labelColor,
            &this->ui->attackModLabel->label,
            &OutlineLabel::setStyleSheet);
    connectLabelWithProvider(this->ui->defLabel,    &dataProviders.defenceSkill);
    connectLabelWithProvider(this->ui->defModLabel, &dataProviders.defenceModifiedSkill);
    connect(&this->dataProviders.defenceModifiedSkill,
            &DeltaStatsProvider::labelColor,
            &this->ui->defModLabel->label,
            &OutlineLabel::setStyleSheet);
    connectLabelWithProvider(this->ui->powerLabel,    &dataProviders.powerSkill);
    connectLabelWithProvider(this->ui->powerModLabel, &dataProviders.powerModifiedSkill);
    connect(&this->dataProviders.powerModifiedSkill,
            &DeltaStatsProvider::labelColor,
            &this->ui->powerModLabel->label,
            &OutlineLabel::setStyleSheet);
    connectLabelWithProvider(this->ui->knowledgeLabel,    &dataProviders.knowledgeSkill);
    connectLabelWithProvider(this->ui->knowledgeModLabel, &dataProviders.knowledgeModifiedSkill);
    connect(&this->dataProviders.knowledgeModifiedSkill,
            &DeltaStatsProvider::labelColor,
            &this->ui->knowledgeModLabel->label,
            &OutlineLabel::setStyleSheet);
    connectLabelWithProvider(this->ui->spAmountImage, &dataProviders.spellPoints);
    connectLabelWithProvider(this->ui->levelLabel,    &dataProviders.heroLevel);
    connect(&dataProviders.heroLevelProgress,
            &LevelProgressProvider::updated,
            this->ui->levelProgressImage,
            &ImageFrame::setProgress);

    connectLabelWithProvider(this->ui->creature0Image, &dataProviders.creature0Count);
    connectLabelWithProvider(this->ui->creature1Image, &dataProviders.creature1Count);
    connectLabelWithProvider(this->ui->creature2Image, &dataProviders.creature2Count);
    connectLabelWithProvider(this->ui->creature3Image, &dataProviders.creature3Count);
    connectLabelWithProvider(this->ui->creature4Image, &dataProviders.creature4Count);
    connectLabelWithProvider(this->ui->creature5Image, &dataProviders.creature5Count);
    connectLabelWithProvider(this->ui->creature6Image, &dataProviders.creature6Count);

    connectLabelWithProvider(this->ui->creature0LossImage, &dataProviders.creature0Loss);
    connectLabelWithProvider(this->ui->creature1LossImage, &dataProviders.creature1Loss);
    connectLabelWithProvider(this->ui->creature2LossImage, &dataProviders.creature2Loss);
    connectLabelWithProvider(this->ui->creature3LossImage, &dataProviders.creature3Loss);
    connectLabelWithProvider(this->ui->creature4LossImage, &dataProviders.creature4Loss);
    connectLabelWithProvider(this->ui->creature5LossImage, &dataProviders.creature5Loss);
    connectLabelWithProvider(this->ui->creature6LossImage, &dataProviders.creature6Loss);

    connectFrameWithProvider(this->ui->creature0Image, &dataProviders.creature0Image);
    connectFrameWithProvider(this->ui->creature1Image, &dataProviders.creature1Image);
    connectFrameWithProvider(this->ui->creature2Image, &dataProviders.creature2Image);
    connectFrameWithProvider(this->ui->creature3Image, &dataProviders.creature3Image);
    connectFrameWithProvider(this->ui->creature4Image, &dataProviders.creature4Image);
    connectFrameWithProvider(this->ui->creature5Image, &dataProviders.creature5Image);
    connectFrameWithProvider(this->ui->creature6Image, &dataProviders.creature6Image);

    connectFrameWithProvider(this->ui->stabledImage, &dataProviders.stabled);
    connectFrameWithProvider(this->ui->trailblazerImage, &dataProviders.trailblaze);

}

void MainWindow::registerUiElements()
{
    editUI.registerImageFrame(this->ui->spIconImage, "Spell points");
    editUI.registerImageFrame(this->ui->spAmountImage, "Spell points Amount", true);

    editUI.registerImageFrame(this->ui->heroSpecialty, "Hero Specialty");

    editUI.registerImageFrame(this->ui->attackImage, "Attack Icon");
    editUI.registerImageFrame(this->ui->defenceImage, "Defence Icon");
    editUI.registerImageFrame(this->ui->powerImage, "Power Icon");
    editUI.registerImageFrame(this->ui->knowledgeImage, "Knowledge Icon");

    editUI.registerImageFrame(this->ui->helmetImage, "Helmet");
    editUI.registerImageFrame(this->ui->neckImage, "Neck");
    editUI.registerImageFrame(this->ui->chestImage, "Chest");
    editUI.registerImageFrame(this->ui->leftRingImage, "Left Ring");
    editUI.registerImageFrame(this->ui->weaponImage, "Weapon");
    editUI.registerImageFrame(this->ui->shieldImage, "Shield");
    editUI.registerImageFrame(this->ui->rightRingImage, "Right Ring");
    editUI.registerImageFrame(this->ui->capeImage, "Cape");
    editUI.registerImageFrame(this->ui->feetImage, "Boots");
    editUI.registerImageFrame(this->ui->arti0Image, "Artifact 1");
    editUI.registerImageFrame(this->ui->arti1Image, "Artifact 2");
    editUI.registerImageFrame(this->ui->arti2Image, "Artifact 3");
    editUI.registerImageFrame(this->ui->arti3Image, "Artifact 4");
    editUI.registerImageFrame(this->ui->arti4Image, "Artifact 5");
    editUI.registerImageFrame(this->ui->creature0Image, "Creature 1", true);
    editUI.registerImageFrame(this->ui->creature1Image, "Creature 2", true);
    editUI.registerImageFrame(this->ui->creature2Image, "Creature 3", true);
    editUI.registerImageFrame(this->ui->creature3Image, "Creature 4", true);
    editUI.registerImageFrame(this->ui->creature4Image, "Creature 5", true);
    editUI.registerImageFrame(this->ui->creature5Image, "Creature 6", true);
    editUI.registerImageFrame(this->ui->creature6Image, "Creature 7", true);

    editUI.registerImageFrame(this->ui->creature0LossImage, "Creature loss 1", true);
    editUI.registerImageFrame(this->ui->creature1LossImage, "Creature loss 2", true);
    editUI.registerImageFrame(this->ui->creature2LossImage, "Creature loss 3", true);
    editUI.registerImageFrame(this->ui->creature3LossImage, "Creature loss 4", true);
    editUI.registerImageFrame(this->ui->creature4LossImage, "Creature loss 5", true);
    editUI.registerImageFrame(this->ui->creature5LossImage, "Creature loss 6", true);
    editUI.registerImageFrame(this->ui->creature6LossImage, "Creature loss 7", true);

    editUI.registerImageFrame(this->ui->catapultImage, "Catapult");
    editUI.registerImageFrame(this->ui->firstAidImage, "First Aid");
    editUI.registerImageFrame(this->ui->balistaImage, "Balista");
    editUI.registerImageFrame(this->ui->ammoCartImage, "Ammo Cart");
    editUI.registerImageFrame(this->ui->heroPotraitImage, "Hero Portrait");
    editUI.registerImageFrame(this->ui->movementPointsIcon, "Movement point icon");
    editUI.registerImageFrame(this->ui->movementPointsImage, "Movement point label", true);
    editUI.registerImageFrame(this->ui->spellbookImage, "Spell Book");
    editUI.registerImageFrame(this->ui->levelProgressImage, "XP");
    editUI.registerImageFrame(this->ui->levelLabel, "Level", true);
    editUI.registerImageFrame(this->ui->heroNameLabel, "Hero Name", true);
    editUI.registerImageFrame(this->ui->attackLabel, "Attack", true);
    editUI.registerImageFrame(this->ui->attackModLabel, "Attack Modifier", true);
    editUI.registerImageFrame(this->ui->defLabel, "Defence", true);
    editUI.registerImageFrame(this->ui->defModLabel, "Defence Modifier", true);
    editUI.registerImageFrame(this->ui->powerLabel, "Power", true);
    editUI.registerImageFrame(this->ui->powerModLabel, "Power Modifier", true);
    editUI.registerImageFrame(this->ui->knowledgeLabel, "Knowledge", true);
    editUI.registerImageFrame(this->ui->knowledgeModLabel, "Knowledge Modifier", true);
    editUI.registerImageFrame(this->ui->moraleImage, "Morale");
    editUI.registerImageFrame(this->ui->luckImage, "Luck");

    editUI.registerImageFrame(this->ui->skill0Image, "Skill 1");
    editUI.registerImageFrame(this->ui->skill1Image, "Skill 2");
    editUI.registerImageFrame(this->ui->skill2Image, "Skill 3");
    editUI.registerImageFrame(this->ui->skill3Image, "Skill 4");
    editUI.registerImageFrame(this->ui->skill4Image, "Skill 5");
    editUI.registerImageFrame(this->ui->skill5Image, "Skill 6");
    editUI.registerImageFrame(this->ui->skill6Image, "Skill 7");
    editUI.registerImageFrame(this->ui->skill7Image, "Skill 8");

    editUI.registerImageFrame(this->ui->stabledImage, "Stabled");
    editUI.registerImageFrame(this->ui->trailblazerImage, "Trailblaze");
}


bool MainWindow::hasDefaultLayoutBeenUpdated()
{
    QCryptographicHash hash(QCryptographicHash::Sha256);
    QByteArray uiHash(hash.hash(generateSave(), QCryptographicHash::Sha256));
    QByteArray storedDefaultUIHash = this->storedSettings.value(SETTINGS_DEFAULT_LAYOUT_HASH).toByteArray();
    this->storedSettings.setValue(SETTINGS_DEFAULT_LAYOUT_HASH, uiHash);
    QString hasStoredUi = this->storedSettings.value(SETTINGS_UI_LAYOUT).toString();

    // Never opened the viewer before.
    if(hasStoredUi.isEmpty())
    {
        return false;
    }
    // Has opened legacy viewer before.
    else if (storedDefaultUIHash.isEmpty())
    {
        return true;
    }
    else
    {
        return storedDefaultUIHash != uiHash;
    }
}

void MainWindow::loadSettings()
{
    /*
    if(this->storedSettings.contains(SETTINGS_WINDOW_X))
    {
        move(this->storedSettings.value(SETTINGS_WINDOW_X).toInt(), y());
    }
    if(this->storedSettings.contains(SETTINGS_WINDOW_Y))
    {
        move(x(), this->storedSettings.value(SETTINGS_WINDOW_Y).toInt());
    }

    if(this->storedSettings.contains(SETTINGS_WINDOW_WIDTH))
    {
        QRect geometry = this->geometry();
        geometry.setWidth(this->storedSettings.value(SETTINGS_WINDOW_WIDTH).toInt());
        this->setGeometry(geometry);
    }
    if(this->storedSettings.contains(SETTINGS_WINDOW_HEIGHT))
    {
        QRect geometry = this->geometry();
        geometry.setHeight(this->storedSettings.value(SETTINGS_WINDOW_HEIGHT).toInt());
        this->setGeometry(geometry);
    }
    */
    if(this->storedSettings.contains(SETTINGS_UI_LAYOUT))
    {
        loadUI(this->storedSettings.value(SETTINGS_UI_LAYOUT).toByteArray());
    }
    if(this->storedSettings.contains(SETTINGS_AUTO_HIDE_INTERACTIONS_ENABLED))
    {
        settings.enableInteractionAutoHideClicked(this->storedSettings.value(SETTINGS_AUTO_HIDE_INTERACTIONS_ENABLED).toBool());
    }
    if(this->storedSettings.contains(SETTINGS_AUTO_HIDE_TIME))
    {
        settings.setHideTimer(this->storedSettings.value(SETTINGS_AUTO_HIDE_TIME).toInt());
    }
    if(this->storedSettings.contains(SETTINGS_AUTO_HIDE_NEW_DAY_ENABLED))
    {
        settings.enableNewDayAutoHideClicked(this->storedSettings.value(SETTINGS_AUTO_HIDE_NEW_DAY_ENABLED).toBool());
    }
}


QList<ImageFrame *> MainWindow::getAllImageFrames()
{
    QList<ImageFrame *> allImageFrames;

    QList<QObject*> mainChildren = this->centralWidget()->children();
    QList<QObject*> allChildren;

    // This should be recursive...
    for(QObject *child : mainChildren)
    {
        allChildren.append(child->children());
    }
    for(QObject *child : allChildren)
    {
        ImageFrame *editItem = dynamic_cast<ImageFrame*>(child);
        if(editItem)
        {
            allImageFrames.append(editItem);
        }
    }
    return allImageFrames;
}

void MainWindow::initImageFrames()
{
    for(ImageFrame *child : getAllImageFrames())
    {
        child->init();
    }
}

void MainWindow::init()
{
    initImageFrames();
    this->ui->spAmountImage->label.setFontSize("21");
    this->ui->spAmountImage->label.setCurrentFontDefault();
    this->ui->heroNameLabel->label.setFontSize("18");
    this->ui->heroNameLabel->label.setCurrentFontDefault();
    this->ui->levelLabel->label.setFontSize("18");
    this->ui->levelLabel->label.setCurrentFontDefault();
    this->ui->movementPointsImage->label.setFontSize("19");
    this->ui->movementPointsImage->label.setCurrentFontDefault();

    toggleToolBar();

    // hasDefaultLayoutBeenUpdated has to be run before loadSettings as it will change
    // the layout to check against.
    bool defaultUIUpdated = hasDefaultLayoutBeenUpdated();
    loadSettings();

    if(defaultUIUpdated)
    {
        QMessageBox updatedView(QMessageBox::Warning,
                                "Updated default view",
                                "The default view has been updated. It is "
                                "recommended to reset the layout.\n"
                                "Open the Edit view (Reset -> Save)?",
                                QMessageBox::Yes | QMessageBox::No);
        if(updatedView.exec() == QMessageBox::Yes)
        {
            editUI.show();
        }
    }
    this->scanner.init();
    this->ui->topLayer->setGeometry(0,
                                    0,
                                    this->geometry().width(),
                                    this->geometry().height());

}


QByteArray MainWindow::generateSave(const bool compact)
{
    QJsonArray allSaveItems;
    for(ImageFrame *child : getAllImageFrames())
    {
        allSaveItems.append(child->generateSave());
    }
    QJsonObject widgets;
    widgets["widgets"] = allSaveItems;
    QJsonDocument doc(widgets);

    return doc.toJson(static_cast<QJsonDocument::JsonFormat>(compact));
}


void MainWindow::updateWindowState(const QString &state)
{
    if(state == QString::number(StatusState::NEW_DAY) &&
            this->autoHideOnNewDayEnabled)
    {
        refreshGuiTimer.stop();
        hideGUI(false);
    }
    else if(state == QString::number(StatusState::MAP_INTERACTION) &&
            this->autoHideOnInteractionEnabled)
    {
        refreshGuiTimer.stop();
        hideGUI(true);
    }
    else if(state == QString::number(StatusState::HERO))
    {
        restoreGUI();
    }
    /*
    else if(state == QString::number(STATUS_IN_MATCH))
    {
        restoreGUI();
    }
    else if(state == QString::number(STATUS_NOT_IN_MATCH))
    {
        hideGUI(false);
    }*/
}

void MainWindow::hideGUI(const bool useTimer)
{
    if(!this->isWindowHidden)
    {
        this->previousGeometry = this->geometry();
    }
    this->isWindowHidden = true;

    // Leave enough of the window visable to still display the stats
    QWidget::setGeometry(QWidget::geometry().x(),
                         QWidget::geometry().y(),
                         170,
                         55);

    if (useTimer)
    {
        this->guiAutoHideTimer.start();
    }
}

void MainWindow::restoreGUI()
{
    if (this->isWindowHidden)
    {
        this->isWindowHidden = false;
        this->setGeometry(this->previousGeometry);

        refreshGuiTimer.start();
    }
}

void MainWindow::setGUIHideTime(const int time)
{
    this->guiAutoHideTimer.setInterval(time);
}

void MainWindow::enableInteractionAutoHide(const bool enable)
{
    this->autoHideOnInteractionEnabled = enable;
    if(!enable)
    {
        updateWindowState(QString::number(StatusState::HERO));
    }
}

void MainWindow::enableNewDayAutoHide(const bool enable)
{
    this->autoHideOnNewDayEnabled = enable;
    if(!enable)
    {
        updateWindowState(QString::number(StatusState::HERO));
    }
}


void MainWindow::openContextMenu(const QSystemTrayIcon::ActivationReason &reason)
{
    switch(reason)
    {
    case QSystemTrayIcon::Trigger:
        this->setWindowState( (windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        this->activateWindow();
        break;
    case QSystemTrayIcon::Context:
        this->contextMenu.exec(QCursor::pos());
        break;
    default:
        ;
    }
}

void MainWindow::hideTopLayer(bool hide)
{
    if(hide)
    {
        this->ui->topLayer->hide();
    }
    else
    {
        this->ui->topLayer->show();
    }
}
/*
void MainWindow::updateAfterRestore()
{
    this->update();
}
*/

void MainWindow::importUiFile(const QString &filePath)
{
    QFile file(filePath);
    bool success = file.open(QIODevice::ReadOnly);
    if(success == false)
    {
        return;
    }
    QByteArray input = file.readAll();
    file.close();
    loadUI(input);
}


void MainWindow::exportUI(const QString &path)
{
    QFile file(path);
    bool success = file.open(QIODevice::WriteOnly);
    if(success == false)
    {
        return;
    }
    if(file.isOpen())
    {
        file.write(generateSave());
    }
}

void MainWindow::saveUI()
{
    this->storedSettings.setValue(SETTINGS_UI_LAYOUT, generateSave(true));
}

void MainWindow::loadUI(const QByteArray &data)
{
    QList<QObject*> mainChildren = this->centralWidget()->children();
    QList<QObject*> allChildren;

    QJsonDocument document = QJsonDocument::fromJson(data);
    QJsonObject json = document.object();
    QJsonArray array = json["widgets"].toArray();

    // This should be recursive...
    for (const QObject *child : mainChildren)
    {
        allChildren.append(child->children());
    }

    for (const QJsonValue &objectv : array)
    {

        for (QObject *child : allChildren)
        {
            QJsonObject object = objectv.toObject();
            ImageFrame *editItem = dynamic_cast<ImageFrame*>(child);
            if(editItem && editItem->objectName() == object["name"].toString())
            {
                editItem->load(object);
            }
        }
    }
}


void MainWindow::updatePlayerColor(QString colorID)
{
    if(colorID.toInt() < 0 || colorID.toInt() > 8)
    {
        colorID = "0";
    }
    this->ui->mainFrame->setStyleSheet("#mainFrame{background-image: url(:/images/main/hero_background_" +
                                       colorID + ".png)}");
}

void MainWindow::connectFrameWithProvider(ImageFrame *frame, DataProvider *provider)
{
    connect(provider,
            &DataProvider::updated,
            frame,
            &ImageFrame::setImage);
}


void MainWindow::connectLabelWithProvider(ImageFrame *frame, DataProvider *provider)
{
    connect(provider,
            &DataProvider::updated,
            &frame->label,
            &OutlineLabel::setText);
}


void MainWindow::refreshPlayerUI()
{
    dataProviders.updatePlayer();
}

void MainWindow::refreshHeroUI()
{
    dataProviders.updateHero();
}

void MainWindow::updateDebugStatus(const bool activated)
{
    debugActivated = activated;
}

void MainWindow::toggleToolBar()
{
    // Menu height is 0 when the menu is hidden, stores it so we can use it
    // even when it is hidden.
    static int menuHeight = ui->menuBar->height();

    // this->y() will update when setting window flags, which is also easier to
    // do before setting geometry.
    int yPos = this->y();

    if(this->borderless)
    {
        QWidget::setWindowFlags( Qt::Window|
                                 Qt::FramelessWindowHint|
                                 Qt::WindowSystemMenuHint|
                                 Qt::CustomizeWindowHint);

        QWidget::setGeometry(this->x(),
                             yPos +
                             QApplication::style()->pixelMetric(QStyle::PM_TitleBarHeight) +
                             menuHeight,
                             this->width(),
                             this->height() - menuHeight);

        ui->menuBar->hide();
    }
    else
    {
        QWidget::setWindowFlags( Qt::Window|
                                 Qt::WindowTitleHint|
                                 Qt::WindowSystemMenuHint|
                                 Qt::WindowMinMaxButtonsHint|
                                 Qt::WindowCloseButtonHint|
                                 Qt::WindowFullscreenButtonHint);

        QWidget::setGeometry(this->x(),
                             yPos - menuHeight,
                             this->width(),
                             this->height() + menuHeight);

        ui->menuBar->show();
    }

    this->borderless = !(this->borderless);
    QWidget::show();
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        toggleToolBar();
    }

    if (event->button() == Qt::LeftButton )
    {
        this->dragingOldMousePos = event->globalPosition();
        this->holdingLeft = true;
    }

}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton )
    {
        this->holdingLeft = false;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (this->holdingLeft)
    {
        const QPointF delta = event->globalPosition() - this->dragingOldMousePos;
        move(this->x() + delta.x(),
             this->y() + delta.y());
        this->dragingOldMousePos = event->globalPosition();
    }
}


void MainWindow::saveSettings()
{
    if(!this->borderless)
    {
        toggleToolBar();
    }
    restoreGUI();
    saveUI();
    this->storedSettings.setValue(SETTINGS_WINDOW_X, this->x());
    this->storedSettings.setValue(SETTINGS_WINDOW_Y, this->y());
    this->storedSettings.setValue(SETTINGS_WINDOW_WIDTH, this->width());
    this->storedSettings.setValue(SETTINGS_WINDOW_HEIGHT, this->height());
    this->storedSettings.setValue(SETTINGS_AUTO_HIDE_INTERACTIONS_ENABLED, this->autoHideOnInteractionEnabled);
    this->storedSettings.setValue(SETTINGS_AUTO_HIDE_NEW_DAY_ENABLED, this->autoHideOnNewDayEnabled);
    this->storedSettings.setValue(SETTINGS_AUTO_HIDE_TIME, this->guiAutoHideTimer.interval());
    this->storedSettings.sync();
}


void MainWindow::exit()
{
    QApplication::exit(0);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
    exit();
}

