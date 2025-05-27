/**
 * @file main.cpp
 *
 * @brief Main entry point for the OpenNetlistView application.
 *
 * It contains a parser for the cli mode
 *
 * @author Lukas Bauer
 */

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QCoreApplication>
#include <QString>
#include <QFile>

#include <tuple>

#include <mainwindow.h>
#include <version/version.h>

using namespace OpenNetlistView;

std::tuple<QString, QString> commandLineParser(QApplication& app);

// NOLINTBEGIN
#ifdef __EMSCRIPTEN__

// this is required to allow exceptions to work with emscripten
// otherwise the program will abort with an exception
// See: https://doc.qt.io/qt-6/wasm.html

QApplication* g_App = nullptr;
MainWindow* g_Window = nullptr;

int main(int argc, char* argv[])
{
    g_App = new QApplication(argc, argv);

    QCoreApplication::setApplicationName("OpenNetlistView");
    QCoreApplication::setApplicationVersion(getDiagViewerVersion());

    g_Window = new MainWindow("", "");

    g_Window->setWindowIcon(QIcon(":/icons/OpenNetlistView.png"));

    g_Window->show();

    return 0;
}

#else
int main(int argc, char* argv[])
{
    QApplication App(argc, argv);

    QCoreApplication::setApplicationName("OpenNetlistView");
    QCoreApplication::setApplicationVersion(getDiagViewerVersion());

    const auto cmdArgs = commandLineParser(App);

    MainWindow Window(std::get<0>(cmdArgs), std::get<1>(cmdArgs));

    Window.setWindowIcon(QIcon(":/icons/OpenNetlistView.png"));

    Window.show();
    return App.exec();
}
#endif
// NOLINTEND

std::tuple<QString, QString> commandLineParser(QApplication& app)
{
    // create a parser with a help
    QCommandLineParser parser;

    parser.setApplicationDescription("OpenNetlistView is a tool for visualizing digital circuits.");
    parser.addHelpOption();
    parser.addVersionOption();

    // add a --skin option
    QCommandLineOption skinFileOption(QStringList() << "s"
                                                    << "skin",
        QCoreApplication::translate("main", "Load a skin file."),
        QCoreApplication::translate("main", "skinfile"));
    parser.addOption(skinFileOption);

    // add a posiotional argument for the JSON file contianing the netlist
    parser.addPositionalArgument("JSON-File", QCoreApplication::translate("main", "The JSON file containing the netlist."));

    parser.process(app);

    const QStringList args = parser.positionalArguments();

    // check if the arguments where parserd
    QString jsonFilename = "";
    QString skinFilename = "";

    if(!args.isEmpty())
    {
        jsonFilename = args.at(0);

        // check if the file exists
        if(!QFile::exists(jsonFilename))
        {
            qCritical() << "JSON File does not exist: " << jsonFilename;
            exit(EXIT_FAILURE);
        }
    }

    if(parser.isSet(skinFileOption))
    {
        skinFilename = parser.value(skinFileOption);

        // check if the file exists
        if(!QFile::exists(skinFilename))
        {
            qCritical() << "Skinfile does not exist: " << skinFilename;
            exit(EXIT_FAILURE);
        }
    }

    return {jsonFilename, skinFilename};
}