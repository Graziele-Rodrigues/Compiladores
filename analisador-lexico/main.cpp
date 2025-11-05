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
#include "parser.h"

/* 
Trabalho de Compiladores - Analisador Léxico
nome: Graziele de Cassia Rodrigues 
matricula: 21.1.8120
data: 11/2025
*/

int main()
{
	Parser parser;
	parser.Start();
}