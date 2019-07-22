/**
 *  @file    jinja2ConverterTest.h
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  MIT License
 */

#ifndef JINJA2CONVERTERTEST_H
#define JINJA2CONVERTERTEST_H

#include <testing/commonTest.h>
#include <utility>
#include <string>
#include <vector>

namespace Kitsune
{
namespace Json
{
class JsonObject;
class JsonItem;
}
namespace Jinja2
{
class KitsuneJinja2Converter;
}
}

class Jinja2ConverterTest: public Kitsune::CommonTest
{

public:
    Jinja2ConverterTest();

private:
    Kitsune::Jinja2::KitsuneJinja2Converter* m_converter = nullptr;
    Kitsune::Json::JsonItem* m_testJson;
    std::string m_testJsonString;

    void initTestCase();

    void plainTextTest();
    void replaceTest();
    void ifConditionTest();
    void forLoopTest();

    void parserFailTest();
    void converterFailTest();

    void cleanupTestCase();
};

#endif // JINJA2CONVERTERTEST_H
