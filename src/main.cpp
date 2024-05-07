#include "ast/ast.h"
#include <fmt/core.h>
#include <stdio.h>
using namespace std;
extern int yyparse();
extern FILE *yyin;
unique_ptr<BaseAST> ast;

int main(int argc, char **argv) {

    if(argc < 2){
        fmt::print("Usage: {} <filename>\n", argv[0]);
        return 1;
    }
    else if (argc > 2){
        fmt::print("Too many arguments\n");
        return 1;
    }

    yyin = fopen(argv[1],"r");//argc == 2
    if (!yyin) {
        fmt::print("Error: cannot open file '{}'\n", argv[1]);
        return 1;
    }

    int ret = yyparse();
    ast->Dump();
    return ret;

}
