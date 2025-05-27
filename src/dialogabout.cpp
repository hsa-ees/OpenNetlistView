#include <QDialog>
#include <QMessageBox>
#include <QWidget>
#include <QString>
#include <QtCore/Qt>
#include <QPixmap>
#include <QLatin1String>

#include <version/version.h>

#include "dialogabout.h"
#include "ui_dialogabout.h"

namespace OpenNetlistView {

DialogAbout::DialogAbout(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DialogAbout)
    , dialogQtAbout(new QMessageBox(this))
{
    ui->setupUi(this);

    ui->lVersion->setText(getDiagViewerVersion());

    QString contributorStr = {};
    for(const auto& contributor : contributors)
    {
        contributorStr += contributor;
        if(contributor != contributors.back())
        {
            contributorStr += "\n";
        }
    }

    ui->lContrib->setText(contributorStr);

    this->setupAboutQt();

    connect(ui->pAboutQt, &QPushButton::clicked, this, &DialogAbout::showDialogQtAbout);
}

DialogAbout::~DialogAbout()
{
    delete ui;
}

void DialogAbout::showDialogQtAbout()
{
    dialogQtAbout->show();
}

void DialogAbout::setupAboutQt()
{
    const QPixmap qtLogoPixmap(QLatin1String(":/trolltech/qmessagebox/images/qtlogo-64.png"));

    dialogQtAbout->setWindowTitle(tr("About Qt"));
    dialogQtAbout->setText(translatedTextAboutQtText);
    dialogQtAbout->setInformativeText(translatedTextAboutQtCaption);

    if(!qtLogoPixmap.isNull())
    {
        dialogQtAbout->setIconPixmap(qtLogoPixmap);
    }

    dialogQtAbout->setStandardButtons(QMessageBox::Ok);
}

} // namespace OpenNetlistView