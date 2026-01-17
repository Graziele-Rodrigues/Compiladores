#include "include/interpreter.hpp"
#include "include/interpreter_helpers.hpp"

using interp_detail::toInt;

LRef Interpreter::evalLValueRef(Env& env, const LValPtr& lv) {
  if (auto v = dynamic_cast<LVar*>(lv.get())) {
    Value& slot = env.locals[v->name]; // cria se nao existir
    return LRef{ &slot };
  }

  if (auto f = dynamic_cast<LField*>(lv.get())) {
    LRef baseRef = evalLValueRef(env, f->base);
    if (!baseRef.slot) throw RuntimeError("LField base invalida");

    Value& baseV = derefHeapIfAddr(heap, *baseRef.slot);

    if (!isRecord(baseV)) throw RuntimeError("Acesso de campo em nao-record");
    Record& rec = *asRecord(baseV);

    auto it = rec.find(f->field);
    if (it == rec.end()) throw RuntimeError("Campo inexistente: " + f->field);
    if (!it->second) throw RuntimeError("Campo nulo interno");

    return LRef{ it->second.get() };
  }

  if (auto idx = dynamic_cast<LIndex*>(lv.get())) {
    LRef baseRef = evalLValueRef(env, idx->base);
    if (!baseRef.slot) throw RuntimeError("LIndex base invalida");

    Value& baseV = derefHeapIfAddr(heap, *baseRef.slot);

    if (!isArray(baseV)) throw RuntimeError("Indexacao em nao-array");
    Array& arr = *asArray(baseV);

    Value iV = evalExpr(env, idx->index);
    long long i = toInt(iV);
    if (i < 0 || (size_t)i >= arr.size()) throw RuntimeError("Indice fora do range");

    if (!arr[(size_t)i]) throw RuntimeError("Elemento nulo interno");
    return LRef{ arr[(size_t)i].get() };
  }

  throw RuntimeError("Tipo de lvalue desconhecido");
}
