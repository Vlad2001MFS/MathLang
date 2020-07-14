#include "ASTExprNumber.hpp"
#include "CodeBuilder.hpp"

ASTExprNumber::ASTExprNumber(double value) {
    mValue = value;
}

void ASTExprNumber::print(int tabs) {
    mPrint(tabs, "ExprNumber{" + std::to_string(mValue) + "}");
}

void ASTExprNumber::codegen(CodeBuilder &builder) {
    builder.genPush(mValue);
}