#include "checker/include/typechecker.hpp"
#include <cctype>
#include <sstream>
#include <algorithm>

//
// Gamma
//
void Gamma::push() { scopes.push_back({}); }
void Gamma::pop() { scopes.pop_back(); }

bool Gamma::existsHere(const std::string& x) const {
  if (scopes.empty()) return false;
  return scopes.back().count(x) != 0;
}

TypePtr Gamma::lookup(const std::string& x) const {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    auto f = it->find(x);
    if (f != it->end()) return f->second;
  }
  return nullptr;
}

void Gamma::declare(const std::string& x, TypePtr t) {
  scopes.back()[x] = std::move(t);
}

//
// Tokenizador simples para tipos
//
static std::string trim(std::string s) {
  auto notSpace = [](unsigned char c){ return !std::isspace(c); };
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
  s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
  return s;
}

static std::vector<std::string> splitTopLevel(const std::string& s, char sep) {
  std::vector<std::string> out;
  std::string cur;
  for (char c : s) {
    if (c == sep) {
      out.push_back(trim(cur));
      cur.clear();
    } else {
      cur.push_back(c);
    }
  }
  if (!cur.empty() || !out.empty()) out.push_back(trim(cur));
  return out;
}

static std::vector<std::string> splitArrow(const std::string& s) {
  std::vector<std::string> parts;
  std::string cur;
  for (size_t i = 0; i < s.size(); ) {
    if (i + 1 < s.size() && s[i] == '-' && s[i+1] == '>') {
      parts.push_back(trim(cur));
      cur.clear();
      i += 2;
    } else {
      cur.push_back(s[i++]);
    }
  }
  if (!cur.empty() || !parts.empty()) parts.push_back(trim(cur));
  return parts;
}

//
// Substituição de variável de tipo (a := concreto) para assinar instâncias/métodos
//
static TypePtr substTyVar(const TypePtr& t, const std::string& var, const TypePtr& concrete) {
  if (!t) return t;

  if (auto v = std::get_if<TyVar>(&t->v)) {
    if (v->name == var) return concrete;
    return t;
  }
  if (auto a = std::get_if<TyArray>(&t->v)) {
    return Type::Array(substTyVar(a->elem, var, concrete));
  }
  if (auto tup = std::get_if<TyTuple>(&t->v)) {
    std::vector<TypePtr> elems;
    elems.reserve(tup->elems.size());
    for (auto& e : tup->elems) elems.push_back(substTyVar(e, var, concrete));
    return Type::Tuple(std::move(elems));
  }
  return t; // primitivos/user/null
}

//
// TypeChecker core
//
[[noreturn]] void TypeChecker::err(const SrcPos& p, const std::string& msg) const {
  throw TypeError(p, msg);
}

void TypeChecker::expectAssignable(const SrcPos& p, const TypePtr& target, const TypePtr& value) {
  if (isNull(value)) {
    if (!nullAssignableTo(target)) {
      err(p, "null não é atribuível a " + typeToString(target));
    }
    return;
  }
  if (!typeEq(target, value)) {
    err(p, "Tipos incompatíveis: esperado " + typeToString(target) + ", obtido " + typeToString(value));
  }
}

//
// parseType + suporte a TyVar dentro de class
//
TypePtr TypeChecker::parseType(const std::string& s0) {
  std::string s = trim(s0);

  int dims = 0;
  while (s.size() >= 2 && s.substr(s.size()-2) == "[]") {
    dims++;
    s = trim(s.substr(0, s.size()-2));
  }

  TypePtr base;
  if (s == "Int") base = Type::Int();
  else if (s == "Float") base = Type::Float();
  else if (s == "Bool") base = Type::Bool();
  else if (s == "Char") base = Type::Char();
  else if (s == "Void") base = Type::Void();
  else if (s == "null") base = Type::Null();
  else {
    if (activeTyVar.has_value() && s == *activeTyVar) base = Type::Var(s);
    else base = Type::User(s);
  }

  for (int i = 0; i < dims; ++i) base = Type::Array(base);
  return base;
}

