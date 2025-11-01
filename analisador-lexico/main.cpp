#include "parser.h"
/*
Compilacao
flex --c++ lexico.l
g++ -std=c++17 main.cpp lex.yy.cc -o analisador

Execucao
./analisador entrada.txt saida.txt

Execucao se tiver compilado usado make
Build/main < teste1.txt

Identificar erro no lexer.l
head -30 lexer.l | cat -A^C
*/

int main()
{
	Parser parser;
	parser.Start();
}