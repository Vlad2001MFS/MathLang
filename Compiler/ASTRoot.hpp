#pragma once
#include <vector>
#include <memory>

class ASTStatement;
class CodeBuilder;

class ASTRoot {
public:
    ASTRoot(std::vector<std::unique_ptr<ASTStatement>> &statements);

    void print();
    void codegen(CodeBuilder &builder);

private:
    std::vector<std::unique_ptr<ASTStatement>> mStatements;
};