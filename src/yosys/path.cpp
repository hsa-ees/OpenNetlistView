#include <QString>
#include <QStringList>
#include <QList>
#include <QGraphicsPathItem>
#include <QPen>
#include <QFont>
#include <QtCore/Qt>
#include <QSet>
#include <QPainterPath>
#include <QVariantList>
#include <qmetatype.h>

#include <libavoid/geomtypes.h>
#include <third_party/libavoid/connector.h>
#include <third_party/libavoid/geomtypes.h>

#include <memory>
#include <vector>
#include <utility>
#include <ostream>
#include <sstream>
#include <algorithm>
#include <tuple>
#include <cstdint>

#include <qnetlistgraphicspath.h>

#include "port.h"
#include "component.h"
#include "node.h"
#include "path.h"

namespace OpenNetlistView::Yosys {

Path::Path(QString name, const uint64_t width, QStringList bits, std::shared_ptr<Port>& sigSource, std::vector<std::shared_ptr<Port>>& sigDestinations, bool hiddenName)
    : Component(std::move(name))
    , width(width)
    , bits(std::move(bits))
    , hiddenName(hiddenName)
    , sigSource(sigSource)
{
    this->sigDestinations = std::make_shared<std::vector<std::shared_ptr<Port>>>(sigDestinations);
    this->alternativeNames = std::vector<std::shared_ptr<QString>>();
}

Path::Path(QString name, QStringList bits, bool hiddenName)
    : Component(std::move(name))
    , bits(std::move(bits))
    , hiddenName(hiddenName)
{
    this->width = this->bits.size();
    this->sigSource = nullptr;
    this->sigDestinations = std::make_shared<std::vector<std::shared_ptr<Port>>>();
    this->alternativeNames = std::vector<std::shared_ptr<QString>>();
}

Path::~Path() = default;

void Path::setSigSource(const std::shared_ptr<Port>& sigSource, bool allowOverwrite)
{
    if(this->sigSource != nullptr && !allowOverwrite)
    {
        return;
    }

    this->sigSource = sigSource;
}

void Path::addSigDestination(const std::shared_ptr<Port>& sigDestination)
{
    this->sigDestinations->emplace_back(sigDestination);
}

void Path::addAvoidPortRelation(Avoid::ConnRef* avoidConnRef, const int colaDestID)
{

    // find the correct port to to link the avoidConnRef to
    for(const auto& port : *this->sigDestinations)
    {
        if(port->getPortConRectID() == colaDestID)
        {
            this->avoidPortRefs.emplace(avoidConnRef, port);
        }
    }
}

void Path::setWidth(uint64_t width)
{
    this->width = width;
}

QStringList& Path::getBits()
{
    return bits;
}

bool Path::isBus() const
{
    return (this->width > 1);
}

bool Path::isNameHidden() const
{
    return hiddenName;
}

std::shared_ptr<Port> Path::getSigSource()
{
    return sigSource;
}

std::shared_ptr<std::vector<std::shared_ptr<Port>>> Path::getSigDestinations()
{
    return sigDestinations;
}

void Path::addAvoidConnRef(Avoid::ConnRef* avoidConnRef)
{
    this->avoidConnRefs.emplace_back(avoidConnRef);
}

void Path::setAvoidConnRefs(std::vector<Avoid::ConnRef*> avoidConnRefs)
{
    this->avoidConnRefs = std::move(avoidConnRefs);
}

std::vector<Avoid::ConnRef*> Path::getAvoidConnRefs()
{
    return this->avoidConnRefs;
}

bool Path::hasConnection() const
{
    // has a connection if the sigSource is present and the sigDestinations are not empty
    return (((this->sigSource != nullptr) && !(this->sigDestinations->empty())) || this->hasNoConnectBitsConnection());
}

bool Path::hasConstBits() const
{
    return std::any_of(this->bits.begin(), this->bits.end(), [](const QString& bit) {
        return bit == "0" || bit == "1";
    });
}

bool Path::hasNoConnectBitsConnection() const
{
    return std::any_of(this->bits.begin(), this->bits.end(), [](const QString& bit) {
        return bit == "x";
    });
}

void Path::addAlternativeName(const QString& name)
{
    this->alternativeNames.emplace_back(std::make_shared<QString>(name));
}

std::vector<std::shared_ptr<QString>>& Path::getAlternativeNames()
{
    return this->alternativeNames;
}

bool Path::partialBitsMatch(const QStringList& bits) const
{

    // if the iterator is not the end of the vector it has found the bits
    auto iter = std::search(this->bits.begin(), this->bits.end(), bits.begin(), bits.end());

    return iter != this->bits.end();
}

QNetlistGraphicsPath* Path::convertToQt()
{

    auto* qPathItem = new QNetlistGraphicsPath();

    if(avoidConnRefs.empty())
    {
        return qPathItem;
    }

    QPainterPath completePainterPath;

    for(auto* avoidConnRef : this->avoidConnRefs)
    {

        // create the path object if it is the first nothing additional needs to be done
        //  else we need to find the point where the paths diverge
        const QPainterPath qSubPainterPath = createPainterPath(*avoidConnRef, qPathItem);

        if(avoidConnRef == avoidConnRefs.front())
        {
            completePainterPath = qSubPainterPath;
        }
        else
        {

            const QPointF endOfIntersection = findEndOfIntersection(completePainterPath,
                qSubPainterPath);

            qPathItem->addDivergingPoint(endOfIntersection);

            completePainterPath.addPath(qSubPainterPath);
        }
    }

    qPathItem->setPath(completePainterPath);

    // set the pen strength
    double lineSize = lineStrength;

    if(this->isBus())
    {
        lineSize = busLineStrength;
    }

    qPathItem->setPen(QPen(Qt::black,
        lineSize,
        Qt::SolidLine,
        Qt::SquareCap,
        Qt::RoundJoin));

    // set this nodes qtitem to the one created
    this->setGraphicsItem(qPathItem);

    return qPathItem;
}

void Path::clearRoutingData()
{
    this->avoidConnRefs.clear();
}

QString Path::generateLabelText(Avoid::ConnRef* avoidRef) const
{

    std::tuple<int, int> portRange = std::make_tuple(-1, -1);

    // check if the source of the path is a split or the destination is a join
    // if it is get the bit range of the split or join
    // it uses the avoidRef to identify the destination that needs to be checked
    if(avoidRef == nullptr && this->sigSource != nullptr &&
        this->sigSource->getParentNode() != nullptr &&
        this->sigSource->getParentNode()->getType() == "split")
    {
        portRange = this->sigSource->getParentNode()->getSplitJoinBitPositions(this->sigSource);
    }
    else if(avoidRef != nullptr)
    {
        auto port = this->avoidPortRefs.find(avoidRef);

        if(port != this->avoidPortRefs.end() &&
            port->second->getParentNode() != nullptr &&
            port->second->getParentNode()->getType() == "join")
        {
            portRange = port->second->getParentNode()->getSplitJoinBitPositions(port->second);
        }
    }

    // create the port if a range is found
    if(portRange != std::make_tuple(-1, -1))
    {
        return "<" + QString::number(std::get<0>(portRange)) +
               ":" + QString::number(std::get<1>(portRange)) + ">";
    }

    // check if it is a real generic symbol that is not just
    // a unknown normal symbol if it is don't print a name
    const QRegularExpression regularExpr("[/\\\\.$]");
    if(avoidRef == nullptr && this->sigSource != nullptr && this->sigSource->getParentNode() != nullptr &&
        this->sigSource->getParentNode()->getSymbol() != nullptr && this->sigSource->getParentNode()->getSymbol()->isGenericSymbol() &&
        !this->sigSource->getParentNode()->getName().contains(regularExpr))
    {
        return "";
    }
    else if(avoidRef != nullptr)
    {
        auto port = this->avoidPortRefs.find(avoidRef);

        if(port != this->avoidPortRefs.end() && port->second->getParentNode() != nullptr &&
            port->second->getParentNode()->getSymbol() != nullptr && port->second->getParentNode()->getSymbol()->isGenericSymbol() &&
            !port->second->getParentNode()->getName().contains(regularExpr))
        {
            return "";
        }
    }

    // if the name is hidden return an empty string
    if(this->hiddenName)
    {
        return "";
    }

    QString pathName = this->getName();

    // remove everything before a // and after [

    auto index = pathName.indexOf("\\");
    if(index != -1)
    {
        pathName = pathName.mid(index + 1);
    }

    index = pathName.indexOf("[");
    if(index != -1)
    {
        pathName = pathName.left(index);
    }

    // if the path is a bus add the width in [:] format

    if(this->isBus())
    {
        pathName += "[" + QString::number((this->bits.size() - 1)) + ":0]";
    }

    return pathName;
}

std::ostream& operator<<(std::ostream& outputStream, const Path& path)
{

    std::stringstream sStream = std::stringstream();

    // add the path name
    sStream << "Path( " << path.getName().toStdString() << ", " << path.width;

    // print all the bits
    sStream << ", Bits: [";

    for(const auto& bit : path.bits)
    {
        sStream << bit.toStdString() << ",";
    }

    sStream << "]";

    // print the left and right neighbors if they exist
    if(path.sigSource != nullptr)
    {
        sStream << ",\nSignal Source: " << path.sigSource->getName().toStdString();
    }

    if(path.sigDestinations != nullptr)
    {
        sStream << ",\nSignal Destinations: (";
        for(const auto& port : *path.sigDestinations)
        {
            sStream << port->getName().toStdString() << ",";
        }
        sStream << ")";
    }

    sStream << ")";

    return outputStream << sStream.str();
}

QPainterPath Path::createPainterPath(Avoid::ConnRef& connRef, QNetlistGraphicsPath* qPathItem)
{

    QPainterPath qPathPainter;

    Avoid::PolyLine avoidPath = connRef.displayRoute();

    for(const auto& point : avoidPath.ps)
    {

        // move to the first point add the position  as the source text position
        // then move to the next point
        if(point == avoidPath.ps.front())
        {

            qPathPainter.moveTo(point.x, point.y);
            qPathItem->setSrcTextPort(QPointF(point.x, point.y));
            continue;
        }

        // the end of the line should have the name of the destination so
        // we need to add the position to the list of destination text positions
        if(point == avoidPath.ps.back())
        {
            qPathItem->addDstTextPort(QPointF(point.x, point.y), &connRef);
        }

        qPathPainter.lineTo(point.x, point.y);
    }

    return qPathPainter;
}

QPointF Path::findEndOfIntersection(const QPainterPath& firstPath, const QPainterPath& secondPath)
{

    // compare the point so find the last common point

    for(int i = 0; i < firstPath.elementCount(); i++)
    {
        // abort if one path does not have the amount of points
        if(i >= secondPath.elementCount() || i >= firstPath.elementCount())
        {
            return {};
        }

        auto pointOnFistPath = firstPath.elementAt(i);
        auto pointOnSecondPath = secondPath.elementAt(i);

        // when the points do not match this is the first diverging point
        if(pointOnFistPath != pointOnSecondPath)
        {
            // get the last common point
            auto lastCommonPoint = firstPath.elementAt(i - 1);

            // the point that is closer to the last common point is the diverging point
            auto distanceToFirstPath = QLineF(lastCommonPoint, pointOnFistPath).length();
            auto distanceToSecondPath = QLineF(lastCommonPoint, pointOnSecondPath).length();

            return (distanceToFirstPath < distanceToSecondPath) ? pointOnFistPath : pointOnSecondPath;
        }
    }

    return {};
}

std::shared_ptr<Path> Path::createSubPath(const QStringList& bits, std::vector<std::shared_ptr<Path>> existingPaths) const
{

    // check if a path with those bits already exists
    auto findIt = std::find_if(existingPaths.begin(), existingPaths.end(), [&](const std::shared_ptr<Path>& path) {
        return path->getBits() == bits;
    });

    // if it already exists return the path
    if(findIt != existingPaths.end())
    {
        return *findIt;
    }

    auto newPath = std::make_shared<Path>(
        this->getName() +
            bits.first() +
            ":" + bits.last(),
        bits,
        true);

    return newPath;
}

} // namespace OpenNetlistView::Yosys