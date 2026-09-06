#include "imageframe.h"

#include <QRect>
#include <QDebug>

#include <QPixmap>
#include <QFile>
#include <QPoint>

#include <QPainter>
#include <QPen>

#include <QJsonDocument>
#include <QJsonArray>

constexpr char SAVE_X[] = "x";
constexpr char SAVE_Y[] = "y";
constexpr char SAVE_WIDTH[] = "width";
constexpr char SAVE_HEIGHT[] = "height";
constexpr char SAVE_LABEL[] = "label";

constexpr int BORDER_SIZE = 2;

ImageFrame::ImageFrame(QWidget *parent):
    QFrame(parent),
    label(this),
    backgroundFrame(this),
    imageFrame(this),
    foregroundImage(":/images/main/hero_foreground.png"),
    border(BORDER_SIZE, this),
    sizeGrip(this),
    backgroundEnabled(true),
    holdingLeft(false),
    editModeEnabled(false),
    visableBeforeEdit(true),
    defaultVisibility(true),
    inConstuctor(true),
    progressBar(this)
{
    setEditMode(false);
    this->setWindowFlags(Qt::SubWindow);
    sizeGrip.setGeometry(this->width()-sizeGrip.width(),
                         this->height()-sizeGrip.height(),
                         10,
                         10);

    this->label.raise();
    this->sizeGrip.raise();
    inConstuctor = false;

}

ImageFrame::~ImageFrame()
{

}

QJsonObject ImageFrame::generateSave()
{
    QJsonObject jsonSave;
    jsonSave[SAVE_X] = this->x();
    jsonSave[SAVE_Y] = this->y();
    jsonSave[SAVE_WIDTH] = this->width();
    jsonSave[SAVE_HEIGHT] = this->height();
    jsonSave[SAVE_NAME] = this->objectName();
    jsonSave[SAVE_HIDDEN] = this->isHidden();
    jsonSave[SAVE_LABEL] = this->label.generateSave();
    return jsonSave;
}

void ImageFrame::load(const QJsonObject &object)
{
    if(OutlineLabel::getString(object, SAVE_NAME, "") == this->objectName())
    {
        int x = OutlineLabel::getInt(object, SAVE_X, this->x());
        int y = OutlineLabel::getInt(object, SAVE_Y, this->y());
        int width = OutlineLabel::getInt(object, SAVE_WIDTH, this->width());
        int height = OutlineLabel::getInt(object, SAVE_HEIGHT, this->height());
        this->setGeometry(x, y, width, height);
        this->setHidden(OutlineLabel::getBool(object, SAVE_HIDDEN, this->isHidden()));

        if(object.contains(SAVE_LABEL) && object[SAVE_LABEL].isObject())
        {
            this->label.load(object[SAVE_LABEL].toObject());
        }
        emit imageFrameUpdated(this);
    }
}

QString CheckIfImageExists(const QString &item)
{
    QFile test(item);
    if (test.exists())
    {
        return item;
    }
    else if(item == "")
    {
        return QString("");
    }
    else if(item.isEmpty())
    {
        return item;
    }
    else
    {
        qWarning() << "Item image does not exist: " << item;
        return QString(":/images/main/image_not_found.png");
    }
    return QString();
}

void ImageFrame::setEditMode(const bool enabled)
{
    this->editModeEnabled = enabled;
    if(enabled)
    {
        geometryBeforeEdit = this->geometry();
        this->visableBeforeEdit = !(this->isHidden());
        sizeGrip.show();
    }
    else
    {
        sizeGrip.hide();
    }
    this->label.setInEditMode(enabled);
    // Trigger draw event to draw "edit mode boxes".
    this->update();
}

void ImageFrame::setVisable(const bool enabled)
{
    if(enabled)
    {
        this->show();
    }
    else
    {
        this->hide();
    }
}

void ImageFrame::init()
{
    this->defaultGeometry = this->geometry();
    this->defaultVisibility = this->isVisible();
    this->label.init();

    this->progressBar.setTextVisible(false);
    this->progressBar.setVisible(false);
    this->progressBar.setStyleSheet("QProgressBar{"
                                    "background-color : rgba(0, 0, 0, 0);"
                                    "border : 1px solid #331a0f;"
                                    "}"
                                    "QProgressBar::chunk {"
                                    "background-color: #FFEE9D;"
                                    "}");

    this->imageFrame.setGeometry(BORDER_SIZE * 2,
                                 BORDER_SIZE * 2,
                                 this->width() - BORDER_SIZE * 4,
                                 this->height() - BORDER_SIZE * 4);
}

void ImageFrame::resetToDefault()
{
    this->setGeometry(this->defaultGeometry);
    this->setVisable(this->defaultVisibility);
    this->label.resetToDefault();
}

