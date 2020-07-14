#include "ASTStatement.hpp"

void ASTStatement::print(int tabs) {
}

void ASTStatement::codegen(CodeBuilder &builder) {
}

void ASTStatement::mPrint(int tabs, const std::string &text) {
    for (int i = 0; i < tabs; i++) {
        printf("  ");
    }
    printf("%s\n", text.data());
}