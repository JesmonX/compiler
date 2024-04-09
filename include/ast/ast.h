#pragma once
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <string>
#include <memory>
using namespace std;


class BaseAST {
 public:
  virtual ~BaseAST() = default;

  virtual void Dump() const = 0;
};
using ASTPtr = std::unique_ptr<BaseAST>;


class FuncDefAST : public BaseAST  {
public:
    std::string func_type;
    std::string ident;
    ASTPtr func_fparams;
    ASTPtr block;

    void Dump() const override {
        std::cout << "FuncDef { ";
        std::cout << "func_type : "<< func_type;
        std::cout << ", " << ident << ", ";
        if(func_fparams!=NULL){
            std::cout << "FuncFParams { ";
            func_fparams->Dump();
            std::cout << " }";
        }
        block->Dump();
        std::cout << " }";
  }
};

class VarDefAST  : public BaseAST {
public:

    int int_const;
    std::string ident;
    ASTPtr exp;
    ASTPtr unit;
    int type;

    void Dump() const override   {
        std::cout << "VarDef { ";
        std::cout << "var_type : "<< "int";
        std::cout << ", " << ident << "  ";
        switch (type) {
            case 1:
                exp->Dump();
                break;

            case 3:
                std::cout << '['<<int_const << "] ";
                if(unit!=NULL)
                    unit->Dump();
                std::cout << ", ";
                break;
            default:
                break;
        }
        std::cout << " }";
  }
};

class DeclAST : public BaseAST {
public:
    ASTPtr func_def;
    ASTPtr var_def;
    void Dump() const override{
        if(func_def)
            func_def->Dump();
        if(var_def) 
            var_def->Dump();
    }
};

class CompUnitAST : public BaseAST{
public:
    ASTPtr comp_units;
    void Dump() const override{
        std::cout << "CompUnit { ";
        comp_units->Dump();
        
        std::cout << " }";
    }
};

class CompUnitsAST : public BaseAST {
public:
    ASTPtr comp_unit;
    ASTPtr def;

    void Dump() const override{
        if(comp_unit)
            comp_unit->Dump();
        if(def) 
            def->Dump();
    }
};




class MulVarDefAST : public BaseAST  {
public:
    ASTPtr mul_var_def;
    ASTPtr var_def;
    void Dump() const override{
        if(mul_var_def)
            mul_var_def->Dump();
        if(var_def) 
            var_def->Dump();
    }
};

class ConstUnitAST  : public BaseAST {
public:
    int int_const;
    ASTPtr unit;
    void Dump()    const override{
        std::cout << '['<<int_const << "] ";
        if(unit!=nullptr)
            unit->Dump();
        std::cout << " }";
    }
};

class FuncFParamsAST : public BaseAST  {
public:
    ASTPtr param;
    ASTPtr params;
    void Dump()   const override {
        param->Dump();
        printf(", ");
        if(params!=NULL)
            params->Dump();
    }
};

class FuncFParamAST : public BaseAST  {
public:
    std::string ident;
    std::string param_type = "int";
    ASTPtr unit;
    int type;
    void Dump()  const override  {
        std::cout << ident << " ";
        std::cout << "param_type : "<< param_type;
        if(type == 1)
            std::cout <<"[]";
        if(unit!=NULL)
            unit->Dump();
    }
};

class ExpAST : public BaseAST  {
public:
    ASTPtr exp;
    ASTPtr add_exp;
    void Dump()  const override {
        if(exp!=NULL)
            exp->Dump();
        if(add_exp!=NULL)
            add_exp->Dump();
    }
};

class BlockAST : public BaseAST  {
public:
    ASTPtr item;
    void Dump()  const override {
        std::cout << "Block { ";
        if(item!=NULL)
            item->Dump();
        std::cout << " }";
    }
};

class BlockItemAST : public BaseAST  {
public:
    ASTPtr block_item;
    ASTPtr mul_var_def;
    ASTPtr stmt;
    void Dump()  const override {
        if(block_item!=NULL)
            block_item->Dump();
        if(mul_var_def!=NULL)
            mul_var_def->Dump();
        if(stmt!=NULL)
            stmt->Dump();
    }
};

class StmtAST : public BaseAST  {
public:
    ASTPtr lval;
    ASTPtr exp;
    ASTPtr block;
    ASTPtr stmt;
    int type;
    void Dump()  const override {
    
    

    
    
    
    
    
    }
};