#include <QMainWindow>
#include <QWidget>
#include <QString>
#include <QFileDialog>
#include <QPushButton>
#include <QDir>
#include <QJsonDocument>
#include <QGraphicsScene>
#include <QDomDocument>
#include <QInputDialog>
#include <QByteArray>
#include <QIODevice>
#include <QMessageBox>
#include <QBrush>
#include <QtCore/Qt>
#include <QtPreprocessorSupport>
#include <qlogging.h>
#include <QFile>
#include <QtGlobal>
#include <QStandardItem>
#include <QStandardItemModel>

#include <stdexcept>
#include <memory>
#include <map>
#include <utility>
#include <algorithm>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif // EMSCRIPTEN

#include <yosys/parser.h>
#include <symbol/symbol_parser.h>
#include <yosys/module.h>

#include "qtreeview.h"
#include "qnetlisttabwidget.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogabout.h"
#include "dialogsettings.h"
#include "dialogsearch.h"

namespace OpenNetlistView {

MainWindow::MainWindow(const QString& jsonFilename, const QString& skinFilename, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , dialogAbout(new DialogAbout(this))
    , dialogSettings(new DialogSettings(this))
    , dialogSearch(new DialogSearch(this))
    , askRemoveDialog(new QMessageBox(this))
    , longRoutingMessage(new QMessageBox(this))
    , diagram(nullptr)
    , currentModule(nullptr)
    , errorMessage(nullptr)
{

    ui->setupUi(this);

    // loads the embedded default symbols if the cli option is not used
    QByteArray symbolByteData;
    if(skinFilename.isEmpty())
    {
        symbolByteData = DialogSettings::getDefaultSymbolData();
    }
    else
    {
        symbolByteData = loadFileCLI(skinFilename);
        dialogSettings->setCustomSymbolsLoadedCLI();
    }

    this->loadUpdatedSymbols(symbolByteData);

    ui->tabNetlists->setSymbols(
        std::make_shared<std::map<QString, std::shared_ptr<Symbol::Symbol>>>(this->symbols));

    // create the dialog that asks if a diagram is overwritten
    if(askRemoveDialog != nullptr)
    {
        askRemoveDialog->setText(tr("Opening a new file will discard the recently generated netlist view. \nDo you want to proceed?"));
        askRemoveDialog->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        askRemoveDialog->setModal(true);
        askRemoveDialog->setIcon(QMessageBox::Icon::Question);
        connect(askRemoveDialog, &QMessageBox::finished, this, &MainWindow::closeAskRemoveLoadedDiagram);
    }

    if(longRoutingMessage != nullptr)
    {
        // create the dialog that shows the routing progress
        longRoutingMessage->setText(tr("You are about open a large module. Routing this may take a while. \nDo you want to proceed?"));
        longRoutingMessage->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        longRoutingMessage->setModal(true);
        longRoutingMessage->setIcon(QMessageBox::Icon::Question);
        connect(longRoutingMessage, &QMessageBox::finished, this, &MainWindow::closeRoutingProgressDialog);
    }

    // OpenFile
    connect(ui->pOpenFile, &QPushButton::clicked, this, &MainWindow::openFile);
    connect(ui->aOpenFile, &QAction::triggered, this, &MainWindow::openFile);

    // LoadExample
    connect(ui->aLoadAdder, &QAction::triggered, this, &MainWindow::loadExampleDiagram);
    connect(ui->aLoadAdderSystem, &QAction::triggered, this, &MainWindow::loadExampleDiagram);
    connect(ui->aLoadAdderTech, &QAction::triggered, this, &MainWindow::loadExampleDiagram);

    // Export
    connect(ui->aExportSchematic, &QAction::triggered, this, &MainWindow::exportSchematicToSvg);
    connect(ui->aExportSelected, &QAction::triggered, this, &MainWindow::exportSelectedToSvg);

    // ZoomIn
    connect(ui->aZoomIn, &QAction::triggered, ui->tabNetlists, &QNetlistTabWidget::zoomIn);
    connect(ui->pZoomIn, &QPushButton::clicked, ui->tabNetlists, &QNetlistTabWidget::zoomIn);

    // ZoomOut
    connect(ui->aZoomOut, &QAction::triggered, ui->tabNetlists, &QNetlistTabWidget::zoomOut);
    connect(ui->pZoomOut, &QPushButton::clicked, ui->tabNetlists, &QNetlistTabWidget::zoomOut);

    // ZoomToFit
    connect(ui->aZoomToFit, &QAction::triggered, ui->tabNetlists, &QNetlistTabWidget::zoomToFit);
    connect(ui->pZoomToFit, &QPushButton::clicked, ui->tabNetlists, &QNetlistTabWidget::zoomToFit);

    // ToggleNames
    connect(ui->aToogleNames, &QAction::triggered, ui->tabNetlists, &QNetlistTabWidget::toggleNames);
    connect(ui->pToggleNames, &QPushButton::clicked, ui->tabNetlists, &QNetlistTabWidget::toggleNames);

    // ClearHighlight
    connect(ui->actionClearHighlight, &QAction::triggered, ui->tabNetlists, &QNetlistTabWidget::clearAllHighlightColors);

    // Search Dialog
    connect(ui->aSearchNode, &QAction::triggered, this, &MainWindow::showSearch);
    connect(ui->pSearchNode, &QPushButton::clicked, this, &MainWindow::showSearch);

    connect(this->dialogSearch, &DialogSearch::finished, this, &MainWindow::closeSearch);
    connect(this->dialogSearch, &DialogSearch::searchText, this->ui->tabNetlists, &QNetlistTabWidget::zoomToNode);

    // Settings Dialog
    connect(ui->aSettings, &QAction::triggered, this, &MainWindow::showSettings);
    connect(this->dialogSettings, &DialogSettings::finished, this, &MainWindow::closeSettings);
    connect(this->dialogSettings, &DialogSettings::symbolsChanged, this, &MainWindow::loadUpdatedSymbols);
    connect(this->dialogSettings, &DialogSettings::routingParametersChanged, ui->tabNetlists, &QNetlistTabWidget::routingParametersChanged);

    // About Dialog
    connect(ui->aAbout, &QAction::triggered, this, &MainWindow::showDialogAbout);
    connect(this->dialogAbout, &DialogAbout::finished, this, &MainWindow::closeDialogAbout);

    connect(ui->tabNetlists, &QNetlistTabWidget::showError, this, &MainWindow::showError);

    // start json parsing
    connect(this, &MainWindow::startJsonParsing, this, &MainWindow::parseJson);

    // create the hierarchy tree
    connect(ui->treeHierarchy, &QTreeView::doubleClicked, this, &MainWindow::clickedOnHierarchyTree);

    // set the hierarchy position when tab changed
    connect(ui->tabNetlists, &QNetlistTabWidget::setHierarchyPos, this, &MainWindow::setHierarchyPos);

    // handle the larger diagrams
    connect(ui->tabNetlists, &QNetlistTabWidget::displayLargeModuleQuestion, this, &MainWindow::showRoutingProgressDialog);
    connect(this, &MainWindow::continueLargeRouting, ui->tabNetlists, &QNetlistTabWidget::largeModuleAccepted);

    ui->treeHierarchy->setVisible(false);

    // set initial routing parameters
    ui->tabNetlists->setRoutingParameters(dialogSettings->getRoutingParameters());

    // disable exit context menu button in wasm to not close the application in the browser
    // leading to a crash
#ifdef EMSCRIPTEN
    ui->aExit->setEnabled(false);
    ui->aExit->setVisible(false);
#endif // EMSCRIPTEN

    QByteArray jsonFile;
    // load the json file if it is not empty
    if(!jsonFilename.isEmpty())
    {
        try
        {
            jsonFile = loadFileCLI(jsonFilename);
        }
        catch(std::runtime_error& e)
        {
            showError(e.what());
            return;
        }
        this->fileContent = jsonFile;
        this->fileName = jsonFilename;
        qInfo() << "Parsing and routing the JSON file: " << jsonFilename;
        qInfo() << "With larger designs this may take a while...";
        emit startJsonParsing();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    ui->tabNetlists->zoomToFit();
}

void MainWindow::openFile()
{

    auto fileContentReady = [this](const QString& fileName, const QByteArray& fileContent) {
        if(fileName.isEmpty())
        {
            qDebug() << "No file selected";
        }
        else
        {
            this->fileName = fileName;
            this->fileContent = fileContent;

            emit startJsonParsing();
        }
    };

    QFileDialog::getOpenFileContent(tr("JSON Files (*.json)"), fileContentReady);
}

void MainWindow::showError(const QString& error)
{
    this->errorMessage = new QMessageBox(this);
    this->errorMessage->setText(error);
    this->errorMessage->setStandardButtons(QMessageBox::Ok);
    this->errorMessage->setModal(true);
    this->errorMessage->setIcon(QMessageBox::Icon::Warning);
    connect(this->errorMessage, &QMessageBox::finished, this, &MainWindow::closeError);
    this->errorMessage->open();
}

void MainWindow::closeError(int result)
{
    Q_UNUSED(result);

    this->errorMessage->close();
    disconnect(this->errorMessage, &QMessageBox::finished, this, &MainWindow::closeError);

    delete this->errorMessage;
    this->errorMessage = nullptr;
}

void MainWindow::showDialogAbout()
{
    this->dialogAbout->show();
}

void MainWindow::closeDialogAbout(int result)
{

    Q_UNUSED(result);

    this->dialogAbout->close();
}

void MainWindow::showSettings()
{
    // get the modules routing parameters of the tab and set them
    const auto routingParams = ui->tabNetlists->getCurrentTabRoutingParameters();
    const auto tabChanged = ui->tabNetlists->getTabChanged();

    this->dialogSettings->setRoutingParameters(routingParams, tabChanged);

    this->dialogSettings->show();
}

void MainWindow::closeSettings(int result)
{
    Q_UNUSED(result);

    this->dialogSettings->close();
}

void MainWindow::showSearch()
{
    this->dialogSearch->show();
}

void MainWindow::closeSearch(int result)
{
    Q_UNUSED(result);

    this->dialogSearch->close();
}

void MainWindow::loadUpdatedSymbols(QByteArray& symbolByteData)
{

    QDomDocument doc;
    doc.setContent(symbolByteData);
    symbolParser.setRootElement(doc.documentElement());

    try
    {
        symbolParser.parse();
    }
    catch(std::runtime_error& e)
    {
        showError(e.what());
    }

    this->symbols = symbolParser.getSymbols();

    ui->tabNetlists->setSymbols(std::make_shared<std::map<QString, std::shared_ptr<Symbol::Symbol>>>(this->symbols));

    // after loading the symbols, reroute the diagram to display the ne symbols
    setNetlisttabDiagramm();
}

void MainWindow::exportSchematicToSvg()
{

    auto svgData = ui->tabNetlists->exportToSvg();

    QString fileName = QFileInfo(this->fileName).baseName();

    fileName += "_export.svg";

    QFileDialog::saveFileContent(svgData, fileName);
}

void MainWindow::exportSelectedToSvg()
{

    auto svgData = ui->tabNetlists->exportToSvg(true);

    QString fileName = QFileInfo(this->fileName).fileName();

    fileName += "_export.svg";

    QFileDialog::saveFileContent(svgData, fileName);
}

void MainWindow::parseJson()
{

    const QJsonDocument jsonDoc = QJsonDocument::fromJson(fileContent);

    if(jsonDoc.isNull())
    {
        showError("Invalid JSON file");
        return;
    }

    // ask if the user wants to remove the loaded diagram if one is loaded
    if(diagramLoaded)
    {
        showAskRemoveLoadedDiagram();
        return;
    }

    // reset and then parse the diagram
    parser.clearDiagram();
    parser.setYosysJsonObject(jsonDoc.object());

    // parse the data
    try
    {
        parser.parse();
        diagram = std::move(parser.getDiagram());
    }
    catch(std::runtime_error& e)
    {
        showError(e.what());
    }

    if(diagram == nullptr)
    {
        return;
    }

    // if no top module is found, show an error message and aboort
    if(diagram->getTopModule() == nullptr)
    {
        showError("The design has no module with the \"top\" attribute.\nYou need to synthesise the design with the \"hierarchy -auto-top\" command");
        return;
    }

    diagramLoaded = true;

    diagram->linkSubModules(diagram->getTopModule());
    createHierarchyTree(diagram->getTopModule());

    // set the window title to the file name
    auto fileName = QFileInfo(this->fileName).fileName();

#ifdef EMSCRIPTEN
    QString script = "document.title = 'OpenNetlistView - " + QFileInfo(this->fileName).fileName() + "'";
    emscripten_run_script(script.toStdString().c_str());
#else
    this->setWindowTitle("OpenNetlistView - " + fileName);
#endif // EMSCRIPTEN

    // route the diagram and display it
    setNetlisttabDiagramm();
}

void MainWindow::showAskRemoveLoadedDiagram()
{

    if(askRemoveDialog == nullptr)
    {
        return;
    }

    askRemoveDialog->open();
}

void MainWindow::closeAskRemoveLoadedDiagram()
{

    if(askRemoveDialog == nullptr)
    {
        return;
    }

    auto result = askRemoveDialog->result();

    if(result == QMessageBox::Yes)
    {
        this->ui->tabNetlists->reset();
        hierarchyModel.clear();
        diagramLoaded = false;
        parseJson();
    }
}

void MainWindow::showRoutingProgressDialog()
{
    if(longRoutingMessage == nullptr)
    {
        return;
    }
    longRoutingMessage->open();
}

void MainWindow::closeRoutingProgressDialog()
{
    if(longRoutingMessage == nullptr)
    {
        return;
    }

    auto result = longRoutingMessage->result();
    if(result == QMessageBox::Yes)
    {
        emit continueLargeRouting();
    }

    longRoutingMessage->close();
}

void MainWindow::createHierarchyTree(const std::shared_ptr<Yosys::Module>& module, QStandardItem* parentItem)
{

    if(module == nullptr)
    {
        return;
    }

    QStandardItem test;
    test.setData(QVariant::fromValue(module));

    // check if we dont have a parent item then we need to create a pparent and start
    // creation of a parent item first
    if(parentItem == nullptr)
    {
        // find the type of the module
        parentItem = new QStandardItem(module->getType());
        parentItem->setData(QVariant::fromValue(module));
        hierarchyModel.appendRow(parentItem);
        hierarchyModel.setHorizontalHeaderLabels({"Module Hierarchy"});
        ui->treeHierarchy->setModel(&hierarchyModel);
    }

    // create a item for each submodule and search for their submodules
    // and call the function again to set the submodules
    for(const auto& [instName, subModule] : module->getSubModules())
    {
        auto* item = new QStandardItem(subModule->getType() + ":" + instName);
        item->setData(QVariant::fromValue(subModule));
        parentItem->appendRow(item);
        createHierarchyTree(subModule, item);
    }
}

void MainWindow::clickedOnHierarchyTree(const QModelIndex& index)
{

    auto* item = hierarchyModel.itemFromIndex(index);

    if(item == nullptr)
    {
        return;
    }

    // get the module
    auto module = item->data().value<std::shared_ptr<Yosys::Module>>();

    if(module == nullptr)
    {
        return;
    }

    // get the module path
    const auto modulePath = createHierarchyModulePath(item);

    // get the instance name
    auto instanceName = item->text();
    // get everything after the :
    instanceName = instanceName.mid(instanceName.indexOf(":") + 1);

    // add the module to the tab
    ui->tabNetlists->addNetlistTab(module, modulePath, instanceName);
}

void MainWindow::setHierarchyPos(const QString& modulePath)
{

    if(modulePath.isEmpty())
    {
        return;
    }

    // generate the module path for each Hierachy View item
    // and check if the module path is equal to the given module path

    auto* item = hierarchyModel.item(0, 0);

    if(item == nullptr)
    {
        return;
    }

    auto* foundItem = searchHierarchyByText(&hierarchyModel, modulePath);

    if(foundItem == nullptr)
    {
        return;
    }

    ui->treeHierarchy->setCurrentIndex(hierarchyModel.indexFromItem(foundItem));

    // expand the tree to the found item not every thing
    ui->treeHierarchy->expandToDepth(hierarchyModel.indexFromItem(foundItem).row());
}

void MainWindow::loadExampleDiagram()
{
    // get the action clicked
    auto* action = qobject_cast<QAction*>(sender());

    if(action == nullptr)
    {
        return;
    }

    // get file by filename in qrc file and then try loading it
    fileName = ":examples/" + action->text();

    try
    {
        fileContent = loadFileCLI(fileName);
    }
    catch(std::runtime_error& e)
    {
        showError(e.what());
        return;
    }

    emit startJsonParsing();
}

void MainWindow::setNetlisttabDiagramm()
{

    // only route and display if the diagram is not null
    if(diagram == nullptr)
    {
        return;
    }

    ui->tabNetlists->setDiagram(std::move(diagram));
}

QString MainWindow::createHierarchyModulePath(QStandardItem* item)
{

    if(item == nullptr)
    {
        return {};
    }

    // get the module in the item
    auto module = item->data().value<std::shared_ptr<Yosys::Module>>();

    if(module == nullptr)
    {
        return {};
    }

    QString modulePath{};

    // if we reached the top module we can stop
    while(item->parent() != nullptr)
    {
        item = item->parent();
        auto parentModule = item->data().value<std::shared_ptr<Yosys::Module>>();

        if(parentModule == nullptr)
        {
            continue;
        }

        // get the instance name from the submodules map
        const auto parentSubModules = parentModule->getSubModules();

        // get the key form the module pointer value
        auto instanceIt = std::find_if(parentSubModules.begin(), parentSubModules.end(), [module](const auto& pair) {
            return pair.second == module;
        });

        if(instanceIt != parentSubModules.end())
        {
            modulePath = instanceIt->first + "/" + modulePath;
        }
    }

    // add the root /
    modulePath = "/" + modulePath;

    return modulePath;
}

QByteArray MainWindow::loadFileCLI(const QString& skinFilename)
{

    QFile file(skinFilename);

    if(!file.open(QIODevice::ReadOnly))
    {
        throw std::runtime_error("Could not open file: " + skinFilename.toStdString());
    }

    return file.readAll();
}

QStandardItem* MainWindow::findItemByText(QStandardItem* parent, const QString& text)
{

    if(parent == nullptr)
    {
        return nullptr;
    }

    if(parent->text() == text)
    {
        return parent;
    }

    for(int i = 0; i < parent->rowCount(); i++)
    {
        auto* found = findItemByText(parent->child(i), text);
        if(found != nullptr)
        {
            return found;
        }
    }

    return nullptr;
}

QStandardItem* MainWindow::searchHierarchyByText(QStandardItemModel* model, const QString& text)
{

    if(model == nullptr)
    {
        return nullptr;
    }

    for(int i = 0; i < model->rowCount(); ++i)
    {
        auto* found = findItemByText(model->item(i), text);
        if(found != nullptr)
        {
            return found;
        }
    }

    return nullptr;
}

} // namespace OpenNetlistView
