/**
 *  @file    jinja2Converter_test.hpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  MIT License
 */

#ifndef JINJA2CONVERTER_TEST_HPP
#define JINJA2CONVERTER_TEST_HPP

#include <testing/test.hpp>
#include <utility>
#include <string>
#include <vector>

namespace Kitsune
{
namespace Common{
class DataItem;
}
namespace Jinja2
{
class Jinja2Converter;
}
}

class Jinja2Converter_Test
        : public Kitsune::Common::Test
{

public:
    Jinja2Converter_Test();

private:
    Kitsune::Jinja2::Jinja2Converter* m_converter = nullptr;
    Kitsune::Common::DataItem* m_testJson;
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

#endif // JINJA2CONVERTER_TEST_HPP
