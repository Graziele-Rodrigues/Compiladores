/* Graziele de Cassia Rodrigues - 21.1.8120 */

%language "c++"
%defines
%define api.parser.class {Parser}
%define api.value.type variant
%define parse.error verbose

%code requires {
    #include "lexer.hpp"
    #include "ast/include/ast.hpp"
    #include <memory>
    #include <string>
    #include <vector>
    #include <optional>
    #include <utility>
}

%parse-param { Lexer& lexer }
%parse-param { std::shared_ptr<Program>& out }
%lex-param   { Lexer& lexer }

%code {
  int yylex(yy::Parser::semantic_type* yylval, Lexer& lexer) {
    return lexer.yylex(static_cast<void*>(yylval));
  }
}

/* ------------ TOKENS ------------ */

%token DATA CLASS INSTANCE FOR
%token IF ELSE ITERATE RETURN_KW NEW
%token TRUE_LIT FALSE_LIT NULL_LIT

%token INT_TYPE CHAR_TYPE BOOL_TYPE FLOAT_TYPE VOID_TYPE MAIN PRINT

%token <std::string> TYID
%token <std::string> ID
%token <long long>   INT
%token <double>      FLOAT
%token <char>        CHAR

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
%left LE GE LT GT
%left MAIS MENOS
%left MULT DIVISAO RESTO
%right NEGACAO UMINUS

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

/* ------------ TIPOS DOS NÃO-TERMINAIS ------------ */

%type <std::shared_ptr<Program>> prog
%type <DeclPtr> decl data func classDec instDec

%type <std::vector<std::pair<std::string,std::string>>> bind_list
%type <std::pair<std::string,std::string>> bind
%type <std::vector<std::string>> id_list
%type <std::vector<DeclPtr>> func_list

%type <CmdPtr> block cmd stmtBlock
%type <std::vector<CmdPtr>> cmd_list

%type <ExprPtr> exp exp_and exp_rel exp_add exp_mul exp_un exp_atom
%type <std::vector<ExprPtr>> exps exps_opt
%type <std::vector<ExprPtr>> exp_list exp_list_opt

%type <LValPtr> lvalue
%type <std::vector<LValPtr>> lvalue_list
%type <std::vector<LValPtr>> call_suffix

%type <std::optional<ExprPtr>> new_suffix
%type <std::pair<std::optional<std::string>, ExprPtr>> loopCond

%type <std::string> typeAnnot tyJoin type btype

%%

/* ------------ PROGRAMA ------------ */

prog
    : /* vazio */
      {
        $$ = std::make_shared<Program>();
        out = $$;
      }
    | prog decl
      {
        $1->decls.push_back($2);
        $$ = $1;
        out = $$;
      }
    ;

decl
    : data      { $$ = $1; }
    | func      { $$ = $1; }
    | classDec  { $$ = $1; }
    | instDec   { $$ = $1; }
    ;

/* ------------ DATA ------------ */

data
    : DATA TYID L_CHAVE bind_list R_CHAVE
      {
        auto d = std::make_shared<DataDecl>();
        d->typeName = $2;
        d->fields = std::move($4);
        $$ = d;
      }
    ;

bind_list
    : /* vazio */
      { $$ = {}; }
    | bind_list bind
      {
        $1.push_back($2);
        $$ = std::move($1);
      }
    ;

bind
    : ID DOUBLE_COLON typeAnnot SEMICOLON
      {
        $$ = { $1, $3 };
      }
    ;

/* ------------ FUNÇÕES ------------ */

func
    : ID id_list DOUBLE_COLON typeAnnot block
      {
        auto f = std::make_shared<FuncDecl>();
        f->name = $1;
        f->params = std::move($2);
        f->typeAnnotText = $4;
        f->body = $5;
        $$ = f;
      }
    ;

id_list
    : /* vazio */
      { $$ = {}; }
    | id_list ID
      {
        $1.push_back($2);
        $$ = std::move($1);
      }
    ;

/* ------------ CLASSES ------------ */

classDec
    : CLASS TYID ID L_CHAVE bind_list R_CHAVE
      {
        // enquanto ClassDecl não está implementado
        $$ = std::make_shared<DummyDecl>();
      }
    ;

/* ------------ INSTÂNCIAS ------------ */

