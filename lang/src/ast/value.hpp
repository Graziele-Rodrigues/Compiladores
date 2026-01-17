#pragma once
#include <variant>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <memory>

struct Null { };

struct Addr { std::string a; }; // "α1", "α2", ...

struct Value; // forward

using ValuePtr = std::shared_ptr<Value>;
using Array    = std::vector<ValuePtr>;
using Record   = std::unordered_map<std::string, ValuePtr>;

struct Value {
  using V = std::variant<long long, double, bool, char, Null,
                         std::shared_ptr<Array>,
                         std::shared_ptr<Record>,
                         Addr>;

  V v;

  static Value makeNull() { return Value{Null{}}; }
  static Value makeAddr(std::string a) { return Value{Addr{std::move(a)}}; }

  static Value makeArray() { return Value{std::make_shared<Array>()}; }
  static Value makeRecord(){ return Value{std::make_shared<Record>()}; }

  template<class T> bool is() const { return std::holds_alternative<T>(v); }
  template<class T> T& as(){ return std::get<T>(v); }
  template<class T> const T& as() const { return std::get<T>(v); }
};

inline bool isNull(const Value& x) { return x.is<Null>(); }

// helpers: detectar array/record sem expor tipo no resto
inline bool isArray(const Value& x){ return x.is<std::shared_ptr<Array>>(); }
inline bool isRecord(const Value& x){ return x.is<std::shared_ptr<Record>>(); }

inline std::shared_ptr<Array>& asArray(Value& x){ return x.as<std::shared_ptr<Array>>(); }
inline std::shared_ptr<Record>& asRecord(Value& x){ return x.as<std::shared_ptr<Record>>(); }

struct RuntimeError : std::runtime_error { using std::runtime_error::runtime_error; };
