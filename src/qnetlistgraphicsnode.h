

/**
 * @file qnetlistgraphicsnode.h
 * @brief Header file for the QNetlistGraphicsItem class, which represents a
 * graphical item for displaying netlist components.
 *
 * This file contains the declaration of the QNetlistGraphicsItem
 * class, which is used to display netlist components as SVG items in a QGraphicsView.
 *
 * The class provides methods for setting and getting the component
 * to be displayed, as well as painting the item and creating text items for ports.
 *
 * The QNetlistGraphicsItem class inherits from QGraphicsSvgItem
 * and overrides the paint method to customize the painting of the item.
 *
 * The class also includes private methods for setting the
 * text path and creating text items for ports.
 *
 * The component to be displayed is stored as a shared pointer
 * to a Yosys::Component object, and the text items for the path
 * are stored in a vector of QGraphicsTextItem pointers.
 *
 * @author Lukas Bauer
 */
#ifndef __QNETLISTGRAPICNODE_H__
#define __QNETLISTGRAPICNODE_H__

#include <QGraphicsSvgItem>
#include <QGraphicsItem>
#include <QPainter>

#include <memory>

#include <yosys/component.h>
#include <symbol/port.h>
#include "qnetlistgraphicstext.h"

namespace OpenNetlistView {

namespace Yosys {
class Port;
class Component;
} // namespace Yosys

/**
 * @class OpenNetlistView::QNetlistGraphicsNode
 * @brief A class representing a graphical item for displaying netlist components in a diagram viewer.
 *
 * This class inherits from QGraphicsSvgItem and provides functionality to display and manage
 * netlist components as SVG items within a QGraphicsView. It supports setting and getting the
 * component to be displayed, as well as painting the item and creating text items for ports.
 */
class QNetlistGraphicsNode : public QGraphicsSvgItem
{

private:
    constexpr const static char* propertyTypeType{"Type:"};                          ///< the type of the item in the properties dialog
    constexpr const static char* propertyTypeName{"Name:"};                          ///< the name of the item in the properties dialog
    constexpr const static char* propertyTypeNeighbors{"Number of destinations:"};   ///< the neighbors of the item in the properties dialog
    constexpr const static char* propertyTypeDirection{"Direction:"};                ///< the direction of the port in the properties dialog
    constexpr const static char* propertyTypePathName{"Connected Path:"};            ///< the name of the connected path in the properties dialog
    constexpr const static char* propertyTypeConstValue{"Value:"};                   ///< the value of the constant port in the properties dialog
    constexpr const static char* propertyTypeNodeInputAmount{"Number of inputs:"};   ///< the number of inputs of the node in the properties dialog
    constexpr const static char* propertyTypeNodeOutputAmount{"Number of outputs:"}; ///< the number of outputs of the node in the properties dialog
    constexpr const static char* propertyTypeNodeInputName{"Input:"};                ///< the name of the input path in the properties dialog
    constexpr const static char* propertyTypeNodeOutputName{"Output:"};              ///< the name of the output path in the properties dialog

    constexpr const static char* propertyValuePortType{"port"};       ///< the type of the port in the properties dialog
    constexpr const static char* propertyValuePortInput{"INPUT"};     ///< the input direction of the port in the properties dialog
    constexpr const static char* propertyValuePortOutput{"OUTPUT"};   ///< the output direction of the port in the properties dialog
    constexpr const static char* propertyValuePortConst{"CONST"};     ///< the constant direction of the port in the properties dialog
    constexpr const static char* propertyValuePortUnknown{"UNKNOWN"}; ///< the unknown direction of the port in the properties dialog

    constexpr const static int hexBase{16}; ///< the base for displaying the value of a const port as hex

    constexpr const static int fontSize{10};     ///< the font size used for the port names
    constexpr const static float fontScale{0.5}; ///< the scale of the font for the port names

public:
    /**
     * @brief Construct a new QNetlistGraphicsItem object
     *
     * @param parent The parent QGraphicsItem.
     */
    QNetlistGraphicsNode(QGraphicsItem* parent = nullptr);

    /**
     * @brief Construct a new QNetlistGraphicsItem object
     *
     * @param component The component to be displayed.
     * @param parent The parent QGraphicsItem.
     */
    QNetlistGraphicsNode(const std::shared_ptr<Yosys::Component>& component,
        QGraphicsItem* parent = nullptr);

    /**
     * @brief Destroy the QNetlistGraphicsItem object
     *
     */
    ~QNetlistGraphicsNode();

    /**
     * @brief Get the Component object
     *
     * @return std::shared_ptr<Yosys::Component> The component to be displayed.
     */
    std::shared_ptr<Yosys::Component> getComponent() const;

    /**
     * @brief Set the Component object
     *
     * @param component The component to be displayed.
     */
    void setComponent(const std::shared_ptr<Yosys::Component>& component);

    /**
     * @brief Set the highlight color for the item.
     *
     * @param color The color to use for highlighting the item.
     */
    void setHighlightColor(const QColor& color);

    /**
     * @brief Clear the highlight color for the item.
     *
     */
    void clearHighlightColor();

    /**
     * @brief get the connected qt path items
     *
     * @return a set of pointers ot the connected qt path items
     */
    std::vector<QGraphicsItem*> getConnectedItems();

    /**
     * @brief Get the properties of the item.
     *
     * @return std::vector<std::pair<QString, QString>> The properties of the item.
     */
    std::vector<std::pair<QString, QString>> getProperties();

protected:
    /**
     * @brief Paints the item as a SVG item.
     *
     * @param painter The painter to use for painting.
     * @param option The style options for the item.
     * @param widget The widget to paint on.
     */
    void paint(QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget) override;

private:
    /**
     * @brief Sets the text path for the component.
     *
     */
    void setTextPath();

    /**
     * @brief Creates a text item for the port.
     *
     * @param text The text to display.
     * @param topOrBottom Whether the text item should be placed on the top or bottom.
     */
    void createPortTextItem(const QString& text,
        bool topOrBottom = false);

    void createGenericPortTextItem(const std::shared_ptr<Yosys::Port>& port,
        const std::shared_ptr<Symbol::Port>& symbolPort);

    /**
     * @brief Get the Node Properties object
     *
     * @return the properties of the node
     */
    std::vector<std::pair<QString, QString>> getNodeProperties();

    /**
     * @brief Get the Port Properties object
     *
     * @return the properties of the port
     */
    std::vector<std::pair<QString, QString>> getPortProperties();

    /**
     * @brief Format the value of a constant port.
     *
     * gets the constant value of the port and formats it
     * when the port is only one bit wide it is in decimal
     * otherwise it is in hex
     *
     * @param port The constant port to get the value of.
     * @return QString The formatted value of the constant port.
     */
    static QString getFormatConstPortValue(const std::shared_ptr<Yosys::Port>& port);

    std::shared_ptr<Yosys::Component> component;      ///< The component to be displayed.
    std::vector<QNetlistGraphicsText*> nodeTextItems; ///< The text items for the paths

    QColor highlightColor = Qt::transparent; ///< The color to use for highlighting the item.
};

} // namespace OpenNetlistView

#endif // __QNETLISTGRAPICNODE_H__