instDec
    : INSTANCE TYID FOR btype L_CHAVE func_list R_CHAVE
      {
        // enquanto InstanceDecl não está implementado
        $$ = std::make_shared<DummyDecl>();
      }
    ;

func_list
    : /* vazio */
      { $$ = {}; }
    | func_list func
      {
        $1.push_back($2);
        $$ = std::move($1);
      }
    ;

/* ------------ TIPOS (string) ------------ */

typeAnnot
    : tyJoin
      { $$ = $1; }
    | type ARROW typeAnnot
      { $$ = $1 + "->" + $3; }
    ;

tyJoin
    : type
      { $$ = $1; }
    | type AND tyJoin
      { $$ = $1 + "&" + $3; }
    ;

type
    : type L_COLCHETE R_COLCHETE
      { $$ = $1 + "[]"; }
    | btype
      { $$ = $1; }
    ;

btype
    : INT_TYPE
      { $$ = "Int"; }
    | CHAR_TYPE
      { $$ = "Char"; }
    | BOOL_TYPE
      { $$ = "Bool"; }
    | FLOAT_TYPE
      { $$ = "Float"; }
    | TYID
      { $$ = $1; }
    | ID
      { $$ = $1; }
    | VOID_TYPE
      { $$ = "Void"; }
    ;

/* ------------ BLOCO ------------ */

block
    : L_CHAVE cmd_list R_CHAVE
      {
        auto b = std::make_shared<CBlock>();
        b->cs = std::move($2);
        $$ = b;
      }
    ;

cmd_list
    : /* vazio */
      { $$ = {}; }
    | cmd_list cmd
      {
        $1.push_back($2);
        $$ = std::move($1);
      }
    ;

stmtBlock
    : block
      { $$ = $1; }
    | cmd
      { $$ = $1; }
    ;

/* ------------ COMANDOS ------------ */

cmd
    : IF L_PARENTESE exp R_PARENTESE stmtBlock %prec LOWER_THAN_ELSE
      {
        auto c = std::make_shared<CIf>();
        c->cond = $3;
        c->thenC = $5;
        c->elseC = std::nullopt;
        $$ = c;
      }
    | IF L_PARENTESE exp R_PARENTESE stmtBlock ELSE stmtBlock
      {
        auto c = std::make_shared<CIf>();
        c->cond = $3;
        c->thenC = $5;
        c->elseC = $7;
        $$ = c;
      }
    | ITERATE L_PARENTESE loopCond R_PARENTESE stmtBlock
      {
        auto c = std::make_shared<CIterate>();
        c->itVar = $3.first;
        c->expr  = $3.second;
        c->body  = $5;
        $$ = c;
      }
    | RETURN_KW exp_list_opt SEMICOLON
      {
        auto c = std::make_shared<CReturn>();
        c->exps = std::move($2);
        $$ = c;
      }
    | lvalue ATTR exp SEMICOLON
      {
        auto c = std::make_shared<CAssign>();
        c->lhs = $1;
        c->rhs = $3;
        $$ = c;
      }
    | ID L_PARENTESE exps_opt R_PARENTESE call_suffix SEMICOLON
      {
        auto c = std::make_shared<CCallStmt>();
        c->name = $1;
        c->args = std::move($3);
        c->rets = std::move($5);
        $$ = c;
      }
    ;

/* ================= CHAMADA ================= */

call_suffix
    : /* vazio */
      { $$ = {}; }
    | LT lvalue_list GT
      { $$ = std::move($2); }
    ;

lvalue_list
    : lvalue
      { $$ = { $1 }; }
    | lvalue_list lvalue
      {
        $1.push_back($2);
        $$ = std::move($1);
      }
    ;

/* ------------ EXPRESSOES ------------ */

exp
    : exp_and
      { $$ = $1; }
    ;

exp_and
    : exp_and ANDAND exp_rel
      { $$ = std::make_shared<EBinary>(EBinary::AndAnd, $1, $3); }
    | exp_rel
      { $$ = $1; }
    ;

