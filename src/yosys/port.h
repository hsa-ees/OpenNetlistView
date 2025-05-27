/**
 * @file port.h
 * @brief Header file for the Port class in the OpenNetlistView namespace.
 *
 * This file contains the definition of the Port class, which represents a port in the OpenNetlistView application.
 * The Port class inherits from the Component class and provides functionalities to manage port properties.
 *
 * @author Lukas Bauer
 */

#ifndef __PORT_H__
#define __PORT_H__

#include <third_party/libavoid/geomtypes.h>
#include <QGraphicsSvgItem>
#include <QStringList>
#include <QVariantList>

#include <symbol/symbol.h>
#include <qnetlistgraphicsnode.h>
#include <cstdint>

#include "component.h"
#include "path.h"

namespace OpenNetlistView::Yosys {

// forward declaration
class Path;
class Node;

/**
 * @class Port
 * @brief Represents a port in the OpenNetlistView application.
 *
 * The Port class provides functionalities to manage port properties such as direction, bits, and constant status.
 */
class Port : public Component
{

private:
    constexpr const static char* bodyColaName{"body"}; ///< The name of the ports ID inside the map containing the IDs

public:
    /**
     * @enum EDirection
     * @brief Enumeration for port direction.
     *
     * This enumeration defines the possible directions for a port: INPUT, OUTPUT, and BIDIR.
     */
    typedef enum
    {
        INPUT = 0, ///< Input direction
        OUTPUT,    ///< Output direction
        CONST      ///< Constant Value Port
    } EDirection;

    /**
     * @brief Constructor for the Port class.
     *
     * Initializes a Port object with the given name, direction, bits, and path.
     *
     * @param name The name of the port.
     * @param direction The direction of the port.
     * @param bits A list containing the bits of the port.
     * @param path The path the port is connected to.
     */
    Port(QString name, Port::EDirection direction, QStringList bits, std::shared_ptr<Path> path);

    /**
     * @brief Constructor for the Port class.
     *
     * Initializes a Port object with the given name, direction, bits, and constant status.
     *
     * @param name The name of the port.
     * @param direction The direction of the port.
     * @param bits A list containing the bits of the port.
     */
    Port(QString name, Port::EDirection direction, QStringList bits);

    /**
     * @brief Destructor for the Port class.
     *
     * Cleans up resources used by the Port object.
     */
    ~Port();

    /**
     * @brief Set the Symbol object to be used by the port.
     *
     * @param symbol the Symbol object to be used by the port.
     */
    void setSymbol(const std::shared_ptr<Symbol::Symbol>& symbol);

    /**
     * @brief Get the Symbol object
     *
     * @return the symbol object used by the port
     */
    std::shared_ptr<Symbol::Symbol> getSymbol() const;

    /**
     * @brief Set the Path object to be used by the port.
     *
     * @param path the Path object to be used by the port.
     */
    void setPath(std::shared_ptr<Path> path);

    /**
     * @brief Get the Path object
     *
     * @return the path object used by the port
     */
    std::shared_ptr<Path> getPath() const;

    /**
     * @brief Set the alias for the symbol name
     *
     * @param alias the alias for the symbol name
     */
    void setSymbolNameAlias(const QString& alias);

    /**
     * @brief Get the alias for the symbol name
     *
     * @return the alias for the symbol name
     */
    QString getSymbolNameAlias() const;

    /**
     * @brief Set the Ports cola rectangle IDs.
     *
     * The IDs needed for the Ports cola rectangles.
     *
     * @param colaPortRectIDs
     */
    void setPortColaRectIDs(const std::map<QString, int> colaPortRectIDs);

    /**
     * @brief Get the Ports cola rectangle IDs.
     *
     * The IDs needed for the Ports cola rectangles.
     *
     * @return the IDs needed for the Ports cola rectangles
     */
    std::map<QString, int> getPortColaRectIDs() const;

    /**
     * @brief Get the ID of the rectangle from the cola layout.
     *
     * This is the ID of the rectangle that represents
     * the port in the cola layout.
     *
     * @param bodyOrPort if true the body rectangle is returned else the port rectangle
     * @return the ID of the rectangle if -1 it is invalid
     */
    int getPortConRectID(bool bodyOrPort = false) const;

