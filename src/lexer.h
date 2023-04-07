#pragma once

#include "tokens.h"

struct Keyword {
    std::string name;
    enum TOKEN value;
};

static Keyword keywords[] =
{
//    { "",             TOK     },
    {   "this",         TOKthis         },
    {   "base",         TOKbase         },
    {   "nil",			TOKnull         },
    {   "true",         TOKtrue         },
    {   "false",        TOKfalse        },
    {   "cast",         TOKcast         },
    {   "new",          TOKnew          },
	{   "let",			TOKlet			},
	{   "sizeof",       TOKsizeof       },
    {   "delete",		TOKdelete       },

    {   "and",          TOKand2         },
    {   "or",           TOKor2         },

    {   "void",         TOKvoid         },
    {   "byte",         TOKbyte         },
    {   "uchar",        TOKuchar        },
    {   "short",        TOKshort        },
    {   "ushort",       TOKushort       },
    {   "int",          TOKint          },
    {   "uint",         TOKuint         },
    {   "long",         TOKlong         },
    {   "ulong",        TOKulong        },
    {   "cent",         TOKcent,        },
    {   "ucent",        TOKucent,       },
    {   "float",        TOKfloat        },
    {   "double",       TOKdouble       },
    {   "real",         TOKreal         },

    {   "bool",         TOKbool         },
    {   "char",         TOKchar         },
    {   "wchar",        TOKwchar        },
    {   "function",		TOKfunction     },
	{   "__link__",		TOKlink			},

	{	"dllimport",	TOKdllimport	},
	{	"dllexport",	TOKdllexport	},
    {   "is",           TOKis           },
    {   "if",           TOKif           },
    {   "else",         TOKelse         },
	{   "then",         TOKthen         },
    {   "for",          TOKfor          },
    {   "match",		TOKmatch       },
    {   "case",         TOKcase         },
    {   "otherwise",	TOKotherwise      },
    {   "break",        TOKbreak        },
    {   "continue",     TOKcontinue     },
    {   "ret",			TOKreturn       },
    {   "goto",         TOKgoto         },
    {   "asm",          TOKasm          },
    {   "struct",       TOKstruct       },
    {   "class",        TOKclass        },
    {   "interface",    TOKinterface    },
    {   "union",        TOKunion        },
    {   "enum",         TOKenum         },
	{   "group",		TOKgroup		},
    {   "import",       TOKimport       },
	{   "open",			TOKopen			},
    {   "static",       TOKstatic       },
    {   "const",        TOKconst        },
	{   "inline",       TOKinline       },
    {   "typedef",      TOKtypedef      },
    {   "alias",        TOKalias        },
    {   "override",     TOKoverride     },
    {   "abstract",     TOKabstract     },
	{   "virtual",      TOKvirtual      },
	{   "var",			TOKvar			},

    {   "align",        TOKalign        },
    {   "extern",		TOKextern       },
    {   "private",      TOKprivate      },
    {   "package",      TOKpackage      },
    {   "protected",    TOKprotected    },
    {   "public",       TOKpublic       },
    {   "export",       TOKexport       },
    {   "ref",          TOKref          },
    {   "macro",        TOKmacro        },

};

class Token {
public:
    Token* next;
    enum TOKEN value;
    static std::string tochars[TOKMAX];
    std::string identifier;
    std::vector<unsigned char> stringbuffer;
    uint64_t uns64value;
    Token() : next(nullptr), value(TOKreserved) {
    }
    std::string toString() const;
};

class Lexer {
public:
    static std::map<std::string, enum TOKEN> gStringTable;
    Token token;
    std::string::const_iterator p;
    std::string::const_iterator endOfFile;
    Lexer(const std::string& buffer);
    static void init();
    TOKEN next();
    TOKEN number(Token *t);
    void scan(Token *t);
    void error(const std::string& err);
};




