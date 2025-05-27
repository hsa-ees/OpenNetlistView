/**
 * @file symbol_parser.h
 * @brief This file contains the definition of the SymbolParser class for parsing SVG files
 * with symbols and definitions used for parsing
 */
#ifndef __SYMBOL_PARSER_H__
#define __SYMBOL_PARSER_H__

#include <QString>
#include <QDomElement>
#include <map>
#include <memory>

#include "symbol.h"

/**
 * @namespace SymbolSvg
 * @brief Contains constant definitions for SVG tag and attribute names used in symbol parsing.
 */
namespace SymbolSvg {

constexpr const char* svgTag{"svg"};       ///< SVG tag name
constexpr const char* groupTag{"g"};       ///< Group tag name
constexpr const char* aliasTag{"s:alias"}; ///< Alias tag name

constexpr const char* widthSAttr{"s:width"};            ///< Width attribute name for groups
constexpr const char* heightSAttr{"s:height"};          ///< Height attribute name for groups
constexpr const char* widthAttr{"width"};               ///< Width attribute name for SVG
constexpr const char* heightAttr{"height"};             ///< Height attribute name for SVG
constexpr const char* typeAttr{"s:type"};               ///< Type attribute name
constexpr const char* valAttr{"val"};                   ///< Value attribute name
constexpr const char* xAttr{"s:x"};                     ///< X coordinate attribute name
constexpr const char* yAttr{"s:y"};                     ///< Y coordinate attribute name
constexpr const char* pidAttr{"s:pid"};                 ///< PID attribute name
constexpr const char* transformAttr{"transform"};       ///< Transform attribute name
constexpr const char* translateValue{"translate(0,0)"}; ///< Translate value for transform attribute

constexpr const int symbolCount{27}; ///< Number of symbols in the SVG file

} // namespace SymbolSvg

namespace OpenNetlistView::Symbol {

/**
 * @class SymbolParser
 * @brief Parses symbols from a given XML document.
 */
class SymbolParser
{

public:
    /**
     * @brief Constructor for SymbolParser.
     */
    SymbolParser();

    /**
     * @brief Destructor for SymbolParser.
     */
    ~SymbolParser();

    /**
     * @brief Sets the root element of the SVG document to be parsed.
     * @param rootElement The root element of the SVG document.
     */
    void setRootElement(const QDomElement& rootElement);

    /**
     * @brief Parses the SVG document to extract symbols.
     *
     * @throw std::runtime_error If data is missing from a symbol
     */
    void parse();

    /**
     * @brief Returns the list of parsed symbols.
     * @return A list of parsed symbols.
     */
    const std::map<QString, std::shared_ptr<Symbol>>& getSymbols() const;

private:
    QDomElement rootElement;                            ///< The root element of the SVG document.
    std::map<QString, std::shared_ptr<Symbol>> symbols; ///< A list of parsed symbols.
    QDomDocument svgMetadata;                           ///< Metadata extracted from the SVG.

    /**
     * @brief Extracts metadata from the SVG.
     */
    void extractSvgMetadata();

    /**
     * @brief Checks if all symbols have been parsed.
     *
     * Checks if all symbols have been parsed and if they have ports,
     * width and height
     *
     * @throw std::runtime_error If data is missing from a symbol
     */
    void checkSymbols();
};

} // namespace OpenNetlistView::Symbol

#endif // __SYMBOL_PARSER_H__