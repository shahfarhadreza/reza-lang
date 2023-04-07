#pragma once

class Lexer;
class Module;
class Initializer;
class Declaration;
class Expression;
class Statement;

class Parser {
public:
    Lexer* lexer;
    Module* me;
public:
    Parser(Module* m);
    ~Parser();
    std::vector<Expression*> parseArguments();
    Expression* parseExpression();
    Expression* parseAssignExp();
    Expression* parseCondExp();
    Expression* parseOrOrExp();
	Expression* parseAndAndExp();
	Expression* parseOrExp();
	Expression* parseXorExp();
	Expression* parseAndExp();
	Expression* parseCmpExp();
	Expression* parseShiftExp();
	Expression* parseAddExp();
    Expression* parseMulExp();
    Expression* parseUnaryExp();
    Expression* parsePrimaryExp();
    Expression* parsePostExp(Expression *e);
    Statement* parseStatement();
    Initializer* parseInitializer();
    void parseDeclaration(std::vector<Declaration*>& decls);
    void parseDeclarations(std::vector<Declaration*>& decls);
    bool parseModule();
};

