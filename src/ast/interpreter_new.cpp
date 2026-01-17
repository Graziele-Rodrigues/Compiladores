#include "include/interpreter.hpp"
#include "include/interpreter_helpers.hpp"

using interp_detail::toInt;

Value Interpreter::newValue(const std::string& typeName, std::optional<long long> sizeOpt) {
  auto defaultPrim = [&](const std::string& t)->Value {
    if (t == "Int")   return Value{ (long long)0 };
    if (t == "Float") return Value{ (double)0.0 };
    if (t == "Bool")  return Value{ false };
    if (t == "Char")  return Value{ (char)0 };
    return Value::makeNull(); // user types => null
  };

  const bool isUserType = (dataDefs.find(typeName) != dataDefs.end());

  std::string a = heap.freshAddr();
  Value allocated;

  if (sizeOpt.has_value()) {
    long long n = *sizeOpt;
    if (n < 0) throw RuntimeError("new com tamanho negativo");

    allocated = Value::makeArray();
    auto arrPtr = asArray(allocated); // shared_ptr<Array>&
    arrPtr->reserve((size_t)n);

    if (isUserType) {
      for (long long i = 0; i < n; ++i) {
        arrPtr->push_back(std::make_shared<Value>(Value::makeNull()));
      }
    } else {
      for (long long i = 0; i < n; ++i) {
        arrPtr->push_back(std::make_shared<Value>(defaultPrim(typeName)));
      }
    }
  } else {
    // sem tamanho -> registro TYID
    if (!isUserType) throw RuntimeError("new sem tamanho so pode para TYID");

    allocated = Value::makeRecord();
    auto recPtr = asRecord(allocated);

    DataDecl* dd = dataDefs[typeName];
    for (auto& [field, ftype] : dd->fields) {
      if (ftype == "Int" || ftype == "Float" || ftype == "Bool" || ftype == "Char") {
        (*recPtr)[field] = std::make_shared<Value>(defaultPrim(ftype));
      } else {
        (*recPtr)[field] = std::make_shared<Value>(Value::makeNull());
      }
    }
  }

  heap.mem[a] = std::move(allocated);
  return Value::makeAddr(a);
}
