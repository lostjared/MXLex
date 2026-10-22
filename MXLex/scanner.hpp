#pragma once
#include <MXLex/token.hpp>
#include <deque>
#include <iostream>
#include <stdexcept>
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
        size_t get_token_count() const;
        size_t get_line() const;

      private:
        std::istream &input;
        struct SourceCharacter {
            char value;
            size_t line;
            size_t column;
        };
        std::deque<SourceCharacter> lookahead;
        size_t input_line = 1;
        size_t input_column = 1;
        int peek(size_t offset = 0);
        int take();
        std::string get_literal(std::string prefix, char quote);
        [[nodiscard]] std::string get_identifier(char c);
        [[nodiscard]] std::pair<std::string, TOKEN_TYPE> get_digits(char c);
        [[nodiscard]] std::pair<std::string, CHAR_TYPE> get_symbols(char c);
        size_t token_count = 0;
        size_t line = 1;
    };
} // namespace mx
