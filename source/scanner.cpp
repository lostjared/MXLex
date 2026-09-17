#include "MXLex/scanner.hpp"
#include <format>
#include <iostream>
#include <string>

namespace mx {

    Scanner::Scanner(std::istream &in) : input(in) {}

    [[nodiscard]] TOKEN_TYPE Scanner::lex(Token &token) {
        token.clear();
        char c;
        do {
            if (!input.get(c))
                return TOKEN_TYPE::TOKEN_NULL;

        } while (CharLayout::get_type(c) == CHAR_TYPE::CHAR_WHITESPACE);

        CHAR_TYPE layout = CharLayout::get_type(c);

        switch (layout) {
        case CHAR_TYPE::CHARACTER:
        case CHAR_TYPE::CHAR_UNDERSCORE: {
            input.putback(c);
            std::string value = get_identifier();
            token.set_token(value, TOKEN_TYPE::IDENTIFIER);
            return TOKEN_TYPE::IDENTIFIER;
        }
        case CHAR_TYPE::DIGIT: {
            input.putback(c);
            auto digits = get_digits();
            token.set_token(digits.first, digits.second);
            return digits.second;
        }
        default:
            if (layout >= CHAR_TYPE::CHAR_LPAREN && layout < CHAR_TYPE::CHAR_WHITESPACE) {
                input.putback(c);
                auto symbol = get_symbols();
                token.set_token(symbol.first, TOKEN_TYPE::OPERATOR, symbol.second);
                return TOKEN_TYPE::OPERATOR;
            }
            token.set_token(std::string(1, c), TOKEN_TYPE::TOKEN_ERROR);
            return TOKEN_TYPE::TOKEN_ERROR;
        }
        return TOKEN_TYPE::TOKEN_ERROR;
    }

    [[nodiscard]] std::string Scanner::get_identifier() {
        std::string token;
        char c;
        while (input.get(c)) {
            CHAR_TYPE ch_type = CharLayout::get_type(c);
            if (ch_type == CHAR_TYPE::CHARACTER || ch_type == CHAR_TYPE::CHAR_UNDERSCORE || ch_type == CHAR_TYPE::DIGIT) {
                token += c;
            } else {
                input.putback(c);
                break;
            }
        }
        return token;
    }

    [[nodiscard]] std::pair<std::string, TOKEN_TYPE> Scanner::get_digits() {
        std::string token;
        char c;
        TOKEN_TYPE t_type = TOKEN_TYPE::INTEGER_VALUE;
        bool float_val = false;
        bool error_val = false;

        while (input.get(c)) {
            CHAR_TYPE ch_type = CharLayout::get_type(c);
            if (ch_type != CHAR_TYPE::DIGIT && ch_type != CHAR_TYPE::CHAR_PERIOD) {
                input.putback(c);
                break;
            }
            if (ch_type == CHAR_TYPE::DIGIT) {
                token += c;
            } else if (ch_type == CHAR_TYPE::CHAR_PERIOD) {
                if (!float_val) {
                    token += c;
                    float_val = true;
                    t_type = TOKEN_TYPE::FLOAT_VALUE;
                } else {
                    error_val = true;
                    token += c;
                }
            }
        }

        if (error_val) {
            throw ScannerError(std::format("Error: Float value: {} has too many periods", token));
        }

        return {token, t_type};
    }

    [[nodiscard]] std::pair<std::string, CHAR_TYPE> Scanner::get_symbols() {
        char c;
        std::string token;
        if (!input.get(c))
            return {token, CHAR_TYPE::CHAR_NULL};

        token += c;
        CHAR_TYPE layout = CharLayout::get_type(c);
        int next_ch = input.peek();
        if (next_ch == std::char_traits<char>::eof())
            return {token, layout};

        CHAR_TYPE c_type = layout;

        char nxt = static_cast<char>(next_ch);
        switch (c) {
        case '+':
            if (nxt == '+') {
                c_type = CHAR_TYPE::CHAR_PLUSPLUS;
                token += static_cast<char>(input.get());
            }
            if (nxt == '=') {
                c_type = CHAR_TYPE::CHAR_PLUSEQ;
                token += static_cast<char>(input.get());
            }
            break;
        case '-':
            if (nxt == '-') {
                c_type = CHAR_TYPE::CHAR_MINUSMINUS;
                token += static_cast<char>(input.get());
            }
            if (nxt == '=') {
                c_type = CHAR_TYPE::CHAR_MINUSEQ;
                token += static_cast<char>(input.get());
            }
            break;
        case '&':
            if (nxt == '&') {
                c_type = CHAR_TYPE::CHAR_ANDAND;
                token += static_cast<char>(input.get());
            }
            if (nxt == '=') {
                c_type = CHAR_TYPE::CHAR_ANDEQ;
                ;
                token += static_cast<char>(input.get());
            }
            break;
        case '|':
            if (nxt == '|') {
                c_type = CHAR_TYPE::CHAR_OROR;
                token += static_cast<char>(input.get());
            }
            if (nxt == '=') {
                c_type = CHAR_TYPE::CHAR_OREQ;
                token += static_cast<char>(input.get());
            }
            break;
        case '=':
            if (nxt == '=') {
                c_type = CHAR_TYPE::CHAR_EQEQ;
                token += static_cast<char>(input.get());
            }
            break;
        case '!':
            if (nxt == '=') {
                token += static_cast<char>(input.get());
                c_type = CHAR_TYPE::CHAR_NOT_EQ;
            }
            break;
        case '<':
            if (nxt == '=') {
                c_type = CHAR_TYPE::CHAR_LESSTHANEQ;
                token += static_cast<char>(input.get());
            }
            if (nxt == '<') {
                c_type = CHAR_TYPE::CHAR_STREAMOUT;
                ;
                token += static_cast<char>(input.get());
            }
            break;
        case '>':
            if (nxt == '=') {
                c_type = CHAR_TYPE::CHAR_GREATEREQ;
                token += static_cast<char>(input.get());
            }
            if (nxt == '>') {
                c_type = CHAR_TYPE::CHAR_STREAMIN;
                ;
                token += static_cast<char>(input.get());
            }
            break;
        }
        return {token, c_type};
    }

} // namespace mx
