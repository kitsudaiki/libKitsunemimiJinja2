/**
 *  @file    jinja2_converter_test.cpp
 *
 *  @author  Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 *  @copyright MIT License
*/

#include "jinja2_converter_test.h"
#include <libKitsunemimiJinja2/jinja2_converter.h>
#include <libKitsunemimiCommon/common_items/data_items.h>
#include <libKitsunemimiJson/json_item.h>

namespace Kitsunemimi
{
namespace Jinja2
{

/**
 * @brief Jinja2ConverterTest::initTestCase
 */
Jinja2Converter_Test::Jinja2Converter_Test()
    : Kitsunemimi::CompareTestHelper("Jinja2Converter_Test")
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
    m_converter = Kitsunemimi::Jinja2::Jinja2Converter::getInstance();

    m_testJsonString = std::string(
                "{\"item\": "
                    "{ \"sub_item\": \"test_value\"},"
                "\"item2\": 42,"
                "\"loop\": "
                    "[ {\"x\" :\"test1\" }, {\"x\" :\"test2\" }, {\"x\" :\"test3\" }]"
                "}");
}

/**
 * @brief plainText_Test
 */
void
Jinja2Converter_Test::plainText_Test()
{
    std::string testString("this is a test");
    ErrorContainer error;
    std::string output = "";
    bool result = m_converter->convert(output,  testString, m_testJsonString, error);

    TEST_EQUAL(result, true);
    TEST_EQUAL(output, testString);
}

/**
 * @brief replace_Test
 */
void
Jinja2Converter_Test::replace_Test()
{
    std::string testString("this is \n a {{ item.sub_item }}");
    ErrorContainer error;
    std::string output = "";
    bool result = m_converter->convert(output, testString, m_testJsonString, error);

    TEST_EQUAL(result, true);
    TEST_EQUAL(output, std::string("this is \n a test_value"));
}

/**
 * @brief ifCondition_Test
 */
void
Jinja2Converter_Test::ifCondition_Test()
{
    bool result;
    std::string testString("this is "
                           "{% if item2 is 42 %}"
                           "a \n"
                           "{{ item.sub_item }}"
                           "{% endif %}"
                           " poi");
    ErrorContainer error;
    std::string output = "";
    result = m_converter->convert(output, testString, m_testJsonString, error);

    TEST_EQUAL(result, true);
    TEST_EQUAL(output, std::string("this is a \ntest_value poi"));

    std::string testString2("this is "
                        "{% if item2 is someother %}"
                        "a \n"
                        "{{ item.sub_item }}"
                        "{% endif %}");
    output.clear();
    result = m_converter->convert(output, testString2, m_testJsonString, error);

    TEST_EQUAL(result, true);
    TEST_EQUAL(output, std::string("this is "));
}

/**
 * @brief forLoop_Test
 */
void
Jinja2Converter_Test::forLoop_Test()
{
    bool result;
    std::string testString("this is"
                           "{% for value in loop %}"
                           " a "
                           "{{ value.x }}"
                           "{% endfor %}");
    ErrorContainer error;
    std::string output = "";
    result = m_converter->convert(output, testString, m_testJsonString, error);

    TEST_EQUAL(result, true);
    TEST_EQUAL(output, std::string("this is a test1 a test2 a test3"));
}

/**
 * @brief parserFail_Test
 */
void
Jinja2Converter_Test::parserFail_Test()
{
    std::string testString("this is "
                           "{% if item2 ist something %}" // ist instread of is
                           "a "
                           "{{ item.sub_item }}"
                           "{% endif %}");
    ErrorContainer error;
    std::string output = "";
    bool result = m_converter->convert(output, testString, m_testJsonString, error);

    TEST_EQUAL(result, false);
}

/**
 * @brief converterFail_Test
 */
void
Jinja2Converter_Test::converterFail_Test()
{
    std::string testString("this is a {{ item_fail.sub_item }}");
    ErrorContainer error;
    std::string output = "";
    bool result = m_converter->convert(output, testString, m_testJsonString, error);

    TEST_EQUAL(result, false);
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
