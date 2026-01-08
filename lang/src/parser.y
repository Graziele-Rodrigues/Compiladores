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
%token EQ NE GT GE LT LE
%token ANDAND

%token ATTR
%token SEMICOLON
%token COMMA
%token COLON

%token L_CHAVE R_CHAVE
%token L_PARENTESE R_PARENTESE
%token L_COLCHETE R_COLCHETE
%token DOT


/* ----------- PRECEDENCIA ----------- */
%left ANDAND
%left EQ NE LT LE GT GE
%left '+' '-'
%left '*' '/' '%'
%right '!' UMINUS

/* ----------- GRAMATICA ----------- */
%%
/* ----------- PROGRAMA ----------- */

prog
    : /* vazio */
    | prog decl
    | main_decl
    ;

/* ----------- DECLARACOES ----------- */
main_decl
    : MAIN DOUBLE_COLON type block
    ;

decl
    : data
    | func
    | classDec
    | instDec
    ;

/* ----------- DATA / BIND ----------- */
data
    : DATA TYID L_CHAVE bind_list R_CHAVE
    ;

bind_list
    : /* vazio */
    | bind_list bind
    ;

bind
    : ID DOUBLE_COLON typeAnnot SEMICOLON
    ;

/* ----------- FUNCOES ----------- */
func
    : ID id_list DOUBLE_COLON typeAnnot block
    ;

id_list
    : /* vazio */
    | id_list ID
    ;

/* ----------- CLASSES/INSTANCIAS ----------- */
classDec
    : CLASS TYID ID L_CHAVE bind_list R_CHAVE
    ;

instDec
    : INSTANCE TYID FOR ID L_CHAVE func_list R_CHAVE
    ;

func_list
    : /* vazio */
    | func_list func
    ;


/* ----------- TIPOS ----------- */
typeAnnot
    : tyJoin
    | type ARROW typeAnnot
    ;

tyJoin
    : type
    | tyJoin '&' type
    ;

type
    : btype
    | type L_COLCHETE R_COLCHETE
    ;

btype
    : INT_TYPE
    | CHAR_TYPE
    | BOOL_TYPE
    | FLOAT_TYPE
    | VOID_TYPE
    | TYID
    ;

/* ----------- BLOCOS ----------- */
block
    : L_CHAVE cmd_list R_CHAVE
    ;

stmtBlock
    : block
    | cmd
    ;

cmd_list
    : /* vazio */
    | cmd_list cmd
    ;

/* ----------- COMANDO ----------- */

cmd
    : IF L_PARENTESE exp R_PARENTESE stmtBlock
    | IF L_PARENTESE exp R_PARENTESE stmtBlock ELSE stmtBlock
    | ITERATE L_PARENTESE loopCond R_PARENTESE stmtBlock
    | RETURN_KW exp_list SEMICOLON
    | lvalue ATTR exp SEMICOLON
    | ID L_PARENTESE exps_opt R_PARENTESE call_suffix SEMICOLON
    ;

/* ----------- CHAMADA FUNCAO ----------- */

call_suffix
    : /* vazio */
    | '<' lvalue lvalue_list '>'
    ;

lvalue_list
    : /* vazio */
    | lvalue_list COMMA lvalue
    ;

/* ----------- LOOP ----------- */

loopCond
    : ID COLON exp
    | exp
    ;

/* ----------- EXPRESSOES ----------- */
exp
    : exp operator exp
    | '!' exp
    | '-' exp %prec UMINUS
    | TRUE_LIT
    | FALSE_LIT
    | NULL_LIT
    | INT
    | FLOAT
    | CHAR
    | NEW type new_suffix
    | ID L_PARENTESE exps_opt R_PARENTESE L_COLCHETE exp R_COLCHETE
    | lvalue
    | L_PARENTESE exp R_PARENTESE
    ;

new_suffix
    : /* vazio */
    | L_COLCHETE exp R_COLCHETE
    ;

/* ----------- OPERADORES ----------- */
operator
    : ANDAND
    | EQ
    | NE
    | LT
    | LE
    | GT
    | GE
    | '+'
    | '-'
    | '*'
    | '/'
    | '%'
    ;

/* ----------- OUTROS ----------- */

lvalue
    : ID
    | lvalue DOT lvalue
    | L_COLCHETE exp R_COLCHETE
    ;

exps
    : exp
    | exps COMMA exp
    ;

exps_opt
    : /* vazio */
    | exps
    ;

exp_list
    : /* vazio */
    | exp_list exp
    ;

%%

void yy::Parser::error(const std::string& msg)
{
    std::cerr << "Erro sintático: " << msg << std::endl;
}
