/* Arquivo: interpreter_program.cpp
 * Autor: Graziele Cassia Rodrigues
 * Matricula: 21.1.8120
 *
 * Descrição:
 * Este programa implementa o carregamento e execução do programa principal no interpretador.
 */

#include "include/interpreter.hpp"

// Percorre as declarações do programa e armazena definições de tipos de dados e funções
void Interpreter::loadProgram(const Program& p) {
  dataDefs.clear();
  funcs.clear();

  for (auto& d : p.decls) {
    if (auto dd = dynamic_cast<DataDecl*>(d.get())) { // definição de tipo de dado
      dataDefs[dd->typeName] = dd;
    } else if (auto fd = dynamic_cast<FuncDecl*>(d.get())) { // definição de função
      funcs[fd->name] = fd;
    } else { 
      // class/instance ignorados por enquanto
    }
  }
}

// chamada da função main do programa
void Interpreter::runMain() {
  Env env;
  auto it = funcs.find("main");
  if (it == funcs.end()) throw RuntimeError("Nao existe funcao main");
  if (!it->second->params.empty()) throw RuntimeError("main nao deve ter parametros");

  (void)callFunction(env, "main", {});
}
