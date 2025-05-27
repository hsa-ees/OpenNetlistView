#include <QWidget>
#include <QtCore/Qt>
#include <QGraphicsView>
#include <QScrollBar>
#include <QPointF>
#include <QWheelEvent>
#include <QSvgGenerator>
#include <QGraphicsScene>
#include <QFile>
#include <QMenu>
#include <QBuffer>
#include <QGraphicsTextItem>
#include <QApplication>
#include <QVariant>
#include <QPoint>
#include <QList>
#include <QColor>
#include <QString>
#include <QPixmap>
#include <QIcon>
#include <QGraphicsItem>
#include <QToolTip>

#include <map>
#include <vector>

#include <yosys/node.h>
#include <yosys/component.h>
#include <symbol/symbol.h>

#include <qnetlistgraphicsnode.h>
#include <qnetlistgraphicspath.h>
#include "dialogproperties.h"

#include "qnetlistview.h"

namespace OpenNetlistView {

QNetListView::QNetListView(QWidget* parent)
    : QGraphicsView(parent)
    , nodeContextMenu(new QMenu(this))
    , pathContextMenu(new QMenu(this))
    , propertiesDialog(new DialogProperties(this))
    , selectedItems({})

{
    setDragMode(QGraphicsView::RubberBandDrag);
    setMouseTracking(true);

    // populate the context menu
    this->populateNodeContextMenu();
    this->populatePathContextMenu();
}

QNetListView::~QNetListView() = default;

QByteArray QNetListView::exportToSvg(bool exportSelected)
{
    // export the scene to an svg file
    QSvgGenerator generator;
    QByteArray svgData;
    QBuffer buffer(&svgData);
    buffer.open(QIODevice::WriteOnly);

    // set the metadata for the svg file
    generator.setOutputDevice(&buffer);
    generator.setTitle(tr("Netlist Export"));
    generator.setDescription(tr("Export of the netlist diagram"));
    generator.setSize(this->scene()->sceneRect().size().toSize());

    // render the scene to a BytesArray
    QPainter painter;
    painter.begin(&generator);

    if(exportSelected)
    {
        this->exportSelectedItems(painter);
    }
    else
    {
        // the processEvents is needed to render
        // the scene without the selection
        // otherwise not all selections are cleared before drawing
        this->preserveSelection();
        this->scene()->clearSelection();
        QApplication::processEvents();
        this->scene()->render(&painter);
        this->restoreSelection();
    }

    painter.end();
    buffer.close();

    return svgData;
}

void QNetListView::zoomIn()
{
    scale(scaleFactor, scaleFactor);
}

void QNetListView::zoomOut()
{
    scale(1.0 / scaleFactor, 1.0 / scaleFactor);
}

void QNetListView::zoomToFit()
{
    fitInView(this->scene()->sceneRect(), Qt::KeepAspectRatio);
}

void QNetListView::toggleNames()
{

    // find all the QtGraphicsTextItems and toggle their visibility

    const auto* qtScene = this->scene();

    for(const auto& item : qtScene->items())
    {
        // only change visibility for path names and not other descriptions
        if(dynamic_cast<QGraphicsTextItem*>(item) != nullptr &&
            dynamic_cast<QNetlistGraphicsPath*>(item->parentItem()) != nullptr)
        {
            item->setVisible(!item->isVisible());
        }
    }
}

void QNetListView::zoomToNode(const QString& nodeName)
{
    // find the item with the name and zoom to it
    const auto* qtScene = this->scene();

    for(const auto& item : qtScene->items())
    {
        // only check nodes and ports
        if(dynamic_cast<QNetlistGraphicsNode*>(item) != nullptr)
        {
            const auto* component = dynamic_cast<QNetlistGraphicsNode*>(item);
            const auto yosysComponent = component->getComponent();

            // when the name matches zoom onto the item
            if(yosysComponent->getName().contains(nodeName))
            {
                // zoom so the item is as big as possible
                this->fitInView(item, Qt::KeepAspectRatio);

                for(int i = 0; i < 4; i++)
                {
                    this->zoomOut();
                }

                centerOn(item);
                return;
            }
        }
    }
}

void QNetListView::clearAllHighlightColors()
{
    // find all the items and clear their highlight color
    const auto* qtScene = this->scene();

    // go through all items and clear the highlight color
    for(const auto& item : qtScene->items())
    {
        if(dynamic_cast<QNetlistGraphicsPath*>(item) != nullptr)
        {
            auto* path = dynamic_cast<QNetlistGraphicsPath*>(item);
            path->clearHighlightColor();
        }
        else if(dynamic_cast<QNetlistGraphicsNode*>(item) != nullptr)
        {
            auto* component = dynamic_cast<QNetlistGraphicsNode*>(item);
            component->clearHighlightColor();
        }
    }
}

void QNetListView::wheelEvent(QWheelEvent* event)
{

    // Zoom if ctrl is pressed, scroll horizontally if shift
    // is pressed else use default wheel event
    if((event->modifiers() & Qt::ControlModifier) != 0)
    {
        scrollZoomView(event);
    }
    else if((event->modifiers() & Qt::ShiftModifier) != 0)
    {
        horizontalScroll(event);
    }
    else
    {
        QGraphicsView::wheelEvent(event);
    }
}

void QNetListView::contextMenuEvent(QContextMenuEvent* event)
{
    this->contextMenuPos = event->globalPos();

    // check what item is under the mouse
    QGraphicsItem* item = itemAt(event->pos());

    // convert it to the right type
    if(dynamic_cast<QNetlistGraphicsPath*>(item) != nullptr)
    {
        this->pathContextMenu->move(contextMenuPos);
        this->pathContextMenu->show();
    }
    else if(dynamic_cast<QNetlistGraphicsNode*>(item) != nullptr)
    {
        this->nodeContextMenu->move(contextMenuPos);
        this->nodeContextMenu->show();
    }
}

void QNetListView::mouseMoveEvent(QMouseEvent* event)
{

#ifdef _DEBUG
    // print out the coordinates of the mouse in a tooltip
    // the coordinates of the scene
    const QPointF scenePos = mapToScene(event->pos());

    this->setMouseTracking(true);
    // print them in the tooltip

    QToolTip::showText(event->globalPosition().toPoint(), QString("x: %1, y: %2").arg(scenePos.x()).arg(scenePos.y()));
#endif // _DEBUG

    QGraphicsView::mouseMoveEvent(event);
}

void QNetListView::mouseDoubleClickEvent(QMouseEvent* mouseEvent)
{

    // check what item is under the mouse
    QGraphicsItem* item = itemAt(mouseEvent->pos());

    if(dynamic_cast<QNetlistGraphicsNode*>(item) == nullptr)
    {
        return;
    }

    auto* graphicNode = dynamic_cast<QNetlistGraphicsNode*>(item);

    auto component = graphicNode->getComponent();

    // check if it is an item with a node inside
    if(std::dynamic_pointer_cast<Yosys::Node>(component) != nullptr)
    {
        auto node = std::dynamic_pointer_cast<Yosys::Node>(component);

        // check if the node is a generic module
        if(!SymbolTypes::isValidSymbolType(node->getType()))
        {
            emit genericModuleDoubleClicked(node->getName(), node->getType());
        }
    }
}

void QNetListView::highlightSelectedObject()
{

    const QColor color = getColorFromAction(sender());

    // get the item under the mouse
    QGraphicsItem* item = getItemAtContextMenu();

    // set the color of the item
    if(dynamic_cast<QNetlistGraphicsPath*>(item) != nullptr)
    {
        auto* path = dynamic_cast<QNetlistGraphicsPath*>(item);
        path->setHighlightColor(color);
    }
    else if(dynamic_cast<QNetlistGraphicsNode*>(item) != nullptr)
    {
        auto* component = dynamic_cast<QNetlistGraphicsNode*>(item);
        component->setHighlightColor(color);
    }
}

void QNetListView::clearHighlightSelectedObject()
{
    // get the item under the mouse
    QGraphicsItem* item = getItemAtContextMenu();

    // clear the highlight color of the item
    if(dynamic_cast<QNetlistGraphicsPath*>(item) != nullptr)
    {
        auto* path = dynamic_cast<QNetlistGraphicsPath*>(item);
        path->clearHighlightColor();
    }
    else if(dynamic_cast<QNetlistGraphicsNode*>(item) != nullptr)
    {
        auto* component = dynamic_cast<QNetlistGraphicsNode*>(item);
        component->clearHighlightColor();
    }
}

void QNetListView::contextMenuSelectConnectivity()
{
    // get the item under the mouse
    QGraphicsItem* item = getItemAtContextMenu();
    auto* netlistItem = dynamic_cast<QNetlistGraphicsNode*>(item);

    // get the paths that are connected to the item
    auto connectedItems = netlistItem->getConnectedItems();

    // check if the path items are valid and select them
    for(auto* connectedItem : connectedItems)
    {
        if(dynamic_cast<QNetlistGraphicsPath*>(connectedItem) != nullptr)
        {
            auto* path = dynamic_cast<QNetlistGraphicsPath*>(connectedItem);
            path->setSelected(true);
        }
    }
}

void QNetListView::contextMenuHighlightConnectivity()
{
    const QColor color = getColorFromAction(sender());

    // get the item under the mouse
    QGraphicsItem* item = getItemAtContextMenu();
    auto* netlistItem = dynamic_cast<QNetlistGraphicsNode*>(item);

    // get the paths that are connected to the item
    auto connectedItems = netlistItem->getConnectedItems();

    // check if the path items are valid and highlight them
    for(auto* connectedItem : connectedItems)
    {
        if(dynamic_cast<QNetlistGraphicsPath*>(connectedItem) != nullptr)
        {
            auto* path = dynamic_cast<QNetlistGraphicsPath*>(connectedItem);
            path->setHighlightColor(color);
        }
    }
}

void QNetListView::contextMenuGoToSource()
{

    // get the item under the mouse
    QGraphicsItem* item = getItemAtContextMenu();

    // convert it to a path
    auto* path = dynamic_cast<QNetlistGraphicsPath*>(item);

    if(path == nullptr)
    {
        return;
    }

    // get the source of the path and zoom to it
    auto* qtItem = dynamic_cast<QNetlistGraphicsNode*>(path->getSrcQtItem());

    if(qtItem == nullptr)
    {
        return;
    }

    this->zoomToNode(qtItem->getComponent()->getName());
}

void QNetListView::contextMenuSelectSource()
{

    // get the item under the mouse
    QGraphicsItem* item = getItemAtContextMenu();

    // convert it to a path
    auto* path = dynamic_cast<QNetlistGraphicsPath*>(item);

    if(path == nullptr)
    {
        return;
    }

    // get the source of the path and select it
    auto* qtItem = dynamic_cast<QNetlistGraphicsNode*>(path->getSrcQtItem());

    if(qtItem == nullptr)
    {
        return;
    }

    qtItem->setSelected(true);
}

void QNetListView::contextMenuSelectDestinations()
{
    // get the item under the mouse
    QGraphicsItem* item = getItemAtContextMenu();

    // convert it to a path
    auto* path = dynamic_cast<QNetlistGraphicsPath*>(item);

    if(path == nullptr)
    {
        return;
    }

    // get the destinations of the path and select them
    auto dstItems = path->getDstQtItems();

    for(auto* dstItem : dstItems)
    {
        auto* qtItem = dynamic_cast<QNetlistGraphicsNode*>(dstItem);

        if(qtItem == nullptr)
        {
            continue;
        }

        qtItem->setSelected(true);
    }
}

void QNetListView::contextZoomTo()
{
    // get the item under the mouse
    QGraphicsItem* item = getItemAtContextMenu();

    // get the type of graphics item and get the name

    if(dynamic_cast<QNetlistGraphicsPath*>(item) != nullptr)
    {
        auto* path = dynamic_cast<QNetlistGraphicsPath*>(item);
        this->fitInView(path, Qt::KeepAspectRatio);
    }
    else if(dynamic_cast<QNetlistGraphicsNode*>(item) != nullptr)
    {
        auto* component = dynamic_cast<QNetlistGraphicsNode*>(item);
        this->zoomToNode(component->getComponent()->getName());
    }
}

void QNetListView::contextOpenProperties()
{

    // get the item under the mouse
    QGraphicsItem* item = getItemAtContextMenu();

    // get the properties of the item
    std::vector<std::pair<QString, QString>> properties;

    if(dynamic_cast<QNetlistGraphicsPath*>(item) != nullptr)
    {
        auto* path = dynamic_cast<QNetlistGraphicsPath*>(item);
        properties = path->getProperties();
    }
    else if(dynamic_cast<QNetlistGraphicsNode*>(item) != nullptr)
    {
        auto* component = dynamic_cast<QNetlistGraphicsNode*>(item);
        properties = component->getProperties();
    }

    this->propertiesDialog->setProperties(properties);

    this->propertiesDialog->show();
}

void QNetListView::scrollZoomView(QWheelEvent* event)
{
    // gets the angle in degrees the wheel moved and then calls
    // zoomSpeed to adjust the zoom sensitivity
    const int numDegrees = event->angleDelta().y() / 8;
    const int numSteps = numDegrees / zoomSpeed;

    // get the position of the mouse courser and center the view on that position
    const QPointF mousePosBeforeZoom = mapToScene(event->position().toPoint());

    // zoom in
    if(numSteps > 0)
    {
        scale(scaleFactor, scaleFactor);
    }
    // zoom out
    else if(numSteps < 0)
    {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }

    // get the position of the mouse courser after zoom
    QPointF mousePosAfterZoom = mapToScene(event->position().toPoint());

    // calculate the difference between the scene point before and after zoom
    QPointF posDelta = mousePosAfterZoom - mousePosBeforeZoom;

    // move the view so that the object under the mouse cursor stays in the same position
    centerOn(mapToScene(viewport()->rect().center()) - posDelta);
}

void QNetListView::horizontalScroll(QWheelEvent* event)
{
    // get the angle in degrees the wheel moved and scroll horizontally
    const int delta = event->angleDelta().y() / 8;
    horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta);
}

