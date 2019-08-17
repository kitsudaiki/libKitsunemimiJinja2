/**
 *  @file    jinja2Converter.h
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  MIT License
 */

#ifndef JINJA2CONVERTER_H
#define JINJA2CONVERTER_H

#include <utility>
#include <string>
#include <data_structure/data_items.h>

namespace Kitsune
{
namespace Jinja2
{
class Jinja2ParserInterface;

class Jinja2Converter
{
public:
    Jinja2Converter(const bool traceParsing = false);
    ~Jinja2Converter();

    std::pair<std::string, bool> convert(const std::string &templateString,
                                         Common::DataObject* input);

private:

    Jinja2ParserInterface* m_driver = nullptr;

    bool processArray(Common::DataObject* input,
                      Common::DataArray* part,
                      std::string* output);
    bool processReplace(Common::DataObject* input,
                        Common::DataArray* replaceObject,
                        std::string *output);
    bool processIfCondition(Common::DataObject* input,
                            Common::DataObject* ifCondition,
                            std::string *output);
    bool processForLoop(Common::DataObject* input,
                        Common::DataObject* forLoop,
                        std::string *output);

    std::pair<std::string, bool> getString(Common::DataObject* input,
                                           Common::DataArray* jsonPath);
    std::pair<Common::DataItem *, bool> getItem(Common::DataObject* input,
                                                  Common::DataArray* jsonPath);

    std::string createErrorMessage(Common::DataArray* jsonPath);
};

}  // namespace Jinja2
}  // namespace Kitsune

#endif // JINJA2CONVERTER_H
