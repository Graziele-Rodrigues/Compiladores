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
