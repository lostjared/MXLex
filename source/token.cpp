#include "MXLex/token.hpp"

namespace mx {

    void Token::set_token(std::string_view token, TOKEN_TYPE value_token_type) {
        token_value = std::string(token);
        token_type = value_token_type;
    }
    void Token::set_token(std::string_view token, TOKEN_TYPE value_token_type, CHAR_TYPE ctype) {
        token_value = std::string(token);
        token_type = value_token_type;
        c_type = ctype;
    }

    [[nodiscard]] TOKEN_TYPE Token::get_type() const { return token_type; }
    [[nodiscard]] const std::string &Token::get_token() const { return token_value; }
    [[nodiscard]] CHAR_TYPE Token::get_char_type() const { return c_type; }
    void Token::clear() {
        token_value.clear();
        token_type = TOKEN_TYPE::TOKEN_NULL;
        c_type = CHAR_TYPE::CHAR_NULL;
    }

    void Token::set_line(const size_t &line_num) { line = line_num; }

    size_t Token::get_line() const { return line; }

    std::ostream &operator<<(std::ostream &out, const Token &token) {
        switch (token.get_type()) {
        case TOKEN_TYPE::IDENTIFIER:
            out << "Identifier: ";
            break;
        case TOKEN_TYPE::INTEGER_VALUE:
            out << "Integer: ";
            break;
        case TOKEN_TYPE::FLOAT_VALUE:
            out << "Float: ";
            break;
        case TOKEN_TYPE::STRING_VALUE:
            out << "String: ";
            break;
        case TOKEN_TYPE::OPERATOR:
            out << "Operator:  ";
            break;
        case TOKEN_TYPE::TOKEN_NULL:
            out << "NULL TOKEN EOF\n";
            return out;
        case TOKEN_TYPE::TOKEN_ERROR:
            out << "Error Token - Value: ";
            break;
        }
        out << token.get_token();
        if (token.get_type() == TOKEN_TYPE::OPERATOR)
            out << " Char Code: " << static_cast<int>(token.get_char_type());
        return out;
    }

} // namespace mx
