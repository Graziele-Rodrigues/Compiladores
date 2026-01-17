#include "include/interpreter.hpp"

std::vector<Value> Interpreter::callFunction(Env& caller, const std::string& name,
                                            const std::vector<Value>& args) {
  // primitivas
  if (name == "print") {
    if (args.size() != 1) throw RuntimeError("print espera 1 argumento");
    prim_print(args[0]);
    return {};
  }

  auto it = funcs.find(name);
  if (it == funcs.end()) throw RuntimeError("Funcao nao declarada: " + name);
  FuncDecl* f = it->second;

  if (args.size() != f->params.size()) throw RuntimeError("Aridade incorreta em " + name);

  Env callee;
  for (size_t i = 0; i < args.size(); ++i) {
    callee.set(f->params[i], args[i]);
  }

  try {
    execCmd(callee, f->body);
    return {}; // sem return
  } catch (const ReturnSignal& rs) {
    return rs.values;
  }
}
