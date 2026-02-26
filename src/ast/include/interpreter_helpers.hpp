/*
 * Arquivo: interpreter_helpers.hpp
 * Autor: Graziele Cassia Rodrigues
 * Matricula: 21.1.8120
 *
 * Descrição:
 * Este programa define funções auxiliares usadas pelo interpretador.
 * Tais funções ajudam na conversão de valores para tipos primitivos.
 */

#pragma once
#include "runtime.hpp"

namespace interp_detail {

inline long long toInt(const Value& v) {
  if (!v.is<long long>()) throw RuntimeError("Esperava Int");
  return v.as<long long>();
}

inline double toFloat(const Value& v) {
  if (v.is<double>()) return v.as<double>();
  if (v.is<long long>()) return (double)v.as<long long>();
  throw RuntimeError("Esperava Float/Int");
}

inline bool toBool(const Value& v) {
  if (!v.is<bool>()) throw RuntimeError("Esperava Bool");
  return v.as<bool>();
}

} 
