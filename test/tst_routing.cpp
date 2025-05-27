/**
 * @file tst_routing.cpp
 * @brief Test file for the symbols portion of the routing library
 *
 * @author Lukas Bauer
 */

#include <QtTest/QTest>
#include <QString>
#include <QDomElement>

#include <symbol/symbol_parser.h>

using namespace OpenNetlistView;

class tst_routing : public QObject
{

    Q_OBJECT

    static QDomElement loadSVG(const QString& filename);

private slots:

    void test_case1();
    void test_case2();
};

// helper that loads in symbol files
QDomElement tst_routing::loadSVG(const QString& filename)
{

    QString verifiedFilename = QFINDTESTDATA(filename);

    QFile symbolFile = QFile(verifiedFilename);
    symbolFile.open(QIODevice::ReadOnly | QIODevice::Text);

    const QByteArray symbolData = symbolFile.readAll();

    QDomDocument symbolDom;
    symbolDom.setContent(symbolData);

    return symbolDom.documentElement();
}

// checks if a symbol file with an missing default type is rejected
void tst_routing::test_case1()
{
    QDomElement symbolRoot = loadSVG("data/routing/test1.svg");

    Symbol::SymbolParser parser;
    parser.setRootElement(symbolRoot);

    QVERIFY_THROWS_EXCEPTION(std::runtime_error, parser.parse());
}

// checks if a file with additional symbols is accepted and the symbol is present
void tst_routing::test_case2()
{
    QDomElement symbolRoot = loadSVG("data/routing/test2.svg");

    Symbol::SymbolParser parser;
    parser.setRootElement(symbolRoot);

    parser.parse();

    auto symbols = parser.getSymbols();

    QVERIFY(symbols.find("MAdderCore") != symbols.end());
}

QTEST_MAIN(tst_routing);
#include "tst_routing.moc"