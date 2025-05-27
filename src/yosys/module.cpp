#include <QGraphicsItem>
#include <QString>
#include <QStringList>

#include <memory>
#include <vector>
#include <ostream>
#include <sstream>
#include <algorithm>
#include <utility>
#include <map>
#include <cmath>
#include <cstdint>

#include <qnetlistgraphicsnode.h>
#include <qnetlistgraphicspath.h>

#include "node.h"
#include "port.h"
#include "path.h"
#include "component.h"
#include "netname.h"

#include "module.h"

namespace OpenNetlistView::Yosys {

Module::Module(QString type)
    : type(std::move(type)){

      };

Module::~Module() = default;

void Module::setType(QString type)
{
    this->type = std::move(type);
}

QString Module::getType() const
{
    return type;
}

void Module::setIsRouted()
{
    isRouted = true;
}

void Module::resetIsRouted()
{
    isRouted = false;
}

bool Module::getIsRouted() const
{
    return isRouted;
}

void Module::addSubModule(const QString& instName, const std::shared_ptr<Module>& module)
{
    subModules[instName] = module;
}

std::map<QString, std::shared_ptr<Module>> Module::getSubModules() const
{
    return subModules;
}

void Module::addPath(const std::shared_ptr<Path>& path)
{
    paths.emplace_back(path);
}

void Module::addNode(const std::shared_ptr<Node>& node)
{
    nodes.emplace_back(node);
}

void Module::addPort(const std::shared_ptr<Port>& port)
{
    ports.emplace_back(port);
}

void Module::addNetname(const std::shared_ptr<Netname>& netname)
{
    netnames.emplace_back(netname);
}

std::unique_ptr<std::vector<std::shared_ptr<Path>>> Module::getPaths() const
{
    return std::make_unique<std::vector<std::shared_ptr<Path>>>(this->paths);
}

std::unique_ptr<std::vector<std::shared_ptr<Node>>> Module::getNodes() const
{
    return std::make_unique<std::vector<std::shared_ptr<Node>>>(this->nodes);
}

std::unique_ptr<std::vector<std::shared_ptr<Port>>> Module::getPorts() const
{
    return std::make_unique<std::vector<std::shared_ptr<Port>>>(this->ports);
}

std::unique_ptr<std::vector<std::shared_ptr<Netname>>> Module::getNetnames() const
{
    return std::make_unique<std::vector<std::shared_ptr<Netname>>>(this->netnames);
}

void Module::removePath(const std::shared_ptr<Path>& path)
{
    // find the path in the vector and remove it
    const auto findIt = std::find(paths.begin(), paths.end(), path);

    if(findIt != paths.end())
    {
        paths.erase(findIt);
    }
}

std::shared_ptr<Node> Module::getNodeByColaRectID(const int colaRectID) const
{
    // find the node that matches the given colaRectID and return it
    auto iterator = std::find_if(nodes.begin(),
        nodes.end(),
        [colaRectID](const std::shared_ptr<Node>& node) {
            return node->getColaRectID() == colaRectID;
        });
    return (iterator != nodes.end()) ? *iterator : nullptr;
}

std::shared_ptr<Port> Module::getPortByColaRectID(const int colaRectID) const
{

    // findes the port that matches the given colaRectID
    auto iterator = std::find_if(ports.begin(),
        ports.end(),
        [colaRectID](const std::shared_ptr<Port>& port) {
            return port->getPortConRectID(true) == colaRectID;
        });

    return (iterator != ports.end()) ? *iterator : nullptr;
}

std::shared_ptr<Path> Module::getPathByColaSrcDstIDs(const int srcID, const int dstID) const
{

    // finds paths that match the given and Port IDs regardless
    // if it is a RectID or a PortConRectID
    auto iterator = std::find_if(paths.begin(),
        paths.end(),
        [srcID, dstID](const std::shared_ptr<Path>& path) {
            if(path->getSigSource()->getPortConRectID() == srcID)
            {
                for(const auto& port : *path->getSigDestinations())
                {
                    if(port->getPortConRectID() == dstID)
                    {
                        return true;
                    }
                }
            }

            return false;
        });

    return (iterator != paths.end()) ? *iterator : nullptr;
}

std::vector<QGraphicsItem*> Module::convertToQt()
{

    // converts all the paths, nodes and ports to QGraphicsItems
    std::vector<QGraphicsItem*> qItems;

    for(const auto& path : paths)
    {
        auto* qtPath = path->convertToQt();

        qtPath->setYosysPath(path);

        qItems.emplace_back(qtPath);
    }

    for(const auto& node : nodes)
    {
        auto* qtNode = node->convertToQt();
        auto component = std::dynamic_pointer_cast<Yosys::Component>(node);
        if(component)
        {
            qtNode->setComponent(component);
        }

        qItems.emplace_back(qtNode);
    }

    for(const auto& port : ports)
    {
        auto* qtPort = port->convertToQt();
        auto component = std::dynamic_pointer_cast<Yosys::Component>(port);

        if(component)
        {
            qtPort->setComponent(component);
        }

        qItems.emplace_back(qtPort);
    }

    return qItems;
}

void Module::clearRoutingData()
{

    // clears the routing data from all paths, nodes and ports
    for(const auto& path : paths)
    {
        path->clearRoutingData();
    }

    for(const auto& node : nodes)
    {
        node->clearRoutingData();
    }

    for(const auto& port : ports)
    {
        port->clearRoutingData();
    }
}

bool Module::hasConnection() const
{
    return std::all_of(ports.begin(), ports.end(), [](const auto& port) { return (port != nullptr && port->hasConnection()); }) &&
           std::all_of(nodes.begin(), nodes.end(), [](const auto& node) { return (node != nullptr && node->hasConnection()); }) &&
           std::all_of(paths.begin(), paths.end(), [](const auto& path) { return (path != nullptr && path->hasConnection()); });
}

bool Module::isEmpty() const
{
    return paths.empty() && nodes.empty() && ports.empty();
}

uint64_t Module::getMaxBitNumber() const
{
    // get the maximum bit number in the module
    uint64_t maxBitNumber = 0;

    for(const auto& port : ports)
    {
        maxBitNumber = std::max(maxBitNumber, port->getMaxBitNumber());
    }

    for(const auto& node : nodes)
    {
        for(const auto& port : node->getPorts())
        {
            maxBitNumber = std::max(maxBitNumber, port->getMaxBitNumber());
        }
    }

    return maxBitNumber;
}
std::shared_ptr<Netname> Module::getNetnameByBits(const QStringList& bits) const
{
    // find the netname that matches the given bits
    auto iterator = std::find_if(netnames.begin(),
        netnames.end(),
        [bits](const std::shared_ptr<Netname>& netname) {
            return netname->getBits() == bits;
        });

    return (iterator != netnames.end()) ? *iterator : nullptr;
}

std::shared_ptr<Path> Module::getPathByBits(const QStringList& bits) const
{
    // find the netname that matches the given bits
    auto iterator = std::find_if(paths.begin(),
        paths.end(),
        [bits](const std::shared_ptr<Path>& netname) {
            return netname->getBits() == bits;
        });

    return (iterator != paths.end()) ? *iterator : nullptr;
}

bool Module::hasModuleInvalidPaths() const
{
    return !std::all_of(paths.begin(), paths.end(), [](const auto& path) { return path->hasConnection(); }) ||
           paths.empty();
}

std::ostream&
operator<<(std::ostream& outputStream, const Module& module)
{

    std::stringstream sStream = std::stringstream();

    sStream << "Module(\n";

    // add all the paths
    sStream << "  Paths: [\n";
    auto paths = module.getPaths();
    for(const auto& path : *paths)
    {
        sStream << "    " << *path << "\n";
    }

    sStream << "  ],\n";

    // add all the nodes
    sStream << "  Nodes: [\n";

    auto nodes = module.getNodes();
    for(const auto& node : *nodes)
    {
        sStream << "    " << *node << "\n";
    }

    sStream << "  ],\n";

    // add all the ports
    sStream << "  Ports: [\n";

    auto ports = module.getPorts();
    for(const auto& port : *ports)
    {
        sStream << "    " << *port << "\n";
    }

    sStream << ")";

    return outputStream << sStream.str();
}

} // namespace OpenNetlistView::Yosys