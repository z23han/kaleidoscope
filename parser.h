#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include <map>
#include <string>


// the lexer returns token [0-255] if it's an unknown character 
// otherwise one of the known things will be returned
enum Token 
{
    tok_eof = -1, 

    // commands
    tok_def = -2, 
    tok_extern = -3, 

    // primary
    tok_identifier = -4, 
    tok_number = -5
};

int gettok();
int getNextToken();

std::unique_ptr<ExprAST> LogError(const char *);

std::unique_ptr<PrototypeAST> LogErrorP(const char *);

// numberExpr ::= number
std::unique_ptr<ExprAST> ParseNumberExpr();

// parenExpr ::= '(' expression ')'
std::unique_ptr<ExprAST> ParseParenExpr();

// identifierExpr 
// ::= identifier
// ::= identifier '(' expression* ')'
std::unique_ptr<ExprAST> ParseIdentifierExpr();

std::unique_ptr<PrototypeAST> ParsePrototype();
std::unique_ptr<FunctionAST> ParseDefinition();
std::unique_ptr<PrototypeAST> ParseExtern();
std::unique_ptr<FunctionAST> ParseTopLevelExpr();
std::unique_ptr<ExprAST> ParseExpression();
std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS);


void initPrecedence();
int GetTokPrecedence();

#endif  // PARSER_H
