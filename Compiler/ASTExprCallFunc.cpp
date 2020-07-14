#include "ASTExprCallFunc.hpp"
#include "CodeBuilder.hpp"

ASTExprCallFunc::ASTExprCallFunc(const std::string &funcName, std::vector<std::unique_ptr<ASTExpr>> &args)
    : mFuncName(funcName), mArgs(std::move(args)) {
}

void ASTExprCallFunc::print(int tabs) {
    mPrint(tabs, "ExprCallFunc{" + mFuncName + "}");
    for (const auto &arg : mArgs) {
        arg->print(tabs + 1);
    }
}

void ASTExprCallFunc::codegen(CodeBuilder &builder) {
    for (auto it = mArgs.cbegin(); it != mArgs.cend(); it++) {
        (*it)->codegen(builder);
    }
    builder.genCall(mFuncName);
}