/**
 * @file path.h
 * @brief Header file for the Path class in the OpenNetlistView namespace.
 *
 * This file contains the declaration of the Path class, which represents a path in the OpenNetlistView application.
 * The Path class inherits from the Component class and provides methods to manipulate and access path properties.
 *
 * @author Lukas Bauer
 */

#ifndef __PATH_H__
#define __PATH_H__

#include <QString>
#include <QStringList>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QVariantList>
#include <third_party/libavoid/connector.h>
#include <third_party/libavoid/geomtypes.h>

#include <vector>
#include <cstdint>

#include <qnetlistgraphicspath.h>
#include "component.h"

namespace OpenNetlistView::Yosys {

class Port;

/**
 * @class Path
 * @brief Represents a path in the OpenNetlistView application.
 *
 * The Path class provides methods to set and get path properties such as width and ports,
 * and to access neighboring nodes.
 */
class Path : public Component
{
private:
    constexpr const static double lineStrength{0.5F};    ///< the strength of the line (not a bus)
    constexpr const static double busLineStrength{2.0F}; ///< the strength of the line (bus)

public:
    /**
     * @brief Constructs a Path object with the specified name, width, bits, and neighboring nodes.
     *
     * @param name The name of the path.
     * @param width The width of the path.
     * @param bits A list containing the bits of the path.
     * @param sigSource Reference to the signal source.
     * @param sigDestinations Reference to the signal destinations.
     */
    Path(QString name, const uint64_t width, QStringList bits, std::shared_ptr<Port>& sigSource, std::vector<std::shared_ptr<Port>>& sigDestinations, bool hiddenName = false);

    /**
     * @brief Constructs a Path object with the specified name and bits.
     *
     * @param name The name of the path.
     * @param bits A list containing the bits of the path.
     */
    Path(QString name, QStringList bits, bool hiddenName = false);

    /**
     * @brief Destructor for the Path class.
     */
    ~Path();

    /**
     * @brief Sets the source signal of the path.
     *
     * @param sigSource The source signal.
     * @param allowOverwrite Flag indicating if the source signal can be overwritten
     * if it is already set. Default is false.
     */
    void setSigSource(const std::shared_ptr<Port>& sigSource, bool allowOverwrite = false);

    /**
     * @brief Adds a signal destination to the path.
     *
     * @param sigDestination The signal destination to be added.
     */
    void addSigDestination(const std::shared_ptr<Port>& sigDestination);

    /**
     * @brief Removes a signal destination from the path.
     *
     * @param sigDestination The signal destination to be removed.
     */
    void removeSigDestination(const std::shared_ptr<Port>& sigDestination);

    /**
     * @brief Adds a relationship between the connection reference and the connected ports of the path.
     *
     * @param avoidConnRef The connection reference to be added.
     * @param colaDestID The ID of the destination port.
     */
    void addAvoidPortRelation(Avoid::ConnRef* avoidConnRef, const int colaDestID);

    /**
     * @brief Sets the width of the path.
     *
     * @param width The new width of the path.
     */
    void setWidth(uint64_t width);

    /**
     * @brief Sets the flag indicating if the path can be split.
     *
     * @param allowSplit The flag indicating if the path can be split.
     */
    void setAllowSplit(bool allowSplit);

    /**
     * @brief indicates if the path can be split
     *
     * @return true if the path can be split, false otherwise
     */
    bool getAllowSplit() const;

    /**
     * @brief Gets the ports of the path.
     *
     * @return A reference to a vector of integers representing the ports of the path.
     */
    QStringList& getBits();

    /**
     * @brief Checks if the path is a bus.
     *
     * @return true if the path is a bus, false otherwise.
     */
    bool isBus() const;

    /**
     * @brief Checks if the name of the path is hidden.
     *
     * @return true if the name of the path is hidden, false otherwise.
     */
    bool isNameHidden() const;

    /**
     * @brief Gets the source signal.
     *
     * @return A shared pointer to the source signal.
     */
    std::shared_ptr<Port> getSigSource();

    /**
     * @brief Gets the signal destinations.
     *
     * @return A shared pointer to the signal destinations.
     */
    std::shared_ptr<std::vector<std::shared_ptr<Port>>> getSigDestinations();

    /**
     * @brief adds a connection reference to the path
     *
     * these references can be used to create qt items
     *
     * @param avoidConnRef the connection reference to add
     */
    void addAvoidConnRef(Avoid::ConnRef* avoidConnRef);

