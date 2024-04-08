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
    int type;

    void Dump() const override {
        std::cout << "FuncDef { ";
        std::cout << "func_type : "<< func_type;
        std::cout << ", " << ident << ", ";
        if(func_fparams!=nullptr)
            func_fparams->Dump();
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
        std::cout << ", " << ident << ", ";
        switch (type) {
            case 1:
                exp->Dump();
                break;

            case 3:
                std::cout << '['<<int_const << "] ";
                if(unit!=nullptr)
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
        else 
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
        else 
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
        else 
            var_def->Dump();
    }
};

class VarDefUnitAST  : public BaseAST {
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
        std::cout << "FuncFParams { ";
        param->Dump();
        if(params!=nullptr)
            params->Dump();
        std::cout << " }";
    }
};

class FuncFParamAST : public BaseAST  {
public:
    
    void Dump()  const override  {
        std::cout << "FuncFParams { ";
        
        std::cout << " }";
    }
};