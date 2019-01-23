/**
 *  @file    jinja2ParserInterface.h
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 */

#ifndef JINJA2PARSERINTERFACE_HH
#define JINJA2PARSERINTERFACE_HH

#include <vector>
#include <string>
#include <jsonObjects.h>

#include <iostream>

namespace Kitsune
{
namespace Jinja2
{
class location;

class Jinja2ParserInterface
{

public:
    Jinja2ParserInterface(const bool traceParsing);

    // connection the the scanner and parser
    void scan_begin(const std::string &inputString);
    void scan_end();
    bool parse(const std::string &inputString);

    // output-handling
    void setOutput(Json::JsonArray* output);
    Json::JsonArray* getOutput() const;

    // Error handling.
    void error(const Kitsune::Jinja2::location &location,
               const std::string& message);
    std::string getErrorMessage() const;

    // static variables, which are used in lexer and parser
    static bool m_inRule;

private:
    Json::JsonArray* m_output;
    std::string m_errorMessage = "";
    std::string m_inputString = "";

    bool m_traceParsing = false;
};

}  // namespace Jinja2
}  // namespace Kitsune

#endif // ! JINJA2PARSERINTERFACE_HH
