#include "interpreter.hpp"
#include <iostream>
#include <sstream>

// ---------- helpers de tipo ----------

static long long toInt(const Value& v) {
  if (!v.is<long long>()) throw RuntimeError("Esperava Int");
  return v.as<long long>();
}

static double toFloat(const Value& v) {
  if (v.is<double>()) return v.as<double>();
  if (v.is<long long>()) return (double)v.as<long long>();
  throw RuntimeError("Esperava Float/Int");
}

static bool toBool(const Value& v) {
  if (!v.is<bool>()) throw RuntimeError("Esperava Bool");
  return v.as<bool>();
}

// ---------- impressão (primitiva print) ----------

void Interpreter::prim_print(const Value& v) {
  if (v.is<char>()) {
    std::cout << v.as<char>();
  } else if (v.is<long long>()) {
    std::cout << v.as<long long>();
  } else if (v.is<double>()) {
    std::cout << v.as<double>();
  } else if (v.is<bool>()) {
    std::cout << (v.as<bool>() ? "true" : "false");
  } else if (v.is<Null>()) {
    std::cout << "null";
  } else if (v.is<Addr>()) {
    std::cout << v.as<Addr>().a;
  } else if (isArray(v)) {
    std::cout << "<array>";
  } else if (isRecord(v)) {
    std::cout << "<record>";
  } else {
    std::cout << "<value>";
  }
  std::cout << "\n";
}

// ---------- carregamento do programa ----------

void Interpreter::loadProgram(const Program& p) {
  dataDefs.clear();
  funcs.clear();

  for (auto& d : p.decls) {
    if (auto dd = dynamic_cast<DataDecl*>(d.get())) {
      dataDefs[dd->typeName] = dd;
    } else if (auto fd = dynamic_cast<FuncDecl*>(d.get())) {
      funcs[fd->name] = fd;
    } else {
      // class/instance ignorados por enquanto
    }
  }
}

void Interpreter::runMain() {
  Env env;
  auto it = funcs.find("main");
  if (it == funcs.end()) throw RuntimeError("Nao existe funcao main");
  if (!it->second->params.empty()) throw RuntimeError("main nao deve ter parametros");

  (void)callFunction(env, "main", {});
}

// ---------- new ----------

Value Interpreter::newValue(const std::string& typeName, std::optional<long long> sizeOpt) {
  auto defaultPrim = [&](const std::string& t)->Value {
    if (t == "Int")   return Value{ (long long)0 };
    if (t == "Float") return Value{ (double)0.0 };
    if (t == "Bool")  return Value{ false };
    if (t == "Char")  return Value{ (char)0 };
    return Value::makeNull(); // user types => null
  };

  const bool isUserType = (dataDefs.find(typeName) != dataDefs.end());

  std::string a = heap.freshAddr();
  Value allocated;

  if (sizeOpt.has_value()) {
    long long n = *sizeOpt;
    if (n < 0) throw RuntimeError("new com tamanho negativo");

    allocated = Value::makeArray();
    auto arrPtr = asArray(allocated); // shared_ptr<Array>&
    arrPtr->reserve((size_t)n);

    if (isUserType) {
      for (long long i = 0; i < n; ++i) {
        arrPtr->push_back(std::make_shared<Value>(Value::makeNull()));
      }
    } else {
      for (long long i = 0; i < n; ++i) {
        arrPtr->push_back(std::make_shared<Value>(defaultPrim(typeName)));
      }
    }
  } else {
    // sem tamanho -> registro TYID
    if (!isUserType) throw RuntimeError("new sem tamanho so pode para TYID");

    allocated = Value::makeRecord();
    auto recPtr = asRecord(allocated);

    DataDecl* dd = dataDefs[typeName];
    for (auto& [field, ftype] : dd->fields) {
      if (ftype == "Int" || ftype == "Float" || ftype == "Bool" || ftype == "Char") {
        (*recPtr)[field] = std::make_shared<Value>(defaultPrim(ftype));
      } else {
        (*recPtr)[field] = std::make_shared<Value>(Value::makeNull());
      }
    }
  }

  heap.mem[a] = std::move(allocated);
  return Value::makeAddr(a);
}

// ---------- lvalue -> referência gravável ----------

