#pragma once
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>
#include <set>

#include "ast/include/ast.hpp"
#include "types/include/types.hpp"

// Erro de tipo com posição
struct TypeError : std::runtime_error {
  SrcPos pos;
  TypeError(SrcPos p, const std::string& msg) : std::runtime_error(msg), pos(p) {}
};

// Assinatura de função 
struct FunSig {
  std::vector<TypePtr> params;
  std::vector<TypePtr> rets;
};

struct ClassInfo {
  std::string tyVar;  // ex: "a"
  std::unordered_map<std::string, FunSig> methods; // nomeMetodo -> assinatura (com TyVar)
};

using Theta = std::unordered_map<std::string, FunSig>;
using Delta = std::unordered_map<std::string, std::unordered_map<std::string, TypePtr>>;

// Γ com escopo (stack de maps)
struct Gamma {
  std::vector<std::unordered_map<std::string, TypePtr>> scopes;

  void push();
  void pop();

  bool existsHere(const std::string& x) const;
  TypePtr lookup(const std::string& x) const;     // procura de dentro pra fora
  void declare(const std::string& x, TypePtr t);  // declara no escopo atual
};

// Checker (Visitor)
class TypeChecker final : public AstVisitor {
public:
  // API principal
  void checkProgram(const Program& prog);

  const Theta& getTheta() const { return theta; }
  const Delta& getDelta() const { return delta; }

private:
  Theta theta;
  Delta delta;
  Gamma gamma;

  // contexto de função atual
  std::vector<TypePtr> currentReturns;

  // “resultado” da última visita (expr/lvalue)
  TypePtr resultType;
  
  // --------- Classes e Instâncias (Typeclasses) ---------

  // nomeClasse -> info da classe (tyVar, métodos)
  std::unordered_map<std::string, ClassInfo> classes;

  // nomeMetodo -> nomeClasse (pra resolver chamada tychr(...) -> pertence a qual class)
  std::unordered_map<std::string, std::string> methodOwner;

  // I: conjunto de instâncias existentes (Classe, TipoConcreto)
  std::set<std::pair<std::string, std::string>> instSet;

  // Passes para construir esses contextos
  void buildClasses(const Program& prog);
  void buildInstances(const Program& prog);

  // para parsing de tipos: variável de tipo ativa (ex: "a" em "class Functor a where ...")
  std::optional<std::string> activeTyVar;

  // ------- Construção de contextos -------
  void buildDelta(const Program& prog);
  void buildTheta(const Program& prog);
  void addPrimitiveFunctions(); // print, printb, read, readb
  void requireMainVoid(const Program& prog);

  // ------- Parsing de tipos a partir de string -------
  TypePtr parseType(const std::string& s); // "Int", "Char[]", "Pessoa[][]"
  FunSig parseFunSig(const std::vector<std::string>& params, const std::string& annot); // "Int -> Int & Int"

  // ------- Helpers -------
  [[noreturn]] void err(const SrcPos& p, const std::string& msg) const;
  void expectAssignable(const SrcPos& p, const TypePtr& target, const TypePtr& value);

  // Expr/LValue helpers
  TypePtr typeOf(const ExprPtr& e);
  TypePtr typeOf(const LValPtr& lv);

  // ------- Visitor overrides -------
  // Decls
  void visit(DataDecl&) override;
  void visit(FuncDecl&) override;

  // class/instance
  void visit(ClassDecl&) override;
  void visit(InstanceDecl&) override;


  // LValues
  void visit(LVar&) override;
  void visit(LField&) override;
  void visit(LIndex&) override;

  // Exprs
  void visit(EInt&) override;
  void visit(EFloat&) override;
  void visit(EChar&) override;
  void visit(EBool&) override;
  void visit(ENull&) override;
  void visit(EVar&) override;
  void visit(EUnary&) override;
  void visit(EBinary&) override;
  void visit(ENew&) override;
  void visit(ECall&) override;
  void visit(ELValue&) override;

  // Cmds
  void visit(CBlock&) override;
  void visit(CAssign&) override;
  void visit(CIf&) override;
  void visit(CIterate&) override;
  void visit(CReturn&) override;
  void visit(CCallStmt&) override;
};