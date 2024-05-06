#pragma once
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <string>
#include <memory>
using namespace std;
#define ENDL std::cout<<endl<<prefix(this->level) 
#define PF std::cout<<prefix(this->level-1)
#define PFX std::cout<<prefix(this->level)
#define PFXX std::cout<<prefix(this->level+1)
std::string prefix(int level);
class BaseAST {
public:
    virtual ~BaseAST() = default;
    virtual void Dump() const = 0;
    int level = 0;
};
using ASTPtr = std::unique_ptr<BaseAST>;


class FuncDefAST : public BaseAST  {
public:
    std::string func_type;
    std::string ident;
    ASTPtr func_fparams;
    ASTPtr block;

    void Dump() const override {
        PFX;
        std::cout << "FuncDef { "<<endl;
        PFXX;
        std::cout << "func_type : "<< func_type<<endl;
        PFXX;
        std::cout << "ident : "<< ident<<endl;
        if(func_fparams!=NULL){
            func_fparams->level = this->level + 1;
            PFXX;
            std::cout <<"FuncFParams { "<<endl;
            func_fparams->Dump();
            PFXX;
            std::cout <<"}"<<endl;
        }
        block->level = this->level + 1;
        block->Dump();
        PFX;
        std::cout << "}"<<endl;
         
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
        PFX;
        std::cout <<"VarDef { "<<endl;
        PFXX;
        std::cout <<"var_type : "<< "int"<<endl;
        PFXX;
        std::cout << "ident : "<< ident<<endl;
        switch (type) {
            case 1:
                exp->level = this->level+1;
                exp->Dump();
                break;
            case 3:
                PFXX;
                std::cout << "/array"<<endl;
                //PFXX;
                //std::cout << "int_const: "<<int_const<<endl;
                if(unit!=NULL){
                    unit ->level = this->level;
                    unit->Dump();
                }
                PFXX;
                std::cout << "array/"<<endl;
                break;
            default:
                break;
        }
        PFX;
        std::cout <<"}"<<endl;
    }
};

class DeclAST : public BaseAST {
public:
    ASTPtr func_def;
    ASTPtr var_def;
    void Dump() const override{  
        if(func_def){
            func_def->level = this->level;
            func_def->Dump();
        }
        if(var_def){
            var_def->level = this->level;
            var_def->Dump();
        }
    }
};

class CompUnitAST : public BaseAST{
public:
    ASTPtr comp_units;

    void Dump() const override{
        PFX;
        std::cout <<"CompUnit {"<<endl;
         
        comp_units->level = this->level + 1;
        comp_units->Dump();
        PFX;
        std::cout <<"}"<<endl;
         
    }
};

class CompUnitsAST : public BaseAST {
public:
    ASTPtr comp_unit;
    ASTPtr def;

    void Dump() const override{
        
        if(def) {
            def->level = this->level;
            def->Dump();
        }
        if(comp_unit){
            comp_unit->level = this->level;
            comp_unit->Dump();
        }
    }
};


class MulVarDefAST : public BaseAST  {
public:
    ASTPtr mul_var_def;
    ASTPtr var_def;
    void Dump() const override{
        if(mul_var_def){
            mul_var_def->level = this->level;
            mul_var_def->Dump();
        }
        if(var_def) {
            var_def->level = this->level;
            var_def->Dump();
        }
    }
};

class ConstUnitAST  : public BaseAST {
public:
    int int_const;
    ASTPtr unit;
    void Dump()    const override{
        PFXX;
        std::cout << "int_const: "<<int_const<<endl;
        if(unit!=nullptr){
            unit->level = this->level;
            unit->Dump();
        }
         
    }
};

class FuncFParamsAST : public BaseAST  {
public:
    ASTPtr param;
    ASTPtr params;
    void Dump()   const override {
        param->level = this->level;
        param->Dump();
        if(params!=NULL){
            params->level = this->level;
            params->Dump();
        }
    }
};

class FuncFParamAST : public BaseAST  {
public:
    std::string ident;
    std::string param_type = "int";
    ASTPtr unit;
    int type;
    void Dump()  const override  {
        PFXX;
        std::cout << "ident : "<<ident<<" ";
        std::cout << "param_type : "<< param_type;
        if(type == 1)
            std::cout <<"[]";
        std::cout<<endl;
        if(unit!=NULL){
            unit->level = this->level;
            unit->Dump();
        }
    }
};



class BlockAST : public BaseAST  {
public:
    ASTPtr item;
    void Dump()  const override {
        PFX;
        std::cout <<"Block { "<<endl;
        if(item!=NULL){
            item->level = this->level + 1;
            item->Dump();
        }
        PFX;
        std::cout << "}"<<endl;
    }
};

