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
            if (c == '\n')
                ++line;

        } while (CharLayout::get_type(c) == CHAR_TYPE::CHAR_WHITESPACE);

        CHAR_TYPE layout = CharLayout::get_type(c);

        switch (layout) {
        case CHAR_TYPE::CHARACTER:
        case CHAR_TYPE::CHAR_UNDERSCORE: {
            input.putback(c);
            std::string value = get_identifier();
            token.set_token(value, TOKEN_TYPE::IDENTIFIER);
            token.set_line(line);
            ++token_count;
            return TOKEN_TYPE::IDENTIFIER;
        }
        case CHAR_TYPE::DIGIT: {
            input.putback(c);
            auto digits = get_digits();
            token.set_token(digits.first, digits.second);
            token.set_line(line);
            ++token_count;
            return digits.second;
        }
        default:
            if (layout >= CHAR_TYPE::CHAR_LPAREN && layout < CHAR_TYPE::CHAR_WHITESPACE) {
                input.putback(c);
                auto symbol = get_symbols();
                token.set_token(symbol.first, TOKEN_TYPE::OPERATOR, symbol.second);
                token.set_line(line);
                ++token_count;
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

    size_t Scanner::get_token_count() const {
        return token_count;
    }
    size_t Scanner::get_line() const {
        return line;
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
        int nxt = next_ch;
        switch (c) {
        case '+':
            if (nxt == '+') {
                c_type = CHAR_TYPE::CHAR_PLUSPLUS;
                token += static_cast<char>(input.get());
            } else if (nxt == '=') {
                c_type = CHAR_TYPE::CHAR_PLUSEQ;
                token += static_cast<char>(input.get());
            }
            break;
        case '-':
            if (nxt == '-') {
                c_type = CHAR_TYPE::CHAR_MINUSMINUS;
                token += static_cast<char>(input.get());
            } else if (nxt == '=') {
                c_type = CHAR_TYPE::CHAR_MINUSEQ;
                token += static_cast<char>(input.get());
            } else if (nxt == '>') {
                c_type = CHAR_TYPE::CHAR_ARROW;
                token += static_cast<char>(input.get());
                nxt = input.peek();
                if (nxt == '*') {
                    c_type = CHAR_TYPE::CHAR_ARROWASTERISK;
                    token += static_cast<char>(input.get());
                }
            }
            break;
        case '&':
            if (nxt == '&') {
                c_type = CHAR_TYPE::CHAR_ANDAND;
                token += static_cast<char>(input.get());
            } else if (nxt == '=') {
                c_type = CHAR_TYPE::CHAR_ANDEQ;
                token += static_cast<char>(input.get());
            }
            break;
        case '|':
            if (nxt == '|') {
                c_type = CHAR_TYPE::CHAR_OROR;
                token += static_cast<char>(input.get());
            } else if (nxt == '=') {
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
                token += static_cast<char>(input.get());
                nxt = input.peek();
                c_type = CHAR_TYPE::CHAR_LESSTHANEQ;
                if (nxt == '>') {
                    token += static_cast<char>(input.get());
                    c_type = CHAR_TYPE::CHAR_SPACESHIP;
                }
            } else if (nxt == '<') {
                c_type = CHAR_TYPE::CHAR_SHIFTLEFT;
                token += static_cast<char>(input.get());
                nxt = input.peek();
                if (nxt == '=') {
                    c_type = CHAR_TYPE::CHAR_SHIFTLEFTEQ;
                    token += static_cast<char>(input.get());
                }
            }
            break;
        case '>':
            if (nxt == '=') {
                c_type = CHAR_TYPE::CHAR_GREATEREQ;
                token += static_cast<char>(input.get());
            } else if (nxt == '>') {
                c_type = CHAR_TYPE::CHAR_SHIFTRIGHT;
                token += static_cast<char>(input.get());
                nxt = input.peek();
                if (nxt == '=') {
                    c_type = CHAR_TYPE::CHAR_SHIFTRIGHTEQ;
                    token += static_cast<char>(input.get());
                }
            }
            break;
        case '*':
            if (nxt == '=') {
                c_type = CHAR_TYPE::CHAR_MULEQ;
                token += static_cast<char>(input.get());
            }
            break;

        case '/':
            if (nxt == '=') {
                c_type = CHAR_TYPE::CHAR_DIVEQ;
                token += static_cast<char>(input.get());
            }
            break;
        case '%':
            if (nxt == '=') {
                c_type = CHAR_TYPE::CHAR_MODEQ;
                token += static_cast<char>(input.get());
            }
            break;
        case ':':
            if (nxt == ':') {
                c_type = CHAR_TYPE::CHAR_COLONCOLON;
                token += static_cast<char>(input.get());
            }
            break;
        case '^':
            if (nxt == '=') {
                c_type = CHAR_TYPE::CHAR_XOREQ;
                token += static_cast<char>(input.get());
            }
            break;

        case '.':
            if (nxt == '*') {
                c_type = CHAR_TYPE::CHAR_PERIODASTERISK;
                token += static_cast<char>(input.get());
            } else if (nxt == '.') {
                char ch = static_cast<char>(input.get());
                int next = input.peek();
                if (next == '.') {
                    token += ch;
                    token += static_cast<char>(input.get());
                    c_type = CHAR_TYPE::CHAR_DOTDOTDOT;
                } else {
                    input.putback(ch);
                }
            }
            break;
        }
        return {token, c_type};
    }
} // namespace mx
