/**
 *  @file    libKitsuneJinja2.h
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  MIT License
 */

#ifndef LIBKITSUNEJINJA2_H
#define LIBKITSUNEJINJA2_H

#include <utility>
#include <string>
#include <jsonItems.h>

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
