#pragma once
#include "Lexer.hpp"
#include "../Bytecode.hpp"
#include <string>
#include <vector>
#include <map>

class Translator {
public:
    Translator(const std::vector<Token> &tokens);

    std::vector<uint8_t> process();

private:
    void mError(const std::string &text);
    void mNextToken();
    void mCheck(TokenType type, const std::string &expected);
    void mTranslate(std::vector<uint8_t> &bc, bool genBytecode);

    std::vector<Token> mTokens;
    size_t mTokensPos;
    Token mLastToken;
    Token mCurToken;
    std::map<std::string, LabelAddress> mLabels;
};