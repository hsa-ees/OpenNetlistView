/**
 * @file qnetlisttabwidget.h
 * @brief Header file for the QNetlistTabWidget class.
 *
 * This file contains the declaration of the QNetlistTabWidget class,
 * which is a widget for displaying and managing netlist tabs in a graphical user interface.
 *
 * The QNetlistTabWidget class provides functionality to display, manage,
 * and interact with netlist tabs. It allows setting symbols and diagrams,
 * zooming, toggling display options, clearing highlight colors, zooming to
 * specific nodes, exporting to SVG, and adding new netlist tabs.
 *
 * The class inherits from QTabWidget and includes several public slots for
 * user interactions.
 *
 * @author Lukas Bauer
 */

#ifndef __QNETLISTTABWIDGET_H__
#define __QNETLISTTABWIDGET_H__

#include <QTabWidget>
#include <QWidget>
#include <QObject>
#include <QByteArray>

#include <memory>
#include <map>

#include <routing/cola_router.h>

namespace OpenNetlistView {

// forward declaration
class NetlistTab;

namespace Symbol {
class Symbol;
} // namespace Symbol

namespace Yosys {
class Module;
class Diagram;
} // namespace Yosys

/**
 * @class QNetlistTabWidget
 * @brief A widget for displaying and managing netlist tabs in a graphical user interface.
 *
 * The QNetlistTabWidget class provides functionality to display, manage, and interact with netlist tabs.
 * It allows setting symbols and diagrams, zooming, toggling display options, clearing highlight colors,
 * zooming to specific nodes, exporting to SVG, and adding new netlist tabs.
 *
 * The class inherits from QTabWidget and includes several public slots for user interactions.
 */
class QNetlistTabWidget : public QTabWidget
{
    Q_OBJECT

private:
    constexpr const static size_t sizeQuestionThreshold = 200; ///< Threshold when to ask if the user wants to continue routing

    constexpr const static double slopePortObj{0.61F};      ///< The solpe for constraint increas on node ports
    constexpr const static double slopeNodeObj{0.16F};      ///< The slope for constraint increas on node objects
    constexpr const static double slopeEPortObj{0.09F};     ///< The slope for constraint increas on edge ports
    constexpr const static double minConstraint{75.0F};     ///< The minimum constraint value
    constexpr const static double defaultEdgeLength{10.0F}; ///< The default edge length

public:
    /**
     * @brief Construct a new QNetlistTabWidget object
     *
     * @param parent The parent widget.
     */
    QNetlistTabWidget(QWidget* parent = nullptr);

    /**
     * @brief Destroy the QNetlistTabWidget object
     *
     */
    ~QNetlistTabWidget();

    /**
     * @brief Set the symbols to use for creating the diagrams in the tabs
     *
     * @param symbols The symbols to be set.
     */
    void setSymbols(const std::shared_ptr<std::map<QString, std::shared_ptr<Symbol::Symbol>>>& symbols);

    /**
     * @brief Set the diagram containing the modules to be displayed
     *
     * @param diagram The diagram to be set.
     */
    void setDiagram(std::unique_ptr<Yosys::Diagram> diagram);

    /**
     * @brief Set the routing parameters for the widget
     *
     * @param routingParameters The routing parameters to be set.
     */
    void setRoutingParameters(const Routing::ColaRoutingParameters& routingParameters);

    /**
     * @brief Get the routing parameters of the current tab
     */
    Routing::ColaRoutingParameters getCurrentTabRoutingParameters() const;

    /**
     * @brief reset the widget
     *
     * remove the tabs and reset the diagram
     *
     */
    void reset();

public slots:

    /**
     * @brief Slot for zooming in the active tab
     *
     */
    void zoomIn();

    /**
     * @brief Slot for zooming out the active tab
     *
     */
    void zoomOut();

    /**
     * @brief Slot for zooming to fit the active tab
     *
     */
    void zoomToFit();

    /**
     * @brief Slot for toggling the display of names in the active tab
     *
     */
    void toggleNames();