LRef Interpreter::evalLValueRef(Env& env, const LValPtr& lv) {
  if (auto v = dynamic_cast<LVar*>(lv.get())) {
    /// Value& slot = env.get(v->name); se nao permitir criar x = 1
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

// ---------- expressão ----------

Value Interpreter::evalExpr(Env& env, const ExprPtr& e) {
  if (dynamic_cast<ENull*>(e.get())) return Value::makeNull();

  if (auto x = dynamic_cast<EInt*>(e.get()))   return Value{ (long long)x->v };
  if (auto x = dynamic_cast<EFloat*>(e.get())) return Value{ (double)x->v };
  if (auto x = dynamic_cast<EChar*>(e.get()))  return Value{ (char)x->v };
  if (auto x = dynamic_cast<EBool*>(e.get()))  return Value{ (bool)x->v };

  if (auto x = dynamic_cast<EVar*>(e.get())) {
    return env.get(x->name);
  }

  if (auto x = dynamic_cast<ELValue*>(e.get())) {
    LRef r = evalLValueRef(env, x->lv);
    if (!r.slot) throw RuntimeError("ELValue invalido");
    return *r.slot;
  }

  if (auto x = dynamic_cast<EUnary*>(e.get())) {
    Value v = evalExpr(env, x->e);
    if (x->op == EUnary::Not) return Value{ !toBool(v) };
    if (x->op == EUnary::Neg) {
      if (v.is<long long>()) return Value{ -v.as<long long>() };
      if (v.is<double>())    return Value{ -v.as<double>() };
      throw RuntimeError("Negacao aritmetica em tipo invalido");
    }
    throw RuntimeError("Unary op desconhecido");
  }

  if (auto x = dynamic_cast<EBinary*>(e.get())) {
    // curto-circuito correto do &&
    if (x->op == EBinary::AndAnd) {
      bool lb = toBool(evalExpr(env, x->l));
      if (!lb) return Value{ false };
      bool rb = toBool(evalExpr(env, x->r));
      return Value{ lb && rb };
    }

    Value L = evalExpr(env, x->l);
    Value R = evalExpr(env, x->r);

    // aritmética Int/Float
    auto isNum = [&](const Value& v){ return v.is<long long>() || v.is<double>(); };

    switch (x->op) {
      case EBinary::Add:
      case EBinary::Sub:
      case EBinary::Mul:
      case EBinary::Div:
      case EBinary::Mod: {
        if (L.is<long long>() && R.is<long long>()) {
          long long a = L.as<long long>(), b = R.as<long long>();
          if (x->op == EBinary::Add) return Value{ a + b };
          if (x->op == EBinary::Sub) return Value{ a - b };
          if (x->op == EBinary::Mul) return Value{ a * b };
          if (x->op == EBinary::Div) {
            if (b == 0) throw RuntimeError("Divisao por zero");
            return Value{ a / b };
          }
          if (x->op == EBinary::Mod) {
            if (b == 0) throw RuntimeError("Modulo por zero");
            return Value{ a % b };
          }
          break;
        }

        if (isNum(L) && isNum(R)) {
          double a = toFloat(L), b = toFloat(R);
          if (x->op == EBinary::Add) return Value{ a + b };
          if (x->op == EBinary::Sub) return Value{ a - b };
          if (x->op == EBinary::Mul) return Value{ a * b };
          if (x->op == EBinary::Div) {
            if (b == 0.0) throw RuntimeError("Divisao por zero");
            return Value{ a / b };
          }
          if (x->op == EBinary::Mod) throw RuntimeError("Modulo nao definido para Float");
        }

        throw RuntimeError("Operacao aritmetica em tipos invalidos");
      }

      case EBinary::Eq: case EBinary::Ne:
      case EBinary::Lt: case EBinary::Le:
      case EBinary::Gt: case EBinary::Ge: {
        auto rel = [&](int c)->Value {
          switch (x->op) {
            case EBinary::Eq: return Value{ c == 0 };
            case EBinary::Ne: return Value{ c != 0 };
            case EBinary::Lt: return Value{ c < 0 };
            case EBinary::Le: return Value{ c <= 0 };
            case EBinary::Gt: return Value{ c > 0 };
            case EBinary::Ge: return Value{ c >= 0 };
            default: throw RuntimeError("rel invalido");
          }
        };

        // num
        if (isNum(L) && isNum(R)) {
          double a = toFloat(L), b = toFloat(R);
          int c = (a < b) ? -1 : (a > b) ? 1 : 0;
          return rel(c);
        }
        // char
        if (L.is<char>() && R.is<char>()) {
          char a = L.as<char>(), b = R.as<char>();
          int c = (a < b) ? -1 : (a > b) ? 1 : 0;
          return rel(c);
        }
        // bool (eq/ne só)
        if (L.is<bool>() && R.is<bool>()) {
          if (x->op == EBinary::Eq) return Value{ L.as<bool>() == R.as<bool>() };
          if (x->op == EBinary::Ne) return Value{ L.as<bool>() != R.as<bool>() };
          throw RuntimeError("Comparacao relacional invalida para Bool");
        }
        // null/addr (eq/ne só)
        if ((L.is<Null>() || L.is<Addr>()) && (R.is<Null>() || R.is<Addr>())) {
          bool eq = false;
          if (L.is<Null>() && R.is<Null>()) eq = true;
          else if (L.is<Addr>() && R.is<Addr>()) eq = (L.as<Addr>().a == R.as<Addr>().a);
          else eq = false;

          if (x->op == EBinary::Eq) return Value{ eq };
          if (x->op == EBinary::Ne) return Value{ !eq };
          throw RuntimeError("Comparacao relacional invalida para Null/Addr");
        }

        throw RuntimeError("Comparacao entre tipos incompativeis");
      }

      default:
        break;
    }

    throw RuntimeError("Binary op desconhecido");
  }

  if (auto x = dynamic_cast<ENew*>(e.get())) {
    std::optional<long long> size;
    if (x->size.has_value()) {
      Value sz = evalExpr(env, *x->size);
      size = toInt(sz);
    }
    return newValue(x->typeName, size);
  }

  if (auto x = dynamic_cast<ECall*>(e.get())) {
    std::vector<Value> args;
    args.reserve(x->args.size());
    for (auto& a : x->args) args.push_back(evalExpr(env, a));

    auto rets = callFunction(env, x->name, args);

    if (x->retIndex.has_value()) {
      long long k = toInt(evalExpr(env, *x->retIndex));
      if (k < 0 || (size_t)k >= rets.size()) throw RuntimeError("Indice de retorno fora do range");
      return rets[(size_t)k];
    }

    // política: se não tiver [k], retorna o primeiro retorno se existir
    if (!rets.empty()) return rets[0];
    return Value::makeNull();
  }

  throw RuntimeError("Expr desconhecida");
}

// ---------- comandos ----------

void Interpreter::execCmd(Env& env, const CmdPtr& c) {
  if (auto b = dynamic_cast<CBlock*>(c.get())) {
    for (auto& cmd : b->cs) execCmd(env, cmd);
    return;
  }

  if (auto a = dynamic_cast<CAssign*>(c.get())) {
    LRef lhs = evalLValueRef(env, a->lhs);
    if (!lhs.slot) throw RuntimeError("Atribuicao em lvalue invalido");
    Value rhs = evalExpr(env, a->rhs);

    // regra: se rhs é Addr, copia conteudo do heap (ajuste se seu professor quiser copiar endereço)
    if (rhs.is<Addr>()) {
      Value& hv = heap.atAddr(rhs.as<Addr>().a);
      *lhs.slot = hv;
    } else {
      *lhs.slot = rhs;
    }
    return;
  }

  if (auto i = dynamic_cast<CIf*>(c.get())) {
    bool cond = toBool(evalExpr(env, i->cond));
    if (cond) execCmd(env, i->thenC);
    else if (i->elseC.has_value()) execCmd(env, *i->elseC);
    return;
  }

  if (auto it = dynamic_cast<CIterate*>(c.get())) {
    Value base = evalExpr(env, it->expr);

    // se for Addr -> deref
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
        env.set(*it->itVar, *arr[ic]); // i recebe o elemento
      }
      execCmd(env, it->body);
    }
    return;
  }

  if (auto r = dynamic_cast<CReturn*>(c.get())) {
    std::vector<Value> vals;
    vals.reserve(r->exps.size());
    for (auto& e : r->exps) vals.push_back(evalExpr(env, e));
    throw ReturnSignal{ std::move(vals) };
  }

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

// ---------- chamada de função ----------

std::vector<Value> Interpreter::callFunction(Env& caller, const std::string& name,
                                            const std::vector<Value>& args) {
  // primitivas
  if (name == "print") {
    if (args.size() != 1) throw RuntimeError("print espera 1 argumento");
    prim_print(args[0]);
    return {};
  }

  auto it = funcs.find(name);
  if (it == funcs.end()) throw RuntimeError("Funcao nao declarada: " + name);
  FuncDecl* f = it->second;

  if (args.size() != f->params.size()) throw RuntimeError("Aridade incorreta em " + name);

  Env callee;
  for (size_t i = 0; i < args.size(); ++i) {
    callee.set(f->params[i], args[i]);
  }

  try {
    execCmd(callee, f->body);
    return {}; // sem return
  } catch (const ReturnSignal& rs) {
    return rs.values;
  }
}
