/**
 *  @file    libKitsuneJinja2.cpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 */

#include "libKitsuneJinja2.h"

#include <jinja2Converter.h>

namespace Kitsune
{
namespace Jinja2
{

/**
 * constructor
 */
KitsuneJinja2Converter::KitsuneJinja2Converter()
{
    m_converter = new Jinja2Converter();
}

/**
 * destructor
 */
KitsuneJinja2Converter::~KitsuneJinja2Converter()
{
    delete m_converter;
}

/**
Public convert-method for the external using. At first it parse the template-string
 * and then it merge the parsed information with the content of the json-input.
 *
 * @param templateString jinj2-formated string
 * @param input json-object with the information, which should be filled in the jinja2-template
 * @return Pair of string and boolean where the boolean shows
 *         if the parsing and converting were successful
 *         and the string contains the output-string, if the search was successful
 *         else the string contains the error-message
 */
std::pair<std::string, bool>
KitsuneJinja2Converter::convert(const std::string &templateString,
                                Json::JsonObject *jsonInput)
{
    return m_converter->convert(templateString, jsonInput);
}

}  // namespace Jinja2
}  // namespace Kitsune
