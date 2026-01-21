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
struct SrcPos { int line=0, col=0; };

struct Decl;
struct DataDecl;
struct FuncDecl;

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

// declaracao do programa
struct Program {
  std::vector<DeclPtr> decls;
};

// declaracao da definicao
struct Decl {
  virtual ~Decl() = default;
  virtual void accept(AstVisitor& v) = 0;
};

// data TYID { field :: TYPE; ... }
struct DataDecl : Decl {
  std::string typeName;
  std::vector<std::pair<std::string,std::string>> fields;

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

// func ID params :: typeAnnot block
struct FuncDecl : Decl {
  std::string name;
  std::vector<std::string> params;
  std::string typeAnnotText;
  CmdPtr body;

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

// Para decls ainda não implementadas (classDec/instanceDec)
struct DummyDecl : Decl {
  void accept(AstVisitor&) override {
    // placeholder: nada a fazer
  }
};


// LValue (base) para atribuição - L de esquerda
struct LValue {
  SrcPos pos;
  virtual ~LValue() = default;
  virtual void accept(AstVisitor& v) = 0;
};

struct LVar : LValue {
  std::string name;
  explicit LVar(std::string n) : name(std::move(n)) {}
  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct LField : LValue {
  LValPtr base;
  std::string field;
  LField(LValPtr b, std::string f) : base(std::move(b)), field(std::move(f)) {}
  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct LIndex : LValue {
  LValPtr base;
  ExprPtr index;
  LIndex(LValPtr b, ExprPtr i) : base(std::move(b)), index(std::move(i)) {}
  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

// Expr (base)
struct Expr {
  SrcPos pos;
  virtual ~Expr() = default;
  virtual void accept(AstVisitor& v) = 0;
};

// Literais
struct EInt : Expr {
  long long v;
  explicit EInt(long long x) : v(x) {}
  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct EFloat : Expr {
  double v;
  explicit EFloat(double x) : v(x) {}
  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct EChar : Expr {
  char v;
  explicit EChar(char x) : v(x) {}
  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct EBool : Expr {
  bool v;
  explicit EBool(bool x) : v(x) {}
  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct ENull : Expr {
  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct EVar : Expr {
  std::string name;
  explicit EVar(std::string n) : name(std::move(n)) {}
  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

// Unário
struct EUnary : Expr {
  enum Op { Neg, Not } op;
  ExprPtr e;
  EUnary(Op o, ExprPtr x) : op(o), e(std::move(x)) {}
  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

// Binário
struct EBinary : Expr {
  enum Op { Add, Sub, Mul, Div, Mod, AndAnd, Eq, Ne, Lt, Le, Gt, Ge } op;
  ExprPtr l, r;
  EBinary(Op o, ExprPtr a, ExprPtr b) : op(o), l(std::move(a)), r(std::move(b)) {}
  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

// new TYPE[expr]  ou  new TYPE
struct ENew : Expr {
  std::string typeName;
  std::optional<ExprPtr> size;
  ENew(std::string t, std::optional<ExprPtr> s)
    : typeName(std::move(t)), size(std::move(s)) {}
  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

// chamada de função: NAME(args...) ou NAME(args...)[retIndex]
struct ECall : Expr {
  std::string name;
  std::vector<ExprPtr> args;
  std::optional<ExprPtr> retIndex;

  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

// LValue como expressão
struct ELValue : Expr {
  LValPtr lv;
  explicit ELValue(LValPtr x) : lv(std::move(x)) {}
  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

// Cmd (base)
struct Cmd {
  SrcPos pos;
  virtual ~Cmd() = default;
  virtual void accept(AstVisitor& v) = 0;
};

struct CBlock : Cmd {
  std::vector<CmdPtr> cs;
  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct CAssign : Cmd {
  LValPtr lhs;
  ExprPtr rhs;
  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct CIf : Cmd {
  ExprPtr cond;
  CmdPtr thenC;
  std::optional<CmdPtr> elseC;
  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct CIterate : Cmd {
  std::optional<std::string> itVar;
  ExprPtr expr;
  CmdPtr body;
  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct CReturn : Cmd {
  std::vector<ExprPtr> exps;
  void accept(AstVisitor& vis) override { vis.visit(*this); }
};

struct CCallStmt : Cmd {
  std::string name;
  std::vector<ExprPtr> args;
  std::vector<LValPtr> rets;
  void accept(AstVisitor& vis) override { vis.visit(*this); }
};
