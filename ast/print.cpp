/* Arquivo: print.cpp
 * Autor: Graziele Cassia Rodrigues
 * Matricula: 21.1.8120
 *
 * Descrição:
 * Implementação das funções primitivas impressao.
 */

#include "include/interpreter.hpp"
#include <iostream>

// ---------- conversões auxiliares ----------
static long long toInt(const Value& v) {
  if (!v.is<long long>()) throw RuntimeError("Esperava Int");
  return v.as<long long>();
}

static char toChar(const Value& v) {
  if (!v.is<char>()) throw RuntimeError("Esperava Char");
  return v.as<char>();
}

static Array& asCharArrayRef(Heap& heap, Value& buff) {
  // Addr
  if (buff.is<Addr>()) {
    Addr a = buff.as<Addr>();
    Value& cell = heap.atAddr(a.a);
    if (!isArray(cell)) throw RuntimeError("Esperava Char[] (array no heap)");
    return *asArray(cell);
  }

  //Array direto
  if (isArray(buff)) {
    return *asArray(buff);
  }

  throw RuntimeError("Esperava Char[]");
}

// ---------- print :: Value -> Void ----------
void Interpreter::prim_print(const Value& v) {
  if (v.is<char>()) std::cout << v.as<char>();
  else if (v.is<long long>()) std::cout << v.as<long long>();
  else if (v.is<double>()) std::cout << v.as<double>();
  else if (v.is<bool>()) std::cout << (v.as<bool>() ? "true" : "false");
  else if (v.is<Null>()) std::cout << "null";
  else if (v.is<Addr>()) std::cout << v.as<Addr>().a;
  else if (isArray(v)) std::cout << "<array>";
  else if (isRecord(v)) std::cout << "<record>";
  else std::cout << "<value>";
}

// ---------- printb :: Char[] -> Int -> Int -> Void ----------
void Interpreter::prim_printb(const Value& buff, const Value& start, const Value& len) {
  long long i = toInt(start);
  long long l = toInt(len);
  if (l <= 0) return;

  Value tmp = buff; 
  Array& arr = asCharArrayRef(heap, tmp);

  if (i < 0) i = 0;
  if (i >= (long long)arr.size()) return;

  long long end = std::min(i + l, (long long)arr.size());
  for (long long k = i; k < end; ++k) {
    auto& vp = arr[(size_t)k];
    if (!vp) throw RuntimeError("printb: elemento nulo no array");
    std::cout << toChar(*vp);
  }
}