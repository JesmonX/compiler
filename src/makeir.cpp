#include "ast/ast.h"
#include "ir/ir.h"
#include "ir/makeir.h"
#include "semantic/enum.h"
#include <iostream>
#include <optional>
#define dc(x,n) dynamic_cast<x*>(n.get())


void irCompUnitAST(CompUnitAST* node, Module* module, symtab* symtable){
    std::cout<<"debug:irCompUnitAST"<<std::endl;
    irCompUnitsAST(dc(CompUnitsAST,node->comp_units),module,symtable);
}

void irCompUnitsAST(CompUnitsAST* node, Module* module, symtab* symtable){
    std::cout<<"debug:irCompUnitsAST"<<std::endl;

    if(node->comp_unit)
        irCompUnitsAST(dc(CompUnitsAST,node->comp_unit),module,symtable);
    if(node->def)
        irDeclAST(dc(DeclAST,node->def),module,symtable);
    
}

void irDeclAST(DeclAST* node, Module* module, symtab* symtable){
    std::cout<<"debug:irDeclAST"<<std::endl;

    if(node->func_def)
        irFuncDefAST(dc(FuncDefAST,node->func_def),module,symtable);
    if(node->var_def)
        irMulVarDefAST(dc(MulVarDefAST,node->var_def),module,symtable,nullptr);
}

void irFuncDefAST(FuncDefAST* node, Module* module, symtab* symtable){
    std::cout<<"debug:irFuncDefAST"<<std::endl;
    auto ty = typehandler(node->func_type);
    int num = (ty==Type::getIntegerTy())?1:0;
    param_list paramty = param_list({{}},std::vector<Type*>(),std::vector<std::string_view>());
    
    if(node->func_fparams)
        paramty = irFuncFParamsAST(dc(FuncFParamsAST,node->func_fparams),module,symtable);

    auto func = Function::Create(FunctionType::get(ty,paramty.types),false,node->ident,module);
    auto bb = BasicBlock::Create(func);
    //basic block
    bb->setName("entry");
    if(node->func_fparams != nullptr)
    {
        Instruction* inst[paramty.types.size()][2];
        for(int i = 0;i<paramty.types.size();i++)
        {
            //alloca and store param
            int size = 1;
            for(auto j:paramty.dims[i])
                size *= j; //size of this param

            inst[i][0] = AllocaInst::Create(paramty.types[i],size,bb);

            //update symbol table
            symtable->insert_or_assign(paramty.names[i],inst[i][0]);
            if(paramty.types[i]->getTypeID() == Type::PointerTyID)
                symtable->set_dims(paramty.names[i],paramty.dims[i]);
            
            inst[i][1] = StoreInst::Create(func->getArg(i),inst[i][0],bb);

        }
    }
    
    //alloca return value

    //PointerType *t = PointerType::get(ty);
    auto ret_al = AllocaInst::Create(ty,num,bb);
    ret_al->setName("ret.addr");
    auto retbb = BasicBlock::Create(func);
    retbb->setName("return");
    auto retinfo = ret_info(ret_al,retbb);
    irBlockAST(dc(BlockAST,node->block),module,bb,symtable,&retinfo);

}

param_list irFuncFParamsAST(FuncFParamsAST* node,Module* module,symtab* symtable){
    std::cout<<"debug:irFuncFParamsAST"<<std::endl;
    std::vector<std::vector<int>> dims;
    std::vector<Type*> types;
    std::vector<std::string_view> names;
        
    auto param = irFuncFParamAST(dc(FuncFParamAST,node->param),module,symtable);
    
    if(node->params) {
        auto tmp = irFuncFParamsAST(dc(FuncFParamsAST,node->params),module,symtable);
        dims.insert(dims.begin(),tmp.dims.begin(),tmp.dims.end());
        types.insert(types.begin(),tmp.types.begin(),tmp.types.end());
        names.insert(names.begin(),tmp.names.begin(),tmp.names.end());
    }
    dims.push_back(param.dim);
    types.push_back(param.type);
    names.push_back(param.name);
    return param_list(dims,types,names);
}
//需要传回去的
//名字··符号表
//size..alloca
//type..alloca ..vector

