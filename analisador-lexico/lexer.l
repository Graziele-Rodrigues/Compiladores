%{
// Graziele de Cassia Rodrigues 21.1.8120
#include <iostream>
#include "tokens.h"

int yylineno = 1;
int yycolumn = 1;
int token_start_column;

#define PRINT_POS() \
    std::cout << "(" << yylineno << " ," << token_start_column << ") " << yytext << std::endl;
%}

%option noyywrap
%option yylineno

delim       [ \t\n]
brancos     {delim}+
letra       [A-Za-z]
digito      [0-9]
id          {letra}({letra}|{digito})*
num         {digito}+(\.{digito}+)?(E[+-]?{digito}+)?
char_lit    \'(\\.|[^\\'])\'

%%

{brancos} {
    for (int i = 0; yytext[i] != '\0'; i++) {
        if (yytext[i] == '\n') {
            yycolumn = 1;  
        } else {
            yycolumn++;
        }
    }
}


"data"       { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return DATA; }
"func"       { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return FUNC; }
"class"      { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return CLASS; }
"instance"   { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return INSTANCE; }
"for"        { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return FOR; }
"if"         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return IF; }
"else"       { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return ELSE; }
"iterate"    { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return ITERATE; }
"return"     { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return RETURN_KW; }
"new"        { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return NEW; }
"true"       { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return TRUE_LIT; }
"false"      { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return FALSE_LIT; }
"null"       { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return NULL_LIT; }

"Int"        { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return INT_TYPE; }
"Char"       { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return CHAR_TYPE; }
"Bool"       { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return BOOL_TYPE; }
"Float"      { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return FLOAT_TYPE; }
"Void"       { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return VOID_TYPE; }

{num}        { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return FLOAT; }
{char_lit}   { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return CHAR; }
[A-Z]({letra}|{digito})*  { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return TYID; }
[a-z]({letra}|{digito})*  { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return ID; }

"::"        { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return DOUBLE_COLON; }
"->"        { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return ARROW; }
"<="        { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return RELOP; }
"<>"        { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return RELOP; }
">="        { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return RELOP; }
"&&"        { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return AND_OP; }
"=="        { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return EQ_OP; }
"!="        { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return NE_OP; }

"="         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return ATTR; }
"&"         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return AMPERSAND; }
";"         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return SEMICOLON; }
"{"         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return L_CHAVE; }
"}"         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return R_CHAVE; }
"("         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return L_PARENTESE; }
")"         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return R_PARENTESE; }
"["         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return L_COLCHETE; }
"]"         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return R_COLCHETE; }
","         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return COMMA; }
"."         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return DOT; }
":"         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return COLON; }
"!"         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return NOT_OP; }
"+"         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return PLUS; }
"-"         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return MINUS; }
"*"         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return MULT; }
"/"         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return DIV; }
"%"         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return MOD; }
"<"         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return RELOP; }
">"         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return RELOP; }

. {
    std::cerr << "Linha " << yylineno << ", Coluna " << yycolumn
              << ": caractere inválido '" << yytext << "'" << std::endl;
    yycolumn += yyleng;
}
%%
