#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QObject>
#include <QtCore/qtypes.h>

#include "qnetlistscene.h"

namespace OpenNetlistView {

QNetlistScene::QNetlistScene(QObject* parent)
    : QGraphicsScene(parent)
{
    connect(this, &QGraphicsScene::selectionChanged, this, &QNetlistScene::onSelectionChanged);
}

QNetlistScene::QNetlistScene(const QRectF& sceneRect, QObject* parent)
    : QGraphicsScene(sceneRect, parent)
{
    connect(this, &QGraphicsScene::selectionChanged, this, &QNetlistScene::onSelectionChanged);
}

QNetlistScene::QNetlistScene(qreal xPos, qreal yPos, qreal width, qreal height, QObject* parent)
    : QGraphicsScene(xPos, yPos, width, height, parent)
{
    connect(this, &QGraphicsScene::selectionChanged, this, &QNetlistScene::onSelectionChanged);
}

QNetlistScene::~QNetlistScene()
{
    // to prevent error during the destruction of the object
    disconnect(this, &QGraphicsScene::selectionChanged, this, &QNetlistScene::onSelectionChanged);
}

void QNetlistScene::onSelectionChanged()
{
    // pass on the selection to the children
    auto selectedItems = this->selectedItems();

    for(auto& item : selectedItems)
    {
        for(auto& child : item->childItems())
        {
            child->setSelected(item->isSelected());
        }
    }
}

} // namespace OpenNetlistView
