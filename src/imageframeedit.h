#ifndef IMAGEFRAMEEDIT_H
#define IMAGEFRAMEEDIT_H

#include <QWidget>

#include "imageframe.h"

namespace Ui {
class ImageFrameEdit;
}

class ImageFrameEdit : public QWidget
{
    Q_OBJECT

public:
    explicit ImageFrameEdit(ImageFrame * imageFrame,
                            const QString &name,
                            const bool includeLabel,
                            QWidget *parent = nullptr);
    ~ImageFrameEdit();

    /**
     * @brief getName Returns the Title (not object name) of the edit object.
     * @return The title of the group box in this object.
     */
    QString getName();

private:
    void readAlignment(const OutlineLabel *label);

    Ui::ImageFrameEdit *ui;
    bool isUpdatingDisplayedValues;

private slots:
    void valuesUpdated(const QString &value);

    void receivedUpdatedAlignment(const QString &unused);
    void receivedUpdatedImageFrame(ImageFrame *imageFrame);
    void receivedUpdatedLabel(OutlineLabel *label);

public slots:
    void setEditmode(const bool enabled);

signals:
    void setNewGeometry(const QRect &newGeometry);
    void setNewAlignment(const int alignment);
};

#endif // IMAGEFRAMEEDIT_H