class BlockItemAST : public BaseAST  {
public:
    ASTPtr block_item;
    ASTPtr mul_var_def;
    ASTPtr stmt;
    void Dump()  const override {
        if(block_item!=NULL){
            block_item->level = this->level;
            block_item->Dump();
        }
        if(mul_var_def!=NULL){
            mul_var_def->level = this->level;
            mul_var_def->Dump();
        }
        if(stmt!=NULL){
            stmt->level = this->level;
            stmt->Dump();
        }
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
                PFX;
                std::cout <<"Assign Stmt{ "<<endl;
                lval->level = this->level + 1;
                lval->Dump();
                exp->level = this->level + 1;
                exp->Dump();
                PFX;   
                std::cout <<"}"<<endl;
                break;
            case 2:
                exp->level = this->level;
                exp->Dump();
                break;
            case 3:
                block->level = this->level;
                block->Dump();
                break;
            case 4:
                PFX;
                std::cout << "If Stmt:{ "<<endl;
                PFX;
                std::cout << "condition( "<<endl;
                exp->level = this->level + 1;
                exp->Dump();
                PFX;
                std::cout << ") "<<endl;
                PFX;
                std::cout << "then("<<endl;
                stmt->level = this->level+1;
                //std::cout << "debug"<<endl;
                stmt->Dump();
                PFX;
                std::cout << ") "<<endl;
                PFX;
                std::cout << "}"<<endl;
                break;
            case 5:
                PFX;
                std::cout << "If Stmt{ "<<endl;
                PFX;
                std::cout << "condition( "<<endl;
                exp->level = this->level + 1;
                exp->Dump();
                PFX;
                std::cout << ") "<<endl;
                PFX;
                std::cout << "then("<<endl;
                stmt->level = this->level + 1;
                stmt->Dump();
                PFX;
                std::cout << ") "<<endl;
                PFX;
                std::cout << "else{ "<<endl;
                stmt2->level = this->level + 1;
                stmt2->Dump();
                PFX;
                std::cout <<"}"<<endl;
                break;
            case 6:
                PFX;
                std::cout << "While Stmt:{ "<<endl;
                PFX;
                std::cout << "condition( "<<endl;
                exp->level = this->level + 1;
                exp->Dump();
                PFX;
                std::cout << ")"<<endl;
                stmt->level = this->level + 1;
                stmt->Dump();
                PFX;
                std::cout <<"}"<<endl;
                break;
            case 7:
                PFX;
                std::cout << "Break Stmt"<<endl;
                break;
            case 8:
                PFX;
                std::cout << "Continue Stmt"<<endl;
                 
                break;
            case 9:
                PFX;
                std::cout << "Return Stmt: {}"<<endl;
                break;
            case 10:
                PFX;
                std::cout << "Return Stmt: { "<<endl;
                exp->level = this->level + 1;
                exp->Dump();
                PFX;
                std::cout << "} "<<endl;
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
        if(lorexp!=NULL){
            lorexp->level = this->level;
            lorexp->Dump();
        }
    }
};

class LvalAST : public BaseAST  {
public:
    std::string ident;
    ASTPtr unit;
    void Dump()  const override {
        PFX;
        std::cout << "ident: "<<ident<<" "<<endl;
        if(unit!=NULL){
            PFX;
            std::cout << "/array"<<endl;
            unit->level = this->level;
            unit->Dump();
            PFX;
            std::cout << "array/"<<endl;
        }

    }
};

class LvalUnitAST : public BaseAST  {
public:
    ASTPtr exp;
    ASTPtr unit;
    void Dump()  const override {
        if (unit!=NULL){
            unit->level = this->level;
            unit->Dump();
        }
    
        exp->level = this->level;
        exp->Dump();
    }
};

class PrimaryExpAST : public BaseAST  {
public:
    int int_const;
    ASTPtr exp;
    ASTPtr lval;
    int type;
    void Dump()  const override {
        if(exp!=NULL){
            exp->level = this->level;
            exp->Dump();
        }
        if(lval!=NULL){
            lval->level = this->level;
            lval->Dump();
        }
        if(exp == NULL && lval == NULL)
        {
            //if(type == 3)
            //    std::cout<< int_const << "";
            //else 
            {
                PFX;
                std::cout << "int_const: "<< int_const << " "<<endl;
            }
        }
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
                primary_exp->level = this->level;
                primary_exp->Dump();
                break;
            case 2:
                PFX;
                std::cout << "ident: "<< ident<<endl;
                break;
            case 3:
                PFX;
                std::cout << "FuncCall {"<<endl;
                PFXX;
                std::cout << "ident: "<< ident<<endl;
                PFXX;
                std::cout << "FuncRParams: { "<<endl;
                 
                params->level = this->level + 1;
                params->Dump();
                PFXX;
                std::cout <<"}"<<endl;
                PFX;
                std::cout <<"}"<<endl;
                 
                break;
            case 4:
                PFX;
                std::cout <<"UnaryExp {"<<endl;

                PFXX;
                switch (op_type){
                    case 1:
                        std::cout << "op: + "<<endl;
                        break;
                    case 2:
                        std::cout << "op: -  "<<endl;
                        break;
                    case 3:
                        std::cout << "op: !  "<<endl;
                        break;
                }
                 
                unary_exp->level = this->level + 1;
                unary_exp->Dump();
                PFX;
                std::cout << "}"<<endl;
                 
                break;
        }
    }
};

