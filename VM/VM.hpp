#pragma once
#include "../Bytecode.hpp"
#include <vector>
#include <stack>
#include <string>
#include <map>

class VM {
    using OpCodeFunc = void(VM::*)();

public:
    VM(const std::vector<uint8_t> &bc);

    void run();

private:
    template<typename T>
    T mGetValue() {
        T value;
        for (int i = 0; i < sizeof(T); i++) {
            if (mIP >= mBC.size()) {
                mError("unexpected end of bytecode");
            }
            reinterpret_cast<uint8_t*>(&value)[i] = mBC[mIP++];
        }
        return value;
    }

    void mError(const std::string &text);
    double mStackPop();
    void mOpCodeJmp();
    void mOpCodeCall();
    void mOpCodeRet();
    void mOpCodeAdd();
    void mOpCodeSub();
    void mOpCodeMul();
    void mOpCodeDiv();
    void mOpCodePush();
    void mOpCodePop();
    void mOpCodeSet();
    void mOpCodeGet();
    void mOpCodeUnset();
    void mOpCodeInt();

    std::vector<uint8_t> mBC;
    uint32_t mIP;
    std::stack<double> mStack;
    std::stack<LabelAddress> mRetStack;
    OpCodeFunc mOpCodeFuncs[static_cast<size_t>(OpCode::_Count)];
    std::map<uint64_t, std::stack<double>> mVars;
};