FunSig TypeChecker::parseFunSig(const std::vector<std::string>& params, const std::string& annot0) {
  std::string annot = trim(annot0);
  auto chain = splitArrow(annot);

  // sem seta: retorno(s) (ou Void)
  if (chain.size() == 1) {
    FunSig sig;
    sig.params.resize(params.size(), nullptr); // não usado aqui
    auto retsStr = splitTopLevel(chain[0], '&');
    for (auto& r : retsStr) sig.rets.push_back(parseType(r));
    return sig;
  }

  FunSig sig;

  // params do texto (a -> Int -> ...)
  size_t nParams = chain.size() - 1;
  for (size_t i = 0; i < nParams; ++i) sig.params.push_back(parseType(chain[i]));

  // retornos (último pedaço pode ter &)
  auto retsStr = splitTopLevel(chain.back(), '&');
  for (auto& r : retsStr) sig.rets.push_back(parseType(r));

  return sig;
}

void TypeChecker::addPrimitiveFunctions() {
  // Mantém para existir em Θ (mas o checker trata print como builtin com overload)
  theta["print"]  = FunSig{ {Type::Char()}, {Type::Void()} };

  theta["printb"] = FunSig{ {Type::Array(Type::Char()), Type::Int(), Type::Int()}, {Type::Void()} };
  theta["read"]   = FunSig{ {}, {Type::Char()} };
  theta["readb"]  = FunSig{ {Type::Array(Type::Char()), Type::Int(), Type::Int()}, {Type::Int()} };
}

void TypeChecker::buildDelta(const Program& prog) {
  for (auto& d : prog.decls) {
    auto dd = std::dynamic_pointer_cast<DataDecl>(d);
    if (!dd) continue;

    if (delta.count(dd->typeName)) {
      err(dd->pos, "Tipo já definido: " + dd->typeName);
    }

    std::unordered_map<std::string, TypePtr> fields;
    for (auto& [fname, fty] : dd->fields) {
      if (fields.count(fname)) {
        err(dd->pos, "Campo repetido em " + dd->typeName + ": " + fname);
      }
      fields[fname] = parseType(fty);
    }
    delta[dd->typeName] = std::move(fields);
  }
}

void TypeChecker::buildTheta(const Program& prog) {
  addPrimitiveFunctions();

  for (auto& d : prog.decls) {
    auto fd = std::dynamic_pointer_cast<FuncDecl>(d);
    if (!fd) continue;

    if (methodOwner.count(fd->name)) {
      err(fd->pos, "Função '" + fd->name + "' conflita com método de classe");
    }

    if (theta.count(fd->name)) {
      err(fd->pos, "Função já definida: " + fd->name);
    }

    FunSig sig = parseFunSig(fd->params, fd->typeAnnotText);

    if (!fd->params.empty() && sig.params.size() != fd->params.size()) {
      err(fd->pos, "Assinatura não bate com quantidade de parâmetros em " + fd->name);
    }

    theta[fd->name] = std::move(sig);
  }
}

//
// buildClasses: registra classes e assinaturas de métodos (com TyVar)
//
void TypeChecker::buildClasses(const Program& prog) {
  for (auto& d : prog.decls) {
    auto cd = std::dynamic_pointer_cast<ClassDecl>(d);
    if (!cd) continue;

    if (classes.count(cd->className))
      err(cd->pos, "Classe já definida: " + cd->className);

    ClassInfo ci;
    ci.tyVar = cd->tyVar;

    // ativa var de tipo (pra parseType reconhecer "a")
    activeTyVar = cd->tyVar;

    for (auto& [mname, annot] : cd->methods) {
      if (ci.methods.count(mname))
        err(cd->pos, "Método repetido na classe " + cd->className + ": " + mname);

      FunSig sig = parseFunSig({}, annot);
      ci.methods[mname] = sig;

      // método deve ser único no programa (senão não dá pra desambiguar chamada)
      if (methodOwner.count(mname))
        err(cd->pos, "Método '" + mname + "' aparece em mais de uma classe (ambíguo)");

      // também não pode conflitar com função normal
      if (theta.count(mname))
        err(cd->pos, "Nome '" + mname + "' já usado como função; conflita com método de classe");

      methodOwner[mname] = cd->className;
    }

    activeTyVar.reset();
    classes[cd->className] = std::move(ci);
  }
}

