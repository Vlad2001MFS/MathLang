#include "ASTRoot.hpp"
#include "ASTStatement.hpp"

ASTRoot::ASTRoot(std::vector<std::unique_ptr<ASTStatement>> &statements) : mStatements(std::move(statements)) {
}

void ASTRoot::print() {
    for (const auto &statement : mStatements) {
        statement->print(0);
    }
}

void ASTRoot::codegen(CodeBuilder &builder) {
    for (const auto &statement : mStatements) {
        statement->codegen(builder);
    }
}