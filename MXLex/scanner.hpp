#pragma once
#include <MXLex/token.hpp>
#include <iostream>
#include <utility>

namespace mx {

    class ScannerError : public std::runtime_error {
      public:
        ScannerError(const std::string &text) : std::runtime_error(text) {};
    };

    class Scanner {
      public:
        Scanner(std::istream &input);
        [[nodiscard]] TOKEN_TYPE lex(Token &token);

      private:
        std::istream &input;
        [[nodiscard]] std::string get_identifier();
        [[nodiscard]] std::pair<std::string, TOKEN_TYPE> get_digits();
        [[nodiscard]] std::pair<std::string, CHAR_TYPE> get_symbols();
    };
} // namespace mx
