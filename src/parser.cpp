#include "stdheaders.h"
#include "file.h"
#include "lexer.h"
#include "module.h"
#include "parser.h"

Parser::Parser(Module* m) : me(m) {
    lexer = new Lexer(m->srcFile->buffer);
}

Parser::~Parser() {
    delete lexer;
}

std::vector<Expression*> Parser::parseArguments() {
	std::vector<Expression*> arguments;
    Expression* arg;
    lexer->next();
    while (lexer->token.value != TOKeof && lexer->token.value != TOKrparen) {
            arg = parseAssignExp();
            arguments.push_back(arg);
            if (lexer->token.value == TOKrparen)
                break;
            if (lexer->token.value != TOKcomma) {
                lexer->error("',' expected, not '" + lexer->token.toString() + "'");
            }
            lexer->next();
    }
    if (lexer->token.value != TOKrparen) {
        lexer->error("')' expected, not '" + lexer->token.toString() + "'");
    }
    lexer->next();
    return arguments;
}

Expression* Parser::parsePrimaryExp() {
    Expression *e;
    std::string id;
    switch (lexer->token.value)
    {
        case TOKidentifier:
			id = lexer->token.identifier;
            lexer->next();
            e = new IdentifierExp(id);
			break;
        case TOKstring:
            {
                auto sb = lexer->token.stringbuffer;
                lexer->next();
                e = new StringExp(sb);
            }
			break;
        case TOKcharv:
        case TOKintv:
            e = new IntegerExp(lexer->token.uns64value, TOKintv);
            lexer->next();
            break;
        default:
            lexer->error("expression expected, not '" +
                         lexer->token.toString() + "', " + std::to_string(lexer->token.value));
        Lerr:
            // Anything for e, as long as it's not NULL
            e = new IntegerExp(0, TOKintv);
            lexer->next();
            break;
    }
    return e;
}

Expression* Parser::parsePostExp(Expression *e) {
    while (1) {
        switch (lexer->token.value) {
            case TOKlparen: // () call expression
                e = new CallExp(e, parseArguments());
                continue;
            default:
                return e;
        }
        lexer->next();
    }
    return e;
}

Expression* Parser::parseAddExp() {
	Expression *e;
    Expression *e2;
	//Loc loc = file->loc;

	e = parseMulExp();

	while (1) {
        switch (lexer->token.value) {
            case TOKadd:
                lexer->next();
                e2 = parseMulExp();
                e = new AddExp(e,e2);
                continue;
            case TOKmin:
                lexer->next();
                e2 = parseMulExp();
                e = new MinExp(e,e2);
                continue;
            //case TOKtilde:  next(); e2 = parseMulExp(); e = new CatExp(loc,e,e2); continue;
            default:
                break;
        }
        break;
    }
    return e;
}

Expression* Parser::parseUnaryExp() {
    Expression *e;
    e = parsePrimaryExp();
    e = parsePostExp(e);
    return e;
}

Expression* Parser::parseMulExp() {
	Expression *e;  //left operand
    Expression *e2; //right operand
	//Loc loc = file->loc;

    e = parseUnaryExp(); //get the left operand

    while (1) {
        switch (lexer->token.value) {
            case TOKmul:
                lexer->next();
                e2 = parseUnaryExp();
                e = new MulExp(e, e2);
                continue;
            case TOKdiv:
                lexer->next();
                e2 = parseUnaryExp();
                e = new DivExp(e, e2);
                continue;
/*
            case TOKmod:
                lexer->next();
                e2 = parseUnaryExp();
                e = new ModExp(e, e2);
                continue;
*/
            default:
                break;
        }
        break;
    }
    return e;
}

Expression* Parser::parseAssignExp() {
    Expression *e;
    Expression *e2;

    e = parseCondExp();

    switch (lexer->token.value) {
        case TOKassign:
            lexer->next();
            e2 = parseAssignExp();
            e = new AssignExp(e, e2);
            break;
        default:
            break;
    }
	return e;
}

Expression *Parser::parseCondExp() {
	Expression *e;
    Expression *e1;
    Expression *e2;

    e = parseOrOrExp();
    /*
    if (token.value == TOKquestion)
    {
        next();
        e1 = parseExpression(sc);
        parseCheck(TOKcolon);
        e2 = parseCondExp(sc);
        e = new CondExp(loc, e, e1, e2);
    }
    */
    return e;
}

