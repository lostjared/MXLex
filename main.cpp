#include <MXLex/scanner.hpp>
#include <MXLex/token.hpp>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>

int main() {
    try {
        std::istringstream stream("0 1 42 123 000123 0.0 1.0 123.456 123. .5 .0001 1e0 1E0 1e1 1E10 1e+10 1e-10 1.5e10 1.5E10 1.5e+10 1.5e-10 .5e2 .5E+2 .5e-2 5.e2 5.E+2 5.e-2 1e000 1e+000 1e-000 0e0 0.0e0 123e456 1e2+3 1e2-3 1e2*3 1e2/3 .5e.5");
        mx::Scanner scanner(stream);
        mx::Token token;
        mx::TOKEN_TYPE token_type;
        while ((token_type = scanner.lex(token)) != mx::TOKEN_TYPE::TOKEN_NULL) {
            if (token_type == mx::TOKEN_TYPE::TOKEN_ERROR) {
                std::cerr << token << "\n";
                break;
            }
            std::cout <<"[" <<  token <<"]\t->\tat line: " << token.get_line() << "\n";
        }
    } catch (const mx::ScannerError &e) {
        std::cerr << "Token Error Exception: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
