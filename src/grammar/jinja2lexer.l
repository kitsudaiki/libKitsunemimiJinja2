/**
 *  @file    jinja2lexer.l
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 */

%{
# include <cerrno>
# include <climits>
# include <cstdlib>
# include <string>
# include <jinja2_parsing/jinja2ParserInterface.h>
# include <jinja2parser.h>

# undef yywrap
# define yywrap() 1


// The location of the current token.
# ifdef YY_DECL
# undef YY_DECL
# endif
# define YY_DECL \
    Kitsune::Jinja2::Jinja2Parser::symbol_type jinja2lex (Kitsune::Jinja2::Jinja2ParserInterface& driver)
YY_DECL;
Kitsune::Jinja2::location jinja2loc;

%}


%option noyywrap nounput batch debug yylineno prefix="jinja2"

id    [a-zA-Z][a-zA-Z_0-9]*
int   [0-9]+
blank [ \t]

%{
    # define YY_USER_ACTION  jinja2loc.columns (yyleng);
%}

%%

%{
    jinja2loc.step();
%}

{blank}+  if(Kitsune::Jinja2::Jinja2ParserInterface::m_inRule) { jinja2loc.step();  } else { return Kitsune::Jinja2::Jinja2Parser::make_DEFAULTRULE(yytext, jinja2loc); }
[\n]      if(Kitsune::Jinja2::Jinja2ParserInterface::m_inRule) { jinja2loc.step(); } else { return Kitsune::Jinja2::Jinja2Parser::make_DEFAULTRULE(yytext, jinja2loc); }
"{{"      return Kitsune::Jinja2::Jinja2Parser::make_EXPRESTART  (jinja2loc);
"}}"      return Kitsune::Jinja2::Jinja2Parser::make_EXPREEND   (jinja2loc);
"{%"      return Kitsune::Jinja2::Jinja2Parser::make_EXPRESTART_SP  (jinja2loc);
"%}"      return Kitsune::Jinja2::Jinja2Parser::make_EXPREEND_SP   (jinja2loc);
"."     if(Kitsune::Jinja2::Jinja2ParserInterface::m_inRule) { return Kitsune::Jinja2::Jinja2Parser::make_DOT(jinja2loc);   } else { return Kitsune::Jinja2::Jinja2Parser::make_DEFAULTRULE(yytext, jinja2loc); }
"["     if(Kitsune::Jinja2::Jinja2ParserInterface::m_inRule) { return Kitsune::Jinja2::Jinja2Parser::make_BRACKOPEN(jinja2loc);   } else { return Kitsune::Jinja2::Jinja2Parser::make_DEFAULTRULE(yytext, jinja2loc); }
"]"     if(Kitsune::Jinja2::Jinja2ParserInterface::m_inRule) { return Kitsune::Jinja2::Jinja2Parser::make_BRACKCLOSE(jinja2loc);  } else { return Kitsune::Jinja2::Jinja2Parser::make_DEFAULTRULE(yytext, jinja2loc); }
"is"     if(Kitsune::Jinja2::Jinja2ParserInterface::m_inRule) { return Kitsune::Jinja2::Jinja2Parser::make_IS(jinja2loc);  } else { return Kitsune::Jinja2::Jinja2Parser::make_DEFAULTRULE(yytext, jinja2loc); }
"in"     if(Kitsune::Jinja2::Jinja2ParserInterface::m_inRule) { return Kitsune::Jinja2::Jinja2Parser::make_IN(jinja2loc);  } else { return Kitsune::Jinja2::Jinja2Parser::make_DEFAULTRULE(yytext, jinja2loc); }
"if"     if(Kitsune::Jinja2::Jinja2ParserInterface::m_inRule) { return Kitsune::Jinja2::Jinja2Parser::make_IF(jinja2loc);  } else { return Kitsune::Jinja2::Jinja2Parser::make_DEFAULTRULE(yytext, jinja2loc); }
"else"     if(Kitsune::Jinja2::Jinja2ParserInterface::m_inRule) { return Kitsune::Jinja2::Jinja2Parser::make_ELSE(jinja2loc);  } else { return Kitsune::Jinja2::Jinja2Parser::make_DEFAULTRULE(yytext, jinja2loc); }
"endif"     if(Kitsune::Jinja2::Jinja2ParserInterface::m_inRule) { return Kitsune::Jinja2::Jinja2Parser::make_ENDIF(jinja2loc);  } else { return Kitsune::Jinja2::Jinja2Parser::make_DEFAULTRULE(yytext, jinja2loc); }
"for"     if(Kitsune::Jinja2::Jinja2ParserInterface::m_inRule) { return Kitsune::Jinja2::Jinja2Parser::make_FOR(jinja2loc);  } else { return Kitsune::Jinja2::Jinja2Parser::make_DEFAULTRULE(yytext, jinja2loc); }
"endfor"     if(Kitsune::Jinja2::Jinja2ParserInterface::m_inRule) { return Kitsune::Jinja2::Jinja2Parser::make_ENDFOR(jinja2loc);  } else { return Kitsune::Jinja2::Jinja2Parser::make_DEFAULTRULE(yytext, jinja2loc); }


{int}      {
    errno = 0;
    long length = strtol(yytext, NULL, 10);
    if (!(INT_MIN <= length
        && length <= INT_MAX
        && errno != ERANGE))
    {
        driver.error(jinja2loc, "integer is out of range");
    }
    if(Kitsune::Jinja2::Jinja2ParserInterface::m_inRule) { return Kitsune::Jinja2::Jinja2Parser::make_NUMBER (length, jinja2loc); } else { return Kitsune::Jinja2::Jinja2Parser::make_DEFAULTRULE(yytext, jinja2loc); }
}

{id}       if(Kitsune::Jinja2::Jinja2ParserInterface::m_inRule) { return Kitsune::Jinja2::Jinja2Parser::make_IDENTIFIER(yytext, jinja2loc); } else { return Kitsune::Jinja2::Jinja2Parser::make_DEFAULTRULE(yytext, jinja2loc); }
.          if(Kitsune::Jinja2::Jinja2ParserInterface::m_inRule) { driver.error(jinja2loc, "invalid character"); } else { return Kitsune::Jinja2::Jinja2Parser::make_DEFAULTRULE(yytext, jinja2loc); }
<<EOF>>    return Kitsune::Jinja2::Jinja2Parser::make_END(jinja2loc);

%%


void Kitsune::Jinja2::Jinja2ParserInterface::scan_begin(const std::string &inputString)
{
    Kitsune::Jinja2::location newJsonloc;
    jinja2loc = newJsonloc;
    yy_flex_debug = m_traceParsing;
    yy_scan_string(inputString.c_str());
}

void Kitsune::Jinja2::Jinja2ParserInterface::scan_end()
{
    yy_delete_buffer(YY_CURRENT_BUFFER);
}


