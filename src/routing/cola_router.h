/**
 * @file cola_router.h
 * @brief Defines the ColaRouter class for routing diagrams using the cola layout algorithm.
 *
 * This file contains the declaration of the ColaRouter class, which is responsible for
 * routing diagrams by creating and processing cola graph representations of nodes and edges.
 * The class provides methods to set and get diagrams, retrieve rectangles and edges, and
 * run the cola layout algorithm. It also includes private methods for creating cola items,
 * graphs, and connections.
 *
 * The ColaRouter class uses the Yosys library for module representation and the cola library
 * for layout processing. It also integrates with the vpsc library for handling rectangles and
 * constraints.
 *
 * @author Lukas Bauer
 */

#ifndef __COLA_ROUTER_H__
#define __COLA_ROUTER_H__

#include <third_party/libcola/cola.h>
#include <third_party/libvpsc/rectangle.h>
#include <yosys/module.h>

#include <vector>
#include <memory>

namespace OpenNetlistView::Routing {

/**
 * @struct RoutingParameters
 * @brief A structure to hold parameters for routing.
 *
 * This structure contains various parameters that are used for routing diagrams.
 * These parameters include constraints, tolerances, and edge lengths that are
 * essential for the cola layout algorithm.

 */
struct ColaRoutingParameters
{
    double defaultXConstraint; ///< The default x constraint.
    double defaultYConstraint; ///< The default y constraint.
    double testTolerance;      ///< The test tolerance.
    int testMaxIterations;     ///< The test iterations.
    double defaultEdgeLength;  ///< default edge length
};

/**
 * @class ColaRouter
 * @brief A class to handle the routing of diagrams using the cola layout algorithm.
 *
 * The ColaRouter class provides functionality to set and get diagrams, retrieve rectangles and edges,
 * and run the cola layout algorithm. It also includes methods to clear the router state and create
 * cola items and graphs. The class uses various constants for default constraints, tolerances, and edge lengths.
 *
 * The module contains paths, nodes, and ports to be routed. The class supports creating cola representations
 * of nodes, running the cola layout algorithm, and handling obstacle avoidance using rectangles and edges.
 *
 */
class ColaRouter
{

public:
    /**
     * @brief Construct a new Cola Router object
     *
     */
    ColaRouter();

    /**
     * @brief Destroy the Cola Router object
     *
     */
    ~ColaRouter();

    /**
     * @brief Set the Module object
     *
     * The module contains the paths, nodes and ports to be routed
     *
     * @param module The module to be set
     */
    void setModule(std::shared_ptr<Yosys::Module> module);

    /**
     * @brief Get the Module object
     *
     * This can be used to get the module after routing
     *
     * @return std::shared_ptr<Yosys::Module> The module
     */
    std::shared_ptr<Yosys::Module> getModule();

    /**
     * @brief sets new routing parameters
     *
     * this is done to enshure that the new parameters are used
     *
     * @param routingParameters the new routing parameters
     */
    void setRoutingParameters(const ColaRoutingParameters& routingParameters);

    /**
     * @brief gets the routing parametes
     *
     * @return the routing parameters of the cola router
     */
    ColaRoutingParameters getRoutingParameters();

    /**
     * @brief Get the Rectangles object
     *
     * This can be used to get the rectangles to further process them
     * for example with libavoid to do obstacle avoidance
     *
     * @return std::vector<vpsc::Rectangle*> The rectangles
     */
    std::vector<vpsc::Rectangle*> getRectangles();

    /**
     * @brief Get the Edges object
     *
     * This can be used to get the edges to further process them
     * for example with libavoid to do obstacle avoidance
     *
     * @return std::vector<cola::Edge> The edges
     */
    std::vector<cola::Edge> getEdges();

    /**
     * @brief Run the cola layout
     *
     * This function runs the cola layout on the module
     *
     */
    void runCola();

    /**
     * @brief Clear the cola router
     *
     * This function clears the cola router and resets the state
     *
     */
    void clear();

private:
    /**
     * @brief Create the cola items
     *
     * This function creates the cola representation
     * of the nodes. This is done by using the reference
     * to the symbol in the node object
     *
     */
    void createColaItems();

    /**
     * @brief Create a Cola Graph object
     *
     * This function calls functions to create the cola
     * edges and the adds them to the list of all edges
     *
     * @throw std::runtime_error if a cola representation could not be generated
     *
     */
    void createColaGraph();

    /**
     * @brief Create a Cola Graph object
     *
     * This function creates the edges that connect
     * the nodes together.
     *
     */
    void createColaConnectionsPaths();

    /**
     * @brief Run the cola layout
     *
     * This function runs the cola layout algorithm
     *
     */
    void runColaLayout();

    std::shared_ptr<Yosys::Module> module;         ///< the module to be routed from the yosys data
    std::vector<cola::Edge> allEdges;              ///< all edges of the graph including those within the symbols
    std::vector<cola::Edge> connEdges;             ///< the edges connecting the symbols
    cola::EdgeLengths edgeLengths;                 ///< the inital edge lengths of the cola graph
    std::vector<vpsc::Rectangle*> rectangles;      ///< the rectangles used in the cola graph
    cola::CompoundConstraints compoundConstraints; ///< the constraints between rectangles and allEdges
    cola::RootCluster* rootCluster;                ///< the top level cluster of objects in cola graph
    cola::TestConvergence* testConv;               ///< the convergence test for cola used in constraint layouting
    ColaRoutingParameters routingParameters;       ///< the routing parameters for the cola router
};

} // namespace OpenNetlistView::Routing

#endif // __COLA_ROUTER_H__