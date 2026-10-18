#include <MXLex/scanner.hpp>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

namespace {
    using T = mx::TOKEN_TYPE;
    using C = mx::CHAR_TYPE;

    void require(bool condition, const std::string &message) {
        if (!condition)
            throw std::runtime_error(message);
    }

    std::vector<mx::Token> scan(const std::string &source) {
        std::istringstream input(source);
        mx::Scanner scanner(input);
        std::vector<mx::Token> tokens;
        mx::Token token;
        while (scanner.lex(token) != T::TOKEN_NULL)
            tokens.push_back(token);
        require(token.get_type() == T::TOKEN_NULL && token.get_char_type() == C::CHAR_NULL, "EOF clears token");
        require(scanner.get_token_count() == tokens.size(), "Token count");
        return tokens;
    }

    void literals(const std::string &source, T type, const std::vector<std::string> &expected) {
        auto tokens = scan(source);
        require(tokens.size() == expected.size(), "Literal count: " + source);
        for (size_t i = 0; i < tokens.size(); ++i) {
            require(tokens[i].get_type() == type && tokens[i].get_token() == expected[i], "Literal: " + expected[i]);
            require(tokens[i].get_char_type() == C::CHAR_NULL, "Literal operator metadata");
        }
    }

    void invalid(const std::string &source) {
        try {
            scan(source);
        } catch (const mx::ScannerError &) {
            return;
        }
        throw std::runtime_error("Expected error: " + source);
    }
} // namespace