param_info irFuncFParamAST(FuncFParamAST* node,Module* module,symtab* symtable){
    std::cout<<"debug:irFuncFParamAST"<<std::endl;
    std::vector<int> dim;
    if(node->unit)
        dim = irConstUnitAST(dc(ConstUnitAST,node->unit),module,symtable);

    auto type = Type::getIntegerTy();
    if(node->type!=0)
        auto type = PointerType::get(Type::getIntegerTy());
    auto name = node->ident;
    return param_info(name,type,dim);
}

void irMulVarDefAST(MulVarDefAST* node, Module* module, symtab* symtable, BasicBlock* bb){
    std::cout<<"debug:irMulVarDefAST"<<std::endl;
    if(node->var_def)
        irVarDefAST(dc(VarDefAST,node->var_def),module,bb,symtable);
    if(node->mul_var_def)
        irMulVarDefAST(dc(MulVarDefAST,node->mul_var_def),module,symtable,bb);
}

void irVarDefAST(VarDefAST* node, Module* module, BasicBlock* bb, symtab* symtable){
    std::cout<<"debug:irVarDefAST"<<std::endl;
    auto ty = typehandler("int");
    int nums = 1;
    std::vector<int> units;
    if(node->unit){
        units = irConstUnitAST(dc(ConstUnitAST,node->unit),module,symtable);
        for(auto i:units)
            nums *= i;//计算数组大小
    }
    if(node->isgloble)
    {
        auto var = GlobalVariable::Create(ty,nums,false,node->ident,module);
        var->setName(node->ident);
        symtable->set_both(node->ident,var,units);
    }
    else
    {
        auto var = AllocaInst::Create(ty,nums,bb);
        std::string_view name = node->ident;
        symtable->set_both(name,var,units);
        std::cout<<name<<std::endl;
        std::cout<<symtable->find_dims(name).size()<<std::endl;
        var->setName(node->ident+".addr");
        if(node->exp){
            auto exp = irExpAST(dc(ExpAST,node->exp),module,bb,symtable);
            auto storevar = StoreInst::Create(exp,var,bb);
        }
    }
    
}

std::vector<int> irConstUnitAST(ConstUnitAST* node, Module* module, symtab* symtable){
    std::cout<<"debug:irConstUnitAST"<<std::endl;
    std::vector<int> res;
    res.push_back(node->int_const);
    if(node->unit)
    {
        auto tmp = irConstUnitAST(dc(ConstUnitAST,node->unit),module,symtable);
        res.insert(res.end(),tmp.begin(),tmp.end());
    }
    return res;
}

BasicBlock* irBlockAST(BlockAST* node, Module* module, BasicBlock* bb, symtab* symtable, ret_info* ret, while_info* whi){
    std::cout<<"debug:irBlockAST"<<std::endl;
    symtable->enter();
    if(node->item)
    {
        auto res = irBlockItemAST(dc(BlockItemAST,node->item),module,bb,symtable,ret);
        symtable->exit();
        return res;
    }
    
    return bb;
}

BasicBlock* irBlockItemAST(BlockItemAST* node, Module* module, BasicBlock* bb, symtab* symtable, ret_info* ret, while_info* whi){
    std::cout<<"debug:irBlockItemAST"<<std::endl;
    BasicBlock* res = bb;
    if(node->block_item)
        res = irBlockItemAST(dc(BlockItemAST,node->block_item),module,bb,symtable,ret);
    //if cur bb changed in the block item , update the bb as res
    if(node->stmt)
        res = irStmtAST(dc(StmtAST,node->stmt),module,res,symtable,ret);
    if(node->mul_var_def)
        irMulVarDefAST(dc(MulVarDefAST,node->mul_var_def),module,symtable,res);
    return res;
}

