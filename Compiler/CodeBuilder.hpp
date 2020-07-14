#pragma once
#include <string>
#include <vector>
#include <map>

struct SymbolName {
    SymbolName(const std::string &name, const std::string &fullName);

    std::string name, fullName;
};

class CodeBuilder {
public:
    CodeBuilder() = default;

    void beginFunc(const std::string &funcName, const std::vector<std::string> &argNames);
    void endFunc();
    void genSet(const std::string &varName);
    void genBinOp(char op);
    void genCall(const std::string &funcName);
    void genPush(double value);
    void genGet(const std::string &varName);
    void genReturn();

    const std::string &getFinalCode() const;

private:
    void mError(const std::string &text);
    void mAddLine(const char *fmt, ...);
    std::string mGetAbsoluteSymbolName(const std::string &name);
    std::string mFindVarAbsolute(const std::string &name);
    std::string mFindFuncAbsolute(const std::string &name);

    std::string mSource;
    std::vector<std::pair<SymbolName, size_t>> mFuncStack;
    std::vector<SymbolName> mVarStack;
    std::vector<SymbolName> mFuncTable;
};