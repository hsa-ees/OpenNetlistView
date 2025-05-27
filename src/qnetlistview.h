/**
 * @file qnetlistview.h
 * @brief Header file for the QNetListView class.
 *
 * This file contains the declaration of the QNetListView class, which is a custom QGraphicsView
 * used for displaying and interacting with netlists in the OpenNetlistView application.
 *
 * @author Lukas Bauer
 */

#ifndef __QNETLISTVIEW_H__
#define __QNETLISTVIEW_H__

#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QByteArray>
#include <QObject>
#include <QtCore/Qt>
#include <QtCore>
#include <QColor>

#include <map>

#include "dialogproperties.h"

namespace OpenNetlistView {

/**
 * @namespace HighlightColors
 * @brief The namespace for the highlight objects
 *
 * This namespace contains the names and color values for highlighting objects in the netlist.
 * These are used in the context menu to highlight objects with different colors.
 */
namespace HighlightColors {

const std::map<QString, QColor> colors = {
    {QObject::tr("Red"), QColor(255, 0, 0)},
    {QObject::tr("Green"), QColor(0, 255, 0)},
    {QObject::tr("Blue"), QColor(0, 0, 255)},
    {QObject::tr("Yellow"), QColor(255, 255, 0)},
    {QObject::tr("Magenta"), QColor(255, 0, 255)},
    {QObject::tr("Orange"), QColor(255, 140, 0)},
    {QObject::tr("Violet"), QColor(128, 0, 128)},
    {QObject::tr("Cyan"), QColor(0, 255, 255)}}; ///< The names and color values for highlighting

} // namespace HighlightColors

/**
 * @class QNetListView
 * @brief A custom QGraphicsView for displaying and interacting with netlists.
 *
 * The QNetListView class provides functionality for zooming, exporting schematics to SVG,
 * toggling the visibility of port names, and zooming to specific nodes within the netlist.
 * It also handles custom wheel and mouse move events to enhance user interaction.
 */
class QNetListView : public QGraphicsView
{

    Q_OBJECT

private:
    constexpr const static int zoomSpeed{1};         ///< Adjust zoom sensitivity
    constexpr const static double scaleFactor{1.15}; ///< Zoom factor

public:
    /**
     * @brief Construct a new QNetListView object
     *
     * @param parent The parent QWidget.
     */
    explicit QNetListView(QWidget* parent = nullptr);

    /**
     * @brief Destroy the QNetListView object
     *
     */
    ~QNetListView();

    /**
     * @brief Export the schematic to SVG.
     *
     * this exports a graphics scene into the svg format.
     * by default the current scene is exported. but only
     * the selected items can be exported
     *
     * @param exportSelected if true only the selected items are exported
     *
     * @return QByteArray The SVG data.
     */
    QByteArray exportToSvg(bool exportSelected = false);

public slots:

    /**
     * @brief Zooms into the diagram.
     *
     */
    void zoomIn();

    /**
     * @brief Zooms out of the diagram.
     *
     */
    void zoomOut();

    /**
     * @brief Zooms to fit the diagram.
     *
     */
    void zoomToFit();

    /**
     * @brief Toggles the visibility of the names of the ports.
     *
     */
    void toggleNames();

    /**
     * @brief Zooms to the node with the given name.
     *
     */
    void zoomToNode(const QString& nodeName);

    /**
     * @brief clears the highlight color of all items
     *
     */
    void clearAllHighlightColors();

signals:

    /**
     * @brief send the data of a generic module as a signal
     *
     * @param moduleName the name of the module
     * @param moduleType the type of the module
     */
    void genericModuleDoubleClicked(const QString& moduleName, const QString& moduleType);

protected:
    /**
     * @brief custom wheel event to add zooming and horizontal scrolling
     *
     * @param event qt wheel event
     */
    void wheelEvent(QWheelEvent* event) override;

    /**
     * @brief custom context menu event to add context menus
     *
     * @param event qt context menu event
     */
    void contextMenuEvent(QContextMenuEvent* event) override;

    /**
     * @brief custom mouse move event to show the mouse position
     *
     * @param event qt mouse event
     */
    void mouseMoveEvent(QMouseEvent* event) override;

    /**
     * @brief custom mouse double click event to handle double clicks
     *
     * @param mouseEvent qt mouse event
     */
    void mouseDoubleClickEvent(QMouseEvent* mouseEvent) override;

private slots:

    /**
     * @brief highlights the selected object
     *
     * this highlights the object that was under the
     * mouse when the context menu was opened
     *
     */
    void highlightSelectedObject();

    /**
     * @brief clears the highlight color of the selected object
     *
     * this clears the highlight color of the object that was under the
     * mouse when the context menu was opened
     *
     */
    void clearHighlightSelectedObject();

    /**
     * @brief selects the connected objects of a port or node
     */
    void contextMenuSelectConnectivity();

    /**
     * @brief highlights the connected objects of a port or node
     */
    void contextMenuHighlightConnectivity();

    /**
     * @brief zooms to the source of a path
     */
    void contextMenuGoToSource();

    /**
     * @brief selects the source of a path
     */
    void contextMenuSelectSource();

    /**
     * @brief selects the destinations of a path
     */
    void contextMenuSelectDestinations();

    /**
     * @brief zooms to the object under the context menu
     */
    void contextZoomTo();

    /**
     * @brief opens the properties dialog for the object under the context menu
     */
    void contextOpenProperties();

private:
    /**
     * @brief handel zooming in and out
     *
     * @param event qt wheel event
     */
    void scrollZoomView(QWheelEvent* event);

    /**
     * @brief handel horizontal scrolling
     *
     * @param event qt wheel event
     */
    void horizontalScroll(QWheelEvent* event);

    /**
     * @brief creates the options for the node context menu
     *
     */
    void populateNodeContextMenu();

    /**
     * @brief creates the options for the path context menu
     *
     */
    void populatePathContextMenu();

    /**
     * @brief creates action elements for the highlight colors
     *
     */
    static std::vector<QAction*> createHighlightColors();

    /**
     * @brief gets the color from the action
     *
     * @param object the object to get the color from
     *
     * @return QColor the color of the action or Qt::transparent if the color could not be converted
     */
    static QColor getColorFromAction(QObject* object);

    /**
     * @brief gets the item under the context menu
     *
     * @return QGraphicsItem* The item under the context menu.
     */
    QGraphicsItem* getItemAtContextMenu();

    /**
     * @brief exports only selected items to SVG
     *
     * @param painter used for generating the SVG data
     */
    void exportSelectedItems(QPainter& painter);

    /**
     * @brief saves which items are selected and saves the selection
     *
     * this also clears the old selection before saving the new one
     */
    void preserveSelection();

    /**
     * @brief restores the selection that was saved
     *
     */
    void restoreSelection();

    QMenu* nodeContextMenu = nullptr; ///< The context menu for nodes.
    QMenu* pathContextMenu = nullptr; ///< The context menu for paths.

    QPoint contextMenuPos; ///< The position of the context menu when it was opened.

    std::vector<QGraphicsItem*> selectedItems; ///< The selected items before the context menu was opened.

    DialogProperties* propertiesDialog = nullptr; ///< The properties dialog for the selected object.
};

} // namespace OpenNetlistView

#endif // __QNETLISTVIEW_H__