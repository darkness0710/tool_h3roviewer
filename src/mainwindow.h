#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QPoint>
#include <QSettings>
#include <QTimer>
#include <QRect>
#include <QMenu>
#include <QEvent>

#include "providercollection.h"
#include "aboutwindow.h"
#include "settingswindow.h"

#include "edituiwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void init();


private slots:

    /**
     * @brief refreshPlayerUI Slot to trigger all data providers which are
     * related to the current player to run.
     */
    void refreshPlayerUI();

    /**
     * @brief refreshHeroUI Slot to trigger all data providers which are
     * related to the selected hero to run.
     */
    void refreshHeroUI();

    /**
     * @brief updateDebugStatus Toggles if debug messages should appear in the log.
     * @param activated If true, debug messages will appear
     */
    void updateDebugStatus(const bool activated);

    /**
     * @brief updateWindowState Hides/restores the main window view depending
     * on the argument to show information behind the window.
     * @param state Controlls if the window should hide itself or reveale itself
     */
    void updateWindowState(const QString &state);

    /**
     * @brief restoreGUI Restore the GUI to its full size
     */
    void restoreGUI();

    /**
     * @brief setGUIHideTime Slot to update the amount of the the GUI should
     * be hidden when interacting with map items
     * @param time the amount of time in ms the window should be hidden
     */
    void setGUIHideTime(const int time);

    /**
     * @brief enableInteractionAutoHide Slot to enable/disable the auto hide
     * feature for map interactions
     * @param enable If true, will enable the auto hide feature
     */
    void enableInteractionAutoHide(const bool enable);

    /**
     * @brief enableNewDayAutoHide Slot to enable/disable the auto hide
     * feature for new day animation
     * @param enable If true, will enable the auto hide feature
     */
    void enableNewDayAutoHide(const bool enable);

    /**
     * @brief openContextMenu The slot to be triggered when right-clicking
     * on the system tray icon.
     * @param reason The reason/action which was triggered.
     */
    void openContextMenu(const QSystemTrayIcon::ActivationReason &reason);

    /**
     * @brief hideTopLayer Hides/restores the top layer which normally is used
     * to catch all right click events (preventing other children from
     * receiving these events).
     * @param hide if set to true, hides the top layer and lets other child
     * widgets receive events. If false, shows the top layer which prevents
     * other children from receiving events.
     */
    void hideTopLayer(bool hide);

    /**
     * @brief importUiFile Opens and reads the given file and use it to
     * setup a previously saved ui state.
     * @param filePath The path to the ui file.
     */
    void importUiFile(const QString &filePath);

    /**
     * @brief exportUI Export the current UI setup to a file which can be
     * loaded manually.
     * @param path The path to export to.
     */
    void exportUI(const QString &path);

    /**
     * @brief saveUI Saves the current UI setup to the application global
     * permanent storage.
     */
    void saveUI();

    /**
     * @brief loadUI Reads the given data and dispatches the JSON objects which
     * corresponds to the widget names.
     * @param data The raw UI data in json format.
     */
    void loadUI(const QByteArray &data);

    /**
     * @brief generateSave Fetches all UI data into a raw JSON format
     * @param compact If set to true, the JSON format is packed, harder to
     * read for a human.
     * @return The raw JSON data of the UI state.
     */
    QByteArray generateSave(const bool compact = false);


private:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);
    void exit();

    /**
     * @brief removeManifest The manifest file made the executable request
     * admin privilage. This is no longer needed but the file must be removed
     * to stop asking for it.
     */
    void removeManifest();

    /**
     * @brief toggleToolBar Switches between having window border/menu and only
     * displaying the main content.
     */
    void toggleToolBar();

    /**
     * @brief hasDefaultLayoutBeenUpdated Checks if the default layout has been
     * updated.
     * Should be run before loadSettings as that function will change the
     * current layout.
     * @return true if the default layout has been updated. Otherwise returns false.
     */
    bool hasDefaultLayoutBeenUpdated();

    /**
     * @brief readSettings Reads from the settings file and updates values
     * depending on what it reads.
     */
    void loadSettings();

    /**
     * @brief writeSettings Writes the settings file to disk.
     */
    void saveSettings();

    /**
     * @brief getAllImageFrames Fetches all ImageFrame widgets in a list
     * @return A list containing all direct children of mainwindow of the
     * ImageFrame type
     */
    QList<ImageFrame*> getAllImageFrames();

    /**
     * @brief initImageFrames calls init on all direct children of
     * mainwindow of the ImageFrame type.
     */
    void initImageFrames();

    /**
     * @brief hideGUI Hides most of the GUI (to display what is under it).
     * @param useTimer If true, will automatically restore the GUI after
     * a set period of time, which is user configurable.
     */
    void hideGUI(const bool useTimer);

    /**
     * @brief updatePlayerColor Changes the background image to match
     * the current player color
     * @param colorID The color number
     * "1" = red, "2" = blue, "3" = tan, "4" = green, "5" = orange, "6" = purple
     * "7" = teal, "8" = pink
     */
    void updatePlayerColor(QString colorID);

    /**
     * @brief connectFrameWithProvider A wrapper to connect a ImageFrame
     * "setImage" with a signal from a data provider.
     * @param frame The ImageFrame to connect to.
     * @param provider The provider to receive data from.
     */
    void connectFrameWithProvider(ImageFrame *frame, DataProvider *provider);

    /**
     * @brief connectLabelWithProvider A wrapper to connect the slot "setText"
     * for the lable in a ImageFrame with a signal from a data provider.
     * @param frame The ImageFrame with the lable to connect to.
     * @param provider The provider to receive data from.
     */
    void connectLabelWithProvider(ImageFrame *frame, DataProvider *provider);

    /**
     * @brief registerUiElements Registers the ImageFrame to the "edit UI"
     * function which enables the user to change the layout.
     */
    void registerUiElements();

    /**
     * @brief connectProviders Sets up all the UI elements with the matching
     * providers.
     */
    void connectProviders();

    /**
     * @brief configureUiElements Setup the bahavior for the UI elements like
     * enable background and text configuraiton
     */
    void configureUiElements();

    /**
     * @brief setupTrayIcon Sets up the tray icon.
     */
    void setupTrayIcon();

    /**
     * @brief connectEvents Connects all other events not relatad to the
     * ImageFrames.
     */
    void connectEvents();

    Ui::MainWindow *ui;

    MemoryScanner scanner;
    ProviderCollection dataProviders;

    bool borderless;
    QPointF dragingOldMousePos;
    bool holdingLeft;

    EditUIWindow editUI;
    AboutWindow about;
    SettingsWindow settings;
    QSettings storedSettings;

    bool autoHideOnInteractionEnabled;
    bool autoHideOnNewDayEnabled;
    QTimer guiAutoHideTimer;
    QRect previousGeometry;

    QTimer refreshGuiTimer;

    QSystemTrayIcon trayIcon;
    QMenu contextMenu;
    bool isWindowHidden;
};
#endif // MAINWINDOW_H
