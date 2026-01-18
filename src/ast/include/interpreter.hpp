/* Arquivo: interpreter.hpp
 * Autor: Graziele Cassia Rodrigues
 * Matricula: 21.1.8120
 *
 * Descrição:
 * Este programa define a estrutura Interpreter para executar o programa.
 */

#pragma once
#include "ast.hpp"
#include "runtime.hpp"
#include "lref.hpp"   

#include <vector>
#include <unordered_map>

struct ReturnSignal {
  std::vector<Value> values;
};

struct Interpreter {
  Heap heap;  // heap global

  std::unordered_map<std::string, DataDecl*> dataDefs; // definições de tipos de dados
  std::unordered_map<std::string, FuncDecl*> funcs; // definições de funções

  void prim_print(const Value& v);  // função primitiva de impressão

  void loadProgram(const Program& p); // carrega definições do programa
  void runMain(); // executa a função main

  Value evalExpr(Env& env, const ExprPtr& e); // avalia expressão
  LRef  evalLValueRef(Env& env, const LValPtr& lv); // avalia referência de lvalue

  void execCmd(Env& env, const CmdPtr& c); // executa comando

  std::vector<Value> callFunction(Env& caller, const std::string& name, const std::vector<Value>& args); // chama função
  Value newValue(const std::string& typeName, std::optional<long long> sizeOpt); // cria valor do tipo especificado
};