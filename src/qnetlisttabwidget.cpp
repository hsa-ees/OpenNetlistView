#include <QTabWidget>
#include <QWidget>
#include <QString>
#include <QByteArray>
#include <QApplication>
#include <QMessageBox>

#include <memory>
#include <map>
#include <utility>
#include <stdexcept>

#include <yosys/module.h>
#include <yosys/diagram.h>
#include <routing/cola_router.h>
#include <symbol/symbol.h>

#include "netlisttab.h"

#include "qnetlisttabwidget.h"

namespace OpenNetlistView {

QNetlistTabWidget::QNetlistTabWidget(QWidget* parent)
    : QTabWidget(parent)
    , routingParameters{}
{

    // connect the close tab signal
    connect(this, &QTabWidget::tabCloseRequested, [this](int index) {
        if(index == 0)
        {
            return;
        }
        auto* tab = this->netlistTabs.at(index);
        this->removeTab(index);
        this->netlistTabs.erase(this->netlistTabs.begin() + index);
        delete tab;
    });

    // connect tab changed
    connect(this, &QTabWidget::currentChanged, [this](int index) {
        // check if the index is valid
        if(index < 0 || index >= this->netlistTabs.size())
        {
            return;
        }

        // check if the tab is  valid
        auto* tab = this->netlistTabs.at(index);

        if(tab == nullptr)
        {
            return;
        }

        try
        {
            tab->upgradeDisplay();
        }
        catch(const std::runtime_error& e)
        {
            emit showError(e.what());
        }

        this->tabChanged = true;

        // mark the module in the hierarchy tree
        emit setHierarchyPos(this->tabText(index));
    });
}

QNetlistTabWidget::~QNetlistTabWidget() = default;

void QNetlistTabWidget::setSymbols(const std::shared_ptr<std::map<QString, std::shared_ptr<Symbol::Symbol>>>& symbols)
{
    this->symbols = symbols;

    for(auto* tab : this->netlistTabs)
    {
        tab->updateSymbols(this->symbols);

        // redroute the current tab
        if(tab == currentWidget())
        {
            try
            {

                tab->upgradeDisplay();
            }
            catch(const std::exception& e)
            {
                emit showError(e.what());
            }
        }
    }
}

void QNetlistTabWidget::setDiagram(std::unique_ptr<Yosys::Diagram> diagram)
{
    // set the pointer
    this->diagram = std::move(diagram);

    // of a new diagram is loaded set the tab changed to true to update reset values in settings
    this->tabChanged = true;

    // creates the top module tab it is the root of the path and has no instance name
    addNetlistTab(this->diagram->getTopModule(), "/", "");
}

void QNetlistTabWidget::setRoutingParameters(const Routing::ColaRoutingParameters& routingParameters)
{
    this->routingParameters = routingParameters;
}

void QNetlistTabWidget::reset()
{
    // clear all tabs do it backwards to prevent crashes when trying to change to
    for(auto tab = this->netlistTabs.rbegin(); tab != this->netlistTabs.rend(); ++tab)
    {
        this->removeTab(this->indexOf(*tab));
        delete *tab;
    }
    this->netlistTabs.clear();
    this->diagram = nullptr;
}

Routing::ColaRoutingParameters QNetlistTabWidget::getCurrentTabRoutingParameters() const
{
    // get the active tab and call the routing parameters function
    auto* tab = dynamic_cast<NetlistTab*>(currentWidget());
    if(tab != nullptr)
    {
        return tab->getRoutingParameters();
    }

    return {};
}

void QNetlistTabWidget::zoomIn()
{
    // get the active tab and call the zoom in function
    auto* tab = dynamic_cast<NetlistTab*>(currentWidget());
    if(tab != nullptr)
    {
        tab->zoomIn();
    }
}

void QNetlistTabWidget::zoomOut()
{
    // get the active tab and call the zoom out function
    auto* tab = dynamic_cast<NetlistTab*>(currentWidget());
    if(tab != nullptr)
    {
        tab->zoomOut();
    }
}

void QNetlistTabWidget::zoomToFit()
{
    // get the active tab and call the zoom to fit function
    auto* tab = dynamic_cast<NetlistTab*>(currentWidget());
    if(tab != nullptr)
    {
        tab->zoomToFit();
    }
}

void QNetlistTabWidget::toggleNames()
{
    // get the active tab and call the toggle names function
    auto* tab = dynamic_cast<NetlistTab*>(currentWidget());
    if(tab != nullptr)
    {
        tab->toggleNames();
    }
}

void QNetlistTabWidget::clearAllHighlightColors()
{
    // get the active tab and call the clear all highlight colors function
    auto* tab = dynamic_cast<NetlistTab*>(currentWidget());
    if(tab != nullptr)
    {
        tab->clearAllHighlightColors();
    }
}

void QNetlistTabWidget::zoomToNode(const QString& nodeName)
{
    // get the active tab and call the zoom to node function
    auto* tab = dynamic_cast<NetlistTab*>(currentWidget());
    if(tab != nullptr)
    {
        tab->zoomToNode(nodeName);
    }
}

QByteArray QNetlistTabWidget::exportToSvg(bool exportSelected)
{
    // get the active tab and call the export to svg function
    auto* tab = dynamic_cast<NetlistTab*>(currentWidget());
    if(tab != nullptr)
    {
        return tab->exportToSvg(exportSelected);
    }

    return {};
}

void QNetlistTabWidget::genericModuleDoubleClicked(const QString& moduleName, const QString& moduleType)
{

    // find the module with the correct type
    auto module = this->diagram->getModuleByName(moduleType);

    if(module == nullptr)
    {
        return;
    }

    const auto modulePath = generateModulePath(module, moduleName);

    addNetlistTab(module, modulePath, moduleName);
}

void QNetlistTabWidget::addNetlistTab(const std::shared_ptr<Yosys::Module>& module, const QString& modulePath, const QString& moduleInstanceName)
{
    // check if symbols are set if not abort
    if(this->symbols == nullptr || module == nullptr)
    {
        return;
    }

    // check if the module path is is already open
    for(auto* tab : this->netlistTabs)
    {
        if(tab->getModulePath() == modulePath)
        {
            setCurrentWidget(tab);
            return;
        }
    }

    // get the number of paths in the module
    const auto paths = module->getPaths();

    int portObjCount = 0;

    for(const auto& path : *paths)
    {
        if(path->getSigSource() != nullptr)
        {
            portObjCount++;
        }

        portObjCount += path->getSigDestinations()->size();
    }

    if(portObjCount > sizeQuestionThreshold)
    {
        lastModule = module;
        lastModulePath = modulePath;
        lastModuleInstanceName = moduleInstanceName;

        emit displayLargeModuleQuestion();
        return;
    }

    calculateRoutingParameters(module);

    createNetlistTab(module, modulePath, moduleInstanceName);
}

void QNetlistTabWidget::largeModuleAccepted()
{
    calculateRoutingParameters(lastModule);
    createNetlistTab(lastModule, lastModulePath, lastModuleInstanceName);

    lastModule = nullptr;
    lastModulePath.clear();
    lastModuleInstanceName.clear();
}

bool QNetlistTabWidget::getTabChanged()
{

    const bool oldTabChanged = this->tabChanged;
    this->tabChanged = false;

    return oldTabChanged;
}

void QNetlistTabWidget::routingParametersChanged(Routing::ColaRoutingParameters routingParameters)
{
    this->routingParameters = routingParameters;

    auto* tab = dynamic_cast<NetlistTab*>(currentWidget());

    // get the current tab
    if(tab != nullptr)
    {
        tab->routingParametersChanged(this->routingParameters);
        tab->upgradeDisplay();
    }
}

QString QNetlistTabWidget::generateModulePath(const std::shared_ptr<Yosys::Module>& module, const QString& moduleInstanceName)
{
    // check if the module is the top module
    if(module->getType() == moduleInstanceName)
    {
        return "/";
    }

    // create the module path
    QString modulePath;
    auto* activeTab = dynamic_cast<NetlistTab*>(currentWidget());
    if(activeTab == nullptr)
    {
        modulePath = "/";
    }
    else
    {
        modulePath += activeTab->getModulePath() + moduleInstanceName + "/";
    }

    return modulePath;
}

void QNetlistTabWidget::createNetlistTab(const std::shared_ptr<Yosys::Module>& module, const QString& modulePath, const QString& moduleInstanceName)
{

    if(module == nullptr)
    {
        return;
    }

    NetlistTab* tab = nullptr;

    try
    {
        tab = new NetlistTab(module, symbols, modulePath, routingParameters, this);
    }
    catch(const std::exception& e)
    {
        emit showError(e.what());
    }

    if(tab == nullptr)
    {
        return;
    }

    this->netlistTabs.emplace_back(tab);

    connect(tab, &NetlistTab::genericModuleDoubleClicked, this, &QNetlistTabWidget::genericModuleDoubleClicked);

    QString tabName = module->getType();

    if(!moduleInstanceName.isEmpty())
    {
        tabName += ":" + moduleInstanceName;
    }

    addTab(tab, tabName);

    // set it as the active tab
    setCurrentWidget(tab);
    tab->zoomToFit();

    // clear the last variables
    lastModule = nullptr;
    lastModulePath.clear();
    lastModuleInstanceName.clear();
}

void QNetlistTabWidget::calculateRoutingParameters(const std::shared_ptr<Yosys::Module>& module)
{
    // check if the module is valid
    if(module == nullptr)
    {
        return;
    }

    // get the number of paths in the module
    const auto paths = module->getPaths();

    int portObjCount = 0;

    for(const auto& path : *paths)
    {
        if(path->getSigSource() != nullptr)
        {
            portObjCount++;
        }

        portObjCount += path->getSigDestinations()->size();
    }

    // get the number of nodes and external ports
    const auto nodeCount = module->getNodes()->size();
    const auto ePortObjCount = module->getPorts()->size();

    // get the constraints based on the slope of the value
    // solopes where determined by running a test
    const auto cPortObj = static_cast<double>(portObjCount) / slopePortObj;
    const auto cNode = static_cast<double>(nodeCount) / slopeNodeObj;
    const auto cEPortObj = static_cast<double>(ePortObjCount) / slopeEPortObj;

    // get the median constraint value
    auto constraintValue = (((cPortObj) + (cNode) + (cEPortObj)) / (3));

    // check if the constraint value is smaller than the minimum constraint
    if(constraintValue < minConstraint)
    {
        constraintValue = minConstraint;
    }

    routingParameters.defaultXConstraint = constraintValue;
    routingParameters.defaultYConstraint = constraintValue;
    routingParameters.defaultEdgeLength = defaultEdgeLength;
}

} // namespace OpenNetlistView