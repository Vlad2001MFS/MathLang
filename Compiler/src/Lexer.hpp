#pragma once
#include <string>
#include <vector>

enum class TokenType {
    Def,
    Var,
    Return,

    Identifier,
    Number,
    Symbol,
};

struct Position {
    Position();

    int index;
    int line;
    int column;
    std::string src;
};

struct Token {
    Token();
    Token(TokenType type, const std::string &value);

    bool operator==(const Token &rhs) const;
    bool operator!=(const Token &rhs) const;

    TokenType type;
    std::string value;
    Position pos;
};

class Lexer {
public:
    Lexer(const std::string &source);

    std::vector<Token> process();

private:
    void mNextChar(bool skip);
    bool mReadToken(Token &token);

    std::string mSource;
    size_t mSourcePos;
    char mLastChar;
    Position mPosition;
    std::vector<std::string> mSourceLines;
};