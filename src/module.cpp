#include "stdheaders.h"
#include "file.h"
#include "parser.h"
#include "module.h"

Module::Module(const std::string& filepath) : Symbol("module") {

    auto filenameOnly = std::filesystem::path(filepath).stem();

    srcFileName = filenameOnly.string() + ".reza";
    asmFileName = filenameOnly.string() + ".asm";

    srcFile = new File(filepath);

    ident = srcFileName;
}
bool Module::read() {
    if (srcFile->read()) {
        return true;
    }
    return false;
}

bool Module::parse() {
    Parser* p = new Parser(this);
    p->parseModule();
    delete p;
    return true;
}

void Module::semantic() {
    Scope* sc = new Scope(nullptr);
    sc->module = this;
    sc->parent = this;

    // add built-in functions
    Function* f = nullptr;
    f = new Function("print");
    sc->insertSymbol(f);
    f = new Function("printint");
    sc->insertSymbol(f);

    for(auto i = decls.begin();i != decls.end();++i) {
        auto d = *i;
        d->semantic(sc);
    }
    delete sc;
}

Symbol* Scope::findSymbol(const std::string& name) {
    auto sc = this;
    while(sc) {
        auto it = sc->symbols.find(name);
        if (it != sc->symbols.end()) {
            return it->second;
        }
        sc = sc->enclosing;
    }
    return nullptr;
}
void Scope::insertSymbol(Symbol* symbol) {
    symbols[symbol->ident] = symbol;
}

void Symbol::error(const std::string& err) {
    std::cout << "error: " << err << std::endl;
    exit(EXIT_FAILURE);
}

void Function::semantic(Scope* sc) {
    this->parent = sc->parent;
    if (body) {
        Scope* sc2 = new Scope(sc);
        sc2->module = sc->module;
        sc2->parent = this;
        // analyze the body
        body->semantic(sc2);
        delete sc2;
    }
}

bool Variable::isDataSeg() const {
    assert(parent);
    if (parent->isModule()) {
        return true;
    }
    return false;
}

void Variable::semantic(Scope* sc) {
    this->parent = sc->parent;

    // semantic is done
    if (type) {
        return;
    }

    auto var = sc->findSymbol(this->ident);
    if (var) {
        error("variable '" + this->ident + "' already exit.");
        return;
    }
    sc->insertSymbol(this);

    if (init) {
        auto ei = init->isExpInitializer();
        if (ei) {
            auto e = ei->exp;
            if (e->isInteger()) {
                type = new IntegerType();
            } else if (e->isString()) {
                type = new StringType();
            } else {
                error("variable '" + this->ident + "': unknown data type.");
            }
        } else {
            error("variable '" + this->ident + "': invalid initializer.");
        }
    } else {
        error("variable '" + this->ident + "': initializer is missing.");
    }
    if (!type) {
        error("variable '" + this->ident + "': couldn't deduce data type.");
    }
}

void CompoundStatement::semantic(Scope* sc) {
    for(auto s: statements) {
        s->semantic(sc);
    }
}

void ExpStatement::semantic(Scope* sc) {
    if (exp) {
        exp = exp->semantic(sc);
    }
}

void IfStatement::semantic(Scope* sc) {
    if (condition) {
        condition = condition->semantic(sc);
    }
    if (ifbody) {
        ifbody->semantic(sc);
    }
    if (elsebody) {
        elsebody->semantic(sc);
    }
}

void Expression::error(const std::string& err) {
    std::cout << "error: " << err << std::endl;
    exit(EXIT_FAILURE);
}

Expression* DeclarationExp::semantic(Scope* sc) {
    if(declaration) {
        declaration->semantic(sc);
    }
    return this;
}

Expression* IdentifierExp::semantic(Scope* sc) {
    auto sym = sc->findSymbol(ident);
    if (!sym) {
        error("undefined identifier '" + ident + "'");
    }
    var = sym->isVariable();
    if (var) {
       // std::cout << var->ident << ": " << var->type->name << std::endl;
    } else {
        //assert(0);
    }
    return this;
}

Expression* CallExp::semantic(Scope* sc) {
    exp = exp->semantic(sc);
    for(size_t i = 0; i < args.size();++i) {
        args[i] = args[i]->semantic(sc);
    }
    return this;
}

Expression* BinExp::semantic(Scope* sc) {
    e1 = e1->semantic(sc);
    e2 = e2->semantic(sc);
    return this;
}




