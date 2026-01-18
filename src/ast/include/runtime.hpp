/*
 * Arquivo: runtime.hpp
 * Autor: Graziele Cassia Rodrigues
 * Matricula: 21.1.8120
 *
 * Descrição:
 * Este programa define as estruturas de ambiente (Env) e heap (Heap)
 * usadas durante a execução do interpretador.
 */
 
#pragma once
#include "value.hpp"
#include <unordered_map>
#include <string>

// Ambiente de variáveis locais
struct Env {
  std::unordered_map<std::string, Value> locals;

  Value& get(const std::string& k) { // obtém referência para permitir atribuição
    auto it = locals.find(k);
    if (it == locals.end()) throw RuntimeError("Variavel nao declarada: " + k);
    return it->second;
  }
  void set(const std::string& k, Value v) { locals[k] = std::move(v); } // atribuição
};

// Heap para alocação dinâmica
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
