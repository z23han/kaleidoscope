#include "parser.h"
#include "handler.h"
#include "forward.h"


void HandleDefinition()
{
    if (ParseDefinition())
    {
        fprintf(stderr, "Parsed a function definition\n");
    }
    else
    {
        getNextToken();
    }
}


void HandleExtern()
{
    if (ParseExtern())
    {
        fprintf(stderr, "Parsed an extern\n");
    }
    else
    {
        getNextToken();
    }
}


void HandleTopLevelExpression()
{
    if (ParseTopLevelExpr())
    {
        fprintf(stderr, "Parsed a top-level expr\n");
    }
    else
    {
        getNextToken();
    }
}


void MainLoop()
{
    while (1)
    {
        fprintf(stderr, "ready>");
        switch (CurTok)
        {
        case tok_eof:
            break;
        case ';':
        {
            getNextToken();
            break;
        }
        case tok_def:
        {
            HandleDefinition();
            break;
        }
        case tok_extern:
        {
            HandleExtern();
            break;
        }
        default:
        {
            HandleTopLevelExpression();
            break;
        }
        }
    }
}
