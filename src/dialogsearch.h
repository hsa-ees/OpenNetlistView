/**
 * @file dialogsearch.h
 * @brief Header file for the DialogSearch class.
 *
 * This file contains the declaration of the DialogSearch class, which provides
 * a dialog for searching within the OpenNetlistView application.
 *
 * @author Lukas Bauer
 */

#ifndef __DIALOGSEARCH_H__
#define __DIALOGSEARCH_H__

#include <QDialog>
#include <QObject>

namespace OpenNetlistView {

QT_BEGIN_NAMESPACE
namespace Ui {
class DialogSearch;
}

/**
 * @class OpenNetlistView::DialogSearch
 * @brief The DialogSearch class provides a dialog for searching text within the OpenNetlistView application.
 *
 * This class inherits from QDialog and provides a user interface for entering and accepting search text.
 * It emits a signal when the search text is accepted and handles the show event to clear the search line edit.
 *
 * The DialogSearch class is part of the OpenNetlistView namespace and uses the Ui::DialogSearch class for its user interface.
 */
class DialogSearch : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for DialogSearch.
     *
     * Initializes the search dialog with an optional parent widget.
     *
     * @param parent The parent widget, or nullptr if there is no parent.
     */
    DialogSearch(QWidget* parent = nullptr);

    /**
     * @brief Destructor for DialogSearch.
     *
     * Cleans up any resources used by the search dialog.
     */
    ~DialogSearch();

signals:

    /**
     * @brief Signal emitted when the search text is accepted.
     *
     * @param text The text to search for.
     */
    void searchText(const QString& text);

protected:
    /**
     * @brief custom show event to clear the search line edit
     *
     * @param event qt show event
     */
    void showEvent(QShowEvent* event) override;

private slots:

    /**
     * @brief Slot to handle the accepted search text.
     *
     * This slot is triggered when the search text is accepted.
     */
    void acceptedSearch();

private:
    Ui::DialogSearch* ui; ///< Pointer to the search dialog user interface.
};

} // namespace OpenNetlistView

#endif // __DIALOGSEARCH_H__