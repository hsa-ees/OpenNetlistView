#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QFont>
#include <QPen>
#include <QPainter>
#include <QRectF>
#include <QtCore/Qt>
#include <QWidget>
#include <QGraphicsScene>
#include <QStyleOption>
#include <QEvent>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QString>
#include <QColor>

#include <third_party/libavoid/connector.h>

#include <memory>
#include <vector>
#include <utility>

#include <yosys/path.h>
#include <yosys/port.h>
#include <yosys/node.h>

#include "qnetlistgraphicsellipse.h"
#include "qnetlistgraphicstext.h"

#include "qnetlistgraphicspath.h"

namespace OpenNetlistView {

QNetlistGraphicsPath::QNetlistGraphicsPath(QGraphicsItem* parent)
    : QGraphicsPathItem(parent)
    , yosysPath(nullptr)
{
    // set the painter path in the parent

    this->setPath(QPainterPath());
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

QNetlistGraphicsPath::QNetlistGraphicsPath(const QPainterPath& painter, QGraphicsItem* parent)
    : QGraphicsPathItem(parent)
    , yosysPath(nullptr)
{
    // set the painter path in the parent

    this->setPath(painter);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

QNetlistGraphicsPath::QNetlistGraphicsPath(const std::shared_ptr<Yosys::Path>& path,
    const QPainterPath& painter,
    QGraphicsItem* parent)
    : QGraphicsPathItem(parent)
    , yosysPath(path)
{
    this->setPath(painter);
    if(this->yosysPath != nullptr)
    {
        this->placePathText();
    }
    this->placeDivergingPoints();
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

QNetlistGraphicsPath::~QNetlistGraphicsPath() = default;

std::shared_ptr<Yosys::Path> QNetlistGraphicsPath::getYosysPath() const
{
    return yosysPath;
}

void QNetlistGraphicsPath::setYosysPath(const std::shared_ptr<Yosys::Path>& path)
{
    this->yosysPath = path;

    if(this->yosysPath != nullptr)
    {
        this->placePathText();
    }
    this->placeDivergingPoints();
}

QPainterPath QNetlistGraphicsPath::shape() const
{
    QPainterPathStroker stroker;
    stroker.setWidth(pen().widthF() + lineSelectionWidth);
    return stroker.createStroke(this->path());
}

void QNetlistGraphicsPath::setSrcTextPort(const QPointF& pos)
{

    // only set it the first time
    if(this->srcTextPos != QPointF())
    {
        return;
    }

    this->srcTextPos = pos;
}

void QNetlistGraphicsPath::addDstTextPort(const QPointF& pos, Avoid::ConnRef* avoidRef)
{
    this->dstTextPosList.emplace_back(pos, avoidRef);
}

void QNetlistGraphicsPath::addDivergingPoint(const QPointF& pos)
{
    this->divergingPoints.push_back(pos);
}

void QNetlistGraphicsPath::setHighlightColor(const QColor& color)
{
    this->highlightColor = color;

    for(auto& divergingPoint : this->divergingPointsSymbols)
    {
        divergingPoint->setHighlightColor(color);
    }

    for(auto& textItem : this->pathTextItems)
    {
        textItem->setHighlightColor(color);
    }

    this->update();
}

void QNetlistGraphicsPath::clearHighlightColor()
{
    this->highlightColor = Qt::transparent;

    for(auto& divergingPoint : this->divergingPointsSymbols)
    {
        divergingPoint->clearHighlightColor();
    }

    for(auto& textItem : this->pathTextItems)
    {
        textItem->clearHighlightColor();
    }

    this->update();
}

QGraphicsItem* QNetlistGraphicsPath::getSrcQtItem() const
{

    auto portParent = this->yosysPath->getSigSource()->getParentNode();

    QGraphicsItem* srcItem = nullptr;

    if(portParent == nullptr)
    {
        srcItem = this->yosysPath->getSigSource()->getGraphicsItem();
    }
    else
    {
        srcItem = portParent->getGraphicsItem();
    }

    return srcItem;
}

std::vector<QGraphicsItem*> QNetlistGraphicsPath::getDstQtItems() const
{
    std::vector<QGraphicsItem*> dstItems;

    for(auto& dst : *(this->yosysPath->getSigDestinations()))
    {
        dstItems.push_back(dst->getParentNode()->getGraphicsItem());
    }

    return dstItems;
}

std::vector<std::pair<QString, QString>> QNetlistGraphicsPath::getProperties()
{
    std::vector<std::pair<QString, QString>> properties;

    // add the type of the path
    properties.emplace_back(QObject::tr(propertyTypeType), propertyValueType);

    // add the name of the path
    properties.emplace_back(QObject::tr(propertyTypeName), this->yosysPath->getName());

    // get the source node
    auto source = this->yosysPath->getSigSource();

    // if the parent node is null it is a port then use its name otherwise the
    // name of the parent
    QString sourceName;
    if(source->getParentNode() == nullptr)
    {
        sourceName = source->getName();
    }
    else
    {
        sourceName = source->getParentNode()->getName();
    }

    properties.emplace_back(QObject::tr(propertyTypeSrcName), sourceName);

    // get all the destinations
    auto destinations = this->yosysPath->getSigDestinations();

    // add the number of destinations
    properties.emplace_back(QObject::tr(propertyTypeNeighbors), QString::number(destinations->size()));

    for(auto& destination : *destinations)
    {
        // if the parent node is null it is a port then use its name otherwise the
        // name of the parent
        QString destinationName;
        if(destination->getParentNode() == nullptr)
        {
            destinationName = destination->getName();
        }
        else
        {
            destinationName = destination->getParentNode()->getName();
        }

        properties.emplace_back(QObject::tr(propertyTypeDstName), destinationName);
    }

    // convert the bits to a list of stings
    QStringList bits;
    for(auto& bit : this->yosysPath->getBits())
    {
        bits.append(QString::number(bit.toInt()));
    }

    // make a normal string
    const QString bitsString = bits.join(", ");

    properties.emplace_back(QObject::tr("Bits"), (bitsString));

    return properties;
}

void QNetlistGraphicsPath::paint(QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget* widget)
{

    // remove the options that should be customized to
    // avoid the default functionality of the base class

    auto modifiedOption = *option;
    modifiedOption.state &= ~QStyle::State_Selected;

    // call the base class paint method to draw the rest
    QGraphicsPathItem::paint(painter, &modifiedOption, widget);

    // handel selection by changing the pen color
    auto pen = this->pen();
    if((option->state & QStyle::State_Selected) != 0)
    {
        pen.setColor(Qt::red);
    }
    else if(this->highlightColor != Qt::transparent)
    {
        pen.setColor(this->highlightColor);
    }
    else
    {
        pen.setColor(Qt::black);
    }
    this->setPen(pen);
}

void QNetlistGraphicsPath::placePathText()
{

    // get the name of the path with the width of the signal
    const QString pathName = this->yosysPath->generateLabelText();

    // create src text item
    if(pathName != "")
    {
        this->createTextItem(pathName, this->srcTextPos);
    }

    // create dst text items
    for(auto& [pos, avoidRef] : this->dstTextPosList)
    {
        const QString pathName = this->yosysPath->generateLabelText(avoidRef);
        if(pathName != "")
        {
            this->createTextItem(pathName, pos, true);
        }
    }
}

void QNetlistGraphicsPath::createTextItem(const QString& text, const QPointF& pos, bool isDst)
{
    // create the text item
    auto* textItem = new QNetlistGraphicsText(this);
    textItem->setPlainText(text);
    textItem->setFont(QFont("Arial", fontSize));
    textItem->setDefaultTextColor(Qt::black);
    textItem->setScale(textScale);

    // when the text is added to the destination
    // the text needs to be moved to the left otherwise it
    // would be placed in th node adjacent to the path
    auto finalPos = pos;
    if(isDst)
    {
        finalPos = pos - QPointF(textItem->boundingRect().width() / 2, 0);
    }
    textItem->setPos(finalPos);

    this->pathTextItems.emplace_back(textItem);
}

void QNetlistGraphicsPath::placeDivergingPoints()
{

    for(const auto& pos : this->divergingPoints)
    {
        auto* divergingPoint = new QNetlistGraphicsEllipse(this);
        QRectF ellipseRect;

        // create he diverging point symbol the size depends on the signal width
        if(this->yosysPath->isBus())
        {
            ellipseRect = QRectF(pos.x() - (divergingPointBusRadius / 2),
                pos.y() - (divergingPointBusRadius / 2),
                divergingPointBusRadius,
                divergingPointBusRadius);
        }
        else
        {
            ellipseRect = QRectF(pos.x() - (divergingPointSignalRadius / 2),
                pos.y() - (divergingPointSignalRadius / 2),
                divergingPointSignalRadius,
                divergingPointSignalRadius);
        }

        divergingPoint->setRect(ellipseRect);
        divergingPoint->setPen(QPen(Qt::black));
        divergingPoint->setBrush(QBrush(Qt::black));

        this->divergingPointsSymbols.emplace_back(divergingPoint);
    }
}

} // namespace OpenNetlistView
