/**
 * @file dialogabout.h
 * @brief Header file for the DialogAbout class.
 *
 * This file contains the declaration of the DialogAbout class, which is a
 * subclass of QDialog. It provides the user interface for the "About" dialog
 * in the OpenNetlistView application.
 *
 * The DialogAbout class includes functionality to display information about
 * the Qt framework used in the application.
 *
 * @author Lukas Bauer
 */

#ifndef __DIALOGABOUT_H__
#define __DIALOGABOUT_H__

#include <QDialog>
#include <QObject>
#include <QMessageBox>
#include <QtCore/Qt>

namespace OpenNetlistView {

QT_BEGIN_NAMESPACE
namespace Ui {
class DialogAbout;
}
QT_END_NAMESPACE

/**
 * @class DialogAbout
 * @brief The DialogAbout class provides a dialog window displaying information about the application.
 *
 * This class inherits from QDialog and is used to create a modal dialog that shows details about the application.
 * It includes a user interface defined in the Ui::DialogAbout class.
 *
 * The class also provides functionality to display information about the Qt framework used in the application.
 */
class DialogAbout : public QDialog
{

    Q_OBJECT

public:
    /**
     * @brief Constructs a DialogAbout object with the specified parent.
     *
     * @param parent The parent of the dialog.
     */
    DialogAbout(QWidget* parent = nullptr);

    /**
     * @brief Destroys the DialogAbout object.
     *
     */
    ~DialogAbout();

private slots:

    /**
     * @brief opens the about Qt dialog
     *
     */
    void showDialogQtAbout();

private:
    Ui::DialogAbout* ui; ///< the user interface for the about dialog

    QMessageBox* dialogQtAbout; ///< the about Qt dialog

    // the following two stings where extracted from the Qt source code
    // because the call for aboutQt() uses exec() which does not work
    // for wasm when asyncify is not enabled
    const QString translatedTextAboutQtCaption = tr(
        "<h3>About Qt</h3>"
        "<p>This program uses Qt version %1.</p>")
                                                     .arg(QLatin1String(QT_VERSION_STR)); ///< the caption of the about Qt dialog

    const QString translatedTextAboutQtText = tr(
        "<p>Qt is a C++ toolkit for cross-platform application "
        "development.</p>"
        "<p>Qt provides single-source portability across MS&nbsp;Windows, "
        "Mac&nbsp;OS&nbsp;X, Linux, and all major commercial Unix variants. "
        "Qt is also available for embedded devices as Qt for Embedded Linux "
        "and Qt for Windows CE.</p>"
        "<p>Qt is available under three different licensing options designed "
        "to accommodate the needs of our various users.</p>"
        "<p>Qt licensed under our commercial license agreement is appropriate "
        "for development of proprietary/commercial software where you do not "
        "want to share any source code with third parties or otherwise cannot "
        "comply with the terms of the GNU LGPL version 2.1 or GNU GPL version "
        "3.0.</p>"
        "<p>Qt licensed under the GNU LGPL version 2.1 is appropriate for the "
        "development of Qt applications (proprietary or open source) provided "
        "you can comply with the terms and conditions of the GNU LGPL version "
        "2.1.</p>"
        "<p>Qt licensed under the GNU General Public License version 3.0 is "
        "appropriate for the development of Qt applications where you wish to "
        "use such applications in combination with software subject to the "
        "terms of the GNU GPL version 3.0 or where you are otherwise willing "
        "to comply with the terms of the GNU GPL version 3.0.</p>"
        "<p>Please see <a href=\"http://qt.nokia.com/products/licensing\">qt.nokia.com/products/licensing</a> "
        "for an overview of Qt licensing.</p>"
        "<p>Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).</p>"
        "<p>Qt is a Nokia product. See <a href=\"http://qt.nokia.com/\">qt.nokia.com</a> "
        "for more information.</p>"); ///< the text of the about Qt dialog

    const QStringList contributors = {
        "Lukas Bauer",
    }; ///< a list of contributors to the application

    /**
     * @brief populates the about Qt dialog with the necessary information
     *
     *
     */
    void setupAboutQt();
};

} // namespace OpenNetlistView

#endif // __DIALOGABOUT_H__