/**
 *  @file    libKitsuneJinja2.hpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  MIT License
 */

#ifndef LIBKITSUNEJINJA2_HPP
#define LIBKITSUNEJINJA2_HPP

#include <utility>
#include <string>
#include <jsonItems.hpp>

namespace Kitsune
{
namespace Jinja2
{
class Jinja2ParserInterface;

class KitsuneJinja2Converter
{
public:
    KitsuneJinja2Converter(const bool traceParsing = false);
    ~KitsuneJinja2Converter();

    std::pair<std::string, bool> convert(const std::string &templateString,
                                         Json::JsonObject *input);

private:

    Jinja2ParserInterface* m_driver = nullptr;

    bool processArray(Json::JsonObject* input,
                      Json::JsonArray* part,
                      std::string* output);
    bool processReplace(Json::JsonObject* input,
                        Json::JsonArray* replaceObject,
                        std::string *output);
    bool processIfCondition(Json::JsonObject* input,
                            Json::JsonObject* ifCondition,
                            std::string *output);
    bool processForLoop(Json::JsonObject* input,
                        Json::JsonObject* forLoop,
                        std::string *output);

    std::pair<std::string, bool> getString(Json::JsonObject* input,
                                           Json::JsonArray* jsonPath);
    std::pair<Json::JsonItem *, bool> getItem(Json::JsonObject* input,
                                                  Json::JsonArray* jsonPath);

    std::string createErrorMessage(Json::JsonArray* jsonPath);
};

}  // namespace Jinja2
}  // namespace Kitsune

#endif // LIBKITSUNEJINJA2_HPP
