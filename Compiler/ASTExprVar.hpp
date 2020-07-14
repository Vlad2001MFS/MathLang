#pragma once
#include "ASTExpr.hpp"

class ASTExprVar : public ASTExpr {
public:
    ASTExprVar(const std::string &varName);

    void print(int tabs) override;
    void codegen(CodeBuilder &builder) override;

private:
    std::string mVarName;
};