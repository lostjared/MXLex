#include "MXLex/scanner.hpp"
#include <regex>
#include <string>

namespace mx {
    namespace {
        constexpr int END = std::char_traits<char>::eof();

        bool digit(int c) { return c >= '0' && c <= '9'; }
        bool letter(int c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
    } // namespace

    Scanner::Scanner(std::istream &in) : input(in) {}

    int Scanner::peek(size_t offset) {
        while (lookahead.size() <= offset) {
            int c = input.get();
            if (c == END)
                return END;
            if (c == '\\' && (input.peek() == '\n' || input.peek() == '\r')) {
                int newline = input.get();
                if (newline == '\r' && input.peek() == '\n')
                    input.get();
                ++input_line;
                input_column = 1;
                continue;
            }
            if (c == '\r') {
                if (input.peek() == '\n')
                    input.get();
                c = '\n';
            }
            lookahead.push_back({static_cast<char>(c), input_line, input_column});
            if (c == '\n') {
                ++input_line;
                input_column = 1;
            } else {
                ++input_column;
            }
        }
        return static_cast<unsigned char>(lookahead[offset].value);
    }

    int Scanner::take() {
        int c = peek();
        if (c != END) {
            line = lookahead.front().line + (c == '\n' ? 1 : 0);
            lookahead.pop_front();
        }
        return c;
    }

    [[nodiscard]] TOKEN_TYPE Scanner::lex(Token &token) {
        token.clear();
        for (;;) {
            int c = peek();
            if (c == END) {
                line = input_line;
                return TOKEN_TYPE::TOKEN_NULL;
            }
            if (CharLayout::get_type(static_cast<char>(c)) == CHAR_TYPE::CHAR_WHITESPACE) {
                take();
                continue;
            }
            if (c == '/' && peek(1) == '/') {
                while (peek() != END && take() != '\n') {
                }
                continue;
            }
            if (c == '/' && peek(1) == '*') {
                size_t start = lookahead.front().line;
                take();
                take();
                while (peek() != END && !(peek() == '*' && peek(1) == '/'))
                    take();
                if (peek() == END)
                    throw ScannerError("Unterminated comment on line " + std::to_string(start));
                take();
                take();
                continue;
            }
            break;
        }

        size_t start = lookahead.front().line;
        size_t start_column = lookahead.front().column;
        char c = static_cast<char>(take());
        if (letter(static_cast<unsigned char>(c))) {
            std::string value = get_identifier(c);
            if ((value == "L" || value == "u" || value == "U" || value == "u8") && (peek() == '"' || peek() == '\'')) {
                char quote = static_cast<char>(take());
                token.set_token(get_literal(value, quote), quote == '"' ? TOKEN_TYPE::STRING_VALUE : TOKEN_TYPE::CHARACTER_VALUE);
            } else {
                token.set_token(value, TOKEN_TYPE::IDENTIFIER);
            }
        } else if (digit(c) || (c == '.' && digit(peek()))) {
            auto value = get_digits(c);
            token.set_token(value.first, value.second);
        } else if (c == '"' || c == '\'') {
            token.set_token(get_literal("", c), c == '"' ? TOKEN_TYPE::STRING_VALUE : TOKEN_TYPE::CHARACTER_VALUE);
        } else {
            auto layout = CharLayout::get_type(c);
            if (layout >= CHAR_TYPE::CHAR_LPAREN && layout <= CHAR_TYPE::CHAR_AT) {
                auto symbol = get_symbols(c);
                token.set_token(symbol.first, TOKEN_TYPE::OPERATOR, symbol.second);
            } else {
                token.set_token(std::string(1, c), TOKEN_TYPE::TOKEN_ERROR);
            }
        }
        token.set_line(start);
        token.set_column(start_column);
        ++token_count;
        return token.get_type();
    }

    [[nodiscard]] std::string Scanner::get_identifier(char c) {
        std::string value(1, c);
        while (letter(peek()) || digit(peek()))
            value += static_cast<char>(take());
        return value;
    }

    std::string Scanner::get_literal(std::string prefix, char quote) {
        size_t start = line;
        std::string value = std::move(prefix);
        value += quote;
        bool has_content = false;
        for (;;) {
            int c = take();
            if (c == END || c == '\n' || c == '\r')
                throw ScannerError("Unterminated literal on line " + std::to_string(start));
            value += static_cast<char>(c);
            if (c == quote) {
                if (quote == '\'' && !has_content)
                    throw ScannerError("Empty character literal on line " + std::to_string(start));
                return value;
            }
            if (c == '\\') {
                c = take();
                if (c == END || c == '\n' || c == '\r')
                    throw ScannerError("Unterminated escape on line " + std::to_string(start));
                value += static_cast<char>(c);
            }
            has_content = true;
        }
    }

