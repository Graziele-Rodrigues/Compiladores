/*
Compilacao
flex --c++ lexico.l
g++ -std=c++17 main.cpp lex.yy.cc -o analisador

Execucao
./analisador entrada.txt saida.txt

Usar makefile
make
make run
make clean

*/
#include "parser.h"

int main()
{
	Parser parser;
	parser.Start();
}

/*int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Uso: ./analisador <arquivo_entrada> <arquivo_saida>" << endl;
        return EXIT_FAILURE;
    }

    ifstream input(argv[1]);
    if (!input.is_open()) {
        cerr << "Erro: não foi possível abrir o arquivo de entrada '" << argv[1] << "'" << endl;
        return EXIT_FAILURE;
    }

    ofstream output(argv[2]);
    if (!output.is_open()) {
        cerr << "Erro: não foi possível criar o arquivo de saída '" << argv[2] << "'" << endl;
        return EXIT_FAILURE;
    }

    // Cria o analisador léxico e executa
    yyFlexLexer lexer(&input, &output);
    lexer.yylex();

    cout << "Análise concluída. Resultado salvo em '" << argv[2] << "'." << endl;
    return EXIT_SUCCESS;
}*/

