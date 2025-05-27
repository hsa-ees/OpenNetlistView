#include <third_party/libavoid/router.h>
#include <third_party/libavoid/geomtypes.h>
#include <third_party/libavoid/shape.h>
#include <third_party/libavoid/connectionpin.h>
#include <third_party/libavoid/connend.h>
#include <third_party/libvpsc/rectangle.h>
#include <third_party/libcola/cola.h>
#include <third_party/libcola/cluster.h>
#include <third_party/libtopology/orthogonal_topology.h>

#include <utility>
#include <memory>
#include <vector>
#include <cmath>

#include <yosys/module.h>
#include <symbol/port.h>

#include "avoid_router.h"

// used for debug output generation in the debug build
#if defined(_DEBUG) && !defined(EMSCRIPTEN)
#include <QLocale>
#include <clocale>
#endif // defined(_DEBUG) && !defined(EMSCRIPTEN)

namespace OpenNetlistView::Routing {

AvoidRouter::AvoidRouter()
    : module(nullptr)
    , topologyAddon(nullptr)
    , avoidRootCluster(new cola::RootCluster())
    , router(new Avoid::Router(Avoid::OrthogonalRouting))

{
    router->setRoutingOption(Avoid::nudgeSharedPathsWithCommonEndPoint, false);
    router->setRoutingParameter(Avoid::shapeBufferDistance, bufferDistance);
    router->setRoutingParameter(Avoid::idealNudgingDistance, nudgeDistance);
}

AvoidRouter::~AvoidRouter() = default;

void AvoidRouter::setModule(std::shared_ptr<Yosys::Module> module)
{
    this->module = std::move(module);
}

std::shared_ptr<Yosys::Module> AvoidRouter::getModule()
{
    return std::move(module);
}

void AvoidRouter::setColaRectangles(std::vector<vpsc::Rectangle*> rectangles)
{
    this->colaRectangles = std::move(rectangles);
}

void AvoidRouter::setColaEdges(std::vector<cola::Edge> edges)
{
    this->colaEdges = std::move(edges);
}

void AvoidRouter::runAvoid()
{

    // only route if there is a module, cola rectangles and edges available
    if(module == nullptr || colaRectangles.empty() || colaEdges.empty())
    {
        return;
    }

    // generate the avoid graph representation and route the lines
    this->createAvoidRep();
    this->routeAvoid();
}

void AvoidRouter::clear()
{

    // delete the router object and create a new one
    delete router;
    this->avoidRectangles.clear();
    this->avoidShapes.clear();
    this->avoidPins.clear();
    this->avoidConnID = 1;
    this->connEnds.clear();
    this->avoidConRefs.clear();

    for(auto& rect : colaRectangles)
    {
        delete rect;
    }

    this->colaRectangles.clear();
    this->colaEdges.clear();

    router = new Avoid::Router(Avoid::OrthogonalRouting);
    router->setRoutingOption(Avoid::nudgeSharedPathsWithCommonEndPoint, false);
    router->setRoutingParameter(Avoid::shapeBufferDistance, bufferDistance);
    router->setRoutingParameter(Avoid::idealNudgingDistance, nudgeDistance);
}

void AvoidRouter::createAvoidRep()
{

    int rectangleID = 0;
    vpsc::Rectangle* rectNode = nullptr;

    for(const auto& rectangle : colaRectangles)
    {
        const double rectHeight = std::abs(rectangle->getMaxY() - rectangle->getMinY());
        const double rectWidth = std::abs(rectangle->getMaxX() - rectangle->getMinX());

        const double centerX = rectangle->getMinX() + (rectWidth / 2.0F);
        const double centerY = rectangle->getMinY() + (rectHeight / 2.0F);

        // checks if the rectangle is one of a node or of a port
        // if it is a node create the rectangle and set it as the rectNode
        if(rectHeight >= 1 + Symbol::Port::portRectHeight &&
            rectWidth >= 1 + Symbol::Port::portRectWidth)
        {
            auto* avoidRect = new Avoid::Rectangle(Avoid::Point(centerX, centerY), rectWidth, rectHeight);

            avoidRectangles.emplace_back(avoidRect);

            auto* avoidShape = new Avoid::ShapeRef(router, *avoidRect, avoidShapeID);
            avoidShapes.emplace_back(avoidShape);

            avoidColaRects.emplace_back(rectangle);

            this->colaIDMap.addMappingForVariable(avoidColaRects.size() - 1, avoidShapeID);

            avoidShapeID++;

            auto node = module->getNodeByColaRectID(rectangleID);
            if(node != nullptr)
            {
                node->setAvoidRectReference(avoidShape);
            }

            auto port = module->getPortByColaRectID(rectangleID);
            if(port != nullptr && node == nullptr)
            {
                port->setAvoidRectReference(avoidShape);
            }

            rectNode = rectangle;
        }
        // if the rectNode is set and the size of the rectangle is smaller than the node
        // it is a pin then create a pin on the last rectNode and calculate the
        // direction and position of the pin
        else if(rectNode != nullptr)
        {
            Avoid::ConnDirFlag connDir = Avoid::ConnDirFlag::ConnDirNone;

            const double nodeHeight = std::abs(rectNode->getMaxY() - rectNode->getMinY());
            const double nodeWidth = std::abs(rectNode->getMaxX() - rectNode->getMinX());

            const double nodeCenterX = rectNode->getMinX() + (nodeWidth / 2.0F);
            const double nodeCenterY = rectNode->getMinY() + (nodeHeight / 2.0F);

            double xOffset = std::abs(rectNode->getMinX() - rectangle->getMinX());
            double yOffset = std::abs(rectNode->getMinY() - rectangle->getMinY());

            // check if the small node is to the left, right, up, down of the large node
            // then calculate the direction and position of the pin

            if(nodeCenterX > centerX &&
                rectNode->getMinY() < rectangle->getMinY() &&
                rectNode->getMaxY() > rectangle->getMaxY())
            {
                connDir = Avoid::ConnDirFlag::ConnDirLeft;
                yOffset += 1;
            }
            else if(nodeCenterX < centerX &&
                    rectNode->getMinY() < rectangle->getMinY() &&
                    rectNode->getMaxY() > rectangle->getMaxY())
            {
                connDir = Avoid::ConnDirFlag::ConnDirRight;
                yOffset += 1;
            }
            else if(nodeCenterY > centerY &&
                    rectNode->getMinX() < rectangle->getMinX() &&
                    rectNode->getMaxX() > rectangle->getMaxX())
            {
                connDir = Avoid::ConnDirFlag::ConnDirUp;
                xOffset += 1;
            }
            else if(nodeCenterY < centerY &&
                    rectNode->getMinX() < rectangle->getMinX() &&
                    rectNode->getMaxX() > rectangle->getMaxX())
            {
                connDir = Avoid::ConnDirFlag::ConnDirDown;
                xOffset += 1;
            }

            // create the pin and connection objects
            auto* avoidPin = new Avoid::ShapeConnectionPin(avoidShapes.back(),
                this->avoidConnID,
                xOffset,
                yOffset,
                false,
                0,
                connDir);

            avoidPin->setExclusive(false);
            avoidPins.emplace_back(avoidPin);
            auto* connEnd = new Avoid::ConnEnd(avoidShapes.back(), this->avoidConnID);
            connEnds[rectangleID] = connEnd;
            this->avoidConnID++;
        }
        else
        {
            // TODO: Find better way to handle if the main rect is a nullptr
            continue;
        }

        rectangleID++;
    }
}

void AvoidRouter::routeAvoid()
{
    this->router->setTransactionUse(true);

    this->topologyAddon = new topology::AvoidTopologyAddon(this->avoidColaRects,
        this->colaConstraints,
        this->avoidRootCluster,
        this->colaIDMap);

    this->router->setTopologyAddon(this->topologyAddon);

    for(const auto& edge : colaEdges)
    {

        // TODO: Convert all int functions to unsigned int because the cola library uses unsigned int / size_t

        // find the connEnds that match the ids of of the rectangles in the edge
        // vector created by the cola library then create a connection reference
        // and add it to the connection object

        if(connEnds.find(static_cast<int>(edge.first)) == connEnds.end() ||
            connEnds.find(static_cast<int>(edge.second)) == connEnds.end())
        {
            continue;
        }
        auto* connRef = new Avoid::ConnRef(this->router,
            *(connEnds[static_cast<int>(edge.first)]),
            *(connEnds[static_cast<int>(edge.second)]));

        auto conn = module->getPathByColaSrcDstIDs(static_cast<int>(edge.first), static_cast<int>(edge.second));

        if(conn != nullptr)
        {
            conn->addAvoidConnRef(connRef);
            conn->addAvoidPortRelation(connRef, static_cast<int>(edge.second));
        }

        avoidConRefs.emplace_back(connRef);
    }

    // route the graph

    this->router->processTransaction();
    this->router->improveOrthogonalTopology();

    this->router->setTransactionUse(false);

// create a svg file with the graph to use for debugging
#if defined(_DEBUG) && !defined(EMSCRIPTEN)
    std::setlocale(LC_NUMERIC, "C");
    QLocale::setDefault(QLocale::C);

    this->router->outputDiagramSVG("avoidRouting");

    std::setlocale(LC_NUMERIC, "");
    QLocale::setDefault(QLocale::system());
#endif // defined(_DEBUG) && !defined(EMSCRIPTEN)
}

} // namespace OpenNetlistView::Routing