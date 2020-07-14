#include "Parser.hpp"
#include "CodeBuilder.hpp"

Parser::Parser(const std::vector<Token> &tokens) : mTokens(tokens) {
    mTokensPos = 0;
}

std::unique_ptr<ASTRoot> Parser::process() {
    std::vector<std::unique_ptr<ASTStatement>> statements;
    try {
        mNextToken();
        while (mTokensPos < mTokens.size()) {
            statements.emplace_back(mParseStatement());
        }
    }
    catch (...) {}
    return std::make_unique<ASTRoot>(statements);
}

void Parser::mError(const std::string &text) {
    printf("ParserError(%d:%d): ", mLastToken.pos.line, mLastToken.pos.column);
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
    throw std::exception("parser error");
}

bool Parser::mMatch(char ch) {
    if (mCurToken.type == TokenType::Symbol && mCurToken.value[0] == ch) {
        mNextToken();
        return true;
    }
    return false;
}

void Parser::mMatchError(char ch) {
    if (!mMatch(ch)) {
        mError("expected '" + std::string(1, ch) + "'");
    }
}

void Parser::mNextToken() {
    if (mTokensPos < mTokens.size()) {
        mLastToken = mCurToken;
        mCurToken = mTokens[mTokensPos];
    }
    mTokensPos++;
    if (mTokensPos == (mTokens.size() + 2)) {
        mError("no tokens");
    }
}

const Token &Parser::mLookToken() {
    return mTokens[mTokensPos];
}

std::unique_ptr<ASTStatement> Parser::mParseStatement() {
    std::unique_ptr<ASTStatement> statement;
    if (mCurToken.type == TokenType::Def) {
        mNextToken();
        statement = mParseFuncDef();
    }
    else if (mCurToken.type == TokenType::Return) {
        mNextToken();
        statement = mParseReturn();
        mMatchError(';');
    }
    else if (mCurToken.type == TokenType::Identifier && mLookToken() == Token(TokenType::Symbol, "=")) {
        statement = mParseAssignment();
        mMatchError(';');
    }
    else {
        statement = mParseExpr();
        mMatchError(';');
    }
    return statement;
}

std::unique_ptr<ASTFuncDef> Parser::mParseFuncDef() {
    if (mCurToken.type != TokenType::Identifier) {
        mError("expected identifier");
    }
    std::string funcName = mCurToken.value;
    mNextToken();
    mMatchError('(');
    std::vector<std::string> argNames;
    while (mCurToken.type == TokenType::Identifier) {
        argNames.push_back(mCurToken.value);
        mNextToken();
        if (!mMatch(',')) {
            break;
        }
    }
    mMatchError(')');

    mMatchError('{');
    std::vector<std::unique_ptr<ASTStatement>> statements;
    while (!mMatch('}')) {
        statements.emplace_back(mParseStatement());
    }
    return std::make_unique<ASTFuncDef>(funcName, argNames, statements);
}

std::unique_ptr<ASTExpr> Parser::mParseExpr() {
    std::unique_ptr<ASTExpr> lhs = mParseExprTerm();
    if (mMatch('+')) {
        std::unique_ptr<ASTExpr> rhs = mParseExpr();
        return std::make_unique<ASTExprBinOp>('+', std::move(lhs), std::move(rhs));
    }
    else if (mMatch('-')) {
        std::unique_ptr<ASTExpr> rhs = mParseExpr();
        return std::make_unique<ASTExprBinOp>('-', std::move(lhs), std::move(rhs));
    }
    return lhs;
}

std::unique_ptr<ASTExpr> Parser::mParseExprTerm() {
    std::unique_ptr<ASTExpr> lhs = mParseExprFactor();
    if (mMatch('*')) {
        std::unique_ptr<ASTExpr> rhs = mParseExprTerm();
        return std::make_unique<ASTExprBinOp>('*', std::move(lhs), std::move(rhs));
    }
    else if (mMatch('/')) {
        std::unique_ptr<ASTExpr> rhs = mParseExprTerm();
        return std::make_unique<ASTExprBinOp>('/', std::move(lhs), std::move(rhs));
    }
    return lhs;
}

std::unique_ptr<ASTExpr> Parser::mParseExprFactor() {
    std::unique_ptr<ASTExpr> expr;
    bool isNegative = mMatch('-');
    switch (mCurToken.type) {
        case TokenType::Number: {
            expr = mParseNumber();
            break;
        }
        case TokenType::Identifier: {
            if (mLookToken() == Token(TokenType::Symbol, "(")) {
                expr = mParseCallFunc();
            }
            else {
                expr = mParseExprIdentifier();
            }
            break;
        }
        case TokenType::Symbol: {
            if (mMatch('(')) {
                expr = mParseExpr();
                mMatch(')');
            }
            else {
                mError("unknown token");
            }
            break;
        }
        default: {
            mError("unknown token");
            break;
        }
    }
    if (isNegative) {
        return std::make_unique<ASTExprBinOp>('-', std::make_unique<ASTExprNumber>(0), std::move(expr));
    }
    else {
        return expr;
    }
}

std::unique_ptr<ASTExpr> Parser::mParseNumber() {
    std::unique_ptr<ASTExpr> expr = std::make_unique<ASTExprNumber>(std::stod(mCurToken.value));
    mNextToken();
    return expr;
}

std::unique_ptr<ASTExpr> Parser::mParseCallFunc() {
    std::string funcName = mCurToken.value;
    mNextToken();
    mMatchError('(');
    std::vector<std::unique_ptr<ASTExpr>> args;
    if (!mMatch(')')) {
        while (true) {
            args.emplace_back(mParseExpr());
            if (!mMatch(',')) {
                mMatchError(')');
                break;
            }
        }
    }
    return std::make_unique<ASTExprCallFunc>(funcName, args);
}

std::unique_ptr<ASTExpr> Parser::mParseExprIdentifier() {
    std::unique_ptr<ASTExpr> expr = std::make_unique<ASTExprVar>(mCurToken.value);
    mNextToken();
    return expr;
}

std::unique_ptr<ASTReturn> Parser::mParseReturn() {
    std::unique_ptr<ASTExpr> expr = mParseExpr();
    return std::make_unique<ASTReturn>(std::move(expr));
}

std::unique_ptr<ASTAssignment> Parser::mParseAssignment() {
    if (mCurToken.type != TokenType::Identifier) {
        mError("expected identifier");
    }
    std::string varName = mCurToken.value;
    mNextToken();
    mMatchError('=');
    std::unique_ptr<ASTExpr> expr = mParseExpr();
    return std::make_unique<ASTAssignment>(varName, std::move(expr));
}
