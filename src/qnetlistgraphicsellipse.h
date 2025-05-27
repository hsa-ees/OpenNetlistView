/**
 * @file qnetlistgraphicsellipse.h
 * @brief Header file for the QNetlistGraphicsEllipse class, which represents an ellipse item in a QGraphicsScene.
 *
 * This file contains the declaration of the QNetlistGraphicsEllipse class, which is a custom QGraphicsEllipseItem
 * used in the OpenNetlistView namespace. The class provides constructors for creating ellipse items with or without
 * a specified rectangle, and it overrides the paint method to handle custom drawing and selection.
 *
 * @author Lukas Bauer
 */

#ifndef __QNETLISTGRAPHICSELLIPSE_H__
#define __QNETLISTGRAPHICSELLIPSE_H__

#include <QGraphicsEllipseItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

namespace OpenNetlistView {

/**
 * @class QNetlistGraphicsEllipse
 * @brief A class representing an ellipse item in a QGraphicsScene.
 *
 * The QNetlistGraphicsEllipse class is a custom
 * QGraphicsEllipseItem used in the OpenNetlistView namespace. It provides constructors for creating ellipse items
 * with or without a specified rectangle, and it overrides the paint method to handle custom drawing and selection.
 */
class QNetlistGraphicsEllipse : public QGraphicsEllipseItem
{
public:
    /**
     * @brief Constructs a QNetlistGraphicsEllipse object with the specified parent.
     *
     * @param parent The parent of the ellipse.
     */
    QNetlistGraphicsEllipse(QGraphicsItem* parent = nullptr);

    /**
     * @brief Constructs a QNetlistGraphicsEllipse object with the specified rectangle and parent.
     *
     * @param rect The rectangle of the ellipse.
     * @param parent The parent of the ellipse.
     */
    QNetlistGraphicsEllipse(const QRectF& rect, QGraphicsItem* parent = nullptr);

    /**
     * @brief Destructor for the QNetlistGraphicsEllipse class.
     */
    ~QNetlistGraphicsEllipse();

    /**
     * @brief overridden paint method to handel selection
     *
     * @param painter The painter to draw the ellipse.
     * @param option The style option for the ellipse.
     * @param widget The widget to draw the ellipse on.
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    /**
     * @brief Sets the color to use for highlighting the item.
     *
     * @param color The color to use for highlighting the item.
     */
    void setHighlightColor(const QColor& color);

    /**
     * @brief Clears the color used for highlighting the item.
     */
    void clearHighlightColor();

private:
    QColor highlightColor = Qt::transparent; ///< The color to use for highlighting the item.
};

} // namespace OpenNetlistView

#endif // __QNETLISTGRAPHICSELLIPSE_H__