//
// buildInstances: registra instâncias no instSet e checa assinaturas
//
void TypeChecker::buildInstances(const Program& prog) {
  for (auto& d : prog.decls) {
    auto id = std::dynamic_pointer_cast<InstanceDecl>(d);
    if (!id) continue;

    if (!classes.count(id->className))
      err(id->pos, "Instância de classe não declarada: " + id->className);

    auto key = std::make_pair(id->className, id->forType);
    if (instSet.count(key))
      err(id->pos, "Instância duplicada: " + id->className + " for " + id->forType);

    instSet.insert(key);

    const ClassInfo& ci = classes.at(id->className);
    TypePtr concrete = parseType(id->forType);

    // map nome -> FuncDecl dentro da instância
    std::unordered_map<std::string, std::shared_ptr<FuncDecl>> impl;
    for (auto& md : id->methods) {
      auto fd = std::dynamic_pointer_cast<FuncDecl>(md);
      if (!fd) continue;
      impl[fd->name] = fd;
    }

    // checar se implementa TODOS os métodos e se assinatura bate após substituição
    for (auto& [mname, classSig] : ci.methods) {
      if (!impl.count(mname))
        err(id->pos, "Instância não implementa método: " + mname);

      auto fd = impl[mname];
      FunSig implSig = parseFunSig(fd->params, fd->typeAnnotText);

      FunSig expected;
      for (auto& p : classSig.params) expected.params.push_back(substTyVar(p, ci.tyVar, concrete));
      for (auto& r : classSig.rets)   expected.rets.push_back(substTyVar(r, ci.tyVar, concrete));

      if (implSig.params.size() != expected.params.size() ||
          implSig.rets.size()   != expected.rets.size()) {
        err(id->pos, "Assinatura não bate no método " + mname + " da instância " +
                     id->className + " for " + id->forType);
      }

      for (size_t i = 0; i < expected.params.size(); ++i) {
        if (!typeEq(implSig.params[i], expected.params[i])) {
          err(id->pos, "Parâmetro " + std::to_string(i) + " de " + mname +
                       " não bate (esperado " + typeToString(expected.params[i]) +
                       ", obtido " + typeToString(implSig.params[i]) + ")");
        }
      }

      for (size_t i = 0; i < expected.rets.size(); ++i) {
        if (!typeEq(implSig.rets[i], expected.rets[i])) {
          err(id->pos, "Retorno " + std::to_string(i) + " de " + mname +
                       " não bate (esperado " + typeToString(expected.rets[i]) +
                       ", obtido " + typeToString(implSig.rets[i]) + ")");
        }
      }
    }
  }
}

void TypeChecker::requireMainVoid(const Program& prog) {
  auto it = theta.find("main");
  if (it == theta.end()) {
    SrcPos p = prog.decls.empty() ? SrcPos{1,1} : prog.decls.front()->pos;
    err(p, "Programa sem função main");
  }

  auto& sig = it->second;

  SrcPos mainPos = SrcPos{1,1};
  for (auto& d : prog.decls) {
    if (auto fd = std::dynamic_pointer_cast<FuncDecl>(d)) {
      if (fd->name == "main") { mainPos = fd->pos; break; }
    }
  }

  if (!sig.params.empty()) err(mainPos, "main não deve receber parâmetros");
  if (sig.rets.size() != 1 || !std::holds_alternative<TyVoid>(sig.rets[0]->v)) {
    err(mainPos, "main deve ter retorno Void");
  }
}

