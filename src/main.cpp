/* Arquivo: main.cpp
 * Autor: Graziele Cassia Rodrigues
 * Matricula: 21.1.8120
 *
 * Descrição:
 * Função principal do compilador.
 */

#include <iostream>
#include <fstream>
#include <string>

#include "lexer.hpp"
#include "parser.hpp"
#include "ast/include/ast.hpp"
#include "ast/include/interpreter.hpp"
#include "checker/include/typechecker.hpp"   // (removi duplicado)

static void print_help()
{
    std::cout <<
        "Uso:\n"
        "  ./compiler <arquivo>\n"
        "  ./compiler -syn <arquivo>\n"
        "  ./compiler -i   <arquivo>\n"
        "  ./compiler -ty  <arquivo>\n"
        "  ./compiler -v\n";
}

int main(int argc, char** argv)
{
    std::string option;
    std::string filename;

    if (argc == 1) {
        print_help();
        return 0;
    }

    if (argc == 2) {
        if (std::string(argv[1]) == "-v") {
            std::cout << "LangV2 - 2025/2 - v:0.1.2\n";
            std::cout << "21.1.8120\n";
            std::cout << "Graziele de Cassia Rodrigues\n";
            return 0;
        }

        option = "-syn";
        filename = argv[1];
    }

    if (argc == 3) {
        option = argv[1];
        filename = argv[2];
    }

    if (argc > 3) {
        print_help();
        return 1;
    }

    // valida opção
    if (option != "-syn" && option != "-i" && option != "-ty") {
        print_help();
        return 1;
    }

    std::ifstream input(filename);
    if (!input) {
        std::cerr << "Erro ao abrir arquivo: " << filename << std::endl;
        return 1;
    }

    Lexer lexer(&input);
    lexer.debug_tokens = (option == "-syn"); // só lista tokens no -syn

    std::shared_ptr<Program> ast;
    yy::Parser parser(lexer, ast);

    int result = parser.parse();

    // ---------------- -syn ----------------
    if (option == "-syn") {
        if (result == 0) std::cout << "accepted\n";
        else             std::cout << "rejected\n";
        return result;
    }

    // qualquer modo diferente de -syn exige parsing OK
    if (result != 0 || !ast) {
        std::cout << "rejected\n";
        return 1;
    }

    // ---------------- -ty ----------------
    if (option == "-ty") {
        try {
            TypeChecker tc;
            tc.checkProgram(*ast);
            std::cout << "well typed\n";
            return 0;
        } catch (const TypeError& e) {
            std::cout << "no typed\n";
            std::cerr << "Erro (" << e.pos.line << "," << e.pos.col << "): " << e.what() << "\n";
            return 1;
        } catch (const std::exception& e) {
            std::cout << "no typed\n";
            std::cerr << "Erro: " << e.what() << "\n";
            return 1;
        }
    }

    // ---------------- -i ----------------
    if (option == "-i") {
        Interpreter itp;
        itp.loadProgram(*ast);
        itp.runMain();
        return 0;
    }
   

    print_help();
    return 1;
}