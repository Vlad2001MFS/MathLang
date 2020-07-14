#pragma once
#include "ASTRoot.hpp"
#include "ASTStatement.hpp"
#include "ASTFuncDef.hpp"
#include "ASTReturn.hpp"
#include "ASTAssignment.hpp"
#include "ASTExpr.hpp"
#include "ASTExprBinOp.hpp"
#include "ASTExprNumber.hpp"
#include "ASTExprCallFunc.hpp"
#include "ASTExprVar.hpp"
#include "Lexer.hpp"

class Parser {
public:
    Parser(const std::vector<Token> &tokens);

    std::unique_ptr<ASTRoot> process();

private:
    void mError(const std::string &text);
    bool mMatch(char ch);
    void mMatchError(char ch);
    void mNextToken();
    const Token &mLookToken();
    std::unique_ptr<ASTStatement> mParseStatement();
    std::unique_ptr<ASTFuncDef> mParseFuncDef();
    std::unique_ptr<ASTExpr> mParseExpr();
    std::unique_ptr<ASTExpr> mParseExprTerm();
    std::unique_ptr<ASTExpr> mParseExprFactor();
    std::unique_ptr<ASTExpr> mParseNumber();
    std::unique_ptr<ASTExpr> mParseCallFunc();
    std::unique_ptr<ASTExpr> mParseExprIdentifier();
    std::unique_ptr<ASTReturn> mParseReturn();
    std::unique_ptr<ASTAssignment> mParseAssignment();

    std::vector<Token> mTokens;
    size_t mTokensPos;
    Token mLastToken;
    Token mCurToken;
};