    /**
     * @brief sets the connection references for the path
     *
     * these references can be used to create qt items
     *
     * @param avoidConnRefs the connection references to set
     */
    void setAvoidConnRefs(std::vector<Avoid::ConnRef*> avoidConnRefs);

    /**
     * @brief gets the connection references for the path
     *
     * these references can be used to create qt items
     *
     * @return std::vector<Avoid::ConnRef*> the connection references
     */
    std::vector<Avoid::ConnRef*> getAvoidConnRefs();

    /**
     * @brief checks if the path has connections
     *
     * checks if the path has one source and at least one destination
     *
     * @return true if the path has connections, false otherwise
     */
    bool hasConnection() const;

    /**
     * @brief checks if the path has any constant bits
     *
     * @return true if the path has constant bits, false otherwise
     */
    bool hasConstBits() const;

    /**
     * @brief checks if the path has no connection bits
     *
     * @return true if the path has no connection bits, false otherwise
     */
    bool hasNoConnectBitsConnection() const;

    /**
     * @brief Adds an alternative name to the path.
     *
     * @param name The alternative name to be added.
     */
    void addAlternativeName(const QString& name);

    /**
     * @brief Gets the alternative names of the path.
     *
     * @return A shared pointer to a QString containing the alternative names of the path.
     */
    std::vector<std::shared_ptr<QString>>& getAlternativeNames();

    /**
     * @brief checks first if the bits are equal and then searches if they are partly equal
     *
     * @param bits the bits to compare to
     */
    bool partialBitsMatch(const QStringList& bits) const;

    /**
     * @brief Converts the path to a Qt path.
     *
     * @return A pointer to the Qt path.
     */
    QNetlistGraphicsPath* convertToQt();

    /**
     * @brief remove the routing data from the path
     *
     * This resets the cola and avoid routing data
     */
    void clearRoutingData();

    /**
     * @brief Generates the label text for the path.
     *
     * If the paths source connects to a split or a destination connects to a join
     * the bit range of the split or join is returned.
     * Otherwise the name of the path plus it's width is returned if the
     * name is not hidden.
     *
     * @param avoidRef The connection reference for the path.
     * @return The label text for the path.
     */
    QString generateLabelText(Avoid::ConnRef* avoidRef = nullptr) const;

    /**
     * @brief Overloaded stream insertion operator for the Path class.
     *
     * @param outputStream The output stream.
     * @param path The Path object to be inserted into the stream.
     * @return A reference to the output stream.
     */
    friend std::ostream& operator<<(std::ostream& outputStream, const Path& path);

private:
    uint64_t width;                                                      ///< The width of the path.
    QStringList bits;                                                    ///< A list containing the bits of the path.
    std::shared_ptr<Port> sigSource;                                     ///< Shared pointer to the source of the signal.
    std::shared_ptr<std::vector<std::shared_ptr<Port>>> sigDestinations; ///< Shared pointer to the right neighboring node.
    bool hiddenName;                                                     ///< Indicates whether the name of the path is hidden.
    std::vector<std::shared_ptr<QString>> alternativeNames;              ///< A vector of alternative names for the path.
    std::vector<Avoid::ConnRef*> avoidConnRefs;                          ///< The connection reference for the path.
    std::map<Avoid::ConnRef*, std::shared_ptr<Port>> avoidPortRefs;      ///< Contains a relationship between the connections begin and end and the connected ports of the path.

    /**
     * @brief Creates a QPainterPath from an Avoid PolyLine.
     *
     * @param avoidPath The Avoid PolyLine to be converted.
     * @param qPathItem The QNetlistGraphicsPath item to be used for the conversion.
     * @return The QPainterPath created from the Avoid PolyLine.
     */
    static QPainterPath createPainterPath(Avoid::ConnRef& connRef,
        QNetlistGraphicsPath* qPathItem);

    /**
     * @brief Finds the end of the intersection of two QPainterPaths.
     *
     * This is the point where the to port diverge from each other.
     *
     * @param firstPath The first QPainterPath.
     * @param secondPath The second QPainterPath.
     * @return The end of the intersection of the two QPainterPaths.
     */
    static QPointF findEndOfIntersection(const QPainterPath& firstPath,
        const QPainterPath& secondPath);

    /**
     * @brief Create a path that is a subset of the path object
     *
     * @param bits the bits to use
     * @return the created path
     */
    std::shared_ptr<Path> createSubPath(const QStringList& bits, std::vector<std::shared_ptr<Path>> existingPaths) const;
};

} // namespace OpenNetlistView::Yosys

#endif // __PATH_H__