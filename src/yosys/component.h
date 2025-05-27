/**
 * @file component.h
 * @brief Header file for the Component class in the OpenNetlistView namespace.
 *
 * This file contains the definition of the Component class, which represents
 * a component with a name in the OpenNetlistView application.
 *
 * @author Lukas Bauer
 */

#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <QString>

namespace OpenNetlistView::Yosys {

/**
 * @class Component
 * @brief Represents a component with a name in the OpenNetlistView application.
 */
class Component
{
public:
    /**
     * @brief Constructs a Component with the given name.
     * @param name The name of the component.
     */
    Component(QString name);

    /**
     * @brief Destructor for the Component class.
     */
    virtual ~Component();

    /**
     * @brief Gets the name of the component.
     * @return The name of the component.
     */
    QString getName() const;

    /**
     * @brief Sets the name of the component.
     * @param name The new name of the component.
     */
    void setName(const QString& name);

    /**
     * @brief sets the pointer to the qt graphics item that represents the component
     * @param item The pointer to the qt graphics item
     */
    void setGraphicsItem(QGraphicsItem* item);

    /**
     * @brief returns the components graphics item
     * @return The pointer to the qt graphics item
     */
    QGraphicsItem* getGraphicsItem() const;

private:
    QString name;                ///< The name of the component.
    QGraphicsItem* graphicsItem; ///< The graphics item that represents the component.
};

} // namespace OpenNetlistView::Yosys

#endif // __COMPONENT_H__