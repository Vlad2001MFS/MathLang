#pragma once
#include "ASTStatement.hpp"
#include <memory>

class ASTExpr;
class CodeBuilder;

class ASTReturn : public ASTStatement {
public:
    ASTReturn(std::unique_ptr<ASTExpr> expr);

    void print(int tabs) override;
    void codegen(CodeBuilder &builder) override;

private:
    std::unique_ptr<ASTExpr> mExpr;
};