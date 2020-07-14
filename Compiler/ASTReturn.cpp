#include "ASTReturn.hpp"
#include "ASTExpr.hpp"
#include "CodeBuilder.hpp"

ASTReturn::ASTReturn(std::unique_ptr<ASTExpr> expr) : mExpr(std::move(expr)) {
}

void ASTReturn::print(int tabs) {
    mPrint(tabs, "Return");
    mExpr->print(tabs + 1);
}

void ASTReturn::codegen(CodeBuilder &builder) {
    mExpr->codegen(builder);
    builder.genReturn();
}