void TypeChecker::checkProgram(const Program& prog) {
  gamma.scopes.clear();
  gamma.push();

  buildDelta(prog);
  buildClasses(prog);
  buildTheta(prog);
  buildInstances(prog);

  requireMainVoid(prog);

  // checar corpos das funções normais
  for (auto& d : prog.decls) {
    if (auto fd = std::dynamic_pointer_cast<FuncDecl>(d)) {
      fd->accept(*this);
    }
  }

  gamma.pop();
}

//
// Helpers de tipagem
//
TypePtr TypeChecker::typeOf(const ExprPtr& e) {
  if (!e) return Type::Void();
  e->accept(*this);
  return resultType;
}

TypePtr TypeChecker::typeOf(const LValPtr& lv) {
  if (!lv) return Type::Void();
  lv->accept(*this);
  return resultType;
}

//
// Visitor: Decls
//
void TypeChecker::visit(DataDecl&) {
  // nada a fazer no passe 2
}

void TypeChecker::visit(FuncDecl& f) {
  auto it = theta.find(f.name);
  if (it == theta.end()) err(f.body ? f.body->pos : SrcPos{}, "Função sem assinatura em Θ: " + f.name);

  gamma.push();

  auto& sig = it->second;
  for (size_t i = 0; i < f.params.size(); ++i) {
    gamma.declare(f.params[i], sig.params[i]);
  }

  currentReturns = sig.rets;

  if (f.body) f.body->accept(*this);

  gamma.pop();
}

void TypeChecker::visit(ClassDecl&) {
  // já processado em buildClasses
}

void TypeChecker::visit(InstanceDecl&) {
  // já processado em buildInstances
}

//
// Visitor: LValues
//
void TypeChecker::visit(LVar& v) {
  auto t = gamma.lookup(v.name);
  if (!t) err(v.pos, "Variável não declarada: " + v.name);
  resultType = t;
}

void TypeChecker::visit(LField& f) {
  TypePtr bt = typeOf(f.base);
  if (isNull(bt)) err(f.pos, "Acesso de campo em null");

  if (!isUser(bt)) err(f.pos, "Acesso de campo requer registro, obtido " + typeToString(bt));
  auto uname = std::get<TyUser>(bt->v).name;

  auto dit = delta.find(uname);
  if (dit == delta.end()) err(f.pos, "Tipo de registro não definido: " + uname);

  auto fit = dit->second.find(f.field);
  if (fit == dit->second.end()) err(f.pos, "Campo inexistente: " + uname + "." + f.field);

  resultType = fit->second;
}

void TypeChecker::visit(LIndex& idx) {
  TypePtr bt = typeOf(idx.base);
  if (isNull(bt)) err(idx.pos, "Indexação em null");

  if (!isArray(bt)) err(idx.pos, "Indexação requer array, obtido " + typeToString(bt));

  TypePtr it = typeOf(idx.index);
  if (!std::holds_alternative<TyInt>(it->v)) {
    err(idx.pos, "Índice de array deve ser Int, obtido " + typeToString(it));
  }

  resultType = std::get<TyArray>(bt->v).elem;
}

//
// Visitor: Exprs
//
void TypeChecker::visit(EInt&)   { resultType = Type::Int(); }
void TypeChecker::visit(EFloat&) { resultType = Type::Float(); }
void TypeChecker::visit(EChar&)  { resultType = Type::Char(); }
void TypeChecker::visit(EBool&)  { resultType = Type::Bool(); }
void TypeChecker::visit(ENull&)  { resultType = Type::Null(); }

void TypeChecker::visit(EVar& v) {
  auto t = gamma.lookup(v.name);
  if (!t) err(v.pos, "Variável não declarada: " + v.name);
  resultType = t;
}

