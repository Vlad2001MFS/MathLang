#include "Lexer.hpp"
#include "Translator.hpp"
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
            jmp @f_end@
            f:
                set x
                push 2.000000
                get x
                add
                set a
                get a
                get x
                mul
                unset x
                ret
            @f_end@:

            push 4.000000
            call f
            set v
            get v
            call print
        )";
    }

    auto lexer = Lexer(source);
    std::vector<Token> tokens = lexer.process();
    for (const auto &token : tokens) {
        printf("[%d: '%s']\n", token.type, token.value.data());
    }

    auto translator = Translator(tokens);
    std::vector<uint8_t> bytecode = translator.process();

    std::string path;
    if (argv[1]) {
        path = std::filesystem::path(argv[1]).stem().string() + ".mlb";
    }
    else {
        path = "out.mlb";
    }
    FILE *f = fopen(path.data(), "wb");
    fwrite(bytecode.data(), 1, bytecode.size(), f);
    fclose(f);

    getchar();
    return 0;
}