#ifndef EDITUIWINDOW_H
#define EDITUIWINDOW_H

#include <QWidget>
#include <QList>

#include "imageframe.h"

namespace Ui {
class EditUIWindow;
}

class EditUIWindow : public QWidget
{
    Q_OBJECT

public:
    explicit EditUIWindow(QWidget *parent = nullptr);
    ~EditUIWindow();

    /**
     * @brief registerImageFrame Registers image frames so they can be
     * manipulated in the edit view.
     * @param imageFrame The image frame to register.
     * @param name The name/title of the view.
     * @param includeLabel If true, enables the lable edit in the edit view.
     */
    void registerImageFrame(ImageFrame *imageFrame,
                            const QString &name,
                            const bool includeLabel = false);

private:
    Ui::EditUIWindow *ui;

    QList <ImageFrame*> imageFrames;
    bool editModeEnabled;
    /**
     * @brief setEditModeOnImageFrames Enables/disables the edit mode for the
     * image frames themselves.
     * @param enabled If true, enables edit mode for all image frames registered.
     * If false, disables edit mode.
     */
    void setEditModeOnImageFrames(const bool enabled);

    /**
     * @brief setEditModeOnEditList Enables/disables the edit mode for all
     * image frames in the edit view.
     * @param enabled If true, enables edit mode for all items in the edit list.
     * If false, disables edit mode.
     */
    void setEditModeOnEditList(const bool enabled);
private slots:

    /**
     * @brief setEditMode Sets the edit mode for all items
     * @param enabled If true, enables edit mode for all items.
     */
    void setEditMode(const bool enabled);

    /**
     * @brief toggleEditMode Toggles between active and deactive edit mode.
     */
    void toggleEditMode();

    /**
     * @brief saveEditing Sends a signal which indicates that the current layout
     * should be saved.
     */
    void saveEditing();

    /**
     * @brief cancelEditing Sends a signal which indicates that the current
     * layout should be discarded and reverted back to before editing.
     */
    void cancelEditing();

    /**
     * @brief resetUI Sends a signal which indicates that the view should be
     * reset to the default layout.
     */
    void resetUI();

    /**
     * @brief filterList Slot to trigger filtering of the frame images in the
     * edit list.
     * @param searchString The search string to base the filter on.
     */
    void filterList(const QString &searchString);

    /**
     * @brief loadPressed Slot to handle when the load button is pressed.
     * Askes the user for a file path to load.
     */
    void loadPressed();

    /**
     * @brief exportPressed Slot to handle when the export button is pressed.
     * Askes the user for a file path to save to.
     */
    void exportPressed();

signals:
    /**
     * @brief saveUiPressed Signal which indicate that the current layout should
     * be saved.
     */
    void saveUiPressed();

    /**
     * @brief canceledPressed Signal which indicate that the current layout
     * should be reverted to before edit.
     */
    void canceledPressed();

    /**
     * @brief restorePressed Signal which indicate that the layout should return
     * to the default layout.
     */
    void restorePressed();

    /**
     * @brief exportFile Signal which indicates that the current layout should
     * be saved to a file.
     * @param path The file path to save to.
     */
    void exportFile(const QString &path);

    /**
     * @brief importFile Signal which indicates that a layout should be loaded
     * from a file.
     * @param path The file to load the layout from.
     */
    void importFile(const QString &path);

    /**
     * @brief editModeChanged Signal which is triggered when the GUI has changed
     * if it is in "edit mode".
     * @param isInEditMode true if the GUI has entered edit mode,
     * otherwise false.
     */
    void editModeChanged(bool isInEditMode);
};

#endif // EDITUIWINDOW_H