BasicBlock* irStmtAST(StmtAST* node, Module* module, BasicBlock* bb, symtab* symtable, ret_info* ret, while_info* whi){
    std::cout<<"debug:irStmtAST"<<std::endl;
    switch (node->type){
    case 1://assign
        {
            //load 的ptr是vardef 的alloca的指针 
            //这里需要符号表
            auto lval = irLvalAST(dc(LvalAST,node->lval),module,bb,symtable);//addr
            auto exp = irExpAST(dc(ExpAST,node->exp),module,bb,symtable);

            if(lval.second.size() != 0)
            {
                auto bound = get_bounds(symtable->find_dims(lval.first->getName().substr(0,lval.first->getName().find("."))));
                std::cout<<"bound"<<bound.size()<<std::endl;
                std::cout<<"lval"<<lval.second.size()<<std::endl;
                auto offset = OffsetInst::Create(Type::getIntegerTy(),lval.first,lval.second,bound,bb);
                auto store = StoreInst::Create(exp,offset,bb);
            }
            else 
            {
                auto store = StoreInst::Create(exp,lval.first,bb);
            }
            
            return bb;
        }
    case 2://exp
        {
            irExpAST(dc(ExpAST,node->exp),module,bb,symtable);
            return bb;
        }
    case 3://block
        {
            return irBlockAST(dc(BlockAST,node->block),module,bb,symtable,ret);
            
        }
    case 4://if
        {
            auto cond = irExpAST(dc(ExpAST,node->exp),module,bb,symtable);
            auto func = bb->getParent();
            auto truebb = BasicBlock::Create(func,ret->bb);
            //auto falsebb = BasicBlock::Create(func);
            auto endbb = BasicBlock::Create(func,ret->bb);
            auto br = BranchInst::Create(truebb,endbb,cond,bb);
            auto tempbb = irStmtAST(dc(StmtAST,node->stmt),module,truebb,symtable,ret);
            auto jp = JumpInst::Create(endbb,tempbb);
            //irStmtAST(dc(StmtAST,node->stmt2),module,falsebb,symtable);
            return endbb;
        }
    case 5://if else
        {
            auto cond = irExpAST(dc(ExpAST,node->exp),module,bb,symtable);
            auto func = bb->getParent();
            auto truebb = BasicBlock::Create(func,ret->bb);
            auto falsebb = BasicBlock::Create(func,ret->bb);
            auto endbb = BasicBlock::Create(func,ret->bb);
            
            auto br = BranchInst::Create(truebb,falsebb,cond,bb);
            
            auto tempbb1 = irStmtAST(dc(StmtAST,node->stmt),module,truebb,symtable,ret);
            auto jp1 = JumpInst::Create(endbb,tempbb1);
            auto tempbb2 = irStmtAST(dc(StmtAST,node->stmt2),module,falsebb,symtable,ret);
            auto jp2 = JumpInst::Create(endbb,tempbb2);
            return endbb;
        }
    case 6://while
        {
            auto func = bb->getParent();
            auto entrybb = BasicBlock::Create(func,ret->bb);
            auto truebb = BasicBlock::Create(func,ret->bb);
            auto endbb = BasicBlock::Create(func,ret->bb);
            auto whileinfo = while_info(entrybb,truebb,endbb);
            auto jp = JumpInst::Create(entrybb,bb);
            auto cond = irExpAST(dc(ExpAST,node->exp),module,bb,symtable);

            auto br = BranchInst::Create(truebb,endbb,cond,entrybb);
            auto tempbb = irStmtAST(dc(StmtAST,node->stmt),module,truebb,symtable,ret,&whileinfo);
            auto jp2 = JumpInst::Create(entrybb,tempbb);
            
            return endbb;
        }
    case 7://break uncertain
        {
            auto jp = JumpInst::Create(whi->end,bb);
            return bb;
        }
    case 8://continue uncertain
        {
            auto jp = JumpInst::Create(whi->entry,bb);
            return bb;
        }
    case 9://return
        {
            auto jp = JumpInst::Create(ret->bb,bb);
            auto voidval = ConstantUnit::Create();
            auto retinst = RetInst::Create(voidval,ret->bb);
            return nullptr;
        }
    case 10://return exp
        {
            auto retexp = irExpAST(dc(ExpAST,node->exp),module,bb,symtable);
            //auto store = StoreInst::Create(retexp,ret->addr,bb);
            auto jp = JumpInst::Create(ret->bb,bb);
            //auto load = LoadInst::Create(ret->addr,ret->bb);
            auto retinst = RetInst::Create(retexp,ret->bb);
            
            return nullptr;
        }
    
    }

    
}