void TypeChecker::visit(ELValue& x) {
  resultType = typeOf(x.lv);
}

void TypeChecker::visit(EUnary& u) {
  TypePtr t = typeOf(u.e);

  if (u.op == EUnary::Not) {
    if (!std::holds_alternative<TyBool>(t->v)) err(u.pos, "Operador ! exige Bool");
    resultType = Type::Bool();
    return;
  }
  if (u.op == EUnary::Neg) {
    if (std::holds_alternative<TyInt>(t->v))   { resultType = Type::Int(); return; }
    if (std::holds_alternative<TyFloat>(t->v)) { resultType = Type::Float(); return; }
    err(u.pos, "Menos unário exige Int ou Float");
  }

  err(u.pos, "Operador unário desconhecido");
}

void TypeChecker::visit(EBinary& b) {
  TypePtr L = typeOf(b.l);
  TypePtr R = typeOf(b.r);

  auto isInt   = [](const TypePtr& t){ return std::holds_alternative<TyInt>(t->v); };
  auto isFloat = [](const TypePtr& t){ return std::holds_alternative<TyFloat>(t->v); };
  auto isChar  = [](const TypePtr& t){ return std::holds_alternative<TyChar>(t->v); };
  auto isBool  = [](const TypePtr& t){ return std::holds_alternative<TyBool>(t->v); };

  auto isArray = [&](const TypePtr& t){ return std::holds_alternative<TyArray>(t->v); };
  auto isUser  = [&](const TypePtr& t){ return std::holds_alternative<TyUser>(t->v); };
  auto isNull  = [&](const TypePtr& t){ return std::holds_alternative<TyNull>(t->v); };

  switch (b.op) {
    case EBinary::AndAnd:
      if (!isBool(L) || !isBool(R)) err(b.pos, "&& exige Bool && Bool");
      resultType = Type::Bool();
      return;

    case EBinary::Add:
    case EBinary::Sub:
    case EBinary::Mul:
    case EBinary::Div:
      if (isInt(L) && isInt(R))     { resultType = Type::Int(); return; }
      if (isFloat(L) && isFloat(R)) { resultType = Type::Float(); return; }
      err(b.pos, "Operação aritmética exige operandos homogêneos (Int/Int ou Float/Float)");

    case EBinary::Mod:
      if (isInt(L) && isInt(R)) { resultType = Type::Int(); return; }
      err(b.pos, "% exige Int % Int");

    // ======= igualdade / diferença =======
    case EBinary::Eq:
    case EBinary::Ne: {
      // permitir null com tipos referência (Array e User/registro)
      if (isNull(L) && (isArray(R) || isUser(R))) { resultType = Type::Bool(); return; }
      if (isNull(R) && (isArray(L) || isUser(L))) { resultType = Type::Bool(); return; }
      if (isNull(L) && isNull(R))                 { resultType = Type::Bool(); return; }

      const bool homoOK =
        (isInt(L) && isInt(R)) ||
        (isFloat(L) && isFloat(R)) ||
        (isChar(L) && isChar(R));

      const bool charIntOK =
        (isChar(L) && isInt(R)) ||
        (isInt(L) && isChar(R));

      if (homoOK || charIntOK) { resultType = Type::Bool(); return; }
      err(b.pos, "Comparação exige operandos compatíveis");
    }

    // ======= relacionais (< <= > >=) =======
    case EBinary::Lt:
    case EBinary::Le:
    case EBinary::Gt:
    case EBinary::Ge: {
      const bool homoOK =
        (isInt(L) && isInt(R)) ||
        (isFloat(L) && isFloat(R)) ||
        (isChar(L) && isChar(R));

      const bool charIntOK =
        (isChar(L) && isInt(R)) ||
        (isInt(L) && isChar(R));

      if (homoOK || charIntOK) { resultType = Type::Bool(); return; }
      err(b.pos, "Comparação exige operandos compatíveis");
    }

    default:
      err(b.pos, "Operador binário não suportado no checker");
  }
}

