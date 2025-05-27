#include <QString>
#include <QList>
#include <QStringList>
#include <QDebug>
#include <QGraphicsSvgItem>
#include <QVariantList>
#include <QVariant>
#include <third_party/libavoid/shape.h>
#include <third_party/libavoid/geomtypes.h>

#include <sstream>
#include <ostream>
#include <algorithm>
#include <utility>
#include <cstddef>
#include <memory>
#include <cmath>
#include <cstdint>
#include <map>
#include <tuple>

#include <qnetlistgraphicsnode.h>
#include <symbol/symbol.h>

#include "component.h"
#include "port.h"
#include "node.h"

namespace OpenNetlistView::Yosys {

Port::Port(QString name, Port::EDirection direction, QStringList bits, std::shared_ptr<Path> path)
    : Component(std::move(name))
    , symbol(nullptr)
    , direction(direction)
    , bits(std::move(bits))
    , path(std::move(path))
    , avoidRectReference(nullptr)
    , parentNode(nullptr)
    , constValue(0)
{
}
Port::Port(QString name, Port::EDirection direction, QStringList bits)
    : Component(std::move(name))
    , direction(direction)
    , bits(std::move(bits))
    , avoidRectReference(nullptr)
    , parentNode(nullptr)
    , constValue(0)
{
    this->path = nullptr;
}

Port::~Port() = default;

void Port::setSymbol(const std::shared_ptr<Symbol::Symbol>& symbol)
{
    this->symbol = symbol;
}

std::shared_ptr<Symbol::Symbol> Port::getSymbol() const
{
    return this->symbol;
}

void Port::setPath(std::shared_ptr<Path> path)
{
    this->path = std::move(path);
}

std::shared_ptr<Path> Port::getPath() const
{
    return this->path;
}

void Port::setSymbolNameAlias(const QString& alias)
{
    this->symbolNameAlias = alias;
}

QString Port::getSymbolNameAlias() const
{
    return this->symbolNameAlias;
}

void Port::setPortColaRectIDs(std::map<QString, int> colaPortRectIDs)
{
    this->colaPortIDs = std::move(colaPortRectIDs);
}

std::map<QString, int> Port::getPortColaRectIDs() const
{
    return this->colaPortIDs;
}

int Port::getPortConRectID(bool bodyOrPort) const
{

    for(const auto& [name, id] : colaPortIDs)
    {

        // gets the port id of the body or the port
        // if the port is part of a node it only has the id of the port itself
        // and no body rectangle if it is standalone both ids can be found
        if(((this->parentNode == nullptr) && (name == bodyColaName) && bodyOrPort) ||
            ((this->parentNode == nullptr) && (name != bodyColaName) && !bodyOrPort) ||
            ((this->parentNode != nullptr) && colaPortIDs.size() == 1))

        {
            return id;
        }
    }

    return -1;
}

void Port::setAvoidRectReference(Avoid::ShapeRef* avoidRect)
{
    this->avoidRectReference = avoidRect;
}

Avoid::ShapeRef* Port::getAvoidRectReference()
{
    return this->avoidRectReference;
}

Port::EDirection Port::getDirection() const
{
    return direction;
}

uint64_t Port::getWidth()
{
    return bits.size();
}

uint64_t Port::getConstPortValue() const
{
    if(direction != EDirection::CONST)
    {
        return 0;
    }

    return constValue;
}

void Port::setConstPortValue(QStringList bits)
{

    uint64_t constValueTmp = 0;

    // go through the bits in reverse because the order of bits is reversed
    for(auto it = bits.rbegin(); it != bits.rend(); ++it)
    {
        constValueTmp = (constValueTmp << 1) | (*it).toUInt();
    }

    constValue = constValueTmp;
}

void Port::setConstPortValue(uint64_t value)
{
    constValue = value;
}

bool Port::hasConnection() const
{

    // if  has a path it has a connection
    return ((path != nullptr) || this->hasNoConnectBitsConnection());
}

bool Port::hasConstantBits() const
{
    return std::any_of(bits.begin(), bits.end(), [](const QString& bit) { return bit == "1" || bit == "0"; });
}

bool Port::hasNoConnectBitsConnection() const
{
    return std::any_of(bits.begin(), bits.end(), [](const QString& bit) { return bit == "x"; });
}

QStringList Port::getBits()
{
    return bits;
}

uint64_t Port::getMaxBitNumber() const
{
    // scan through every bit and search for the maximum it must be an qlonglong
    // because the QVariantList can contain QStrings and qlonglongs
    uint64_t maxBitNumber = 0;

    for(const auto& bit : bits)
    {
        if(bit != "x" && bit != "0" && bit != "1")
        {
#ifdef EMSCRIPTEN
            maxBitNumber = std::max(maxBitNumber, bit.toULongLong());
#else
            maxBitNumber = std::max(maxBitNumber, bit.toULong());
#endif
        }
    }

    return maxBitNumber;
}

void Port::replaceBits(std::tuple<uint64_t, uint64_t> pos, QStringList bits)
{
    // replace the bits at the given position
    for(uint64_t i = std::get<0>(pos); i <= std::get<1>(pos); i++)
    {
        this->bits[i] = bits[i - std::get<0>(pos)];
    }
}

void Port::setParentNode(std::shared_ptr<Node> node)
{
    this->parentNode = std::move(node);
}

std::shared_ptr<Node> Port::getParentNode()
{
    return this->parentNode;
}

QNetlistGraphicsNode* Port::convertToQt()
{

    auto* svgItem = new QNetlistGraphicsNode();

    auto* qRenderer = symbol->getQRenderer();

    if(qRenderer == nullptr || avoidRectReference == nullptr)
    {
        return svgItem;
    }

    // set the symbols renderer
    svgItem->setSharedRenderer(qRenderer);

    // set the position of the symbol
    const Avoid::Point centerPoint = this->avoidRectReference->position();

    const auto boundingBox = symbol->getBoundingBox();

    svgItem->setPos(centerPoint.x - (boundingBox.first / 2), centerPoint.y - (boundingBox.second / 2));

    // set the qtitem as the paths item
    this->setGraphicsItem(svgItem);

    return svgItem;
}

void Port::clearRoutingData()
{
    this->colaPortIDs.clear();
    this->avoidRectReference = nullptr;
}

std::ostream& operator<<(std::ostream& outputStream, const Port& port)
{

    std::stringstream sStream = std::stringstream();

    // add the port name
    sStream << "Port( " << port.getName().toStdString() << ", ";

    // create string representation of direction
    switch(port.direction)
    {
        case Port::EDirection::INPUT:
            sStream << "INPUT, ";
            break;
        case Port::EDirection::OUTPUT:
            sStream << "OUTPUT, ";
            break;
        case Port::EDirection::CONST:
            sStream << "CONST, ";
            break;
    }

    if(port.path != nullptr)
    {
        sStream << "Path: " << port.path->getName().toStdString() << ", ";
    }
    // add all the bits
    sStream << "Bits: [";

    for(const auto& bit : port.bits)
    {
        sStream << bit.toStdString() << ", ";
    }

    // add if the port is constant
    sStream << "])";

    if(port.symbol != nullptr)
    {
        sStream << "\nSymbol: " << port.symbol->getName().toStdString() << "\n";
    }

    if(!port.colaPortIDs.empty())
    {
        sStream << "Cola Port Rect IDs: \n";

        for(const auto& [name, id] : port.colaPortIDs)
        {
            sStream << "\t" << name.toStdString() << ": " << id << "\n";
        }
    }

    return outputStream << sStream.str();
}

} // namespace OpenNetlistView::Yosys