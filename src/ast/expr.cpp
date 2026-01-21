/* Arquivo: expr.cpp
 * Autor: Graziele Cassia Rodrigues
 * Matricula: 21.1.8120
 *
 * Descrição:
 * Implementação da avaliação de expressões usando Visitor.
 */

#include "include/interpreter.hpp"
#include "include/interpreter_helpers.hpp"

using interp_detail::toInt;
using interp_detail::toFloat;
using interp_detail::toBool;

namespace {

struct EvalExprVisitor final : AstVisitor {
  Interpreter& I;
  Env& env;
  Value result;

  EvalExprVisitor(Interpreter& it, Env& e)
    : I(it), env(e), result(Value::makeNull()) {}

  // ---------------- Exprs ----------------

  void visit(ENull&) override { result = Value::makeNull(); }
  void visit(EInt& x) override { result = Value{ (long long)x.v }; }
  void visit(EFloat& x) override { result = Value{ (double)x.v }; }
  void visit(EChar& x) override { result = Value{ (char)x.v }; }
  void visit(EBool& x) override { result = Value{ (bool)x.v }; }

  void visit(EVar& x) override {
    result = env.get(x.name);
  }

  void visit(ELValue& x) override {
    LRef r = I.evalLValueRef(env, x.lv);
    if (!r.slot) throw RuntimeError("ELValue invalido");
    result = *r.slot;
  }

  void visit(EUnary& x) override {
    Value v = I.evalExpr(env, x.e);

    if (x.op == EUnary::Not) {
      result = Value{ !toBool(v) };
      return;
    }

    if (x.op == EUnary::Neg) {
      if (v.is<long long>()) { result = Value{ -v.as<long long>() }; return; }
      if (v.is<double>())    { result = Value{ -v.as<double>() }; return; }
      throw RuntimeError("Negacao aritmetica em tipo invalido");
    }

    throw RuntimeError("Unary op desconhecido");
  }

