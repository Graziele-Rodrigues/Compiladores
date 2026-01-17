#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

struct SrcPos { int line=0, col=0; };

// ---------- ForwaQrd ----------
struct Expr; struct Cmd; struct LValue;
using ExprPtr = std::shared_ptr<Expr>;
using CmdPtr  = std::shared_ptr<Cmd>;
using LValPtr = std::shared_ptr<LValue>;

// ---------- Program / Decls ----------
struct Decl { virtual ~Decl() = default; };
using DeclPtr = std::shared_ptr<Decl>;

struct Program {
  std::vector<DeclPtr> decls;
};

struct DataDecl : Decl {
  std::string typeName; // TYID
  // campo -> tipo (você pode guardar string do tipo por enquanto)
  std::vector<std::pair<std::string,std::string>> fields;
};

struct FuncDecl : Decl {
  std::string name;
  std::vector<std::string> params; // {ID}
  // se quiser: guardar a anotação de tipo como string ou árvore de tipos
  std::string typeAnnotText;
  CmdPtr body; // bloco
};

// ---------- LValue ----------
struct LValue { SrcPos pos; virtual ~LValue() = default; };

struct LVar : LValue {
  std::string name;
  explicit LVar(std::string n) : name(std::move(n)) {}
};

struct LField : LValue {
  LValPtr base;
  std::string field;
  LField(LValPtr b, std::string f) : base(std::move(b)), field(std::move(f)) {}
};

struct LIndex : LValue {
  LValPtr base;
  ExprPtr index;
  LIndex(LValPtr b, ExprPtr i) : base(std::move(b)), index(std::move(i)) {}
};

// ---------- Expr ----------
struct Expr { SrcPos pos; virtual ~Expr() = default; };

struct EInt   : Expr { long long v; explicit EInt(long long x):v(x){} };
struct EFloat : Expr { double v; explicit EFloat(double x):v(x){} };
struct EChar  : Expr { char v; explicit EChar(char x):v(x){} };
struct EBool  : Expr { bool v; explicit EBool(bool x):v(x){} };
struct ENull  : Expr { };

struct EVar   : Expr { std::string name; explicit EVar(std::string n):name(std::move(n)){} };

struct EUnary : Expr {
  enum Op { Neg, Not } op;
  ExprPtr e;
  EUnary(Op o, ExprPtr x):op(o),e(std::move(x)){}
};

struct EBinary : Expr {
  enum Op { Add, Sub, Mul, Div, Mod, AndAnd, Eq, Ne, Lt, Le, Gt, Ge } op;
  ExprPtr l, r;
  EBinary(Op o, ExprPtr a, ExprPtr b):op(o),l(std::move(a)),r(std::move(b)){}
};

// new type [exp]?
struct ENew : Expr {
  std::string typeName;          // "Int"/"Char"/TYID etc (btype/type)
  std::optional<ExprPtr> size;   // se existir [exp]
  ENew(std::string t, std::optional<ExprPtr> s) : typeName(std::move(t)), size(std::move(s)) {}
};

// chamada como expressão: ID(args)[idx] OU (no seu parser também ID(args) puro)
struct ECall : Expr {
  std::string name;
  std::vector<ExprPtr> args;
  // se você quiser suportar “pegar retorno k”, coloca indexOpt
  std::optional<ExprPtr> retIndex;
};

// lvalue como expressão
struct ELValue : Expr { LValPtr lv; explicit ELValue(LValPtr x):lv(std::move(x)){} };

// ---------- Cmd ----------
struct Cmd { SrcPos pos; virtual ~Cmd() = default; };

struct CBlock : Cmd { std::vector<CmdPtr> cs; };

struct CAssign : Cmd { LValPtr lhs; ExprPtr rhs; };

struct CIf : Cmd { ExprPtr cond; CmdPtr thenC; std::optional<CmdPtr> elseC; };

struct CIterate : Cmd {
  // iterate(expr) stmtBlock  ou  iterate(i:expr) stmtBlock
  std::optional<std::string> itVar; // i
  ExprPtr expr;
  CmdPtr body;
};

struct CReturn : Cmd { std::vector<ExprPtr> exps; };

struct CCallStmt : Cmd {
  std::string name;
  std::vector<ExprPtr> args;
  std::vector<LValPtr> rets; // < lvalues... > (pode ser vazio)
};
