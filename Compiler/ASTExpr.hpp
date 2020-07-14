#pragma once
#include "ASTStatement.hpp"

class ASTExpr : public ASTStatement {
public:
    void print(int tabs) override;
};