    /**
     * @brief Slot for clearing all highlight colors in the active tab
     *
     */
    void clearAllHighlightColors();

    /**
     * @brief Slot for zooming to a specific node in the active tab
     */
    void zoomToNode(const QString& nodeName);

    /**
     * @brief Slot for exporting the active tab to svg
     *
     * @param exportSelected True if only the selected items should be exported.
     * @return QByteArray The svg data.
     */
    QByteArray exportToSvg(bool exportSelected = false);

    /**
     * @brief Slot for adding a new netlist tab
     *
     * @param moduleName The name of the module.
     * @param moduleType The type of the module.
     */
    void genericModuleDoubleClicked(const QString& moduleName, const QString& moduleType);

    /**
     * @brief Checks if the module is smaller then routes directly otherwise
     * asks the user if the routing should be continued
     *
     * @param module The module to be displayed.
     * @param modulePath The path of the module.
     * @param moduleInstanceName the module path to use
     */
    void addNetlistTab(const std::shared_ptr<Yosys::Module>& module, const QString& modulePath, const QString& moduleInstanceName);

    /**
     * @brief Slot starts the creation of a tab if the
     * module is larger than a certain threshold
     *
     * The size threshold is defined by the sizeQuestionThreshold
     *
     */
    void largeModuleAccepted();

    /**
     * @brief Gets if the tab has changed
     *
     * This resets the value after calling the function
     *
     * @return true if the tab has changed
     *
     */
    bool getTabChanged();

signals:

    /**
     * @brief creates an error message
     *
     * this is escalated to the main window where a message box is created
     *
     */
    void showError(const QString& message);

    /**
     * @brief Set the hierarchy position of the module
     *
     * @param modulePath The path of the module.
     */
    void setHierarchyPos(const QString& modulePath);

    /**
     * @brief Signal to display a question if the user wants to continue with a large module
     *
     * the signal is emitted when the module is larger than a certain threshold
     * it is definded by the sizeQuestionThreshold
     *
     */
    void displayLargeModuleQuestion();

public slots:

    /**
     * @brief slot when the routing parameters have changed.
     *
     * updates the current tab
     *
     * @param routingParameters The new routing parameters.
     */
    void routingParametersChanged(Routing::ColaRoutingParameters routingParameters);

private:
    /**
     * @brief Generate the module path for a new tab
     *
     * @param module The module to be displayed.
     * @param moduleInstanceName The instance name of the module.
     * @return
     */
    QString generateModulePath(const std::shared_ptr<Yosys::Module>& module, const QString& moduleInstanceName);

    /**
     * @brief Create a new netlist tab
     *
     * @param module The module to be displayed.
     * @param modulePath The path of the module.
     * @param moduleInstanceName The instance name of the module.
     */
    void createNetlistTab(const std::shared_ptr<Yosys::Module>& module, const QString& modulePath, const QString& moduleInstanceName);

    /**
     * @brief Calculate the routing parameters for the module
     *
     * This function calculates the routing parameters for the module
     * and sets them to the routing parameters in the instance.
     *
     * @param module The module to be displayed.
     */
    void calculateRoutingParameters(const std::shared_ptr<Yosys::Module>& module);

    std::vector<NetlistTab*> netlistTabs;                                                  ///< Vector of netlist tabs for the widget.
    std::unique_ptr<Yosys::Diagram> diagram = nullptr;                                     ///< The diagram for the widget.
    std::shared_ptr<std::map<QString, std::shared_ptr<Symbol::Symbol>>> symbols = nullptr; ///< Vector of symbols for the widget.
    Routing::ColaRoutingParameters routingParameters;                                      ///< The routing parameters for the widget.

    std::shared_ptr<Yosys::Module> lastModule = nullptr; ///< The last (larger) module that was added to the widget.
    QString lastModulePath;                              ///< The last (larger) module path that was added to the widget.
    QString lastModuleInstanceName;                      ///< The last (larger) module instance name that was added to the widget.

    bool tabChanged = true; ///< Flag to check if the tab has changed.
};

} // namespace OpenNetlistView

#endif // __QNETLISTTABWIDGET_H__