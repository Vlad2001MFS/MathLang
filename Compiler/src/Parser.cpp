#include "Parser.hpp"

ASTRoot::ASTRoot(std::vector<std::unique_ptr<ASTStatement>> &statements) : mStatements(std::move(statements)) {
}

void ASTRoot::print() {
    for (const auto &statement : mStatements) {
        statement->print(0);
    }
}

void ASTRoot::codegen(const std::unique_ptr<CodeBuilder> &builder) {
    for (const auto &statement : mStatements) {
        statement->codegen(builder);
    }
}

void ASTStatement::print(int tabs) {
}

void ASTStatement::codegen(const std::unique_ptr<CodeBuilder> &builder) {
}

void ASTStatement::mPrint(int tabs, const std::string &text) {
    for (int i = 0; i < tabs; i++) {
        printf("  ");
    }
    printf("%s\n", text.data());
}

ASTFuncDef::ASTFuncDef(const std::string &funcName, const std::vector<std::string> &argNames,
    std::vector<std::unique_ptr<ASTStatement>> &statements) : mFuncName(funcName), mArgNames(argNames), mStatements(std::move(statements)) {
    for (const auto &argName : argNames) {
        mArgs.emplace_back(std::make_unique<ASTVarDef>(argName, std::make_unique<ASTExprNumber>(0)));
    }
}

void ASTFuncDef::print(int tabs) {
    std::string args = "[";
    for (const auto &argName : mArgNames) {
        args += argName + ", ";
    }
    args.pop_back();
    args.pop_back();
    args += "]";
    mPrint(tabs, "FuncDef{" + mFuncName + "; " + args + "}");
    for (const auto &statement : mStatements) {
        statement->print(tabs + 1);
    }
}

void ASTFuncDef::codegen(const std::unique_ptr<CodeBuilder> &builder) {
}

ASTVarDef::ASTVarDef(const std::string &varName, std::unique_ptr<ASTExpr> expr) : mVarName(varName), mExpr(std::move(expr)) {
}

void ASTVarDef::print(int tabs) {
    mPrint(tabs, "VarDef{" + mVarName + "}");
    mExpr->print(tabs + 1);
}

void ASTVarDef::codegen(const std::unique_ptr<CodeBuilder> &builder) {
}

ASTReturn::ASTReturn(std::unique_ptr<ASTExpr> expr) : mExpr(std::move(expr)) {
}

void ASTReturn::print(int tabs) {
    mPrint(tabs, "Return");
    mExpr->print(tabs + 1);
}

ASTAssignment::ASTAssignment(const std::string &varName, std::unique_ptr<ASTExpr> expr) : mVarName(varName), mExpr(std::move(expr)) {
}

void ASTAssignment::print(int tabs) {
    mPrint(tabs, "Assignment{" + mVarName + "}");
    mExpr->print(tabs + 1);
}

void ASTExpr::print(int tabs) {
}

ASTExprBinOp::ASTExprBinOp(char op, std::unique_ptr<ASTExpr> lhs, std::unique_ptr<ASTExpr> rhs) : mLHS(std::move(lhs)), mRHS(std::move(rhs)) {
    mOp = op;
}

void ASTExprBinOp::print(int tabs) {
    mPrint(tabs, "ExprBinOp{" + std::string(1, mOp) + "}");
    mLHS->print(tabs + 1);
    mRHS->print(tabs + 1);
}

void ASTExprBinOp::codegen(const std::unique_ptr<CodeBuilder> &builder) {
}

ASTExprNumber::ASTExprNumber(double value) {
    mValue = value;
}

void ASTExprNumber::print(int tabs) {
    mPrint(tabs, "ExprNumber{" + std::to_string(mValue) + "}");
}

void ASTExprNumber::codegen(const std::unique_ptr<CodeBuilder> &builder) {
}

ASTExprCallFunc::ASTExprCallFunc(const std::string &funcName, std::vector<std::unique_ptr<ASTExpr>> &args)
    : mFuncName(funcName), mArgs(std::move(args)) {
}

void ASTExprCallFunc::print(int tabs) {
    mPrint(tabs, "ExprCallFunc{" + mFuncName + "}");
    for (const auto &arg : mArgs) {
        arg->print(tabs + 1);
    }
}

ASTExprVar::ASTExprVar(const std::string &varName) : mVarName(varName) {
}

void ASTExprVar::print(int tabs) {
    mPrint(tabs, "ExprVar{" + mVarName + "}");
}

Parser::Parser(const std::vector<Token> &tokens) : mTokens(tokens) {
    mTokensPos = 0;
}

std::unique_ptr<ASTRoot> Parser::process() {
    std::vector<std::unique_ptr<ASTStatement>> statements;
    mNextToken();
    while (mTokensPos < mTokens.size()) {
        statements.emplace_back(mParseStatement());
    }
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
    else if (mCurToken.type == TokenType::Var) {
        mNextToken();
        statement = mParseVarDef();
        mMatchError(';');
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

std::unique_ptr<ASTVarDef> Parser::mParseVarDef() {
    if (mCurToken.type != TokenType::Identifier) {
        mError("expected identifier");
    }
    std::string varName = mCurToken.value;
    mNextToken();
    mMatchError('=');
    std::unique_ptr<ASTExpr> expr = mParseExpr();
    return std::make_unique<ASTVarDef>(varName, std::move(expr));
}

std::unique_ptr<ASTExpr> Parser::mParseExpr() {
    std::unique_ptr<ASTExpr> lhs = mParseExprTerm();
    if (mMatch('+')) {
        std::unique_ptr<ASTExpr> rhs = mParseExprTerm();
        return std::make_unique<ASTExprBinOp>('+', std::move(lhs), std::move(rhs));
    }
    else if (mMatch('-')) {
        std::unique_ptr<ASTExpr> rhs = mParseExprTerm();
        return std::make_unique<ASTExprBinOp>('-', std::move(lhs), std::move(rhs));
    }
    return lhs;
}

std::unique_ptr<ASTExpr> Parser::mParseExprTerm() {
    std::unique_ptr<ASTExpr> lhs = mParseExprFactor();
    if (mMatch('*')) {
        std::unique_ptr<ASTExpr> rhs = mParseExprFactor();
        return std::make_unique<ASTExprBinOp>('*', std::move(lhs), std::move(rhs));
    }
    else if (mMatch('/')) {
        std::unique_ptr<ASTExpr> rhs = mParseExprFactor();
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
    mMatchError('=');
    std::unique_ptr<ASTExpr> expr = mParseExpr();
    return std::make_unique<ASTAssignment>(varName, std::move(expr));
}
