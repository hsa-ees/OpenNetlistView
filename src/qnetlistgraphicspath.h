/**
 * @file qnetlistgraphicspath.h
 * @brief Header file for the QNetlistGraphicsPath class.
 *
 * This file contains the definition of the QNetlistGraphicsPath class,
 *  which is used to represent and manipulate graphical paths in a netlist.
 *
 * @author Lukas Bauer
 */

#ifndef __QNETLISTGRAPHICPATH_H__
#define __QNETLISTGRAPHICPATH_H__

#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QGraphicsItem>

#include <memory>
#include <tuple>

#include "qnetlistgraphicsellipse.h"
#include "qnetlistgraphicstext.h"

namespace Avoid {
class ConnRef;
}

namespace OpenNetlistView {

namespace Yosys {
class Path;
}

/**
 * @class QNetlistGraphicsPath
 * @brief A class to represent and manipulate graphical paths in a netlist.
 *
 * The QNetlistGraphicsPath class provides methods to construct and manipulate
 * graphical paths, including setting and getting the yosys path,
 * adding text ports, and placing diverging points. It also handles the
 *  painting of the path and its associated text.
 */
class QNetlistGraphicsPath : public QGraphicsPathItem
{

private:
    constexpr const static double textScale{0.5F};                                 ///< the scale of the text at the path ends
    constexpr const static int fontSize{10};                                       ///< the font size used for the path names
    constexpr const static double lineSelectionWidth{5.0F};                        ///< the width of the line when selected
    constexpr const static double divergingPointSignalRadius{2.0F};                ///< the radius of the diverging point signal
    constexpr const static double divergingPointBusRadius{4.0F};                   ///< the radius of the diverging point bus
    constexpr const static char* propertyValueType{"path"};                        ///< the type of the path in the properties dialog
    constexpr const static char* propertyTypeType{"Type:"};                        ///< the type of the path in the properties dialog
    constexpr const static char* propertyTypeName{"Name:"};                        ///< the name of the path in the properties dialog
    constexpr const static char* propertyTypeNeighbors{"Number of destinations:"}; ///< the neighbors of the path in the properties dialog
    constexpr const static char* propertyTypeSrcName{"Source:"};                   ///< the name of the source in the properties dialog
    constexpr const static char* propertyTypeDstName{"Destination:"};              ///< the name of the destination in the properties dialog

public:
    /**
     * @brief Constructs a QNetlistGraphicsPath object with the specified parent.
     *
     * @param parent The parent of the path.
     */
    QNetlistGraphicsPath(QGraphicsItem* parent = nullptr);

    /**
     * @brief Constructs a QNetlistGraphicsPath object with the specified painter path and parent.
     *
     * @param painter The painter path of the path.
     * @param parent The parent of the path.
     */
    QNetlistGraphicsPath(const QPainterPath& painter,
        QGraphicsItem* parent = nullptr);

    /**
     * @brief Constructs a QNetlistGraphicsPath object with the specified yosys path, painter path, and parent.
     *
     * @param path The yosys path of the path.
     * @param painter The painter path of the path.
     * @param parent The parent of the path.
     */
    QNetlistGraphicsPath(const std::shared_ptr<Yosys::Path>& path,
        const QPainterPath& painter,
        QGraphicsItem* parent = nullptr);

    /**
     * @brief Destroy the QNetlistGraphicsPath object
     *
     */
    ~QNetlistGraphicsPath();

    /**
     * @brief Gets the yosys path of the path.
     *
     * @return The yosys path of the path.
     */
    std::shared_ptr<Yosys::Path> getYosysPath() const;

    /**
     * @brief Sets the yosys path of the path.
     *
     * @param path The yosys path to set.
     */
    void setYosysPath(const std::shared_ptr<Yosys::Path>& path);

    /**
     * @brief Gets the shape of the path.
     *
     * @return The shape of the path.
     */
    QPainterPath shape() const override;

    /**
     * @brief Sets the source text port of the path.
     *
     * @param pos The position of the source text port.
     */
    void setSrcTextPort(const QPointF& pos);

    /**
     * @brief Adds a destination text port to the path.
     *
     * @param pos The position of the destination text port.
     */
    void addDstTextPort(const QPointF& pos, Avoid::ConnRef* avoidRef);

    /**
     * @brief Adds a diverging point to the path.
     *
     * @param pos The position of the diverging point.
     */
    void addDivergingPoint(const QPointF& pos);

    /**
     * @brief Set the color to highlight the object with
     *
     * @param color the color to use in highlighting
     */
    void setHighlightColor(const QColor& color);

    /**
     * @brief Clear the highlight color
     */
    void clearHighlightColor();

    /**
     * @brief Get the Qt object that depicts the source of the path
     *
     * @return pointer to the Qt object
     */
    QGraphicsItem* getSrcQtItem() const;

    /**
     * @brief Get the Qt objects that depict the destination of the path
     *
     * @return vector of pointers to the Qt objects
     */
    std::vector<QGraphicsItem*> getDstQtItems() const;

    /**
     * @brief Get the properties of the path
     *
     * These properties are displayed in the properties dialog
     *
     * @return vector of pairs of strings
     */
    std::vector<std::pair<QString, QString>> getProperties();

protected:
    /**
     * @brief Places the text at the path ends.
     *
     */
    void paint(QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget) override;

private:
    /**
     * @brief Places the text at the path ends.
     *
     */
    void placePathText();

    /**
     * @brief Creates a text item at the specified position.
     *
     * @param text The text to create.
     * @param pos The position to create the text at.
     * @param isDst A flag indicating if the text is a destination.
     */
    void createTextItem(const QString& text,
        const QPointF& pos,
        bool isDst = false);

    /**
     * @brief Places the diverging points on the path.
     *
     */
    void placeDivergingPoints();

    std::shared_ptr<Yosys::Path> yosysPath;                           ///< The yosys path of the path.
    QPointF srcTextPos;                                               ///< The position of the source text.
    std::vector<std::tuple<QPointF, Avoid::ConnRef*>> dstTextPosList; ///< The list of destination text positions.
    std::vector<QNetlistGraphicsText*> pathTextItems;                 ///< The list of path text items.
    std::vector<QPointF> divergingPoints;                             ///< The list of diverging points.
    std::vector<QNetlistGraphicsEllipse*> divergingPointsSymbols;     ///< The list of diverging point symbols.

    QColor highlightColor = Qt::transparent; ///< The color to use for highlighting the item.
};

} // namespace OpenNetlistView

#endif // __QNETLISTGRAPHICPATH_H__