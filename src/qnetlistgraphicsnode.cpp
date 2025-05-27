#include <QGraphicsSvgItem>
#include <QGraphicsItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QSvgGenerator>
#include <QIODevice>
#include <QTextStream>
#include <QRectF>
#include <QPointF>
#include <QDomDocument>
#include <QGraphicsScene>
#include <QPen>
#include <QFont>
#include <QGraphicsTextItem>
#include <QtCore/Qt>
#include <QColor>
#include <QObject>
#include <QRegularExpression>
#include <QStringList>

#include <memory>
#include <vector>
#include <utility>
#include <cstddef>
#include <algorithm>

#include <yosys/component.h>
#include <yosys/port.h>
#include <yosys/node.h>
#include <yosys/path.h>
#include <symbol/port.h>

#include "qnetlistgraphicstext.h"

#include "qnetlistgraphicsnode.h"

namespace OpenNetlistView {

QNetlistGraphicsNode::QNetlistGraphicsNode(QGraphicsItem* parent)
    : QGraphicsSvgItem(parent)
    , component(nullptr)
{

    setCacheMode(QGraphicsItem::NoCache);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

QNetlistGraphicsNode::QNetlistGraphicsNode(const std::shared_ptr<Yosys::Component>& component, QGraphicsItem* parent)
    : QGraphicsSvgItem(parent)
    , component(component)
{
    setCacheMode(QGraphicsItem::NoCache);
    setFlag(QGraphicsItem::ItemIsSelectable, true);

    if(this->component != nullptr)
    {
        this->setTextPath();
    }
}

QNetlistGraphicsNode::~QNetlistGraphicsNode() = default;

std::shared_ptr<Yosys::Component> QNetlistGraphicsNode::getComponent() const
{
    return component;
}

void QNetlistGraphicsNode::setComponent(const std::shared_ptr<Yosys::Component>& component)
{
    this->component = component;

    if(this->component != nullptr)
    {
        this->setTextPath();
    }
}

void QNetlistGraphicsNode::setHighlightColor(const QColor& color)
{
    this->highlightColor = color;

    for(auto& textItem : this->nodeTextItems)
    {
        textItem->setHighlightColor(color);
    }

    this->update();
}

void QNetlistGraphicsNode::clearHighlightColor()
{
    this->highlightColor = Qt::transparent;

    for(auto& textItem : this->nodeTextItems)
    {
        textItem->clearHighlightColor();
    }

    this->update();
}

std::vector<QGraphicsItem*> QNetlistGraphicsNode::getConnectedItems()
{
    // get the port or node
    std::vector<QGraphicsItem*> connectedItems;

    // get the connected items for the node or port
    if(std::dynamic_pointer_cast<Yosys::Port>(component) != nullptr)
    {
        auto port = std::dynamic_pointer_cast<Yosys::Port>(component);

        connectedItems.push_back(port->getPath()->getGraphicsItem());
    }
    else if(std::dynamic_pointer_cast<Yosys::Node>(component) != nullptr)
    {
        auto node = std::dynamic_pointer_cast<Yosys::Node>(component);

        for(const auto& port : node->getPorts())
        {
            connectedItems.push_back(port->getPath()->getGraphicsItem());
        }
    }

    return connectedItems;
}

std::vector<std::pair<QString, QString>> QNetlistGraphicsNode::getProperties()
{

    std::vector<std::pair<QString, QString>> properties;

    if(std::dynamic_pointer_cast<Yosys::Port>(this->component) != nullptr)
    {
        properties = this->getPortProperties();
    }
    else if(std::dynamic_pointer_cast<Yosys::Node>(this->component) != nullptr)
    {
        properties = this->getNodeProperties();
    }

    return properties;
}

void QNetlistGraphicsNode::paint(QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget* widget)
{

    // remove the options that should be customized to
    // avoid the default functionality of the base class
    auto modifiedOption = *option;
    modifiedOption.state &= ~QStyle::State_Selected;

    // call the base class paint method to draw the rest
    QGraphicsSvgItem::paint(painter, &modifiedOption, widget);

    // draws the selection rectangle above the svg item
    if((option->state & QStyle::State_Selected) != 0)
    {
        const QPen selectionPen(Qt::red, 1, Qt::DashLine);
        painter->setPen(selectionPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect());
    }
    else if(highlightColor != Qt::transparent)
    {
        const QPen selectionPen(highlightColor, 1, Qt::DashLine);
        painter->setPen(selectionPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect());
    }
}

void QNetlistGraphicsNode::setTextPath()
{
    // check if the component is a port

    auto portInst = std::dynamic_pointer_cast<Yosys::Port>(component);

    if(portInst != nullptr)
    {

        // check if the port is constant or not
        if(portInst->getDirection() == Yosys::Port::EDirection::CONST)
        {

            const QString displayText = QNetlistGraphicsNode::getFormatConstPortValue(portInst);
            this->createPortTextItem(displayText);
        }
        else
        {
            this->createPortTextItem(portInst->getName(), true);
        }
    }

    // add text if it is a generic module

    // check if the component is a node
    auto nodeInst = std::dynamic_pointer_cast<Yosys::Node>(component);

    // only add the type of the module if it is a generic module
    if(nodeInst == nullptr ||
        (nodeInst->getSymbol() != nullptr && !nodeInst->getSymbol()->isGenericSymbol()))
    {
        return;
    }

    // only adds a type if it is really a generic module and not a
    // symbol with a generated name like $procmux$1

    QString symbolLabelText = nodeInst->getType();
    const QRegularExpression regularExpr("[/\\\\.$]");

    if(!nodeInst->getName().contains(regularExpr))
    {
        symbolLabelText += ":" + nodeInst->getName();
    }

    this->createPortTextItem(symbolLabelText, true);

    // get the ports of the node and the ports of the symbol
    const auto ports = nodeInst->getPorts();
    const auto symbolPorts = nodeInst->getSymbol()->getPorts();

    for(const auto& port : ports)
    {
        // get the matching port from the node and the symbol
        auto symbolPortIt = std::find_if(symbolPorts.begin(), symbolPorts.end(), [&port](const std::shared_ptr<Symbol::Port>& symbolPort) {
            return symbolPort->getName() == port->getName() || symbolPort->getName() == port->getSymbolNameAlias();
        });

        // if the port is not found in the symbol ports
        // it is a port that is not connected to the symbol
        if(symbolPortIt == symbolPorts.end())
        {
            continue;
        }

        createGenericPortTextItem(port, *symbolPortIt);
    }
}

void QNetlistGraphicsNode::createGenericPortTextItem(const std::shared_ptr<Yosys::Port>& port,
    const std::shared_ptr<Symbol::Port>& symbolPort)
{

    // get the name of the port
    const QString portName = port->getName();

    // get the coordinates of the port
    const auto symbolPortX = symbolPort->getXPos();
    const auto symbolPortY = symbolPort->getYPos();

    // translate it form the top left corner of the node
    const auto textPosX = symbolPortX - this->boundingRect().topLeft().x();
    const auto textPosY = symbolPortY - this->boundingRect().topLeft().y();

    QFont font("Arial", fontSize);
    font.setBold(true);

    QPointF textPos(textPosX, textPosY);

    auto* textItem = new QNetlistGraphicsText(this);
    textItem->setPlainText(portName);
    textItem->setFont(font);
    textItem->setScale(fontScale);
    textItem->setDefaultTextColor(Qt::black);

    // pull the to the left to render the text outside of the node
    if(port->getDirection() == Yosys::Port::EDirection::INPUT)
    {
        textPos = textPos - QPointF(textItem->boundingRect().width() / 2, 0);
    }

    textItem->setPos(textPos);
    this->nodeTextItems.push_back(textItem);
}

void QNetlistGraphicsNode::createPortTextItem(const QString& text, bool topOrBottom)
{

    const QFont font("Arial", 10);
    QPointF textPos;

    // create the text item
    auto* textItem = new QNetlistGraphicsText(this);
    textItem->setPlainText(text);
    textItem->setFont(font);
    textItem->setDefaultTextColor(Qt::black);

    // set the position of the text item
    // according to if the item is on the top or bottom
    textPos.setX(this->boundingRect().center().x() - (textItem->boundingRect().width() / 2));
    if(topOrBottom)
    {
        textPos.setY(this->boundingRect().top() - textItem->boundingRect().height());
    }
    else
    {
        textPos.setY(this->boundingRect().bottom());
    }

    textItem->setPos(textPos);
    this->nodeTextItems.push_back(textItem);
}

std::vector<std::pair<QString, QString>> QNetlistGraphicsNode::getNodeProperties()
{

    // get the node instance from the component
    auto nodeInst = std::dynamic_pointer_cast<Yosys::Node>(component);

    if(nodeInst == nullptr)
    {
        return {};
    }

    std::vector<std::pair<QString, QString>> properties;

    // add the type of the node
    properties.emplace_back(QObject::tr(propertyTypeType), nodeInst->getType());

    // add the name of the node
    properties.emplace_back(QObject::tr(propertyTypeName), nodeInst->getName());

    // get the number of inputs and outputs
    long inputs = 0;
    long outputs = 0;

    const bool isGeneric = nodeInst->getSymbol()->isGenericSymbol();

    // add the names of the input paths
    for(const auto& port : nodeInst->getPorts())
    {
        QString name;
        if(isGeneric)
        {
            name = port->getName();
        }
        else
        {
            if(port->getPath() == nullptr)
            {
                continue;
            }

            name = port->getPath()->getName();
        }

        if(port->getDirection() == Yosys::Port::EDirection::INPUT && !port->hasNoConnectBitsConnection())
        {

            properties.emplace_back(QObject::tr(propertyTypeNodeInputName), name);
            inputs++;
        }
    }
    properties.insert(properties.end() - inputs,
        std::make_pair<QString, QString>(QObject::tr(propertyTypeNodeInputAmount), QString::number(inputs)));

    // add the names of the output paths
    for(const auto& port : nodeInst->getPorts())
    {

        QString name;
        if(isGeneric)
        {
            name = port->getName();
        }
        else
        {
            if(port->getPath() == nullptr)
            {
                continue;
            }

            name = port->getPath()->getName();
        }

        if(port->getDirection() == Yosys::Port::EDirection::OUTPUT && !port->hasNoConnectBitsConnection())
        {

            properties.emplace_back(QObject::tr(propertyTypeNodeOutputName), name);
            outputs++;
        }
    }
    properties.insert(properties.end() - outputs,
        std::make_pair<QString, QString>(QObject::tr(propertyTypeNodeOutputAmount), QString::number(outputs)));

    // print out all the bits to the ports of the node#

    for(const auto& port : nodeInst->getPorts())
    {
        if(port->hasNoConnectBitsConnection())
        {
            continue;
        }

        QStringList bits;
        for(auto& bit : port->getBits())
        {
            bits.append(QString::number(bit.toInt()));
        }

        // make a normal string
        const QString bitsString = bits.join(", ");

        properties.emplace_back(QObject::tr("Bits"), port->getName() + ": " + (bitsString));
    }

    return properties;
}

std::vector<std::pair<QString, QString>> QNetlistGraphicsNode::getPortProperties()
{

    // get the port instance from the component
    auto portInst = std::dynamic_pointer_cast<Yosys::Port>(component);

    if(portInst == nullptr)
    {
        return {};
    }

    std::vector<std::pair<QString, QString>> properties;

    // add the type of the port
    properties.emplace_back(QObject::tr(propertyTypeType), propertyValuePortType);

    // add the name of the port
    properties.emplace_back(QObject::tr(propertyTypeName), portInst->getName());

    // add the direction of the port

    QString direction;

    switch(portInst->getDirection())
    {
        case Yosys::Port::EDirection::INPUT:
            direction = QObject::tr(propertyValuePortInput);
            break;
        case Yosys::Port::EDirection::OUTPUT:
            direction = QObject::tr(propertyValuePortOutput);
            break;
        case Yosys::Port::EDirection::CONST:
            direction = QObject::tr(propertyValuePortConst);
            break;
        default:
            direction = QObject::tr(propertyValuePortUnknown);
            break;
    }

    properties.emplace_back(QObject::tr(propertyTypeDirection), direction);

    // if it is a constant add the value
    if(portInst->getDirection() == Yosys::Port::EDirection::CONST)
    {

        const QString constValueStr = QNetlistGraphicsNode::getFormatConstPortValue(portInst);

        properties.emplace_back(QObject::tr(propertyTypeConstValue), constValueStr);
    }

    // add the connected path
    if(portInst->getPath() != nullptr)
    {
        properties.emplace_back(QObject::tr(propertyTypePathName), portInst->getPath()->getName());
    }

    // convert the bits to a list of stings
    QStringList bits;
    for(auto& bit : portInst->getBits())
    {
        bits.append(QString::number(bit.toInt()));
    }

    // make a normal string
    const QString bitsString = bits.join(", ");

    properties.emplace_back(QObject::tr("Bits"), (bitsString));

    return properties;
}

QString QNetlistGraphicsNode::getFormatConstPortValue(const std::shared_ptr<Yosys::Port>& port)
{

    const unsigned long long constValue = port->getConstPortValue();

    QString constValueStr;

    if(port->getWidth() > 1)
    {
        constValueStr = "0x" + QString::number(constValue, hexBase).toUpper();
    }
    else
    {
        constValueStr = QString::number(constValue);
    }

    return constValueStr;
}

} // namespace OpenNetlistView