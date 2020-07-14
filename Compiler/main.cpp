#include "Lexer.hpp"
#include "Parser.hpp"
#include "CodeBuilder.hpp"
#include <filesystem>
#include <cstdio>

int main(int argc, char **argv) {
    std::string source;

    if (argc > 1) {
       FILE *f = fopen(argv[1], "rb");
       if (f) {
           fseek(f, 0, SEEK_END);
           source.resize(ftell(f));
           fseek(f, 0, SEEK_SET);
           fread(source.data(), 1, source.size(), f);
           fclose(f);
       }
       else {
           printf("Error: file not found\n");
       }
    }
    else {
        source = R"(
            def f(a, b) {
                def sum(a, b) { return a + b; }
                c = sum(a, b);
                return c / 2;
            }

            a = f(3);
            print(a);
        )";
    }

    auto lexer = Lexer(source);
    std::vector<Token> tokens = lexer.process();
    for (const auto &token : tokens) {
        printf("[%d: '%s']\n", token.type, token.value.data());
    }
    printf("--------------------------------------\n");

    auto parser = Parser(tokens);
    std::unique_ptr<ASTRoot> root = parser.process();
    root->print();
    printf("--------------------------------------\n");

    CodeBuilder builder;
    root->codegen(builder);
    printf("%s\n", builder.getFinalCode().data());

    std::string path;
    if (argv[1]) {
        path = std::filesystem::path(argv[1]).stem().string() + ".mla";
    }
    else {
        path = "out.mla";
    }
    FILE *f = fopen(path.data(), "wb");
    fwrite(builder.getFinalCode().data(), 1, builder.getFinalCode().size(), f);
    fclose(f);

    getchar();
    return 0;
}