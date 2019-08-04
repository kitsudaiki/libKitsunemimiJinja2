/**
 *  @file    jinja2Converter_test.cpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  MIT License
 */

#include "jinja2Converter_test.hpp"
#include <jinja2Converter.hpp>
#include <jsonItems.hpp>

/**
 * @brief Jinja2ConverterTest::initTestCase
 */
Jinja2Converter_Test::Jinja2Converter_Test(): Kitsune::CommonTest("Jinja2ConverterTest")
{
    initTestCase();

    plainTextTest();
    replaceTest();
    ifConditionTest();
    forLoopTest();

    parserFailTest();
    converterFailTest();

    cleanupTestCase();
}

void Jinja2Converter_Test::initTestCase()
{
    m_converter = new Kitsune::Jinja2::Jinja2Converter();

    m_testJsonString = std::string("{\"item\": "
                                       "{ \"sub_item\": \"test_value\"},"
                                   "\"item2\": "
                                       "{ \"sub_item2\": 42},"
                                   "\"loop\": "
                                       "[ {\"x\" :\"test1\" }, {\"x\" :\"test2\" }, {\"x\" :\"test3\" }]"
                                   "}");

    m_testJson = Kitsune::Json::JsonItem::parseString(m_testJsonString);
}

/**
 * @brief Jinja2ConverterTest::plainTextTest
 */
void Jinja2Converter_Test::plainTextTest()
{
    std::string testString("this is a test");
    std::pair<std::string, bool> result = m_converter->convert(testString, m_testJson->toObject());

    UNITTEST(result.second, true);
    UNITTEST(result.first, testString);
}

/**
 * @brief Jinja2ConverterTest::replaceTest
 */
void Jinja2Converter_Test::replaceTest()
{
    std::string testString("this is a {{ item.sub_item }}");
    std::pair<std::string, bool> result = m_converter->convert(testString, m_testJson->toObject());

    UNITTEST(result.second, true);
    UNITTEST(result.first, std::string("this is a test_value"));
}

/**
 * @brief Jinja2ConverterTest::ifConditionTest
 */
void Jinja2Converter_Test::ifConditionTest()
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
 * @brief Jinja2ConverterTest::forLoopTest
 */
void Jinja2Converter_Test::forLoopTest()
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
 * @brief Jinja2ConverterTest::parserFailTest
 */
void Jinja2Converter_Test::parserFailTest()
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
 * @brief Jinja2ConverterTest::converterFailTest
 */
void Jinja2Converter_Test::converterFailTest()
{
    std::string testString("this is a {{ item_fail.sub_item }}");
    std::pair<std::string, bool> result = m_converter->convert(testString, m_testJson->toObject());

    UNITTEST(result.second, false);
}

/**
 * @brief Jinja2ConverterTest::cleanupTestCase
 */
void Jinja2Converter_Test::cleanupTestCase()
{
    delete m_converter;
}