void ImageFrame::resetToBeforeEdit()
{
    this->setGeometry(this->geometryBeforeEdit);
    this->setVisable(this->visableBeforeEdit);
    this->label.resetToBeforeEdit();
}

void ImageFrame::setImage(const QString &filePath)
{
    this->imageName = filePath;
    if(filePath.isEmpty())
    {
        this->imageFrame.setPixmap(QPixmap());
        return;
    }
    QPixmap tmp(CheckIfImageExists(filePath));
    if (!tmp.isNull())
    {
        this->imageFrame.setPixmap(tmp.scaled(imageFrame.width(),
                                              imageFrame.height(),
                                              Qt::IgnoreAspectRatio,
                                              Qt::SmoothTransformation));
    }

}

void ImageFrame::setBackgroundEnabled(const bool enabled)
{
    this->backgroundEnabled = enabled;

    if(enabled)
    {
        this->border.show();
    }
    else
    {
        this->border.hide();
        this->backgroundFrame.setPixmap(QPixmap());
    }
}


void ImageFrame::drawEditOutline()
{
    QPainter painter(this);
    QPen pen(Qt::yellow, 1, Qt::SolidLine);
    painter.setPen(pen);
    QRect square(0, 0, this->width() - 1, this->height() - 1);
    painter.drawRect(square);
}

void ImageFrame::setProgress(const QString &percentage)
{
    bool ok;
    int value = percentage.toInt(&ok);
    if(!this->progressBar.isVisible())
    {
        this->progressBar.setVisible(true);
    }
    if(ok)
    {
        if (value > 100)
        {
            value = 100;
        }
        if (value < 0)
        {
            value = 0;
        }
        this->progressBar.setValue(value);
        // Update should not have to be called. But without it, the bar does only
        // moves when there is sufficient enough differance between current
        // value and new value.
        this->progressBar.update();
    }
}


void ImageFrame::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if(this->editModeEnabled)
    {
        drawEditOutline();
    }
}


void ImageFrame::mousePressEvent(QMouseEvent *event)
{

    if (event->button() == Qt::LeftButton && this->editModeEnabled)
    {
        this->dragingOldWindowPos = event->globalPosition();
        this->holdingLeft = true;
    }
}

void ImageFrame::mouseReleaseEvent(QMouseEvent *event)
{

    if (event->button() == Qt::LeftButton )
    {
        this->holdingLeft = false;
    }
}

void ImageFrame::mouseMoveEvent(QMouseEvent *event)
{
    if (this->holdingLeft && this->editModeEnabled)
    {
        const QPointF delta = event->globalPosition() - this->dragingOldWindowPos;
        move(x()+delta.x(), y()+delta.y());
        this->dragingOldWindowPos = event->globalPosition();
    }
}

void ImageFrame::resizeEvent(QResizeEvent *event)
{
    sizeGrip.setGeometry(this->width()-sizeGrip.width(),
                         this->height()-sizeGrip.height(),
                         10,
                         10);

    QRect labelGeometry = this->label.geometry();
    labelGeometry.setWidth(this->geometry().width() - 4 * this->backgroundEnabled);
    labelGeometry.setHeight(this->geometry().height() - 2 * this->backgroundEnabled);
    this->label.setGeometry(labelGeometry);
    this->progressBar.setGeometry(0, 0, this->width(), this->height());

    // Resize the image to also fit the border
    this->imageFrame.setGeometry(BORDER_SIZE * 2,
                                 BORDER_SIZE * 2,
                                 this->width() - BORDER_SIZE * 4,
                                 this->height() - BORDER_SIZE * 4);

    setImage(this->imageName);

    if(this->backgroundEnabled)
    {
        QRect croppedRect(this->x() + BORDER_SIZE,
                          this->y(),
                          this->width(),
                          this->height());
        QPixmap cropped = foregroundImage.copy(croppedRect);
        backgroundFrame.setPixmap(cropped);
    }

    this->backgroundFrame.setGeometry(BORDER_SIZE,
                                      BORDER_SIZE,
                                      this->width()-BORDER_SIZE * 2,
                                      this->height()-BORDER_SIZE * 2);

    this->border.setGeometry(0,
                             0,
                             this->width(),
                             this->height());


    if (!inConstuctor)
    {
        emit imageFrameUpdated(this);
    }

    QWidget::resizeEvent(event);
}

void ImageFrame::moveEvent(QMoveEvent *event)
{
    if (!inConstuctor)
    {
        if(this->backgroundEnabled)
        {
            QRect croppedRect(this->x() + BORDER_SIZE,
                              this->y(),
                              this->width(),
                              this->height());
            QPixmap cropped = foregroundImage.copy(croppedRect);
            backgroundFrame.setPixmap(cropped);
        }
        emit imageFrameUpdated(this);
    }

    QWidget::moveEvent(event);
}

void ImageFrame::updateGeometry(const QRect &geometry)
{
    this->setGeometry(geometry);
}
