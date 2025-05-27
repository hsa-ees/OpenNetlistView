#include <third_party/libcola/cola.h>
#include <third_party/libvpsc/rectangle.h>
#include <third_party/libcola/cluster.h>
#include <third_party/libcola/compound_constraints.h>

#include <QString>

#include <utility>
#include <memory>
#include <map>
#include <vector>
#include <stdexcept>

#include <yosys/module.h>

#include "cola_router.h"

// used for creating debug output of the cola graph
#if defined(_DEBUG) && !defined(EMSCRIPTEN)
#include <clocale>
#include <QLocale>
#endif // defined(_DEBUG) && !defined(EMSCRIPTEN)
namespace OpenNetlistView::Routing {

ColaRouter::ColaRouter()
    : testConv(new cola::TestConvergence)
    , rootCluster(new cola::RootCluster())
{
    this->allEdges = std::vector<cola::Edge>();
    this->edgeLengths = cola::EdgeLengths();
    this->rectangles = std::vector<vpsc::Rectangle*>();
    this->compoundConstraints = cola::CompoundConstraints();
}

ColaRouter::~ColaRouter() = default;

void ColaRouter::setModule(std::shared_ptr<Yosys::Module> module)
{
    this->module = std::move(module);
}

std::shared_ptr<Yosys::Module> ColaRouter::getModule()
{
    return std::move(module);
}

void ColaRouter::setRoutingParameters(const ColaRoutingParameters& routingParameters)
{
    this->routingParameters = routingParameters;

    delete this->testConv;
    this->testConv = new cola::TestConvergence(routingParameters.testTolerance, routingParameters.testMaxIterations);
}

ColaRoutingParameters ColaRouter::getRoutingParameters()
{
    return routingParameters;
}

std::vector<vpsc::Rectangle*> ColaRouter::getRectangles()
{
    return std::move(rectangles);
}

std::vector<cola::Edge> ColaRouter::getEdges()
{
    return std::move(connEdges);
}

void ColaRouter::runCola()
{
    // check if the module is set
    if(!this->module)
    {
        return;
    }

    this->createColaItems();
    this->createColaGraph();
    this->runColaLayout();
}

void ColaRouter::clear()
{
    // delete the contents and recreate the objects
    for(auto& rect : this->rectangles)
    {
        delete rect;
    }

    for(auto& constraint : this->compoundConstraints)
    {
        delete constraint;
    }

    delete this->rootCluster;
    this->rootCluster = new cola::RootCluster();

    delete testConv;
    this->testConv = new cola::TestConvergence(routingParameters.testTolerance, routingParameters.testMaxIterations);

    // reset the vectors
    this->allEdges.clear();
    this->edgeLengths.clear();
    this->rectangles.clear();
    this->compoundConstraints.clear();
}

void ColaRouter::createColaItems()
{

    // create cola representations of nodes and their constant ports
    // set the IDs of the rectangles in the nodes and ports to reference them later
    auto nodes = this->module->getNodes();
    for(auto& node : *nodes)
    {

        if(node->getSymbol() == nullptr)
        {
            throw std::runtime_error("Error durring routing could not generate a cola representation for node: " + node->getName().toStdString());
        }

        // nodes
        const std::map<QString, int> rectIDs = node->getSymbol()->generateColaRep(
            this->allEdges,
            this->edgeLengths,
            this->rectangles,
            this->compoundConstraints,
            this->rootCluster);

        node->setColaRectIDs(rectIDs);
    }

    // create all the external ports of the module as a cola representation
    // set the IDs of the rectangles in the ports to reference them later
    auto ports = this->module->getPorts();

    for(auto& port : *ports)
    {

        if(port->getSymbol() == nullptr)
        {
            throw std::runtime_error("Error durring routing could not generate a cola representation for port: " + port->getName().toStdString());
        }

        const std::map<QString, int> rectIDs = port->getSymbol()->generateColaRep(
            this->allEdges,
            this->edgeLengths,
            this->rectangles,
            this->compoundConstraints,
            this->rootCluster);

        port->setPortColaRectIDs(rectIDs);
    }
}

void ColaRouter::createColaGraph()
{

    this->createColaConnectionsPaths();

    // adds the edges used to connect symbols to the vector of all edges
    this->allEdges.insert(this->allEdges.end(), this->connEdges.begin(), this->connEdges.end());
}

void ColaRouter::createColaConnectionsPaths()
{

    // gets the paths and converts them to cola edges
    auto paths = this->module->getPaths();

    for(auto& path : *paths)
    {

        // if the connection is between two nodes the default length is different
        // then if it is between a node and a port
        double defaultLength = routingParameters.defaultEdgeLength;

        if(path->hasNoConnectBitsConnection())
        {
            continue;
        }

        if(path->getSigSource() == nullptr)
        {
            throw std::runtime_error("Error durring routing could not generate a cola representation for path: " + path->getName().toStdString());
        }

        // get the source of the port
        auto sourcePortID = path->getSigSource()->getPortConRectID();

        // create a edge for each destination of the path
        for(auto& destPort : *(path->getSigDestinations()))
        {
            auto destPortID = destPort->getPortConRectID();

            // if the source port Node is not set, it is a port
            // if the sourceID is -1 its an input or constant
            // if the destID is -1 its an output
            if(sourcePortID == -1)
            {
                // find the object in the map of sigsource that is not body
                auto sigSource = path->getSigSource();

                sourcePortID = sigSource->getPortConRectID();

                defaultLength = routingParameters.defaultEdgeLength;
            }

            if(destPortID == -1)
            {

                destPortID = destPort->getPortConRectID();

                defaultLength = routingParameters.defaultEdgeLength;
            }

            // create the connection and constraints for them
            this->connEdges.emplace_back(sourcePortID, destPortID);
            this->edgeLengths.push_back(defaultLength);

            compoundConstraints.push_back(new cola::SeparationConstraint(vpsc::XDIM, sourcePortID, destPortID, routingParameters.defaultXConstraint, false));
            compoundConstraints.push_back(new cola::SeparationConstraint(vpsc::YDIM, sourcePortID, destPortID, routingParameters.defaultYConstraint, false));
        }
    }
}

void ColaRouter::runColaLayout()
{

    // setup the contraint algorithm
    cola::ConstrainedFDLayout layoutAlg(this->rectangles,
        this->allEdges,
        routingParameters.defaultEdgeLength,
        this->edgeLengths,
        this->testConv);

    layoutAlg.setConstraints(this->compoundConstraints);
    layoutAlg.setClusterHierarchy(this->rootCluster);

    // run the algorithm so that the algorithm will avoid
    // overlapping groups of nodes
    layoutAlg.setAvoidNodeOverlaps(false);
    layoutAlg.run();
    layoutAlg.setAvoidNodeOverlaps(true);
    layoutAlg.run();

#ifndef EMSCRIPTEN
    layoutAlg.makeFeasible();
#endif // EMSCRIPTEN

// creates a svg representation of the graph for debugging
#if defined(_DEBUG) && !defined(EMSCRIPTEN)
    QLocale::setDefault(QLocale::C);
    std::setlocale(LC_NUMERIC, "C");
    layoutAlg.outputInstanceToSVG("rectangularClusters");
    QLocale::setDefault(QLocale::system());
    std::setlocale(LC_NUMERIC, "");
#endif // defined(_DEBUG) && !defined(EMSCRIPTEN)
}

} // namespace OpenNetlistView::Routing