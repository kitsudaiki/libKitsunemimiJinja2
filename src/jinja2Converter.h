/**
 *  @file    jinja2Converter.h
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 */

#ifndef JINJA2CONVERTER_H
#define JINJA2CONVERTER_H

#include <utility>
#include <vector>
#include <string>
#include <jsonObjects.h>

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
    std::pair<Json::AbstractJson *, bool> getItem(Json::JsonObject* input,
                                                  Json::JsonArray* jsonPath);

    std::string createErrorMessage(Json::JsonArray* jsonPath);
};

}  // namespace Jinja2
}  // namespace Kitsune

#endif // JINJA2CONVERTER_H
