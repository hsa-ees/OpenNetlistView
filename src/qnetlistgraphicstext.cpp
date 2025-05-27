#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QStyle>
#include <QtCore/Qt>
#include <QGraphicsTextItem>
#include <QString>

#include "qnetlistgraphicstext.h"

namespace OpenNetlistView {

QNetlistGraphicsText::QNetlistGraphicsText(QGraphicsItem* parent)
    : QGraphicsTextItem(parent)
{

    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

QNetlistGraphicsText::QNetlistGraphicsText(const QString& text, QGraphicsItem* parent)
    : QGraphicsTextItem(text, parent)
{

    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

QNetlistGraphicsText::~QNetlistGraphicsText() = default;

void QNetlistGraphicsText::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{

    auto modifiedOption = *option;
    modifiedOption.state &= ~QStyle::State_Selected;

    QGraphicsTextItem::paint(painter, &modifiedOption, widget);

    // set the color or the highlight color else the default color
    if((option->state & QStyle::State_Selected) != 0)
    {
        this->setDefaultTextColor(Qt::red);
    }
    else if(this->highlightColor != Qt::transparent)
    {
        this->setDefaultTextColor(this->highlightColor);
    }
    else
    {
        this->setDefaultTextColor(Qt::black);
    }
}

void QNetlistGraphicsText::setHighlightColor(const QColor& color)
{
    this->highlightColor = color;
    this->update();
}

void QNetlistGraphicsText::clearHighlightColor()
{
    this->highlightColor = Qt::transparent;
    this->update();
}

} // namespace OpenNetlistView