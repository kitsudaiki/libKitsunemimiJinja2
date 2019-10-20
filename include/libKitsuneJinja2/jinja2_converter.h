/**
 *  @file    jinja2Converter.h
 *
 *  @author  Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 *  @copyright MIT License
 */

#ifndef JINJA2CONVERTER_H
#define JINJA2CONVERTER_H

#include <utility>
#include <string>
#include <libKitsuneCommon/common_items/data_items.h>

namespace Kitsune
{
namespace Jinja2
{
class Jinja2ParserInterface;
class Jinja2Item;
class ReplaceItem;
class IfItem;
class ForLoopItem;

class Jinja2Converter
{
public:
    Jinja2Converter(const bool traceParsing = false);
    ~Jinja2Converter();

    const std::pair<bool, std::string> convert(const std::string &templateString,
                                               const std::string &jsonInput);

    const std::pair<bool, std::string> convert(const std::string &templateString,
                                               Common::DataMap* input);

private:

    Jinja2ParserInterface* m_driver = nullptr;

    bool processItem(Common::DataMap* input,
                      Kitsune::Jinja2::Jinja2Item* part,
                      std::string* output);
    bool processReplace(Common::DataMap* input,
                        ReplaceItem* replaceObject,
                        std::string* output);
    bool processIfCondition(Common::DataMap* input,
                            IfItem* ifCondition,
                            std::string* output);
    bool processForLoop(Common::DataMap* input,
                        ForLoopItem* forLoop,
                        std::string* output);

    std::pair<std::string, bool> getString(Common::DataMap* input,
                                           Common::DataArray* jsonPath);
    std::pair<Common::DataItem*, bool> getItem(Common::DataMap* input,
                                               Common::DataArray* jsonPath);

    std::string createErrorMessage(Common::DataArray* jsonPath);
};

}  // namespace Jinja2
}  // namespace Kitsune

#endif // JINJA2CONVERTER_H
