#include "outlinelabel.h"

constexpr char SAVE_FONT_SIZE[] = "font_size";
constexpr char SAVE_FONT_COLOR[] = "font_color";
constexpr char SAVE_FONT_ALIGNMENT[] = "font_alignment";

#include <QFont>
#include <QPalette>
#include <QPainter>

OutlineLabel::OutlineLabel(QWidget *parent)
    :QLabel(parent),
      outlineEnabled(false)
{
    outlinePen.setWidth(5);
    outlinePen.setColor(Qt::black);

    this->setFontSize("18");
    this->setBold(true);
    this->outlineFont.setBold(true);

    setFontColor("ffffff");
}

QJsonObject OutlineLabel::generateSave()
{
    QJsonObject jsonSave;
    jsonSave[SAVE_NAME] = this->parent()->objectName() + ".label";
    jsonSave[SAVE_FONT_SIZE] = this->font().pixelSize();
    jsonSave[SAVE_FONT_COLOR] = this->palette().color(QPalette::WindowText).name().removeFirst();
    jsonSave[SAVE_FONT_ALIGNMENT] = static_cast<int>(this->alignment());
    return jsonSave;
}

void OutlineLabel::load(const QJsonObject &object)
{
    if(getString(object, SAVE_NAME, "") == parent()->objectName() + ".label")
    {
        QString fontSize = QString::number(getInt(object, SAVE_FONT_SIZE, this->font().pixelSize()));
        setFontSize(fontSize);
        QPalette palette = this->palette();

        setFontColor(getString(object, SAVE_FONT_COLOR, palette.color(QPalette::WindowText).name()));
        this->setAlignment(static_cast<Qt::Alignment>(getInt(object, SAVE_FONT_ALIGNMENT, this->alignment())));
        emit labelUpdated(this);
    }
}

void OutlineLabel::enableOutline(const bool enabled)
{
    this->outlineEnabled = enabled;
    emit labelUpdated(this);
    // Trigger new paintEvent
    this->update();
}


void OutlineLabel::setBold(const bool bold)
{
    QFont font = this->font();
    font.setBold(bold);
    this->setFont(font);
    this->outlineFont.setBold(bold);
    if (bold)
    {
        this->outlinePen.setWidth(5);
    }
    else
    {
        this->outlinePen.setWidth(3);
    }
    emit labelUpdated(this);
    this->update();
}

void OutlineLabel::setFontColor(const QString &hexColor)
{
    if(hexColor.length() == 6 )
    {
        this->setStyleSheet("QLabel{color: #" + hexColor + "}");
        emit labelUpdated(this);
    }
}

void OutlineLabel::setFontSize(const QString &fontSize)
{
    bool ok = false;
    int value = fontSize.toInt(&ok);
    if (ok)
    {
        QFont font = this->font();
        font.setPixelSize(value);
        this->setFont(font);
        this->outlineFont.setPixelSize(this->font().pixelSize());
        emit labelUpdated(this);
    }
}

void OutlineLabel::setCurrentFontDefault()
{
    this->defaultFont = this->font();
}

void OutlineLabel::setTextAlignment(const int alignment)
{
    this->setAlignment(static_cast<Qt::Alignment>(alignment));
    emit labelUpdated(this);
}

void OutlineLabel::resetToBeforeEdit()
{
    this->setFont(this->fontBeforeEdit);
    this->setBold(this->font().bold());
    this->setPalette(this->paletteBeforeEdit);
    this->enableOutline(this->outlineBeforeEdit);
    this->setAlignment(static_cast<Qt::Alignment>(this->alignmentBeforeEdit));
}


bool OutlineLabel::getBool(const QJsonObject &object,
                           const QString &attribute,
                           const bool defaultValue)
{
    if(object.contains(attribute) && object[attribute].isBool())
    {
        return object[attribute].toBool(defaultValue);
    }
    return defaultValue;
}

QString OutlineLabel::getString(const QJsonObject &object,
                                const QString &attribute,
                                const QString &defaultValue)
{
    if(object.contains(attribute) && object[attribute].isString())
    {
        return object[attribute].toString(defaultValue);
    }
    return defaultValue;
}

int OutlineLabel::getInt(const QJsonObject &object,
                         const QString &attribute,
                         const int defaultValue)
{
    if(object.contains(attribute) && object[attribute].isDouble())
    {
        return object[attribute].toInt(defaultValue);
    }
    return defaultValue;
}

void OutlineLabel::resetToDefault()
{
    this->setFont(this->defaultFont);
    this->setBold(this->font().bold());
    this->setPalette(this->defaultPalette);
    this->enableOutline(this->defaultOutline);
    this->setAlignment(static_cast<Qt::Alignment>(this->defaultAlignment));
}

void OutlineLabel::setInEditMode(const bool enabled)
{
    if(enabled)
    {
        this->fontBeforeEdit = this->font();
        this->outlineBeforeEdit = this->outlineEnabled;
        this->paletteBeforeEdit = this->palette();
    }
}

void OutlineLabel::init()
{
    this->outlineFont.setFamily(this->font().family());
    this->outlineFont.setWeight(QFont::Thin);
    this->setFontSize("18");
    this->setBold(true);

    this->defaultFont = this->font();
    this->defaultOutline = this->outlineEnabled;
    this->defaultPalette = this->palette();
    this->defaultAlignment = this->alignment();

    this->fontBeforeEdit = this->font();
    this->outlineBeforeEdit = this->outlineEnabled;
    this->paletteBeforeEdit = this->palette();
    this->alignmentBeforeEdit = this->alignment();

    this->update();
}


QString OutlineLabel::getNamedColor()
{
    return this->QLabel::palette().color(QPalette::WindowText).name();
}

bool OutlineLabel::isOutlined()
{
    return this->outlineEnabled;
}


void OutlineLabel::drawOutline()
{
    QPainter painter(this);

    // Clear the last outline drawing
    this->outlinePath.clear();

    QFontMetrics fm(this->font());
    int textWide = fm.horizontalAdvance(this->text());
    int textHeight = fm.height();
    int textX = 0;
    int textY = 0;

    // These are just tested into OK values. Might be a better way to align this
    if (this->alignment() & Qt::AlignLeft)
        textX = 0;
    if (this->alignment() & Qt::AlignHCenter)
        textX = ( this->width() - textWide) / 2;
    if (this->alignment() & Qt::AlignRight)
        textX = this->width() - textWide ;

    if ((this->alignment() & Qt::AlignTop) == Qt::AlignTop)
        textY = textHeight - this->font().pixelSize() / 4;
    if ((this->alignment() & Qt::AlignVCenter) == Qt::AlignVCenter)
        textY =  ( this->height() + textHeight) / 2  - this->font().pixelSize() / 3 + 1;
    if ((this->alignment() & Qt::AlignBottom) == Qt::AlignBottom)
        textY = this->height() - this->font().pixelSize() / 4;

    painter.setFont(this->outlineFont);
    painter.setPen(this->outlinePen);


    this->outlinePath.addText(textX, textY, this->outlineFont, this->text());

    // Draw the outline
    painter.drawPath(this->outlinePath);
}

void OutlineLabel::paintEvent(QPaintEvent *event)
{
    if(this->outlineEnabled)
    {
        drawOutline();
    }

    // Draw the "normal" text
    QLabel::paintEvent(event);
}
