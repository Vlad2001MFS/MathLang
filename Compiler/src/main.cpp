#include "Lexer.hpp"
#include "Parser.hpp"
#include <cstdio>

const char gTestSource[] = R"(
def func(a) {
    var b = a + 1;
    return b*2;
}

var a = func(1.5);
print(a);
)";

int main() {
    Lexer lexer = Lexer(gTestSource);
    std::vector<Token> tokens = lexer.process();
    for (const auto &token : tokens) {
        printf("[%d: '%s']\n", token.type, token.value.data());
    }
    Parser parser = Parser(tokens);
    std::unique_ptr<ASTRoot> root = parser.process();
    root->print();
    getchar();
    return 0;
}