Expression* Parser::parseOrOrExp() {
	Expression *e;
    Expression *e2;

    e = parseAndAndExp();

    while (lexer->token.value == TOKoror | lexer->token.value == TOKor2) {
        lexer->next();
        e2 = parseAndAndExp();
        e = new OrOrExp(e, e2);
    }
    return e;
}

Expression* Parser::parseAndAndExp() {
	Expression *e;
    Expression *e2;
	//Loc loc = file->loc;

    e = parseOrExp();

    while (lexer->token.value == TOKandand || lexer->token.value == TOKand2) {
        lexer->next();
        e2 = parseOrExp();
        e = new AndAndExp(e, e2);
    }
    return e;
}

Expression* Parser::parseOrExp() {
    Expression *e = parseXorExp();
/*
    while (token.value == TOKor)
    {
        //checkParens(TOKor, e);
        next();
        Expression *e2 = parseXorExp(sc);
        //checkParens(TOKor, e2);
        e = new OrExp(loc, e, e2);
    }
*/
    return e;
}

Expression *Parser::parseXorExp() {
    Expression *e = parseAndExp();
/*
    while (token.value == TOKxor)
    {
        //checkParens(TOKxor, e);
        next();
        Expression *e2 = parseAndExp(sc);
        //checkParens(TOKxor, e2);
        e = new XorExp(loc, e, e2);
    }
*/
    return e;
}

Expression* Parser::parseAndExp() {
    Expression *e = NULL;

	e = parseCmpExp();
/*
    while (token.value == TOKand)
    {
        //checkParens(TOKand, e);
        next();
        Expression *e2 = parseCmpExp(sc);
        //checkParens(TOKand, e2);
        e = new AndExp(loc, e, e2);
        loc = loc;
    }
*/
    return e;
}

Expression* Parser::parseCmpExp() {
	Expression *e;
    Expression *e2;
    Token* t;

    e = parseShiftExp();

	enum TOKEN op = lexer->token.value;

	switch (op)
    {
        case TOKequal:
        case TOKnotequal:
            lexer->next();
            e2 = parseShiftExp();
            e = new EqualExp(op, e, e2);
            break;
        /*
        case TOKlt:
        case TOKle:
        case TOKgt:
        case TOKge:
            lexer->next();
            e2 = parseShiftExp(sc);
            e = new CmpExp(op, loc, e, e2);
            break;
        */
        default:
            break;
    }

	return e;
}

Expression* Parser::parseShiftExp() {
	Expression *e;
    Expression *e2;

    e = parseAddExp();
/*
    while (1)
    {
        switch (token.value)
        {
            case TOKshl:  next(); e2 = parseAddExp(sc); e = new ShlExp(loc,e,e2);  continue;
            case TOKshr:  next(); e2 = parseAddExp(sc); e = new ShrExp(loc,e,e2);  continue;
            //case TOKushr: lex.next(); e2 = parse_AddExp(); e = new UshrExp(loc,e,e2); continue;
            default:
                break;
        }
        break;
    }
*/
    return e;
}

Expression* Parser::parseExpression() {
    Expression *e = parseAssignExp();
	return e;
}

Statement* Parser::parseStatement() {
    Statement *s = nullptr;
Lagain:
    switch (lexer->token.value) {
        case TOKlcurly:
        {
			lexer->next();
			std::vector<Statement*> statements;
            while (lexer->token.value != TOKrcurly && lexer->token.value != TOKeof) {
                auto s2 = parseStatement();
                if (s2 != nullptr) {
                    statements.push_back(s2);
                }
            }
            s = new CompoundStatement(statements);
            //if (flags & (PSscope | PScurlyscope))
                //s = new ScopeStatement(loc, s);
            if (lexer->token.value != TOKrcurly) {
                lexer->error("'}' expected, not '" + lexer->token.toString() + "'");
            }
            lexer->next();
            break;
        }
        case TOKidentifier:
            {
                Expression* e = parseExpression();
                if (lexer->token.value != TOKnewline) {
                    lexer->error("new line expected, not '" + lexer->token.toString() + "'");
                }
                lexer->next();
                s = new ExpStatement(e);
                break;
            }
        case TOKvar:
            {
                std::vector<Declaration*> decls;
                parseDeclaration(decls);
                if (decls.size() == 1) {
                    Declaration* d = decls[0];
                    Expression* e = new DeclarationExp(d);
                    s = new ExpStatement(e);
                } else {
                    assert(0);
                }
                break;
            }
        case TOKif:
            {
                lexer->next();
                Expression* condition;
                Statement* ifbody;
                Statement* elsebody;

                condition = parseExpression();
                ifbody = parseStatement();
                if (lexer->token.value == TOKelse) {
                    lexer->next();
                    elsebody = parseStatement();
                }
                else {
                    elsebody = nullptr;
                }
                if (condition && ifbody) {
                    s = new IfStatement(condition, ifbody, elsebody);
                }
                else {
                    s = nullptr;               // don't propagate parsing errors
                }
                break;
            }
        case TOKnewline:
            lexer->next();
            break;
        default:
			lexer->error("statement expected, not '" + lexer->token.toString() + "'");
            goto Lerror;
        Lerror:
            while (lexer->token.value != TOKrcurly &&
                   lexer->token.value != TOKnewline &&
				   lexer->token.value != TOKeof) {
                lexer->next();
			}
			if (lexer->token.value == TOKnewline) {
                lexer->next();
			}
			s = nullptr;
            break;
    }
    return s;
}