void TypeChecker::visit(ENew& n) {
  TypePtr base = parseType(n.typeName);

  if (n.size.has_value()) {
    TypePtr sz = typeOf(*n.size);
    if (!std::holds_alternative<TyInt>(sz->v)) err(n.pos, "Tamanho de new deve ser Int");
    resultType = Type::Array(base);
    return;
  }

  if (!isUser(base)) err(n.pos, "new sem tamanho só é permitido para tipo registro (TYID)");
  auto uname = std::get<TyUser>(base->v).name;
  if (!delta.count(uname)) err(n.pos, "Tipo de registro não definido: " + uname);

  resultType = base;
}

//
// ECall
//
void TypeChecker::visit(ECall& c) {
  // print aceita Char/Int/Float/Bool
  if (c.name == "print") {
    if (c.args.size() != 1) err(c.pos, "Aridade incorreta em print");

    TypePtr at = typeOf(c.args[0]);

    const bool ok =
      std::holds_alternative<TyChar>(at->v)  ||
      std::holds_alternative<TyInt>(at->v)   ||
      std::holds_alternative<TyFloat>(at->v) ||
      std::holds_alternative<TyBool>(at->v);

    if (!ok) err(c.pos, "print aceita Char, Int, Float ou Bool (obtido " + typeToString(at) + ")");

    resultType = Type::Void();
    return;
  }

  auto mo = methodOwner.find(c.name);
  if (mo != methodOwner.end()) {
    const std::string& cls = mo->second;
    const ClassInfo& ci = classes.at(cls);
    const FunSig& msig = ci.methods.at(c.name);

    if (c.args.size() != msig.params.size())
      err(c.pos, "Aridade incorreta em " + c.name);

    if (c.args.empty())
      err(c.pos, "Método de classe precisa de argumentos para inferir tipo");

    TypePtr concrete = typeOf(c.args[0]);
    std::string concreteName = typeToString(concrete);

    if (!instSet.count({cls, concreteName}))
      err(c.pos, "Não existe instância: " + cls + " for " + concreteName);

    for (size_t i = 0; i < c.args.size(); ++i) {
      TypePtr ai = typeOf(c.args[i]);
      TypePtr expectedPi = substTyVar(msig.params[i], ci.tyVar, concrete);
      expectAssignable(c.pos, expectedPi, ai);
    }

    if (msig.rets.empty()) err(c.pos, "Método sem retornos: " + c.name);

    std::vector<TypePtr> retsSub;
    retsSub.reserve(msig.rets.size());
    for (auto& r : msig.rets) retsSub.push_back(substTyVar(r, ci.tyVar, concrete));

    if (!c.retIndex.has_value()) {
      resultType = retsSub[0];
      return;
    }

    TypePtr idxT = typeOf(*c.retIndex);
    if (!std::holds_alternative<TyInt>(idxT->v)) err(c.pos, "Índice de retorno deve ser Int");

    if (auto lit = std::dynamic_pointer_cast<EInt>(*c.retIndex)) {
      long long k = lit->v;
      if (k < 0 || (size_t)k >= retsSub.size()) err(c.pos, "Índice de retorno fora do range");
      resultType = retsSub[(size_t)k];
      return;
    }

    resultType = retsSub[0];
    return;
  }

  auto it = theta.find(c.name);
  if (it == theta.end()) err(c.pos, "Função não declarada: " + c.name);

  auto& sig = it->second;
  if (c.args.size() != sig.params.size()) {
    err(c.pos, "Aridade incorreta em " + c.name);
  }

  for (size_t i = 0; i < c.args.size(); ++i) {
    TypePtr ai = typeOf(c.args[i]);
    expectAssignable(c.pos, sig.params[i], ai);
  }

  if (!c.retIndex.has_value()) {
    if (sig.rets.empty()) err(c.pos, "Função sem retornos: " + c.name);
    resultType = sig.rets[0];
    return;
  }

  TypePtr idxT = typeOf(*c.retIndex);
  if (!std::holds_alternative<TyInt>(idxT->v)) err(c.pos, "Índice de retorno deve ser Int");

  if (auto lit = std::dynamic_pointer_cast<EInt>(*c.retIndex)) {
    long long k = lit->v;
    if (k < 0 || (size_t)k >= sig.rets.size()) err(c.pos, "Índice de retorno fora do range");
    resultType = sig.rets[(size_t)k];
    return;
  }

  resultType = sig.rets[0];
}