class FuncRParamsAST : public BaseAST  {
public:
    ASTPtr exp;
    ASTPtr params;
    void Dump()  const override {
        if(params!=NULL){
            params->level = this->level;
            params->Dump();
        }
        exp->level = this->level+1;
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
            PFX;
            std::cout << "MulExp{"<<endl;
            
            PFXX;
            if(op_type == 1)
                std::cout << "op: mul"<<endl;
            else if (op_type == 2)
                std::cout << "op: div"<<endl;
            else if (op_type == 3)
                std::cout << "op: mod"<<endl;
             
            mul_exp->level = this->level+1;
            mul_exp->Dump();
             
            unary_exp->level = this->level+1;
            unary_exp->Dump();
            PFX;
            std::cout << "}"<<endl;
             
        }
        else {
            unary_exp->level = this->level;
            unary_exp->Dump();
        }
    }
};

class AddExpAST : public BaseAST  {
public:
    ASTPtr add_exp;
    ASTPtr mul_exp;
    int op_type;
    void Dump()  const override {
        if(add_exp!=NULL){
            PFX;
            std::cout << "AddExp{"<<endl;

            PFXX;
            if(op_type == 1)
                std::cout << "op: add "<<endl;
            else if (op_type == 2)
                std::cout << "op: sub "<<endl;
             
            add_exp->level = this->level+1;
            add_exp->Dump();
             
            mul_exp->level = this->level+1;
            mul_exp->Dump();
            PFX;
            std::cout << "}"<<endl;
             
        }
        else{
            mul_exp->level = this->level;
            mul_exp->Dump();
        }
        
    }
};

class RelExpAST : public BaseAST  {
public:
    ASTPtr rel_exp;
    ASTPtr add_exp;
    int op_type;
    void Dump()  const override {
        if(rel_exp!=NULL){
            PFXX;
            std::cout << "RelExp{"<<endl;
            PFXX;
            if(op_type == 1)
                std::cout << "op: < "<<endl;
            else if (op_type == 2)
                std::cout << "op: > "<<endl;
            else if (op_type == 3)
                std::cout << "op: <= "<<endl;
            else if (op_type == 4)
                std::cout << "op: >= "<<endl;
             
            rel_exp->level = this->level+1;
            rel_exp->Dump();
             
            add_exp->level = this->level+1;
            add_exp->Dump();
            PFX;
            std::cout << "}"<<endl;
             
        }
        else{
            add_exp->level = this->level;
            add_exp->Dump();
        }
    }
};

class EqExpAST : public BaseAST  {
public:
    ASTPtr eq_exp;
    ASTPtr rel_exp;
    int op_type;
    void Dump()  const override {
        if(eq_exp!=NULL){
            PFX;
            std::cout << "EqExp{"<<endl;
            PFXX;
            if(op_type == 1)
                std::cout << "op: == "<<endl;
            else if (op_type == 2)
                std::cout << "op: != "<<endl;
             
            eq_exp->level = this->level+1;
            eq_exp->Dump();
             
            rel_exp->level = this->level+1;
            rel_exp->Dump();
            PFX;
            std::cout << "}"<<endl;
             
        }
        else{
            rel_exp->level = this->level;
            rel_exp->Dump();
        }
    }
};

class LAndExpAST : public BaseAST  {
public:
    ASTPtr land_exp;
    ASTPtr eq_exp;
    void Dump()  const override {
        if(land_exp!=NULL){
            PFX;
            std::cout << "LAndExp{ "<<endl;
             
            land_exp->level = this->level+1;
            land_exp->Dump();
             
            eq_exp->level = this->level+1;
            eq_exp->Dump();
            PFX;
            std::cout << "}"<<endl;
             
        }
        else{
            eq_exp->level = this->level;
            eq_exp->Dump();
        }
    }
};

class LOrExpAST : public BaseAST  {
public:
    ASTPtr lor_exp;
    ASTPtr land_exp;
    void Dump()  const override {
        if(lor_exp!=NULL){
            PFX;
            std::cout << "LOrExp { "<<endl;
             
            lor_exp->level = this->level+1;
            lor_exp->Dump();
             
            land_exp->level = this->level+1;
            land_exp->Dump();
            PFX;
            std::cout << "}"<<endl;
             
        }
        else{
            land_exp->level = this->level;
            land_exp->Dump();
        }
    }
};