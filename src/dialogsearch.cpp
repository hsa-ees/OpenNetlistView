#include <QDialog>
#include <QWidget>
#include <QtCore/qtmetamacros.h>
#include <QDialogButtonBox>

#include "dialogsearch.h"
#include "ui_dialogsearch.h"

namespace OpenNetlistView {

DialogSearch::DialogSearch(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DialogSearch)
{
    ui->setupUi(this);

    ui->lineENodeName->setFocus();

    connect(ui->dialogButtons, &QDialogButtonBox::accepted, this, &DialogSearch::acceptedSearch);
}

DialogSearch::~DialogSearch()
{
    delete ui;
}

void DialogSearch::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);
    ui->lineENodeName->clear();
}

void DialogSearch::acceptedSearch()
{
    emit searchText(ui->lineENodeName->text());
}

} // namespace OpenNetlistView