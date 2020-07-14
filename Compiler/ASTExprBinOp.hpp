#pragma once
#include "ASTExpr.hpp"
#include <memory>

class ASTExprBinOp : public ASTExpr {
public:
    ASTExprBinOp(char op, std::unique_ptr<ASTExpr> lhs, std::unique_ptr<ASTExpr> rhs);

    void print(int tabs) override;
    void codegen(CodeBuilder &builder) override;

private:
    char mOp;
    std::unique_ptr<ASTExpr> mLHS, mRHS;
};