/**
 *  @file    jinja2ConverterTest.h
 *  @version 0.1.0
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 */

#ifndef JINJA2CONVERTERTEST_H
#define JINJA2CONVERTERTEST_H

#include <QObject>
#include <QJsonDocument>
#include <Json::JsonObject>
#include <Json::JsonArray>
#include <std::string>
#include <QPair>
#include <QtTest/QtTest>

namespace Kitsune
{
namespace Jinja2
{
class KitsuneJinja2Converter;
}
}

class Jinja2ConverterTest: public QObject
{
    Q_OBJECT

public:
    Jinja2ConverterTest() {}

private:
    Kitsune::Jinja2::KitsuneJinja2Converter* m_converter = nullptr;
    Json::JsonObject m_testJson;
    std::string m_testJsonString;

private slots:
    void initTestCase();

    void plainTextTest();
    void replaceTest();
    void ifConditionTest();
    void forLoopTest();

    void parserFailTest();
    void converterFailTest();

    void cppIncludeTest();

    void cleanupTestCase();
};

#endif // JINJA2CONVERTERTEST_H
