#include "ast/ast.h"

#include <fmt/core.h>
#include <stdio.h>
using namespace std;
extern int yyparse();
extern FILE *yyin;
unique_ptr<BaseAST> ast;

int main(int argc, char **argv) {

    yyin = fopen(argv[1],"r");//argc == 2
    //return yyparse();
    int ret = yyparse();
    ast->Dump();
    return ret;
    //print_expr(static_cast<ExprPtr>(root));
    //fmt::print("Hello, World!\n");

}
