/* Graziele de Cassia Rodrigues
 * Matrícula: 21.1.8120
 */

#include <iostream>
#include <fstream>
#include <string>

#include "lexer.hpp"
#include "parser.hpp"

void print_help()
{
    std::cout <<
        "Uso:\n"
        "  ./compiler <arquivo>\n"
        "  ./compiler -syn <arquivo>\n"
        "  ./compiler -i <arquivo>\n"
        "  ./compiler -v\n";
}

int main(int argc, char** argv)
{
    std::string option;
    std::string filename;

    if (argc == 1) { // quando não há argumentos
        print_help();
        return 0;
    }

    if (argc == 2) { // quando há um argumento
        if (std::string(argv[1]) == "-v") {
            std::cout << "LangV2 - 2025/2 - v:0.1.2\n";
            std::cout << "21.1.8120\n";
            std::cout << "Graziele de Cassia Rodrigues\n";
            return 0;
        }

        option = "-syn";
        filename = argv[1];
    }

    if (argc == 3) { // quando há dois argumentos
        option = argv[1];
        filename = argv[2];
    }

    if (argc > 3) { // quando há mais de dois argumentos
        print_help();
        return 1;
    }

    std::ifstream input(filename);
    if (!input) {
        std::cerr << "Erro ao abrir arquivo: " << filename << std::endl;
        return 1;
    }

    Lexer lexer(&input);
    yy::Parser parser(lexer);

    int result = parser.parse();

    /* ---------------- -syn ---------------- */
    if (option == "-syn") {
        if (result == 0)
            std::cout << "accepted\n";
        else
            std::cout << "rejected\n";
        return result;
    }

    /* ---------------- -i ---------------- */
    if (option == "-i") {
        if (result != 0) {
            std::cout << "rejected\n";
            return result;
        }

        /* TODO:
         * - construir AST
         * - executar interpretador
         */
        std::cout << "accepted\n";
        return 0;
    }

    print_help();
    return 1;
}
