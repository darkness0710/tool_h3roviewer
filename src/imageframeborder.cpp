
#include <QPainter>
#include <QPen>

#include "imageframeborder.h"


ImageFrameBorder::ImageFrameBorder(const int borderSize, QWidget *parent):
    QLabel(parent),
    borderSize(borderSize)
{

}

ImageFrameBorder::~ImageFrameBorder()
{

}

void ImageFrameBorder::drawBorder()
{
    QPainter painter(this);
    int blackAlpha = 70;
    int whiteAlpha = 50;
    QPen pen(Qt::black, 1, Qt::SolidLine);
    for(int i = 0;i < this->borderSize;i++)
    {
        // Draw all the dark parts of the border
        pen.setColor(QColor(0, 0, 0, blackAlpha));
        painter.setPen(pen);

        // Top line
        painter.drawLine(this->borderSize,
                         i,
                         this->width()-this->borderSize-1,
                         i);

        // Left line
        painter.drawLine(i,
                         this->borderSize,
                         i,
                         this->height()-this->borderSize-1);

        // Bottom line
        painter.drawLine(this->borderSize * 2,
                         this->height() - this->borderSize * 2 + i,
                         this->width() - (this->borderSize * 2),
                         this->height() - this->borderSize * 2 + i);

        // Right line
        painter.drawLine(this->width() - (this->borderSize * 2 - i),
                         this->borderSize * 2,
                         this->width() - (this->borderSize * 2 - i),
                         this->height() - (this->borderSize * 2 - i));

        blackAlpha +=  50;

        // Draw all the Light parts of the border
        pen.setColor(QColor(255,255,255,whiteAlpha));
        painter.setPen(pen);

        // Right line
        painter.drawLine(this->width() - (i + 1),
                         this->borderSize,
                         this->width() - (i + 1),
                         this->height() - this->borderSize);

        // Bottom line
        painter.drawLine(this->borderSize,
                         this->height() - (i + 1),
                         this->width() - this->borderSize,
                         this->height() - (i + 1));

        // Top line
        painter.drawLine(this->borderSize * 2 - 1 - i,
                         this->borderSize * 2 - 1 - i ,
                         this->width() - this->borderSize - 1,
                         this->borderSize * 2 - 1 - i);

        // Left line
        painter.drawLine(this->borderSize * 2 - 1 - i,
                         this->borderSize * 2 - i,
                         this->borderSize * 2 - 1 - i,
                         this->height() - this->borderSize - 1);
        whiteAlpha += 20;
    }
}

void ImageFrameBorder::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    drawBorder();
}
