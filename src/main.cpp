#include "ast/ast.h"

#include <fmt/core.h>
#include <stdio.h>

extern int yyparse();
extern FILE *yyin;

NodePtr root;

int main(int argc, char **argv) {

    yyin = fopen(argv[1],"r");//argc == 2
    
    return yyparse();
    //print_expr(static_cast<ExprPtr>(root));
    //fmt::print("Hello, World!\n");

}
