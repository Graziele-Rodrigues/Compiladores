#ifndef LEXER_HPP
#define LEXER_HPP

#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif

class Lexer : public yyFlexLexer {
public:
    explicit Lexer(std::istream* in)
        : yyFlexLexer(in), debug_tokens(false) {}

    int yylex(void* yylval);

    bool debug_tokens;
};

#endif