lvalpair irLvalAST(LvalAST* node, Module* module, BasicBlock* bb, symtab* symtable)
{
    std::cout<<"debug:irLvalAST"<<std::endl;
    auto res = symtable->find(node->ident);
    std::vector<Value*> units;
    if(node->unit)
        units = irLvalUnitAST(dc(LvalUnitAST,node->unit),module,bb,symtable);
    return lvalpair(res,units);
}



Value* irExpAST(ExpAST* node, Module* module, BasicBlock* bb, symtab* symtable)
{
    std::cout<<"debug:irExpAST"<<std::endl;

    return irLOrExpAST(dc(LOrExpAST,node->lorexp),module,bb,symtable);
}


Value* irLOrExpAST(LOrExpAST* node, Module* module, BasicBlock* bb, symtab* symtable)
{
    std::cout<<"debug:irLOrExpAST"<<std::endl;
    if(node->lor_exp)
    {
        auto l = irLOrExpAST(dc(LOrExpAST,node->lor_exp),module,bb,symtable);
        //short cut
        auto debug = l->getType();
        std::cout <<"[DEBUG]"<< debug->getTypeID() << std::endl;
        if(static_cast<ConstantInt*>(l)->getValue() == 1)
            return l;
        
        auto r = irLAndExpAST(dc(LAndExpAST,node->land_exp),module,bb,symtable);
        auto ty = typehandler("int");
        auto res = BinaryInst::Create(BinaryInst::Or,l,r,ty,bb);
        std::cout<<"type:lor"<<std::endl;
        std::cout<<r->getType()->getTypeID()<<std::endl;
        std::cout<<l->getType()->getTypeID()<<std::endl;
        return res;
    }
    else
        return irLAndExpAST(dc(LAndExpAST,node->land_exp),module,bb,symtable);
}

Value* irLAndExpAST(LAndExpAST* node, Module* module, BasicBlock* bb, symtab* symtable)
{
    std::cout<<"debug:irLAndExpAST"<<std::endl;
    if(node->land_exp)
    {
        auto l = irLAndExpAST(dc(LAndExpAST,node->land_exp),module,bb,symtable);

        if(static_cast<ConstantInt*>(l)->getValue() == 0)
            return l;

        auto r = irEqExpAST(dc(EqExpAST,node->eq_exp),module,bb,symtable);
        auto ty = typehandler("int");
        auto res = BinaryInst::Create(BinaryInst::And,l,r,ty,bb);
        std::cout<<"type:land"<<std::endl;
        std::cout<<r->getType()->getTypeID()<<std::endl;
        std::cout<<l->getType()->getTypeID()<<std::endl;
        return res;
    }
    else
        return irEqExpAST(dc(EqExpAST,node->eq_exp),module,bb,symtable);
}

Value* irEqExpAST(EqExpAST* node, Module* module, BasicBlock* bb, symtab* symtable)
{
    std::cout<<"debug:irEqExpAST"<<std::endl;
    if(node->eq_exp)
    {
        auto l = irEqExpAST(dc(EqExpAST,node->eq_exp),module,bb,symtable);
        auto r = irRelExpAST(dc(RelExpAST,node->rel_exp),module,bb,symtable);
        auto ty = typehandler("int");
        std::cout<<"type:eq"<<std::endl;
        std::cout<<r->getType()->getTypeID()<<std::endl;
        std::cout<<l->getType()->getTypeID()<<std::endl;
        std::cout<<l->getName()<<std::endl;
        std::cout<<l->getValueID()<<std::endl;
        if(node->op_type == 1)
            return BinaryInst::Create(BinaryInst::Eq,l,r,ty,bb);
        else
            return BinaryInst::Create(BinaryInst::Ne,l,r,ty,bb);
    }
    else
        return irRelExpAST(dc(RelExpAST,node->rel_exp),module,bb,symtable);
}