    /**
     * @brief Sets the reference to the rectangle in the avoid layout.
     *
     * @param avoidRect the reference to the rectangle in the avoid layout.
     */
    void setAvoidRectReference(Avoid::ShapeRef* avoidRect);

    /**
     * @brief Gets the reference to the rectangle in the avoid layout.
     *
     * @return the reference to the rectangle in the avoid layout.
     */
    Avoid::ShapeRef* getAvoidRectReference();

    /**
     * @brief Gets the width of the port.
     *
     * Returns the width of the port, which is the number of bits.
     *
     * @return The width of the port.
     */
    uint64_t getWidth();

    /**
     * @brief Get the constant value of the port.
     *
     * @return the constant value of the port as an integer
     * @return if the port is not a constant port -1
     * @return if the port is a constant port with more than one bit -1
     */
    uint64_t getConstPortValue() const;

    /**
     * @brief Set the constant value of the port.
     *
     * @param bits the bits of the constant value
     */
    void setConstPortValue(QStringList bits);

    /**
     * @brief Set the constant value of the port.
     *
     * @param value the constant value
     */
    void setConstPortValue(uint64_t value);

    /**
     * @brief Gets the direction of the port.
     *
     * Returns the direction of the port as an EDirection value.
     *
     * @return The direction of the port.
     */
    Port::EDirection getDirection() const;

    /**
     * @brief checks if the port has a connection to a port.
     *
     * @return true if there is a connection, false otherwise
     */
    bool hasConnection() const;

    /**
     * @brief checks if the port has bits with constant values
     *
     * @return true if the port contains bits with constant values, false otherwise
     */
    bool hasConstantBits() const;

    /**
     * @brief checks if the path has no connection bits
     *
     * @return true if the path has no connection bits, false otherwise
     */
    bool hasNoConnectBitsConnection() const;

    /**
     * @brief Gets the bits of the port.
     *
     * Returns a reference to a vector of integers representing the bits of the port.
     *
     * @return A reference to the vector of integers representing the bits of the port.
     */
    QStringList getBits();

    /**
     * @brief Gets the bit number of the port.
     *
     * @return The number of bits of the port.
     */
    uint64_t getMaxBitNumber() const;

    /**
     * @brief replaces bits a given starting position with new bits
     *
     * @param pos the starting position of the bits to replace
     * @param bits the new bits to replace the old bits
     */
    void replaceBits(std::tuple<uint64_t, uint64_t> pos, QStringList bits);

    /**
     * @brief Sets the parent node of the port.
     *
     * Sets the parent node of the port to the given node.
     *
     * @param node The node to set as the parent node.
     */
    void setParentNode(std::shared_ptr<Node> node);

    /**
     * @brief Gets the parent node of the port.
     *
     * Returns the parent node of the port.
     *
     * @return The parent node of the port.
     */
    std::shared_ptr<Node> getParentNode();

    /**
     * @brief Converts the port to a QGraphicsSvgItem.
     *
     * Converts the port to a QGraphicsSvgItem for display in the Qt application.
     *
     * @return The QGraphicsSvgItem representing the port.
     */
    QNetlistGraphicsNode* convertToQt();

    /**
     * @brief remove the routing data from the port
     *
     * This resets the cola and avoid routing data
     */
    void clearRoutingData();

    /**
     * @brief Overloaded stream insertion operator for the Port class.
     *
     * Allows the Port object to be printed to an output stream.
     *
     * @param outputStream The output stream to print to.
     * @param port The Port object to print.
     * @return The output stream with the Port object printed.
     */
    friend std::ostream& operator<<(std::ostream& outputStream, const Port& port);

private:
    Port::EDirection direction;             ///< The direction of the port.
    QStringList bits;                       ///< A vector containing the bits of the port.
    std::shared_ptr<Path> path;             ///< The path the port is connected to.
    std::shared_ptr<Symbol::Symbol> symbol; ///< The symbol the the port uses.
    std::map<QString, int> colaPortIDs;     ///< The IDs needed for Ports cola rectangles
    Avoid::ShapeRef* avoidRectReference;    ///< The reference to the rectangle in the avoid layout.
    std::shared_ptr<Node> parentNode;       ///< The node the port is part of.
    QString symbolNameAlias = "";           ///< The alias for the port name that can be used for the svg symbol
    uint64_t constValue;                    ///< The constant value of the port
};

} // namespace OpenNetlistView::Yosys

#endif // __PORT_H__