
/**
 * @file module.h
 * @brief Header file for the Module class in the OpenNetlistView namespace.
 *
 * This file contains the definition of the Module class, which is used to
 * represent a module consisting of paths, nodes, and ports.
 *
 * @author Lukas Bauer
 */

#ifndef __YOSYS_MODULE_H__
#define __YOSYS_MODULE_H__

#include <QString>
#include <QGraphicsItem>
#include <QVariant>

#include <vector>
#include <memory>
#include <map>
#include <QString>

#include "component.h"
#include "path.h"
#include "node.h"
#include "port.h"
#include "netname.h"

namespace OpenNetlistView::Yosys {

/**
 * @class Module
 * @brief Represents a module consisting of paths, nodes, and ports.
 */
class Module
{
public:
    /**
     * @brief Constructs a new Module object.
     */
    Module(QString type);

    /**
     * @brief Destroys the Module object.
     */
    ~Module();

    /**
     * @brief Sets the type of the module.
     *
     * @param type The type of the module.
     */
    void setType(QString type);

    /**
     * @brief Retrieves the type of the module.
     *
     * @return The type of the module.
     */
    QString getType() const;

    /**
     * @brief Sets the isRouted flag.
     */
    void setIsRouted();

    /**
     * @brief Resets the isRouted flag to false.
     */
    void resetIsRouted();

    /**
     * @brief Retrieves the isRouted flag.
     *
     * @return The isRouted flag.
     */
    bool getIsRouted() const;

    /**
     * @brief Adds a submodule to the module.
     *
     * @param instName The instance name of the submodule.
     * @param module A shared pointer to the Module object to be added.
     */
    void addSubModule(const QString& instName, const std::shared_ptr<Module>& module);

    /**
     * @brief Retrieves all submodules in the module.
     *
     * @return A map of the instance name and the submodule.
     */
    std::map<QString, std::shared_ptr<Module>> getSubModules() const;

    /**
     * @brief Adds a path to the module.
     *
     * @param path A shared pointer to the Path object to be added.
     */
    void addPath(const std::shared_ptr<Path>& path);

    /**
     * @brief Adds a node to the module.
     *
     * @param node A shared pointer to the Node object to be added.
     */
    void addNode(const std::shared_ptr<Node>& node);

    /**
     * @brief Adds a port to the module.
     *
     * @param port A shared pointer to the Port object to be added.
     */
    void addPort(const std::shared_ptr<Port>& port);

    /**
     * @brief Adds a netname to the module.
     *
     * @param netname A shared pointer to the Netname object to be added.
     */
    void addNetname(const std::shared_ptr<Netname>& netname);

    /**
     * @brief Retrieves all paths in the module.
     *
     * @return A unique pointer to a vector of shared pointers to Path objects.
     */
    std::unique_ptr<std::vector<std::shared_ptr<Path>>> getPaths() const;

    /**
     * @brief Retrieves all nodes in the module.
     *
     * @return A unique pointer to a vector of shared pointers to Node objects.
     */
    std::unique_ptr<std::vector<std::shared_ptr<Node>>> getNodes() const;

    /**
     * @brief Retrieves all ports in the module.
     *
     * @return A unique pointer to a vector of shared pointers to Port objects.
     */
    std::unique_ptr<std::vector<std::shared_ptr<Port>>> getPorts() const;

    /**
     * @brief Retrieves all netnames in the module.
     *
     * @return A unique pointer to a vector of shared pointers to Netnames objects.
     */

    std::unique_ptr<std::vector<std::shared_ptr<Netname>>> getNetnames() const;

    /**
     * @brief Removes a path from the module.
     *
     * @param path A shared pointer to the Path object to be removed.
     */
    void removePath(const std::shared_ptr<Path>& path);

    /**
     * @brief Get the Node By ColaRectID object
     *
     * this colaRectID is the position of the node in the vector of rectangles
     *
     * @param colaRectID
     * @return std::shared_ptr<Node>
     */
    std::shared_ptr<Node> getNodeByColaRectID(const int colaRectID) const;

    /**
     * @brief Get the Port By ColaRectID object
     *
     * this colaRectID is the position of the port in the vector of rectangles
     *
     * @param colaRectID
     * @return std::shared_ptr<Port>
     */
    std::shared_ptr<Port> getPortByColaRectID(const int colaRectID) const;

    /**
     * @brief Get the Path By ColaSrcDstIDs object
     *
     * this function is used to get the source and destination rectangles
     * of a path by their colaRectIDs
     *
     * @param srcID the id of the signal source rectangle
     * @param dstID the id of the signal destination rectangle
     * @return std::shared_ptr<Path>
     */
    std::shared_ptr<Path> getPathByColaSrcDstIDs(const int srcID, const int dstID) const;

    /**
     * @brief converts all paths, nodes and ports to QGraphicsItems
     *
     * @return std::vector<QGraphicsItem*>
     */
    std::vector<QGraphicsItem*> convertToQt();

    /**
     * @brief clears the routing data from all paths and ports and nodes
     *
     * This resets the cola and avoid routing data
     */
    void clearRoutingData();

    /**
     * @brief Checks if all components in the module have a connection.
     *
     * @return true if all components have a connection, false otherwise.
     */
    bool hasConnection() const;

    /**
     * @brief Checks if the module is empty.
     *
     * @return true if the module is empty, false otherwise.
     */
    bool isEmpty() const;

    /**
     * @brief Retrieves the maximum bit number in the module.
     *
     * @return The maximum bit number in the module.
     */
    uint64_t getMaxBitNumber() const;

    /**
     * @brief Retrieves the Netname object by the bits.
     *
     * @param bits The bits of the Netname object.
     * @return A shared pointer to the Netname object.
     */
    std::shared_ptr<Netname> getNetnameByBits(const QStringList& bits) const;

    /**
     * @brief Retrieves the Path object by the bits.
     *
     * @param bits The bits of the Path object.
     * @return A shared pointer to the Path object.
     */
    std::shared_ptr<Path> getPathByBits(const QStringList& bits) const;

    /**
     * @brief Checks if the module has invalid paths.
     *
     * a module has invalid paths if one path has no connection or the list
     * of paths is empty
     *
     * @return true if the module has invalid paths, false otherwise.
     */
    bool hasModuleInvalidPaths() const;

    /**
     * @brief Overloads the stream insertion operator to print the module.
     *
     * @param outputStream The output stream to which the module will be printed.
     * @param module The Module object to be printed.
     * @return The output stream with the module data.
     */
    friend std::ostream&
    operator<<(std::ostream& outputStream, const Module& module);

private:
    QString type;                                   ///< The type of the module.
    std::vector<std::shared_ptr<Path>> paths;       ///< Vector of shared pointers to Path objects.
    std::vector<std::shared_ptr<Node>> nodes;       ///< Vector of shared pointers to Node objects.
    std::vector<std::shared_ptr<Port>> ports;       ///< Vector of shared pointers to Port objects.
    std::vector<std::shared_ptr<Netname>> netnames; ///< Vector of shared pointers to Netnames objects.

    std::map<QString, std::shared_ptr<Module>> subModules; ///< Vector of shared pointers to submodules.

    bool isRouted = false; ///< Flag indicating if the module has been routed.
};

} // namespace OpenNetlistView::Yosys

Q_DECLARE_METATYPE(OpenNetlistView::Yosys::Module);
Q_DECLARE_METATYPE(std::shared_ptr<OpenNetlistView::Yosys::Module>);

#endif // __YOSYS_MODULE_H__