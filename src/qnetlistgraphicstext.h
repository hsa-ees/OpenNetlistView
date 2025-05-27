/**
 * @file qnetlistgraphicstext.h
 * @brief Header file for the QNetlistGraphicsText class, which represents an text item in a QGraphicsScene.
 *
 * This file contains the declaration of the QNetlistGraphicsText class, which is a custom QGraphicstextItem
 * used in the OpenNetlistView namespace. The class provides constructors for creating text items with or without
 * a specified rectangle, and it overrides the paint method to handle custom drawing and selection.
 *
 * @author Lukas Bauer
 */

#ifndef __QNETLISTGRAPHICSTEXT_H__
#define __QNETLISTGRAPHICSTEXT_H__

#include <QGraphicsTextItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

namespace OpenNetlistView {

/**
 * @class QNetlistGraphicsText
 * @brief A class representing an text item in a QGraphicsScene.
 *
 * The QNetlistGraphicsText class is a custom
 * QGraphicsTextItem used in the OpenNetlistView namespace. It provides constructors for creating text items
 * with or without a specified rectangle, and it overrides the paint method to handle custom drawing and selection.
 */
class QNetlistGraphicsText : public QGraphicsTextItem
{

public:
    /**
     * @brief Constructs a QNetlistGraphicsText object with the specified parent.
     *
     * @param parent The parent of the text.
     */
    QNetlistGraphicsText(QGraphicsItem* parent = nullptr);

    /**
     * @brief Constructs a QNetlistGraphicsText object with the specified rectangle and parent.
     *
     * @param rect the text to display.
     * @param parent The parent of the text.
     */
    QNetlistGraphicsText(const QString& text, QGraphicsItem* parent = nullptr);

    /**
     * @brief Destructor for the QNetlistGraphicsText class.
     */
    ~QNetlistGraphicsText();

    /**
     * @brief overridden paint method to handel selection
     *
     * @param painter The painter to draw the text.
     * @param option The style option for the text.
     * @param widget The widget to draw the text on.
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    /**
     * @brief Sets the color to use for highlighting the item.
     *
     * @param color The color to use for highlighting the item.
     */
    void setHighlightColor(const QColor& color);

    /**
     * @brief Clears the highlight color.
     */
    void clearHighlightColor();

private:
    QColor highlightColor = Qt::transparent; ///< The color to use for highlighting the item.
};

} // namespace OpenNetlistView

#endif // __QNETLISTGRAPHICSText_H__