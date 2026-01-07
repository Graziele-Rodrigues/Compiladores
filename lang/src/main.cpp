/* Graziele de Cassia Rodrigues 21.1.8120 */

#include <iostream>
#include <fstream>
#include "lexer.hpp"
#include "parser.hpp"

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Uso: ./compiler arquivo.txt\n";
        return 1;
    }

    std::ifstream input(argv[1]);
    if (!input) {
        std::cerr << "Erro ao abrir arquivo\n";
        return 1;
    }

    Lexer lexer(&input);
    yy::Parser parser(lexer);

    int result = parser.parse();

    if (result == 0)
        std::cout << "Análise sintática concluída com sucesso.\n";
    else
        std::cout << "Erro durante a análise sintática.\n";

    return result;
}
