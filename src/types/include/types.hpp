#pragma once
#include <memory>
#include <string>
#include <vector>
#include <variant>

struct Type;
using TypePtr = std::shared_ptr<Type>;

// Tipos básicos
struct TyInt {};
struct TyFloat {};
struct TyBool {};
struct TyChar {};
struct TyVoid {};
struct TyVar { std::string name; };  // variável de tipo, ex: a

// Tipos compostos
struct TyUser { std::string name; };          // data Pessoa {...}
struct TyArray { TypePtr elem; };             // τ[]
struct TyTuple { std::vector<TypePtr> elems; }; // (τ1, τ2, ...)
struct TyNull {};                             // null (marcador especial)

// Representação de tipo
struct Type {
  using V = std::variant<TyInt, TyFloat, TyBool, TyChar, TyVoid, TyUser, TyArray, TyTuple, TyNull, TyVar>;
  V v;

  static TypePtr Int();
  static TypePtr Float();
  static TypePtr Bool();
  static TypePtr Char();
  static TypePtr Void();
  static TypePtr User(std::string name);
  static TypePtr Array(TypePtr elem);
  static TypePtr Tuple(std::vector<TypePtr> elems);
  static TypePtr Null();
  static TypePtr Var(std::string name);
};

// Utilidades
bool typeEq(const TypePtr& a, const TypePtr& b);
bool isPrimitive(const TypePtr& t);      // Int/Float/Bool/Char/Void
bool isArray(const TypePtr& t);
bool isUser(const TypePtr& t);
bool isTuple(const TypePtr& t);
bool isNull(const TypePtr& t);
bool isVar(const TypePtr& t);


// Regra do null: null pode ser atribuído a User e Array (mas não primitivos)
bool nullAssignableTo(const TypePtr& target);

// Para mensagens de erro
std::string typeToString(const TypePtr& t);