void QNetListView::populateNodeContextMenu()
{

    if(this->nodeContextMenu == nullptr)
    {
        this->nodeContextMenu = new QMenu(this);
    }

    auto* highlightMenu = new QMenu(tr("Highlight"), this->nodeContextMenu);

    auto highlightColors = createHighlightColors();

    for(auto* action : highlightColors)
    {
        highlightMenu->addAction(action);
        connect(action, &QAction::triggered, this, &QNetListView::highlightSelectedObject);
    }

    this->nodeContextMenu->addMenu(highlightMenu);

    // add the unset highlight color option
    auto* clearHighlightAction = new QAction(tr("Clear Highlight"), this->nodeContextMenu);
    this->nodeContextMenu->addAction(clearHighlightAction);
    connect(clearHighlightAction, &QAction::triggered, this, &QNetListView::clearHighlightSelectedObject);

    // add a separator
    this->nodeContextMenu->addSeparator();

    // add select connectivity
    auto* showConnectivityAction = new QAction(tr("Select Connectivity"), this->nodeContextMenu);
    this->nodeContextMenu->addAction(showConnectivityAction);
    connect(showConnectivityAction, &QAction::triggered, this, &QNetListView::contextMenuSelectConnectivity);

    // add the highlight connectivity menu
    auto* highlightConnectivityMenu = new QMenu(tr("Highlight Connectivity"), this->nodeContextMenu);

    auto highlightConnectivityColors = createHighlightColors();

    for(auto* action : highlightConnectivityColors)
    {
        highlightConnectivityMenu->addAction(action);
        connect(action, &QAction::triggered, this, &QNetListView::contextMenuHighlightConnectivity);
    }

    this->nodeContextMenu->addMenu(highlightConnectivityMenu);

    // add a separator
    this->nodeContextMenu->addSeparator();

    // add zoomTo
    auto* zoomToAction = new QAction(tr("Zoom to"), this->nodeContextMenu);
    this->nodeContextMenu->addAction(zoomToAction);
    connect(zoomToAction, &QAction::triggered, this, &QNetListView::contextZoomTo);

    // add a separator
    this->nodeContextMenu->addSeparator();

    // add the properties option
    auto* propertiesAction = new QAction(tr("Properties..."), this->nodeContextMenu);
    this->nodeContextMenu->addAction(propertiesAction);
    connect(propertiesAction, &QAction::triggered, this, &QNetListView::contextOpenProperties);
}

