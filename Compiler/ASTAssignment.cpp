#include "ASTAssignment.hpp"
#include "ASTExpr.hpp"
#include "CodeBuilder.hpp"

ASTAssignment::ASTAssignment(const std::string &varName, std::unique_ptr<ASTExpr> expr) : mVarName(varName), mExpr(std::move(expr)) {
}

void ASTAssignment::print(int tabs) {
    mPrint(tabs, "Assignment{" + mVarName + "}");
    mExpr->print(tabs + 1);
}

void ASTAssignment::codegen(CodeBuilder &builder) {
    mExpr->codegen(builder);
    builder.genSet(mVarName);
}
