#include "VM.hpp"

VM::VM(const std::vector<uint8_t> &bc) : mBC(bc) {
    mIP = 0;
    mOpCodeFuncs[static_cast<size_t>(OpCode::Jmp)] = &VM::mOpCodeJmp;
    mOpCodeFuncs[static_cast<size_t>(OpCode::Call)] = &VM::mOpCodeCall;
    mOpCodeFuncs[static_cast<size_t>(OpCode::Ret)] = &VM::mOpCodeRet;
    mOpCodeFuncs[static_cast<size_t>(OpCode::Add)] = &VM::mOpCodeAdd;
    mOpCodeFuncs[static_cast<size_t>(OpCode::Sub)] = &VM::mOpCodeSub;
    mOpCodeFuncs[static_cast<size_t>(OpCode::Mul)] = &VM::mOpCodeMul;
    mOpCodeFuncs[static_cast<size_t>(OpCode::Div)] = &VM::mOpCodeDiv;
    mOpCodeFuncs[static_cast<size_t>(OpCode::Push)] = &VM::mOpCodePush;
    mOpCodeFuncs[static_cast<size_t>(OpCode::Pop)] = &VM::mOpCodePop;
    mOpCodeFuncs[static_cast<size_t>(OpCode::Set)] = &VM::mOpCodeSet;
    mOpCodeFuncs[static_cast<size_t>(OpCode::Get)] = &VM::mOpCodeGet;
    mOpCodeFuncs[static_cast<size_t>(OpCode::Unset)] = &VM::mOpCodeUnset;
    mOpCodeFuncs[static_cast<size_t>(OpCode::Int)] = &VM::mOpCodeInt;
}

void VM::run() {
    mIP = 0;
    try {
        while (mIP < mBC.size()) {
            size_t opcode = mBC[mIP++];
            if (opcode >= static_cast<size_t>(OpCode::_Count)) {
                mError("unknown opcode '" + std::to_string(static_cast<uint32_t>(opcode)) + "'");
            }
            else {
                (this->*mOpCodeFuncs[opcode])();
            }
        }
    }
    catch (...) {
    }
}

void VM::mError(const std::string &text) {
    printf("RuntimeError(%d): %s\n", mIP, text.data());
    throw std::exception("runtime error");
}

double VM::mStackPop() {
    double value = mStack.top();
    mStack.pop();
    return value;
}

void VM::mOpCodeJmp() {
    mIP = mGetValue<LabelAddress>();
}

void VM::mOpCodeCall() {
    auto address = mGetValue<LabelAddress>();
    mRetStack.push(mIP);
    mIP = address;
}

void VM::mOpCodeRet() {
    mIP = mRetStack.top();
    mRetStack.pop();
}

void VM::mOpCodeAdd() {
    double arg1 = mStackPop();
    double arg2 = mStackPop();
    mStack.push(arg1 + arg2);
}

void VM::mOpCodeSub() {
    double arg1 = mStackPop();
    double arg2 = mStackPop();
    mStack.push(arg1 - arg2);
}

void VM::mOpCodeMul() {
    double arg1 = mStackPop();
    double arg2 = mStackPop();
    mStack.push(arg1*arg2);
}

void VM::mOpCodeDiv() {
    double arg1 = mStackPop();
    double arg2 = mStackPop();
    mStack.push(arg1 / arg2);
}

void VM::mOpCodePush() {
    auto value = mGetValue<double>();
    mStack.push(value);
}

void VM::mOpCodePop() {
    mStack.pop();
}

void VM::mOpCodeSet() {
    double arg1 = mStackPop();
    auto hash = mGetValue<uint64_t>();
    mVars[hash].push(arg1);
}

void VM::mOpCodeGet() {
    auto hash = mGetValue<uint64_t>();
    auto it = mVars.find(hash);
    if (it != mVars.end() && !it->second.empty()) {
        mStack.push(mVars[hash].top());
    }
    else {
        mError("variable '" + std::to_string(hash) + "' not found (get)");
    }
}

void VM::mOpCodeUnset() {
    auto hash = mGetValue<uint64_t>();
    auto it = mVars.find(hash);
    if (it != mVars.end() && !it->second.empty()) {
        it->second.pop();
        if (it->second.empty()) {
            mVars.erase(hash);
        }
    }
    else {
        mError("variable '" + std::to_string(hash) + "' not found (unset)");
    }
}

void VM::mOpCodeInt() {
    auto id = mGetValue<uint8_t>();
    if (id == 0) {
        double arg1 = mStackPop();
        printf("=> %f\n", arg1);
    }
    else {
        mError("unknown interruption '" + std::to_string(id) + "'");
    }
}
