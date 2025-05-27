#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QVariantList>
#include <QStringList>
#include <QList>
#include <QVariant>
#include <QVariantMap>

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>
#include <cstdint>
#include <iterator>
#include <stack>
#include <map>
#include <tuple>

#include <symbol/symbol.h>

#include "port.h"
#include "node.h"
#include "path.h"
#include "diagram.h"
#include "module.h"
#include "netname.h"

#include "parser.h"

namespace OpenNetlistView::Yosys {

Parser::Parser()
{
    this->diagram = Diagram();
    this->yosysJsonObject = QJsonObject();
}

Parser::~Parser() = default;

void Parser::setYosysJsonObject(const QJsonObject& yosysJsonObject)
{
    this->yosysJsonObject = yosysJsonObject;
}

std::unique_ptr<Diagram> Parser::getDiagram()
{
    return std::make_unique<Diagram>(this->diagram);
}

void Parser::clearDiagram()
{
    this->diagram = Diagram();
}

void Parser::parse()
{

    // get the modules out of the json data
    const QJsonObject yosysModules = this->yosysJsonObject[YosysJson::modules].toObject();

    // check if there is a modules key in the json object
    if(yosysModules.isEmpty())
    {
        throw std::runtime_error("No modules found in Yosys JSON object");
    }

    // iterate over all modules
    for(auto [name, module] : yosysModules.toVariantMap().asKeyValueRange())
    {

        // check if the module has a blackbox attribute meaning it is part of the library
        // and if it is skip the module
        // alternatively we could check if the src string contains lib/../share/yosys
        if(!module.toJsonObject()[YosysJson::attributes].toObject()[YosysJson::blackbox].isNull())
        {
            continue;
        }

        this->currentModule = std::make_shared<Module>(name);

        // create path objects for the module
        const QJsonObject moduleNetnames = module.toJsonObject()[YosysJson::netnames].toObject();
        this->parseNetnames(moduleNetnames);

        // create port objects for the module
        const QJsonObject modulePorts = module.toJsonObject()[YosysJson::ports].toObject();
        this->parsePorts(modulePorts);

        // create cell objects for the module
        const QJsonObject moduleCells = module.toJsonObject()[YosysJson::cells].toObject();
        this->parseCells(moduleCells);

        auto ports = *this->currentModule->getPorts();
        auto nodes = *this->currentModule->getNodes();

        // if ports or nodes are empty this means the module is invalid
        if(ports.empty() && nodes.empty())
        {
            throw std::runtime_error("Error while parsing " + name.toStdString() + ": Module has no Ports or Nodes");
        }

        // replace the constant bits in the ports with generated bits
        this->replaceConstBits();

        // create connections between all the components
        this->connectDiagramConnections();

        // remove all unconnected paths
        this->removeUnconnectedPaths();

        // check if all components have a connection
        if(this->currentModule->hasModuleInvalidPaths())
        {
            throw std::runtime_error("Error while parsing " + name.toStdString() + ": Module has no Paths or Nodes");
        }

        // check if diagram is empty
        if(this->currentModule->isEmpty())
        {
            throw std::runtime_error("Error while parsing " + name.toStdString() + ": Module has no components");
        }

        // add the diagram to the module
        this->diagram.addModule(this->currentModule);

        // check if the module is the top module
        if(!module.toJsonObject()[YosysJson::attributes].toObject()["top"].isNull())
        {
            this->diagram.setTopModule(this->currentModule);
        }
    }
}

void Parser::connectDiagramConnections()
{

    auto ports = this->currentModule->getPorts();
    auto nodes = this->currentModule->getNodes();

    QList<QStringList> srcPorts;
    QList<QStringList> destPorts;

    // collecting the src and dest ports from the external ports
    for(const auto& port : *ports)
    {

        if(port->hasNoConnectBitsConnection())
        {
            continue;
        }

        if(port->getDirection() == Port::EDirection::INPUT || port->getDirection() == Port::EDirection::CONST)
        {
            srcPorts.push_back(port->getBits());
        }
        else if(port->getDirection() == Port::EDirection::OUTPUT)
        {
            destPorts.push_back(port->getBits());
        }
    }

    // collect the src and dest ports from the nodes
    for(const auto& node : *nodes)
    {
        auto nodePorts = node->getPorts();

        for(const auto& port : nodePorts)
        {
            // skip if the port has a connection bit or more
            if(port->hasNoConnectBitsConnection())
            {
                continue;
            }

            if(port->getDirection() == Port::EDirection::INPUT)
            {
                destPorts.push_back(port->getBits());
            }
            else if(port->getDirection() == Port::EDirection::OUTPUT)
            {
                srcPorts.push_back(port->getBits());
            }
        }
    }

    auto copyDestPorts = destPorts;
    std::map<QStringList, QList<QStringList>> splitInfo = {};
    std::map<QStringList, QList<QStringList>> joinInfo = {};

    for(auto& destPort : destPorts)
    {
        createSplitJoin(srcPorts,
            copyDestPorts,
            destPort,
            0,
            destPort.length(),
            splitInfo,
            joinInfo);
    }

    // create the split and join objects
    createJoinNodes(joinInfo);
    createSplitNodes(splitInfo);

    // create the paths and connect to the ports
    createSignalConnections();
}

void Parser::parsePorts(const QJsonObject& ports)
{

    // iterate over all ports
    for(auto [name, value] : ports.toVariantMap().asKeyValueRange())
    {

        // create a port object
        QJsonObject portData = value.toJsonObject();
        const auto portInstance = Parser::createPort(name, portData[YosysJson::bits], portData[YosysJson::direction]);

        // add the port to the diagram
        this->currentModule->addPort(portInstance);
    }
}

void Parser::parseCells(const QJsonObject& cells)
{

    // iterate over all cells
    for(auto [name, value] : cells.toVariantMap().asKeyValueRange())
    {

        // get the data and type from the cell
        QJsonObject cellData = value.toJsonObject();
        auto cellType = cellData[YosysJson::type];

        // check if the type is valid if not abort parsing
        if(!cellType.isString())
        {
            throw std::runtime_error("Error while parsing " + name.toStdString() + ": Cell type is not valid");
        }

        auto portDirections = cellData[YosysJson::port_directions].toObject();
        auto portConnections = cellData[YosysJson::connections].toObject();

        // check if the port directions and connections are not empty if they are abort parsing
        if(portDirections.isEmpty() || portConnections.isEmpty())
        {
            throw std::runtime_error("Error while parsing " + name.toStdString() + ": No port directions or connections found");
        }

        // check if the directions and connections are the same size else abort parsing
        if(portDirections.size() != portConnections.size())
        {
            throw std::runtime_error("Error while parsing " + name.toStdString() +
                                     ": the number of port direction definitions does not match the number of port connections");
        }

        int indexIn = 0;
        int indexOut = 0;
        // create ports for the cell
        std::vector<std::shared_ptr<Port>> ports;
        for(const auto& portName : portDirections.toVariantMap().keys())
        {

            auto port = Parser::createPort(portName, portConnections[portName], portDirections[portName]);

            QString symbolNameAlias = "";
            if(portDirections[portName].toString() == "input" && !SymbolTypes::isValidSymbolType(cellType.toString()))
            {
                symbolNameAlias = "in" + QString::number(indexIn++);
            }
            else if(portDirections[portName].toString() == "output" && !SymbolTypes::isValidSymbolType(cellType.toString()))
            {
                symbolNameAlias = "out" + QString::number(indexOut++);
            }

            port->setSymbolNameAlias(symbolNameAlias);

            ports.emplace_back(port);
        }

        // check if all ports were created successfully if not abort parsing
        if(portDirections.size() != ports.size())
        {
            throw std::runtime_error("Error while parsing " + name.toStdString() + ": Not all ports could be created successfully");
        }

        // add the finished cell to the diagram
        auto cellNode = std::make_shared<Node>(name, cellType.toString(), ports);
        this->currentModule->addNode(cellNode);

        // add the node to the ports as parent
        for(auto& port : ports)
        {
            port->setParentNode(cellNode);
        }
    }
}

void Parser::parseNetnames(const QJsonObject& paths)
{

    for(auto [pathName, pathData] : paths.toVariantMap().asKeyValueRange())
    {

        // check if hide_name is set if it is skip the netname
        auto netnameDataObject = pathData.toJsonObject();

        bool hiddenName = false;
        if(netnameDataObject[YosysJson::hide_name].toInt() == 1)
        {
            hiddenName = true;
        }

        // get the bits of the netname if they are not present abort parsing
        auto bitsArray = netnameDataObject[YosysJson::bits].toArray();
        if(bitsArray.isEmpty())
        {
            throw std::runtime_error("Error while parsing the netname " + pathName.toStdString() + ": No bits found");
        }

        if(std::all_of(bitsArray.begin(), bitsArray.end(), [](const QJsonValue& bit) { return bit.isString() || bit.toString() == "x"; }))
        {
            continue;
        }

        // check if the port has the field "unused_bits" these bits need to be
        // removed because they are not needed in the diagram and can cause problems
        // while connecting the paths
        const auto unusedBits = netnameDataObject[YosysJson::attributes].toObject()[YosysJson::unused_bits];
        if(unusedBits.isString())
        {
            // parse the string and split it at the spaces
            const auto unusedBitsArray = unusedBits.toString().split(" ");

            // convert it to integers and remove the index from the bitsArray do it from the back to the front
            // to not mess up the index
            std::for_each(unusedBitsArray.rbegin(), unusedBitsArray.rend(), [&](const QString& bit) {
                bitsArray.removeAt(bit.toInt());
            });
        }

        // convert the bits to integers
        const QVariantList bits = QVariantList(bitsArray.toVariantList());

        QStringList bitStrings = {};

        for(const auto& bit : bits)
        {
            bitStrings.push_back(bit.toString());
        }

        // check if the path is already in the diagram if it is skip it
        bool foundAlternative = false;
        auto diagNetnames = this->currentModule->getNetnames();
        for(const auto& diagNetname : *diagNetnames)
        {
            if(diagNetname->getBits() == bitStrings)
            {
                diagNetname->addAlternativeName(pathName);
                foundAlternative = true;
                continue;
            }
        }

        // add to the diagram
        if(!foundAlternative)
        {
            this->currentModule->addNetname(std::make_shared<Netname>(pathName, bitStrings, hiddenName));
        }
    }
}

std::shared_ptr<Port> Parser::createPort(const QString& name, const QJsonValue& bitData, const QJsonValue& directionData)
{

    // get the correct direction value
    Port::EDirection direction = Port::EDirection::INPUT; // Default initialization
    const QString directionStr = directionData.toString();

    if(directionStr == YosysJson::input_dir)
    {
        direction = Port::EDirection::INPUT;
    }
    else if(directionStr == YosysJson::output_dir)
    {
        direction = Port::EDirection::OUTPUT;
    }
    else
    {
        throw std::runtime_error("Error while parsing the port " + name.toStdString() + ": Invalid direction");
    }

    // get the bits values
    auto bitDataArray = bitData.toArray();

    if(bitDataArray.isEmpty())
    {
        throw std::runtime_error("Error while parsing the port " + name.toStdString() + ": No bits found");
    }

    const QVariantList bitValues = QVariantList(bitDataArray.toVariantList());
    // convert bits to strings
    QStringList bitValueStrings;
    for(const auto& bit : bitValues)
    {
        bitValueStrings.push_back(bit.toString());
    }

    std::shared_ptr<Port> portInstance = std::make_shared<Port>(name, direction, bitValueStrings);

    return portInstance;
}

std::shared_ptr<Port> Parser::createConstantPort(const QString& name, const QStringList& bits, const QStringList& constValue)
{

    auto constPort = std::make_shared<Port>(name, Port::EDirection::CONST, bits);

    constPort->setConstPortValue(constValue);

    return constPort;
}

std::map<std::tuple<uint64_t, uint64_t>, QStringList> Parser::splitBits(const QStringList& bits)
{
    if(bits.empty())
    {
        return {};
    }

    std::map<std::tuple<uint64_t, uint64_t>, QStringList> splitBits;

    QStringList currentBits;
    uint64_t startIdx = 0;
    bool lastWasConst = (bits.at(0) == "0" || bits.at(0) == "1");

    // split the bits into segments at points where the bits switch from strings to integers
    for(const auto& bit : bits)
    {
        const bool isConst = (bit == "0" || bit == "1");
        if(currentBits.empty() || isConst == lastWasConst)
        {
            currentBits.append(bit);
        }
        else
        {
            splitBits[std::make_tuple(startIdx, startIdx + currentBits.size() - 1)] = currentBits;
            startIdx += currentBits.size();
            currentBits = QStringList();
            currentBits.append(bit);
        }

        lastWasConst = isConst;
    }

    if(!currentBits.empty())
    {
        splitBits[std::make_tuple(startIdx, startIdx + currentBits.size() - 1)] = currentBits;
    }

    return splitBits;
}

// NOLINTBEGIN(misc-no-recursion)
void Parser::createSplitJoin(QList<QStringList>& srcPorts,
    QList<QStringList>& destPorts,
    QStringList toSolve,
    int64_t startIdx,
    int64_t endIdx,
    std::map<QStringList, QList<QStringList>>& splitInfo,
    std::map<QStringList, QList<QStringList>>& joinInfo)
{

    std::stack<Task> tasks;
    tasks.push({startIdx, endIdx, toSolve.mid(startIdx, endIdx - startIdx)});

    while(!tasks.empty())
    {
        const Task current = tasks.top();
        tasks.pop();

        const int64_t destIdx = destPorts.indexOf(toSolve);
        if(destIdx != -1)
        {
            destPorts.remove(destIdx, 1);
        }

        if(current.startIdx >= toSolve.length() || current.endIdx - current.startIdx < 1)
        {
            continue;
        }

        QStringList querryBits = current.querryBits;

        if(srcPorts.contains(querryBits))
        {
            if(querryBits != toSolve)
            {
                addToMap(joinInfo, toSolve, querryBits);
            }
            tasks.push({current.endIdx, toSolve.length(), toSolve.mid(current.endIdx, toSolve.length() - current.endIdx)});
            continue;
        }

        const int64_t indexOfSubPath = indexOfContains(srcPorts, querryBits);
        if(indexOfSubPath != -1)
        {
            if(querryBits != toSolve)
            {
                addToMap(joinInfo, toSolve, querryBits);
            }

            addToMap(splitInfo, srcPorts[indexOfSubPath], querryBits);
            srcPorts.push_back(querryBits);

            tasks.push({current.endIdx, toSolve.length(), toSolve.mid(current.endIdx, toSolve.length() - current.endIdx)});
            continue;
        }

        if(indexOfContains(destPorts, querryBits) != -1)
        {
            if(querryBits != toSolve)
            {
                addToMap(joinInfo, toSolve, querryBits);
            }

            QList<QStringList> tmpDstPorts;
            createSplitJoin(srcPorts, tmpDstPorts, querryBits, 0, querryBits.length(), splitInfo, joinInfo);
            srcPorts.push_back(querryBits);

            if(std::search(toSolve.begin(), toSolve.end(), querryBits.begin(), querryBits.end()) != toSolve.end())
            {
                tasks.push({current.endIdx, toSolve.length(), toSolve.mid(current.endIdx, toSolve.length() - current.endIdx)});
            }
            continue;
        }

        tasks.push({current.startIdx, current.startIdx + querryBits.length() - 1, toSolve.mid(current.startIdx, querryBits.length() - 1)});
    }
}
// NOLINTEND(misc-no-recursion)

int64_t Parser::indexOfContains(QList<QStringList> list, QStringList element)
{
    auto foundIdxIt = std::find_if(list.begin(), list.end(), [&](const QStringList& haystack) {
        return std::search(haystack.begin(), haystack.end(), element.begin(), element.end()) != haystack.end();
    });

    return (foundIdxIt != list.end()) ? std::distance(list.begin(), foundIdxIt) : -1;
}

void Parser::addToMap(std::map<QStringList, QList<QStringList>>& map, const QStringList& key, const QStringList& value)
{

    if(map.find(key) == map.end())
    {
        map[key] = {value};
    }
    else
    {
        map[key].push_back(value);
    }
}

void Parser::replaceConstBits()
{

    std::vector<std::shared_ptr<Port>> srcPorts = {};

    // get all dest ports
    auto modulePorts = this->currentModule->getPorts();
    auto moduleNodes = this->currentModule->getNodes();

    for(const auto& port : *modulePorts)
    {
        if(port->getDirection() == Port::EDirection::OUTPUT && port->hasConstantBits())
        {
            srcPorts.push_back(port);
        }
    }

    for(const auto& node : *moduleNodes)
    {
        for(const auto& port : node->getPorts())
        {
            if(port->getDirection() == Port::EDirection::INPUT && port->hasConstantBits())
            {
                srcPorts.push_back(port);
            }
        }
    }
    // get the maximum bit number in the module
    unsigned long long maxBitNumber = this->currentModule->getMaxBitNumber();
    for(auto& srcPort : srcPorts)
    {

        auto splitBitsMap = Parser::splitBits(srcPort->getBits());
        const auto originalBits = srcPort->getBits();

        for(auto& [pos, bitValue] : splitBitsMap)
        {
            if((bitValue.at(0) != "0" && bitValue.at(0) != "1"))
            {
                continue;
            }

            // create the bits for it
            QStringList bits = {};
            for(int i = 0; i < bitValue.length(); i++)
            {
                maxBitNumber++;
                bits.push_back(QString::number(maxBitNumber));
            }

            // create the port
            auto constPort = createConstantPort(srcPort->getName() + "_const", bits, bitValue);
            this->currentModule->addPort(constPort);

            // replace the const parts in the port with the generated bits
            srcPort->replaceBits(pos, bits);
        }

        // add the bits to a translation map
        this->constToNonConstPortBits[originalBits] = srcPort->getBits();
    }
}

void Parser::createSplitNodes(const std::map<QStringList, QList<QStringList>>& splitInfo)
{

    int splitIndex = 0;

    for(const auto& [srcBits, destBits] : splitInfo)
    {
        std::vector<std::shared_ptr<Port>> spliterPorts = {};

        // create the output Ports
        spliterPorts.emplace_back(std::make_shared<Port>("in", Port::EDirection::INPUT, srcBits));

        int index = 0;

        for(const auto& destBit : destBits)
        {
            spliterPorts.emplace_back(std::make_shared<Port>("out" + QString::number(index++), Port::EDirection::OUTPUT, destBit));
        }

        // create the node
        auto spliterNode = std::make_shared<Node>("split" + QString::number(splitIndex++), YosysJson::splitType, spliterPorts);

        for(const auto& port : spliterPorts)
        {
            port->setParentNode(spliterNode);
        }

        this->currentModule->addNode(spliterNode);
    }
}

void Parser::createJoinNodes(const std::map<QStringList, QList<QStringList>>& joinInfo)
{

    int joinIndex = 0;

    for(const auto& [srcBits, destBits] : joinInfo)
    {
        std::vector<std::shared_ptr<Port>> joinerPorts = {};

        // create the input Ports
        int index = 0;
        for(const auto& destBit : destBits)
        {
            joinerPorts.emplace_back(std::make_shared<Port>("in" + QString::number(index++), Port::EDirection::INPUT, destBit));
        }

        // create the output Port
        joinerPorts.emplace_back(std::make_shared<Port>("out", Port::EDirection::OUTPUT, srcBits));

        // create the node
        auto joinerNode = std::make_shared<Node>("join" + QString::number(joinIndex++), YosysJson::joinType, joinerPorts);

        for(const auto& port : joinerPorts)
        {
            port->setParentNode(joinerNode);
        }

        this->currentModule->addNode(joinerNode);
    }
}

void Parser::createSignalConnections()
{
    // get all the input and output ports separated
    auto modulePorts = this->currentModule->getPorts();
    auto moduleNodes = this->currentModule->getNodes();

    std::vector<std::shared_ptr<Port>> srcPorts = {};
    std::vector<std::shared_ptr<Port>> destPorts = {};

    // collect all src and dest ports
    for(const auto& port : *modulePorts)
    {
        if(port->getDirection() == Port::EDirection::INPUT || port->getDirection() == Port::EDirection::CONST)

        {
            srcPorts.push_back(port);
        }
        else
        {
            destPorts.push_back(port);
        }
    }

    for(const auto& node : *moduleNodes)
    {
        for(const auto& port : node->getPorts())
        {
            if(port->getDirection() == Port::EDirection::OUTPUT)
            {
                srcPorts.push_back(port);
            }
            else
            {
                destPorts.push_back(port);
            }
        }
    }

    // connect the signal sources to the paths
    connectSignalSrcConnections(srcPorts);

    // connect the signal destinations to the paths
    connectSignalDestConnections(destPorts);
}

void Parser::connectSignalSrcConnections(std::vector<std::shared_ptr<Port>>& srcPorts)
{

    // create a signal for each srcPort
    for(const auto& srcPort : srcPorts)
    {
        bool isHidden = true;
        QString pathName = srcPort->getName() + "_sig";

        // search if the bits are inside the netnames
        if(srcPort->getDirection() == Port::EDirection::CONST)
        {
            auto findIt = this->constToNonConstPortBits.find(srcPort->getBits());

            if(findIt != this->constToNonConstPortBits.end())
            {
                // get the netname by the findIt bits in second
                auto netname = this->currentModule->getNetnameByBits(findIt->second);

                if(netname != nullptr)
                {
                    pathName = netname->getName();
                    isHidden = netname->getIsHidden();
                }
            }
        }
        else
        {
            auto findIt = this->currentModule->getNetnameByBits(srcPort->getBits());

            if(findIt != nullptr)
            {
                pathName = findIt->getName();
                isHidden = findIt->getIsHidden();
            }
        }

        // create the path
        auto path = std::make_shared<Path>(pathName, srcPort->getBits(), isHidden);
        path->setSigSource(srcPort);
        srcPort->setPath(path);
        this->currentModule->addPath(path);
    }
}

void Parser::connectSignalDestConnections(std::vector<std::shared_ptr<Port>>& destPorts)
{
    // go through all the destPorts and connect them to the signals
    for(const auto& destPort : destPorts)
    {
        // get the signal by bits
        auto path = this->currentModule->getPathByBits(destPort->getBits());

        if(path == nullptr)
        {
            continue;
        }

        path->addSigDestination(destPort);
        destPort->setPath(path);
    }
}

void Parser::removeUnconnectedPaths()
{
    auto paths = this->currentModule->getPaths();

    std::vector<std::shared_ptr<Path>> pathsToRemove;

    for(const auto& path : *paths)
    {
        if(!path->hasConnection())
        {
            pathsToRemove.push_back(path);
        }
    }

    for(const auto& path : pathsToRemove)
    {
        this->currentModule->removePath(path);
    }
}

} // namespace OpenNetlistView::Yosys