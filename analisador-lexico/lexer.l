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
tyid        [A-Z]({letra}|{digito})*
id          [a-z]({letra}|{digito})*
int_lit     {digito}+
float_lit   {digito}*"."{digito}+([eE][+-]?{digito}+)?
char_lit    \'([^\\'\n\r]|\\[nrtb'\\]|\\[0-9]{3})\'


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


"\{-"([^-\}]|"-"[^}]|"\}"[^{])*"\{-" {
    std::cerr << "Erro léxico: comentário aninhado não suportado na linha "
              << yylineno << std::endl;
}

"\{-"([^-\}]|"-"[^}]|"\}"[^{])*"-\}" {
    yycolumn += yyleng;
}

"\{-"([^-\}]|"-"[^}]|"\}"[^{])*$ {
    std::cerr << "Erro léxico: comentário não fechado na linha "
              << yylineno << std::endl;
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
"main"       { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return MAIN; }
"print"      { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return PRINT; }
"tychr"      { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return TYCHR; }


{float_lit} { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return FLOAT; }
{int_lit}   { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return INT; }
{char_lit}   { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return CHAR; }
{tyid}  { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return TYID; }
{id}  { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return ID; }

"::"        { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return DOUBLE_COLON; }
"->"        { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return ARROW; }
"<"         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return LT; }
"<="        { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return LE; }
">"         { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return GT; }
">="        { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return GE; }
"=="        { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return EQ; }
"!="        { token_start_column = yycolumn; yycolumn += yyleng; PRINT_POS(); return NE; }


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

. {
    std::cerr << "Linha " << yylineno << ", Coluna " << yycolumn
              << ": caractere inválido '" << yytext << "'" << std::endl;
    yycolumn += yyleng;
}
%%