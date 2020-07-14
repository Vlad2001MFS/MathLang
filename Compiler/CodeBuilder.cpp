#include "CodeBuilder.hpp"
#include <cstdarg>

SymbolName::SymbolName(const std::string &name, const std::string &fullName) : name(name), fullName(fullName) {}

void CodeBuilder::beginFunc(const std::string &funcName, const std::vector<std::string> &argNames) {
    std::string fullName = mGetAbsoluteSymbolName(funcName);
    mAddLine("");
    mAddLine("jmp @%s_end@", fullName.data());
    mAddLine("%s:", fullName.data());
    mFuncStack.push_back(std::make_pair(SymbolName(funcName, fullName), mVarStack.size()));
    for (auto it = argNames.cbegin(); it != argNames.cend(); it++) {
        genSet(*it);
    }
    mFuncTable.push_back(SymbolName(funcName, fullName));
}

void CodeBuilder::endFunc() {
    while (mVarStack.size() != mFuncStack.back().second) {
        mAddLine("unset %s", mVarStack.back().fullName.data());
        mVarStack.pop_back();
    }
    mAddLine("ret");

    std::string funcName = mFuncStack.back().first.fullName;
    mFuncStack.pop_back();

    mAddLine("@%s_end@:", funcName.data());
    mAddLine("");
}

void CodeBuilder::genSet(const std::string &varName) {
    std::string fullName = mGetAbsoluteSymbolName(varName);
    mAddLine("set %s", fullName.data());
    mVarStack.push_back(SymbolName(varName, fullName));
}

void CodeBuilder::genBinOp(char op) {
    if (op == '+') {
        mAddLine("add");
    }
    else if (op == '-') {
        mAddLine("sub");
    }
    else if (op == '*') {
        mAddLine("mul");
    }
    else if (op == '/') {
        mAddLine("div");
    }
}

void CodeBuilder::genCall(const std::string &funcName) {
    mAddLine("call %s", mFindFuncAbsolute(funcName).data());
}

void CodeBuilder::genPush(double value) {
    mAddLine("push %f", value);
}

void CodeBuilder::genGet(const std::string &varName) {
    mAddLine("get %s", mFindVarAbsolute(varName).data());
}

void CodeBuilder::genReturn() {}

const std::string &CodeBuilder::getFinalCode() const {
    return mSource;
}

void CodeBuilder::mError(const std::string &text) {
    printf("CodeBuilderError: %s\n", text.data());
    throw std::exception("parser error");
}

void CodeBuilder::mAddLine(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buf[1024];
    vsprintf_s(buf, fmt, args);
    va_end(args);
    for (size_t i = 0; i < mFuncStack.size(); i++) {
        mSource += "    ";
    }
    mSource += buf;
    mSource += "\n";
}

std::string CodeBuilder::mGetAbsoluteSymbolName(const std::string &name) {
    std::string absoluteName;
    for (const auto &funcName : mFuncStack) {
        absoluteName += funcName.first.name + ".";
    }
    return absoluteName + name;
}

std::string CodeBuilder::mFindVarAbsolute(const std::string &name) {
    for (auto it = mVarStack.rbegin(); it != mVarStack.rend(); it++) {
        if (it->name == name) {
            return it->fullName;
        }
    }
    mError("variable '" + name + "' not found");
}

std::string CodeBuilder::mFindFuncAbsolute(const std::string &name) {
    if (name == "print") {
        return "print";
    }
    for (auto it = mFuncTable.rbegin(); it != mFuncTable.rend(); it++) {
        if (it->name == name) {
            return it->fullName;
        }
    }
    mError("function '" + name + "' not found");
}
