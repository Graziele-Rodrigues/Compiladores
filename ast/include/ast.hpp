/*
 * Arquivo: ast.hpp
 * Autor: Graziele Cassia Rodrigues
 * Matricula: 21.1.8120
 *
 * Descrição:
 * Nós do AST (Árvore de Sintaxe Abstrata) com Visitor (node + visitor).
 */

#pragma once
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <utility>

// pegar posição no código-fonte
struct SrcPos {
  int line = 0, col = 0;
};

// Forward decls
struct Decl;
struct DataDecl;
struct FuncDecl;
struct ClassDecl;
struct InstanceDecl;

struct LValue;
struct LVar;
struct LField;
struct LIndex;

struct Expr;
struct EInt;
struct EFloat;
struct EChar;
struct EBool;
struct ENull;
struct EVar;
struct EUnary;
struct EBinary;
struct ENew;
struct ECall;
struct ELValue;

struct Cmd;
struct CBlock;
struct CAssign;
struct CIf;
struct CIterate;
struct CReturn;
struct CCallStmt;

// Visitor base
struct AstVisitor {
  virtual ~AstVisitor() = default;

  // Decls
  virtual void visit(DataDecl&) = 0;
  virtual void visit(FuncDecl&) = 0;

  // class/instance
  virtual void visit(ClassDecl&) = 0;
  virtual void visit(InstanceDecl&) = 0;

  // LValues
  virtual void visit(LVar&) = 0;
  virtual void visit(LField&) = 0;
  virtual void visit(LIndex&) = 0;

  // Exprs
  virtual void visit(EInt&) = 0;
  virtual void visit(EFloat&) = 0;
  virtual void visit(EChar&) = 0;
  virtual void visit(EBool&) = 0;
  virtual void visit(ENull&) = 0;
  virtual void visit(EVar&) = 0;
  virtual void visit(EUnary&) = 0;
  virtual void visit(EBinary&) = 0;
  virtual void visit(ENew&) = 0;
  virtual void visit(ECall&) = 0;
  virtual void visit(ELValue&) = 0;

  // Cmds
  virtual void visit(CBlock&) = 0;
  virtual void visit(CAssign&) = 0;
  virtual void visit(CIf&) = 0;
  virtual void visit(CIterate&) = 0;
  virtual void visit(CReturn&) = 0;
  virtual void visit(CCallStmt&) = 0;
};

// Ponteiros inteligentes
using DeclPtr = std::shared_ptr<Decl>;
using ExprPtr = std::shared_ptr<Expr>;
using CmdPtr  = std::shared_ptr<Cmd>;
using LValPtr = std::shared_ptr<LValue>;

// Programa
struct Program {
  std::vector<DeclPtr> decls;
};

// ===========================
// Decl (BASE) — AGORA COM POS
// ===========================
struct Decl {
  SrcPos pos;

  Decl() = default;
  explicit Decl(SrcPos p) : pos(p) {}

  virtual ~Decl() = default;
  virtual void accept(AstVisitor& v) = 0;
};

// data TYID { field :: TYPE; ... }
struct DataDecl : Decl {
  std::string typeName;
  std::vector<std::pair<std::string, std::string>> fields;

  DataDecl() = default;
  explicit DataDecl(SrcPos p) : Decl(p) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

// func ID params :: typeAnnot block
struct FuncDecl : Decl {
  std::string name;
  std::vector<std::string> params;
  std::string typeAnnotText;
  CmdPtr body;

  FuncDecl() = default;
  explicit FuncDecl(SrcPos p) : Decl(p) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

// class TYID ID { methods... }
struct ClassDecl : Decl {
  std::string className;   // ex: "Chr"
  std::string tyVar;       // ex: "a"
  std::vector<std::pair<std::string, std::string>> methods; // (nomeMetodo, annot)

  ClassDecl() = default;
  explicit ClassDecl(SrcPos p) : Decl(p) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

// instance TYID for T { func_list }
struct InstanceDecl : Decl {
  std::string className;
  std::string forType;               // tipo concreto como string (ex: Int, Pessoa, Char[])
  std::vector<DeclPtr> methods;      // normalmente FuncDecls

  InstanceDecl() = default;
  explicit InstanceDecl(SrcPos p) : Decl(p) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};


struct LValue {
  SrcPos pos;

  LValue() = default;
  explicit LValue(SrcPos p) : pos(p) {}

  virtual ~LValue() = default;
  virtual void accept(AstVisitor& v) = 0;
};

struct LVar : LValue {
  std::string name;

