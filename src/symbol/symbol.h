/**
 * @file symbol.h
 * @brief Declaration of the Symbol class used in the OpenNetlistView::Symbol namespace.
 *
 * This file contains the declaration of the Symbol class, which represents a symbol
 * with a name, bounding box dimensions, aliases, ports, and SVG data that represents
 * the symbol
 *
 * @author Lukas Bauer
 */

#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <QString>
#include <QSvgRenderer>
#include <third_party/libcola/cola.h>
#include <third_party/libvpsc/rectangle.h>
#include <third_party/libcola/connected_components.h>

#include <vector>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <unordered_set>

#include "port.h"

/**
 * @namespace SymbolTypes
 * @brief Contains all valid symbol types and a way to check
 * if a given symbol type is valid.
 *
 */
namespace SymbolTypes {

// clang-format off
const std::unordered_set<QString> defaultSymbols = {
    "mux", "mux-bus", "tribuf", "and", "nand", "or", "not",
    "add", "sub", "eq", "dff", "dffn", "lt", "le", "ge",
    "gt", "inputExt", "constant", "outputExt", "split",
    "join", "generic", }; ///< All valid symbol types
// clang-format on

/**
 * @brief checks if a given name is a valid symbol type
 *
 * @param type the type to check
 * @return true if the type is valid, false otherwise
 */
inline bool isValidSymbolType(const QString& type)
{
    // remove a $ infront of the type
    QString typeWithoutDollar = type;

    if(typeWithoutDollar.startsWith("$"))
    {
        typeWithoutDollar = typeWithoutDollar.remove(0, 1);
    }

    return defaultSymbols.find(typeWithoutDollar) != defaultSymbols.end();
}

} // namespace SymbolTypes
namespace OpenNetlistView::Symbol {

/**
 * @class Symbol
 * @brief Represents a symbol with a name, bounding box dimensions, aliases, ports, and SVG data.
 */
class Symbol
{

private:
    constexpr const static double clusterMargin{0.0F};     ///< The margin of the cluster.
    constexpr const static double clusterPadding{20.0F};   ///< The padding of the cluster.
    constexpr const static double defaultEdgeLength{0.1F}; ///< The default edge length.
    constexpr const static char* symbolIDName{"body"};     ///< The name of the symbol ID.

public:
    /**
     * @brief Constructs a Symbol with the given name and bounding box dimensions.
     *
     * @param name The name of the symbol.
     * @param boundingBoxWidth The width of the bounding box.
     * @param boundingBoxHeight The height of the bounding box.
     */
    Symbol(QString name, double boundingBoxWidth, double boundingBoxHeight);

    /**
     * @brief Destructor for the Symbol class.
     */
    ~Symbol();

    /**
     * @brief Adds an alias to the symbol.
     *
     * @param alias The alias to add.
     */
    void addAlias(const QString& alias);

    /**
     * @brief Set the symbol to be a generic symbol or not
     *
     * @param isGeneric indicates if the symbol is a generic symbol
     */
    void setGeneric(bool isGeneric);

    /**
     * @brief Adds a port to the symbol.
     *
     * @param port The port to add.
     */
    void addPort(const std::shared_ptr<Port>& port);

    /**
     * @brief Sets the ports of the symbol.
     *
     * @param ports The ports to set.
     */
    void setPorts(const std::vector<std::shared_ptr<Port>>& ports);

    /**
     * @brief Get the ports of the symbol.
     *
     * @return The ports of the symbol.
     */
    const std::vector<std::shared_ptr<Port>>& getPorts() const;

    /**
     * @brief Adds SVG data to the symbol.
     *
     * @param svgData The SVG data to add.
     */
    void addSvgData(QString svgData);

    /**
     * @brief Get the Svg Data of the symbol
     *
     * @return The SVG data of the symbol
     */
    QString getSvgData();

    /**
     * @brief Get the name of the symbol.
     *
     * @return name of the symbol
     */
    const QString& getName() const;

    /**
     * @brief checks the if given string matches the symbol type
     *
     * for this it checks the name and aliases of the symbol
     * against the given type is it matches it is a symbol of this type
     *
     * @param type the type to check the symbol against
     * @return true if the type matches, false otherwise
     */
    bool isSymbolType(const QString& type) const;

    /**
     * @brief checks if the symbol is a generic symbol
     *
     * @return true if the symbol is a generic symbol, false otherwise
     */
    bool isGenericSymbol() const;

