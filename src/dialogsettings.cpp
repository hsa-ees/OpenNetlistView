#include <QWidget>
#include <QDialog>
#include <QFile>
#include <QByteArray>
#include <QIODevice>
#include <QFileDialog>
#include <QObject>
#include <QDialogButtonBox>
#include <QDebug>
#include <qlogging.h>
#include <QtCore/qtmetamacros.h>
#include <QtPreprocessorSupport>
#include <QLoggingCategory>

#include <routing/cola_router.h>

#include "dialogsettings.h"
#include "ui_dialogsettings.h"

namespace OpenNetlistView {

DialogSettings::DialogSettings(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DialogSettings)
{
    ui->setupUi(this);

    connect(ui->pChange, &QPushButton::clicked, this, &DialogSettings::uploadSymbols);
    connect(ui->dialogButtonBox, &QDialogButtonBox::accepted, this, &DialogSettings::acceptedChanges);
    connect(ui->pReset, &QPushButton::clicked, this, &DialogSettings::resetSymbols);

    connect(ui->pRouterReset, &QPushButton::clicked, this, &DialogSettings::resetRoutingParameters);

    // set the default values to the spin boxes
    setDefaultRoutingParameters();
}

DialogSettings::~DialogSettings()
{
    delete ui;
}

QByteArray DialogSettings::getDefaultSymbolData()
{
    QByteArray defaultSymbols;

    QFile file(defaultSymbolsPath);
    if(file.open(QIODevice::ReadOnly))
    {

        defaultSymbols = file.readAll();
        file.close();
    }
    else
    {
        qWarning() << "Could not open default symbol file";
    }

    return defaultSymbols;
}

Routing::ColaRoutingParameters DialogSettings::getRoutingParameters()
{

    Routing::ColaRoutingParameters routingParameters{};

    routingParameters.defaultXConstraint = ui->dSpinXConstraint->value();
    routingParameters.defaultYConstraint = ui->dSpinYConstraint->value();
    routingParameters.testTolerance = ui->dSpinTestToll->value();
    routingParameters.testMaxIterations = ui->spinTestMaxIt->value();
    routingParameters.defaultEdgeLength = ui->dSpinDefEdgeLen->value();

    return routingParameters;
}

void DialogSettings::setCustomSymbolsLoadedCLI()
{
    this->defaultSymbolsLoaded = false;
    this->ui->pReset->setEnabled(true);
}

void DialogSettings::setRoutingParameters(const Routing::ColaRoutingParameters& routingParameters, bool tabChanged)
{
    this->ui->dSpinXConstraint->setValue(routingParameters.defaultXConstraint);
    this->ui->dSpinYConstraint->setValue(routingParameters.defaultYConstraint);
    this->ui->dSpinTestToll->setValue(routingParameters.testTolerance);
    this->ui->spinTestMaxIt->setValue(routingParameters.testMaxIterations);
    this->ui->dSpinDefEdgeLen->setValue(routingParameters.defaultEdgeLength);

    // only set the values for the routing parameters if the tab changed
    if(tabChanged)
    {
        // save the routing parameters
        this->loadedRoutingParameters = routingParameters;
    }
}

void DialogSettings::acceptedChanges()
{
    if(!this->defaultSymbolsLoaded)
    {
        emit symbolsChanged(this->symbolsData);
    }
    else
    {
        auto defaultSymbols = getDefaultSymbolData();
        emit symbolsChanged(defaultSymbols);
    }

    const auto routingParameters = getRoutingParameters();
    emit routingParametersChanged(routingParameters);
}

void DialogSettings::uploadSymbols()
{

    auto fileContentReady = [this](const QString& fileName, const QByteArray& fileContent) {
        if(fileName.isEmpty())
        {
            qDebug() << "No file selected";
        }
        else
        {
            this->symbolsData = fileContent;
            this->defaultSymbolsLoaded = false;
            this->ui->pReset->setEnabled(true);
        }
    };

    QFileDialog::getOpenFileContent(tr("SVG Files (*.svg)"), fileContentReady);
}

void DialogSettings::resetSymbols()
{
    this->defaultSymbolsLoaded = true;
    this->ui->pReset->setEnabled(false);
}

void DialogSettings::resetRoutingParameters()
{
    ui->dSpinXConstraint->setValue(loadedRoutingParameters.defaultXConstraint);
    ui->dSpinYConstraint->setValue(loadedRoutingParameters.defaultYConstraint);
    ui->dSpinTestToll->setValue(loadedRoutingParameters.testTolerance);
    ui->spinTestMaxIt->setValue(loadedRoutingParameters.testMaxIterations);
    ui->dSpinDefEdgeLen->setValue(loadedRoutingParameters.defaultEdgeLength);
}

void DialogSettings::setDefaultRoutingParameters()
{
    ui->dSpinXConstraint->setValue(defaultXConstraint);
    ui->dSpinYConstraint->setValue(defaultYConstraint);
    ui->dSpinTestToll->setValue(defaultTestTolerance);
    ui->spinTestMaxIt->setValue(defaultTestMaxIterations);
    ui->dSpinDefEdgeLen->setValue(defaultEdgeLength);
}

} // namespace OpenNetlistView