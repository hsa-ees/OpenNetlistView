/**
 * @file mainwindow.h
 * @brief Header file for the MainWindow class in the OpenNetlistView namespace.
 *
 * This file contains the declaration of the MainWindow class, which is the main
 * window of the OpenNetlistView application. It includes methods for opening and saving
 * files, as well as private members for the user interface and JSON document handling.
 *
 * @author Lukas Bauer
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QJsonDocument>
#include <QMessageBox>
#include <QString>
#include <QStandardItem>

#include <memory>
#include <map>
#include <vector>

#include <yosys/parser.h>
#include <yosys/module.h>
#include <symbol/symbol.h>
#include <symbol/symbol_parser.h>
#include <routing/router.h>

#include "dialogabout.h"
#include "dialogsettings.h"
#include "dialogsearch.h"
#include "qnetlistscene.h"

Q_DECLARE_SMART_POINTER_METATYPE(std::shared_ptr);

namespace OpenNetlistView {

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow; ///< Forward declaration of the Ui::MainWindow class.
}
QT_END_NAMESPACE

namespace Yosys {
class Module;
}

/**
 * @class MainWindow
 * @brief The main window class for the OpenNetlistView application.
 *
 * This class inherits from QMainWindow and provides the main interface for the
 * OpenNetlistView application. It includes methods for opening and saving files, and
 * manages the user interface and JSON document handling.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for MainWindow.
     *
     * Initializes the main window with an optional parent widget.
     *
     * @param parent The parent widget, or nullptr if there is no parent.
     */
    MainWindow(const QString& jsonFilename, const QString& skinFilename, QWidget* parent = nullptr);

    /**
     * @brief Destructor for MainWindow.
     *
     * Cleans up any resources used by the main window.
     */
    ~MainWindow();

protected:
    void showEvent(QShowEvent* event) override;

private slots:

    /**
     * @brief Slot to open a file.
     *
     * This slot is triggered to open a file and load its contents into the application.
     */
    void openFile();

    /**
     * @brief Slot to show an error message.
     *
     * This slot is triggered to show an error message dialog with the given error message.
     *
     * @param error The error message to display.
     */
    void showError(const QString& error);

    /**
     * @brief Slot to close an error message.
     *
     * This slot is triggered to close the error message dialog.
     *
     * @param result The result of the error message dialog.
     */
    void closeError(int result);

    /**
     * @brief Slot to open the about dialog.
     */
    void showDialogAbout();

    /**
     * @brief Slot to close the about dialog.
     *
     * This slot is triggered to close the about dialog.
     *
     * @param result The result of the about dialog.
     */
    void closeDialogAbout(int result);

    /**
     * @brief Slot to show the settings dialog.
     */
    void showSettings();

    /**
     * @brief Slot to close the settings dialog.
     *
     * This slot is triggered to close the settings dialog.
     *
     * @param result The result of the settings dialog.
     */
    void closeSettings(int result);

    /**
     * @brief Slot to save the diagram as an image.
     */
    void showSearch();

    /**
     * @brief Slot to close the search dialog.
     *
     * This slot is triggered to close the search dialog.
     *
     * @param result The result of the search dialog.
     */
    void closeSearch(int result);

    /**
     * @brief Slot to load updated symbols.
     *
     * This slot is triggered to load updated symbols from a byte array.
     * It parses the byte array and updates the symbols in the diagram.
     *
     * @param symbolByteData The byte array containing the updated symbols.
     */
    void loadUpdatedSymbols(QByteArray& symbolByteData);

    /**
     * @brief Slot to export the whole schematic to an SVG file.
     */
    void exportSchematicToSvg();

    /**
     * @brief Slot to export the currently selected items to an SVG file.
     */
    void exportSelectedToSvg();

    /**
     * @brief Slot to open a dialog asking to load a new diagram.
     *
     * This slot is triggered when a new diagram is loaded after allready having a diagram loaded.
     */
    void showAskRemoveLoadedDiagram();

    /**
     * @brief Slot to close the dialog asking to remove the loaded diagram.
     *
     * When Yes is clicked the old diagram is removed and the new one is loaded.
     * When No is clicked the dialog is closed.
     *
     */
    void closeAskRemoveLoadedDiagram();

    /**
     * @brief Method to parse a JSON document.
     *
     * This method parses a JSON document and updates the diagram with the parsed data.

     */
    void parseJson();

    /**
     * @brief Slot to show a routing progress dialog.
     *
     * This slot is triggered to show a dialog indicating a routing can take a while.
     */
    void showRoutingProgressDialog();

    /**
     * @brief Slot to close the routing progress dialog.
     *
     * This slot is triggered to close the routing progress dialog.
     */
    void closeRoutingProgressDialog();

    /**
     * @brief Slot to create the hierarchy tree.
     *
     * This slot is triggered to create the hierarchy tree for the current diagram.
     */
    void createHierarchyTree(const std::shared_ptr<Yosys::Module>& module, QStandardItem* parentItem = nullptr);

    /**
     * @brief Slot to handle a click on the hierarchy tree.
     *
     * This slot is triggered when an item in the hierarchy tree is clicked.
     *
     * @param index The index of the clicked item.
     */
    void clickedOnHierarchyTree(const QModelIndex& index);

    /**
     * @brief Slot to set the hierarchy position of the module.
     *
     * This slot is triggered to set the hierarchy position of the module in the diagram.
     *
     * @param modulePath The path of the module.
     */
    void setHierarchyPos(const QString& modulePath);

    /**
     * @brief Slot to load a example file to display
     */
    void loadExampleDiagram();

