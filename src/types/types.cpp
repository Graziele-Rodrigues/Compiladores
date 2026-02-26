#include "include/types.hpp"
#include <sstream>

TypePtr Type::Int()   { return std::make_shared<Type>(Type{TyInt{}}); }
TypePtr Type::Float() { return std::make_shared<Type>(Type{TyFloat{}}); }
TypePtr Type::Bool()  { return std::make_shared<Type>(Type{TyBool{}}); }
TypePtr Type::Char()  { return std::make_shared<Type>(Type{TyChar{}}); }
TypePtr Type::Void()  { return std::make_shared<Type>(Type{TyVoid{}}); }
TypePtr Type::Null()  { return std::make_shared<Type>(Type{TyNull{}}); }
TypePtr Type::User(std::string name) { return std::make_shared<Type>(Type{TyUser{std::move(name)}}); }
TypePtr Type::Array(TypePtr elem) { return std::make_shared<Type>(Type{TyArray{std::move(elem)}}); }
TypePtr Type::Tuple(std::vector<TypePtr> elems) { return std::make_shared<Type>(Type{TyTuple{std::move(elems)}}); }
TypePtr Type::Var(std::string name) { return std::make_shared<Type>(Type{TyVar{std::move(name)}}); }

bool isVar(const TypePtr& t) {
  return t && std::holds_alternative<TyVar>(t->v);
}

bool isPrimitive(const TypePtr& t) {
  if (!t) return false;
  return std::holds_alternative<TyInt>(t->v)   ||
         std::holds_alternative<TyFloat>(t->v) ||
         std::holds_alternative<TyBool>(t->v)  ||
         std::holds_alternative<TyChar>(t->v)  ||
         std::holds_alternative<TyVoid>(t->v);
}

bool isArray(const TypePtr& t) { return t && std::holds_alternative<TyArray>(t->v); }
bool isUser(const TypePtr& t)  { return t && std::holds_alternative<TyUser>(t->v); }
bool isTuple(const TypePtr& t) { return t && std::holds_alternative<TyTuple>(t->v); }
bool isNull(const TypePtr& t)  { return t && std::holds_alternative<TyNull>(t->v); }

bool nullAssignableTo(const TypePtr& target) {
  if (!target) return false;
  return isArray(target) || isUser(target);
}

bool typeEq(const TypePtr& a, const TypePtr& b) {
  if (!a || !b) return false;
  if (a->v.index() != b->v.index()) return false;

  if (auto ua = std::get_if<TyUser>(&a->v)) {
    auto ub = std::get<TyUser>(b->v);
    return ua->name == ub.name;
  }
  if (auto aa = std::get_if<TyArray>(&a->v)) {
    auto ab = std::get<TyArray>(b->v);
    return typeEq(aa->elem, ab.elem);
  }
  if (auto ta = std::get_if<TyTuple>(&a->v)) {
    auto tb = std::get<TyTuple>(b->v);
    if (ta->elems.size() != tb.elems.size()) return false;
    for (size_t i = 0; i < ta->elems.size(); ++i) {
      if (!typeEq(ta->elems[i], tb.elems[i])) return false;
    }
    return true;
  }
  // primitivos, Null e TyVar (mesmo index => igual)
  return true;
}

std::string typeToString(const TypePtr& t) {
  if (!t) return "<null-type-ptr>";

  if (std::holds_alternative<TyInt>(t->v)) return "Int";
  if (std::holds_alternative<TyFloat>(t->v)) return "Float";
  if (std::holds_alternative<TyBool>(t->v)) return "Bool";
  if (std::holds_alternative<TyChar>(t->v)) return "Char";
  if (std::holds_alternative<TyVoid>(t->v)) return "Void";
  if (std::holds_alternative<TyNull>(t->v)) return "null";

  if (auto u = std::get_if<TyUser>(&t->v)) return u->name;

  if (auto a = std::get_if<TyArray>(&t->v)) return typeToString(a->elem) + "[]";

  if (auto tup = std::get_if<TyTuple>(&t->v)) {
    std::ostringstream oss;
    oss << "(";
    for (size_t i = 0; i < tup->elems.size(); ++i) {
      if (i) oss << ", ";
      oss << typeToString(tup->elems[i]);
    }
    oss << ")";
    return oss.str();
  }

  if (auto tv = std::get_if<TyVar>(&t->v)) return tv->name;

  return "<?>"; // fallback
}