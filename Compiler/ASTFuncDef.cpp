#include "ASTFuncDef.hpp"
#include "CodeBuilder.hpp"

ASTFuncDef::ASTFuncDef(const std::string &funcName, const std::vector<std::string> &argNames,
    std::vector<std::unique_ptr<ASTStatement>> &statements) : mFuncName(funcName), mArgNames(argNames), mStatements(std::move(statements)) {
}

void ASTFuncDef::print(int tabs) {
    std::string args = "[";
    for (const auto &argName : mArgNames) {
        args += argName + ", ";
    }
    args.pop_back();
    args += "]";
    mPrint(tabs, "FuncDef{" + mFuncName + "; " + args + "}");
    for (const auto &statement : mStatements) {
        statement->print(tabs + 1);
    }
}

void ASTFuncDef::codegen(CodeBuilder &builder) {
    builder.beginFunc(mFuncName, mArgNames);
    for (const auto &it : mStatements) {
        it->codegen(builder);
    }
    builder.endFunc();
}