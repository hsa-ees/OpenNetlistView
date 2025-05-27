#include <QDialog>
#include <QObject>
#include <QWidget>
#include <QString>
#include <QTableWidgetItem>

#include <vector>
#include <utility>

#include "dialogproperties.h"
#include "ui_dialogproperties.h"

namespace OpenNetlistView {

DialogProperties::DialogProperties(const std::vector<std::pair<QString, QString>>& properties,
    QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DialogProperties)
{
    ui->setupUi(this);

    // setup the table
    ui->tableProperties->setColumnCount(static_cast<int>(
        PropertySettings::columnNames.size()));
    ui->tableProperties->setHorizontalHeaderLabels(
        PropertySettings::columnNames);
    ui->tableProperties->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);

    this->setProperties(properties);
}

DialogProperties::DialogProperties(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DialogProperties)
{
    ui->setupUi(this);

    ui->tableProperties->setColumnCount(static_cast<int>(
        PropertySettings::columnNames.size()));
    ui->tableProperties->setHorizontalHeaderLabels(
        PropertySettings::columnNames);
    ui->tableProperties->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
}

DialogProperties::~DialogProperties()
{
    delete ui;
}

void DialogProperties::setProperties(const std::vector<std::pair<QString, QString>>& properties)
{
    this->properties = properties;
    this->updateDisplay();
}

void DialogProperties::addProperty(const QString& key, const QString& value)
{
    this->properties.emplace_back(key, value);
    this->updateDisplay();
}

void DialogProperties::addProperty(const std::pair<QString, QString>& property)
{
    this->properties.emplace_back(property);
    this->updateDisplay();
}

void DialogProperties::clearProperties()
{
    this->properties.clear();
    ui->tableProperties->clearContents();
    ui->tableProperties->setRowCount(0);
}

void DialogProperties::updateDisplay()
{
    ui->tableProperties->clearContents();
    ui->tableProperties->setRowCount(0);

    for(const auto& [key, value] : this->properties)
    {
        ui->tableProperties->insertRow(ui->tableProperties->rowCount());
        ui->tableProperties->setItem(ui->tableProperties->rowCount() - 1,
            0,
            new QTableWidgetItem(key));

        ui->tableProperties->setItem(ui->tableProperties->rowCount() - 1,
            1,
            new QTableWidgetItem(value));
    }
}

} // namespace OpenNetlistView