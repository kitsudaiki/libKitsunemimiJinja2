/**
 *  @file    jinja2parser.y
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  MIT License
 */

%skeleton "lalr1.cc"

%defines

//requires 3.2 to avoid the creation of the stack.hh
%require "3.0.4"
%define parser_class_name {Jinja2Parser}

%define api.prefix {jinja2}
%define api.namespace {Kitsune::Jinja2}
%define api.token.constructor
%define api.value.type variant

%define parse.assert

%code requires
{
#include <string>
#include <iostream>
#include <vector>
#include <common_items/data_items.h>

using Kitsune::Common::DataItem;
using Kitsune::Common::DataArray;
using Kitsune::Common::DataValue;
using Kitsune::Common::DataMap;

namespace Kitsune
{
namespace Jinja2
{

class Jinja2ParserInterface;

}  // namespace Jinja2
}  // namespace Kitsune
}

// The parsing context.
%param { Kitsune::Jinja2::Jinja2ParserInterface& driver }

%locations

%code
{
#include <jinja2_parsing/jinja2_parser_interface.h>
# undef YY_DECL
# define YY_DECL \
    Kitsune::Jinja2::Jinja2Parser::symbol_type jinja2lex (Kitsune::Jinja2::Jinja2ParserInterface& driver)
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
%token <int> NUMBER "number"

%type  <DataArray*> part
%type  <DataMap*> replace_rule
%type  <DataArray*> json_path
%type  <std::string> defaultroute

%type  <DataMap*> if_condition
%type  <DataMap*> if_condition_start

%type  <DataMap*> for_loop
%type  <DataMap*> for_loop_start

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
        DataMap* textItem = new DataMap();
        textItem->insert("type", new DataValue("text"));
        textItem->insert("content", new DataValue($2));

        $1->append(textItem);
        $$ = $1;
    }
|
    part replace_rule
    {
        $1->append($2);
        $$ = $1;
    }
|
    part if_condition
    {
        $1->append($2);
        $$ = $1;
    }
|
    part for_loop
    {
        $1->append($2);
        $$ = $1;
    }
|
    defaultroute
    {
        DataMap* textItem = new DataMap();
        textItem->insert("type", new DataValue("text"));
        textItem->insert("content", new DataValue($1));

        DataArray* tempItem = new DataArray();
        tempItem->append(textItem);
        $$ = tempItem;
    }

replace_rule:
    expression_start json_path expression_end
    {
        DataMap* result = new DataMap();
        result->insert("type", new DataValue("replace"));
        result->insert("content", $2);
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
       DataMap* result = new DataMap();
       result->insert("type", new DataValue("if"));
       result->insert("condition", $1);
       result->insert("if", $2);
       result->insert("else", $4);
       $$ = result;
   }
|
   if_condition_start part if_condition_end
   {
       DataMap* result = new DataMap();
       result->insert("type", new DataValue("if"));
       result->insert("condition", $1);
       result->insert("if", $2);
       $$ = result;
   }

if_condition_start:
    expression_sp_start "if" json_path "is" "identifier" expression_sp_end
    {
        DataMap* result = new DataMap();
        result->insert("json", $3);
        result->insert("compare", new DataValue($5));
        $$ = result;
    }
|
    expression_sp_start "if" json_path "is" "number" expression_sp_end
    {
        DataMap* result = new DataMap();
        result->insert("json", $3);
        result->insert("compare", new DataValue($5));
        $$ = result;
    }
|
    expression_sp_start "if" json_path expression_sp_end
    {
        DataMap* result = new DataMap();
        result->insert("json", $3);
        $$ = result;
    }

if_condition_else:
    expression_sp_start "else" expression_sp_end

if_condition_end:
    expression_sp_start "endif" expression_sp_end

for_loop:
    for_loop_start part for_loop_end
    {
        DataMap* result = new DataMap();
        result->insert("type", new DataValue("forloop"));
        result->insert("loop", $1);
        result->insert("content", $2);
        $$ = result;
    }

for_loop_start:
    expression_sp_start "for" "identifier" "in" json_path expression_sp_end
    {
        DataMap* result = new DataMap();
        result->insert("loop_var", new DataValue($3));
        result->insert("json", $5);
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

void Kitsune::Jinja2::Jinja2Parser::error(const Kitsune::Jinja2::location& location,
                                              const std::string& message)
{
    driver.error(location, message);
}
