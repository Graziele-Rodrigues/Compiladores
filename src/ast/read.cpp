/* Arquivo: read.cpp
 * Autor: Graziele Cassia Rodrigues
 * Matricula: 21.1.8120
 *
 * Descrição:
 * Implementação das funções primitivas leitura.
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

// ---------- read :: Char ----------
Value Interpreter::prim_read() {
  char c = '\0';
  if (!std::cin.get(c)) c = '\0';
  return Value{c};
}

// ---------- readb :: Char[] -> Int -> Int -> Int ----------
Value Interpreter::prim_readb(const Value& buff, const Value& start, const Value& len) {
  long long i = toInt(start);
  long long l = toInt(len);
  if (l <= 0) return Value{0LL};

  Value tmp = buff;
  Array& arr = asCharArrayRef(heap, tmp);

  if (i < 0) i = 0;
  if (i >= (long long)arr.size()) return Value{0LL};

  long long maxWrite = std::min(l, (long long)arr.size() - i);
  long long count = 0;

  for (; count < maxWrite; ++count) {
    char c;
    if (!std::cin.get(c)) break;

    size_t pos = (size_t)(i + count);
    if (!arr[pos]) arr[pos] = std::make_shared<Value>(Value{'\0'});
    *arr[pos] = Value{c};
  }

  return Value{count};
}