    /**
     * @brief checks if the symbol is valid
     *
     * A symbol is valid if it has a valid type,
     * a width and height that is bigger than 0,
     * at least one port and contains SVG data
     *
     * @return true if the symbol is valid, false otherwise
     */
    bool checkValid() const;

    /**
     * @brief Get the bounding box of the symbol.
     *
     * @return The bounding box of the symbol.
     */
    std::pair<double, double> getBoundingBox() const;

    /**
     * @brief Generates the cola representation of the symbol.
     *
     * Generates the cola representation of the symbol with the given edges, edge lengths,
     * rectangles, compound constraints, and root cluster.
     *
     * @param edges The edges of the symbol.
     * @param edgeLengths The edge lengths of the symbol.
     * @param rectangles The rectangles of the symbol.
     * @param compoundConstraints The compound constraints of the symbol.
     * @param rootCluster The root cluster of the symbol.
     * @return The rectangle IDs of the symbol.
     */
    std::map<QString, int> generateColaRep(std::vector<cola::Edge>& edges,
        cola::EdgeLengths& edgeLengths,
        std::vector<vpsc::Rectangle*>& rectangles,
        cola::CompoundConstraints& compoundConstraints,
        cola::RootCluster* rootCluster);

    /**
     * @brief Get the SVG renderer for the symbol.
     *
     * @return The SVG renderer for the symbol.
     */
    QSvgRenderer* getQRenderer();

    /**
     * @brief Overloads the output stream operator for the Symbol class.
     *
     * @param outStream The output stream.
     * @param symbol The symbol to output.
     * @return The output stream.
     */
    friend std::ostream& operator<<(std::ostream& outStream, const Symbol& symbol);

    /**
     * @brief Creates a split or join symbol with the given port count and base symbol.
     *
     * The split or join symbol is created based on the port count and base symbol.
     * If it is a split symbol then the number of ports represents the output ports.
     * If it is a join symbol then the number of ports represents the input ports.
     *
     * @param portCount The number of ports for the split or join symbol.
     * @param baseSymbol The base symbol either split or join.
     * @return The split or join symbol created or null if it is not a spit or join symbol.
     *
     */
    static std::shared_ptr<Symbol> createJoinSplit(const int portCount, const std::shared_ptr<Symbol>& baseSymbol);

    /**
     * @brief Creates a generic symbol for the given node.
     *
     * The generic symbol is created based on the generic symbol parsed from the svg file
     *
     * @param inputCount The number of input ports for the generic symbol.
     * @param outputCount The number of output ports for the generic symbol.
     * @param baseSymbol The base symbol for the generic symbol.
     * @return The generic symbol created.
     */
    static std::shared_ptr<Symbol> createGenericSymbol(const int inputCount, const int outputCount, const std::shared_ptr<Symbol>& baseSymbol);

private:
    /**
     * @brief Creates the SVG renderer for the symbol.
     *
     * For this the SVG data of the symbol is used
     */
    void createQRenderer();

    /**
     * @brief Creates a split or join symbol with the given input and output ports and base symbol.
     *
     * The split or join symbol is created based on the input and output ports and base symbol.
     *
     * @param inputPorts The number of input ports for the join symbol.
     * @param outputPorts The number of output ports for the split symbol.
     * @param baseSymbol The base symbol for the split or join symbol.
     * @param isJoin True if it is a join symbol, false if it is a split symbol.
     * @return The split or join symbol created.
     */
    static std::shared_ptr<Symbol> createJoinSplitHelper(const int inputPorts, const int outputPorts, const std::shared_ptr<Symbol>& baseSymbol, bool isJoin);

    QString name;                                  ///< The name of the symbol.
    std::vector<std::shared_ptr<QString>> aliases; ///< The aliases of the symbol.
    std::vector<std::shared_ptr<Port>> ports;      ///< The ports of the symbol.
    double boundingBoxWidth;                       ///< The width of the bounding box.
    double boundingBoxHeight;                      ///< The height of the bounding box.
    QString svgData;                               ///< The SVG data of the symbol.
    QSvgRenderer* qRenderer;                       ///< The SVG renderer for the symbol.
    bool isGeneric = false;                        ///< True if the symbol is a generic symbol, false otherwise.
};

} // namespace OpenNetlistView::Symbol

#endif // __SYMBOL_H__