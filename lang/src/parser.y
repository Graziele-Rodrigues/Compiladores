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
    int yylex(yy::Parser::semantic_type*, Lexer& lexer) {
        return lexer.yylex();
    }
}

/* ---------- TOKENS ---------- */

%token DATA CLASS INSTANCE FOR
%token IF ELSE ITERATE RETURN_KW NEW
%token TRUE_LIT FALSE_LIT NULL_LIT

%token INT_TYPE CHAR_TYPE BOOL_TYPE FLOAT_TYPE VOID_TYPE
%token INT FLOAT CHAR TYID ID

%token DOUBLE_COLON ARROW COLON
%token EQ NE LT LE GE
%token ANDAND
%token AND

%token MAIS MENOS MULT DIVISAO  RESTO
%token L_CHAVE R_CHAVE L_PARENTESE R_PARENTESE L_COLCHETE R_COLCHETE

%token ATTR
%token SEMICOLON COMMA DOT

/* ---------- PRECEDÊNCIA ---------- */

%left ANDAND
%left EQ NE
%nonassoc LT LE GE
%left MAIS MENOS
%left MULT DIV RESTO
%right NEGACAO UMINUS

%%

prog
    : /* vazio */
    | prog decl
    ;

decl
    : data
    | func
    | classDec
    | instDec
    ;

data
    : DATA TYID '{' bind_list '}'
    ;

bind_list
    : /* vazio */
    | bind_list bind
    ;

bind
    : ID DOUBLE_COLON typeAnnot SEMICOLON
    ;

func
    : ID id_list DOUBLE_COLON typeAnnot block
    ;

id_list
    : /* vazio */
    | id_list ID
    ;

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

typeAnnot
    : tyJoin
    | type ARROW typeAnnot
    ;

tyJoin
    : type
    | tyJoin AND type
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

block
    : L_CHAVE cmd_list R_CHAVE
    ;

cmd_list
    : /* vazio */
    | cmd_list cmd
    ;

cmd
    : IF L_PARENTESE exp R_PARENTESE stmtBlock
    | IF L_PARENTESE exp R_PARENTESE stmtBlock ELSE stmtBlock
    | ITERATE L_PARENTESE loopCond R_PARENTESE stmtBlock
    | RETURN_KW exp_list SEMICOLON
    | lvalue ATTR exp SEMICOLON
    | ID L_PARENTESE exps_opt R_PARENTESE call_suffix SEMICOLON
    ;

stmtBlock
    : block
    | cmd
    ;

call_suffix
    : /* vazio */
    | GE lvalue_list LT
    ;

lvalue_list
    : lvalue
    | lvalue_list COMMA lvalue
    ;

loopCond
    : ID COLON exp
    | exp
    ;

exp
    : exp operator exp
    | NEGACAO exp
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

operator
    : ANDAND
    | EQ
    | NE
    | LT
    | LE
    | GE
    | MAIS
    | MENOS
    | MULT
    | DIVISAO
    | RESTO
    ;

lvalue
    : ID
    | lvalue DOT lvalue
    | lvalue L_COLCHETE exp R_COLCHETE
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
