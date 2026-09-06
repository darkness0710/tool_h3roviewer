#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

    void setHideTimer(const int time);

public slots:
    void enableInteractionAutoHideClicked(const bool enabled);
    void enableNewDayAutoHideClicked(const bool enabled);


private:
    Ui::SettingsWindow *ui;

private slots:
    void hideTimeSliderChanged(const int newValue);

signals:
    void hideTimeUpdated(const int activated);
    void interactionAutoHideUpdated(const bool enabled);
    void newDayAutoHideUpdated(const bool enabled);
};

#endif // SETTINGSWINDOW_H
