#include "stdheaders.h"
#include "tokens.h"
#include "lexer.h"

std::map<std::string, enum TOKEN> Lexer::gStringTable;

std::string Token::tochars[TOKMAX];

static unsigned char cmtable[256];

const int CMoctal	= 0x1;
const int CMhex		= 0x2;
const int CMidchar	= 0x4;

inline unsigned char isoctal(unsigned char c) { return cmtable[c] & CMoctal; }
inline unsigned char ishex(unsigned char c) { return cmtable[c] & CMhex; }
inline unsigned char isidchar(unsigned char c) { return cmtable[c] & CMidchar; }

static void cmtable_init() {
    const size_t tableSize = sizeof(cmtable) / sizeof(cmtable[0]);
    for (unsigned c = 0; c < tableSize; c++) {
        if ('0' <= c && c <= '7')
            cmtable[c] |= CMoctal;
        if (isdigit(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'))
            cmtable[c] |= CMhex;
        if (isalnum(c) || c == '_')
            cmtable[c] |= CMidchar;
    }
}

std::string Token::toString() const {
    if (value == TOKidentifier) {
        return identifier;
    } else if (value == TOKstring) {
        std::string str;
        for (const auto& ch : stringbuffer) {
            str += ch;
        }
        return str;
    }
    return tochars[value];
}

void Lexer::init() {
    size_t nkeywords = sizeof(keywords) / sizeof(keywords[0]);

    cmtable_init();

	for (size_t u = 0; u < nkeywords; u++) {
        const std::string& s = keywords[u].name;
        enum TOKEN v = keywords[u].value;

        Lexer::gStringTable[s] = v;
        /*
        StringValue *sv = stringtable.insert(s, strlen(s));
		assert(sv);
        sv->ptrvalue = (void *) new Identifier(sv->toDchars(),v);

        //printf("tochars[%d] = '%s'\n",v, s);
        */
        Token::tochars[v] = s;
    }

    Token::tochars[TOKeof]              = "end of file";
	Token::tochars[TOKnewline]			= "new line";
	Token::tochars[TOKspace]			= "Space";
	Token::tochars[TOKdblslash]			= "\\";
    Token::tochars[TOKlcurly]           = "{";
    Token::tochars[TOKrcurly]           = "}";
    Token::tochars[TOKlparen]           = "(";
    Token::tochars[TOKrparen]           = ")";
    Token::tochars[TOKlbracket]         = "[";
    Token::tochars[TOKrbracket]         = "]";
    Token::tochars[TOKsemicolon]        = ";";
    Token::tochars[TOKcolon]            = ":";
    Token::tochars[TOKcomma]            = ",";
    Token::tochars[TOKdot]              = ".";
	Token::tochars[TOKptrid]            = "->";
	Token::tochars[TOKresolution]       = "::";
    Token::tochars[TOKxor]              = "^";
    Token::tochars[TOKxorass]           = "^=";
    Token::tochars[TOKassign]           = "=";
    Token::tochars[TOKconstruct]        = "=";
    Token::tochars[TOKlt]               = "<";
    Token::tochars[TOKgt]               = ">";
    Token::tochars[TOKle]               = "<=";
    Token::tochars[TOKge]               = ">=";
    Token::tochars[TOKequal]            = "==";
    Token::tochars[TOKnotequal]         = "!=";
    Token::tochars[TOKnotidentity]      = "!is";
    Token::tochars[TOKtobool]           = "!!";

    Token::tochars[TOKunord]            = "!<>=";
    Token::tochars[TOKue]               = "!<>";
    Token::tochars[TOKlg]               = "<>";
    Token::tochars[TOKleg]              = "<>=";
    Token::tochars[TOKule]              = "!>";
    Token::tochars[TOKul]               = "!>=";
    Token::tochars[TOKuge]              = "!<";
    Token::tochars[TOKug]               = "!<=";

    Token::tochars[TOKnot]              = "!";
    Token::tochars[TOKtobool]           = "!!";
    Token::tochars[TOKshl]              = "<<";
    Token::tochars[TOKshr]              = ">>";
    Token::tochars[TOKushr]             = ">>>";
    Token::tochars[TOKadd]              = "+";
    Token::tochars[TOKmin]              = "-";
    Token::tochars[TOKmul]              = "*";
    Token::tochars[TOKdiv]              = "/";
    Token::tochars[TOKmod]              = "%";
    Token::tochars[TOKslice]            = "..";
    Token::tochars[TOKdotdotdot]        = "...";
    Token::tochars[TOKand]              = "&";
    Token::tochars[TOKandand]           = "&&";
    Token::tochars[TOKand2]             = "and";
    Token::tochars[TOKor]               = "|";
    Token::tochars[TOKoror]             = "||";
    Token::tochars[TOKor2]              = "or";
    Token::tochars[TOKarray]            = "[]";
    Token::tochars[TOKindex]            = "[i]";
    Token::tochars[TOKaddress]          = "&";
    Token::tochars[TOKstar]             = "*";
    Token::tochars[TOKtilde]            = "~";
    Token::tochars[TOKdollar]           = "$";
    Token::tochars[TOKcast]             = "cast";
    Token::tochars[TOKplusplus]         = "++";
    Token::tochars[TOKminusminus]       = "--";
    Token::tochars[TOKpreplusplus]      = "++";
    Token::tochars[TOKpreminusminus]    = "--";
    Token::tochars[TOKtype]             = "type";
    Token::tochars[TOKquestion]         = "?";
    Token::tochars[TOKneg]              = "-";
    Token::tochars[TOKuadd]             = "+";
    Token::tochars[TOKvar]              = "var";
    Token::tochars[TOKaddass]           = "+=";
    Token::tochars[TOKminass]           = "-=";
    Token::tochars[TOKmulass]           = "*=";
    Token::tochars[TOKdivass]           = "/=";
    Token::tochars[TOKmodass]           = "%=";
    Token::tochars[TOKshlass]           = "<<=";
    Token::tochars[TOKshrass]           = ">>=";
    Token::tochars[TOKushrass]          = ">>>=";
    Token::tochars[TOKandass]           = "&=";
    Token::tochars[TOKorass]            = "|=";
    Token::tochars[TOKcatass]           = "~=";
    Token::tochars[TOKcat]              = "~";
    Token::tochars[TOKcall]             = "call";
    Token::tochars[TOKidentity]         = "is";
    Token::tochars[TOKnotidentity]      = "!is";

    Token::tochars[TOKorass]            = "|=";
    Token::tochars[TOKidentifier]       = "identifier";


     // For debugging
    Token::tochars[TOKerror]            = "error";
    Token::tochars[TOKstring]           = "string";
}

Lexer::Lexer(const std::string& buffer) {
    p = buffer.cbegin();
    endOfFile = buffer.cend();
}

TOKEN Lexer::next() {
    scan(&token);
    return token.value;
}

TOKEN Lexer::number(Token *t)
{
    // We use a state machine to collect numbers
    enum STATE { STATE_initial, STATE_0, STATE_decimal, STATE_octal, STATE_octale,
        STATE_hex, STATE_binary, STATE_hex0, STATE_binary0,
        STATE_hexh, STATE_error };
    enum STATE state;

    enum FLAGS
    {
        FLAGS_none     = 0,
        FLAGS_decimal  = 1,             // decimal
        FLAGS_unsigned = 2,             // u or U suffix
        FLAGS_long     = 4,             // l or L suffix
    };
    enum FLAGS flags = FLAGS_decimal;

    unsigned c;
    TOKEN result;

    //printf("Lexer::number()\n");
    state = STATE_initial;
    std::string stringbuffer = "";
    //stringbuffer.reset();
    auto start = p;
    while (1)
    {
        c = *p;
        switch (state)
        {
            case STATE_initial:         // opening state
                if (c == '0')
                    state = STATE_0;
                else
                    state = STATE_decimal;
                break;

            case STATE_0:
                flags = (FLAGS) (flags & ~FLAGS_decimal);
                switch (c)
                {
                    case 'X':
                    case 'x':
                        state = STATE_hex0;
                        break;

                    case '.':
                        if (p[1] == '.')        // .. is a separate token
                            goto done;
                        if (isalpha(p[1]) || p[1] == '_' || (p[1] & 0x80))
                            goto done;
                    case 'i':
                    case 'f':
                    case 'F':
                        goto real;
                    case 'B':
                    case 'b':
                        state = STATE_binary0;
                        break;

                    case '0': case '1': case '2': case '3':
                    case '4': case '5': case '6': case '7':
                        state = STATE_octal;
                        break;

                    case '_':
                        state = STATE_octal;
                        p++;
                        continue;

                    case 'L':
                        if (p[1] == 'i')
                            goto real;
                        goto done;

                    default:
                        goto done;
                }
                break;

            case STATE_decimal:         // reading decimal number
                if (!isdigit(c))
                {
                    if (c == '_')               // ignore embedded _
                    {   p++;
                        continue;
                    }
                    if (c == '.' && p[1] != '.')
                    {
                        if (isalpha(p[1]) || p[1] == '_' || (p[1] & 0x80))
                            goto done;
                        goto real;
                    }
                    else if (c == 'i' || c == 'f' || c == 'F' ||
                             c == 'e' || c == 'E')
                    {
            real:       // It's a real number. Back up and rescan as a real
                        p = start;
                        // TODO: REAL NUMBERS!!!
                        assert(0);
                        //return inreal(t);
                    }
                    else if (c == 'L' && p[1] == 'i')
                        goto real;
                    goto done;
                }
                break;

            case STATE_error:           // for error recovery
                if (!isdigit(c))        // scan until non-digit
                    goto done;
                break;

            default:
                assert(0);
        }
        //stringbuffer.writeByte(c);
        stringbuffer += c;
        p++;
    }
done:
    //stringbuffer.writeByte(0);          // terminate string
    if (state == STATE_octale)
        error("Octal digit expected");

    char *end;
    uint64_t n = strtoull(stringbuffer.c_str(), &end, 10);            // unsigned >=64 bit integer type

    
    switch (flags)
    {
        case FLAGS_none:
            // Octal or Hexadecimal constant.
            // First that fits: int, uint, long, ulong
            //
            if (n & 0x8000000000000000LL)
                    result = TOKulongv;
            else if (n & 0xFFFFFFFF00000000LL)
                    result = TOKlongv;
            else if (n & 0x80000000)
                    result = TOKuintv;
            else
                    result = TOKintv;
            break;

        case FLAGS_decimal:
            // First that fits: int, long, long long
            //
            if (n & 0x8000000000000000LL)
            {       error("signed integer overflow");
                    result = TOKulongv;
            }
            else if (n & 0xFFFFFFFF80000000LL)
                    result = TOKlongv;
            else
                    result = TOKintv;
            break;

        case FLAGS_unsigned:
        case FLAGS_decimal | FLAGS_unsigned:
            // First that fits: uint, ulong
            //
            if (n & 0xFFFFFFFF00000000LL)
                    result = TOKulongv;
            else
                    result = TOKuintv;
            break;

        case FLAGS_decimal | FLAGS_long:
            if (n & 0x8000000000000000LL)
            {       error("signed integer overflow");
                    result = TOKulongv;
            }
            else
                    result = TOKlongv;
            break;

        case FLAGS_long:
            if (n & 0x8000000000000000LL)
                    result = TOKulongv;
            else
                    result = TOKlongv;
            break;

        case FLAGS_unsigned | FLAGS_long:
        case FLAGS_decimal | FLAGS_unsigned | FLAGS_long:
            result = TOKulongv;
            break;

        default:
            #ifdef DEBUG
                printf("%x\n",flags);
            #endif
            assert(0);
    }
    t->uns64value = n;
    return result;
}

void Lexer::scan(Token *t) {
    while (1) {
        if (p == endOfFile) {
            t->value = TOKeof; // end of file
            return;
        }
        switch (*p)
        {
            case 0:
            case 0x1A:
                t->value = TOKeof;                      // end of file
                return;
            case ' ':
            case '\t':
            case '\v':
            case '\f':
                p++;
                continue; // skip white space
            case '\r':
            case '\n':
                p++;
                //continue; // skip new line
                t->value = TOKnewline;
                return;
            case '(':
                p++;
                t->value = TOKlparen;
                return;
            case ')':
                p++;
                t->value = TOKrparen;
                return;
            case '{':
                p++;
                t->value = TOKlcurly;
                return;
            case '}':
                p++;
                t->value = TOKrcurly;
                return;
            case ';':
                p++;
                t->value = TOKsemicolon;
                return;
            case '!':
                p++;
                if (*p == '=') {
                    p++;
                    t->value = TOKnotequal;         // !=
                }
                else {
                    t->value = TOKnot;          // !
                }
                return;
            case '=':
                p++;
                if (*p == '=') {
                    p++;
                    t->value = TOKequal;            // ==
                }
                else if (*p == '>') {
                    p++;
                    t->value = TOKgoesto;               // =>
                }
                else {
                    t->value = TOKassign;               // =
                }
                return;
            case '+':
                p++;
                t->value = TOKadd;
                return;
            case '-':
                p++;
                t->value = TOKmin;
                return;
            case '*':
                p++;
                t->value = TOKmul;
                return;
            case '/':
                p++;
                // check if it's a // comment
                if (*p == '/') {
                    while (1) {
                        unsigned char c = *++p;
                        switch (c) {
                            case '\n':
                                break;
                            case '\r':
                                if (*(p+1) == '\n')
                                    p++;
                                break;
                            case 0:
                            case 0x1A:
                                p = endOfFile;
                                t->value = TOKeof;
                                return;
                            default:
                                continue;
                        }
                        break;
                    }
                    p++;
                    continue;
                }
                t->value = TOKdiv;
                return;
            case '0':   case '1':   case '2':   case '3':   case '4':
            case '5':   case '6':   case '7':   case '8':   case '9':
                t->value = number(t);
                return;
            case '\'':
                {
                    p++;
                    unsigned c = *p++;
                    t->uns64value = c;
                    if (*p != '\'') {
                        error("unterminated character constant");
                    }
                    p++;
                    t->value = TOKcharv;
                }

                return;
            case '"':
                {
                    unsigned c;
                    p++;
                    t->stringbuffer.clear();
                    while (1) {
                        c = *p++;
                        switch (c) {
                            case '\\':
                                if (*p == 'n') {
                                    c = 10;
                                    p++;
                                } else if (*p == 'r') {
                                    c = 13;
                                    p++;
                                } else {
                                    error("undefined escape sequence");
                                }
                                break;
                            case '\n':
                                //file->loc.line++;
                                break;
                            case '"':
                                goto LdoneString;
                            case 0:
                            case 0x1A:
                                error("unterminated string constant/unexpected eof");
                                break;
                            default:
                                break;
                        }
                        t->stringbuffer.push_back(c);
                    }
                    LdoneString:
                        t->value = TOKstring;
                }
                return;
            case 'a':   case 'b':   case 'c':   case 'd':   case 'e':
            case 'f':   case 'g':   case 'h':   case 'i':   case 'j':
            case 'k':   case 'l':   case 'm':   case 'n':   case 'o':
            case 'p':   case 'q':   case 'r':   case 's':   case 't':
            case 'u':   case 'v':   case 'w':   case 'x':   case 'y':
            case 'z':
            case 'A':   case 'B':   case 'C':   case 'D':   case 'E':
            case 'F':   case 'G':   case 'H':   case 'I':   case 'J':
            case 'K':   case 'L':   case 'M':   case 'N':   case 'O':
            case 'P':   case 'Q':   case 'R':   case 'S':   case 'T':
            case 'U':   case 'V':   case 'W':   case 'X':   case 'Y':
            case 'Z':
            case '_':
			case_ident:
            {
                std::string ident = "";
				unsigned char c = *p;

				while (1) {
                    ident += c;
					p += 1;
					c = *p;
                    if (isidchar(c)) {
                        continue;
                    }
                    break;
                }
                auto it = gStringTable.find(ident);
                if (it != gStringTable.end()) {
                    t->value = it->second;
                } else {
                    t->identifier = ident;
                    t->value = TOKidentifier;
                }
				return;
			}
            default:
                std::cout << "Unrecognized Character: " << *p << std::endl;
                p++;
                continue;
        }
    }
}

void Lexer::error(const std::string& err) {
    std::cout << "error: " << err << std::endl;
    exit(EXIT_FAILURE);
}



