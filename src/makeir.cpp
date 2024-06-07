#include "ast/ast.h"
#include "ir/ir.h"
#include "ir/makeir.h"
#include "semantic/enum.h"
#include <iostream>
#define dc(x,n) dynamic_cast<x*>(n.get())


void irCompUnitAST(CompUnitAST* node, Module* module){
    irCompUnitsAST(dc(CompUnitsAST,node->comp_units),module);
}

void irCompUnitsAST(CompUnitsAST* node, Module* module){
    if(node->def)
        irDeclAST(dc(DeclAST,node->def),module);
    if(node->comp_unit)
        irCompUnitsAST(dc(CompUnitsAST,node->comp_unit),module);
}

void irDeclAST(DeclAST* node, Module* module){
    if(node->func_def)
        irFuncDefAST(dc(FuncDefAST,node->func_def),module);
    if(node->var_def)
        irMulVarDefAST(dc(MulVarDefAST,node->var_def),module);
}

void irFuncDefAST(FuncDefAST* node, Module* module){
    auto ty = typehandler(node->func_type);
    auto paramty = irFuncFParamsAST(dc(FuncFParamsAST,node->func_fparams),module);
    auto func = Function::Create(FunctionType::get(ty,paramty),false,node->ident,module);
    auto bb = BasicBlock::Create(func);
    //basic block
    bb->setName("entry");
    Instruction* inst[paramty.size()][2];
    for(int i = 0;i<paramty.size();i++)
    {
        //alloca and store param
        inst[i][0] = AllocaInst::Create(paramty[i]->getPointerElementType(),paramty[i]->getFunctionNumParams(),bb);
        inst[i][1] = StoreInst::Create(func->getArg(i),inst[i][0],bb);
    }
    //alloca return value
    auto ret_al = AllocaInst::Create(ty,ty->getFunctionNumParams(),bb);
    irBlockAST(dc(BlockAST,node->block),module,bb);

}

std::vector<Type*> irFuncFParamAST(FuncFParamsAST* node,Module* module){
    std::vector<Type*> res;
    auto ty = irFuncFParamAST(dc(FuncFParamAST,node->param),module);
    res.push_back(ty);
    if(node->params) {
        auto tmp = irFuncFParamAST(dc(FuncFParamsAST,node->params),module);
        res.insert(res.end(),tmp.begin(),tmp.end());
    }
    return res;
}

Type* irFuncFParamAST(FuncFParamAST* node,Module* module){
    return typehandler(node->param_type);
    //未处理指针
}

void irMulVarDefAST(MulVarDefAST* node, Module* module, BasicBlock* bb = nullptr){
    if(node->var_def)
        irVarDefAST(dc(VarDefAST,node->var_def),module,bb);
    if(node->mul_var_def)
        irMulVarDefAST(dc(MulVarDefAST,node->mul_var_def),module,bb);
}

void irVarDefAST(VarDefAST* node, Module* module, BasicBlock* bb){
    auto ty = typehandler("int");
    int nums = node->int_const;
    if(node->unit){
        int tmp = irConstUnitAST(dc(ConstUnitAST,node->unit),module);
        nums *= tmp;//数组大小
    }
    if(node->isgloble)
    {
        auto var = GlobalVariable::Create(ty,nums,false,node->ident,module);
        var->setName(node->ident);
    }
    else
    {
        auto var = AllocaInst::Create(ty,nums,bb);
        var->setName(node->ident+".addr");
        auto exp = irExpAST(dc(ExpAST,node->exp),module,bb);
        auto storevar = StoreInst::Create(exp,var,bb);
    }
    
}

int irConstUnitAST(ConstUnitAST* node, Module* module){
    if(node->unit)
        return irConstUnitAST(dc(ConstUnitAST,node->unit),module) * node->int_const;
    else 
        return node->int_const;
}

void irBlockAST(BlockAST* node, Module* module, BasicBlock* bb){
    if(node->item)
        irBlockItemAST(dc(BlockItemAST,node->item),module,bb);
}

void irBlockItemAST(BlockItemAST* node, Module* module, BasicBlock* bb){
    if(node->block_item)
        irBlockItemAST(dc(BlockItemAST,node->block_item),module,bb);
    if(node->stmt)
        irStmtAST(dc(StmtAST,node->stmt),module,bb);
    if(node->mul_var_def)
        irMulVarDefAST(dc(MulVarDefAST,node->mul_var_def),module,bb);
}

void irStmtAST(StmtAST* node, Module* module, BasicBlock* bb){
    switch (node->type)
    {
    case 1://assign
        {
            auto lval = irLvalAST(dc(LvalAST,node->lval),module,bb);
            auto exp = irExpAST(dc(ExpAST,node->exp),module,bb);
            //load 的ptr是vardef 的alloca的指针 
            //这里需要符号表
            auto load = LoadInst::Create(,bb);
            auto store = StoreInst::Create(exp,load,bb);
            break;
        }
    }
    
}

/*
case 1:
                PFX;
                std::cout <<"Assign Stmt{ "<<std::endl;
                lval->level = this->level + 1;
                lval->Dump();
                exp->level = this->level + 1;
                exp->Dump();
                PFX;   
                std::cout <<"}"<<std::endl;
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
                std::cout << "If Stmt:{ "<<std::endl;
                PFX;
                std::cout << "condition( "<<std::endl;
                exp->level = this->level + 1;
                exp->Dump();
                PFX;
                std::cout << ") "<<std::endl;
                PFX;
                std::cout << "then("<<std::endl;
                stmt->level = this->level+1;
                //std::cout << "debug"<<std::endl;
                stmt->Dump();
                PFX;
                std::cout << ") "<<std::endl;
                PFX;
                std::cout << "}"<<std::endl;
                break;
            case 5:
                PFX;
                std::cout << "If Stmt{ "<<std::endl;
                PFX;
                std::cout << "condition( "<<std::endl;
                exp->level = this->level + 1;
                exp->Dump();
                PFX;
                std::cout << ") "<<std::endl;
                PFX;
                std::cout << "then("<<std::endl;
                stmt->level = this->level + 1;
                stmt->Dump();
                PFX;
                std::cout << ") "<<std::endl;
                PFX;
                std::cout << "else{ "<<std::endl;
                stmt2->level = this->level + 1;
                stmt2->Dump();
                PFX;
                std::cout <<"}"<<std::endl;
                break;
            case 6:
                PFX;
                std::cout << "While Stmt:{ "<<std::endl;
                PFX;
                std::cout << "condition( "<<std::endl;
                exp->level = this->level + 1;
                exp->Dump();
                PFX;
                std::cout << ")"<<std::endl;
                stmt->level = this->level + 1;
                stmt->Dump();
                PFX;
                std::cout <<"}"<<std::endl;
                break;
            case 7:
                PFX;
                std::cout << "Break Stmt"<<std::endl;
                break;
            case 8:
                PFX;
                std::cout << "Continue Stmt"<<std::endl;
                 
                break;
            case 9:
                PFX;
                std::cout << "Return Stmt: {}"<<std::endl;
                break;
            case 10:
                PFX;
                std::cout << "Return Stmt: { "<<std::endl;
                exp->level = this->level + 1;
                exp->Dump();
                PFX;
                std::cout << "} "<<std::endl;
                break;
            default:
                break;*/
