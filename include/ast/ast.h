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
    ASTPtr stmt2;//only else stmt
    int type;
    void Dump()  const override {
        switch (type) {
            case 1:
                std::cout << "Assign Stmt:{ ";
                lval->Dump();
                std::cout << " = ";
                exp->Dump();
                break;
            case 2:
                exp->Dump();
                break;
            case 3:
                block->Dump();
                break;
            case 4:
                std::cout << "If Stmt:{ ";
                std::cout << "condition( ";
                exp->Dump();
                std::cout << ") ";
                std::cout << "then ";
                stmt->Dump();
                std::cout << " } ";
                break;
            case 5:
                std::cout << "If Stmt:{ ";
                std::cout << "condition( ";
                exp->Dump();
                std::cout << ") ";
                std::cout << "then ";
                stmt->Dump();
                std::cout << " else:{ ";
                stmt2->Dump();
                std::cout << " } ";
                break;
            case 6:
                std::cout << "While Stmt:{ ";
                std::cout << "condition( ";
                exp->Dump();
                std::cout << ") ";
                stmt->Dump();
                std::cout << "} ";
                break;
            case 7:
                std::cout << "Break Stmt; ";
                break;
            case 8:
                std::cout << "Continue Stmt; ";
                break;
            case 9:
                std::cout << "Return Stmt: {} ";
                break;
            case 10:
                std::cout << "Return Stmt: { ";
                exp->Dump();
                std::cout << "} ";
                break;
            default:
                break;
        }
    }
};

class ExpAST : public BaseAST  {
public:
    ASTPtr lorexp;
    void Dump()  const override {
        if(lorexp!=NULL)
            lorexp->Dump();
    }
};

class LvalAST : public BaseAST  {
public:
    std::string ident;
    ASTPtr unit;
    void Dump()  const override {
        std::cout << "ident: " <<ident <<" ";
        if(unit!=NULL)
            unit->Dump();

    }
};

class LvalUnitAST : public BaseAST  {
public:
    ASTPtr exp;
    ASTPtr unit;
    void Dump()  const override {
        if (unit!=NULL)
            unit->Dump();
        std::cout << "[";
        exp->Dump();
        std::cout << "] ";
    }
};

class PrimaryExpAST : public BaseAST  {
public:
    int int_const;
    ASTPtr exp;
    ASTPtr lval;
    void Dump()  const override {
        if(exp!=NULL)
            exp->Dump();
        if(lval!=NULL)
            lval->Dump();
        if(exp == NULL && lval == NULL)
            std::cout << "int_const: "<< int_const << " ";
    }
};

class UnaryExpAST : public BaseAST  {
public:
    ASTPtr primary_exp;
    ASTPtr unary_exp;
    ASTPtr params;
    std::string ident;
    int op_type;
    int type;
    void Dump()  const override {
        switch (type) {
            case 1:
                primary_exp->Dump();
                break;
            case 2:
                std::cout << "ident: "<< ident;
                break;
            case 3:
                std::cout << "ident: "<< ident;
                std::cout << " FuncRParams: { ";
                params->Dump();
                std::cout << " }";
                break;
            case 4:
                std::cout << "UnaryExp: ";
                switch (op_type){
                    case 1:
                        std::cout << "op: + {";
                        break;
                    case 2:
                        std::cout << "op: - { ";
                        break;
                    case 3:
                        std::cout << "op: ! { ";
                        break;
                }
                unary_exp->Dump();
                std::cout << " }";
                break;
        }
    }
};

class FuncRParamsAST : public BaseAST  {
public:
    ASTPtr exp;
    ASTPtr params;
    void Dump()  const override {
        if(params!=NULL)
            params->Dump();
        exp->Dump();
    }
};

class MulExpAST : public BaseAST  {
public:
    ASTPtr mul_exp;
    ASTPtr unary_exp;
    int op_type;
    void Dump()  const override {
        if(mul_exp!=NULL){
            std::cout << "MulExp: ";
            
            if(op_type == 1)
                std::cout << " op: mul { ";
            else if (op_type == 2)
                std::cout << " op: div { ";
            else if (op_type == 3)
                std::cout << " op: mod { ";
            mul_exp->Dump();
            std::cout << " , ";
            unary_exp->Dump();
            std::cout << " }";
        }
        else 
            unary_exp->Dump();
    }
};

class AddExpAST : public BaseAST  {
public:
    ASTPtr add_exp;
    ASTPtr mul_exp;
    int op_type;
    void Dump()  const override {
        if(add_exp!=NULL){
            std::cout << "AddExp: ";
            if(op_type == 1)
                std::cout << " op: add { ";
            else if (op_type == 2)
                std::cout << " op: sub {";
            add_exp->Dump();
            std::cout << " , ";
            mul_exp->Dump();
            std::cout << " }";
        }
        else
            mul_exp->Dump();
        
    }
};

class RelExpAST : public BaseAST  {
public:
    ASTPtr rel_exp;
    ASTPtr add_exp;
    int op_type;
    void Dump()  const override {
        if(rel_exp!=NULL){
            std::cout << "RelExp: ";
            if(op_type == 1)
                std::cout << " op: < { ";
            else if (op_type == 2)
                std::cout << " op: > { ";
            else if (op_type == 3)
                std::cout << " op: <= { ";
            else if (op_type == 4)
                std::cout << " op: >= { ";
            rel_exp->Dump();
            std::cout << " , ";
            add_exp->Dump();
            std::cout << " }";
        }
        else
            add_exp->Dump();
    }
};

class EqExpAST : public BaseAST  {
public:
    ASTPtr eq_exp;
    ASTPtr rel_exp;
    int op_type;
    void Dump()  const override {
        if(eq_exp!=NULL){
            std::cout << "EqExp: ";
            if(op_type == 1)
                std::cout << " op: == { ";
            else if (op_type == 2)
                std::cout << " op: != { ";
            eq_exp->Dump();
            std::cout << " , ";
            rel_exp->Dump();
            std::cout << " }";
        }
        else
            rel_exp->Dump();
    }
};

class LAndExpAST : public BaseAST  {
public:
    ASTPtr land_exp;
    ASTPtr eq_exp;
    void Dump()  const override {
        if(land_exp!=NULL){
            std::cout << "LAndExp: { ";
            land_exp->Dump();
            std::cout << " , ";
            eq_exp->Dump();
            std::cout << " }";
        }
        else
            eq_exp->Dump();
    }
};

class LOrExpAST : public BaseAST  {
public:
    ASTPtr lor_exp;
    ASTPtr land_exp;
    void Dump()  const override {
        if(lor_exp!=NULL){
            std::cout << "LOrExp: { ";
            lor_exp->Dump();
            std::cout << " , ";
            land_exp->Dump();
            std::cout << " }";
        }
        else
            land_exp->Dump();
    }
};