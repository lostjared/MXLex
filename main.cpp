#include <MXLex/scanner.hpp>
#include <MXLex/token.hpp>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>

int main() {
    try {
        std::istringstream stream("int main()\n{ char c;\nstd::cin >> c;\nstd::cout << c;\n// test comment\n /*test 3} test  1 2 3 */ test");
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
