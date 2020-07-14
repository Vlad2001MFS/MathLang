#pragma once
#include "ASTStatement.hpp"
#include <vector>
#include <memory>

class ASTFuncDef : public ASTStatement {
public:
    ASTFuncDef(const std::string &funcName, const std::vector<std::string> &argNames, 
               std::vector<std::unique_ptr<ASTStatement>> &statements);

    void print(int tabs) override;
    void codegen(CodeBuilder &builder) override;

private:
    std::string mFuncName;
    std::vector<std::string> mArgNames;
    std::vector<std::unique_ptr<ASTStatement>> mStatements;
};