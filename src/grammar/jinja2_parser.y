/**
 *  @file    jinja2parser.y
 *
 *  @author  Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 *  @copyright MIT License
*/

%skeleton "lalr1.cc"

%defines

//requires 3.2 to avoid the creation of the stack.hh
%require "3.0.4"
%define parser_class_name {Jinja2Parser}

%define api.prefix {jinja2}
%define api.namespace {Kitsunemimi::Jinja2}
%define api.token.constructor
%define api.value.type variant

%define parse.assert
%define parse.trace
%debug

%code requires
{
#include <string>
#include <iostream>
#include <vector>
#include <libKitsunemimiCommon/common_items/data_items.h>
#include <jinja2_items.h>

using Kitsunemimi::Common::DataItem;
using Kitsunemimi::Common::DataArray;
using Kitsunemimi::Common::DataValue;
using Kitsunemimi::Common::DataMap;

namespace Kitsunemimi
{
namespace Jinja2
{

class Jinja2ParserInterface;

}  // namespace Jinja2
}  // namespace Kitsunemimi
}

// The parsing context.
%param { Kitsunemimi::Jinja2::Jinja2ParserInterface& driver }

%locations

%code
{
#include <jinja2_parsing/jinja2_parser_interface.h>
# undef YY_DECL
# define YY_DECL \
    Kitsunemimi::Jinja2::Jinja2Parser::symbol_type jinja2lex (Kitsunemimi::Jinja2::Jinja2ParserInterface& driver)
YY_DECL;
}

// Token
%define api.token.prefix {Jinja2_}
%token
    END  0  "end of file"
    BLANK  " "
    LINEBREAK  "\n"
    EXPRESTART  "{{"
    EXPREEND  "}}"
    EXPRESTART_SP  "{%"
    EXPREEND_SP  "%}"
    DOT  "."
    BRACKOPEN  "["
    BRACKCLOSE  "]"
    IS  "is"
    IN  "in"
    IF  "if"
    FOR  "for"
    ENDFOR  "endfor"
    ELSE  "else"
    ENDIF  "endif"
;


%token <std::string> DEFAULTRULE "defaultrule"
%token <std::string> IDENTIFIER "identifier"
%token <long> NUMBER "number"

%type <std::string> blancs
%type <std::string> linebreaks

%type  <Jinja2Item*> part
%type  <Jinja2Item*> replace_rule
%type  <DataArray*> json_path
%type  <std::string> defaultroute

%type  <Jinja2Item*> if_condition_start

%type  <Jinja2Item*> for_loop
%type  <Jinja2Item*> for_loop_start

%%
%start startpoint;


startpoint:
    part
    {
        driver.setOutput($1->startPoint);
    }

part:
    part replace_rule
    {
        $1->next = $2;
        $2->startPoint = $1->startPoint;

        $$ = $2;
    }
|
    part if_condition_start part if_condition_end
    {
        IfItem* tempItem = dynamic_cast<IfItem*>($2);
        tempItem->ifChild = $3->startPoint;

        $1->next = tempItem;
        tempItem->startPoint = $1->startPoint;

        $$ = tempItem;
    }
|
    part if_condition_start part if_condition_else part if_condition_end
    {
        IfItem* tempItem = dynamic_cast<IfItem*>($2);
        tempItem->ifChild = $3->startPoint;
        tempItem->elseChild = $5->startPoint;

        $1->next = tempItem;
        tempItem->startPoint = $1->startPoint;

        $$ = tempItem;
    }
|
    part for_loop_start part for_loop_end
    {
        ForLoopItem* tempItem = dynamic_cast<ForLoopItem*>($2);
        tempItem->forChild = $3->startPoint;

        $1->next = tempItem;
        tempItem->startPoint = $1->startPoint;

        $$ = tempItem;
    }
|
    part defaultroute
    {
        TextItem* tempItem = new TextItem();
        tempItem->text = $2;

        $1->next = tempItem;
        tempItem->startPoint = $1->startPoint;

        $$ = tempItem;
    }
|
    replace_rule
    {
        $1->startPoint = $1;
        $$ = $1;
    }
