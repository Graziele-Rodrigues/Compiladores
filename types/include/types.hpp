#pragma once
#include <memory>
#include <string>
#include <variant>
#include <vector>

struct Type;
using TypePtr = std::shared_ptr<Type>;

struct TyInt   {};
struct TyFloat {};
struct TyBool  {};
struct TyChar  {};
struct TyVoid  {};
struct TyNull  {};

struct TyUser  { std::string name; };
struct TyArray { TypePtr elem; };
struct TyTuple { std::vector<TypePtr> elems; };
struct TyVar   { std::string name; };

struct Type {
  std::variant<TyInt, TyFloat, TyBool, TyChar, TyVoid, TyNull, TyUser, TyArray, TyTuple, TyVar> v;

  static TypePtr Int();
  static TypePtr Float();
  static TypePtr Bool();
  static TypePtr Char();
  static TypePtr Void();
  static TypePtr Null();
  static TypePtr User(std::string name);
  static TypePtr Array(TypePtr elem);
  static TypePtr Tuple(std::vector<TypePtr> elems);
  static TypePtr Var(std::string name);
};

bool isVar(const TypePtr& t);
bool isPrimitive(const TypePtr& t);
bool isArray(const TypePtr& t);
bool isUser(const TypePtr& t);
bool isTuple(const TypePtr& t);
bool isNull(const TypePtr& t);

bool nullAssignableTo(const TypePtr& target);
bool typeEq(const TypePtr& a, const TypePtr& b);

std::string typeToString(const TypePtr& t);