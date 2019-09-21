/**
 *  @file    jinja2ParserInterface.cpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  MIT License
 */

#include <jinja2_parsing/jinja2_parser_interface.h>
#include <jinja2_parser.h>

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
 * @param output parser-output as Common::DataArray
 */
void
Jinja2ParserInterface::setOutput(Jinja2Item *output)
{
     //m_output = output;
}

/**
 * getter for the json-output of the parser
 *
 * @return parser-output as Common::DataArray
 */
Common::DataArray*
Jinja2ParserInterface::getOutput() const
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
    const uint32_t errorStart = location.begin.column;
    const uint32_t errorLength = location.end.column - location.begin.column;
    const std::string errorStringPart = m_inputString.substr(errorStart, errorLength);

    // build error-message
    m_errorMessage =  "error while parsing jinja2-template \n";
    m_errorMessage += "parser-message: " + message + " \n";
    m_errorMessage += "line-number: " + std::to_string(location.begin.line) + " \n";
    m_errorMessage += "position in line: " + std::to_string(location.begin.column) + " \n";
    m_errorMessage += "broken part in template: \"" + errorStringPart + "\" \n";
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
