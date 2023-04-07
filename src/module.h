#pragma once

class File;
class Symbol;
class Scope;
class Module;
class Declaration;
class Function;
class Variable;
class StringExp;
class IntegerExp;
class AsmWriter;
class AsmState;

#include "lexer.h"

enum ASM_REG {
    ASM_REG_INVALID,
    ASM_REG_NONE,
    ASM_REG_RAX,
    ASM_REG_RCX,
    ASM_REG_RDX,
    ASM_REG_R8,
    ASM_REG_R9,
    ASM_REG_MAX,
};

static std::string AsmRegString[] = {"invalid", "none", "rax", "rcx", "rdx", "r8", "r9"};

class AsmLabel {
public:
    std::string name;
    AsmLabel(const std::string& name) : name(name) {
    }
};

class AsmBlock {
public:
    AsmLabel* blockLabel;
    AsmBlock() {
    }
};

class AsmState {
public:
    bool freeRegisters[ASM_REG_MAX];
    AsmLabel* blockExit;
    std::vector<AsmLabel*> labels;
    AsmState() {
        freeRegisters[ASM_REG_INVALID] = false;
        freeRegisters[ASM_REG_NONE] = false;
        for(uint32_t reg = ASM_REG_RAX;reg < ASM_REG_MAX;reg++) {
            freeRegisters[reg] = true;
        }
        blockExit = nullptr;
    }
    ~AsmState() {
        for(auto lbl : labels) {
            delete lbl;
        }
        labels.clear();
    }
    AsmLabel* getNewLabel() {
        AsmLabel* lbl = new AsmLabel("L" + std::to_string(labels.size()));
        labels.push_back(lbl);
        return lbl;
    }
    enum ASM_REG useFreeRegister() {
        for(uint32_t reg = ASM_REG_RAX;reg < ASM_REG_MAX;reg++) {
            if (freeRegisters[reg]) {
                freeRegisters[reg] = false;
                return (enum ASM_REG)reg;
            }
        }
    }
    enum ASM_REG useRegister(enum ASM_REG reg) {
        freeRegisters[reg] = false;
        return reg;
    }
    void freeRegister(enum ASM_REG reg) {
        freeRegisters[reg] = true;
    }
};

class Symbol {
public:
    std::string ident;
    Symbol* parent;
    Symbol(const std::string& ident) : ident(ident), parent(nullptr) {
    }
    void error(const std::string& err);
    virtual std::string toString() { return "Symbol"; }
    virtual Module* isModule() { return nullptr; }
    virtual Function* isFunction() { return nullptr; }
    virtual Variable* isVariable() { return nullptr; }
};

class Expression {
public:
    enum TOKEN op;
    Expression(enum TOKEN op) : op(op) { }
    void error(const std::string& err);
    virtual Expression* semantic(Scope* sc) { return this; }
    virtual std::string toAsmCode(AsmState* as, enum ASM_REG retreg) { return ""; }

    virtual IntegerExp* isInteger() { return nullptr; }
    virtual StringExp* isString() { return nullptr; }
};

class AsmReadyOnlyData;

class StringExp : public Expression {
public:
    std::vector<unsigned char> ustring;
    AsmReadyOnlyData* asmdata;
    StringExp(const std::vector<unsigned char>& value)
        : Expression(TOKstring), ustring(value), asmdata(nullptr) {
    }
    virtual std::string toAsmCode(AsmState* as, enum ASM_REG retreg);

    virtual StringExp* isString() { return this; }
};

class IntegerExp  : public Expression {
public:
    int value;
    IntegerExp(int value, enum TOKEN op) : Expression(op), value(value) { }
    virtual std::string toAsmCode(AsmState* as, enum ASM_REG retreg);

    virtual IntegerExp* isInteger() { return this; }
};

class Variable;

class IdentifierExp  : public Expression {
public:
    std::string ident;
    Variable* var;
    IdentifierExp(const std::string& ident) : Expression(TOKidentifier), ident(ident), var(nullptr) { }
    virtual Expression* semantic(Scope* sc);
    virtual std::string toAsmCode(AsmState* as, enum ASM_REG retreg);
};

class CallExp  : public Expression {
public:
    Expression* exp;
    std::vector<Expression*> args;
    CallExp(Expression* e, const std::vector<Expression*>& exps) : Expression(TOKcall), exp(e), args(exps) { }
    virtual Expression* semantic(Scope* sc);
    virtual std::string toAsmCode(AsmState* as, enum ASM_REG retreg);
};

class BinExp : public Expression {
public:
    Expression* e1;
	Expression* e2;
    BinExp(enum TOKEN op, Expression* e1, Expression* e2) : Expression(op), e1(e1), e2(e2) {
    }
    virtual Expression* semantic(Scope* sc);
    virtual std::string toAsmCode(AsmState* as, enum ASM_REG retreg) = 0;
};

class AssignExp : public BinExp {
public:
    AssignExp(Expression* e1, Expression* e2) : BinExp(TOKassign, e1, e2) {
    }
    virtual std::string toAsmCode(AsmState* as, enum ASM_REG retreg);
};

class AddExp : public BinExp {
public:
    AddExp(Expression* e1, Expression* e2) : BinExp(TOKadd, e1, e2) {
    }
    virtual std::string toAsmCode(AsmState* as, enum ASM_REG retreg);
};

class MinExp : public BinExp {
public:
    MinExp(Expression* e1, Expression* e2) : BinExp(TOKmin, e1, e2) {
    }
    virtual std::string toAsmCode(AsmState* as, enum ASM_REG retreg);
};

