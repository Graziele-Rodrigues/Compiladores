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
  Heap heap;

  std::unordered_map<std::string, DataDecl*> dataDefs;
  std::unordered_map<std::string, FuncDecl*> funcs;

  void prim_print(const Value& v);

  void loadProgram(const Program& p);
  void runMain();

  Value evalExpr(Env& env, const ExprPtr& e);
  LRef  evalLValueRef(Env& env, const LValPtr& lv);

  void execCmd(Env& env, const CmdPtr& c);

  std::vector<Value> callFunction(Env& caller, const std::string& name, const std::vector<Value>& args);
  Value newValue(const std::string& typeName, std::optional<long long> sizeOpt);
};
