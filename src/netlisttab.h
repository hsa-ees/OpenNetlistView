

/**
 * @file netlisttab.h
 * @author Lukas Bauer
 * @brief Header file for the NetlistTab class, which provides a user interface
 * for displaying and interacting with a netlist module.
 */

#ifndef __NETLISTTAB_H__
#define __NETLISTTAB_H__

#include <QWidget>
#include <QObject>
#include <QString>
#include <QByteArray>

#include <memory>
#include <map>

#include <routing/router.h>
#include <symbol/symbol.h>
#include <yosys/module.h>
#include <routing/cola_router.h>

namespace OpenNetlistView {

// forward declaration
class QNetlistScene;

QT_BEGIN_NAMESPACE
namespace Ui {
class NetlistTab;
}
QT_END_NAMESPACE

/**
 * @class NetlistTab
 * @brief A widget for displaying and interacting with a netlist module.
 *
 * The NetlistTab class provides a user interface for displaying a netlist module
 * and its associated symbols. It allows for upgrading the display and clearing
 * routing data.
 */

class NetlistTab : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new Netlist Tab object
     *
     * @param module The module to be displayed in the tab.
     * @param symbols The symbols used for display.
     * @param modulePath The path of the module in the design.
     * @param routingParameters The routing parameters for the module.
     * @param parent The parent widget.
     */
    NetlistTab(const std::shared_ptr<Yosys::Module>& module,
        const std::shared_ptr<std::map<QString, std::shared_ptr<Symbol::Symbol>>>& symbols,
        const QString& modulePath,
        const Routing::ColaRoutingParameters& routingParameters,
        QWidget* parent = nullptr);

    /**
     * @brief Destroy the Netlist Tab object
     *
     */
    ~NetlistTab();

    /**
     * @brief Upgrade the display
     *
     */
    void upgradeDisplay();

    /**
     * @brief Clear the routing data
     *
     */
    void clearRoutingData();

    /**
     * @brief Set the path of the module in the design
     *
     * @param modulePath The path of the module.
     */
    void setModulePath(const QString& modulePath);

    /**
     * @brief Get the path of the module in the design
     *
     * @return The path of the module.
     */
    QString getModulePath() const;

    /**
     * @brief update the symbols for drawing the netlist
     *
     * @param symbols the updated symbols
     */
    void updateSymbols(const std::shared_ptr<std::map<QString, std::shared_ptr<Symbol::Symbol>>>& symbols);

    /**
     * @brief recievs the changed routing parameters and sends them to the router
     */
    void routingParametersChanged(const Routing::ColaRoutingParameters& routingParameters);

    /**
     * @brief get the current routing parameters
     */
    Routing::ColaRoutingParameters getRoutingParameters();

signals:

    /**
     * @brief Signal for zooming into the scene
     *
     */
    void zoomIn();

    /**
     * @brief Signal for zooming out of the scene
     *
     */
    void zoomOut();

    /**
     * @brief Signal for zooming to fit the scene
     *
     */
    void zoomToFit();

    /**
     * @brief Signal for toggling the display of names
     *
     */
    void toggleNames();

    /**
     * @brief Signal for clearing all highlight colors
     *
     */
    void clearAllHighlightColors();

    /**
     * @brief Signal for zooming to a specific node
     *
     * @param nodeName The name of the node to zoom to.
     */
    void zoomToNode(const QString& nodeName);

    /**
     * @brief Signal for exporting the scene to an SVG file
     *
     * @param exportSelected Whether to export only the selected items.
     * @return The SVG file as a byte array.
     */
    QByteArray exportToSvg(bool exportSelected = false);

    /**
     * @brief Signal for a generic module being double clicked
     *
     * @param moduleName The name of the module.
     * @param moduleType The type of the module.
     */
    void genericModuleDoubleClicked(const QString& moduleName, const QString& moduleType);

private:
    Ui::NetlistTab* ui;   ///< The user interface for the tab.
    QNetlistScene* scene; ///< The scene for the tab.

    QString modulePath;                                                          ///< The path of the module in the design.
    std::shared_ptr<Yosys::Module> module;                                       ///< The module to be displayed in the tab.
    std::shared_ptr<std::map<QString, std::shared_ptr<Symbol::Symbol>>> symbols; ///< The symbols used for display
    Routing::Router router;                                                      ///< The router for the module.

    /**
     * @brief Set the visibility of the module path
     *
     * It is visible if the path is not / otherwise it is invisible
     *
     */
    void setModuleHierarchyVisible();
};

} // namespace OpenNetlistView

#endif // __NETLISTTAB_H__