  explicit LVar(std::string n) : LValue(), name(std::move(n)) {}
  LVar(SrcPos p, std::string n) : LValue(p), name(std::move(n)) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct LField : LValue {
  LValPtr base;
  std::string field;

  LField(LValPtr b, std::string f) : LValue(), base(std::move(b)), field(std::move(f)) {}
  LField(SrcPos p, LValPtr b, std::string f) : LValue(p), base(std::move(b)), field(std::move(f)) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct LIndex : LValue {
  LValPtr base;
  ExprPtr index;

  LIndex(LValPtr b, ExprPtr i) : LValue(), base(std::move(b)), index(std::move(i)) {}
  LIndex(SrcPos p, LValPtr b, ExprPtr i) : LValue(p), base(std::move(b)), index(std::move(i)) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};


struct Expr {
  SrcPos pos;

  Expr() = default;
  explicit Expr(SrcPos p) : pos(p) {}

  virtual ~Expr() = default;
  virtual void accept(AstVisitor& v) = 0;
};

// Literais
struct EInt : Expr {
  long long v;

  explicit EInt(long long x) : Expr(), v(x) {}
  EInt(SrcPos p, long long x) : Expr(p), v(x) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct EFloat : Expr {
  double v;

  explicit EFloat(double x) : Expr(), v(x) {}
  EFloat(SrcPos p, double x) : Expr(p), v(x) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct EChar : Expr {
  char v;

  explicit EChar(char x) : Expr(), v(x) {}
  EChar(SrcPos p, char x) : Expr(p), v(x) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct EBool : Expr {
  bool v;

  explicit EBool(bool x) : Expr(), v(x) {}
  EBool(SrcPos p, bool x) : Expr(p), v(x) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct ENull : Expr {
  ENull() = default;
  explicit ENull(SrcPos p) : Expr(p) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct EVar : Expr {
  std::string name;

  explicit EVar(std::string n) : Expr(), name(std::move(n)) {}
  EVar(SrcPos p, std::string n) : Expr(p), name(std::move(n)) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

// Unário
struct EUnary : Expr {
  enum Op { Neg, Not } op;
  ExprPtr e;

  EUnary(Op o, ExprPtr x) : Expr(), op(o), e(std::move(x)) {}
  EUnary(SrcPos p, Op o, ExprPtr x) : Expr(p), op(o), e(std::move(x)) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

// Binário
struct EBinary : Expr {
  enum Op { Add, Sub, Mul, Div, Mod, AndAnd, Eq, Ne, Lt, Le, Gt, Ge } op;
  ExprPtr l, r;

  EBinary(Op o, ExprPtr a, ExprPtr b) : Expr(), op(o), l(std::move(a)), r(std::move(b)) {}
  EBinary(SrcPos p, Op o, ExprPtr a, ExprPtr b) : Expr(p), op(o), l(std::move(a)), r(std::move(b)) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

// new TYPE[expr]  ou  new TYPE
struct ENew : Expr {
  std::string typeName;
  std::optional<ExprPtr> size;

  ENew(std::string t, std::optional<ExprPtr> s)
    : Expr(), typeName(std::move(t)), size(std::move(s)) {}
  ENew(SrcPos p, std::string t, std::optional<ExprPtr> s)
    : Expr(p), typeName(std::move(t)), size(std::move(s)) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

// chamada de função: NAME(args...) ou NAME(args...)[retIndex]
struct ECall : Expr {
  std::string name;
  std::vector<ExprPtr> args;
  std::optional<ExprPtr> retIndex;

  ECall() = default;
  explicit ECall(SrcPos p) : Expr(p) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

// LValue como expressão
struct ELValue : Expr {
  LValPtr lv;

  explicit ELValue(LValPtr x) : Expr(), lv(std::move(x)) {}
  ELValue(SrcPos p, LValPtr x) : Expr(p), lv(std::move(x)) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct Cmd {
  SrcPos pos;

  Cmd() = default;
  explicit Cmd(SrcPos p) : pos(p) {}

  virtual ~Cmd() = default;
  virtual void accept(AstVisitor& v) = 0;
};

struct CBlock : Cmd {
  std::vector<CmdPtr> cs;

  CBlock() = default;
  explicit CBlock(SrcPos p) : Cmd(p) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct CAssign : Cmd {
  LValPtr lhs;
  ExprPtr rhs;

  CAssign() = default;
  explicit CAssign(SrcPos p) : Cmd(p) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct CIf : Cmd {
  ExprPtr cond;
  CmdPtr thenC;
  std::optional<CmdPtr> elseC;

  CIf() = default;
  explicit CIf(SrcPos p) : Cmd(p) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct CIterate : Cmd {
  std::optional<std::string> itVar;
  ExprPtr expr;
  CmdPtr body;

  CIterate() = default;
  explicit CIterate(SrcPos p) : Cmd(p) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct CReturn : Cmd {
  std::vector<ExprPtr> exps;

  CReturn() = default;
  explicit CReturn(SrcPos p) : Cmd(p) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct CCallStmt : Cmd {
  std::string name;
  std::vector<ExprPtr> args;
  std::vector<LValPtr> rets;

  CCallStmt() = default;
  explicit CCallStmt(SrcPos p) : Cmd(p) {}

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};