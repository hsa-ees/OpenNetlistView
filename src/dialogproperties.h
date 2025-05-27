/**
 * @file dialogproperties.h
 * @brief Header file for the DialogProperties class.
 *
 * This file contains the declaration of the DialogProperties class and the PropertySettings namespace.
 * The DialogProperties class provides a dialog to display and manage properties in a table view.
 *
 * @author Lukas Bauer
 */

#ifndef __DIALOGPROPERTIES_H__
#define __DIALOGPROPERTIES_H__

#include <QDialog>
#include <QObject>
#include <QString>
#include <QStringList>

#include <utility>

namespace OpenNetlistView {

QT_BEGIN_NAMESPACE
namespace Ui {
class DialogProperties;
}
QT_END_NAMESPACE

/**
 * @namespace PropertySettings
 * @brief The settings for the properties dialog.
 *
 * They are used to setup the e.g the table view.
 */
namespace PropertySettings {

const QStringList columnNames =
    {
        QObject::tr("Property"),
        QObject::tr("Value")}; ///< The column names for the properties dialog

} // namespace PropertySettings

/**
 * @class OpenNetlistView::DialogProperties
 * @brief The DialogProperties class provides a dialog to display and manage properties.
 *
 * This class allows the user to display a list of properties in a dialog. Properties can be set, added, or cleared.
 * The dialog can be updated to reflect the current properties.
 *
 * The properties are displayed in a table view with columns for the property name and value.
 */
class DialogProperties : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new Dialog Properties object
     *
     * @param properties The properties to display.
     * @param parent The parent QWidget.
     */
    DialogProperties(const std::vector<std::pair<QString, QString>>& properties,
        QWidget* parent = nullptr);

    /**
     * @brief Construct a new Dialog Properties object
     *
     * @param parent The parent QWidget.
     */
    DialogProperties(QWidget* parent = nullptr);

    /**
     * @brief Destroy the Dialog Properties object
     *
     */
    ~DialogProperties();

    /**
     * @brief Set the properties to display.
     *
     * @param properties The properties to display.
     */
    void setProperties(const std::vector<std::pair<QString, QString>>& properties);

    /**
     * @brief Add a property to the dialog.
     *
     * @param key The key of the property.
     * @param value The value of the property.
     */
    void addProperty(const QString& key, const QString& value);

    /**
     * @brief Add a property to the dialog.
     *
     * @param property The property to add.
     */
    void addProperty(const std::pair<QString, QString>& property);

    /**
     * @brief Clear all properties from the dialog.
     *
     */
    void clearProperties();

private:
    /**
     * @brief Update the display of the dialog.
     *
     * This will add the properties to the dialog.
     */
    void updateDisplay();

    Ui::DialogProperties* ui;                            ///< The UI for the dialog.
    std::vector<std::pair<QString, QString>> properties; ///< The properties to display.
};

} // namespace OpenNetlistView

#endif // __DIALOGPROPERTIES_H__