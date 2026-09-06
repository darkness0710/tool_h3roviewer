#include "aboutwindow.h"
#include "ui_aboutwindow.h"

#include <QApplication>

AboutWindow::AboutWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutWindow)
{
    ui->setupUi(this);
    ui->qtVersionLabel->setText(QString::number(QT_VERSION_MAJOR) + "." +
                                QString::number(QT_VERSION_MINOR) + "." +
                                QString::number(QT_VERSION_PATCH));

#ifdef __GNUC__
    ui->gccVersionLabel->setText(QString::number(__GNUC__) + "." +
                                 QString::number(__GNUC_MINOR__) + "." +
                                 QString::number(__GNUC_PATCHLEVEL__));

#endif

#ifdef _MSC_VER
    ui->compiler->setText("msvc:");
    QString msvcVersion(QString::number(_MSC_VER));
    msvcVersion = msvcVersion.mid(0,2) + "." + msvcVersion.mid(2,2);
    ui->gccVersionLabel->setText(msvcVersion);
#endif

    ui->appVersionLabel->setText(QApplication::applicationVersion());

    connect(ui->debugCheckBox, &QCheckBox::clicked, this, &AboutWindow::debugClicked);

}

AboutWindow::~AboutWindow()
{
    delete ui;
}

void AboutWindow::addToLog(const QString &text)
{
    ui->textEdit->append(text);
}

void AboutWindow::debugClicked(bool checked)
{
    emit activateDebugMessages(checked);
}
