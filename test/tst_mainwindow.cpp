/**
 * @file tst_mainwindow.cpp
 * @brief Test file for the MainWindow class in the OpenNetlistView namespace.
 *
 * @author Lukas Bauer
 */

#include "mainwindow.h"
#include <QtTest/QTest>

using namespace OpenNetlistView;

class tst_mainwindow : public QObject
{
    Q_OBJECT

private slots:
    void test_case1();
    void test_case2();
    void test_case3();
};

// Test case 1: Check if the MainWindow is visible when created with valid parameters
void tst_mainwindow::test_case1()
{
    MainWindow w("", "");
    w.show();
    QVERIFY(w.isVisible());
}

// Check if the MainWindow opens when the skin file path is invalid
void tst_mainwindow::test_case2()
{

    try
    {
        MainWindow w{"../", ""};
    }
    catch(const std::runtime_error&)
    {
        QFAIL("Expected no std::runtime_error");
        // Expected exception
    }
}

// Test case 3: Check if the MainWindow throws an exception when the netlist path is invalid
void tst_mainwindow::test_case3()
{

    try
    {
        MainWindow w{"", "../"};
        QFAIL("Expected std::runtime_error");
    }
    catch(const std::runtime_error&)
    {
        // Expected exception
    }
}

QTEST_MAIN(tst_mainwindow)
#include "tst_mainwindow.moc"