Value* irRelExpAST(RelExpAST* node, Module* module, BasicBlock* bb, symtab* symtable)
{
    std::cout<<"debug:irRelExpAST"<<std::endl;
    if(node->rel_exp)
    {
        auto l = irRelExpAST(dc(RelExpAST,node->rel_exp),module,bb,symtable);
        auto r = irAddExpAST(dc(AddExpAST,node->add_exp),module,bb,symtable);
        auto ty = typehandler("int");
        std::cout<<"type:rel"<<std::endl;
        std::cout<<r->getType()->getTypeID()<<std::endl;
        std::cout<<l->getType()->getTypeID()<<std::endl;
        if(node->op_type == 1)
            return BinaryInst::Create(BinaryInst::Lt,l,r,ty,bb);
        else if(node->op_type == 2)
            return BinaryInst::Create(BinaryInst::Gt,l,r,ty,bb);
        else if(node->op_type == 3)
            return BinaryInst::Create(BinaryInst::Le,l,r,ty,bb);
        else
            return BinaryInst::Create(BinaryInst::Ge,l,r,ty,bb);
    }
    else
        return irAddExpAST(dc(AddExpAST,node->add_exp),module,bb,symtable);
}

Value* irAddExpAST(AddExpAST* node, Module* module, BasicBlock* bb, symtab* symtable)
{
    std::cout<<"debug:irAddExpAST"<<std::endl;
    if(node->add_exp)
    {
        
        auto l = irAddExpAST(dc(AddExpAST,node->add_exp),module,bb,symtable);
        auto r = irMulExpAST(dc(MulExpAST,node->mul_exp),module,bb,symtable);
        auto ty = typehandler("int");
        std::cout<<"type:add"<<std::endl;
        std::cout<<r->getType()->getTypeID()<<std::endl;
        std::cout<<l->getType()->getTypeID()<<std::endl;
        if(node->op_type == 1)
            return BinaryInst::Create(BinaryInst::Add,l,r,ty,bb);
        else
            return BinaryInst::Create(BinaryInst::Sub,l,r,ty,bb);
    }
    else
        return irMulExpAST(dc(MulExpAST,node->mul_exp),module,bb,symtable);
}

Value* irMulExpAST(MulExpAST* node, Module* module, BasicBlock* bb, symtab* symtable)
{
    std::cout<<"debug:irMulExpAST"<<std::endl;
    if(node->mul_exp)
    {
        auto l = irMulExpAST(dc(MulExpAST,node->mul_exp),module,bb,symtable);
        auto r = irUnaryExpAST(dc(UnaryExpAST,node->unary_exp),module,bb,symtable);
        auto ty = typehandler("int");
        std::cout<<"type:mul"<<std::endl;
        std::cout<<r->getType()->getTypeID()<<std::endl;
        std::cout<<l->getType()->getTypeID()<<std::endl;
        if(node->op_type == 1)
            return BinaryInst::Create(BinaryInst::Mul,l,r,ty,bb);
        else if(node->op_type == 2)
            return BinaryInst::Create(BinaryInst::Div,l,r,ty,bb);
        else
            return BinaryInst::Create(BinaryInst::Mod,l,r,ty,bb);
    }
    else
        return irUnaryExpAST(dc(UnaryExpAST,node->unary_exp),module,bb,symtable);
}

