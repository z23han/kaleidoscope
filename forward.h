#ifndef FORWARD_H
#define FORWARD_H

#include <string>
#include <map>

extern int CurTok;
extern std::string IdentifierStr;
extern double NumVal;

// map holds the precedence of binary operator
extern std::map<char, int> BinopPrecedence;

#endif // FORWARD_H
