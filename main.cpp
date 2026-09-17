#include <MXLex/scanner.hpp>
#include <MXLex/token.hpp>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>

int main() {
    try {
        std::istringstream stream("a++ && b-- || c == d && e != f && x <= y && x >= y && a << 2 && b >> 1");
        mx::Scanner scanner(stream);
        mx::Token token;
        mx::TOKEN_TYPE token_type;
        while ((token_type = scanner.lex(token)) != mx::TOKEN_TYPE::TOKEN_NULL) {
            if (token_type == mx::TOKEN_TYPE::TOKEN_ERROR) {
                std::cerr << token << "\n";
                break;
            }
            std::cout << token << "\n";
        }
    } catch (const mx::ScannerError &e) {
        std::cerr << "Token Error Exception: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
    ;
}
