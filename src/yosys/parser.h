/**
 * @file parser.h
 * @brief Header file for the OpenNetlistView::Parser class.
 *
 * This file contains the declaration of the Parser class, which is responsible for parsing
 * Yosys JSON objects and converting them into internal diagram representations.
 *
 * It also defines a namespace YosysJson that contains constants representing key values
 * used in the JSON objects to be parsed. These constants help in identifying and accessing
 * specific parts of the JSON structure.
 *
 * @author Lukas Bauer
 */

#ifndef __PARSER_H__
#define __PARSER_H__

#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QList>

#include <cstdint>

#include "diagram.h"
#include "port.h"

/**
 * @namespace YosysJson
 * @brief This namespace contains key value constants for JSON fields in Yosys JSON files used for parsing.
 */
namespace YosysJson {

constexpr const char* modules{"modules"}; ///< Key for modules field in Yosys JSON.
constexpr const char* ports{"ports"};     ///< Key for ports field in Yosys JSON.
constexpr const char* cells{"cells"};     ///< Key for cells field in Yosys JSON.

constexpr const char* src{"src"};               ///< Key for source field in Yosys JSON.
constexpr const char* attributes{"attributes"}; ///< Key for attributes field in Yosys JSON.
constexpr const char* blackbox{"blackbox"};     ///< Key for blackbox field in Yosys JSON.

constexpr const char* direction{"direction"}; ///< Key for direction field in Yosys JSON.
constexpr const char* bits{"bits"};           ///< Key for bits field in Yosys JSON.

constexpr const char* input_dir{"input"};   ///< Key for input direction in Yosys JSON.
constexpr const char* output_dir{"output"}; ///< Key for output direction in Yosys JSON.
constexpr const char* inout_dir{"inout"};   ///< Key for inout direction in Yosys JSON.

constexpr const char* type{"type"};                       ///< Key for type field in Yosys JSON.
constexpr const char* port_directions{"port_directions"}; ///< Key for port directions field in Yosys JSON.
constexpr const char* connections{"connections"};         ///< Key for connections field in Yosys JSON.

constexpr const char* netnames{"netnames"};   ///< Key for netnames field in Yosys JSON.
constexpr const char* hide_name{"hide_name"}; ///< Key for hide name field in Yosys JSON.

constexpr const char* joinType{"join"};   ///< Key for join type in Yosys JSON.
constexpr const char* splitType{"split"}; ///< Key for split type in Yosys JSON.

constexpr const char* unused_bits{"unused_bits"}; ///< Key for unused bits field in Yosys JSON.

} // namespace YosysJson

namespace OpenNetlistView::Yosys {

// Forward declaration
class Module;

/**
 * @class Parser
 * @brief A class to parse Yosys JSON objects into internal diagram representations.
 */
class Parser
{
private:
    constexpr const static char* constantPortName{"Y"};     ///< The name of the constant port.
    constexpr const static char* constantNodeName{"const"}; ///< The name of the constant node.

public:
    /**
     * @brief Constructor for the Parser class.
     */
    Parser();

    /**
     * @brief Destructor for the Parser class.
     */
    ~Parser();

    /**
     * @brief Sets the Yosys JSON object to be parsed.
     * @param yosysJsonObject The QJsonObject containing Yosys data.
     */
    void setYosysJsonObject(const QJsonObject& yosysJsonObject);

    /**
     * @brief Retrieves a shared pointer to a Diagram object.
     *
     * This function returns a shared pointer that points to a Diagram object.
     * The Diagram object contains the parsed data and structure necessary for
     * further processing or visualization.
     *
     * @return std::shared_ptr<Diagram> A shared pointer to the Diagram object.
     */
    std::unique_ptr<Diagram> getDiagram();

    /**
     * @brief Clears the diagram.
     *
     * This function clears the internal diagram representation.
     */
    void clearDiagram();

    /**
     * @brief Parses the Yosys JSON object.
     *
     * This function processes the Yosys JSON object that has been set using
     * the setYosysJsonObject function. It extracts and processes the relevant
     * data to populate the internal Diagram representation.
     *
     * If the parsing fails, an exception is thrown with an appropriate error message.
     *
     * @throws std::runtime_error if parsing fails.
     */
    void parse();

private:
    QJsonObject yosysJsonObject; ///< The QJsonObject containing Yosys data.
    Diagram diagram;             ///< The internal representation of the diagram.

    std::shared_ptr<Module> currentModule; ///< The current module being processed.

    std::map<QStringList, QStringList> constToNonConstPortBits; ///< Map of constant to non-constant port bits.

    int constCounter = 0; ///< Counter for constant ports.

    /**
     * @brief connects the ports of the components of diagram
     *
     * This function connects the ports of the components of the diagram
     * by collecting all the src and dest ports then connecting the src ports
     * before the dest ports
     */
    void connectDiagramConnections();

    /**
     * @brief Parses the ports from a given JSON object.
     *
     * This function takes a QJsonObject containing port information and processes it accordingly.
     *
     * @param ports A QJsonObject containing the port data to be parsed.
     * @throws std::runtime_error if the parsing fails or the JSON object is invalid.
     */
    void parsePorts(const QJsonObject& ports);

