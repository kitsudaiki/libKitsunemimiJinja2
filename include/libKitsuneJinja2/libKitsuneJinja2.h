/**
 *  @file    libKitsuneJinja2.h
 *  @version 0.1.0
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 */

#ifndef LIBKITSUNEJINJA2_H
#define LIBKITSUNEJINJA2_H

#include <utility>
#include <string>
#include <jsonObjects.h>

namespace Kitsune
{
namespace Jinja2
{
class Jinja2Converter;

class KitsuneJinja2Converter
{
public:
    KitsuneJinja2Converter();
    ~KitsuneJinja2Converter();

    std::pair<std::string, bool> convert(const std::string &templateString,
                                         Kitsune::Json::JsonObject *jsonInput);

private:
    Jinja2Converter* m_converter = nullptr;
};

}  // namespace Jinja2
}  // namespace Kitsune

#endif // LIBKITSUNEJINJA2_H
