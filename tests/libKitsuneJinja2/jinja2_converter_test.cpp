/**
 *  @file    jinja2_converter_test.cpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  MIT License
 */

#include "jinja2_converter_test.h"
#include <jinja2_converter.h>
#include <data_structure/data_items.h>
#include <json_item.h>

namespace Kitsune
{
namespace Jinja2
{

/**
 * @brief Jinja2ConverterTest::initTestCase
 */
Jinja2Converter_Test::Jinja2Converter_Test()
    : Kitsune::Common::Test("Jinja2Converter_Test")
{
    initTestCase();

    plainText_Test();
    replace_Test();
    ifCondition_Test();
    forLoop_Test();

    parserFail_Test();
    converterFail_Test();

    cleanupTestCase();
}

/**
 * @brief initTestCase
 */
void
Jinja2Converter_Test::initTestCase()
{
    m_converter = new Kitsune::Jinja2::Jinja2Converter();

    m_testJsonString = std::string(
                "{\"item\": "
                    "{ \"sub_item\": \"test_value\"},"
                "\"item2\": "
                    "{ \"sub_item2\": 42},"
                "\"loop\": "
                    "[ {\"x\" :\"test1\" }, {\"x\" :\"test2\" }, {\"x\" :\"test3\" }]"
                "}");


    Kitsune::Json::JsonItem item;
    item.parse(m_testJsonString);
    m_testJson = item.getItemContent()->copy();
}

/**
 * @brief plainText_Test
 */
void
Jinja2Converter_Test::plainText_Test()
{
    std::string testString("this is a test");
    std::pair<std::string, bool> result = m_converter->convert(testString, m_testJson->toObject());

    UNITTEST(result.second, true);
    UNITTEST(result.first, testString);
}

/**
 * @brief replace_Test
 */
void
Jinja2Converter_Test::replace_Test()
{
    std::string testString("this is a {{ item.sub_item }}");
    std::pair<std::string, bool> result = m_converter->convert(testString, m_testJson->toObject());

    UNITTEST(result.second, true);
    UNITTEST(result.first, std::string("this is a test_value"));
}

/**
 * @brief ifCondition_Test
 */
void
Jinja2Converter_Test::ifCondition_Test()
{
    std::pair<std::string, bool> result;
    std::string testString("this is "
                       "{% if item2.sub_item2 is 42 %}"
                       "a "
                       "{{ item.sub_item }}"
                       "{% endif %}");
    result = m_converter->convert(testString, m_testJson->toObject());

    UNITTEST(result.second, true);
    UNITTEST(result.first, std::string("this is a test_value"));

    std::string testString2("this is "
                        "{% if item2.sub_item2 is someother %}"
                        "a "
                        "{{ item.sub_item }}"
                        "{% endif %}");
    result = m_converter->convert(testString2, m_testJson->toObject());

    UNITTEST(result.second, true);
    UNITTEST(result.first, std::string("this is "));
}

/**
 * @brief forLoop_Test
 */
void
Jinja2Converter_Test::forLoop_Test()
{
    std::pair<std::string, bool> result;
    std::string testString("this is"
                       "{% for value in loop %}"
                       " a "
                       "{{ value.x }}"
                       "{% endfor %}");
    result = m_converter->convert(testString, m_testJson->toObject());

    UNITTEST(result.second, true);
    UNITTEST(result.first, std::string("this is a test1 a test2 a test3"));
}

/**
 * @brief parserFail_Test
 */
void
Jinja2Converter_Test::parserFail_Test()
{
    std::string testString("this is "
                       "{% if item2.sub_item2 ist something %}" // ist instread of is
                       "a "
                       "{{ item.sub_item }}"
                       "{% endif %}");
    std::pair<std::string, bool> result = m_converter->convert(testString, m_testJson->toObject());

    UNITTEST(result.second, false);
}

/**
 * @brief converterFail_Test
 */
void
Jinja2Converter_Test::converterFail_Test()
{
    std::string testString("this is a {{ item_fail.sub_item }}");
    std::pair<std::string, bool> result = m_converter->convert(testString, m_testJson->toObject());

    UNITTEST(result.second, false);
}

/**
 * cleanupTestCase
 */
void
Jinja2Converter_Test::cleanupTestCase()
{
    delete m_converter;
}

}
}
