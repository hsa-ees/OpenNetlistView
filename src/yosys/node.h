/**
 * @file node.h
 * @brief This file contains the definition of the Node class used in the OpenNetlistView namespace.
 *
 * The Node class represents a node in the diagram viewer, which can have ports
 *
 * @author Lukas Bauer
 */

#ifndef __NODE_H__
#define __NODE_H__

#include <QString>
#include <QGraphicsSvgItem>
#include <third_party/libavoid/shape.h>

#include <tuple>

#include <symbol/symbol.h>
#include <qnetlistgraphicsnode.h>

#include "component.h"
#include "port.h"

namespace OpenNetlistView::Yosys {

/**
 * @class Node
 * @brief Represents a node in the diagram viewer.
 *
 * The Node class inherits from the Component class and contains information about the node's type,
 * ports
 */
class Node : public Component
{
private:
    constexpr const static char* nodeColaName{"body"}; ///< The name of the nodes ID inside the map containing the IDs

public:
    /**
     * @brief Constructs a Node with the specified name, type, ports
     *
     * @param name The name of the node.
     * @param type The type of the node.
     * @param ports A vector of shared pointers to the ports of the node.
     */
    Node(QString name, QString type, std::vector<std::shared_ptr<Port>>& ports);

    /**
     * @brief Destructor for the Node class.
     */
    ~Node();

    /**
     * @brief Adds ports to the node.
     *
     * @param ports A vector of shared pointers to the ports to be added.
     */
    void addPorts(std::vector<std::shared_ptr<Port>>& ports);

    /**
     * @brief Set the Symbol object to be used by the node.
     *
     * @param symbol the Symbol object to be used by the node.
     */
    void setSymbol(const std::shared_ptr<Symbol::Symbol>& symbol);

    /**
     * @brief Gets the symbol used by the node.
     *
     * @return A shared pointer to the symbol used by the node.
     */
    std::shared_ptr<Symbol::Symbol> getSymbol() const;

    /**
     * @brief Set the Cola Rect IDs for the node
     *
     * Sets the IDs for assignment to the cola rectangles
     * for the node and its ports.
     *
     * @param colaRectIDs contains the cola rectangle IDs for the node and its ports
     * @throws std::runtime_error if a matching ID for a port of the node could not be found
     */
    void setColaRectIDs(const std::map<QString, int>& colaRectIDs);

    /**
     * @brief Get the ID of the rectangle from the cola layout.
     *
     * This is the ID of the rectangle that represents
     * the node in the cola layout.
     *
     * @return the ID of the rectangle
     */
    int getColaRectID() const;

    /**
     * @brief Set the Avoid Rect Reference for the node
     *
     * Sets the reference to the rectangle that represents the node
     * in the avoid layout.
     *
     * @param avoidRectReference the reference to the rectangle
     */
    void setAvoidRectReference(Avoid::ShapeRef* avoidRectReference);

    /**
     * @brief Get the Avoid Rect Reference for the node
     *
     * Gets the reference to the rectangle that represents the node
     * in the avoid layout.
     *
     * @return the reference to the rectangle
     */
    Avoid::ShapeRef* getAvoidRectReference();

    /**
     * @brief Gets the ports of the node.
     *
     * @return A vector of shared pointers to the ports of the node.
     */
    std::vector<std::shared_ptr<Port>>& getPorts();

    /**
     * @brief Gets the type of the node.
     *
     * @return The type of the node as a QString.
     */
    QString getType();

    /**
     * @brief Sets the type of the node.
     *
     * @param type The type of the node as a QString.
     */
    void setType(QString type);

    /**
     * @brief Calculates the positions of the bits of one of the
     * split or join ports within the ports of the split or join node.
     *
     *
     * @param labelPort The port to get the bit positions for.
     * @return A tuple containing the upper and lower bit positions.
     * If the port is not a split or join port, the tuple contains -1, -1.
     */
    std::tuple<int, int> getSplitJoinBitPositions(const std::shared_ptr<Port>& labelPort);

    /**
     * @brief Checks if the nodes ports have a connection.
     *
     * @return true if there is a connection, false otherwise
     */
    bool hasConnection() const;

    /**
     * @brief Converts the node to a QGraphicsSvgItem.
     *
     * @return A vector of QGraphicsSvgItem representing the node.
     */
    QNetlistGraphicsNode* convertToQt();

    /**
     * @brief clear the routing data from the node
     *
     * This resets the cola and avoid routing data
     */
    void clearRoutingData();

    /**
     * @brief Overloads the << operator to print the node's information.
     *
     * @param outputStream The output stream to print to.
     * @param node The node to be printed.
     * @return The output stream with the node's information.
     */
    friend std::ostream&
    operator<<(std::ostream& outputStream, const Node& node);

private:
    QString type;                             ///< The type of the node.
    std::vector<std::shared_ptr<Port>> ports; ///< The ports of the node.
    std::shared_ptr<Symbol::Symbol> symbol;   ///< The symbol that the node uses.
    int colaRectID;                           ///< The ID of the node's rectangle in the cola layout.
    Avoid::ShapeRef* avoidRectReference;      ///< The rectangle that represents the node in the avoid layout.
};

} // namespace OpenNetlistView::Yosys

#endif // __NODE_H__