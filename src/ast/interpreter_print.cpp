#include "include/interpreter.hpp"
#include <iostream>

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
