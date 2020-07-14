#include "VM.hpp"
#include <cstdio>

int main(int argc, char **argv) {
    std::vector<uint8_t> bc;

    if (argc > 0) {
        FILE *f = fopen(argv[1], "rb");
        if (f) {
            fseek(f, 0, SEEK_END);
            bc.resize(ftell(f));
            fseek(f, 0, SEEK_SET);
            fread(bc.data(), 1, bc.size(), f);
            fclose(f);
        }
        else {
            printf("Error: file not found\n");
        }
    }
    else {
        printf("Error: bytecode does not loaded\n");
    }

    auto vm = VM(bc);
    vm.run();

    getchar();
    return 0;
}