    /**
     * @brief Parses the given JSON object representing cells.
     *
     * This function takes a QJsonObject containing cell data and processes it
     * to extract relevant information about the cells.
     *
     * If the parsing fails, an exception is thrown with an appropriate error message.
     *
     * @param cells A reference to a QJsonObject containing the cell data to be parsed.
     * @throws std::runtime_error if parsing fails.
     */
    void parseCells(const QJsonObject& cells);

    /**
     * @brief Parses the given JSON object to extract and process paths.
     *
     * This function takes a QJsonObject containing path information and processes
     * it accordingly. The exact nature of the processing depends on the implementation
     * details within the function.
     *
     * If the parsing fails, an exception is thrown with an appropriate error message.
     *
     * @param paths A QJsonObject containing the paths to be parsed.
     * @throws std::runtime_error if parsing fails.
     */
    void parseNetnames(const QJsonObject& paths);

    /**
     * @brief Creates a Port object from JSON data.
     * @param name The name of the port.
     * @param bitData The JSON value containing bit data.
     * @param directionData The JSON value containing direction data.
     *
     * @return A shared pointer to the created Port object.
     */
    static std::shared_ptr<Port> createPort(const QString& name, const QJsonValue& bitData, const QJsonValue& directionData);

    /**
     * @brief creates a constant port
     *
     * creates a constant port and the path used to connect it to
     * the port of the node or port and adds them to the diagram
     *
     * @param name the name of the constant port
     * @param bits the bits of the constant port
     * @param constValue the constant value of the port
     *
     * @return the port created
     */
    static std::shared_ptr<Port> createConstantPort(const QString& name, const QStringList& bits, const QStringList& constValue);

    /**
     * @brief splits the bits of a path into segments
     *
     * the segments are created where the bits change from constant to
     * non constant bits and vice versa
     *
     * @param bits the bits to split
     * @return a map with the bit position in the bits list and the value of the parts
     */
    static std::map<std::tuple<uint64_t, uint64_t>, QStringList> splitBits(const QStringList& bits);

    /**
     * @brief creates a split and join object for the given bits
     *
     * the function creates a split and join object for the given bits
     * and adds them to the diagram
     *
     * @param srcPorts the source ports of the connection
     * @param destPorts the destination ports of the connection
     * @param toSolve the bits to solve
     * @param startIdx the start index of the bits to solve
     * @param endIdx the end index of the bits to solve
     * @param splitInfo the split info map
     * @param joinInfo the join info map
     */
    void createSplitJoin(QList<QStringList>& srcPorts,
        QList<QStringList>& destPorts,
        QStringList toSolve,
        int64_t startIdx,
        int64_t endIdx,
        std::map<QStringList, QList<QStringList>>& splitInfo,
        std::map<QStringList, QList<QStringList>>& joinInfo);

    /**
     * @brief checks if a StringList inside the given list contains a certain
     * sub-StringList
     *
     * @param list the list of StringLists to search in
     * @param element the sub-StringList to search for
     * @return index of the found element or -1 if not found
     */
    static int64_t indexOfContains(QList<QStringList> list, QStringList element);

    /**
     * @brief adds a key value pair to a map
     *
     * adds a key value pair to a map if the key is not already in the map
     * otherwise it appends the value to the list of values
     *
     * @param map the map to add the key value pair to
     * @param key the key to add
     * @param value the value to add
     */
    static void addToMap(std::map<QStringList, QList<QStringList>>& map, const QStringList& key, const QStringList& value);

    /**
     * @brief replaces constant bits in the ports with generated bits
     */
    void replaceConstBits();

    /**
     * @brief create splitter nodes for the given split info
     * @param splitInfo the split info map
     */
    void createSplitNodes(const std::map<QStringList, QList<QStringList>>& splitInfo);

    /**
     * @brief create join nodes for the given join info
     * @param joinInfo the join info map
     */
    void createJoinNodes(const std::map<QStringList, QList<QStringList>>& joinInfo);

    /**
     * @brief create signal connections for the current module
     */
    void createSignalConnections();

    /**
     * @brief connect the signal source connections
     * @param srcPorts the source ports
     */
    void connectSignalSrcConnections(std::vector<std::shared_ptr<Port>>& srcPorts);

    /**
     * @brief connect the signal destination connections
     * @param destPorts the destination ports
     */
    void connectSignalDestConnections(std::vector<std::shared_ptr<Port>>& destPorts);

    /**
     * @brief create a path for the given bits
     * @param bits the bits of the path
     * @param sigSource the source of the path
     * @param sigDestinations the destinations of the path
     * @param hiddenName indicates if the name of the path is hidden
     * @param allowSplit indicates if the path can be split
     * @return the created path
     */
    void removeUnconnectedPaths();
};

/**
 * @struct Task
 * @brief A struct to represent a task for the createSplitJoin function.
 */
struct Task
{
    int64_t startIdx;       ///< The start index of the task.
    int64_t endIdx;         ///< The end index of the task.
    QStringList querryBits; ///< The bits to be queried.
};

} // namespace OpenNetlistView::Yosys

#endif // __PARSER_H__