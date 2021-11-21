#ifndef AST_H
#define AST_H

#include <string>
#include <memory>
#include <vector>


// ExprAST - base class 
class ExprAST 
{
public:
    virtual ~ExprAST() {}
};


// NumberExprAST - expression class for numeric literals like "1.0"
class NumberExprAST : public ExprAST 
{
private:
    double Val;

public:
    NumberExprAST(double Val) : Val(Val) {}
};


// VariableExprAST - expression class for a variable like "a"
class VariableExprAST : public ExprAST 
{
private:
    std::string Name;

public:
    VariableExprAST(const std::string &Name) : Name(Name) {}
};


// BinaryExprAST - expression class for binary operator
class BinaryExprAST : public ExprAST 
{
private:
    char Op;
    std::unique_ptr<ExprAST> LHS, RHS;

public:
    BinaryExprAST(char op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS) 
        : Op(op)
        , LHS(std::move(LHS))
        , RHS(std::move(RHS)) 
    {}
};


// CallExprAST - expression class for function call
class CallExprAST : public ExprAST 
{
private:
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;

public:
    CallExprAST(const std::string &Callee, std::vector<std::unique_ptr<ExprAST>> Args) 
        : Callee(Callee)
        , Args(std::move(Args)) 
    {}
};


// PrototypeAST - this class represents the 'prototype' for a function 
// which captures its name, and its argument names ( implicitly the number
// of arguments the function takes
class PrototypeAST 
{
private:
    std::string Name;
    std::vector<std::string> Args;

public:
    PrototypeAST(const std::string &name, std::vector<std::string> Args) 
        : Name(name)
        , Args(std::move(Args)) 
    {}

    const std::string &getName() const { return Name; }
};


// FunctionAST - this class represents a function definition itself
class FunctionAST 
{
private:
    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<ExprAST> Body;

public:
    FunctionAST(std::unique_ptr<PrototypeAST> Proto, std::unique_ptr<ExprAST> Body) 
        : Proto(std::move(Proto))
        , Body(std::move(Body)) 
    {}
};



#endif  // AST_H
