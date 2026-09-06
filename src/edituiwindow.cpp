#include "edituiwindow.h"
#include "ui_edituiwindow.h"

#include "imageframeedit.h"

#include <QMessageBox>
#include <QLayout>
#include <QFileInfo>

#include <QFileDialog>

EditUIWindow::EditUIWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditUIWindow),
    editModeEnabled(false)
{
    ui->setupUi(this);
    connect(this->ui->editSaveButton, &QPushButton::clicked, this, &EditUIWindow::toggleEditMode);
    connect(this->ui->searchEdit, &QLineEdit::textChanged, this, &EditUIWindow::filterList);
    connect(this->ui->CancelButton, &QPushButton::clicked, this, &EditUIWindow::cancelEditing);
    connect(this->ui->ResetButton, &QPushButton::clicked, this, &EditUIWindow::resetUI);
    connect(this->ui->loadButton, &QPushButton::clicked, this, &EditUIWindow::loadPressed);
    connect(this->ui->ExportButton, &QPushButton::clicked, this, &EditUIWindow::exportPressed);

    this->ui->CancelButton->setEnabled(false);

}

EditUIWindow::~EditUIWindow()
{
    delete ui;
}

void EditUIWindow::registerImageFrame(ImageFrame *imageFrame,
                                      const QString &name,
                                      const bool includeLabel)
{
    this->imageFrames.append(imageFrame);
    connect(this, &EditUIWindow::canceledPressed, imageFrame, &ImageFrame::resetToBeforeEdit);
    connect(this, &EditUIWindow::restorePressed, imageFrame, &ImageFrame::resetToDefault);
    ImageFrameEdit * imageFrameEdit = new ImageFrameEdit(imageFrame, name, includeLabel, this);
    this->ui->scrollAreaWidgetContents->layout()->addWidget(imageFrameEdit);
}

void EditUIWindow::setEditModeOnImageFrames(const bool enabled)
{
    for(ImageFrame *item : imageFrames)
    {
        item->setEditMode(enabled);
    }
}

void EditUIWindow::setEditModeOnEditList(const bool enabled)
{
    for(QObject* object : this->ui->scrollAreaWidgetContents->children())
    {
        ImageFrameEdit* widget = dynamic_cast<ImageFrameEdit*>(object);
        if(widget)
        {
            widget->setEditmode(enabled);
        }
    }
}

void EditUIWindow::setEditMode(const bool enabled)
{
    this->ui->CancelButton->setEnabled(enabled);
    if (enabled)
    {
        emit editModeChanged(true);
        this->ui->editSaveButton->setText("Save");
    }
    else
    {
        emit editModeChanged(false);
        this->ui->editSaveButton->setText("Edit");
        saveEditing();
    }
    editModeEnabled = enabled;
    setEditModeOnImageFrames(enabled);
    setEditModeOnEditList(enabled);
}

void EditUIWindow::toggleEditMode()
{
    setEditMode(!editModeEnabled);
}

void EditUIWindow::saveEditing()
{
    emit saveUiPressed();
}

void EditUIWindow::cancelEditing()
{
    if(editModeEnabled)
    {
        emit canceledPressed();
        toggleEditMode();
    }
}

void EditUIWindow::resetUI()
{

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  "Reset UI",
                                  "Do you want to reset the UI?",
                                  QMessageBox::Yes | QMessageBox::No);
    if(reply == QMessageBox::Yes)
    {
        if(!editModeEnabled)
        {
            toggleEditMode();
        }
        emit restorePressed();
    }
}


void EditUIWindow::filterList(const QString &searchString)
{
    for(QObject* object : this->ui->scrollAreaWidgetContents->children())
    {
        ImageFrameEdit* widget = dynamic_cast<ImageFrameEdit*>(object);
        if(widget && !(widget->getName().toLower().contains(searchString.toLower())))
        {
            widget->hide();
        }
        else if(widget)
        {
            widget->show();
        }
    }
}

void EditUIWindow::loadPressed()
{
    QString selectedFile = QFileDialog::getOpenFileName(this, "Select json file to load","","(*.json)");
    if(selectedFile.isEmpty())
    {
        return;
    }
    QFileInfo fileInfo(selectedFile);
    if(!fileInfo.exists() && !fileInfo.isReadable())
    {
        return;
    }
    if(!editModeEnabled)
    {
        this->setEditMode(true);
    }
    emit importFile(fileInfo.absoluteFilePath());
}

void EditUIWindow::exportPressed()
{
    QString selectedFile = QFileDialog::getSaveFileName(this, "Select path to save file","","(*.json)");

    if(selectedFile.isEmpty())
    {
        return;
    }
    if (!selectedFile.endsWith(".json"))
    {
        selectedFile.append(".json");
    }

    emit exportFile(selectedFile);
}
