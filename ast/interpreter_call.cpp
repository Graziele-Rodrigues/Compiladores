/* Arquivo: interpreter_call.cpp
 * Autor: Graziele Cassia Rodrigues
 * Matricula: 21.1.8120
 *
 * Descrição:
 * Este programa implementa a chamada de funções no interpretador.
 */

#include "include/interpreter.hpp"

std::vector<Value> Interpreter::callFunction(Env& caller, const std::string& name,
                                            const std::vector<Value>& args) {
  // primitivas
  if (name == "print") {
    if (args.size() != 1) throw RuntimeError("print espera 1 argumento");
    prim_print(args[0]);
    return {};
  }
  
  if (name == "printb") {
    if (args.size() != 3) throw RuntimeError("printb espera 3 argumentos");
    prim_printb(args[0], args[1], args[2]);
    return {};
  }

  if (name == "read") {
    if (args.size() != 0) throw RuntimeError("read espera 0 argumentos");
    Value c = prim_read();
    return { c };
  }

  if (name == "readb") {
    if (args.size() != 3) throw RuntimeError("readb espera 3 argumentos");
    Value n = prim_readb(args[0], args[1], args[2]);
    return { n };
  }

  // funções definidas pelo usuário
  auto it = funcs.find(name);
  if (it == funcs.end()) throw RuntimeError("Funcao nao declarada: " + name);
  FuncDecl* f = it->second;

  // checa aridade que dize quantos argumentos a função espera
  if (args.size() != f->params.size()) throw RuntimeError("Aridade incorreta em " + name);

  // cria ambiente da função e atribui argumentos aos parâmetros -  bind
  Env callee;
  for (size_t i = 0; i < args.size(); ++i) {
    callee.set(f->params[i], args[i]);
  }

  // tenta executar o corpo da função
  try {
    execCmd(callee, f->body);
    return {}; // sem return
  } catch (const ReturnSignal& rs) { 
    return rs.values; 
  }
}