Initializer* Parser::parseInitializer()
{
    Expression* e = parseAssignExp();
    ExpInitializer* ie = new ExpInitializer(e);
    return ie;
}

void Parser::parseDeclaration(std::vector<Declaration*>& decls) {
Lagain:
    switch (lexer->token.value) {
        case TOKvar:
            {
                lexer->next();
                if (lexer->token.value == TOKidentifier) {
                    auto ident = lexer->token.identifier;
                    lexer->next();


                    Initializer* init = nullptr;
                    if (lexer->token.value == TOKassign) {
                        lexer->next();
                        init = parseInitializer();
                    }

                    if (lexer->token.value != TOKnewline) {
                        lexer->error("new line expected after variable initialization.");
                    }
                    lexer->next();

                    //printf("token: %s\n", lexer->token.toString().c_str());

                    auto v = new Variable(ident);
                    v->init = init;

                    decls.push_back(v);
                } else {
                    lexer->error("variable's identifier expected");
                }
            }
            break;
        case TOKfunction:
            {
                lexer->next();
                if (lexer->token.value == TOKidentifier) {
                    auto ident = lexer->token.identifier;
                    lexer->next();
                    if (lexer->token.value != TOKlparen) {
                        lexer->error("function's parameters missing. '(' expected after function's name.");
                        break;
                    }
                    lexer->next();
                    if (lexer->token.value != TOKrparen) {
                        lexer->error("closing ')' missing.");
                        break;
                    }
                    lexer->next();

                    Statement* body = nullptr;

                    if (lexer->token.value == TOKlcurly) {
                        body = parseStatement();
                    } else if (lexer->token.value == TOKnewline) {
                        lexer->next();
                    } else {
                        lexer->error("new line or function body '{ ... }' expected.");
                    }
                    //printf("function ident: %s\n", ident.c_str());

                     //printf("after token: %s\n", lexer->token.toString().c_str());

                    auto f = new Function(ident);
                    f->body = body;
                    decls.push_back(f);
                } else {
                    lexer->error("function's identifier expected");
                }
            }
            break;
        case TOKnewline:
            lexer->next();
            goto Lagain;
        default:
            lexer->error("declaration expected, not '" + lexer->token.toString() + "'");
        Lerror:
            while (lexer->token.value != TOKnewline && lexer->token.value != TOKeof) {
                lexer->next();
            }
            lexer->next();
            break;
    }
}

void Parser::parseDeclarations(std::vector<Declaration*>& decls) {
    do {
        switch (lexer->token.value) {
            case TOKfunction:
            case TOKvar:
                {
                    std::vector<Declaration*> a;
                    parseDeclaration(a);
                    decls.insert(decls.end(), a.begin(), a.end());
                }
                continue;
            case TOKeof:
                return;
			case TOKnewline:          // empty declaration
                lexer->next();
                continue;
            default:
                lexer->error("declaration expected, not '" + lexer->token.toString() + "'");
            Lerror:
                while (lexer->token.value != TOKnewline && lexer->token.value != TOKeof) {
                    lexer->next();
                }
                lexer->next();
                continue;
        }
    } while(1);
}

bool Parser::parseModule() {
    lexer->next();

    std::vector<Declaration*> decls;
    parseDeclarations(decls);
    me->decls.insert(me->decls.end(), decls.begin(), decls.end());
	if (lexer->token.value != TOKeof) {
		lexer->error("unrecognized declaration '" + lexer->token.toString() + "'");
        goto Lerr;
    }
	return true;
Lerr:
	while (lexer->token.value != TOKnewline && lexer->token.value != TOKeof) {
        lexer->next();
	}
    lexer->next();
    return false;
}


