#pragma once
#include "Lexer.hpp"
#include <memory>

/*
whitespaces = ' ' {' '}
digit = '0' | ... | '9'
alpha = 'a' | ... | 'z'
opAdd = '+' | '-'
opMul = '*' | '/'

expression = term {opAdd term}
term = factor {opMul factor}
factor = ['-'] (number | identifier | callFunc | ('(' expression ')'))
number = digit {digit} ['.' digit {digit}]
identifier = alpha {digit | alpha}
callFunc = identifier '(' [argList] ')'
argList = argument {',' argument}
argument = number | identifier | callFunc | expression

varDef = 'var' whitespaces identifier '=' expression
funcDef = 'def' whitespaces identifier '(' [paramList] ')' '{' {statement} '}'
paramList = identifier {',' identifier}

assignment = identifier '=' expression
statement = (funcDef | varDef | assignment | expression) ';'
root = {statement}
*/

class CodeBuilder;
class ASTRoot;
class ASTStatement;
class ASTFuncDef;
class ASTVarDef;
class ASTReturn;
class ASTAssignment;
class ASTExpr;
class ASTExprBinOp;
class ASTExprNumber;
class ASTExprCallFunc;
class ASTExprVar;

class ASTRoot {
public:
    ASTRoot(std::vector<std::unique_ptr<ASTStatement>> &statements);

    void print();
    void codegen(const std::unique_ptr<CodeBuilder> &builder);

private:
    std::vector<std::unique_ptr<ASTStatement>> mStatements;
};

class ASTStatement {
public:
    virtual void print(int tabs);
    virtual void codegen(const std::unique_ptr<CodeBuilder> &builder);

protected:
    void mPrint(int tabs, const std::string &text);
};

class ASTFuncDef : public ASTStatement {
public:
    ASTFuncDef(const std::string &funcName, const std::vector<std::string> &argNames, 
        std::vector<std::unique_ptr<ASTStatement>> &statements);

    void print(int tabs) override;
    void codegen(const std::unique_ptr<CodeBuilder> &builder) override;

private:
    std::string mFuncName;
    std::vector<std::string> mArgNames;
    std::vector<std::unique_ptr<ASTVarDef>> mArgs;
    std::vector<std::unique_ptr<ASTStatement>> mStatements;
};

class ASTVarDef : public ASTStatement {
public:
    ASTVarDef(const std::string &varName, std::unique_ptr<ASTExpr> expr);

    void print(int tabs) override;
    void codegen(const std::unique_ptr<CodeBuilder> &builder) override;

private:
    std::string mVarName;
    std::unique_ptr<ASTExpr> mExpr;
};

class ASTReturn : public ASTStatement {
public:
    ASTReturn(std::unique_ptr<ASTExpr> expr);

    void print(int tabs) override;

private:
    std::unique_ptr<ASTExpr> mExpr;
};

class ASTAssignment : public ASTStatement {
public:
    ASTAssignment(const std::string &varName, std::unique_ptr<ASTExpr> expr);

    void print(int tabs) override;

private:
    std::string mVarName;
    std::unique_ptr<ASTExpr> mExpr;
};

class ASTExpr : public ASTStatement {
public:
    void print(int tabs) override;
};

class ASTExprBinOp : public ASTExpr {
public:
    ASTExprBinOp(char op, std::unique_ptr<ASTExpr> lhs, std::unique_ptr<ASTExpr> rhs);

    void print(int tabs) override;
    void codegen(const std::unique_ptr<CodeBuilder> &builder) override;

private:
    char mOp;
    std::unique_ptr<ASTExpr> mLHS, mRHS;
};

class ASTExprNumber : public ASTExpr {
public:
    ASTExprNumber(double value);

    void print(int tabs) override;
    void codegen(const std::unique_ptr<CodeBuilder> &builder) override;

private:
    double mValue;
};

class ASTExprCallFunc : public ASTExpr {
public:
    ASTExprCallFunc(const std::string &funcName, std::vector<std::unique_ptr<ASTExpr>> &args);

    void print(int tabs) override;

private:
    std::string mFuncName;
    std::vector<std::unique_ptr<ASTExpr>> mArgs;
};

class ASTExprVar : public ASTExpr {
public:
    ASTExprVar(const std::string &varName);

    void print(int tabs) override;

private:
    std::string mVarName;
};

class Parser {
public:
    Parser(const std::vector<Token> &tokens);

    std::unique_ptr<ASTRoot> process();

private:
    void mError(const std::string &text);
    bool mMatch(char ch);
    void mMatchError(char ch);
    void mNextToken();
    const Token &mLookToken();
    std::unique_ptr<ASTStatement> mParseStatement();
    std::unique_ptr<ASTFuncDef> mParseFuncDef();
    std::unique_ptr<ASTVarDef> mParseVarDef();
    std::unique_ptr<ASTExpr> mParseExpr();
    std::unique_ptr<ASTExpr> mParseExprTerm();
    std::unique_ptr<ASTExpr> mParseExprFactor();
    std::unique_ptr<ASTExpr> mParseNumber();
    std::unique_ptr<ASTExpr> mParseCallFunc();
    std::unique_ptr<ASTExpr> mParseExprIdentifier();
    std::unique_ptr<ASTReturn> mParseReturn();
    std::unique_ptr<ASTAssignment> mParseAssignment();

    std::vector<Token> mTokens;
    size_t mTokensPos;
    Token mLastToken;
    Token mCurToken;
};