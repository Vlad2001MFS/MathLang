#include "ASTExprVar.hpp"
#include "CodeBuilder.hpp"

ASTExprVar::ASTExprVar(const std::string &varName) : mVarName(varName) {
}

void ASTExprVar::print(int tabs) {
    mPrint(tabs, "ExprVar{" + mVarName + "}");
}

void ASTExprVar::codegen(CodeBuilder &builder) {
    builder.genGet(mVarName);
}