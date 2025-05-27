#include <QDomDocument>
#include <QByteArray>
#include <QFile>
#include <QString>
#include <QDomElement>
#include <QDomNodeList>
#include <QDomNode>
#include <QDebug>
#include <QTextStream>

#include <memory>
#include <map>
#include <vector>
#include <stdexcept>

#include "symbol_parser.h"
#include "port.h"
#include "symbol.h"

namespace OpenNetlistView::Symbol {

SymbolParser::SymbolParser()
{
    this->rootElement = QDomElement();
}

SymbolParser::~SymbolParser() = default;

void SymbolParser::setRootElement(const QDomElement& rootElement)
{
    this->rootElement = rootElement;

    // clear the symbols
    this->symbols.clear();

    extractSvgMetadata();
}

void SymbolParser::parse()
{

    // get all tag on the root level
    const QDomNodeList nodeList = this->rootElement.childNodes();

    std::vector<QDomElement> symbolElements;

    // extract all group tags, those contain the symbol information
    for(int i = 0; i < nodeList.count(); i++)
    {

        const QDomNode node = nodeList.at(i);

        if(node.isElement() && node.toElement().tagName() == SymbolSvg::groupTag)
        {
            const QDomElement element = node.toElement();
            symbolElements.push_back(element);
        }
    }

    // iterate over all group tags  and create a symbol for each one
    for(auto& symbolElement : symbolElements)
    {

        const QString symbolName = symbolElement.attribute(SymbolSvg::typeAttr);
        const double symbolWidth = symbolElement.attribute(SymbolSvg::widthSAttr).toDouble();
        const double symbolHeight = symbolElement.attribute(SymbolSvg::heightSAttr).toDouble();

        auto symbol = std::make_shared<Symbol>(symbolName, symbolWidth, symbolHeight);

        // add the symbol to the map
        this->symbols[symbolName] = symbol;

        QString svgData;
        QTextStream svgDataStream(&svgData);

        const QDomDocument svgDocument = this->svgMetadata.cloneNode().toDocument();

        // set the attributes width and height to the symbol width and height
        svgDocument.documentElement().setAttribute(SymbolSvg::widthAttr, symbolWidth);
        svgDocument.documentElement().setAttribute(SymbolSvg::heightAttr, symbolHeight);

        // set the transform attribute to translate(0,0)
        symbolElement.setAttribute(SymbolSvg::transformAttr, SymbolSvg::translateValue);

        // add svg data to the document
        svgDocument.documentElement().appendChild(symbolElement.cloneNode());

        // convert to string
        svgDocument.save(svgDataStream, QDomNode::EncodingFromTextStream);

        // export the svg data
        symbol->addSvgData(svgData);

        // get the aliases to the symbols name
        const QDomNodeList symbolAliases = symbolElement.elementsByTagName(SymbolSvg::aliasTag);

        for(int j = 0; j < symbolAliases.count(); j++)
        {
            const QDomNode alias = symbolAliases.at(j);

            if(alias.isElement() && alias.toElement().tagName() == SymbolSvg::aliasTag)
            {
                const QString aliasName = alias.toElement().attribute(SymbolSvg::valAttr);
                symbol->addAlias(aliasName);
                this->symbols[aliasName] = symbol;
            }
        }

        // get the groups of the symbol, those contain the ports of the symbol
        const QDomNodeList symbolPorts = symbolElement.elementsByTagName(SymbolSvg::groupTag);

        for(int j = 0; j < symbolPorts.count(); j++)
        {
            const QDomNode port = symbolPorts.at(j);

            if(port.isElement() && port.toElement().tagName() == SymbolSvg::groupTag)
            {

                const QDomElement portElement = port.toElement();

                const QString portName = portElement.attribute(SymbolSvg::pidAttr);
                const double portX = portElement.attribute(SymbolSvg::xAttr).toDouble();
                const double portY = portElement.attribute(SymbolSvg::yAttr).toDouble();

                auto port = std::make_shared<Port>(portName, portX, portY);

                symbol->addPort(port);
            }
        }
    }

    this->checkSymbols();
}

const std::map<QString, std::shared_ptr<Symbol>>& SymbolParser::getSymbols() const
{
    return this->symbols;
}

void SymbolParser::extractSvgMetadata()
{
    QDomDocument svgDocument;
    QDomElement svgElement = svgDocument.createElement(SymbolSvg::svgTag);

    svgDocument.appendChild(svgElement);

    // get the data out of the existing svg file

    // copy the attributes of the svg tag
    const QDomNamedNodeMap attributes = this->rootElement.attributes();
    for(int i = 0; i < attributes.count(); i++)
    {
        const QDomNode attribute = attributes.item(i);
        svgElement.setAttribute(attribute.nodeName(), attribute.nodeValue());
    }

    // get all tags except the g tags
    const auto settingsNodes = this->rootElement.childNodes();

    // add it as children of the svg tag
    for(int i = 0; i < settingsNodes.count(); i++)
    {
        const QDomNode node = settingsNodes.at(i);

        if(node.isElement() && node.toElement().tagName() != SymbolSvg::groupTag)
        {
            svgElement.appendChild(node.cloneNode());
        }
    }

    this->svgMetadata = svgDocument;
}

void SymbolParser::checkSymbols()
{

    // check if all defaultTypes are present but allow additional symbols

    for(const auto& defaultSymbol : SymbolTypes::defaultSymbols)
    {
        if(this->symbols.find(defaultSymbol) == this->symbols.end())
        {
            throw std::runtime_error("SymbolParser: Missing default type: " + defaultSymbol.toStdString());
        }
    }
}

} // namespace OpenNetlistView::Symbol