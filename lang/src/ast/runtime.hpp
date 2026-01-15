// memoria para saber onde estao as variaveis e valores em runtime

#pragma once
#include "value.hpp"
#include <unordered_map>
#include <string>

struct Env {
  std::unordered_map<std::string, Value> locals;

  Value& get(const std::string& k) {
    auto it = locals.find(k);
    if (it == locals.end()) throw RuntimeError("Variavel nao declarada: " + k);
    return it->second;
  }
  void set(const std::string& k, Value v) { locals[k] = std::move(v); }
};

struct Heap {
  std::unordered_map<std::string, Value> mem;
  long long nextId = 1;

  std::string freshAddr() { return "α" + std::to_string(nextId++); }
  Value& atAddr(const std::string& a) {
    auto it = mem.find(a);
    if (it == mem.end()) throw RuntimeError("Endereco invalido: " + a);
    return it->second;
  }
};
