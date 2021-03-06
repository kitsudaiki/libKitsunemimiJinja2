%{ /* -*- C++ -*- */
/**
 *  @file    jinja2_lexer.l
 *
 *  @author  Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 *  @copyright MIT License
*/

# include <cerrno>
# include <climits>
# include <cstdlib>
# include <string>
# include <jinja2_parsing/jinja2_parser_interface.h>
# include <jinja2_parser.h>

# undef yywrap
# define yywrap() 1

// The location of the current token.
# ifdef YY_DECL
# undef YY_DECL
# endif
# define YY_DECL \
    Kitsunemimi::Jinja2::Jinja2Parser::symbol_type jinja2lex (Kitsunemimi::Jinja2::Jinja2ParserInterface& driver)
YY_DECL;

Kitsunemimi::Jinja2::location jinja2loc;
%}


%option noyywrap nounput batch debug yylineno prefix="jinja2"

id    [a-zA-Z][a-zA-Z_0-9]*
long   -?([0-9]+)
blank [ \t]

%{
    # define YY_USER_ACTION  jinja2loc.columns (yyleng);
%}

%%

%{
    jinja2loc.step();
%}

" "         return Kitsunemimi::Jinja2::Jinja2Parser::make_BLANK (jinja2loc);
"\n"        return Kitsunemimi::Jinja2::Jinja2Parser::make_LINEBREAK (jinja2loc);
"{{"        return Kitsunemimi::Jinja2::Jinja2Parser::make_EXPRESTART  (jinja2loc);
"}}"        return Kitsunemimi::Jinja2::Jinja2Parser::make_EXPREEND   (jinja2loc);
"{%"        return Kitsunemimi::Jinja2::Jinja2Parser::make_EXPRESTART_SP   (jinja2loc);
"%}"        return Kitsunemimi::Jinja2::Jinja2Parser::make_EXPREEND_SP   (jinja2loc);
"."         return Kitsunemimi::Jinja2::Jinja2Parser::make_DOT(jinja2loc);
"["         return Kitsunemimi::Jinja2::Jinja2Parser::make_BRACKOPEN(jinja2loc);
"]"         return Kitsunemimi::Jinja2::Jinja2Parser::make_BRACKCLOSE(jinja2loc);
"is"        return Kitsunemimi::Jinja2::Jinja2Parser::make_IS(jinja2loc);
"in"        return Kitsunemimi::Jinja2::Jinja2Parser::make_IN(jinja2loc);
"if"        return Kitsunemimi::Jinja2::Jinja2Parser::make_IF(jinja2loc);
"for"       return Kitsunemimi::Jinja2::Jinja2Parser::make_FOR(jinja2loc);
"else"      return Kitsunemimi::Jinja2::Jinja2Parser::make_ELSE(jinja2loc);
"endif"     return Kitsunemimi::Jinja2::Jinja2Parser::make_ENDIF(jinja2loc);
"endfor"    return Kitsunemimi::Jinja2::Jinja2Parser::make_ENDFOR(jinja2loc);

{long}      {
    errno = 0;
    long length = strtol(yytext, NULL, 10);
    if (!(LONG_MIN <= length
        && length <= LONG_MAX
        && errno != ERANGE))
    {
        driver.error(jinja2loc, "integer is out of range");
    }
    return Kitsunemimi::Jinja2::Jinja2Parser::make_NUMBER (length, jinja2loc);
}

{id}       return Kitsunemimi::Jinja2::Jinja2Parser::make_IDENTIFIER(yytext, jinja2loc);
.           return Kitsunemimi::Jinja2::Jinja2Parser::make_DEFAULTRULE(yytext, jinja2loc);
<<EOF>>    return Kitsunemimi::Jinja2::Jinja2Parser::make_END(jinja2loc);

%%


void Kitsunemimi::Jinja2::Jinja2ParserInterface::scan_begin(const std::string &inputString)
{
    Kitsunemimi::Jinja2::location newLoc;
    jinja2loc = newLoc;
    yy_flex_debug = m_traceParsing;
    yy_scan_string(inputString.c_str());
}

void Kitsunemimi::Jinja2::Jinja2ParserInterface::scan_end()
{
    yy_delete_buffer(YY_CURRENT_BUFFER);
}


