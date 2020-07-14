#pragma once
#include "ASTStatement.hpp"
#include <memory>

class ASTExpr;

class ASTAssignment : public ASTStatement {
public:
    ASTAssignment(const std::string &varName, std::unique_ptr<ASTExpr> expr);

    void print(int tabs) override;
    void codegen(CodeBuilder &builder) override;

private:
    std::string mVarName;
    std::unique_ptr<ASTExpr> mExpr;
};