Value* irUnaryExpAST(UnaryExpAST* node, Module* module, BasicBlock* bb, symtab* symtable)
{
    std::cout<<"debug:irUnaryExpAST"<<std::endl;
    switch (node->type)
    {
    case 1:
        {
            return irPrimaryExpAST(dc(PrimaryExpAST,node->primary_exp),module,bb,symtable);

        }
    case 2://call
        {
            std::vector<Value*> args;//none args
            args.resize(0);
            auto func = module->getFunction(std::string_view(node->ident));
            auto call = CallInst::Create(func,args,bb);
            return call;
        }
    case 3://call and args
        {
            auto args = irFuncRParamsAST(dc(FuncRParamsAST,node->params),module,bb,symtable);
            auto func = module->getFunction(node->ident);
            auto call = CallInst::Create(func,args,bb);
            return call;
        }
    case 4://unary op
        {
            auto exp = irUnaryExpAST(dc(UnaryExpAST,node->unary_exp),module,bb,symtable);
            auto ty = typehandler("int");
            auto zero = ConstantInt::Create(0);
            std::cout<<"type:unary"<<std::endl;
            
            std::cout<<exp->getType()->getTypeID()<<std::endl;
            std::cout<<zero->getType()->getTypeID()<<std::endl;
            if(node->op_type == 1)
                return BinaryInst::Create(BinaryInst::Add,zero,exp,ty,bb);
            else if(node->op_type == 2)
                return BinaryInst::Create(BinaryInst::Sub,zero,exp,ty,bb);
            else if(node->op_type == 3)
                return BinaryInst::Create(BinaryInst::Eq,zero,exp,ty,bb);
        }
    }
}

std::vector<Value*> irFuncRParamsAST(FuncRParamsAST* node, Module* module, BasicBlock* bb, symtab* symtable)
{
    std::cout<<"debug:irFuncRParamsAST"<<std::endl;
    std::vector<Value*> res;
    if(node->params)
    {
        auto tmp = irFuncRParamsAST(dc(FuncRParamsAST,node->params),module,bb,symtable);
        res.insert(res.begin(),tmp.begin(),tmp.end());
    }
    auto exp = irExpAST(dc(ExpAST,node->exp),module,bb,symtable);
    res.push_back(exp);
    return res;
}

Value* irPrimaryExpAST(PrimaryExpAST* node, Module* module, BasicBlock* bb, symtab* symtable)
{
    std::cout<<"debug:irPrimaryExpAST"<<std::endl;

    if(node->exp)
        return irExpAST(dc(ExpAST,node->exp),module,bb,symtable);
    else if(node->lval)
    {
        auto lval = irLvalAST(dc(LvalAST,node->lval),module,bb,symtable);

        if(lval.second.size() != 0)
        {
            auto bound = get_bounds(symtable->find_dims(lval.first->getName().substr(0,lval.first->getName().find("."))));
            std::cout<<"bound"<<bound.size()<<std::endl;
            std::cout<<"lval"<<lval.second.size()<<std::endl;
            auto offset = OffsetInst::Create(Type::getIntegerTy(),lval.first,lval.second,bound,bb);
            auto load = LoadInst::Create(offset,bb);
            return load;
            
        }
        else{
            auto load = LoadInst::Create(lval.first,bb);
            return load;
        }

    }
    else 
    {
        auto constint = ConstantInt::Create(node->int_const);

        std::cout<<node->int_const<<std::endl;
        return constint;
    }
    return nullptr;
}

std::vector<Value*> irLvalUnitAST(LvalUnitAST* node, Module* module, BasicBlock* bb, symtab* symtable)
{
    std::cout<<"debug:irLvalUnitAST"<<std::endl;
    std::vector<Value*> res;
    auto exp = irExpAST(dc(ExpAST,node->exp),module,bb,symtable);
    if(node->unit)
    {
        auto tmp = irLvalUnitAST(dc(LvalUnitAST,node->unit),module,bb,symtable);
        res.insert(res.begin(),tmp.begin(),tmp.end());
    }
    res.push_back(exp);
    return res;
}



std::vector<std::optional<std::size_t>> get_bounds(std::vector<int> dims)
{
    std::vector<std::optional<std::size_t>> res;
    for(auto i:dims)
    {
        if(i == 0)
            res.push_back(std::nullopt);
        else
            res.push_back(i);
    }
    return res;
}