/* Arquivo: interpreter_cmd.cpp
 * Autor: Graziele Cassia Rodrigues
 * Matricula: 21.1.8120
 *
 * Descrição:
 * Implementação da execução de comandos.
 */

#include "include/interpreter.hpp"
#include "include/interpreter_helpers.hpp"

using interp_detail::toInt;
using interp_detail::toBool;

void Interpreter::execCmd(Env& env, const CmdPtr& c) {

  // Se for bloco, executa cada comando sequencialmente
  if (auto b = dynamic_cast<CBlock*>(c.get())) {
    for (auto& cmd : b->cs) execCmd(env, cmd);
    return;
  }

  // Atribuição
  if (auto a = dynamic_cast<CAssign*>(c.get())) {
    LRef lhs = evalLValueRef(env, a->lhs);
    if (!lhs.slot) throw RuntimeError("Atribuicao em lvalue invalido");
    Value rhs = evalExpr(env, a->rhs);

    if (rhs.is<Addr>()) {
      Value& hv = heap.atAddr(rhs.as<Addr>().a);
      *lhs.slot = hv;
    } else {
      *lhs.slot = rhs;
    }
    return;
  }
  
  // If-Then-Else
  if (auto i = dynamic_cast<CIf*>(c.get())) {
    bool cond = toBool(evalExpr(env, i->cond));
    if (cond) execCmd(env, i->thenC);
    else if (i->elseC.has_value()) execCmd(env, *i->elseC);
    return;
  }

  // Iterate
  if (auto it = dynamic_cast<CIterate*>(c.get())) {
    Value base = evalExpr(env, it->expr);

    Value* basePtr = &base;
    if (base.is<Addr>()) basePtr = &heap.atAddr(base.as<Addr>().a);

    // caso Int
    if (basePtr->is<long long>()) {
      long long n = basePtr->as<long long>();
      if (n <= 0) return;

      for (long long ic = 1; ic <= n; ++ic) {
        if (it->itVar.has_value()) {
          env.set(*it->itVar, Value{ (long long)(ic - 1) }); // i = Ic-1
        }
        execCmd(env, it->body);
      }
      return;
    }

    // caso Array
    if (!isArray(*basePtr)) throw RuntimeError("iterate espera Int ou Array");
    Array& arr = *asArray(*basePtr);

    for (size_t ic = 0; ic < arr.size(); ++ic) {
      if (it->itVar.has_value()) {
        if (!arr[ic]) throw RuntimeError("Elemento nulo interno");
        env.set(*it->itVar, *arr[ic]); 
      }
      execCmd(env, it->body);
    }
    return;
  }

  // Return
  if (auto r = dynamic_cast<CReturn*>(c.get())) {
    std::vector<Value> vals;
    vals.reserve(r->exps.size());
    for (auto& e : r->exps) vals.push_back(evalExpr(env, e));
    throw ReturnSignal{ std::move(vals) };
  }

  // CallStmt, com possível atribuição de retornos
  if (auto cs = dynamic_cast<CCallStmt*>(c.get())) {
    std::vector<Value> args;
    args.reserve(cs->args.size());
    for (auto& a : cs->args) args.push_back(evalExpr(env, a));

    auto rets = callFunction(env, cs->name, args);

    if (!cs->rets.empty()) {
      if (rets.size() < cs->rets.size()) throw RuntimeError("Retornos insuficientes para <...>");

      for (size_t i = 0; i < cs->rets.size(); ++i) {
        LRef ref = evalLValueRef(env, cs->rets[i]);
        if (!ref.slot) throw RuntimeError("lvalue invalido em call_suffix");

        if (rets[i].is<Addr>()) {
          Value& hv = heap.atAddr(rets[i].as<Addr>().a);
          *ref.slot = hv;
        } else {
          *ref.slot = rets[i];
        }
      }
    }
    return;
  }

  throw RuntimeError("Cmd desconhecido");
}
