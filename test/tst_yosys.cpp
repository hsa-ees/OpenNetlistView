/**
 * @file tst_yosys.cpp
 * @brief Test file for the parsing and connection of JSON netlist files
 *
 * @author Lukas Bauer
 */

#include "qobject.h"
#include <QtTest/QTest>
#include <QJsonObject>
#include <QJsonDocument>
#include <qlogging.h>
#include <QFile>
#include <QString>

#include <yosys/parser.h>
#include <yosys/port.h>

using namespace OpenNetlistView;

class tst_yosys : public QObject
{
    Q_OBJECT

    static const QJsonObject load_json(QString filename);

private slots:
    void test_case1();
    void test_case2();
    void test_case3();
    void test_case4();
    void test_case5();
    void test_case6();
    void test_case7();
    void test_case8();
    void test_case9();
    void test_case10();
    void test_case11();
    void test_case12();
    void test_case13();
    void test_case14();
    void test_case15();
    void test_case16();
    void test_case17();
    void test_case18();
    void test_case19();
    void test_case20();
    void test_case21();
    void test_case22();
    void test_case23();
    void test_case24();
    void test_case25();
    void test_case26();
    void test_case27();
    void test_case28();
    void test_case29();
    void test_case30();
    void test_case31();
    void test_case32();
    void test_case33();
    void test_case34();
    void test_case35();
    void test_case36();
    void test_case37();
    void test_case38();
    void test_case39();
};

// Helper functions
const QJsonObject tst_yosys::load_json(QString filename)
{
    QString verifiedFilename = QFINDTESTDATA(filename);
    QFile file(verifiedFilename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray fileContent = file.readAll();
    file.close();
    return QJsonDocument::fromJson(fileContent).object();
}

// Test if an json object that is not from yosys throws an error
void tst_yosys::test_case1()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test1.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);

    QVERIFY_THROWS_EXCEPTION(std::runtime_error, parser.parse());
}

// Test parsing of a yosys json ports
void tst_yosys::test_case2()
{
    const QJsonObject yosysJsonObject = load_json("data/yosys/test2.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    // fails because not all ports have connections
    QVERIFY_THROWS_EXCEPTION(std::runtime_error, parser.parse());
}

// Test parsing of yosys netnames
void tst_yosys::test_case3()
{
    const QJsonObject yosysJsonObject = load_json("data/yosys/test3.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);

    // fails because not all ports have connections
    QVERIFY_THROWS_EXCEPTION(std::runtime_error, parser.parse());
}

// check a json file where the cells have ports but a port has no bits
// this should throw an error because bits and drivers are required
// for each port
void tst_yosys::test_case4()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test4.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_EXCEPTION(std::runtime_error, parser.parse());
}

// check a json file where the cells have ports a port has matching bits
// and direction count but not all connections are present
void tst_yosys::test_case5()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test5.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    // fails because not all ports have connections
    QVERIFY_THROWS_EXCEPTION(std::runtime_error, parser.parse());
}

// Check if modules are skipped if they have lib/../share/yosys in the src string
void tst_yosys::test_case6()
{
    const QJsonObject yosysJsonObject = load_json("data/yosys/test6.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);

    // fails because not all ports have connections
    QVERIFY_THROWS_EXCEPTION(std::runtime_error, parser.parse());
}

// check if there is an error if a cell does not have a string type field
void tst_yosys::test_case7()
{
    const QJsonObject yosysJsonObject = load_json("data/yosys/test7.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_EXCEPTION(std::runtime_error, parser.parse());
}

// test if the if an invalidly set port direction is detected
void tst_yosys::test_case8()
{
    const QJsonObject yosysJsonObject = load_json("data/yosys/test8.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_EXCEPTION(std::runtime_error, parser.parse());
}

// test if an empty port bits array is detected
void tst_yosys::test_case9()
{
    const QJsonObject yosysJsonObject = load_json("data/yosys/test9.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_EXCEPTION(std::runtime_error, parser.parse());
}

// test if a netnames empty bits array is detected
void tst_yosys::test_case10()
{
    const QJsonObject yosysJsonObject = load_json("data/yosys/test10.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_EXCEPTION(std::runtime_error, parser.parse());
}

// test if a cell with no port directions is detected
void tst_yosys::test_case11()
{
    const QJsonObject yosysJsonObject = load_json("data/yosys/test11.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_EXCEPTION(std::runtime_error, parser.parse());
}

// test if a cell with no connections is detected
void tst_yosys::test_case12()
{
    const QJsonObject yosysJsonObject = load_json("data/yosys/test12.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_EXCEPTION(std::runtime_error, parser.parse());
}

// test if a valid file passes
void tst_yosys::test_case13()
{
    const QJsonObject yosysJsonObject = load_json("data/yosys/test13.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// test a simple joiner configuration
void tst_yosys::test_case14()
{
    const QJsonObject yosysJsonObject = load_json("data/yosys/test14.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// test a simple joiner configuration with constant value
void tst_yosys::test_case15()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test15.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// check a direct connection from in to output
void tst_yosys::test_case16()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test16.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// test another simple joiner configuration
void tst_yosys::test_case17()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test17.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// test a splitter that only connects parts of the input to the output
void tst_yosys::test_case18()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test18.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// test the creation of a split join configuration that sorts bits into the
// correct order
void tst_yosys::test_case19()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test19.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// test adding a constant value in the middle of an existing signal
void tst_yosys::test_case20()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test20.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// test adding a constant value in the middle of an existing signal
void tst_yosys::test_case21()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test21.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// test adding a constant value in the middle of an existing signal
void tst_yosys::test_case22()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test22.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// test adding a constant value in the middle of an existing signal
void tst_yosys::test_case23()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test23.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// sorting two inputs into the correct order
void tst_yosys::test_case24()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test24.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// connect only a constant
void tst_yosys::test_case25()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test25.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// complex connection with mux and joiner
void tst_yosys::test_case26()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test26.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// joining signals then adding diffenert constants
void tst_yosys::test_case27()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test27.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// joining directly with wrong netname
void tst_yosys::test_case28()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test28.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// joining directly with wrong netname
void tst_yosys::test_case29()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test29.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// mixing consts and signals in a joiner
void tst_yosys::test_case30()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test30.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// complex connection adding constants to parts of signals then mux them
void tst_yosys::test_case31()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test31.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// connecting one in to out directly and the part of the same in to another out
void tst_yosys::test_case32()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test32.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// doing the same as in 32 but connecting part signal to two outs
void tst_yosys::test_case33()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test33.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// complex connections
void tst_yosys::test_case34()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test34.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// joiner example
void tst_yosys::test_case35()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test35.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// connecting the same bit multiple times to a join
void tst_yosys::test_case36()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test36.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// two independent connections
void tst_yosys::test_case37()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test37.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// complex connections
void tst_yosys::test_case38()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test38.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

// conecting a submodule
void tst_yosys::test_case39()
{

    const QJsonObject yosysJsonObject = load_json("data/yosys/test39.json");

    QVERIFY(yosysJsonObject.isEmpty() != true);

    Yosys::Parser parser;
    parser.setYosysJsonObject(yosysJsonObject);
    QVERIFY_THROWS_NO_EXCEPTION(parser.parse());
}

QTEST_MAIN(tst_yosys)
#include "tst_yosys.moc"
