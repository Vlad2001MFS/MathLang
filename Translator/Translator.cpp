#include "Translator.hpp"

Translator::Translator(const std::vector<Token> &tokens) : mTokens(tokens) {
    mTokensPos = 0;
}

std::vector<uint8_t> Translator::process() {
    std::vector<uint8_t> bc;
    try {
        mTranslate(bc, false);

        mTokensPos = 0;
        mTranslate(bc, true);
    }
    catch (...) {
    }

    return bc;
}

void Translator::mError(const std::string &text) {
    printf("TranslatorError(%d:%d): ", mLastToken.pos.line, mLastToken.pos.column);
    printf("%s\n", text.data());
    int tabsCount = 0;
    for  (int i = 0; i < mLastToken.pos.column; i++) {
        if (mLastToken.pos.src[i] == '\t') {
            tabsCount++;
        }
    }
    std::string newSrc;
    for (const auto &ch : mLastToken.pos.src) {
        if (ch == '\t') {
            newSrc += "    ";
        }
        else {
            newSrc += ch;
        }
    }
    printf("%s\n", newSrc.data());
    for (int i = 0; i < mLastToken.pos.column + tabsCount*3; i++) {
        printf(" ");
    }
    printf("^\n");
    throw std::exception("translator error");
}

void Translator::mNextToken() {
    if (mTokensPos < mTokens.size()) {
        mLastToken = mCurToken;
        mCurToken = mTokens[mTokensPos];
    }
    mTokensPos++;
    if (mTokensPos == (mTokens.size() + 2)) {
        mError("no tokens");
    }
}

void Translator::mCheck(TokenType type, const std::string &expected) {
    if (mCurToken.type != type) {
        mError("expected '" + expected + "' instead '" + mCurToken.value + "'");
    }
}

void Translator::mTranslate(std::vector<uint8_t> &bc, bool genBytecode) {
    uint32_t pos = 0;

    auto add = [&](auto data) {
        if (genBytecode) {
            uint8_t *p = reinterpret_cast<uint8_t*>(&data);
            for (int i = 0; i < sizeof(data); i++) {
                bc.push_back(p[i]);
            }
        }
        else {
            pos += sizeof(data);
        }
    };

    auto getLabelAddress = [&](const std::string &name) {
        if (genBytecode) {
            auto it = mLabels.find(name);
            if (it == mLabels.end()) {
                mError("undefined label '" + name + "'");
            }
            return it->second;
        }
        else {
            return static_cast<LabelAddress>(0);
        }
    };

    while (mTokensPos < mTokens.size()) {
        mNextToken();
        if (mCurToken.type == TokenType::Jmp) {
            add(OpCode::Jmp);
            mNextToken();
            mCheck(TokenType::Identifier, "label name");
            add(getLabelAddress(mCurToken.value));
        }
        else if (mCurToken.type == TokenType::Call) {
            mNextToken();
            mCheck(TokenType::Identifier, "label name");
            if (mLabels.count(mCurToken.value) != 0) {
                add(OpCode::Call);
                add(getLabelAddress(mCurToken.value));
            }
            else if (mCurToken.value == "print") {
                add(OpCode::Int);
                add(static_cast<uint8_t>(0));
            }
        }
        else if (mCurToken.type == TokenType::Ret) {
            add(OpCode::Ret);
        }
        else if (mCurToken.type == TokenType::Add) {
            add(OpCode::Add);
        }
        else if (mCurToken.type == TokenType::Sub) {
            add(OpCode::Sub);
        }
        else if (mCurToken.type == TokenType::Mul) {
            add(OpCode::Mul);
        }
        else if (mCurToken.type == TokenType::Div) {
            add(OpCode::Div);
        }
        else if (mCurToken.type ==  TokenType::Push) {
            add(OpCode::Push);
            mNextToken();
            mCheck(TokenType::Number, "number");
            add(std::stod(mCurToken.value));
        }
        else if (mCurToken.type == TokenType::Pop) {
            add(OpCode::Pop);
        }
        else if (mCurToken.type == TokenType::Set) {
            add(OpCode::Set);
            mNextToken();
            mCheck(TokenType::Identifier, "variable name");
            uint64_t hash = std::hash<std::string>()(mCurToken.value);
            add(hash);
        }
        else if (mCurToken.type == TokenType::Get) {
            add(OpCode::Get);
            mNextToken();
            mCheck(TokenType::Identifier, "variable name");
            uint64_t hash = std::hash<std::string>()(mCurToken.value);
            add(hash);
        }
        else if (mCurToken.type == TokenType::Unset) {
            add(OpCode::Unset);
            mNextToken();
            mCheck(TokenType::Identifier, "variable name");
            uint64_t hash = std::hash<std::string>()(mCurToken.value);
            add(hash);
        }
        else if (mCurToken.type == TokenType::Label) {
            if (!genBytecode) {
                if (mLabels.count(mCurToken.value) == 0) {
                    mLabels[mCurToken.value] = pos;
                }
                else {
                    mError("label '" + mCurToken.value + "' already exist");
                }
            }
        }
        else {
            mError("unknown instruction '" + mCurToken.value + "'");
        }
    }
}
