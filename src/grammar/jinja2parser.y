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
#include <jsonItems.hpp>

namespace Kitsune
{
namespace Jinja2
{

class Jinja2ParserInterface;
using namespace Kitsune::Json;

}  // namespace Jinja2
}  // namespace Kitsune
}

// The parsing context.
%param { Kitsune::Jinja2::Jinja2ParserInterface& driver }

%locations

%code
{
#include <jinja2_parsing/jinja2ParserInterface.hpp>
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

%type  <JsonArray*> part
%type  <JsonObject*> replace_rule
%type  <JsonArray*> json_path
%type  <std::string> defaultroute

%type  <JsonObject*> if_condition
%type  <JsonObject*> if_condition_start

%type  <JsonObject*> for_loop
%type  <JsonObject*> for_loop_start

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
        JsonObject* textItem = new JsonObject();
        textItem->insert("type", new JsonValue("text"));
        textItem->insert("content", new JsonValue($2));

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
        JsonObject* textItem = new JsonObject();
        textItem->insert("type", new JsonValue("text"));
        textItem->insert("content", new JsonValue($1));

        JsonArray* tempItem = new JsonArray();
        tempItem->append(textItem);
        $$ = tempItem;
    }

replace_rule:
    expression_start json_path expression_end
    {
        JsonObject* result = new JsonObject();
        result->insert("type", new JsonValue("replace"));
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
       JsonObject* result = new JsonObject();
       result->insert("type", new JsonValue("if"));
       result->insert("condition", $1);
       result->insert("if", $2);
       result->insert("else", $4);
       $$ = result;
   }
|
   if_condition_start part if_condition_end
   {
       JsonObject* result = new JsonObject();
       result->insert("type", new JsonValue("if"));
       result->insert("condition", $1);
       result->insert("if", $2);
       $$ = result;
   }

if_condition_start:
    expression_sp_start "if" json_path "is" "identifier" expression_sp_end
    {
        JsonObject* result = new JsonObject();
        result->insert("json", $3);
        result->insert("compare", new JsonValue($5));
        $$ = result;
    }
|
    expression_sp_start "if" json_path "is" "number" expression_sp_end
    {
        JsonObject* result = new JsonObject();
        result->insert("json", $3);
        result->insert("compare", new JsonValue($5));
        $$ = result;
    }
|
    expression_sp_start "if" json_path expression_sp_end
    {
        JsonObject* result = new JsonObject();
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
        JsonObject* result = new JsonObject();
        result->insert("type", new JsonValue("forloop"));
        result->insert("loop", $1);
        result->insert("content", $2);
        $$ = result;
    }

for_loop_start:
    expression_sp_start "for" "identifier" "in" json_path expression_sp_end
    {
        JsonObject* result = new JsonObject();
        result->insert("loop_var", new JsonValue($3));
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
        $1->append(new JsonValue($3));
        $$ = $1;
    }
|
    "identifier"
    {
        JsonArray* tempItem = new JsonArray();
        tempItem->append(new JsonValue($1));
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
