/* Graziele de Cassia Rodrigues 21.1.8120 */
%language "c++"
%defines
%define api.parser.class {Parser}
%define api.value.type variant
%define parse.error verbose

%code requires {
    #include "lexer.hpp"
}

%parse-param { Lexer& lexer }
%lex-param   { Lexer& lexer }

%code {
    #include <iostream>

    int yylex(yy::Parser::semantic_type*, Lexer& lexer) {
        return lexer.yylex();
    }
}

/* ----------- TOKENS ----------- */

%token DATA FUNC CLASS INSTANCE FOR IF ELSE ITERATE RETURN_KW NEW
%token TRUE_LIT FALSE_LIT NULL_LIT MAIN PRINT TYCHR

%token INT_TYPE CHAR_TYPE BOOL_TYPE FLOAT_TYPE VOID_TYPE

%token INT FLOAT CHAR TYID ID

%token DOUBLE_COLON ARROW
%token EQ GT GE LT LE NE

%token ATTR
%token SEMICOLON

%token L_CHAVE R_CHAVE
%token L_PARENTESE R_PARENTESE
%token COMMA

%%

/* ----------- GRAMATICA ----------- */

program
    : main_decl
    ;

main_decl
    : MAIN DOUBLE_COLON type block
    ;

type
    : VOID_TYPE
    | INT_TYPE
    | FLOAT_TYPE
    | CHAR_TYPE
    | BOOL_TYPE
    ;

block
    : L_CHAVE stmt_list R_CHAVE
    ;

stmt_list
    : /* vazio */
    | stmt_list stmt
    ;

stmt
    : assignment
    | return_stmt
    ;

assignment
    : ID ATTR expr SEMICOLON
    ;

return_stmt
    : RETURN_KW SEMICOLON
    ;

expr
    : INT
    | ID
    ;

%%

void yy::Parser::error(const std::string& msg)
{
    std::cerr << "Erro sintático: " << msg << std::endl;
}
