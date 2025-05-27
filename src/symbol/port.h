/**
 * @file port.h
 * @brief Declaration of the Port class used in the OpenNetlistView::Symbol namespace.
 *
 * This file contains the declaration of the Port class, which represents a port
 * with a name and coordinates. For the symbols extracted form svg files
 *
 * @author Lukas Bauer
 */

#ifndef __SYM_PORT_H__
#define __SYM_PORT_H__

#include <QString>
#include <third_party/libcola/cola.h>
#include <third_party/libvpsc/rectangle.h>

#include <ostream>

namespace OpenNetlistView::Symbol {

/**
 * @class Port
 * @brief Represents a port with a name and coordinates.
 */
class Port
{
public:
    constexpr const static double portRectWidth{2.0F};  ///< The width of the port rectangle
    constexpr const static double portRectHeight{2.0F}; ///< The height of the port rectangle

public:
    /**
     * @brief Constructs a Port object with the given name and coordinates.
     *
     * @param name The name of the port.
     * @param xPos The x-coordinate of the port.
     * @param yPos The y-coordinate of the port.
     */
    Port(QString name, double xPos, double yPos);

    /**
     * @brief Destructor for the Port class.
     */
    ~Port();

    /**
     * @brief Gets the name of the port.
     *
     * @return The name of the port.
     */
    const QString& getName() const;

    /**
     * @brief Gets the x-coordinate of the port.
     *
     * @return The x-coordinate of the port.
     */
    double getXPos() const;

    /**
     * @brief Gets the y-coordinate of the port.
     *
     * @return The y-coordinate of the port.
     */
    double getYPos() const;

    /**
     * @brief Generates the cola representation of the port.
     *
     * Generates a rectangle for libcola routing of a default size defined
     *
     * @param rectangles reference to the vector that contains rectangles for libcola routing
     * @param bodyRect the rectangle depicting the bounding box of the symbol
     * @return The name and id of the port.
     */
    std::pair<QString, int> generateColaRep(std::vector<vpsc::Rectangle*>& rectangles, vpsc::Rectangle* bodyRect);

    /**
     * @brief Overloads the << operator to output the port details to an output stream.
     *
     * @param outStream The output stream.
     * @param port The port object to output.
     * @return The output stream with the port details.
     */
    friend std::ostream& operator<<(std::ostream& outStream, const Port& port);

private:
    QString name; ///< The name of the port.
    double xPos;  ///< The x-coordinate of the port.
    double yPos;  ///< The y-coordinate of the port.
};

} // namespace OpenNetlistView::Symbol

#endif // __SYM_PORT_H__