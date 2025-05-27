#include <QString>
#include <QByteArray>
#include <QSvgRenderer>
#include <QDomDocument>
#include <third_party/libcola/cola.h>
#include <third_party/libcola/cluster.h>
#include <third_party/libcola/compound_constraints.h>
#include <third_party/libvpsc/rectangle.h>

#include <QTextStream>
#include <QDebug>

#include <algorithm>
#include <vector>
#include <map>
#include <memory>
#include <utility>
#include <ostream>
#include <sstream>
#include <cmath>

#include "symbol.h"
#include "port.h"
#include "symbol_parser.h"

namespace OpenNetlistView::Symbol {

Symbol::Symbol(QString name, double boundingBoxWidth, double boundingBoxHeight)
    : name(std::move(name))
    , boundingBoxWidth(boundingBoxWidth)
    , boundingBoxHeight(boundingBoxHeight)
    , qRenderer(nullptr)
{
    this->aliases = std::vector<std::shared_ptr<QString>>();
    this->ports = std::vector<std::shared_ptr<Port>>();
    this->svgData = QString();
}

Symbol::~Symbol() = default;

void Symbol::addAlias(const QString& alias)
{
    aliases.emplace_back(std::make_shared<QString>(alias));
}

void Symbol::setGeneric(bool isGeneric)
{
    this->isGeneric = isGeneric;
}

void Symbol::addPort(const std::shared_ptr<Port>& port)
{
    ports.push_back(port);
}

void Symbol::setPorts(const std::vector<std::shared_ptr<Port>>& ports)
{
    this->ports = ports;
}

const std::vector<std::shared_ptr<Port>>& Symbol::getPorts() const
{
    return ports;
}

void Symbol::addSvgData(QString svgData)
{
    this->svgData = std::move(svgData);
    this->createQRenderer();
}

QString Symbol::getSvgData()
{
    return this->svgData;
}

const QString& Symbol::getName() const
{
    return name;
}

bool Symbol::isSymbolType(const QString& type) const
{
    if(type == this->name)
    {
        return true;
    }

    if(std::any_of(this->aliases.begin(), this->aliases.end(), [&type](const std::shared_ptr<QString>& alias) {
           return type == *alias;
       }))
    {
        return true;
    }

    return false;
}

bool Symbol::isGenericSymbol() const
{
    return this->isGeneric;
}

bool Symbol::checkValid() const
{

    // clang-format off
    return  SymbolTypes::isValidSymbolType(this->name) &&
            (this->boundingBoxWidth >= 0.0F) &&
            (this->boundingBoxHeight >= 0.0F) &&
            !(this->ports.empty());
            !(this->svgData.isEmpty());
    // clang-format on
}

std::pair<double, double> Symbol::getBoundingBox() const
{
    return std::make_pair(this->boundingBoxWidth, this->boundingBoxHeight);
}

std::map<QString, int> Symbol::generateColaRep(std::vector<cola::Edge>& edges,
    cola::EdgeLengths& edgeLengths,
    std::vector<vpsc::Rectangle*>& rectangles,
    cola::CompoundConstraints& compoundConstraints,
    cola::RootCluster* rootCluster)
{

    // create the cluster for the symbol with padding and margin
    auto* cluster = new cola::RectangularCluster();
    cluster->setMargin(clusterMargin);
    cluster->setPadding(clusterPadding);

    vpsc::Rectangle* bodyRectangle = nullptr;
    std::map<QString, int> rectangleIDs;

    // Create the main rectangle
    bodyRectangle = new vpsc::Rectangle(0,
        this->boundingBoxWidth,
        0,
        this->boundingBoxHeight);
    rectangles.push_back(bodyRectangle);

    // create the entry in the rectangleIDs map and add it to the cluster
    const std::pair<QString, int> bodyID = std::make_pair(symbolIDName,
        rectangles.size() - 1);
    rectangleIDs.insert(bodyID);

    cluster->addChildNode(bodyID.second);

    cola::SeparationConstraint* sepConstraint = nullptr;

    for(const auto& port : this->ports)
    {
        const std::pair<QString, int> portID = port->generateColaRep(rectangles,
            bodyRectangle);

        rectangleIDs.insert(portID);

        // Connect the port to the body
        edges.emplace_back(bodyID.second, portID.second);

        edgeLengths.push_back(defaultEdgeLength);

        // get the rectangle of the port and add it to the cluster
        auto* portRect = rectangles.back();

        cluster->addChildNode(portID.second);

        // get the distance between the center of the port and the center of the body
        const double xSeparation = (portRect->getCentreX() - bodyRectangle->getCentreX());
        const double ySeparation = (portRect->getCentreY() - bodyRectangle->getCentreY());

        sepConstraint = new cola::SeparationConstraint(vpsc::Dim::XDIM,
            bodyID.second,
            portID.second,
            xSeparation,
            true);
        compoundConstraints.push_back(sepConstraint);

        sepConstraint = new cola::SeparationConstraint(vpsc::Dim::YDIM,
            bodyID.second,
            portID.second,
            ySeparation,
            true);
        compoundConstraints.push_back(sepConstraint);
    }

    rootCluster->addChildCluster(cluster);
    return rectangleIDs;
}

QSvgRenderer* Symbol::getQRenderer()
{
    return this->qRenderer;
}

std::shared_ptr<Symbol> Symbol::createJoinSplit(const int portCount, const std::shared_ptr<Symbol>& baseSymbol)
{

    const QString symbolType = baseSymbol->getName();

    std::shared_ptr<Symbol> newSymbol = nullptr;

    if(symbolType == "split")
    {
        newSymbol = createJoinSplitHelper(1, portCount, baseSymbol, false);
    }
    else if(symbolType == "join")
    {
        newSymbol = createJoinSplitHelper(portCount, 1, baseSymbol, true);
    }

    return newSymbol;
}

std::ostream& operator<<(std::ostream& outStream, const Symbol& symbol)
{

    std::stringstream sStream = std::stringstream();

    sStream << "Symbol: " << symbol.name.toStdString()
            << " (Width: " << symbol.boundingBoxWidth
            << ", Height: " << symbol.boundingBoxHeight << ")"
            << "\n";

    for(const auto& alias : symbol.aliases)
    {
        sStream << "\tAlias: " << alias->toStdString() << "\n";
    }

    for(const auto& port : symbol.ports)
    {
        sStream << "\t" << *port << "\n";
    }

    sStream << "\tSVG Data: " << symbol.svgData.toStdString() << "\n";

    return outStream << sStream.str();
}

void Symbol::createQRenderer()
{

    const QByteArray svgData = this->svgData.toUtf8();

    delete this->qRenderer;

    this->qRenderer = new QSvgRenderer(svgData);
}

std::shared_ptr<Symbol> Symbol::createJoinSplitHelper(const int inputPorts, const int outputPorts, const std::shared_ptr<Symbol>& baseSymbol, bool isJoin)
{
    const QString splitJoinName = baseSymbol->getName() + "_i" + QString::number(inputPorts) + "_o" + QString::number(outputPorts);
    const QString baseSvgData = baseSymbol->getSvgData();

    // convert to QDomDoc
    QDomDocument svgSymbol;
    svgSymbol.setContent(baseSvgData);

    // get the groupElement
    QDomElement groupElement = svgSymbol.documentElement().firstChildElement(SymbolSvg::groupTag);

    // get the width field
    const double width = groupElement.attribute(SymbolSvg::widthSAttr).toDouble();

    // get the elements with the s:pid attribute set to in0 or in1
    const QDomNodeList portElements = groupElement.elementsByTagName(SymbolSvg::groupTag);

    std::vector<double> yPositions;
    std::vector<QDomElement> toDelete;
    QDomElement mainElement;

    // gets the positions of the ports and deletes the opposite ports
    for(int i = 0; i < portElements.count(); i++)
    {
        const QDomElement portElement = portElements.at(i).toElement();

        if((isJoin && portElement.attribute(SymbolSvg::pidAttr).contains("in")) ||
            (!isJoin && portElement.attribute(SymbolSvg::pidAttr).contains("out")))
        {
            yPositions.push_back(portElement.attribute(SymbolSvg::yAttr).toDouble());
            toDelete.push_back(portElement);
        }
        else if((isJoin && portElement.attribute(SymbolSvg::pidAttr).contains("out")) ||
                (!isJoin && portElement.attribute(SymbolSvg::pidAttr).contains("in")))
        {
            mainElement = portElement;
        }
    }

    std::for_each(toDelete.begin(), toDelete.end(), [&groupElement](const QDomElement& element) {
        groupElement.removeChild(element);
    });

    // get the distance between the ports
    const double distance = std::abs(yPositions[1] - yPositions[0]);

    // calculate the new height
    const double height = distance * (isJoin ? inputPorts : outputPorts);

    // set the new height
    groupElement.setAttribute(SymbolSvg::heightSAttr, height);
    // set the rectangles height
    groupElement.firstChildElement("rect").setAttribute(SymbolSvg::heightAttr, height);
    // set the height of the svg tag
    svgSymbol.documentElement().setAttribute(SymbolSvg::heightAttr, height);

    std::vector<std::shared_ptr<Port>> symbolPorts;

    // create ports
    for(int i = 0; i < (isJoin ? inputPorts : outputPorts); i++)
    {
        // create a new subgroup
        QDomElement port = svgSymbol.createElement(SymbolSvg::groupTag);
        port.setAttribute(SymbolSvg::pidAttr, (isJoin ? "in" : "out") + QString::number(i));
        port.setAttribute(SymbolSvg::yAttr, QString::number((distance / 2) + (i * distance)));
        port.setAttribute(SymbolSvg::xAttr, isJoin ? "0" : QString::number(width));
        groupElement.appendChild(port);
        QString portName;
        double xPos = 0.0F;
        if(isJoin)
        {
            portName = "in" + QString::number(i);
            xPos = 0;
        }
        else
        {
            portName = "out" + QString::number(i);
            xPos = width;
        }
        symbolPorts.emplace_back(std::make_shared<Port>(portName, xPos, (distance / 2) + (i * distance)));
    }

    // set the main port to the middle of the symbol
    mainElement.setAttribute(SymbolSvg::yAttr, QString::number(height / 2));
    if(isJoin)
    {
        symbolPorts.emplace_back(std::make_shared<Port>("out", width, height / 2));
    }
    else
    {
        symbolPorts.emplace_back(std::make_shared<Port>("in", 0, height / 2));
    }

    // convert the QDomDoc back to string
    QString newSvgData;
    QTextStream newSvgDataStream(&newSvgData);

    svgSymbol.save(newSvgDataStream, QDomDocument::EncodingFromTextStream);

    // generate new Symbol
    auto newSymbol = std::make_shared<Symbol>(splitJoinName, width, height);
    newSymbol->setPorts(symbolPorts);
    newSymbol->addSvgData(newSvgData);

    return newSymbol;
}

std::shared_ptr<Symbol> Symbol::createGenericSymbol(const int inputCount, const int outputCount, const std::shared_ptr<Symbol>& baseSymbol)
{
    const QString splitJoinName = baseSymbol->getName() + "_i" + QString::number(inputCount) + "_o" + QString::number(outputCount);
    const QString baseSvgData = baseSymbol->getSvgData();

    // convert to QDomDoc
    QDomDocument svgSymbol;
    svgSymbol.setContent(baseSvgData);

    // get the groupElement
    QDomElement groupElement = svgSymbol.documentElement().firstChildElement(SymbolSvg::groupTag);

    // get the width field
    const double width = groupElement.attribute(SymbolSvg::widthSAttr).toDouble();

    // get the elements with the s:pid attribute set to in0 or in1 and out0 or out1
    const QDomNodeList portElements = groupElement.elementsByTagName(SymbolSvg::groupTag);

    std::vector<double> yPosIn;
    std::vector<double> yPosOut;
    std::vector<QDomElement> toDelete;

    // gets the positions of the ports and delete them
    for(int i = 0; i < portElements.count(); i++)
    {
        const QDomElement portElement = portElements.at(i).toElement();

        if(portElement.attribute(SymbolSvg::pidAttr).contains("in"))
        {
            yPosIn.push_back(portElement.attribute(SymbolSvg::yAttr).toDouble());
            toDelete.push_back(portElement);
        }
        else if(portElement.attribute(SymbolSvg::pidAttr).contains("out"))
        {
            yPosOut.push_back(portElement.attribute(SymbolSvg::yAttr).toDouble());
            toDelete.push_back(portElement);
        }
    }

    std::for_each(toDelete.begin(), toDelete.end(), [&groupElement](const QDomElement& element) {
        groupElement.removeChild(element);
    });

    // get the distance between the ports in and out

    const double distanceIn = std::abs(yPosIn[1] - yPosIn[0]);
    const double distanceOut = std::abs(yPosOut[1] - yPosOut[0]);

    // calculate the new height
    const double height = std::max(distanceIn * inputCount, distanceOut * outputCount);

    // set the new height
    groupElement.setAttribute(SymbolSvg::heightSAttr, height);

    // set the rectangles height
    groupElement.firstChildElement("rect").setAttribute(SymbolSvg::heightAttr, height);

    // set the height of the svg tag
    svgSymbol.documentElement().setAttribute(SymbolSvg::heightAttr, height);

    std::vector<std::shared_ptr<Port>> symbolPorts;

    // create ports

    for(int i = 0; i < inputCount; i++)
    {
        // create a new subgroup
        QDomElement port = svgSymbol.createElement(SymbolSvg::groupTag);
        port.setAttribute(SymbolSvg::pidAttr, "in" + QString::number(i));
        port.setAttribute(SymbolSvg::yAttr, QString::number((distanceIn / 2) + (i * distanceIn)));
        port.setAttribute(SymbolSvg::xAttr, "0");
        groupElement.appendChild(port);
        symbolPorts.emplace_back(std::make_shared<Port>("in" + QString::number(i), 0, (distanceIn / 2) + (i * distanceIn)));
    }

    for(int i = 0; i < outputCount; i++)
    {
        // create a new subgroup
        QDomElement port = svgSymbol.createElement(SymbolSvg::groupTag);
        port.setAttribute(SymbolSvg::pidAttr, "out" + QString::number(i));
        port.setAttribute(SymbolSvg::yAttr, QString::number((distanceOut / 2) + (i * distanceOut)));
        port.setAttribute(SymbolSvg::xAttr, QString::number(width));
        groupElement.appendChild(port);
        symbolPorts.emplace_back(std::make_shared<Port>("out" + QString::number(i), width, (distanceOut / 2) + (i * distanceOut)));
    }

    // convert the QDomDoc back to string
    QString newSvgData;
    QTextStream newSvgDataStream(&newSvgData);

    svgSymbol.save(newSvgDataStream, QDomDocument::EncodingFromTextStream);

    // generate new Symbol

    auto newSymbol = std::make_shared<Symbol>(splitJoinName, width, height);

    newSymbol->setPorts(symbolPorts);

    newSymbol->addSvgData(newSvgData);

    return newSymbol;
}

} // namespace OpenNetlistView::Symbol