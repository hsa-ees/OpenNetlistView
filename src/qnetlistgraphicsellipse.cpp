#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QStyle>
#include <QGraphicsEllipseItem>
#include <QtCore/Qt>

#include "qnetlistgraphicsellipse.h"

namespace OpenNetlistView {

QNetlistGraphicsEllipse::QNetlistGraphicsEllipse(QGraphicsItem* parent)
    : QGraphicsEllipseItem(parent)
{

    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

QNetlistGraphicsEllipse::QNetlistGraphicsEllipse(const QRectF& rect, QGraphicsItem* parent)
    : QGraphicsEllipseItem(rect, parent)
{

    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

QNetlistGraphicsEllipse::~QNetlistGraphicsEllipse() = default;

void QNetlistGraphicsEllipse::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{

    auto modifiedOption = *option;
    modifiedOption.state &= ~QStyle::State_Selected;

    QGraphicsEllipseItem::paint(painter, &modifiedOption, widget);

    auto pen = this->pen();
    auto brush = this->brush();

    // when item is selected make it red if it is highlighted use the highlight color
    // else set to the default color
    if((option->state & QStyle::State_Selected) != 0)
    {
        pen.setColor(Qt::red);
        brush.setColor(Qt::red);
    }
    else if(this->highlightColor != Qt::transparent)
    {
        pen.setColor(this->highlightColor);
        brush.setColor(this->highlightColor);
    }
    else
    {
        pen.setColor(Qt::black);
        brush.setColor(Qt::black);
    }

    this->setPen(pen);
    this->setBrush(brush);
}

void QNetlistGraphicsEllipse::setHighlightColor(const QColor& color)
{
    this->highlightColor = color;
    this->update();
}

void QNetlistGraphicsEllipse::clearHighlightColor()
{
    this->highlightColor = Qt::transparent;
    this->update();
}

} // namespace OpenNetlistView