// Visitor: Cmds
void TypeChecker::visit(CBlock& b) {
  gamma.push();
  for (auto& c : b.cs) if (c) c->accept(*this);
  gamma.pop();
}

void TypeChecker::visit(CAssign& a) {
  if (auto lv = std::dynamic_pointer_cast<LVar>(a.lhs)) {
    TypePtr rhsT = typeOf(a.rhs);

    if (isNull(rhsT)) err(a.pos, "Não é permitido declarar variável com null sem tipo");

    auto existing = gamma.lookup(lv->name);
    if (!existing) {
      if (!gamma.scopes.empty() && gamma.existsHere(lv->name)) {
        err(a.pos, "Variável redeclarada no mesmo escopo: " + lv->name);
      }
      gamma.declare(lv->name, rhsT);
      return;
    }

    expectAssignable(a.pos, existing, rhsT);
    return;
  }

  TypePtr lhsT = typeOf(a.lhs);
  TypePtr rhsT = typeOf(a.rhs);
  expectAssignable(a.pos, lhsT, rhsT);
}

void TypeChecker::visit(CIf& c) {
  TypePtr ct = typeOf(c.cond);
  if (!std::holds_alternative<TyBool>(ct->v)) err(c.pos, "if exige condição Bool");

  if (c.thenC) { gamma.push(); c.thenC->accept(*this); gamma.pop(); }
  if (c.elseC.has_value()) { gamma.push(); (*c.elseC)->accept(*this); gamma.pop(); }
}

void TypeChecker::visit(CIterate& it) {
  TypePtr et = typeOf(it.expr);

  bool intCase = std::holds_alternative<TyInt>(et->v);
  bool arrCase = std::holds_alternative<TyArray>(et->v);

  if (!intCase && !arrCase) err(it.pos, "iterate exige Int ou Array");

  gamma.push();

  if (it.itVar.has_value()) {
    const std::string& v = *it.itVar;

    if (!gamma.lookup(v)) {
      if (intCase) gamma.declare(v, Type::Int());
      else gamma.declare(v, std::get<TyArray>(et->v).elem);
    } else {
      TypePtr expected = intCase ? Type::Int() : std::get<TyArray>(et->v).elem;
      expectAssignable(it.pos, gamma.lookup(v), expected);
    }
  }

  if (it.body) it.body->accept(*this);
  gamma.pop();
}

void TypeChecker::visit(CReturn& r) {

   if (r.exps.empty()) {
    if (currentReturns.size() == 1 &&
        std::holds_alternative<TyVoid>(currentReturns[0]->v)) {
      return; // ok
    }
    err(r.pos, "return com quantidade incorreta de valores");
  }

  // caso normal: número de expressões precisa bater com a aridade do retorno
  if (r.exps.size() != currentReturns.size()) {
    err(r.pos, "return com quantidade incorreta de valores");
  }

  for (size_t i = 0; i < r.exps.size(); ++i) {
    TypePtr ti = typeOf(r.exps[i]);
    expectAssignable(r.pos, currentReturns[i], ti);
  }
}

