#include "parser.h"
#include "forward.h"

#include <stdio.h>
#include <ctype.h>
#include <cstdio>
#include <cstdlib>


int CurTok;
std::string IdentifierStr;
double NumVal;
std::map<char, int> BinopPrecedence;


// gettok - return the next token from standard input
int gettok() 
{
    static int LastChar = ' ';

    // skip any whitespace
    while (isspace(LastChar))
    {
        LastChar = getchar();
    }

    // identifier: [a-zA-Z0-9]*
    if (isalpha(LastChar)) 
    {
        IdentifierStr = LastChar;

        while (isalnum(LastChar = getchar()))
        {
            IdentifierStr += LastChar;
        }

        if (IdentifierStr == "def")
            return tok_def;
        if (IdentifierStr == "extern")
            return tok_extern;

        return tok_identifier;
    }

    // Number: [0-9.]+
    if (isdigit(LastChar) || LastChar == '.') 
    {
        std::string NumStr;
        do {
            NumStr += LastChar;
            LastChar = getchar();
        } while (isdigit(LastChar) || LastChar == '.');

        NumVal = strtod(NumStr.c_str(), 0);
        return tok_number;
    }

    // Comment until end of line
    if (LastChar == '#') 
    {
        do {
            LastChar = getchar();
        } while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

        if (LastChar != EOF) 
            return gettok();
    }

    if (LastChar == EOF)
        return tok_eof;

    // Otherwise just return the character as its ascii value
    int ThisChar = LastChar;
    LastChar = getchar();
    return ThisChar;
}


int getNextToken() 
{
    return CurTok = gettok();
}


// Error handling helper function
std::unique_ptr<ExprAST> LogError(const char *Str) 
{
    fprintf(stderr, "LogError: %s\n", Str);
    return nullptr;
}

std::unique_ptr<PrototypeAST> LogErrorP(const char *Str) 
{
    LogError(Str);
    return nullptr;
}


// this function is to be called when tok_number is current token
std::unique_ptr<ExprAST> ParseNumberExpr() 
{
    // construct the result from NumVal
    auto Result = std::make_unique<NumberExprAST>(NumVal);

    getNextToken();
    return std::move(Result);
}


// ( Expression )
std::unique_ptr<ExprAST> ParseParenExpr() 
{
    // take the first '('
    getNextToken();

    // helper function to parse expression
    auto V = ParseExpression();

    // if we didn't get subexpression, we return null on this error
    if (!V)
        return nullptr;
    
    // if we get subexpression, but didn't get ), we return error
    if (CurTok != ')')
        return LogError("expected ')'");

    getNextToken();

    return V;
}


// Identifier = ( Expression1, Expression2 ,... )
std::unique_ptr<ExprAST> ParseIdentifierExpr() 
{
    // store identifierStr to IdName
    std::string IdName = IdentifierStr;

    getNextToken();
    
    // if we didn't get '(', it's a variable like 'a'
    if (CurTok != '(') 
        return std::make_unique<VariableExprAST>(IdName);

    // we did get '(' before, then we parse the variable 
    getNextToken();

    std::vector<std::unique_ptr<ExprAST>> Args;

    // we do recursive call to search for ')'
    if (CurTok != ')') 
    {
        while (1) 
        {
            // if it's not ')', it has to be a variable or an expression
            if (auto Arg = ParseExpression())
                Args.push_back(std::move(Arg));
            else
                return nullptr;

            // if we get ')', we end the loop 
            if (CurTok == ')')
                break;

            // we expect either ')' to end the expression, or , for a separate arguments
            if (CurTok != ',')
                return LogError("Expected ')' or ',' in the argument list");

            getNextToken();
        }
    }

    // we already got ')', jump to next one
    getNextToken();

    return std::make_unique<CallExprAST>(IdName, std::move(Args));
}



// primary - handle basic expression
// ::= identifierExpr
// ::= numberExpr
// ::= parenExpr
std::unique_ptr<ExprAST> ParsePrimary() 
{
    switch (CurTok) 
    {
        case tok_identifier:
            return ParseIdentifierExpr();
        case tok_number:
            return ParseNumberExpr();
        case '(':
            return ParseParenExpr();
        default:
            return LogError("Unknown token when expecting an expression");
    }
}


// returns CurTok's precedence
// if it's not binary operator, returns -1
int GetTokPrecedence() 
{
    // if it's not ascii, return -1
    if (!isascii(CurTok)) 
        return -1;
    
    int TokPrec = BinopPrecedence[CurTok];

    if (TokPrec <= 0)
        return -1;

    return TokPrec;
}


// initialize the precedence map
// we define 4 binary operator precedence, and * has the highest precedence 
void initPrecedence() 
{
    BinopPrecedence['<'] = 10;
    BinopPrecedence['+'] = 20;
    BinopPrecedence['-'] = 30;
    BinopPrecedence['*'] = 40;
}


// we consider the expressions as primary + [binop, primary] 
// for example, "a+b*c" can be viewed as "a" , [+, b], [*, c]
std::unique_ptr<ExprAST> ParseExpression() 
{
    // we consider LHS always primary 
    auto LHS = ParsePrimary();
    if (!LHS)
        return nullptr;

    // primary expression precedence is set as 0
    return ParseBinOpRHS(0, std::move(LHS));
}


std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS) 
{
    while (1) 
    {
        int TokPrec = GetTokPrecedence();

        // if RHS precedence is lower, we know LHS has higher precedence 
        if (TokPrec < ExprPrec)
            return LHS;

        int BinOp = CurTok;
        getNextToken();

        auto RHS = ParsePrimary();
        if (!RHS)
            return nullptr;

        // we have to decide on the next precedence 
        // if the formula is "(a+b) binop unparsed" or "a+(b binop unparsed)"
        int NextPrec = GetTokPrecedence();

        // if the next precedence is higher, we have to deal with RHS first
        // we put RHS as the LHS to recursively call ParseBinOpRHS
        if (TokPrec < NextPrec) 
        {
            RHS = ParseBinOpRHS(TokPrec+1, std::move(RHS));

            if (!RHS)
                return nullptr;
        }

        // merge LHS/RHS
        LHS = std::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
    }
}



std::unique_ptr<PrototypeAST> ParsePrototype() 
{
    if (CurTok != tok_identifier) 
        return LogErrorP("Expected function name in prototype");
    
    std::string FnName = IdentifierStr;
    getNextToken();
    
    if (CurTok != '(')
        return LogErrorP("Expected '(' in prototype");
    
    // Read the list of argument names
    std::vector<std::string> ArgNames;
    
    while (getNextToken() == tok_identifier)
    {
        ArgNames.push_back(IdentifierStr);
    }
    
    if (CurTok != ')')
        return LogErrorP("Expected ')' in prototype");
    
    // success: eat ')'
    getNextToken();

    return std::make_unique<PrototypeAST>(FnName, std::move(ArgNames));
}


// definition ::= id (id*)
std::unique_ptr<FunctionAST> ParseDefinition() 
{
    getNextToken(); // eat def
    auto Proto = ParsePrototype();
    
    if (!Proto)
        return nullptr;
    
    if (auto E = ParseExpression())
        return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));

    return nullptr;
}


std::unique_ptr<PrototypeAST> ParseExtern() 
{
    getNextToken();
    return ParsePrototype();
}



// expr := expression
std::unique_ptr<FunctionAST> ParseTopLevelExpr() 
{
    if (auto E = ParseExpression()) 
    {
        auto Proto = std::make_unique<PrototypeAST>("", std::vector<std::string>());
        return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
    }
    return nullptr;
}