int main() {
    try {
        const std::vector<std::pair<std::string, C>> operators = {
            {"(", C::CHAR_LPAREN},   {")", C::CHAR_RPAREN}, {"[", C::CHAR_LBRACKET}, {"]", C::CHAR_RBRACKET}, {"{", C::CHAR_LBRACE}, {"}", C::CHAR_RBRACE}, {"+", C::CHAR_PLUS},   {"-", C::CHAR_MINUS}, {"*", C::CHAR_ASTERISK}, {"/", C::CHAR_SLASH},       {"%", C::CHAR_PERCENT},    {"=", C::CHAR_EQUAL},      {"<", C::CHAR_LESS},        {">", C::CHAR_GREATER},       {"!", C::CHAR_EXCLAMATION},    {"&", C::CHAR_AMPERSAND}, {"|", C::CHAR_PIPE},  {"^", C::CHAR_CARET},  {"~", C::CHAR_TILDE},       {"?", C::CHAR_QUESTION},    {":", C::CHAR_COLON},           {";", C::CHAR_SEMICOLON},       {",", C::CHAR_COMMA},       {".", C::CHAR_PERIOD},    {"#", C::CHAR_HASH},      {"##", C::CHAR_HASHHASH}, {"++", C::CHAR_PLUSPLUS}, {"--", C::CHAR_MINUSMINUS}, {"+=", C::CHAR_PLUSEQ},
            {"-=", C::CHAR_MINUSEQ}, {"*=", C::CHAR_MULEQ}, {"/=", C::CHAR_DIVEQ},   {"%=", C::CHAR_MODEQ},   {"&=", C::CHAR_ANDEQ}, {"|=", C::CHAR_OREQ},  {"^=", C::CHAR_XOREQ}, {"==", C::CHAR_EQEQ}, {"!=", C::CHAR_NOT_EQ},  {"<=", C::CHAR_LESSTHANEQ}, {">=", C::CHAR_GREATEREQ}, {"<<", C::CHAR_SHIFTLEFT}, {">>", C::CHAR_SHIFTRIGHT}, {"<<=", C::CHAR_SHIFTLEFTEQ}, {">>=", C::CHAR_SHIFTRIGHTEQ}, {"&&", C::CHAR_ANDAND},   {"||", C::CHAR_OROR}, {"->", C::CHAR_ARROW}, {"...", C::CHAR_DOTDOTDOT}, {"::", C::CHAR_COLONCOLON}, {".*", C::CHAR_PERIODASTERISK}, {"->*", C::CHAR_ARROWASTERISK}, {"<=>", C::CHAR_SPACESHIP}, {"<:", C::CHAR_LBRACKET}, {":>", C::CHAR_RBRACKET}, {"<%", C::CHAR_LBRACE},   {"%>", C::CHAR_RBRACE},   {"%:", C::CHAR_HASH},       {"%:%:", C::CHAR_HASHHASH},
        };
        for (const auto &[spelling, type] : operators) {
            auto tokens = scan(spelling);
            require(tokens.size() == 1 && tokens[0].get_type() == T::OPERATOR && tokens[0].get_token() == spelling && tokens[0].get_char_type() == type, "Operator: " + spelling);
        }
        auto dots = scan("..");
        require(dots.size() == 2 && dots[0].get_token() == "." && dots[1].get_token() == ".", "Dots at EOF");
        auto adjacent = scan("a+++b>>=2; x->y!=0&&z<=3");
        require(adjacent.size() == 16 && adjacent[1].get_token() == "++" && adjacent[4].get_token() == ">>=", "Adjacent operators");
        literals("0 077 123 0xff 0XABu 0b101 42UL 42lu 42LL 42uLL 42LLU", T::INTEGER_VALUE, {"0", "077", "123", "0xff", "0XABu", "0b101", "42UL", "42lu", "42LL", "42uLL", "42LLU"});
        literals(".5 1. 1e3 2E-4 1.25f 2.e+3L 0x1.fp+2 0X.8P-1F", T::FLOAT_VALUE, {".5", "1.", "1e3", "2E-4", "1.25f", "2.e+3L", "0x1.fp+2", "0X.8P-1F"});
        literals(R"("" "a\"b\\c" "/* text */" L"wide" u8"utf8" u"utf16" U"utf32")", T::STRING_VALUE, {R"("")", R"("a\"b\\c")", R"("/* text */")", R"(L"wide")", R"(u8"utf8")", R"(u"utf16")", R"(U"utf32")"});
        literals(R"('a' '\n' '\'' '\\' L'x' u'x' U'x' u8'x')", T::CHARACTER_VALUE, {"'a'", R"('\n')", R"('\'')", R"('\\')", "L'x'", "u'x'", "U'x'", "u8'x'"});
        for (const auto &source : {"0x", "0b102", "09", "1e+", "1e2e3", "1.2.3", "123abc", "1f", "1lL", "0x1.2", "''", "'x", "\"x", "\"x\n\"", "/* unterminated"})
            invalid(source);

        auto lines = scan("// ignored\r\n/* block\ncomment */\v\f_name2\r\n\"ok\"\r'x'");
        require(lines.size() == 3 && lines[0].get_line() == 3 && lines[1].get_line() == 4 && lines[2].get_line() == 5, "Comment, whitespace and newline tracking");
        auto spliced = scan("na\\\nme +\\\r\n= 1; // hidden\\\n still hidden\n\"a\\\nb\" next");
        require(spliced.size() == 6 && spliced[0].get_token() == "name" && spliced[0].get_line() == 1 && spliced[1].get_token() == "+=" && spliced[1].get_line() == 2 && spliced[4].get_token() == "\"ab\"" && spliced[4].get_line() == 5 && spliced[5].get_line() == 6, "Line splicing");
        auto program = scan("#define CAT(a,b) a ## b\nint main(void) { const char *s = \"hello\"; return s[0] == 'h' ? 0 : 1; }");
        require(program.size() == 37 && program[0].get_char_type() == C::CHAR_HASH, "C program");
        require(scan("// eof").empty() && scan("/* ok */").empty(), "Comments at EOF");
        auto error = scan(std::string(1, '\x01'));
        require(error.size() == 1 && error[0].get_type() == T::TOKEN_ERROR, "Unknown byte");
        mx::Token token;
        token.set_token("+", T::OPERATOR, C::CHAR_PLUS);
        token.set_token("id", T::IDENTIFIER);
        require(token.get_char_type() == C::CHAR_NULL, "Reset stale operator metadata");
        std::cout << "Scanner regression tests passed\n";
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
