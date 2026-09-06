#ifndef IMAGEFRAME_H
#define IMAGEFRAME_H

#include <QFrame>
#include <QPixmap>
#include <QLabel>
#include <QRect>
#include <QSizeGrip>
#include <QHBoxLayout>

#include <QMouseEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QMoveEvent>

#include <QTimer>

#include <QPainter>
#include <QPen>

#include <QProgressBar>

#include "outlinelabel.h"
#include "imageframeborder.h"


class ImageFrame : public QFrame
{
    Q_OBJECT
public:
    ImageFrame(QWidget *parent);
    ~ImageFrame();

    /**
     * @brief generateSave Reads the current state of this object and generates
     * a JSON objet which can be used later to load.
     * @return A JSON string containing all relevant information for
     * saving/loading this object.
     */
    QJsonObject generateSave();

    /**
     * @brief load Parses the JSON object and sets parameters according to the
     * JSON object
     * @param object The JSON object to load
     */
    void load(const QJsonObject &object);

    /**
     * @brief setBackgroundEnabled Sets if the darker "hero outline" background
     * should be visable or not.
     * @param enabled Enables the background if set to true.
     */
    void setBackgroundEnabled(const bool enabled);

    /**
     * @brief init initializes parts of the object and its child which can't
     * be initialized in the constructor.
     */
    void init();


    OutlineLabel label;

private:

    /**
     * @brief drawEditOutline Draws the outline when edit mode is enabled.
     * Should only be called from paintEvent.
     */
    void drawEditOutline();


    QLabel backgroundFrame;
    QLabel imageFrame;

    QString imageName;
    QPixmap foregroundImage;

    ImageFrameBorder border;

    QSizeGrip sizeGrip;

    QPointF dragingOldWindowPos;

    bool backgroundEnabled;
    bool holdingLeft;
    bool editModeEnabled;

    QRect geometryBeforeEdit;
    bool visableBeforeEdit;
    QRect defaultGeometry;
    bool defaultVisibility;
    bool inConstuctor;

    QProgressBar progressBar;

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void moveEvent(QMoveEvent *event);

signals:
    /**
     * @brief imageFrameUpdated Signal to alert external object that this
     * object has been updated with new data, eg. geometry.
     * @param imageFrame A pointer to this object.
     */
    void imageFrameUpdated(ImageFrame * imageFrame);

public slots:

    /**
     * @brief setImage Sets the foreground image for this object.
     * @param filePath The path to the file to display.
     */
    void setImage(const QString &filePath);

    /**
     * @brief setProgress Makes an progressbar visable and set its value.
     * @param percentage Should only be integer values in string format.
     */
    void setProgress(const QString &percentage);

    /**
     * @brief updateGeometry Sets new geometry to the image frame.
     * @param geometry The new geometry to set.
     */
    void updateGeometry(const QRect &geometry);

    /**
     * @brief setEditMode Sets the image frame to edit mode
     * @param enabled If true, enables edit mode. If false, disables edit mode.
     */
    void setEditMode(const bool enabled);

    /**
     * @brief setVisable Makes the image frame either visable or invisable.
     * @param enabled If true, sets the image frame to visable. If false, sets
     * the image frame to invisible.
     */
    void setVisable(const bool enabled);

    /**
     * @brief resetToDefault Resets the image frame to its default configuraiton.
     */
    void resetToDefault();

    /**
     * @brief resetToBeforeEdit Cancels an edit mode and restores the image
     * frame to the state it was before the edit mode was enabled.
     */
    void resetToBeforeEdit();
};

#endif // IMAGEFRAME_H
