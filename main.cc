#include "parser.h"
#include "ast.h"
#include "forward.h"
#include "handler.h"


int main(int argc, char **argv)
{
    initPrecedence();

    fprintf(stderr, "ready> ");
    getNextToken();

    MainLoop();

    return 0;
}
