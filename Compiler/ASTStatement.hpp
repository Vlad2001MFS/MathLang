#pragma once
#include <string>

class CodeBuilder;

class ASTStatement {
public:
    virtual void print(int tabs);
    virtual void codegen(CodeBuilder &builder);

protected:
    void mPrint(int tabs, const std::string &text);
};