exp_rel
  : exp_add
    { $$ = $1; }
  | exp_add EQ exp_add
    { $$ = std::make_shared<EBinary>(EBinary::Eq, $1, $3); }
  | exp_add NE exp_add
    { $$ = std::make_shared<EBinary>(EBinary::Ne, $1, $3); }
  | exp_add LE exp_add
    { $$ = std::make_shared<EBinary>(EBinary::Le, $1, $3); }
  | exp_add GE exp_add
    { $$ = std::make_shared<EBinary>(EBinary::Ge, $1, $3); }
  | exp_add LT exp_add
    { $$ = std::make_shared<EBinary>(EBinary::Lt, $1, $3); }
  | exp_add GT exp_add
    { $$ = std::make_shared<EBinary>(EBinary::Gt, $1, $3); }
  ;


exp_add
    : exp_add MAIS exp_mul
      { $$ = std::make_shared<EBinary>(EBinary::Add, $1, $3); }
    | exp_add MENOS exp_mul
      { $$ = std::make_shared<EBinary>(EBinary::Sub, $1, $3); }
    | exp_mul
      { $$ = $1; }
    ;

exp_mul
    : exp_mul MULT exp_un
      { $$ = std::make_shared<EBinary>(EBinary::Mul, $1, $3); }
    | exp_mul DIVISAO exp_un
      { $$ = std::make_shared<EBinary>(EBinary::Div, $1, $3); }
    | exp_mul RESTO exp_un
      { $$ = std::make_shared<EBinary>(EBinary::Mod, $1, $3); }
    | exp_un
      { $$ = $1; }
    ;

exp_un
    : NEGACAO exp_un
      { $$ = std::make_shared<EUnary>(EUnary::Not, $2); }
    | MENOS exp_un %prec UMINUS
      { $$ = std::make_shared<EUnary>(EUnary::Neg, $2); }
    | exp_atom
      { $$ = $1; }
    ;

exp_atom
    : TRUE_LIT
      { $$ = std::make_shared<EBool>(true); }
    | FALSE_LIT
      { $$ = std::make_shared<EBool>(false); }
    | NULL_LIT
      { $$ = std::make_shared<ENull>(); }
    | INT
      { $$ = std::make_shared<EInt>($1); }
    | FLOAT
      { $$ = std::make_shared<EFloat>($1); }
    | CHAR
      { $$ = std::make_shared<EChar>($1); }

    | NEW type new_suffix
      { $$ = std::make_shared<ENew>($2, $3); }

    | ID L_PARENTESE exps_opt R_PARENTESE
      {
        auto c = std::make_shared<ECall>();
        c->name = $1;
        c->args = std::move($3);
        c->retIndex = std::nullopt;
        $$ = c;
      }

    | ID L_PARENTESE exps_opt R_PARENTESE L_COLCHETE exp R_COLCHETE
      {
        auto c = std::make_shared<ECall>();
        c->name = $1;
        c->args = std::move($3);
        c->retIndex = $6;
        $$ = c;
      }

    | lvalue
      { $$ = std::make_shared<ELValue>($1); }

    | L_PARENTESE exp R_PARENTESE
      { $$ = $2; }
    ;

new_suffix
    : /* vazio */
      { $$ = std::nullopt; }
    | L_COLCHETE exp R_COLCHETE
      { $$ = $2; }
    ;

/* ------------ LVALUE ------------ */

lvalue
    : ID
      { $$ = std::make_shared<LVar>($1); }
    | lvalue DOT ID
      { $$ = std::make_shared<LField>($1, $3); }
    | lvalue L_COLCHETE exp R_COLCHETE
      { $$ = std::make_shared<LIndex>($1, $3); }
    ;

/* ------------ LISTAS ------------ */

exps
    : exp
      { $$ = { $1 }; }
    | exps COMMA exp
      {
        $1.push_back($3);
        $$ = std::move($1);
      }
    ;

exps_opt
    : /* vazio */
      { $$ = {}; }
    | exps
      { $$ = std::move($1); }
    ;

exp_list_opt
    : /* vazio */
      { $$ = {}; }
    | exp_list
      { $$ = std::move($1); }
    ;

exp_list
    : exp
      { $$ = { $1 }; }
    | exp_list exp
      {
        $1.push_back($2);
        $$ = std::move($1);
      }
    ;

loopCond
    : ID COLON exp
      { $$ = { $1, $3 }; }
    | exp
      { $$ = { std::nullopt, $1 }; }
    ;

%%

void yy::Parser::error(const std::string& msg)
{
    std::cerr << "Erro sintático: " << msg << std::endl;
}
