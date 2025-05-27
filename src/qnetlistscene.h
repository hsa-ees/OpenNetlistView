/**
 * @file qnetlistscene.h
 * @author Lukas Bauer
 * @brief Header file for the QNetlistScene class, which provides a custom
 * QGraphicsScene for displaying and interacting with netlists.
 */
#ifndef __QNETLISTSCENE_H__
#define __QNETLISTSCENE_H__

#include <QGraphicsScene>
#include <QObject>
#include <QtCore/Qt>

namespace OpenNetlistView {

/**
 * @class QNetlistScene
 * @brief The QNetlistScene class provides a custom QGraphicsScene for displaying and interacting with netlists.
 *
 * This class extends QGraphicsScene to provide additional functionality specific to netlist visualization
 * and interaction. It includes multiple constructors for different initialization scenarios and a destructor
 * for cleanup. The class also includes a private slot to handle selection changes within the scene.
 */
class QNetlistScene : public QGraphicsScene
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new QNetlistScene object
     *
     * @param parent The parent QObject.
     */
    QNetlistScene(QObject* parent = nullptr);

    /**
     * @brief Construct a new QNetlistScene object
     *
     * @param sceneRect The rectangle of the scene.
     * @param parent The parent QObject.
     */
    QNetlistScene(const QRectF& sceneRect, QObject* parent = nullptr);

    /**
     * @brief Construct a new QNetlistScene object
     *
     * @param x The x-coordinate of the scene.
     * @param y The y-coordinate of the scene.
     * @param width The width of the scene.
     * @param height The height of the scene.
     * @param parent The parent QObject.
     */
    QNetlistScene(qreal x, qreal y, qreal width, qreal height, QObject* parent = nullptr);

    /**
     * @brief Destroy the QNetlistScene object
     *
     */
    ~QNetlistScene();

private slots:

    /**
     * @brief Emits the selectionChanged signal when the selection changes.
     *
     */
    void onSelectionChanged();
};

} // namespace OpenNetlistView

#endif // __QNETLISTSCENE_H__