#pragma once
#include "ASTExpr.hpp"
#include <vector>
#include <memory>

class ASTExprCallFunc : public ASTExpr {
public:
    ASTExprCallFunc(const std::string &funcName, std::vector<std::unique_ptr<ASTExpr>> &args);

    void print(int tabs) override;
    void codegen(CodeBuilder &builder) override;

private:
    std::string mFuncName;
    std::vector<std::unique_ptr<ASTExpr>> mArgs;
};