void QNetListView::populatePathContextMenu()
{

    if(this->pathContextMenu == nullptr)
    {
        this->pathContextMenu = new QMenu(this);
    }

    // create the highlight menu
    auto* highlightMenu = new QMenu(tr("Highlight"), this->pathContextMenu);

    auto colorActions = createHighlightColors();

    for(auto* action : colorActions)
    {
        highlightMenu->addAction(action);
        connect(action, &QAction::triggered, this, &QNetListView::highlightSelectedObject);
    }

    this->pathContextMenu->addMenu(highlightMenu);

    // add the unset highlight color option
    auto* clearHighlightAction = new QAction(tr("Clear Highlight"), this->pathContextMenu);
    this->pathContextMenu->addAction(clearHighlightAction);
    connect(clearHighlightAction, &QAction::triggered, this, &QNetListView::clearHighlightSelectedObject);

    // add a separator
    this->pathContextMenu->addSeparator();

    // add go to signal source
    auto* goToSourceAction = new QAction(tr("Go to Source"), this->pathContextMenu);
    this->pathContextMenu->addAction(goToSourceAction);
    connect(goToSourceAction, &QAction::triggered, this, &QNetListView::contextMenuGoToSource);

    // add select source
    auto* selectSourceAction = new QAction(tr("Select Source"), this->pathContextMenu);
    this->pathContextMenu->addAction(selectSourceAction);
    connect(selectSourceAction, &QAction::triggered, this, &QNetListView::contextMenuSelectSource);

    // add select destinations
    auto* selectDestinationsAction = new QAction(tr("Select Destinations"), this->pathContextMenu);
    this->pathContextMenu->addAction(selectDestinationsAction);
    connect(selectDestinationsAction, &QAction::triggered, this, &QNetListView::contextMenuSelectDestinations);

    // add a separator
    this->pathContextMenu->addSeparator();

    // add zoomTo
    auto* zoomToAction = new QAction(tr("Zoom to"), this->pathContextMenu);
    this->pathContextMenu->addAction(zoomToAction);
    connect(zoomToAction, &QAction::triggered, this, &QNetListView::contextZoomTo);

    // add a separator
    this->pathContextMenu->addSeparator();

    // add the properties option
    auto* propertiesAction = new QAction(tr("Properties..."), this->pathContextMenu);
    this->pathContextMenu->addAction(propertiesAction);
    connect(propertiesAction, &QAction::triggered, this, &QNetListView::contextOpenProperties);
}