class DivExp : public BinExp {
public:
    DivExp(Expression* e1, Expression* e2) : BinExp(TOKdiv, e1, e2) {
    }
    virtual std::string toAsmCode(AsmState* as, enum ASM_REG retreg);
};

class MulExp : public BinExp {
public:
    MulExp(Expression* e1, Expression* e2) : BinExp(TOKmul, e1, e2) {
    }
    virtual std::string toAsmCode(AsmState* as, enum ASM_REG retreg);
};

// == and !=
class EqualExp : public BinExp {
public:
// Methods:
	EqualExp(enum TOKEN op, Expression* e1, Expression* e2) : BinExp(op, e1, e2) {
	}
	virtual std::string toAsmCode(AsmState* as, enum ASM_REG retreg);
};

class AndAndExp : public BinExp {
public:
// Methods:
	AndAndExp(Expression* e1, Expression* e2) : BinExp(TOKandand, e1, e2) {
	}
	virtual std::string toAsmCode(AsmState* as, enum ASM_REG retreg);
};

class OrOrExp : public BinExp {
public:
// Methods:
	OrOrExp(Expression* e1, Expression* e2) : BinExp(TOKoror, e1, e2) {
	}
	virtual std::string toAsmCode(AsmState* as, enum ASM_REG retreg);
};

class DeclarationExp  : public Expression {
public:
    Declaration* declaration;
    DeclarationExp(Declaration* d) : Expression(TOKdeclaration), declaration(d) { }
    virtual Expression* semantic(Scope* sc);
    virtual std::string toAsmCode(AsmState* as, enum ASM_REG retreg);
};


class Statement {
public:
    Statement() { }
    virtual void semantic(Scope* sc) { }
    virtual std::string genAsmCode(AsmState* as) { }
};

class CompoundStatement : public Statement {
public:
    std::vector<Statement*> statements;
    CompoundStatement(const std::vector<Statement*>& s) : statements(s) {
    }
    CompoundStatement(Statement* s) {
        statements.push_back(s);
    }
    virtual void semantic(Scope* sc);
    virtual std::string genAsmCode(AsmState* as);
};

class ExpStatement : public Statement {
public:
    Expression* exp;
    ExpStatement(Expression* e) : exp(e) {
    }
    virtual void semantic(Scope* sc);
    virtual std::string genAsmCode(AsmState* as);
};

class IfStatement : public Statement {
public:
// Members:
	Expression* condition;
	Statement* ifbody;
    Statement* elsebody;
// Methods:
	IfStatement(Expression* cond, Statement* ifbody, Statement* elsebody)
        : condition(cond), ifbody(ifbody), elsebody(elsebody) {
	}

	virtual void semantic(Scope* sc);
	virtual std::string genAsmCode(AsmState* as);
};

class ExpInitializer;

class Initializer {
public:
    Initializer() {
    }
    virtual void semantic(Scope* sc) { }
    virtual void genAsmCode(AsmState* as) { }

    virtual ExpInitializer* isExpInitializer() { return nullptr; }
};

class ExpInitializer : public Initializer {
public:
    Expression* exp;
    ExpInitializer(Expression* e) : exp(e) {
    }
    virtual void genAsmCode(AsmState* as) { }

    virtual ExpInitializer* isExpInitializer() { return this; }
};

class AsmSymbol;

class Type;
class IntegerType;
class ArrayType;
class StringType;

class Type {
public:
    std::string name;
    Type(const std::string& name) : name(name) {
    }

    virtual StringType* isStringType() { return nullptr; }
};

class IntegerType : public Type {
public:
    IntegerType() : Type("int") {
    }
};

class ArrayType : public Type {
public:
    ArrayType() : Type("array") {
    }
};

class StringType : public Type {
public:
    StringType() : Type("string") {
    }

    virtual StringType* isStringType() { return this; }
};

class Declaration : public Symbol {
public:
    Declaration(const std::string& ident) : Symbol(ident) {
    }
    virtual std::string toString() { return "Declaration"; }
    virtual void semantic(Scope* sc) { }
    virtual void genAsmCode() { }
    virtual AsmSymbol* getAsmSymbol() { return nullptr; }
};

class Variable : public Declaration {
public:
    Type* type;
    Initializer* init;
    AsmSymbol* asmsym;
    Variable(const std::string& ident) : Declaration(ident), type(nullptr), init(nullptr), asmsym(nullptr) {
    }
    virtual std::string toString() { return "Variable"; }
    virtual Variable* isVariable() { return this; }
    bool isDataSeg() const;
    virtual void semantic(Scope* sc);
    virtual void genAsmCode();
    virtual AsmSymbol* getAsmSymbol();
};

class Function : public Declaration {
public:
    Statement* body;
    Function(const std::string& ident) : Declaration(ident), body(nullptr) {
    }
    virtual std::string toString() { return "Function"; }
    virtual Function* isFunction() { return this; }
    virtual void semantic(Scope* sc);
    virtual void genAsmCode();
};

class Scope {
public:
    Scope* enclosing;
    Symbol* parent;
    Module* module; // Root module
    std::map<std::string, Symbol*> symbols;
    Scope(Scope* enclosing) : enclosing(enclosing) {
    }
    Symbol* findSymbol(const std::string& name);
    void insertSymbol(Symbol* symbol);
};

class Module : public Symbol {
public:
    std::vector<Declaration*> decls;
    std::string srcFileName;
	std::string asmFileName;
    File* srcFile;
public:
    Module(const std::string& filename);
    virtual std::string toString() { return "Module"; }
    virtual Module* isModule() { return this; }
    bool read();
    bool parse();
    void semantic();
    void genAsmFile();
};


