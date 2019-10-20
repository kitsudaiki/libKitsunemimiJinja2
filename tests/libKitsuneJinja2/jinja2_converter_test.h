/**
 *  @file    jinja2_converter_test.h
 *
 *  @author  Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 *  @copyright MIT License
*/

#ifndef JINJA2CONVERTER_TEST_H
#define JINJA2CONVERTER_TEST_H

#include <libKitsuneCommon/unit_test.h>
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

class Jinja2Converter_Test
        : public Kitsune::Common::UnitTest
{

public:
    Jinja2Converter_Test();

private:
    Kitsune::Jinja2::Jinja2Converter* m_converter = nullptr;
    Kitsune::Common::DataItem* m_testJson;
    std::string m_testJsonString;

    void initTestCase();

    void plainText_Test();
    void replace_Test();
    void ifCondition_Test();
    void forLoop_Test();

    void parserFail_Test();
    void converterFail_Test();

    void cleanupTestCase();
};

}
}

#endif // JINJA2CONVERTER_TEST_H
