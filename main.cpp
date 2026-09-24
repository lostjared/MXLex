#include <MXLex/scanner.hpp>
#include <MXLex/token.hpp>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Requires on argument filename\n" << argv[0] << " input.\n";
        return EXIT_FAILURE;
    }
    try {
        std::fstream file;
        file.open(argv[1], std::ios::in);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << argv[1] << "\n";
            return EXIT_FAILURE;
        }
        mx::Scanner scanner(file);
        mx::Token token;
        mx::TOKEN_TYPE token_type;
        while ((token_type = scanner.lex(token)) != mx::TOKEN_TYPE::TOKEN_NULL) {
            if (token_type == mx::TOKEN_TYPE::TOKEN_ERROR) {
                std::cerr << token << "\n";
                break;
            }
            std::cout << "[" << token.get_line() << "] -> " << token << "\n";
        }
    } catch (const mx::ScannerError &e) {
        std::cerr << "Token Error Exception: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
