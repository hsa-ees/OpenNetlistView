/**
 * @file avoid_router.h
 * @brief Header file for the AvoidRouter class used for avoid line routing in diagrams.
 *
 * This file contains the definition of the AvoidRouter class, which is responsible for
 * routing lines in diagrams while avoiding overlaps with shapes and other lines. It uses
 * the libavoid library for routing and provides methods to set the module, rectangles,
 * and edges to be routed, as well as to run the routing process.
 *
 * The AvoidRouter class is part of the OpenNetlistView::Routing namespace.
 *
 * @author Lukas Bauer
 */

#ifndef __AVOID_ROUTER_H__
#define __AVOID_ROUTER_H__

#include <third_party/libavoid/router.h>
#include <third_party/libvpsc/rectangle.h>
#include <third_party/libcola/cola.h>
#include <third_party/libtopology/orthogonal_topology.h>

#include <memory>
#include <vector>

#include <yosys/module.h>

namespace OpenNetlistView::Routing {

/**
 * @class AvoidRouter
 * @brief A class for performing avoid line routing in diagrams.
 *
 * The AvoidRouter class provides functionality to route lines in a module
 * while avoiding overlaps with shapes and other lines. It uses a set of
 * rectangles and edges from a cola graph to determine the routing paths.
 * The class offers methods to set the module, rectangles, and edges, as well
 * as to run the routing algorithm and clear the router's state.
 *
 * The routing process involves creating an internal representation of the
 * avoid lines and then performing the routing based on this representation.
 * The class maintains various internal data structures to manage the routing
 * process, including the module, rectangles, edges, and connections.
 */
class AvoidRouter
{

private:
    constexpr const static double bufferDistance{10.0F}; ///< The distance between lines and shapes
    constexpr const static double nudgeDistance{7.5F};   ///< The distance to between lines and lines

public:
    /**
     * @brief Constructor for the AvoidRouter class.
     *
     * Initializes the AvoidRouter object with the default routing options.
     */
    AvoidRouter();

    /**
     * @brief Destructor for the AvoidRouter class.
     *
     * Cleans up resources used by the AvoidRouter object.
     */
    ~AvoidRouter();

    /**
     * @brief Sets the module to be routed.
     *
     * @param module The module to be routed.
     */
    void setModule(std::shared_ptr<Yosys::Module> module);

    /**
     * @brief Gets the module to be routed.
     *
     * @return The module to be routed.
     */
    std::shared_ptr<Yosys::Module> getModule();

    /**
     * @brief Sets the rectangles from the cola graph to route.
     *
     * @param rectangles The rectangles from the cola graph to route.
     */
    void setColaRectangles(std::vector<vpsc::Rectangle*> rectangles);

    /**
     * @brief Sets the edges from the cola graph to route.
     *
     * @param edges The edges from the cola graph to route.
     */
    void setColaEdges(std::vector<cola::Edge> edges);

    /**
     * @brief Runs the avoid line routing.
     */
    void runAvoid();

    /**
     * @brief cleans the state of the avoid router
     *
     */
    void clear();

private:
    /**
     * @brief Creates the avoid line routing representation.
     */
    void createAvoidRep();

    /**
     * @brief Routes the avoid lines.
     */
    void routeAvoid();

    std::shared_ptr<Yosys::Module> module;        ///< the module to be routed
    std::vector<vpsc::Rectangle*> colaRectangles; ///< the rectangles from the cola graph to route
    std::vector<cola::Edge> colaEdges;            ///< the edges from the cola graph to route
    cola::CompoundConstraints colaConstraints;    ///< the constraints from the cola graph to route
    cola::VariableIDMap colaIDMap;                ///< the ID map from the cola graph to route
    topology::AvoidTopologyAddon* topologyAddon;  ///< the topology addon for the avoid line routing

    Avoid::Router* router;                             ///< the router to be used for the avoid line routing
    std::vector<Avoid::Rectangle*> avoidRectangles;    ///< the rectangles to be used for the avoid line routing
    std::vector<Avoid::ShapeRef*> avoidShapes;         ///< the shapes to be used for the avoid line routing
    std::vector<Avoid::ShapeConnectionPin*> avoidPins; ///< the pins to be used for the avoid line routing
    std::map<int, Avoid::ConnEnd*> connEnds;           ///< the ends of the connections to be used for the avoid line routing
    std::vector<Avoid::ConnRef*> avoidConRefs;         ///< the connections to be used for the avoid line routing
    std::vector<vpsc::Rectangle*> avoidColaRects;      ///< the rectangles to be used for the avoid line routing
    cola::RootCluster* avoidRootCluster;               ///< the root cluster to be used for the avoid line routing

    int avoidConnID = 1;  ///< the ID of the avoid connection
    int avoidShapeID = 1; ///< the ID of the avoid shape
};

} // namespace OpenNetlistView::Routing

#endif // __AVOID_ROUTER_H__