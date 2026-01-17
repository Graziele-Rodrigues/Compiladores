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

/* ------------ TOKENS ------------ */

%token DATA CLASS INSTANCE FOR
%token IF ELSE ITERATE RETURN_KW NEW

%token TRUE_LIT FALSE_LIT NULL_LIT

%token INT_TYPE CHAR_TYPE BOOL_TYPE FLOAT_TYPE VOID_TYPE MAIN PRINT 
%token TYID ID
%token INT FLOAT CHAR

%token DOUBLE_COLON ARROW COLON
%token ANDAND AND
%token EQ NE LT GT LE GE
%token MAIS MENOS MULT DIVISAO RESTO
%token NEGACAO

%token ATTR
%token SEMICOLON COMMA DOT

%token L_CHAVE R_CHAVE
%token L_PARENTESE R_PARENTESE
%token L_COLCHETE R_COLCHETE

/* ------------ PRECEDÊNCIA ------------ */

%left ANDAND
%left EQ NE
%nonassoc LT
%left MAIS MENOS
%left MULT DIVISAO RESTO
%right NEGACAO UMINUS

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

/* ------------ PROGRAMA ------------ */

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

/* ------------ DATA ------------ */

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

/* ------------ FUNÇÕES ------------ */

func
    : ID id_list DOUBLE_COLON typeAnnot block
    ;

id_list
    : /* vazio */
    | id_list ID
    ;

/* ------------ CLASSES ------------ */

classDec
    : CLASS TYID ID L_CHAVE bind_list R_CHAVE
    ;

/* ------------ INSTÂNCIAS ------------ */

instDec
    : INSTANCE TYID FOR btype L_CHAVE func_list R_CHAVE
    ;

func_list
    : /* vazio */
    | func_list func
    ;

/* ------------ TIPOS ------------ */

typeAnnot
    : tyJoin
    | type ARROW typeAnnot
    ;

tyJoin
    : type
    | type AND tyJoin
    ;

type
    : type L_COLCHETE R_COLCHETE
    | btype
    ;

btype
    : INT_TYPE
    | CHAR_TYPE
    | BOOL_TYPE
    | FLOAT_TYPE
    | TYID
    | ID    /* confirmar professor - por conta teste4*/
    | VOID_TYPE
    ;

/* ------------ BLOCO ------------ */

block
    : L_CHAVE cmd_list R_CHAVE
    ;

cmd_list
    : /* vazio */
    | cmd_list cmd
    ;

stmtBlock
    : block
    | cmd
    ;

/* ------------ COMANDOS ------------ */

cmd
    : IF L_PARENTESE exp R_PARENTESE stmtBlock %prec LOWER_THAN_ELSE
    | IF L_PARENTESE exp R_PARENTESE stmtBlock ELSE stmtBlock
    | ITERATE L_PARENTESE loopCond R_PARENTESE stmtBlock
    | RETURN_KW exp_list_opt SEMICOLON
    | lvalue ATTR exp SEMICOLON
    | ID L_PARENTESE exps_opt R_PARENTESE call_suffix SEMICOLON
    ;

/* ================= CHAMADA ================= */

call_suffix
    : /* vazio */
    | LT lvalue_list GT
    ;

lvalue_list
    : lvalue
    | lvalue_list COMMA lvalue
    ;

/* ------------ EXPRESSOES ------------ */

exp
    : exp_and
    ;

exp_and
    : exp_and ANDAND exp_rel
    | exp_rel
    ;

exp_rel
    : exp_rel EQ exp_add
    | exp_rel NE exp_add
    | exp_rel LE exp_add
    | exp_rel GE exp_add
    | exp_rel LT exp_add
    | exp_rel GT exp_add
    | exp_add
    ;

exp_add
    : exp_add MAIS exp_mul
    | exp_add MENOS exp_mul
    | exp_mul
    ;

exp_mul
    : exp_mul MULT exp_un
    | exp_mul DIVISAO exp_un
    | exp_mul RESTO exp_un
    | exp_un
    ;

exp_un
    : NEGACAO exp_un
    | MENOS exp_un %prec UMINUS
    | exp_atom
    ;

exp_atom
    : TRUE_LIT
    | FALSE_LIT
    | NULL_LIT
    | INT
    | FLOAT
    | CHAR
    | NEW type new_suffix
    | ID L_PARENTESE exps_opt R_PARENTESE    /* confirmar professor se eh permitido */             
    | ID L_PARENTESE exps_opt R_PARENTESE L_COLCHETE exp R_COLCHETE
    | lvalue
    | L_PARENTESE exp R_PARENTESE
    ;

new_suffix
    : /* vazio */
    | L_COLCHETE exp R_COLCHETE
    ;

/* ------------ LVALUE ------------ */

lvalue
    : ID
    | lvalue DOT ID
    | lvalue L_COLCHETE exp R_COLCHETE
    ;


/* ------------ LISTAS ------------ */

exps
    : exp
    | exps COMMA exp
    ;

exps_opt
    : /* vazio */
    | exps
    ;

exp_list_opt
    : /* vazio */
    | exp_list
    ;

exp_list
    : exp
    | exp_list exp
    ;
    

loopCond
    : ID COLON exp
    | exp
    ;

%%

void yy::Parser::error(const std::string& msg)
{
    std::cerr << "Erro sintático: " << msg << std::endl;
}
