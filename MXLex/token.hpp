#pragma once
#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

namespace mx {

    enum class CHAR_TYPE { CHAR_NULL, CHARACTER, DIGIT, SYMBOL, CHAR_LPAREN, CHAR_RPAREN, CHAR_LBRACKET, CHAR_RBRACKET, CHAR_LBRACE, CHAR_RBRACE, CHAR_PLUS, CHAR_MINUS, CHAR_ASTERISK, CHAR_SLASH, CHAR_PERCENT, CHAR_EQUAL, CHAR_LESS, CHAR_GREATER, CHAR_EXCLAMATION, CHAR_AMPERSAND, CHAR_PIPE, CHAR_CARET, CHAR_TILDE, CHAR_QUESTION, CHAR_COLON, CHAR_SEMICOLON, CHAR_COMMA, CHAR_PERIOD, CHAR_SINGLE_QUOTE, CHAR_DOUBLE_QUOTE, CHAR_BACKSLASH, CHAR_HASH, CHAR_DOLLAR, CHAR_AT, CHAR_PLUSEQ, CHAR_PLUSPLUS, CHAR_MINUSEQ, CHAR_MINUSMINUS, CHAR_NOT_EQ, CHAR_LESSTHANEQ, CHAR_SHIFTRIGHT, CHAR_GREATEREQ, CHAR_SHIFTLEFT, CHAR_OROR, CHAR_OREQ, CHAR_ANDEQ, CHAR_ANDAND, CHAR_EQEQ, CHAR_SHIFTLEFTEQ, CHAR_SHIFTRIGHTEQ, CHAR_MULEQ, CHAR_MODEQ, CHAR_DIVEQ, CHAR_XOREQ, CHAR_ARROW, CHAR_COLONCOLON, CHAR_DOTDOTDOT, CHAR_SPACESHIP, CHAR_PERIODASTERISK, CHAR_ARROWASTERISK, CHAR_WHITESPACE, CHAR_UNDERSCORE };

    enum class TOKEN_TYPE { TOKEN_NULL, TOKEN_ERROR, IDENTIFIER, INTEGER_VALUE, FLOAT_VALUE, OPERATOR, STRING_VALUE };

    class Token {
      public:
        Token() = default;
        Token(const Token &token) = default;
        Token(Token &&token) = default;
        Token &operator=(const Token &token) = default;
        Token &operator=(Token &&token) = default;
        void set_token(std::string_view token, TOKEN_TYPE value_token_type);
        void set_token(std::string_view token, TOKEN_TYPE value_token_type, CHAR_TYPE c_type);
        [[nodiscard]] TOKEN_TYPE get_type() const;
        [[nodiscard]] const std::string &get_token() const;
        [[nodiscard]] CHAR_TYPE get_char_type() const;
        void clear();
        friend std::ostream &operator<<(std::ostream &out, const Token &token);
	void set_line(const size_t &value);
	size_t get_line() const;
      private:
        std::string token_value{};
        TOKEN_TYPE token_type{TOKEN_TYPE::TOKEN_NULL};
        CHAR_TYPE c_type{CHAR_TYPE::CHAR_NULL};
	size_t line = 0;
    };

    class CharLayout {
      public:
        static constexpr std::size_t SIZE = 256;
        static constexpr CHAR_TYPE type_from_char(unsigned char c) {
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
                return CHAR_TYPE::CHARACTER;

            if (c >= '0' && c <= '9')
                return CHAR_TYPE::DIGIT;
            switch (c) {
            case '(':
                return CHAR_TYPE::CHAR_LPAREN;
            case ')':
                return CHAR_TYPE::CHAR_RPAREN;
            case '[':
                return CHAR_TYPE::CHAR_LBRACKET;
            case ']':
                return CHAR_TYPE::CHAR_RBRACKET;
            case '{':
                return CHAR_TYPE::CHAR_LBRACE;
            case '}':
                return CHAR_TYPE::CHAR_RBRACE;
            case '+':
                return CHAR_TYPE::CHAR_PLUS;
            case '-':
                return CHAR_TYPE::CHAR_MINUS;
            case '*':
                return CHAR_TYPE::CHAR_ASTERISK;
            case '/':
                return CHAR_TYPE::CHAR_SLASH;
            case '%':
                return CHAR_TYPE::CHAR_PERCENT;
            case '=':
                return CHAR_TYPE::CHAR_EQUAL;
            case '<':
                return CHAR_TYPE::CHAR_LESS;
            case '>':
                return CHAR_TYPE::CHAR_GREATER;
            case '!':
                return CHAR_TYPE::CHAR_EXCLAMATION;
            case '&':
                return CHAR_TYPE::CHAR_AMPERSAND;
            case '|':
                return CHAR_TYPE::CHAR_PIPE;
            case '^':
                return CHAR_TYPE::CHAR_CARET;
            case '~':
                return CHAR_TYPE::CHAR_TILDE;
            case '?':
                return CHAR_TYPE::CHAR_QUESTION;
            case ':':
                return CHAR_TYPE::CHAR_COLON;
            case ';':
                return CHAR_TYPE::CHAR_SEMICOLON;
            case ',':
                return CHAR_TYPE::CHAR_COMMA;
            case '.':
                return CHAR_TYPE::CHAR_PERIOD;
            case '\'':
                return CHAR_TYPE::CHAR_SINGLE_QUOTE;
            case '"':
                return CHAR_TYPE::CHAR_DOUBLE_QUOTE;
            case '\\':
                return CHAR_TYPE::CHAR_BACKSLASH;
            case '#':
                return CHAR_TYPE::CHAR_HASH;
            case '$':
                return CHAR_TYPE::CHAR_DOLLAR;
            case '@':
                return CHAR_TYPE::CHAR_AT;
            case '_':
                return CHAR_TYPE::CHAR_UNDERSCORE;
            case ' ':
                return CHAR_TYPE::CHAR_WHITESPACE;
            case '\t':
                return CHAR_TYPE::CHAR_WHITESPACE;
            case '\n':
                return CHAR_TYPE::CHAR_WHITESPACE;
            case '\r':
                return CHAR_TYPE::CHAR_WHITESPACE;
            }

            return CHAR_TYPE::CHAR_NULL;
        }
        static constexpr std::array<CHAR_TYPE, SIZE> fill_chars() {
            std::array<CHAR_TYPE, SIZE> ch_array{};
            for (std::size_t i = 0; i < SIZE; ++i) {
                ch_array[i] = type_from_char(static_cast<unsigned char>(i));
            }
            return ch_array;
        }

        static constexpr CHAR_TYPE get_type(char c) { return characters[static_cast<unsigned char>(c)]; }
        static const std::array<CHAR_TYPE, SIZE> characters;
    };

    inline constexpr std::array<CHAR_TYPE, CharLayout::SIZE> CharLayout::characters = CharLayout::fill_chars();

} // namespace mx

// namespace mx