    size_t Scanner::get_token_count() const { return token_count; }
    size_t Scanner::get_line() const { return line; }

    [[nodiscard]] std::pair<std::string, TOKEN_TYPE> Scanner::get_digits(char c) {
        std::string value(1, c);
        while (letter(peek()) || digit(peek()) || peek() == '.' || ((peek() == '+' || peek() == '-') && (value.back() == 'e' || value.back() == 'E' || value.back() == 'p' || value.back() == 'P'))) {
            value += static_cast<char>(take());
        }

        static const std::regex integer(R"((0[xX][0-9a-fA-F]+|0[bB][01]+|0[0-7]*|[1-9][0-9]*)([uU](ll|LL|[lL])?|(ll|LL|[lL])[uU]?)?)");
        static const std::regex floating(R"(((([0-9]+\.[0-9]*|\.[0-9]+)([eE][+-]?[0-9]+)?|[0-9]+[eE][+-]?[0-9]+)|0[xX]([0-9a-fA-F]+(\.[0-9a-fA-F]*)?|\.[0-9a-fA-F]+)[pP][+-]?[0-9]+)[fFlL]?)");
        if (std::regex_match(value, integer))
            return {value, TOKEN_TYPE::INTEGER_VALUE};
        if (std::regex_match(value, floating))
            return {value, TOKEN_TYPE::FLOAT_VALUE};
        throw ScannerError("Invalid numeric literal '" + value + "' on line " + std::to_string(line));
    }

    [[nodiscard]] std::pair<std::string, CHAR_TYPE> Scanner::get_symbols(char c) {
        using C = CHAR_TYPE;
        struct Symbol {
            std::string_view text;
            C type;
        };
        // Longest matches first, including the C alternative punctuator spellings.
        static constexpr Symbol symbols[] = {
            {"%:%:", C::CHAR_HASHHASH}, {">>=", C::CHAR_SHIFTRIGHTEQ}, {"<<=", C::CHAR_SHIFTLEFTEQ}, {"...", C::CHAR_DOTDOTDOT}, {"->*", C::CHAR_ARROWASTERISK}, {"<=>", C::CHAR_SPACESHIP}, {"++", C::CHAR_PLUSPLUS}, {"+=", C::CHAR_PLUSEQ}, {"--", C::CHAR_MINUSMINUS}, {"-=", C::CHAR_MINUSEQ}, {"->", C::CHAR_ARROW}, {"&&", C::CHAR_ANDAND}, {"&=", C::CHAR_ANDEQ}, {"||", C::CHAR_OROR}, {"|=", C::CHAR_OREQ}, {"==", C::CHAR_EQEQ}, {"!=", C::CHAR_NOT_EQ}, {"<=", C::CHAR_LESSTHANEQ}, {"<<", C::CHAR_SHIFTLEFT}, {">=", C::CHAR_GREATEREQ}, {">>", C::CHAR_SHIFTRIGHT}, {"*=", C::CHAR_MULEQ}, {"/=", C::CHAR_DIVEQ}, {"%=", C::CHAR_MODEQ}, {"^=", C::CHAR_XOREQ}, {"::", C::CHAR_COLONCOLON}, {".*", C::CHAR_PERIODASTERISK}, {"##", C::CHAR_HASHHASH}, {"<:", C::CHAR_LBRACKET}, {":>", C::CHAR_RBRACKET}, {"<%", C::CHAR_LBRACE}, {"%>", C::CHAR_RBRACE}, {"%:", C::CHAR_HASH},
        };
        for (const auto &symbol : symbols) {
            if (symbol.text.front() != c)
                continue;
            bool matches = true;
            for (size_t i = 1; i < symbol.text.size(); ++i) {
                if (peek(i - 1) != symbol.text[i]) {
                    matches = false;
                    break;
                }
            }
            if (matches) {
                for (size_t i = 1; i < symbol.text.size(); ++i)
                    take();
                return {std::string(symbol.text), symbol.type};
            }
        }
        return {std::string(1, c), CharLayout::get_type(c)};
    }
} // namespace mx
