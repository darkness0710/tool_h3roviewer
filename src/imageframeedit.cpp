#include "imageframeedit.h"
#include "ui_imageframeedit.h"

#include "outlinelabel.h"

#include <QValidator>
#include <QComboBox>

ImageFrameEdit::ImageFrameEdit(ImageFrame * imageFrame,
                               const QString &name,
                               const bool includeLabel,
                               QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageFrameEdit),
    isUpdatingDisplayedValues(false)
{
    ui->setupUi(this);

    this->ui->visableBox->setChecked(!imageFrame->isHidden());

    readAlignment(&imageFrame->label);

    connect(imageFrame, &ImageFrame::imageFrameUpdated, this, &ImageFrameEdit::receivedUpdatedImageFrame);
    connect(&(imageFrame->label), &OutlineLabel::labelUpdated, this, &ImageFrameEdit::receivedUpdatedLabel);

    connect(this->ui->verticalComboBox,   &QComboBox::currentTextChanged, this, &ImageFrameEdit::receivedUpdatedAlignment);
    connect(this->ui->horizontalComboBox, &QComboBox::currentTextChanged, this, &ImageFrameEdit::receivedUpdatedAlignment);

    connect(this, &ImageFrameEdit::setNewGeometry, imageFrame, &ImageFrame::updateGeometry);
    connect(this, &ImageFrameEdit::setNewAlignment, &(imageFrame->label), &OutlineLabel::setTextAlignment);

    connect(this->ui->visableBox, &QCheckBox::clicked, imageFrame, &ImageFrame::setVisable);
    connect(this->ui->boldBox,    &QCheckBox::clicked, &(imageFrame->label), &OutlineLabel::setBold);
    connect(this->ui->outlineBox, &QCheckBox::clicked, &(imageFrame->label), &OutlineLabel::enableOutline);

    connect(this->ui->xEdit,      &QLineEdit::textChanged, this, &ImageFrameEdit::valuesUpdated);
    connect(this->ui->yEdit,      &QLineEdit::textChanged, this, &ImageFrameEdit::valuesUpdated);
    connect(this->ui->widthEdit,  &QLineEdit::textChanged, this, &ImageFrameEdit::valuesUpdated);
    connect(this->ui->heightEdit, &QLineEdit::textChanged, this, &ImageFrameEdit::valuesUpdated);

    connect(this->ui->rgbEdit,      &QLineEdit::textChanged, &(imageFrame->label), &OutlineLabel::setFontColor);
    connect(this->ui->fontSizeEdit, &QLineEdit::textChanged, &(imageFrame->label), &OutlineLabel::setFontSize);

    this->ui->xEdit->setValidator(new QIntValidator(0, 10000, this));
    this->ui->yEdit->setValidator(new QIntValidator(0, 10000, this));
    this->ui->widthEdit->setValidator(new QIntValidator(0, 10000, this));
    this->ui->heightEdit->setValidator(new QIntValidator(0, 10000, this));

    if(!includeLabel)
    {
        int height = this->ui->labelFrame->geometry().height();
        this->ui->labelFrame->hide();
        QRect geometry = this->geometry();
        geometry.setHeight(geometry.height() - height);
        this->setGeometry(geometry);
    }
/*
    this->ui->rgbEdit->setText(imageFrame->label.palette().color(QPalette::WindowText).name());
    this->ui->fontSizeEdit->setText(QString::number(imageFrame->label.font().pointSize()));

    this->ui->boldBox->setChecked(imageFrame->label.font().bold());
    this->ui->outlineBox->setChecked(imageFrame->label.isOutlined());
*/

    setEditmode(false);
    this->ui->groupBox->setTitle(name);
}

ImageFrameEdit::~ImageFrameEdit()
{
    delete ui;
}


void ImageFrameEdit::valuesUpdated(const QString &value)
{
    Q_UNUSED(value)
    if(!isUpdatingDisplayedValues)
    {
        QRect newGeometry;
        newGeometry.setX(this->ui->xEdit->text().toInt());
        newGeometry.setY(this->ui->yEdit->text().toInt());
        newGeometry.setWidth(this->ui->widthEdit->text().toInt());
        newGeometry.setHeight(this->ui->heightEdit->text().toInt());

        emit setNewGeometry(newGeometry);
    }

}

void ImageFrameEdit::receivedUpdatedImageFrame(ImageFrame *imageFrame)
{
    isUpdatingDisplayedValues = true;
    this->ui->xEdit->setText(QString::number(imageFrame->x()));
    this->ui->yEdit->setText(QString::number(imageFrame->y()));
    this->ui->widthEdit->setText(QString::number(imageFrame->width()));
    this->ui->heightEdit->setText(QString::number(imageFrame->height()));
    isUpdatingDisplayedValues = false;
}

void ImageFrameEdit::receivedUpdatedLabel(OutlineLabel *label)
{
    this->ui->rgbEdit->setText(label->palette().color(QPalette::WindowText).name());
    this->ui->fontSizeEdit->setText(QString::number(label->font().pixelSize()));
    this->ui->boldBox->setChecked(label->font().bold());
    this->ui->outlineBox->setChecked(label->isOutlined());
}

void ImageFrameEdit::setEditmode(const bool enabled)
{
    this->ui->groupBox->setEnabled(enabled);
}


void ImageFrameEdit::readAlignment(const OutlineLabel *label)
{
    QString hSelected;
    QString vSelected;
    if(label->alignment() & Qt::AlignTop)
    {
        vSelected = "VTop";
    }
    else if(label->alignment() & Qt::AlignVCenter)
    {
        vSelected = "VCenter";
    }
    else if(label->alignment() & Qt::AlignBottom)
    {
        vSelected = "VBottom";
    }
    if(label->alignment() & Qt::AlignLeft)
    {
        hSelected = "HLeft";
    }
    else if(label->alignment() & Qt::AlignHCenter)
    {
        hSelected = "HCenter";
    }
    else if(label->alignment() & Qt::AlignRight)
    {
        hSelected = "HRight";
    }
    for(int i = 0; i< ui->verticalComboBox->count(); i++)
    {
        if(ui->verticalComboBox->itemText(i) == vSelected)
        {
            ui->verticalComboBox->setCurrentIndex(i);
        }
    }

    for(int i = 0; i< ui->horizontalComboBox->count(); i++)
    {
        if(ui->horizontalComboBox->itemText(i) == hSelected)
        {
            ui->horizontalComboBox->setCurrentIndex(i);
        }
    }
}


void ImageFrameEdit::receivedUpdatedAlignment(const QString &unused)
{
    Q_UNUSED(unused)
    int alignment = 0;
    if (this->ui->verticalComboBox->currentText() == "VTop")
    {
        alignment = Qt::AlignTop;
    }
    else if(this->ui->verticalComboBox->currentText() == "VCenter")
    {
        alignment = Qt::AlignVCenter;
    }
    else if(this->ui->verticalComboBox->currentText() == "VBottom")
    {
        alignment = Qt::AlignBottom;
    }

    if (this->ui->horizontalComboBox->currentText() == "HLeft")
    {
        alignment = alignment | Qt::AlignLeft;
    }
    else if(this->ui->horizontalComboBox->currentText() == "HCenter")
    {
        alignment = alignment | Qt::AlignHCenter;
    }
    else if(this->ui->horizontalComboBox->currentText() == "HRight")
    {
        alignment = alignment | Qt::AlignRight;
    }
    emit setNewAlignment(alignment);
}

QString ImageFrameEdit::getName()
{
    return this->ui->groupBox->title();
}

