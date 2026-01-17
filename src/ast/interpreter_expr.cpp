#include "include/interpreter.hpp"
#include "include/interpreter_helpers.hpp"

using interp_detail::toInt;
using interp_detail::toFloat;
using interp_detail::toBool;

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

        if (isNum(L) && isNum(R)) {
          double a = toFloat(L), b = toFloat(R);
          int c = (a < b) ? -1 : (a > b) ? 1 : 0;
          return rel(c);
        }

        if (L.is<char>() && R.is<char>()) {
          char a = L.as<char>(), b = R.as<char>();
          int c = (a < b) ? -1 : (a > b) ? 1 : 0;
          return rel(c);
        }

        if (L.is<bool>() && R.is<bool>()) {
          if (x->op == EBinary::Eq) return Value{ L.as<bool>() == R.as<bool>() };
          if (x->op == EBinary::Ne) return Value{ L.as<bool>() != R.as<bool>() };
          throw RuntimeError("Comparacao relacional invalida para Bool");
        }

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

    if (!rets.empty()) return rets[0];
    return Value::makeNull();
  }

  throw RuntimeError("Expr desconhecida");
}