|
    if_condition_start part if_condition_end
    {
        IfItem* tempItem = dynamic_cast<IfItem*>($1);
        tempItem->ifChild = $2->startPoint;
        tempItem->startPoint = tempItem;
        $$ = tempItem;
    }

|
    if_condition_start part if_condition_else part if_condition_end
    {
        IfItem* tempItem = dynamic_cast<IfItem*>($1);
        tempItem->ifChild = $2->startPoint;
        tempItem->elseChild = $4->startPoint;
        tempItem->startPoint = tempItem;
        $$ = tempItem;
    }
|
    for_loop_start part for_loop_end
    {
        ForLoopItem* tempItem = dynamic_cast<ForLoopItem*>($1);
        tempItem->forChild = $2->startPoint;
        tempItem->startPoint = tempItem;
        $$ = tempItem;
    }
|
    defaultroute
    {
        TextItem* tempItem = new TextItem();
        tempItem->text = $1;
        tempItem->startPoint = tempItem;
        $$ = tempItem;
    }

replace_rule:
    "{{" blancs json_path blancs "}}"
    {
        ReplaceItem* result = new ReplaceItem();
        result->iterateArray = *$3;
        $$ = result;
    }

if_condition_start:
    "{%" blancs "if" blancs json_path blancs "is" blancs "identifier" blancs "%}"
    {
        IfItem* result = new IfItem();
        result->leftSide = *$5;
        result->rightSide = DataValue($9);
        $$ = result;
    }
|
    "{%" blancs "if" blancs json_path blancs "is" blancs "number" blancs "%}"
    {
        IfItem* result = new IfItem();
        result->leftSide = *$5;
        result->rightSide = DataValue($9);
        $$ = result;
    }
|
    "{%" blancs "if" blancs json_path blancs "%}"
    {
        IfItem* result = new IfItem();
        result->leftSide = *$5;
        result->rightSide = DataValue(true);
        $$ = result;
    }

if_condition_else:
   "{%" blancs "else" blancs "%}"

if_condition_end:
   "{%" blancs "endif" blancs "%}"

for_loop_start:
    "{%" blancs "for" blancs "identifier" blancs "in" blancs json_path blancs "%}"
    {
        ForLoopItem* result = new ForLoopItem();
        result->tempVarName = $5;
        result->iterateArray = *$9;
        $$ = result;
    }

for_loop_end:
    "{%" blancs "endfor" blancs "%}"

json_path:
    json_path "." "identifier"
    {
        $1->append(new DataValue($3));
        $$ = $1;
    }
|
    "identifier"
    {
        DataArray* tempItem = new DataArray();
        tempItem->append(new DataValue($1));
        $$ = tempItem;
    }

defaultroute:
    linebreaks
    {
        $$ = $1;
    }
|
    "in"
    {
        $$ = "in";
    }
|
    "is"
    {
        $$ = "is";
    }
|
    "for"
    {
        $$ = "for";
    }
|
    "if"
    {
        $$ = "if";
    }
|
    "endfor"
    {
        $$ = "endfor";
    }
|
    "else"
    {
        $$ = "else";
    }
|
    "endif"
    {
        $$ = "endif";
    }
|
    "["
    {
        $$ = "[";
    }
|
    "]"
    {
        $$ = "]";
    }
|
    "."
    {
        $$ = ".";
    }
|
    "number"
    {
        $$ = std::to_string($1);
    }
|
    "identifier"
    {
        $$ = $1;
    }
|
    "defaultrule"
    {
        $$ = $1;
    }
|
    blancs
    {
        $$ = $1;
    }

blancs:
    blancs " "
    {
        $$ = $1 + " ";
    }
|
    " "
    {
        $$ = std::string(" ");
    }
|
    %empty
    {
        $$ = std::string();
    }

linebreaks:
    linebreaks "\n"
    {
        $$ = $1 + "\n";
    }
|
    "\n"
    {
        $$ = std::string("\n");
    }
%%

void Kitsunemimi::Jinja2::Jinja2Parser::error(const Kitsunemimi::Jinja2::location& location,
                                          const std::string& message)
{
    driver.error(location, message);
}
