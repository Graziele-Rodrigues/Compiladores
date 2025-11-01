#include "parser.h"
#include "tokens.h"
#include <iostream>
#include <fstream>

using namespace std;

// Variáveis definidas no lexer.l
extern int yylineno;
extern int yycolumn;

void Parser::Start()
{
    
    int tokenCount = 0;

    // enquanto não atingir o fim da entrada
    while ((lookahead = scanner.yylex()) != 0)
    {
        tokenCount++;
        string tokenText = scanner.YYText();

        // calcula a posição inicial do token (coluna - tamanho + 1)
        int startColumn = yycolumn - tokenText.size() + 1;
    }
}
