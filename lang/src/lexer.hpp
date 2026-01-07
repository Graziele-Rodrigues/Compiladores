/* Graziele de Cassia Rodrigues 21.1.8120 */
#ifndef LEXER_HPP
#define LEXER_HPP

#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif

class Lexer : public yyFlexLexer {
public:
    Lexer(std::istream* in) : yyFlexLexer(in) {}
    int yylex();
};

#endif