// CCallStmt
void TypeChecker::visit(CCallStmt& cs) {
  // print aceita Char/Int/Float/Bool
  if (cs.name == "print") {
    if (cs.args.size() != 1) err(cs.pos, "Aridade incorreta em print");
    if (!cs.rets.empty()) err(cs.pos, "print não retorna valores em <...>");

    TypePtr at = typeOf(cs.args[0]);

    const bool ok =
      std::holds_alternative<TyChar>(at->v)  ||
      std::holds_alternative<TyInt>(at->v)   ||
      std::holds_alternative<TyFloat>(at->v) ||
      std::holds_alternative<TyBool>(at->v);

    if (!ok) err(cs.pos, "print aceita Char, Int, Float ou Bool (obtido " + typeToString(at) + ")");
    return;
  }

  auto mo = methodOwner.find(cs.name);
  if (mo != methodOwner.end()) {
    const std::string& cls = mo->second;
    const ClassInfo& ci = classes.at(cls);
    const FunSig& msig = ci.methods.at(cs.name);

    if (cs.args.size() != msig.params.size())
      err(cs.pos, "Aridade incorreta em " + cs.name);

    if (cs.args.empty())
      err(cs.pos, "Método de classe precisa de argumentos para inferir tipo");

    TypePtr concrete = typeOf(cs.args[0]);
    std::string concreteName = typeToString(concrete);

    if (!instSet.count({cls, concreteName}))
      err(cs.pos, "Não existe instância: " + cls + " for " + concreteName);

    for (size_t i = 0; i < cs.args.size(); ++i) {
      TypePtr ai = typeOf(cs.args[i]);
      TypePtr expectedPi = substTyVar(msig.params[i], ci.tyVar, concrete);
      expectAssignable(cs.pos, expectedPi, ai);
    }

    // retornos substituídos
    std::vector<TypePtr> retsSub;
    retsSub.reserve(msig.rets.size());
    for (auto& r : msig.rets) retsSub.push_back(substTyVar(r, ci.tyVar, concrete));

    if (!cs.rets.empty()) {
      if (cs.rets.size() != retsSub.size())
        err(cs.pos, "Quantidade de lvalues em <...> não bate com retornos de " + cs.name);

      for (size_t i = 0; i < cs.rets.size(); ++i) {
        // para <..>
        if (auto lv = std::dynamic_pointer_cast<LVar>(cs.rets[i])) {
          TypePtr existing = gamma.lookup(lv->name);
          if (!existing) {
            gamma.declare(lv->name, retsSub[i]);
          } else {
            expectAssignable(cs.pos, existing, retsSub[i]);
          }
          continue;
        }

        // outros lvalues precisam existir
        TypePtr lt = typeOf(cs.rets[i]);
        expectAssignable(cs.pos, lt, retsSub[i]);
      }
    }

    return;
  }

  // função normal
  auto it = theta.find(cs.name);
  if (it == theta.end()) err(cs.pos, "Função não declarada: " + cs.name);

  auto& sig = it->second;

  if (cs.args.size() != sig.params.size()) err(cs.pos, "Aridade incorreta em " + cs.name);
  for (size_t i = 0; i < cs.args.size(); ++i) {
    TypePtr ai = typeOf(cs.args[i]);
    expectAssignable(cs.pos, sig.params[i], ai);
  }

  if (!cs.rets.empty()) {
    if (cs.rets.size() != sig.rets.size()) {
      err(cs.pos, "Quantidade de lvalues em <...> não bate com retornos de " + cs.name);
    }

    for (size_t i = 0; i < cs.rets.size(); ++i) {
      // ✅ declarar variável nova em <...>
      if (auto lv = std::dynamic_pointer_cast<LVar>(cs.rets[i])) {
        TypePtr existing = gamma.lookup(lv->name);
        if (!existing) {
          gamma.declare(lv->name, sig.rets[i]);
        } else {
          expectAssignable(cs.pos, existing, sig.rets[i]);
        }
        continue;
      }

      // outros lvalues precisam existir
      TypePtr lt = typeOf(cs.rets[i]);
      expectAssignable(cs.pos, lt, sig.rets[i]);
    }
  }
}