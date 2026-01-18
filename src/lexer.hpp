/* Arquivo: lexer.hpp
 * Autor: Graziele Cassia Rodrigues
 * Matricula: 21.1.8120
 *
 * Descrição:
 * Definição da classe Lexer.
 */

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
