#include "SemanticAnalysis.h"
#include "ast/ast.h"
#include <fmt/core.h>
#include <stdio.h>

using namespace std;
extern int          yyparse();
extern FILE*        yyin;
unique_ptr<BaseAST> ast;

int main(int argc, char** argv) {
    if (argc < 2) {
        fmt::print("Usage: {} <filename>\n", argv[0]);
        return 1;
    } else if (argc > 2) {
        fmt::print("Too many arguments\n");
        return 1;
    }

    yyin = fopen(argv[1], "r");  // argc == 2
    if (!yyin) {
        fmt::print("Error: cannot open file '{}'\n", argv[1]);
        return 1;
    }

    int ret = yyparse();
    std::cout << "---------Grammatical Analysis---------" << std::endl;
    if(!ret) {
        ast->Dump();
        std::cout << "[INFO] Grammatical analysis success" << std::endl;
        std::cout << "--------------------------------------" << std::endl;
    }
    else {
        std::cout << "[ERROR] Grammatical analysis failed" << std::endl;
        std::cout << "--------------------------------------" << std::endl;
        return ret;
    }
    std::cout << "----------Semantic Analysis-----------" << std::endl;
    // traverseAST(ast);
    SmAnalysis semantic_analysis;
    try {
        semantic_analysis.traverse_by_node(ast);
    } catch (const std::runtime_error& e) {
        std::cout << "Exception occurred: " << e.what() << std::endl;
        std::cout << "--------------------------------------" << std::endl;
        exit(1);
    }
    std::cout << "[INFO] Semantic analysis success" << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    
    return ret;
}
