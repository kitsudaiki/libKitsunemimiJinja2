/**
 *  @file    jinja2ParserInterface.cpp
 *
 *  @author  Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 *  @copyright MIT License
*/

#include <jinja2_parsing/jinja2_parser_interface.h>
#include <jinja2_parser.h>
#include <libKitsunemimiCommon/methods/string_methods.h>

# define YY_DECL \
    Kitsunemimi::Jinja2::Jinja2Parser::symbol_type jinja2lex (Kitsunemimi::Jinja2::Jinja2ParserInterface& driver)
YY_DECL;

namespace Kitsunemimi
{
namespace Jinja2
{

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
Jinja2ParserInterface::parse(const std::string &inputString,
                             ErrorContainer &error)
{
    // init global values
    m_inputString = inputString;
    m_errorMessage = "";
    m_output = nullptr;

    // run parser-code
    this->scan_begin(inputString);
    Kitsunemimi::Jinja2::Jinja2Parser parser(*this);
    int res = parser.parse();
    this->scan_end();


    // handle negative result
    if(res != 0
            || m_errorMessage.size() > 0)
    {
        error.addMeesage(m_errorMessage);
        LOG_ERROR(error);
        return false;
    }

    return true;
}

/**
 * Is called for the parser after successfully parsing the input-string
 *
 * @param output parser-output as DataArray
 */
void
Jinja2ParserInterface::setOutput(Jinja2Item *output)
{
     m_output = output;
}

/**
 * getter for the json-output of the parser
 *
 * @return parser-output as DataArray
 */
Jinja2Item*
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
Jinja2ParserInterface::error(const Kitsunemimi::Jinja2::location& location,
                             const std::string& message)
{
    if(m_errorMessage.size() > 0) {
        return;
    }

    // get the broken part of the parsed string
    const uint32_t errorStart = location.begin.column;
    const uint32_t errorLength = location.end.column - location.begin.column;
    const uint32_t linenumber = location.begin.line;

    std::vector<std::string> splittedContent;
    splitStringByDelimiter(splittedContent, m_inputString, '\n');

    // build error-message
    m_errorMessage =  "ERROR while parsing jinja2-formated string \n";
    m_errorMessage += "parser-message: " + message + " \n";
    m_errorMessage += "line-number: " + std::to_string(linenumber) + " \n";

    if(splittedContent[linenumber - 1].size() > errorStart - 1 + errorLength)
    {
        m_errorMessage.append("position in line: " +  std::to_string(location.begin.column) + "\n");
        m_errorMessage.append("broken part in string: \""
                              + splittedContent[linenumber - 1].substr(errorStart - 1, errorLength)
                              + "\"");
    }
    else
    {
        m_errorMessage.append("position in line: UNKNOWN POSITION (maybe a string was not closed)");
    }
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
}  // namespace Kitsunemimi
