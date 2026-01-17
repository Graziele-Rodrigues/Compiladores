%option noyywrap
%option c++
%option yyclass="Lexer"

%{
/* Graziele de Cassia Rodrigues - 21.1.8120 */

#include <iostream>
#include <string>
#include <cstdlib>

#include "parser.hpp"
#include "lexer.hpp"

int yylineno = 1;
int yycolumn = 1;
int token_start_column;

#define YY_DECL int Lexer::yylex(void* yylval_void)

#define PRINT_POS() \
    if (debug_tokens) \
        std::cout << "(" << yylineno << " ," << token_start_column << ") " << yytext << std::endl;

static char parse_char_lit(const char* yytext) {
    if (yytext[1] != '\\') return yytext[1];
    char e = yytext[2];
    switch (e) {
        case 'n': return '\n';
        case 'r': return '\r';
        case 't': return '\t';
        case 'b': return '\b';
        case '\\': return '\\';
        case '\'': return '\'';
        default:
            if (e >= '0' && e <= '9') {
                int d1 = yytext[2] - '0';
                int d2 = yytext[3] - '0';
                int d3 = yytext[4] - '0';
                int val = d1*64 + d2*8 + d3;
                return static_cast<char>(val);
            }
            return e;
    }
}
%}


%x COMMENT

delim       [ \t\r]+
letra       [A-Za-z]
digito      [0-9]

tyid        [A-Z][A-Za-z0-9_]*
id          [a-z][A-Za-z0-9_]*

int_lit     {digito}+
float_lit   ({digito}*"."{digito}+)
char_lit    \'([^\\'\n\r]|\\[nrtb'\\]|\\[0-9]{3})\'

%%

{delim}      { yycolumn += yyleng; }

\n {
    yylineno++;
    yycolumn = 1;
}


"--".* { yycolumn += yyleng; }

"\{-" {
    token_start_column = yycolumn;
    yycolumn += yyleng;
    BEGIN(COMMENT);
}

<COMMENT>"-\}" {
    yycolumn += yyleng;
    BEGIN(INITIAL);
}
<COMMENT>\n {
    yylineno++;
    yycolumn = 1;
}
<COMMENT>. {
    yycolumn++;
}

"data"      { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::DATA; }
"class"     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::CLASS; }
"instance"  { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::INSTANCE; }
"for"       { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::FOR; }

"if"        { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::IF; }
"else"      { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::ELSE; }
"iterate"   { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::ITERATE; }
"return"    { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::RETURN_KW; }
"new"       { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::NEW; }

"true"      { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::TRUE_LIT; }
"false"     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::FALSE_LIT; }
"null"      { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::NULL_LIT; }


"Int"       { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::INT_TYPE; }
"Char"      { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::CHAR_TYPE; }
"Bool"      { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::BOOL_TYPE; }
"Float"     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::FLOAT_TYPE; }
"Void"      { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::VOID_TYPE; }


{float_lit} {
    token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS();
    auto* yylval = static_cast<yy::Parser::semantic_type*>(yylval_void);
    yylval->emplace<double>(std::stod(yytext));
    return yy::Parser::token::FLOAT;
}

{int_lit} {
    token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS();
    auto* yylval = static_cast<yy::Parser::semantic_type*>(yylval_void);
    yylval->emplace<long long>(std::stoll(yytext));
    return yy::Parser::token::INT;
}

{char_lit} {
    token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS();
    auto* yylval = static_cast<yy::Parser::semantic_type*>(yylval_void);
    yylval->emplace<char>(parse_char_lit(yytext));
    return yy::Parser::token::CHAR;
}

{tyid} {
    token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS();
    auto* yylval = static_cast<yy::Parser::semantic_type*>(yylval_void);
    yylval->emplace<std::string>(yytext);
    return yy::Parser::token::TYID;
}

{id} {
    token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS();
    auto* yylval = static_cast<yy::Parser::semantic_type*>(yylval_void);
    yylval->emplace<std::string>(yytext);
    return yy::Parser::token::ID;
}


"::"    { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::DOUBLE_COLON; }
":"     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::COLON; }
"->"    { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::ARROW; }

"=="    { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::EQ; }
"!="    { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::NE; }
"<="    { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::LE; }
">="    { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::GE; }
"<"     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::LT; }
">"     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::GT; }

"&&"    { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::ANDAND; }
"!"     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::NEGACAO; }
"&"     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::AND; }

"="     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::ATTR; }

";"     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::SEMICOLON; }
","     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::COMMA; }
"."     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::DOT; }

"("     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::L_PARENTESE; }
")"     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::R_PARENTESE; }
"{"     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::L_CHAVE; }
"}"     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::R_CHAVE; }
"["     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::L_COLCHETE; }
"]"     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::R_COLCHETE; }

"+"     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::MAIS; }
"-"     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::MENOS; }
"*"     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::MULT; }
"/"     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::DIVISAO; }
"%"     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return yy::Parser::token::RESTO; }


. {
    std::cerr << "Erro léxico em (" << yylineno << "," << yycolumn
              << "): caractere inválido '" << yytext << "'\n";
    yycolumn++;
}

%%
