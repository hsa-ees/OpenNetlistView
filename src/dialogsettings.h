/**
 * @file dialogsettings.h
 * @brief Header file for the DialogSettings class in the OpenNetlistView namespace.
 *
 * This file contains the declaration of the DialogSettings class, which provides
 * a dialog for configuring settings related to symbols in the OpenNetlistView application.
 *
 * The DialogSettings class inherits from QDialog and provides functionality for
 * loading, uploading, and resetting symbol data. It also emits a signal when the
 * symbols data has changed.
 *
 * @author Lukas Bauer
 */
#ifndef __DIALOGSETTINGS_H__
#define __DIALOGSETTINGS_H__

#include <QObject>
#include <QDialog>
#include <QtCore/Qt>
#include <QByteArray>

#include <routing/cola_router.h>

namespace OpenNetlistView {

QT_BEGIN_NAMESPACE
namespace Ui {
class DialogSettings;
}
QT_END_NAMESPACE

/**
 * @class DialogSettings
 * @brief The DialogSettings class provides a dialog for managing symbol settings.
 *
 * This class inherits from QDialog and provides functionality to manage and update
 * symbol settings within the application. It allows users to upload custom symbols,
 * reset to default symbols, and emits signals when the symbols data changes.
 *
 * The class includes methods for initializing the dialog, handling user interactions,
 * and managing the symbols data. It also provides a static method to retrieve the
 * default symbols data.
 *
 */
class DialogSettings : public QDialog
{
    Q_OBJECT

private:
    constexpr const static char* defaultSymbolsPath{":/symbols/default.svg"};

    constexpr const static double defaultXConstraint{1000.0F}; ///< The default x constraint.
    constexpr const static double defaultYConstraint{0.0F};    ///< The default y constraint.

    constexpr const static double defaultTestTolerance{1.0E-4};  ///< The test tolerance.
    constexpr const static int defaultTestMaxIterations{10000U}; ///< The test iterations.

    constexpr const static double defaultEdgeLength{10.0F}; ///< The default edge length.

public:
    /**
     * @brief Constructor for DialogSettings.
     *
     * Initializes the dialog settings with an optional parent widget.
     *
     * @param parent The parent widget, or nullptr if there is no parent.
     */
    DialogSettings(QWidget* parent = nullptr);

    /**
     * @brief Destructor for DialogSettings.
     *
     * Cleans up any resources used by the dialog settings.
     */
    ~DialogSettings();

    /**
     * @brief Gets the default symbols data.
     *
     * @return The default symbols data.
     */
    static QByteArray getDefaultSymbolData();

    /**
     * @brief Gets the routing parameters.
     *
     * @return The routing parameters.
     */
    Routing::ColaRoutingParameters getRoutingParameters();

signals:

    /**
     * @brief signal emitted when the symbols data has changed.
     *
     * @param symbols The new symbols data.
     */
    void symbolsChanged(QByteArray& symbols);

    /**
     * @brief signal emitted when the routing parameters have changed.
     *
     * @param routingParameters The new routing parameters.
     */
    void routingParametersChanged(Routing::ColaRoutingParameters routingParameters);

public slots:
    /**
     * @brief slot is used to enable the reset button.
     *
     * this is needed when the symbols are loaded through the cli option.
     *
     */
    void setCustomSymbolsLoadedCLI();

    /**
     * @brief slot is used to set the routing parameters
     *
     * @param routingParameters The routing parameters to be set.
     * @param tabChanged A flag to indicate if the tab has changed.
     *
     */
    void setRoutingParameters(const Routing::ColaRoutingParameters& routingParameters, bool tabChanged);

private slots:

    /**
     * @brief emits the symbolsChanged signal with the new symbols data.
     *
     * if defaultSymbolsLoaded is false it sends the symbolsData,
     * otherwise it sends the default symbols.
     *
     */
    void acceptedChanges();

    /**
     * @brief this slot is called when the user clicks the change button.
     *
     * It opens a file dialog to select a new symbols file.
     * And loads the symbols data into the symbolsData member.
     * It also sets the defaultSymbolsLoaded to false and enables the reset button.
     *
     */
    void uploadSymbols();

    /**
     * @brief this slot is called when the user clicks the reset button.
     *
     * it sets the defaultSymbolsLoaded to true and disables the reset button.
     *
     */
    void resetSymbols();

    /**
     * @brief this slot is called when the user clicks the reset button.
     *
     * It resets the routing parameters to the default values.
     *
     */
    void resetRoutingParameters();

private:
    /**
     * @brief sets the default routing parameters.
     *
     */
    void setDefaultRoutingParameters();

    Ui::DialogSettings* ui;                                 ///< The UI object for the dialog settings.
    QByteArray symbolsData;                                 ///< The users custom symbols data.
    bool defaultSymbolsLoaded = true;                       ///< A flag to indicate if the default symbols are loaded.
    Routing::ColaRoutingParameters loadedRoutingParameters; ///< The routing parameters got form the calculation
};

} // namespace OpenNetlistView

#endif // __DIALOGSETTINGS_H__