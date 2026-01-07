%option noyywrap
%option c++
%option yyclass="Lexer"

%{
/* Graziele de Cassia Rodrigues 21.1.8120 */

#include <iostream>
#include "parser.hpp"

int yycolumn = 1;
int yylineno = 1;
int token_start_column;

#define YY_DECL int Lexer::yylex()

#define PRINT_POS() \
    std::cout << "(" << yylineno << " ," << token_start_column << ") " << yytext << std::endl;
%}

delim       [ \t\n]
brancos     {delim}+

letra       [A-Za-z]
digito      [0-9]

tyid        [A-Z]({letra}|{digito})*
id          [a-z]({letra}|{digito})*

int_lit     {digito}+
float_lit   {digito}*"."{digito}+([eE][+-]?{digito}+)?
char_lit    \'([^\\'\n\r]|\\[nrtb'\\]|\\[0-9]{3})\'

%%

{brancos} {
    for (int i = 0; yytext[i] != '\0'; i++) {
        if (yytext[i] == '\n') {
            yylineno++;
            yycolumn = 1;
        } else {
            yycolumn++;
        }
    }
}

"data"       { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::DATA; }
"func"       { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::FUNC; }
"class"      { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::CLASS; }
"instance"   { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::INSTANCE; }
"for"        { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::FOR; }
"if"         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::IF; }
"else"       { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::ELSE; }
"iterate"    { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::ITERATE; }
"return"     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::RETURN_KW; }
"new"        { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::NEW; }

"true"       { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::TRUE_LIT; }
"false"      { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::FALSE_LIT; }
"null"       { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::NULL_LIT; }

"main"       { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::MAIN; }
"print"      { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::PRINT; }
"tychr"      { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::TYCHR; }

"Int"        { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::INT_TYPE; }
"Char"       { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::CHAR_TYPE; }
"Bool"       { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::BOOL_TYPE; }
"Float"      { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::FLOAT_TYPE; }
"Void"       { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::VOID_TYPE; }

{float_lit}  { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::FLOAT; }
{int_lit}    { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::INT; }
{char_lit}   { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::CHAR; }

{tyid}       { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::TYID; }
{id}         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::ID; }

"::"         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::DOUBLE_COLON; }
"->"         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::ARROW; }

"<="         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::LE; }
">="         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::GE; }
"=="         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::EQ; }
"!="         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::NE; }
"<"          { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::LT; }
">"          { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::GT; }

"="          { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::ATTR; }

";"          { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::SEMICOLON; }
"{"          { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::L_CHAVE; }
"}"          { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::R_CHAVE; }
"("          { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::L_PARENTESE; }
")"          { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::R_PARENTESE; }
","          { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::COMMA; }

. {
    std::cerr << "Linha " << yylineno
              << ", Coluna " << yycolumn
              << ": caractere inválido '" << yytext << "'\n";
    yycolumn += yyleng;
}

%%
