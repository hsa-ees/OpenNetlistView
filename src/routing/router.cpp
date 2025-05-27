#include <memory>
#include <utility>
#include <map>
#include <algorithm>

#include <yosys/module.h>
#include <yosys/port.h>
#include <symbol/symbol.h>

#include "router.h"
#include "cola_router.h"
#include "avoid_router.h"

namespace OpenNetlistView::Routing {

Router::Router()
    : module(nullptr)
{
}

Router::~Router() = default;

void Router::setModule(std::shared_ptr<Yosys::Module> module)
{
    this->module = std::move(module);
}

std::shared_ptr<Yosys::Module> Router::getModule()
{
    return std::move(module);
}

void Router::setSymbols(const std::shared_ptr<std::map<QString, std::shared_ptr<Symbol::Symbol>>>& symbols)
{
    this->symbols = symbols;
}

void Router::setRoutingParameters(const ColaRoutingParameters& routingParameters)
{
    cola.setRoutingParameters(routingParameters);
}

ColaRoutingParameters Router::getRoutingParameters()
{
    return cola.getRoutingParameters();
}

void Router::runRouter()
{

    // if the symbols or module are not set abort
    // also abort when the module is already routed
    if((symbols != nullptr && symbols->empty()) ||
        module == nullptr || module->getIsRouted())
    {
        return;
    }

    this->assignSymbols();
    this->runCola();

    this->runAvoid();

    this->module->setIsRouted();
}

void Router::clear()
{
    // clean the avoid and cola routers
    this->cola.clear();
    this->avoid.clear();

    // clear the diagrams routing data
    module->clearRoutingData();

    // reset the isRouted flag
    module->resetIsRouted();
}

void Router::assignSymbols()
{

    if(symbols == nullptr)
    {
        return;
    }

    // set the symbols for the nodes
    auto nodes = module->getNodes();

    for(auto& node : *nodes)
    {
        // get the ports of the node
        auto ports = node->getPorts();

        // check if any port is a bus
        const bool isBus = std::any_of(ports.begin(),
            ports.end(),
            [](const std::shared_ptr<Yosys::Port>& port) { return port->getWidth() > 1; });
        auto busSymbol = this->symbols->find(node->getType() + busIdentifier);

        // set the symbol for the node
        auto symbol = this->symbols->find(node->getType());

        if(node->getType() == "split" || node->getType() == "join")
        {
            node->setSymbol(this->createJoinSplit(node));
        }
        else if(isBus && busSymbol != this->symbols->end())
        {
            node->setType(node->getType() + busIdentifier);
            node->setSymbol(busSymbol->second);
        }
        else if(symbol != this->symbols->end())
        {
            node->setSymbol(this->symbols->at(node->getType()));
        }
        else
        {
            node->setSymbol(this->createGenericSymbol(node));
        }
    }

    // set the in and out symbols for the ports
    auto ports = module->getPorts();

    for(auto& port : *ports)
    {

        switch(port->getDirection())
        {

            case Yosys::Port::EDirection::INPUT:
                port->setSymbol(this->symbols->at("inputExt"));
                break;

            case Yosys::Port::EDirection::OUTPUT:

                port->setSymbol(this->symbols->at("outputExt"));
                break;

            case Yosys::Port::EDirection::CONST:

                port->setSymbol(this->symbols->at("constant"));
                break;

            default:
                break;
        }
    }
}

void Router::runCola()
{

    // run the cola layout on the module
    cola.setModule(std::move(module));
    cola.runCola();
    this->module = std::move(cola.getModule());
}

void Router::runAvoid()
{

    // run the obstacle avoidance on the module
    avoid.setModule(std::move(module));
    avoid.setColaRectangles(cola.getRectangles());
    avoid.setColaEdges(cola.getEdges());
    avoid.runAvoid();
    this->module = std::move(avoid.getModule());
}

std::shared_ptr<Symbol::Symbol> Router::createJoinSplit(const std::shared_ptr<Yosys::Node>& node)
{

    // get the ports of the node
    auto ports = node->getPorts();

    int inputs = 0;
    int outputs = 0;
    const QString type = node->getType();

    // count the inputs and outputs
    for(const auto& port : ports)
    {
        if(port->getDirection() == Yosys::Port::EDirection::INPUT)
        {
            inputs++;
        }
        else if(port->getDirection() == Yosys::Port::EDirection::OUTPUT)
        {
            outputs++;
        }
    }

    // generate the name of the symbol
    const QString splitJoinName = type + "_i" + QString::number(inputs) + "_o" + QString::number(outputs);

    // check if the symbol is already generated then use it
    auto foundSymbol = this->symbols->find(splitJoinName);
    if(foundSymbol != this->symbols->end())
    {
        return foundSymbol->second;
    }

    const auto baseSymbol = this->symbols->at(type);

    // else generate the symbol

    std::shared_ptr<Symbol::Symbol> generatedSymbol;

    if(type == "split")
    {
        generatedSymbol = Symbol::Symbol::createJoinSplit(outputs, baseSymbol);
    }

    else if(type == "join")
    {
        generatedSymbol = Symbol::Symbol::createJoinSplit(inputs, baseSymbol);
    }

    this->symbols->insert({splitJoinName, generatedSymbol});

    return generatedSymbol;
}

std::shared_ptr<Symbol::Symbol> Router::createGenericSymbol(const std::shared_ptr<Yosys::Node>& node)
{
    // get the number of in and outputs
    auto ports = node->getPorts();

    int inputs = 0;
    int outputs = 0;

    for(const auto& port : ports)
    {
        if(port->getDirection() == Yosys::Port::EDirection::INPUT)
        {
            inputs++;
        }
        else if(port->getDirection() == Yosys::Port::EDirection::OUTPUT)
        {
            outputs++;
        }
    }

    // generate the name of the symbol
    const QString moduleName = "generic_i" + QString::number(inputs) + "_o" + QString::number(outputs);

    // check if the symbol is already generated then use it
    auto foundSymbol = this->symbols->find(moduleName);
    if(foundSymbol != this->symbols->end())
    {
        foundSymbol->second->setGeneric(true);
        return foundSymbol->second;
    }

    // else generate the symbol
    const auto baseSymbol = this->symbols->at("generic");

    std::shared_ptr<Symbol::Symbol> generatedSymbol = Symbol::Symbol::createGenericSymbol(inputs, outputs, baseSymbol);

    this->symbols->insert({moduleName, generatedSymbol});

    generatedSymbol->setGeneric(true);

    return generatedSymbol;
}

} // namespace OpenNetlistView::Routing