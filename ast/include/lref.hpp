
/*
 * Arquivo: lref.hpp
 * Autor: Graziele Cassia Rodrigues
 * Matricula: 21.1.8120
 *
 * Descrição:
 * Este programa define a estrutura LRef para referências a valores
 * e uma função auxiliar para desreferenciar valores do heap.
 * 
 * Lref é uma estrutura que representa uma referência a um valor
 * em memória. Ela contém um ponteiro para um Value, permitindo
 * operações de leitura e escrita indiretas.
 */

#pragma once
#include "runtime.hpp"
#include "ast.hpp"

struct LRef {
  Value* slot = nullptr;
};

inline Value& derefHeapIfAddr(Heap& H, Value& v){
  if (v.is<Addr>()) return H.atAddr(v.as<Addr>().a);
  return v;
}
