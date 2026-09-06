#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    connect(ui->timeHiddenSlider, &QSlider::valueChanged, this, &SettingsWindow::hideTimeSliderChanged);
    connect(ui->enableInteractionAutoHideBox, &QCheckBox::clicked, this, &SettingsWindow::enableInteractionAutoHideClicked);
    connect(ui->enableNewDayAutoHideBox, &QCheckBox::clicked, this, &SettingsWindow::enableNewDayAutoHideClicked);
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::setHideTimer(const int time)
{
    if (time >= 100 && time <= 10000)
    {
        ui->hideTimeLabel->setText(QString::number(time) + " ms");
        ui->timeHiddenSlider->setValue(time);
        emit hideTimeUpdated(time);
    }
}

void SettingsWindow::hideTimeSliderChanged(const int newValue)
{
    setHideTimer(newValue);
}

void SettingsWindow::enableInteractionAutoHideClicked(const bool enabled)
{
    ui->timeHiddenSlider->setEnabled(enabled);
    ui->hideTimeLabel->setEnabled(enabled);
    ui->enableInteractionAutoHideBox->setChecked(enabled);
    emit interactionAutoHideUpdated(enabled);
}

void SettingsWindow::enableNewDayAutoHideClicked(const bool enabled)
{
    ui->enableNewDayAutoHideBox->setChecked(enabled);
    emit newDayAutoHideUpdated(enabled);
}
