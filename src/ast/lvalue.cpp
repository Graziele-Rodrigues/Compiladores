/* Arquivo: lvalue.cpp
 * Autor: Graziele Cassia Rodrigues
 * Matricula: 21.1.8120
 *
 * Descrição:
 * Implementação da avaliação de lvalues usando Visitor.
 */

#include "include/interpreter.hpp"
#include "include/interpreter_helpers.hpp"

using interp_detail::toInt;

namespace {

struct EvalLValueRefVisitor final : AstVisitor {
  Interpreter& I;
  Env& env;
  LRef result;

  EvalLValueRefVisitor(Interpreter& it, Env& e) : I(it), env(e), result{} {}

  // ---------------- LValues ----------------

  void visit(LVar& v) override {
    Value& slot = env.locals[v.name]; // cria se nao existir
    result = LRef{ &slot };
  }

  void visit(LField& f) override {
    LRef baseRef = I.evalLValueRef(env, f.base);
    if (!baseRef.slot) throw RuntimeError("LField base invalida");

    Value& baseV = derefHeapIfAddr(I.heap, *baseRef.slot);

    if (!isRecord(baseV)) throw RuntimeError("Acesso de campo em nao-record");
    Record& rec = *asRecord(baseV);

    auto it = rec.find(f.field);
    if (it == rec.end()) throw RuntimeError("Campo inexistente: " + f.field);
    if (!it->second) throw RuntimeError("Campo nulo interno");

    result = LRef{ it->second.get() };
  }

  void visit(LIndex& idx) override {
    LRef baseRef = I.evalLValueRef(env, idx.base);
    if (!baseRef.slot) throw RuntimeError("LIndex base invalida");

    Value& baseV = derefHeapIfAddr(I.heap, *baseRef.slot);

    if (!isArray(baseV)) throw RuntimeError("Indexacao em nao-array");
    Array& arr = *asArray(baseV);

    Value iV = I.evalExpr(env, idx.index);
    long long i = toInt(iV);
    if (i < 0 || (size_t)i >= arr.size()) throw RuntimeError("Indice fora do range");

    if (!arr[(size_t)i]) throw RuntimeError("Elemento nulo interno");
    result = LRef{ arr[(size_t)i].get() };
  }

  // ---------------- Não usados aqui ----------------

  void visit(DataDecl&) override { throw RuntimeError("EvalLValueRefVisitor: decl inesperado"); }
  void visit(FuncDecl&) override { throw RuntimeError("EvalLValueRefVisitor: decl inesperado"); }

  void visit(EInt&) override { throw RuntimeError("EvalLValueRefVisitor: expr inesperada"); }
  void visit(EFloat&) override { throw RuntimeError("EvalLValueRefVisitor: expr inesperada"); }
  void visit(EChar&) override { throw RuntimeError("EvalLValueRefVisitor: expr inesperada"); }
  void visit(EBool&) override { throw RuntimeError("EvalLValueRefVisitor: expr inesperada"); }
  void visit(ENull&) override { throw RuntimeError("EvalLValueRefVisitor: expr inesperada"); }
  void visit(EVar&) override { throw RuntimeError("EvalLValueRefVisitor: expr inesperada"); }
  void visit(EUnary&) override { throw RuntimeError("EvalLValueRefVisitor: expr inesperada"); }
  void visit(EBinary&) override { throw RuntimeError("EvalLValueRefVisitor: expr inesperada"); }
  void visit(ENew&) override { throw RuntimeError("EvalLValueRefVisitor: expr inesperada"); }
  void visit(ECall&) override { throw RuntimeError("EvalLValueRefVisitor: expr inesperada"); }
  void visit(ELValue&) override { throw RuntimeError("EvalLValueRefVisitor: expr inesperada"); }

  void visit(CBlock&) override { throw RuntimeError("EvalLValueRefVisitor: cmd inesperado"); }
  void visit(CAssign&) override { throw RuntimeError("EvalLValueRefVisitor: cmd inesperado"); }
  void visit(CIf&) override { throw RuntimeError("EvalLValueRefVisitor: cmd inesperado"); }
  void visit(CIterate&) override { throw RuntimeError("EvalLValueRefVisitor: cmd inesperado"); }
  void visit(CReturn&) override { throw RuntimeError("EvalLValueRefVisitor: cmd inesperado"); }
  void visit(CCallStmt&) override { throw RuntimeError("EvalLValueRefVisitor: cmd inesperado"); }
};

} // namespace

LRef Interpreter::evalLValueRef(Env& env, const LValPtr& lv) {
  EvalLValueRefVisitor vis(*this, env);
  lv->accept(vis);
  return vis.result;
}
