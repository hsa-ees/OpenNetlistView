#include <QString>
#include <QRegularExpression>
#include <QGraphicsSvgItem>
#include <third_party/libavoid/shape.h>
#include <third_party/libavoid/geomtypes.h>

#include <vector>
#include <memory>
#include <utility>
#include <sstream>
#include <ostream>
#include <algorithm>
#include <map>
#include <tuple>
#include <cmath>
#include <stdexcept>
#include <iterator>

#include <symbol/symbol.h>
#include <qnetlistgraphicsnode.h>

#include "port.h"
#include "component.h"

#include "node.h"

namespace OpenNetlistView::Yosys {

Node::Node(QString name, QString type, std::vector<std::shared_ptr<Port>>& ports)
    : Component(std::move(name))
    , type(std::move(type))
    , ports(ports)
    , colaRectID(-1)
    , avoidRectReference(nullptr)
{
}

Node::~Node() = default;

void Node::addPorts(std::vector<std::shared_ptr<Port>>& ports)
{
    this->ports = ports;
}

void Node::setSymbol(const std::shared_ptr<Symbol::Symbol>& symbol)
{
    this->symbol = symbol;
}

std::shared_ptr<Symbol::Symbol> Node::getSymbol() const
{
    return this->symbol;
}

void Node::setColaRectIDs(const std::map<QString, int>& colaRectIDs)
{

    this->colaRectID = colaRectIDs.at(nodeColaName);

    // set the ids of the ports below
    for(auto& port : this->ports)
    {
        auto name = port->getName();
        // get the key value pair for the port
        auto iterator = colaRectIDs.find(name);

        // if the name of port did not match try the name alias
        if(iterator == colaRectIDs.end())
        {
            iterator = colaRectIDs.find(port->getSymbolNameAlias());
        }

        if(iterator == colaRectIDs.end())
        {
            throw std::runtime_error("Error durring routing could not assign a cola rectangle to the port: " +
                                     name.toStdString() + " of node: " + this->getName().toStdString());
        }

        port->setPortColaRectIDs({*iterator});
    }
}

int Node::getColaRectID() const
{
    return this->colaRectID;
}

void Node::setAvoidRectReference(Avoid::ShapeRef* avoidRectReference)
{
    this->avoidRectReference = avoidRectReference;
}

Avoid::ShapeRef* Node::getAvoidRectReference()
{
    return this->avoidRectReference;
}

std::vector<std::shared_ptr<Port>>& Node::getPorts()
{
    return ports;
}

QString Node::getType()
{
    return type;
}

void Node::setType(QString type)
{
    this->type = std::move(type);
}

std::tuple<int, int> Node::getSplitJoinBitPositions(const std::shared_ptr<Port>& labelPort)
{

    // abort if the node is not a split or join node
    if(this->type != "split" && this->type != "join")
    {
        return std::make_tuple(-1, -1);
    }

    QString searchText{};

    if(this->type == "split")
    {
        searchText = "in";
    }
    else if(this->type == "join")
    {
        searchText = "out";
    }

    // search for the port with this name
    auto portIt = std::find_if(this->ports.begin(), this->ports.end(), [&labelPort, &searchText](const std::shared_ptr<Port>& port) {
        return port->getName() == searchText;
    });

    if(portIt == this->ports.end())
    {
        return std::make_tuple(-1, -1);
    }

    const auto mainBits = (*portIt)->getBits();
    const auto labelBits = labelPort->getBits();

    // search for the label ports in the bits of the found port
    auto labelPortIt = std::search(mainBits.begin(), mainBits.end(), labelBits.begin(), labelBits.end());

    if(labelPortIt == (*portIt)->getBits().end())
    {
        return std::make_tuple(-1, -1);
    }

    // get the distance of the label port to the first bit
    const auto distance = std::distance(mainBits.begin(), labelPortIt);

    return std::make_tuple(distance + labelPort->getWidth() - 1, distance);
}

bool Node::hasConnection() const
{
    return std::all_of(this->ports.begin(), this->ports.end(), [](const std::shared_ptr<Port>& port) {
        return (port != nullptr && port->hasConnection());
    });
}

QNetlistGraphicsNode* Node::convertToQt()
{

    auto* svgItem = new QNetlistGraphicsNode();

    // get the renderer if it is not null set it to the svg item
    auto* qRenderer = symbol->getQRenderer();

    if(qRenderer != nullptr)
    {
        svgItem->setSharedRenderer(qRenderer);
    }

    // set the position of the symbol
    const Avoid::Point centerPoint = this->avoidRectReference->position();

    const auto boundingBox = symbol->getBoundingBox();

    svgItem->setPos(centerPoint.x - (boundingBox.first / 2),
        centerPoint.y - (boundingBox.second / 2));

    // set this nodes qtitem to the one created
    this->setGraphicsItem(svgItem);

    return svgItem;
}

void Node::clearRoutingData()
{
    this->colaRectID = -1;
    this->avoidRectReference = nullptr;
}

std::ostream&
operator<<(std::ostream& outputStream, const Node& node)
{

    std::stringstream sStream = std::stringstream();

    // add the node name
    sStream << "Node( " << node.getName().toStdString() << ", " << node.type.toStdString() << ", ";

    // add all ports
    sStream << "Ports: [";

    for(const auto& port : node.ports)
    {
        sStream << *port << ",\n";
    }

    sStream << "])";

    if(node.symbol != nullptr)
    {
        sStream << "\nSymbol: " << node.symbol->getName().toStdString() << "\n";
    }

    sStream << "colaRectID: " << node.colaRectID << "\n";

    return outputStream << sStream.str();
}

} // namespace OpenNetlistView::Yosys