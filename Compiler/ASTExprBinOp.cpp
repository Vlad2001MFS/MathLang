#include "ASTExprBinOp.hpp"
#include "CodeBuilder.hpp"

ASTExprBinOp::ASTExprBinOp(char op, std::unique_ptr<ASTExpr> lhs, std::unique_ptr<ASTExpr> rhs) : mLHS(std::move(lhs)), mRHS(std::move(rhs)) {
    mOp = op;
}

void ASTExprBinOp::print(int tabs) {
    mPrint(tabs, "ExprBinOp{" + std::string(1, mOp) + "}");
    mLHS->print(tabs + 1);
    mRHS->print(tabs + 1);
}

void ASTExprBinOp::codegen(CodeBuilder &builder) {
    mRHS->codegen(builder);
    mLHS->codegen(builder);
    builder.genBinOp(mOp);
}
