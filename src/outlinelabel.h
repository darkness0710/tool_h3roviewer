#ifndef OUTLINELABEL_H
#define OUTLINELABEL_H

#include <QLabel>
#include <QPaintEvent>
#include <QPen>
#include <QPainterPath>
#include <QFont>
#include <QPainter>
#include <QTimer>
#include <QPalette>

#include <QJsonObject>

constexpr char SAVE_NAME[] = "name";
constexpr char SAVE_HIDDEN[] = "hidden";

class OutlineLabel : public QLabel
{
    Q_OBJECT
public:
    OutlineLabel(QWidget *parent);

    QJsonObject generateSave();
    void load(const QJsonObject &object);

    QString getNamedColor();

    bool isOutlined();

    void setInEditMode(const bool enabled);

    void resetToDefault();
    void resetToBeforeEdit();

    static bool getBool(const QJsonObject &object,
                        const QString &attribute,
                        const bool defaultValue);

    static QString getString(const QJsonObject &object,
                             const QString &attribute,
                             const QString &defaultValue);

    static int getInt(const QJsonObject &object,
                      const QString &attribute,
                      const int defaultValue);
    void setCurrentFontDefault();
private:
    void drawOutline();

    QPainterPath outlinePath;
    QPen outlinePen;
    QFont outlineFont;

    bool outlineEnabled;

    QFont defaultFont;
    QFont fontBeforeEdit;

    bool defaultOutline;
    bool outlineBeforeEdit;

    QPalette defaultPalette;
    QPalette paletteBeforeEdit;

    int defaultAlignment;
    int alignmentBeforeEdit;

public slots:

    void enableOutline(const bool enabled);
    void setBold(const bool bold);
    void setFontColor(const QString &hexColor);
    void setFontSize(const QString &fontSize);

    void setTextAlignment(const int alignment);

    void init();
protected:
    void paintEvent(QPaintEvent* event);

signals:
    void labelUpdated(OutlineLabel * label);
};

#endif // OUTLINELABEL_H

