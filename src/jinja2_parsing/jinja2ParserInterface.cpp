/**
 *  @file    jinja2ParserInterface.cpp
 *  @version 0.1.0
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 */

#include <jinja2_parsing/jinja2ParserInterface.h>
#include <jinja2parser.h>
#include <jinja2Converter.h>

# define YY_DECL \
    Kitsune::Jinja2::Jinja2Parser::symbol_type jinja2lex (Kitsune::Jinja2::Jinja2ParserInterface& driver)
YY_DECL;

namespace Kitsune
{
namespace Jinja2
{

bool Jinja2ParserInterface::m_inRule = false;

/**
 * The class is the interface for the bison-generated parser.
 * It starts the parsing-process and store the returned values.
 *
 * @param traceParsing If set to true, the scanner prints all triggered rules.
 *                     It is only for better debugging.
 */
Jinja2ParserInterface::Jinja2ParserInterface(const bool traceParsing)
{
    m_traceParsing = traceParsing;
}

/**
 * Start the scanner and parser
 *
 * @param inputFile string which should be parsed
 * @return true, if parsing was successful, else false
 */
bool
Jinja2ParserInterface::parse(const std::string &inputString)
{
    // init static variables for new run
    m_inRule = false;

    // init global values
    m_inputString = inputString;
    m_errorMessage = "";
    m_output = nullptr;

    // run parser-code
    this->scan_begin(inputString);
    Kitsune::Jinja2::Jinja2Parser parser(*this);
    int res = parser.parse();
    this->scan_end();

    if(res != 0) {
        return false;
    }
    return true;
}

/**
 * Is called for the parser after successfully parsing the input-string
 *
 * @param output parser-output as Json::JsonArray
 */
void
Jinja2ParserInterface::setOutput(Json::JsonArray *output)
{
     m_output = output;
}

/**
 * getter for the json-output of the parser
 *
 * @return parser-output as Json::JsonArray
 */
Json::JsonArray *Jinja2ParserInterface::getOutput() const
{
    return m_output;
}

/**
 * Is called from the parser in case of an error
 *
 * @param location location-object of the bison-parser,
 *                 which contains the informations of the location
 *                 of the syntax-error in the parsed string
 * @param message error-specific message from the parser
 */
void
Jinja2ParserInterface::error(const Kitsune::Jinja2::location& location,
                             const std::string& message)
{
    // get the broken part of the parsed string
    // const int errorStart = static_cast<int>(location.begin.column) - 1;
    // const int errorLength = static_cast<int>(location.end.column - location.begin.column);
    // const std::string errorStringPart = m_inputString.mid(errorStart, errorLength);

    // build error-message
    // m_errorMessage =  "error while parsing jinja2-template \n";
    // m_errorMessage += "parser-message: " + std::string(message.c_str()) + " \n";
    // m_errorMessage += "line-number: " + std::string::number(location.begin.line) + " \n";
    // m_errorMessage += "broken part in template: " + errorStringPart + " \n";
}

/**
 * getter fot the error-message in case of an error while parsing
 *
 * @return error-message
 */
std::string
Jinja2ParserInterface::getErrorMessage() const
{
    return m_errorMessage;
}

}  // namespace Jinja2
}  // namespace Kitsune