signals:

    /**
     * @brief Signal to start JSON parsing.
     *
     * This signal is emitted to start the JSON parsing process.
     */
    void startJsonParsing();

    /**
     * @brief Signal to continue large routing.
     *
     * This signal is emitted to continue the routing process for a large module.
     */
    void continueLargeRouting();

private:
    Ui::MainWindow* ui;                                         ///< Pointer to the user interface.
    Yosys::Parser parser;                                       ///< Instance of the Parser class for handling file parsing.
    std::unique_ptr<Yosys::Diagram> diagram;                    ///< Instance of the Diagram class for handling diagram data.
    std::shared_ptr<Yosys::Module> currentModule;               ///< Pointer to the current module in the diagram.
    Symbol::SymbolParser symbolParser;                          ///< Instance of the SymbolParser class for handling symbol parsing.
    QByteArray fileContent;                                     ///< The content of the file to be loaded
    QString fileName;                                           ///< The name of the file to be loaded
    QStandardItemModel hierarchyModel;                          ///< Model for the hierarchy tree
    bool diagramLoaded = false;                                 ///< indicates whether a diagram has been loaded
    std::map<QString, std::shared_ptr<Symbol::Symbol>> symbols; ///< Vector of symbols for the current diagram.
    QJsonDocument yosysJsonDoc;                                 ///< JSON document for handling Yosys data.
    DialogAbout* dialogAbout;                                   ///< About dialog for displaying information about the application.
    DialogSettings* dialogSettings;                             ///< Settings dialog for configuring application settings.
    DialogSearch* dialogSearch;                                 ///< Search dialog for searching the diagram.
    QMessageBox* longRoutingMessage;                            ///< Dialog for showing the routing can take a while
    QMessageBox* askRemoveDialog;                               ///< Dialog for asking to remove the loaded diagram
    QMessageBox* errorMessage;                                  ///< Error message dialog for displaying errors.

    /**
     * @brief Method to upgrade the display.
     *
     * This method upgrades the display of the diagram after loading new symbols.
     */
    void setNetlisttabDiagramm();

    /**
     * @brief generate the module path from a hierarchy tree item
     *
     * @param item the item to generate the path for
     * @return the path on success otherwise an empty string
     */
    static QString createHierarchyModulePath(QStandardItem* item);

    /**
     * @brief load a file given its path
     *
     * @param skinFilename the path to the file
     *
     * @return the file content as a byte array
     */
    QByteArray loadFileCLI(const QString& skinFilename);

    /**
     * @brief find an item in a hierarchy tree by its text
     *
     * @param parent the parent item to search in
     * @param text the text to search for
     * @return the item on success otherwise nullptr
     */
    static QStandardItem* findItemByText(QStandardItem* parent, const QString& text);

    /**
     * @brief search the hierarchy tree for an item by its text
     *
     * @param model the model to search in
     * @param text the text to search for
     * @return the item on success otherwise nullptr
     */
    static QStandardItem* searchHierarchyByText(QStandardItemModel* model, const QString& text);
};

} // namespace OpenNetlistView

#endif // MAIN_WINDOW_H
