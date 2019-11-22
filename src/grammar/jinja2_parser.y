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
    ELSE  "else"
    ENDIF  "endif"
    FOR  "for"
    ENDFOR  "endfor"
;


%token <std::string> DEFAULTRULE "defaultrule"
%token <std::string> IDENTIFIER "identifier"
%token <long> NUMBER "number"

%type  <Jinja2Item*> part
%type  <Jinja2Item*> replace_rule
%type  <DataArray*> json_path
%type  <std::string> defaultroute

%type  <Jinja2Item*> if_condition
%type  <Jinja2Item*> if_condition_start

%type  <Jinja2Item*> for_loop
%type  <Jinja2Item*> for_loop_start

%%
%start startpoint;


startpoint:
    part
    {
        driver.setOutput($1);
    }

part:
    part defaultroute
    {
        TextItem* textItem = new TextItem();
        textItem->text = $2;

        $1->next = textItem;
        $$ = $1;
    }
|
    part replace_rule
    {
        $1->next = $2;
        $$ = $1;
    }
|
    part if_condition
    {
        $1->next = $2;
        $$ = $1;
    }
|
    part for_loop
    {
        $1->next = $2;
        $$ = $1;
    }
|
    defaultroute
    {
        TextItem* textItem = new TextItem();
        textItem->text = $1;
        $$ = textItem;
    }

replace_rule:
    expression_start json_path expression_end
    {
        ReplaceItem* result = new ReplaceItem();
        result->iterateArray = *$2;
        $$ = result;
    }

expression_start:
    "{{"
    {
        Jinja2ParserInterface::m_inRule = true;
    }

expression_end:
    "}}"
    {
        Jinja2ParserInterface::m_inRule = false;
    }

if_condition:
   if_condition_start part if_condition_else part if_condition_end
   {
       IfItem* result = dynamic_cast<IfItem*>($1);
       result->ifChild = $2;
       result->elseChild = $4;
       $$ = result;
   }
|
   if_condition_start part if_condition_end
   {
       IfItem* result = dynamic_cast<IfItem*>($1);
       result->ifChild = $2;
       $$ = result;
   }

if_condition_start:
    expression_sp_start "if" json_path "is" "identifier" expression_sp_end
    {
        IfItem* result = new IfItem();
        result->leftSide = *$3;
        result->rightSide = DataValue($5);
        $$ = result;
    }
|
    expression_sp_start "if" json_path "is" "number" expression_sp_end
    {
        IfItem* result = new IfItem();
        result->leftSide = *$3;
        result->rightSide = DataValue($5);
        $$ = result;
    }
|
    expression_sp_start "if" json_path expression_sp_end
    {
        IfItem* result = new IfItem();
        result->leftSide = *$3;
        result->rightSide = DataValue(true);
        $$ = result;
    }

if_condition_else:
    expression_sp_start "else" expression_sp_end

if_condition_end:
    expression_sp_start "endif" expression_sp_end

for_loop:
    for_loop_start part for_loop_end
    {
        ForLoopItem* result = dynamic_cast<ForLoopItem*>($1);
        result->forChild = $2;
        $$ = result;
    }

for_loop_start:
    expression_sp_start "for" "identifier" "in" json_path expression_sp_end
    {
        ForLoopItem* result = new ForLoopItem();
        result->tempVarName = $3;
        result->iterateArray = *$5;
        $$ = result;
    }

for_loop_end:
    expression_sp_start "endfor" expression_sp_end

expression_sp_start:
    "{%"
    {
        Jinja2ParserInterface::m_inRule = true;
    }

expression_sp_end:
    "%}"
    {
        Jinja2ParserInterface::m_inRule = false;
    }

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
    defaultroute "defaultrule"
    {
        $$ = $1.append($2);
    }
|
    "defaultrule"
    {
        $$ = $1;
    }
|
    %empty
    {
        std::string tempItem = "";
        $$ = tempItem;
    }
%%

void Kitsunemimi::Jinja2::Jinja2Parser::error(const Kitsunemimi::Jinja2::location& location,
                                          const std::string& message)
{
    driver.error(location, message);
}
