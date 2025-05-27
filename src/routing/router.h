/**
 * @file router.h
 * @brief Header file for the Router class in the OpenNetlistView::Routing namespace.
 *
 * This file contains the declaration of the Router class, which is responsible for routing diagrams
 * using specified symbols. The Router class integrates the cola constraint layout algorithm and the
 * avoid line routing algorithm to achieve the desired routing.
 *
 * The Router class provides methods to set and get the module, run the router, and assign symbols
 * to nodes and ports. It also contains private methods to run the cola layout and avoid line routing.
 *
 * The Router class uses unique pointers for managing the module and shared pointers for managing
 * the symbols. It also contains instances of ColaRouter and AvoidRouter for performing the respective
 * routing tasks.
 *
 * @author Lukas Bauer
 */

#ifndef __ROUTER_H__
#define __ROUTER_H__

#include <QString>
#include <QGraphicsSvgItem>

#include <memory>
#include <vector>
#include <map>

#include <yosys/module.h>
#include <symbol/symbol.h>

#include "cola_router.h"
#include "avoid_router.h"

namespace OpenNetlistView::Routing {

/**
 * @class Router
 * @brief The Router class is responsible for managing the routing of diagrams.
 *
 * This class provides functionalities to set and get diagrams, assign symbols,
 * and run the routing algorithms. It uses the ColaRouter for layout and the
 * AvoidRouter for line routing. The Router class ensures that symbols are
 * assigned to nodes and ports, and it manages the routing data within the module.
 *
 * Usage:
 * - Set the module and symbols using setModule() and setSymbols().
 * - Run the routing process using runRouter().
 * - Clear the routing data using clear().
 */
class Router
{

public:
    constexpr const static char* busIdentifier = "-bus"; ///< the identifier for bus symbols

public:
    /**
     * @brief Construct a new Router object
     *
     */
    Router();

    /**
     * @brief Destroy the Router object
     *
     */
    ~Router();

    /**
     * @brief Set the Module object
     *
     * @param module the module to set
     */
    void setModule(std::shared_ptr<Yosys::Module> module);

    /**
     * @brief Get the Module object
     *
     * @return std::shared_ptr<Yosys::Module> the module
     */
    std::shared_ptr<Yosys::Module> getModule();

    /**
     * @brief Set the Symbols object
     *
     * @param symbols the symbols to set for the routing
     */
    void setSymbols(const std::shared_ptr<std::map<QString, std::shared_ptr<Symbol::Symbol>>>& symbols);

    /**
     * @brief Set the Routing Parameters object
     *
     * also resets the routing from cola and avoid router to ensure that the new parameters are used
     *
     * @param routingParameters the routing parameters to set
     */
    void setRoutingParameters(const ColaRoutingParameters& routingParameters);

    /**
     * @brief gets the routing parameter object
     *
     * @return cola routing parameters of router
     */
    ColaRoutingParameters getRoutingParameters();

    /**
     * @brief Run the router
     *
     * - this assigns the symbols to the nodes and ports
     * - runs the cola constraint layout algorithm
     * - runs the avoid router
     *
     */
    void runRouter();

    /**
     * @brief Clear the router
     *
     * This resets the cola and avoid routers and clears the routing data from the module
     */
    void clear();

private:
    /**
     * @brief assign the symbols to the nodes and ports
     *
     */
    void assignSymbols();

    /**
     * @brief run the cola layout
     *
     */
    void runCola();

    /**
     * @brief run the avoid line routing
     *
     */
    void runAvoid();

    /**
     * @brief create a join or split symbol
     *
     * Checks if the split or join symbol that is needed already exists
     * if it does use it otherwise create a new one and add it to the
     * symbols map
     *
     * @param node the split or join the symbol needs to be created for
     * @return std::shared_ptr<Symbol::Symbol> the created symbol
     */
    std::shared_ptr<Symbol::Symbol> createJoinSplit(const std::shared_ptr<Yosys::Node>& node);

    /**
     * @brief create a module symbol
     *
     * Creates a symbol for a module node symbol if the one needed
     * does not already exist. Otherwise it returns a pointer to the
     * existing symbol.
     *
     * @param node the module node to create the symbol for
     * @return std::shared_ptr<Symbol::Symbol> the created symbol
     */
    std::shared_ptr<Symbol::Symbol> createGenericSymbol(const std::shared_ptr<Yosys::Node>& node);

    std::shared_ptr<Yosys::Module> module;                                       ///< the module to route
    std::shared_ptr<std::map<QString, std::shared_ptr<Symbol::Symbol>>> symbols; ///< the symbols to use in the routing

    ColaRouter cola;   ///< the instance of the cola router
    AvoidRouter avoid; ///< the instance of the avoid router
};

} // namespace OpenNetlistView::Routing

#endif // __ROUTER_H__