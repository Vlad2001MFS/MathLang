#pragma once
#include "ASTExpr.hpp"

class ASTExprNumber : public ASTExpr {
public:
    ASTExprNumber(double value);

    void print(int tabs) override;
    void codegen(CodeBuilder &builder) override;

private:
    double mValue;
};