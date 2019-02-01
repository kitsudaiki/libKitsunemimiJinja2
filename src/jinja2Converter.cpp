/**
 *  @file    jinja2Converter.cpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  MIT License
 */

#include <jinja2Converter.h>
#include <jinja2_parsing/jinja2ParserInterface.h>

namespace Kitsune
{
namespace Jinja2
{

/**
 * Converter-class for the public usage
 *
 * @param traceParsing If set to true, the scanner prints all triggered rules.
 *                     It is only for better debugging.
 */
Jinja2Converter::Jinja2Converter(const bool traceParsing)
{
    m_driver = new Jinja2ParserInterface(traceParsing);
}

/**
 * destructor which only deletes the parser-interface to avoid memory-lead
 */
Jinja2Converter::~Jinja2Converter()
{
    delete m_driver;
}

/**
 * Public convert-method for the external using. At first it parse the template-string
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
Jinja2Converter::convert(const std::string &templateString,
                         Json::JsonObject *input)
{
    std::pair<std::string, bool> result;

    // parse jinja2-template into a json-tree
    result.second = m_driver->parse(templateString);

    // process a failure
    if(result.second == false)
    {
        result.first = m_driver->getErrorMessage();
        return result;
    }

    // convert the json-tree from the parser into a string
    // by filling the input into it
    Json::JsonArray* output = m_driver->getOutput();
    result.second = processArray(input, output, &result.first);

    return result;
}

/**
 * Process a json-array, which is a list of parsed parts of the jinja2-template
 *
 * @param input The json-object with the items, which sould be filled in the template
 * @param part The json-array with the jinja2-content
 * @return true, if step was successful, else false
 */
bool
Jinja2Converter::processArray(Json::JsonObject *input,
                              Json::JsonArray *part,
                              std::string* output)
{
    for(uint32_t i = 0; i < part->getSize(); i++)
    {
        Json::AbstractJson* tempItem = part->get(i);

        //------------------------------------------------------
        if(tempItem->get("type")->toValue()->getString()== "text")
        {
            output->append(tempItem->get("content")->toValue()->getString());
        }
        //------------------------------------------------------
        if(tempItem->get("type")->toValue()->getString() == "replace")
        {
            if(processReplace(input, tempItem->get("content")->toArray(), output) == false) {
                return false;
            }
        }
        //------------------------------------------------------
        if(tempItem->get("type")->toValue()->getString() == "if")
        {
            if(processIfCondition(input, tempItem->toObject(), output) == false) {
                return false;
            }
        }
        //------------------------------------------------------
        if(tempItem->get("type")->toValue()->getString() == "forloop")
        {
            if(processForLoop(input, tempItem->toObject(), output) == false) {
                return false;
            }
        }
        //------------------------------------------------------
    }

    return true;
}

/**
 * Resolve an replace-rule of the parsed jinja2-template
 *
 * @param input The json-object with the items, which sould be filled in the template
 * @param replaceObject Json-object with the replacement-information
 * @param output Pointer to the output-string for the result of the convertion
 * @return true, if step was successful, else false
 */
bool
Jinja2Converter::processReplace(Json::JsonObject *input,
                                Json::JsonArray *replaceObject,
                                std::string* output)
{
    // get information
    std::pair<std::string, bool> item = getString(input, replaceObject);

    // process a failure
    if(item.second == false)
    {
        output->clear();
        output->append(createErrorMessage(replaceObject));
        return false;
    }

    // insert the replacement
    output->append(item.first);

    return true;
}

/**
 * Resolve an if-condition of the parsed jinja2-template
 *
 * @param input The json-object with the items, which sould be filled in the template
 * @param ifCondition Json-object with the if-condition-information
 * @param output Pointer to the output-string for the result of the convertion
 * @return true, if step was successful, else false
 */
bool
Jinja2Converter::processIfCondition(Json::JsonObject *input,
                                    Json::JsonObject *ifCondition,
                                    std::string *output)
{
    // get information
    Json::JsonObject* condition = ifCondition->get("condition")->toObject();
    std::pair<std::string, bool> item = getString(input, condition->get("json")->toArray());

    // process a failure
    if(item.second == false)
    {
        output->clear();
        output->append(createErrorMessage(condition->get("json")->toArray()));
        return false;
    }

    // run the if-condition of the jinja2-template
    if((condition->get("compare") != nullptr
        && item.first == condition->get("compare")->toValue()->getString())
        || (item.first == "True"))
    {
        processArray(input, ifCondition->get("if")->toArray(), output);
    }
    else
    {
        if(ifCondition->get("else") != nullptr) {
            processArray(input, ifCondition->get("else")->toArray(), output);
        }
    }

    return true;
}

/**
 * Resolve an for-loop of the parsed jinja2-template
 *
 * @param input The json-object with the items, which sould be filled in the template
 * @param forLoop Json-object with the loop-information
 * @param output Pointer to the output-string for the result of the convertion
 * @return true, if step was successful, else false
 */
bool
Jinja2Converter::processForLoop(Json::JsonObject *input,
                                Json::JsonObject *forLoop,
                                std::string *output)
{
    // get information
    Json::JsonObject* loop = forLoop->get("loop")->toObject();
    std::pair<Json::AbstractJson*, bool> item = getItem(input, loop->get("json")->toArray());

    // process a failure
    if(item.second == false)
    {
        output->clear();
        output->append(createErrorMessage(loop->get("json")->toArray()));
        return false;
    }

    // loop can only work on json-arrays
    if(item.first->getType() != Json::AbstractJson::ARRAY_TYPE) {
        return false;
    }

    // run the loop of the jinja2-template
    Json::JsonArray* array = item.first->toArray();
    for(uint32_t i = 0; i < array->getSize(); i++)
    {
        Json::JsonObject* tempLoopInput = input;
        tempLoopInput->insert(loop->get("loop_var")->toValue()->getString(),
                              array->get(i), true);

        if(processArray(tempLoopInput, forLoop->get("content")->toArray(), output) == false) {
            return false;
        }
    }

    return true;
}

/**
 * Search a specific string-typed item in the json-input
 *
 * @param input The json-object in which the item sould be searched
 * @param jsonPath Path item in the json-object. It is a Json::JsonArray
 *                 which contains only string-objects
 * @return Pair of string and boolean where the boolean shows
 *         if the item was found and is a string-type
 *         and the string contains the item, if the search was successful
 */
std::pair<std::string, bool>
Jinja2Converter::getString(Json::JsonObject *input,
                           Json::JsonArray *jsonPath)
{
    // init
    std::pair<std::string, bool> result;
    result.second = false;

    // make a generic item-search and than try to convert to string
    std::pair<Json::AbstractJson*, bool> item = getItem(input, jsonPath);
    if(item.second == false) {
        return result;
    }

    if(item.first->getType() == Json::AbstractJson::STRING_TYPE)
    {
        result.second = item.second;
        result.first = item.first->toValue()->getString();
    }

    if(item.first->getType() == Json::AbstractJson::INT_TYPE)
    {
        result.second = item.second;
        const int intValue = item.first->toValue()->getInt();
        result.first = std::to_string(intValue);
    }

    return result;
}

/**
 * Search a specific item in the json-input
 *
 * @param input The json-object in which the item sould be searched
 * @param jsonPath Path item in the json-object. It is a Json::JsonArray
 *                 which contains only string-objects
 * @return Pair of json-value and boolean where the boolean shows
 *         if the item was found and the json-value contains the item,
 *         if the search was successful
 */
std::pair<Json::AbstractJson*, bool>
Jinja2Converter::getItem(Json::JsonObject *input,
                         Json::JsonArray *jsonPath)
{
    // init
    std::pair<Json::AbstractJson*, bool> result;
    result.second = false;

    // search for the item
    Json::AbstractJson* tempJson = input;
    for(uint32_t i = 0; i < jsonPath->getSize(); i++)
    {
        tempJson = tempJson->get(jsonPath->get(i)->toValue()->getString());
        if(tempJson == nullptr) {
            return result;
        }
    }
    result.second = true;
    result.first = tempJson;
    return result;
}

/**
 * Is called, when an error occurs while compiling and generates an error-message for output
 *
 * @param jsonPath path within the json-object to the item which was not found
 * @return error-messaage for the user
 */
std::string
Jinja2Converter::createErrorMessage(Json::JsonArray *jsonPath)
{
    std::string errorMessage = "";
    errorMessage =  "error while converting jinja2-template \n";
    errorMessage += "    can not find item in path in json-input: ";

    // convert jsonPath into a string
    for(uint32_t i = 0; i < jsonPath->getSize(); i++)
    {
        if(i != 0) {
            errorMessage += ".";
        }
        //errorMessage.append(jsonPath->get(i));
    }

    errorMessage += "\n";
    errorMessage += "    or maybe the item does not have a valid format";
    errorMessage +=    " or the place where it should be used \n";

    return errorMessage;
}

}  // namespace Jinja2
}  // namespace Kitsune
