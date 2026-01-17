#include "include/interpreter.hpp"

void Interpreter::loadProgram(const Program& p) {
  dataDefs.clear();
  funcs.clear();

  for (auto& d : p.decls) {
    if (auto dd = dynamic_cast<DataDecl*>(d.get())) {
      dataDefs[dd->typeName] = dd;
    } else if (auto fd = dynamic_cast<FuncDecl*>(d.get())) {
      funcs[fd->name] = fd;
    } else {
      // class/instance ignorados por enquanto
    }
  }
}

void Interpreter::runMain() {
  Env env;
  auto it = funcs.find("main");
  if (it == funcs.end()) throw RuntimeError("Nao existe funcao main");
  if (!it->second->params.empty()) throw RuntimeError("main nao deve ter parametros");

  (void)callFunction(env, "main", {});
}
