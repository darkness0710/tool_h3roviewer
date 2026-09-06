#ifndef IMAGEFRAMEBORDER_H
#define IMAGEFRAMEBORDER_H

#include <QLabel>

class ImageFrameBorder : public QLabel
{
public:
    ImageFrameBorder(const int borderSize, QWidget * parent);
    ~ImageFrameBorder();


protected:
    void paintEvent(QPaintEvent *event);

private:
    /**
     * @brief drawBorder Draws a gradient bezel border around the widget.
     */
    void drawBorder();

    int borderSize;

};

#endif // IMAGEFRAMEBORDER_H
