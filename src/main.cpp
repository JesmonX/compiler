#include "semantic/SemanticAnalysis.h"
#include "ast/ast.h"
#include "ir/ir.h"
#include "ir/type.h"
#include "ir/makeir.h"
#include <fmt/core.h>
#include <cstdio>
#include <fstream>



using namespace std;
extern int          yyparse();
extern FILE*        yyin;
unique_ptr<BaseAST> ast;

int main(int argc, char** argv) {
    if (argc < 2) {
        fmt::print("Usage: {} <input file> [<output file>]\n", argv[0]);
        return 1;
    } else if (argc > 3) {
        fmt::print("Too many arguments\n");
        return 1;
    }

    
    yyin = fopen(argv[1], "r");

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
    
    
    std::cout << "------------IR Generation-------------" << std::endl;
    Module module;
    symtab symtab;

    // add library functions
    FunctionType *getint_ty = FunctionType::get(Type::getIntegerTy(),{});
    FunctionType *getch_ty = FunctionType::get(Type::getIntegerTy(),{});
    FunctionType *getarray_ty = FunctionType::get(Type::getIntegerTy(),{PointerType::get(Type::getIntegerTy())});
    FunctionType *putint_ty = FunctionType::get(Type::getUnitTy(),{Type::getIntegerTy()});
    FunctionType *putch_ty = FunctionType::get(Type::getUnitTy(),{Type::getIntegerTy()});
    FunctionType *putarray_ty = FunctionType::get(Type::getUnitTy(),{Type::getIntegerTy(),PointerType::get(Type::getIntegerTy())});
    FunctionType *starttime_ty = FunctionType::get(Type::getUnitTy(),{});
    FunctionType *stoptime_ty = FunctionType::get(Type::getUnitTy(),{});
    Function::Create(getint_ty,true,"getint",&module);
    Function::Create(getch_ty,true,"getch",&module);
    Function::Create(getarray_ty,true,"getarray",&module);
    Function::Create(putint_ty,true,"putint",&module);
    Function::Create(putch_ty,true,"putch",&module);
    Function::Create(putarray_ty,true,"putarray",&module);
    Function::Create(starttime_ty,true,"starttime",&module);
    Function::Create(stoptime_ty,true,"stoptime",&module);

    
    fstream file;
    string Filename;
    if(argc == 2)
        Filename = "./target/" + string(argv[1]).substr(string(argv[1]).find_last_of("/\\") + 1, string(argv[1]).find_last_of(".") - string(argv[1]).find_last_of("/\\") - 1) + ".acc";
    else 
        Filename = argv[2];
    file.open(Filename, ios::out);

    std::vector<Value*> global_vars;
    std::vector<Value*> global_vals;
    std::vector<std::vector<int>> global_dims;
    MakeIR makeir(global_vars, global_vals, global_dims);
    try{
        makeir.irCompUnitAST(static_cast<CompUnitAST*>(ast.get()), &module, &symtab);
    }
    catch(const std::runtime_error& e){
        std::cout << "Exception occurred: " << e.what() << std::endl;
        std::cout << "--------------------------------------" << std::endl;
        return 1;
    }
    std::cout << "[INFO] IR generation success" << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    
    module.print(file,0);

    return ret;
}
