/**
 *  @file    jinja2Converter.cpp
 *
 *  @author  Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 *  @copyright MIT License
*/

#include <libKitsunemimiJinja2/jinja2_converter.h>

#include <jinja2_parsing/jinja2_parser_interface.h>
#include <libKitsunemimiJson/json_item.h>

#include <jinja2_items.h>

using Kitsunemimi::DataItem;
using Kitsunemimi::DataArray;
using Kitsunemimi::DataValue;
using Kitsunemimi::DataMap;

namespace Kitsunemimi
{
namespace Jinja2
{

/**
 * @brief Iconstructor
 */
Jinja2Converter::Jinja2Converter(const bool traceParsing)
{
    m_driver = new Jinja2ParserInterface(traceParsing);
}

/**
 * @brief Idestructor which only deletes the parser-interface to avoid memory-lead
 */
Jinja2Converter::~Jinja2Converter()
{
    delete m_driver;
}

/**
 * @brief convert-method for the external using to fill a jinja2-formated template
 *
 * @param templateString jinj2-formated string
 * @param jsonInput json-formated string with the information,
 *                  which should be filled in the jinja2-template
 * @param errorMessage reference for error-message output
 *
 * @return Pair of boolean and string where the boolean shows
 *         success: first is true and second contains the converted string
 *         failed: first is false and second contains the error-message
 */
const std::pair<bool, std::string>
Jinja2Converter::convert(const std::string &templateString,
                         const std::string &jsonInput,
                         std::string &errorMessage)
{
    Kitsunemimi::Json::JsonItem item;

    bool result = item.parse(jsonInput, errorMessage);

    if(result == false) {
        return std::pair<bool, std::string>(result, "");
    }

    return convert(templateString,
                   item.getItemContent()->copy()->toMap(),
                   errorMessage);
}

/**
 * @brief convert-method for the external using to fill a jinja2-formated template
 *
 * @param templateString jinj2-formated string
 * @param input data-object with the information, which should be filled in the jinja2-template
 * @param errorMessage reference for error-message output
 *
 * @return Pair of boolean and string where the boolean shows
 *         success: first is true and second contains the converted string
 *         failed: first is false and second contains the error-message
 */
const std::pair<bool, std::string>
Jinja2Converter::convert(const std::string &templateString,
                         DataMap* input,
                         std::string &errorMessage)
{
    std::pair<bool, std::string> result;
    m_lock.lock();
    // parse jinja2-template into a json-tree
    result.first = m_driver->parse(templateString);

    // process a failure
    if(result.first == false)
    {
        errorMessage = m_driver->getErrorMessage();
        m_lock.unlock();
        return result;
    }

    // convert the json-tree from the parser into a string
    // by filling the input into it
    Jinja2Item* output = m_driver->getOutput();
    result.first = processItem(input,
                               output,
                               result.second,
                               errorMessage);

    delete output;
    m_lock.unlock();

    return result;
}

/**
 * @brief Process a json-array, which is a list of parsed parts of the jinja2-template
 *
 * @param input The json-object with the items, which sould be filled in the template
 * @param part The Jinja2Item with the jinja2-content
 * @param output
 * @param errorMessage reference for error-message output
 *
 * @return true, if step was successful, else false
 */
bool
Jinja2Converter::processItem(DataMap* input,
                             Jinja2Item* part,
                             std::string &output,
                             std::string &errorMessage)
{
    if(part == nullptr) {
        return true;
    }

    //------------------------------------------------------
    if(part->getType() == Jinja2Item::TEXT_ITEM)
    {
        TextItem* textItem = dynamic_cast<TextItem*>(part);
        output.append(textItem->text);
        return processItem(input, part->next, output, errorMessage);
    }
    //------------------------------------------------------
    if(part->getType() == Jinja2Item::REPLACE_ITEM)
    {
        ReplaceItem* replaceItem = dynamic_cast<ReplaceItem*>(part);
        if(processReplace(input, replaceItem, output, errorMessage) == false) {
            return false;
        }
    }
    //------------------------------------------------------
    if(part->getType() == Jinja2Item::IF_ITEM)
    {
        IfItem* ifItem = dynamic_cast<IfItem*>(part);
        if(processIfCondition(input, ifItem, output, errorMessage) == false) {
            return false;
        }
    }
    //------------------------------------------------------
    if(part->getType() == Jinja2Item::FOR_ITEM)
    {
        ForLoopItem* forLoopItem = dynamic_cast<ForLoopItem*>(part);
        if(processForLoop(input, forLoopItem, output, errorMessage) == false) {
            return false;
        }
    }
    //------------------------------------------------------

    return true;
}

/**
 * @brief Resolve an replace-rule of the parsed jinja2-template
 *
 * @param input The json-object with the items, which sould be filled in the template
 * @param replaceObject ReplaceItem with the replacement-information
 * @param output Pointer to the output-string for the result of the convertion
 * @param errorMessage reference for error-message output
 *
 * @return true, if step was successful, else false
 */
bool
Jinja2Converter::processReplace(DataMap* input,
                                ReplaceItem* replaceObject,
                                std::string &output,
                                std::string &errorMessage)
{
    // get information
    std::pair<std::string, bool> jsonValue = getString(input, &replaceObject->iterateArray);

    // process a failure
    if(jsonValue.second == false)
    {
        errorMessage = createErrorMessage(&replaceObject->iterateArray);
        return false;
    }

    // insert the replacement
    output.append(jsonValue.first);

    return processItem(input, replaceObject->next, output, errorMessage);
}

/**
 * @brief Resolve an if-condition of the parsed jinja2-template
 *
 * @param input The json-object with the items, which sould be filled in the template
 * @param ifCondition Jinja2Item with the if-condition-information
 * @param output Pointer to the output-string for the result of the convertion
 *
 * @return true, if step was successful, else false
 */
bool
Jinja2Converter::processIfCondition(DataMap* input,
                                    IfItem* ifCondition,
                                    std::string &output,
                                    std::string &errorMessage)
{
    // get information
    std::pair<std::string, bool> jsonValue = getString(input, &ifCondition->leftSide);

    // process a failure
    if(jsonValue.second == false)
    {
        errorMessage = createErrorMessage(&ifCondition->leftSide);
        return false;
    }

    // run the if-condition of the jinja2-template
    if(jsonValue.first == ifCondition->rightSide.toString()
        || jsonValue.first == "True"
        || jsonValue.first == "true")
    {
        processItem(input, ifCondition->ifChild, output, errorMessage);
    }
    else
    {
        if(ifCondition->elseChild != nullptr) {
            processItem(input, ifCondition->elseChild, output, errorMessage);
        }
    }

    return processItem(input, ifCondition->next, output, errorMessage);
}

/**
 * @brief Resolve an for-loop of the parsed jinja2-template
 *
 * @param input The json-object with the items, which sould be filled in the template
 * @param forLoop ForLoopItem with the loop-information
 * @param output Pointer to the output-string for the result of the convertion
 *
 * @return true, if step was successful, else false
 */
bool
Jinja2Converter::processForLoop(DataMap* input,
                                ForLoopItem* forLoop,
                                std::string &output,
                                std::string &errorMessage)
{
    // get information
    std::pair<DataItem*, bool> jsonValue = getItem(input, &forLoop->iterateArray);

    // process a failure
    if(jsonValue.second == false)
    {
        errorMessage = createErrorMessage(&forLoop->iterateArray);
        return false;
    }

    // loop can only work on json-arrays
    if(jsonValue.first->getType() != DataItem::ARRAY_TYPE)
    {
        // TODO: error-message
        return false;
    }

    // run the loop of the jinja2-template
    DataArray* array = jsonValue.first->toArray();
    for(uint32_t i = 0; i < array->size(); i++)
    {
        DataMap* tempLoopInput = input;
        tempLoopInput->insert(forLoop->tempVarName,
                              array->get(i), true);

        if(processItem(tempLoopInput, forLoop->forChild, output, errorMessage) == false) {
            return false;
        }
    }

    return processItem(input, forLoop->next, output, errorMessage);
}

/**
 * @brief Search a specific string-typed item in the json-input
 *
 * @param input The json-object in which the item sould be searched
 * @param jsonPath Path item in the json-object. It is a DataArray
 *                 which contains only string-objects
 *
 * @return Pair of string and boolean where the boolean shows
 *         if the item was found and is a string-type
 *         and the string contains the item, if the search was successful
 */
const std::pair<std::string, bool>
Jinja2Converter::getString(DataMap* input,
                           DataArray* jsonPath)
{
    // init
    std::pair<std::string, bool> result;
    result.second = false;

    // make a generic item-search and than try to convert to string
    std::pair<DataItem*, bool> item = getItem(input, jsonPath);
    if(item.second == false) {
        return result;
    }

    result.second = true;
    result.first = item.first->toString(true);

    return result;
}

/**
 * @brief Search a specific item in the json-input
 *
 * @param input The json-object in which the item sould be searched
 * @param jsonPath Path item in the json-object. It is a DataArray
 *                 which contains only string-objects
 *
 * @return Pair of json-value and boolean where the boolean shows
 *         if the item was found and the json-value contains the item,
 *         if the search was successful
 */
const std::pair<DataItem*, bool>
Jinja2Converter::getItem(DataMap* input,
                         DataArray* jsonPath)
{
    // init
    std::pair<DataItem*, bool> result;
    result.second = false;

    // search for the item
    DataItem* tempJson = input;
    for(uint32_t i = 0; i < jsonPath->size(); i++)
    {
        tempJson = tempJson->get(jsonPath->get(i)->toString());
        if(tempJson == nullptr) {
            return result;
        }
    }

    result.second = true;
    result.first = tempJson;

    return result;
}

/**
 * @brief Is called, when an error occurs while compiling and generates an error-message for output
 *
 * @param jsonPath path within the json-object to the item which was not found
 *
 * @return error-messaage for the user
 */
const std::string
Jinja2Converter::createErrorMessage(DataArray* jsonPath)
{
    std::string errorMessage = "";
    errorMessage =  "error while converting jinja2-template \n";
    errorMessage += "    can not find item in path in json-input: ";

    // convert jsonPath into a string
    for(uint32_t i = 0; i < jsonPath->size(); i++)
    {
        if(i != 0) {
            errorMessage += ".";
        }
        errorMessage += jsonPath->get(i)->toString();
    }

    errorMessage += "\n";
    errorMessage += "    or maybe the item does not have a valid format";
    errorMessage +=    " or the place where it should be used \n";

    return errorMessage;
}

}  // namespace Jinja2
}  // namespace Kitsunemimi
