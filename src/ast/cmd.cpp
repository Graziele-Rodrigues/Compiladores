/* Arquivo: cmd.cpp
 * Autor: Graziele Cassia Rodrigues
 * Matricula: 21.1.8120
 *
 * Descrição:
 * Implementação da execução de comandos usando Visitor.
 */

#include "include/interpreter.hpp"
#include "include/interpreter_helpers.hpp"

using interp_detail::toBool;

namespace {

struct ExecCmdVisitor final : AstVisitor {
  Interpreter& I;
  Env& env;

  ExecCmdVisitor(Interpreter& it, Env& e) : I(it), env(e) {}

  // ---------------- Cmds ----------------

  void visit(CBlock& b) override {
    for (auto& cmd : b.cs) {
      cmd->accept(*this);
    }
  }

  void visit(CAssign& a) override {
    LRef lhs = I.evalLValueRef(env, a.lhs);
    if (!lhs.slot) throw RuntimeError("Atribuicao em lvalue invalido");

    Value rhs = I.evalExpr(env, a.rhs);

    if (rhs.is<Addr>()) {
      Value& hv = I.heap.atAddr(rhs.as<Addr>().a);
      *lhs.slot = hv;
    } else {
      *lhs.slot = rhs;
    }
  }

  void visit(CIf& c) override {
    bool cond = toBool(I.evalExpr(env, c.cond));
    if (cond) {
      c.thenC->accept(*this);
    } else if (c.elseC.has_value()) {
      (*c.elseC)->accept(*this);
    }
  }

  void visit(CIterate& it) override {
    Value base = I.evalExpr(env, it.expr);

    Value* basePtr = &base;
    if (base.is<Addr>()) basePtr = &I.heap.atAddr(base.as<Addr>().a);

    // caso Int
    if (basePtr->is<long long>()) {
      long long n = basePtr->as<long long>();
      if (n <= 0) return;

      for (long long ic = 1; ic <= n; ++ic) {
        if (it.itVar.has_value()) {
          env.set(*it.itVar, Value{ (long long)(ic - 1) }); // i = ic-1
        }
        it.body->accept(*this);
      }
      return;
    }

    // caso Array
    if (!isArray(*basePtr)) throw RuntimeError("iterate espera Int ou Array");
    Array& arr = *asArray(*basePtr);

    for (size_t ic = 0; ic < arr.size(); ++ic) {
      if (it.itVar.has_value()) {
        if (!arr[ic]) throw RuntimeError("Elemento nulo interno");
        env.set(*it.itVar, *arr[ic]);
      }
      it.body->accept(*this);
    }
  }

  void visit(CReturn& r) override {
    std::vector<Value> vals;
    vals.reserve(r.exps.size());
    for (auto& e : r.exps) vals.push_back(I.evalExpr(env, e));
    throw ReturnSignal{ std::move(vals) };
  }

  void visit(CCallStmt& cs) override {
    std::vector<Value> args;
    args.reserve(cs.args.size());
    for (auto& a : cs.args) args.push_back(I.evalExpr(env, a));

    auto rets = I.callFunction(env, cs.name, args);

    if (!cs.rets.empty()) {
      if (rets.size() < cs.rets.size()) throw RuntimeError("Retornos insuficientes para <...>");

      for (size_t i = 0; i < cs.rets.size(); ++i) {
        LRef ref = I.evalLValueRef(env, cs.rets[i]);
        if (!ref.slot) throw RuntimeError("lvalue invalido em call_suffix");

        if (rets[i].is<Addr>()) {
          Value& hv = I.heap.atAddr(rets[i].as<Addr>().a);
          *ref.slot = hv;
        } else {
          *ref.slot = rets[i];
        }
      }
    }
  }

  // expectativas de erros para nós inesperados
  // como este visitor é apenas para comandos
  // nao devem ocorrer visitas a outros tipos de nós
  void visit(DataDecl&) override { throw RuntimeError("ExecCmdVisitor: decl inesperado"); }
  void visit(FuncDecl&) override { throw RuntimeError("ExecCmdVisitor: decl inesperado"); }

  void visit(LVar&) override { throw RuntimeError("ExecCmdVisitor: lvalue inesperado"); }
  void visit(LField&) override { throw RuntimeError("ExecCmdVisitor: lvalue inesperado"); }
  void visit(LIndex&) override { throw RuntimeError("ExecCmdVisitor: lvalue inesperado"); }

  void visit(EInt&) override { throw RuntimeError("ExecCmdVisitor: expr inesperada"); }
  void visit(EFloat&) override { throw RuntimeError("ExecCmdVisitor: expr inesperada"); }
  void visit(EChar&) override { throw RuntimeError("ExecCmdVisitor: expr inesperada"); }
  void visit(EBool&) override { throw RuntimeError("ExecCmdVisitor: expr inesperada"); }
  void visit(ENull&) override { throw RuntimeError("ExecCmdVisitor: expr inesperada"); }
  void visit(EVar&) override { throw RuntimeError("ExecCmdVisitor: expr inesperada"); }
  void visit(EUnary&) override { throw RuntimeError("ExecCmdVisitor: expr inesperada"); }
  void visit(EBinary&) override { throw RuntimeError("ExecCmdVisitor: expr inesperada"); }
  void visit(ENew&) override { throw RuntimeError("ExecCmdVisitor: expr inesperada"); }
  void visit(ECall&) override { throw RuntimeError("ExecCmdVisitor: expr inesperada"); }
  void visit(ELValue&) override { throw RuntimeError("ExecCmdVisitor: expr inesperada"); }
};

} // namespace

void Interpreter::execCmd(Env& env, const CmdPtr& c) {
  ExecCmdVisitor vis(*this, env);
  c->accept(vis);
}