  void visit(EBinary& x) override {
    if (x.op == EBinary::AndAnd) {
      bool lb = toBool(I.evalExpr(env, x.l));
      if (!lb) { result = Value{ false }; return; }
      bool rb = toBool(I.evalExpr(env, x.r));
      result = Value{ lb && rb };
      return;
    }

    Value L = I.evalExpr(env, x.l);
    Value R = I.evalExpr(env, x.r);

    auto isNum = [&](const Value& v){ return v.is<long long>() || v.is<double>(); };

    switch (x.op) {
      case EBinary::Add:
      case EBinary::Sub:
      case EBinary::Mul:
      case EBinary::Div:
      case EBinary::Mod: {
        if (L.is<long long>() && R.is<long long>()) {
          long long a = L.as<long long>(), b = R.as<long long>();
          if (x.op == EBinary::Add) { result = Value{ a + b }; return; }
          if (x.op == EBinary::Sub) { result = Value{ a - b }; return; }
          if (x.op == EBinary::Mul) { result = Value{ a * b }; return; }
          if (x.op == EBinary::Div) {
            if (b == 0) throw RuntimeError("Divisao por zero");
            result = Value{ a / b };
            return;
          }
          if (x.op == EBinary::Mod) {
            if (b == 0) throw RuntimeError("Modulo por zero");
            result = Value{ a % b };
            return;
          }
        }

        if (isNum(L) && isNum(R)) {
          double a = toFloat(L), b = toFloat(R);
          if (x.op == EBinary::Add) { result = Value{ a + b }; return; }
          if (x.op == EBinary::Sub) { result = Value{ a - b }; return; }
          if (x.op == EBinary::Mul) { result = Value{ a * b }; return; }
          if (x.op == EBinary::Div) {
            if (b == 0.0) throw RuntimeError("Divisao por zero");
            result = Value{ a / b };
            return;
          }
          if (x.op == EBinary::Mod) throw RuntimeError("Modulo nao definido para Float");
        }

        throw RuntimeError("Operacao aritmetica em tipos invalidos");
      }

      case EBinary::Eq: case EBinary::Ne:
      case EBinary::Lt: case EBinary::Le:
      case EBinary::Gt: case EBinary::Ge: {

        auto rel = [&](int c)->Value {
          switch (x.op) {
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
          result = rel(c);
          return;
        }

        if (L.is<char>() && R.is<char>()) {
          char a = L.as<char>(), b = R.as<char>();
          int c = (a < b) ? -1 : (a > b) ? 1 : 0;
          result = rel(c);
          return;
        }

        if (L.is<bool>() && R.is<bool>()) {
          if (x.op == EBinary::Eq) { result = Value{ L.as<bool>() == R.as<bool>() }; return; }
          if (x.op == EBinary::Ne) { result = Value{ L.as<bool>() != R.as<bool>() }; return; }
          throw RuntimeError("Comparacao relacional invalida para Bool");
        }

        if ((L.is<Null>() || L.is<Addr>()) && (R.is<Null>() || R.is<Addr>())) {
          bool eq = false;
          if (L.is<Null>() && R.is<Null>()) eq = true;
          else if (L.is<Addr>() && R.is<Addr>()) eq = (L.as<Addr>().a == R.as<Addr>().a);
          else eq = false;

          if (x.op == EBinary::Eq) { result = Value{ eq }; return; }
          if (x.op == EBinary::Ne) { result = Value{ !eq }; return; }
          throw RuntimeError("Comparacao relacional invalida para Null/Addr");
        }

        throw RuntimeError("Comparacao entre tipos incompativeis");
      }

      default:
        break;
    }

    throw RuntimeError("Binary op desconhecido");
  }

  void visit(ENew& x) override {
    std::optional<long long> size;
    if (x.size.has_value()) {
      Value sz = I.evalExpr(env, *x.size);
      size = toInt(sz);
    }
    result = I.newValue(x.typeName, size);
  }

  void visit(ECall& x) override {
    std::vector<Value> args;
    args.reserve(x.args.size());
    for (auto& a : x.args) args.push_back(I.evalExpr(env, a));

    auto rets = I.callFunction(env, x.name, args);

    if (x.retIndex.has_value()) {
      long long k = toInt(I.evalExpr(env, *x.retIndex));
      if (k < 0 || (size_t)k >= rets.size()) throw RuntimeError("Indice de retorno fora do range");
      result = rets[(size_t)k];
      return;
    }

    if (!rets.empty()) result = rets[0];
    else result = Value::makeNull();
  }

  // ---------------- Não usados aqui ----------------

  void visit(DataDecl&) override { throw RuntimeError("EvalExprVisitor: decl inesperado"); }
  void visit(FuncDecl&) override { throw RuntimeError("EvalExprVisitor: decl inesperado"); }

  void visit(LVar&) override { throw RuntimeError("EvalExprVisitor: lvalue inesperado"); }
  void visit(LField&) override { throw RuntimeError("EvalExprVisitor: lvalue inesperado"); }
  void visit(LIndex&) override { throw RuntimeError("EvalExprVisitor: lvalue inesperado"); }

  void visit(CBlock&) override { throw RuntimeError("EvalExprVisitor: cmd inesperado"); }
  void visit(CAssign&) override { throw RuntimeError("EvalExprVisitor: cmd inesperado"); }
  void visit(CIf&) override { throw RuntimeError("EvalExprVisitor: cmd inesperado"); }
  void visit(CIterate&) override { throw RuntimeError("EvalExprVisitor: cmd inesperado"); }
  void visit(CReturn&) override { throw RuntimeError("EvalExprVisitor: cmd inesperado"); }
  void visit(CCallStmt&) override { throw RuntimeError("EvalExprVisitor: cmd inesperado"); }
};

} // namespace

Value Interpreter::evalExpr(Env& env, const ExprPtr& e) {
  EvalExprVisitor vis(*this, env);
  e->accept(vis);
  return vis.result;
}
