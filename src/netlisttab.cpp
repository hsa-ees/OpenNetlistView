#include <QWidget>
#include <QBrush>
#include <QColor>
#include <QtCore/Qt>
#include <QString>
#include <QByteArray>

#include <memory>

#include <yosys/module.h>
#include <routing/cola_router.h>

#include "qnetlistscene.h"

#include "netlisttab.h"
#include "ui_netlisttab.h"

namespace OpenNetlistView {

NetlistTab::NetlistTab(const std::shared_ptr<Yosys::Module>& module,
    const std::shared_ptr<std::map<QString, std::shared_ptr<Symbol::Symbol>>>& symbols,
    const QString& modulePath,
    const Routing::ColaRoutingParameters& routingParameters,
    QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::NetlistTab)
    , scene(new QNetlistScene(this))
    , module(module)
    , modulePath(modulePath)
    , symbols(symbols)
{

    ui->setupUi(this);
    ui->netlistView->setBackgroundBrush(QBrush(Qt::white));

    this->setModuleHierarchyVisible();

    ui->labelPath->setText(modulePath);

    router.setRoutingParameters(routingParameters);

    connect(this, &NetlistTab::zoomIn, ui->netlistView, &QNetListView::zoomIn);
    connect(this, &NetlistTab::zoomOut, ui->netlistView, &QNetListView::zoomOut);
    connect(this, &NetlistTab::zoomToFit, ui->netlistView, &QNetListView::zoomToFit);
    connect(this, &NetlistTab::toggleNames, ui->netlistView, &QNetListView::toggleNames);
    connect(this, &NetlistTab::clearAllHighlightColors, ui->netlistView, &QNetListView::clearAllHighlightColors);
    connect(this, &NetlistTab::zoomToNode, ui->netlistView, &QNetListView::zoomToNode);
    connect(this, &NetlistTab::exportToSvg, ui->netlistView, &QNetListView::exportToSvg);
    connect(ui->netlistView, &QNetListView::genericModuleDoubleClicked, this, &NetlistTab::genericModuleDoubleClicked);

    this->scene->setParent(ui->netlistView);
    ui->netlistView->setScene(scene);
    this->upgradeDisplay();
}

NetlistTab::~NetlistTab()
{
    delete ui;
}

void NetlistTab::upgradeDisplay()
{

    // set the module and symbols
    router.setModule(module);
    router.setSymbols(symbols);

    // run the router
    router.runRouter();

    // clear the scene
    scene->clear();

    // convert the routed objects to Qt objects
    auto diagramItems = module->convertToQt();

    for(auto* item : diagramItems)
    {
        scene->addItem(item);
    }

    // render the graphicsView
    ui->netlistView->viewport()->update();
}

void NetlistTab::clearRoutingData()
{
    router.clear();
}

void NetlistTab::setModulePath(const QString& modulePath)
{
    this->modulePath = modulePath;
    this->setModuleHierarchyVisible();
    ui->labelPath->setText(modulePath);
}

QString NetlistTab::getModulePath() const
{
    return modulePath;
}

void NetlistTab::updateSymbols(const std::shared_ptr<std::map<QString, std::shared_ptr<Symbol::Symbol>>>& symbols)
{
    this->symbols = symbols;
    this->clearRoutingData();
}

void NetlistTab::routingParametersChanged(const Routing::ColaRoutingParameters& routingParameters)
{
    router.setRoutingParameters(routingParameters);
    router.clear();
}

Routing::ColaRoutingParameters NetlistTab::getRoutingParameters()
{
    return router.getRoutingParameters();
}

void NetlistTab::setModuleHierarchyVisible()
{
    if(modulePath == "/")
    {
        ui->constLabelPath->setVisible(false);
        ui->labelPath->setVisible(false);
    }
    else
    {
        ui->constLabelPath->setVisible(true);
        ui->labelPath->setVisible(true);
    }
}

} // namespace OpenNetlistView