std::vector<QAction*> QNetListView::createHighlightColors()
{
    std::vector<QAction*> colorActions;

    // add colors
    for(const auto& [colorName, colorType] : HighlightColors::colors)
    {
        auto colorVariant = QVariant::fromValue(colorType);
        auto colorAction = new QAction(colorName);
        colorAction->setData(colorVariant);

        // add the color as an icon before the action
        QPixmap pixmap(16, 16);
        pixmap.fill(colorType);
        QIcon icon(pixmap);
        colorAction->setIcon(icon);

        colorActions.emplace_back(colorAction);
    }

    return colorActions;
}

QColor QNetListView::getColorFromAction(QObject* object)
{

    // get the source of the signal
    auto* action = qobject_cast<QAction*>(object);

    if(action == nullptr && !action->inherits("QAction"))
    {
        return Qt::transparent;
    }

    auto qData = action->data();

    if(!qData.canConvert<QColor>())
    {
        return Qt::transparent;
    }

    return qData.value<QColor>();
}

QGraphicsItem* QNetListView::getItemAtContextMenu()
{
    const QPoint viewPoint = mapFromGlobal(this->contextMenuPos);

    const QPointF scenePoint = mapToScene(viewPoint);

    return scene()->itemAt(scenePoint, transform());
}

void QNetListView::exportSelectedItems(QPainter& painter)
{

    // Save the current visibility state of all items
    std::map<QGraphicsItem*, bool> visibilityMap;
    QRectF selectedItemsBoundingRect;
    for(auto item : this->scene()->items())
    {
        visibilityMap[item] = item->isVisible();
        if(!item->isSelected())
        {
            item->setVisible(false);
        }
        else
        {
            selectedItemsBoundingRect = selectedItemsBoundingRect.united(item->sceneBoundingRect());
        }
    }

    // the processEvents is needed to render
    // the scene without the selection
    // otherwise not all selections are cleared before drawing
    this->preserveSelection();
    this->scene()->clearSelection();
    QApplication::processEvents();

    // Save the original scene rect
    auto originalSceneRect = this->scene()->sceneRect();

    // shrink the scene to the selected items
    this->scene()->setSceneRect(selectedItemsBoundingRect);

    // render the scene to the painter
    this->scene()->render(&painter);

    // Restore the scene rect
    this->scene()->setSceneRect(originalSceneRect);

    this->restoreSelection();

    // Restore the visibility state of all items
    for(auto [item, value] : visibilityMap)
    {
        item->setVisible(visibilityMap[item]);
    }
}

void QNetListView::preserveSelection()
{
    this->selectedItems.clear();
    for(auto item : this->scene()->selectedItems())
    {
        this->selectedItems.push_back(item);
    }
}

void QNetListView::restoreSelection()
{
    for(auto item : this->selectedItems)
    {
        item->setSelected(true);
    }
}

} // namespace OpenNetlistView