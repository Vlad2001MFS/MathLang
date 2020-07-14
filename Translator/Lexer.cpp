#include "Lexer.hpp"
#include <algorithm>
#include <locale>

std::string stringToLower(const std::string &str) {
    std::string out(str.size(), '\0');
    std::transform(str.begin(), str.end(), out.begin(), tolower);
    return out;
}

char stringToLower(char ch) {
    return std::tolower(ch, std::locale());
}

bool stringContainsSymbol(const std::string &str, char symbol, bool caseSensitive) {
    std::string s1 = caseSensitive ? str : stringToLower(str);
    char ch = caseSensitive ? symbol : stringToLower(symbol);
    for (const auto &it : s1) {
        if (it == ch) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> stringSplit(const std::string &str, const std::string &separatorsList, bool saveSeparators) {
    std::vector<std::string> tokens;
    std::string token;
    for (const auto &it : str) {
        if (stringContainsSymbol(separatorsList, it, false)) {
            tokens.push_back(token);
            token.clear();
            if (saveSeparators) {
                token += it;
                tokens.push_back(token);
                token.clear();
            }
        }
        else {
            token += it;
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}

Position::Position() {
    this->index = 0;
    this->line = 1;
    this->column = 0;
}

Token::Token() {
}

Token::Token(TokenType type, const std::string &value) : value(value) {
    this->type = type;
}

bool Token::operator==(const Token &rhs) const {
    return this->type == rhs.type && this->value == rhs.value;
}

bool Token::operator!=(const Token &rhs) const {
    return this->type != rhs.type || this->value != rhs.value;
}

Lexer::Lexer(const std::string &source) : mSource(source), mSourceLines(stringSplit(source, "\n", false)) {
    mSourcePos = 0;
    mLastChar = 0;
}

std::vector<Token> Lexer::process() {
    std::vector<Token> tokens;
    try {
        mNextChar(true);
        while (mLastChar != 0) {
            Token token;
            if (mReadToken(token)) {
                tokens.push_back(token);
            }
        }
    }
    catch (...) {}
    return tokens;
}

void Lexer::mNextChar(bool skip) {
    do {
        if (mSourcePos < mSource.size()) {
            mLastChar = mSource[mSourcePos];
            mSourcePos++;

            mPosition.index++;
            mPosition.column++;
            mPosition.src = mSourceLines[mPosition.line - 1];
            if (mLastChar == '\n') {
                mPosition.line++;
                mPosition.column = 0;
            }
        }
        else {
            mLastChar = 0;
            return;
        }
    } while (skip && std::isspace(mLastChar));
}

bool Lexer::mReadToken(Token &token) {
    token.pos = mPosition;
    if (std::isalpha(mLastChar) || stringContainsSymbol("_@", mLastChar, false)) {
        token.value += mLastChar;
        mNextChar(false);
        while (std::isalnum(mLastChar) || stringContainsSymbol("_@:.", mLastChar, false)) {
            token.value += mLastChar;
            mNextChar(false);
        }

        if (token.value == "jmp") {
            token.type = TokenType::Jmp;
        }
        else if (token.value == "call") {
            token.type = TokenType::Call;
        }
        else if (token.value == "ret") {
            token.type = TokenType::Ret;
        }
        else if (token.value == "add") {
            token.type = TokenType::Add;
        }
        else if (token.value == "sub") {
            token.type = TokenType::Sub;
        }
        else if (token.value == "mul") {
            token.type = TokenType::Mul;
        }
        else if (token.value == "div") {
            token.type = TokenType::Div;
        }
        else if (token.value == "push") {
            token.type = TokenType::Push;
        }
        else if (token.value == "pop") {
            token.type = TokenType::Pop;
        }
        else if (token.value == "set") {
            token.type = TokenType::Set;
        }
        else if (token.value == "get") {
            token.type = TokenType::Get;
        }
        else if (token.value == "unset") {
            token.type = TokenType::Unset;
        }
        else if (token.value.find(":") != std::string::npos) {
            token.type = TokenType::Label;
            token.value = token.value.substr(0, token.value.find(":"));
        }
        else {
            token.type = TokenType::Identifier;
        }
    }
    else if (std::isdigit(mLastChar)) {
        token.value += mLastChar;
        mNextChar(false);
        bool isDotFound = false;
        while (std::isdigit(mLastChar) || (!isDotFound && mLastChar == '.')) {
            if (mLastChar == '.') {
                isDotFound = true;
            }
            token.value += mLastChar;
            mNextChar(false);
        }
        token.type = TokenType::Number;
    }
    else if (std::isspace(mLastChar)) {
        mNextChar(true);
        return false;
    }
    else {
        mError(std::string("unknown symbol '") + mLastChar + "'");
        return false;
    }
    return true;
}

void Lexer::mError(const std::string &text) {
    printf("LexerError(%d:%d): %s\n", mPosition.line, mPosition.column, text.data());
    throw std::exception("lexer error");
}
