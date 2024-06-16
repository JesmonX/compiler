#include "ast/ast.h"
#include "ir/ir.h"
#include "ir/makeir.h"
#include "semantic/enum.h"
#include <iostream>
#include <optional>
#define dc(x, n) dynamic_cast<x *>(n.get())

void MakeIR::irCompUnitAST(CompUnitAST *node, Module *module, symtab *symtable)
{
    std::cout << "debug:irCompUnitAST" << std::endl;
    irCompUnitsAST(dc(CompUnitsAST, node->comp_units), module, symtable);
}

void MakeIR::irCompUnitsAST(CompUnitsAST *node, Module *module, symtab *symtable)
{
    std::cout << "debug:irCompUnitsAST" << std::endl;

    if (node->comp_unit)
        irCompUnitsAST(dc(CompUnitsAST, node->comp_unit), module, symtable);
    if (node->def)
        irDeclAST(dc(DeclAST, node->def), module, symtable);
}

void MakeIR::irDeclAST(DeclAST *node, Module *module, symtab *symtable)
{
    std::cout << "debug:irDeclAST" << std::endl;

    if (node->func_def)
    {
        symtable->enter();
        irFuncDefAST(dc(FuncDefAST, node->func_def), module, symtable);
        symtable->exit();
    }
    if (node->var_def)
        irMulVarDefAST(dc(MulVarDefAST, node->var_def), module, symtable, nullptr);
}

void MakeIR::irFuncDefAST(FuncDefAST *node, Module *module, symtab *symtable)
{
    std::cout << "debug:irFuncDefAST" << std::endl;
    auto ty = typehandler(node->func_type);
    int num = (ty == Type::getIntegerTy()) ? 1 : 0;
    param_list paramty = param_list(std::vector<std::vector<std::optional<std::size_t>>>(), std::vector<Type *>(), std::vector<std::string>());

    if (node->func_fparams)
        paramty = irFuncFParamsAST(dc(FuncFParamsAST, node->func_fparams), module, symtable);

    auto func = Function::Create(FunctionType::get(ty, paramty.types), false, node->ident, module);
    auto bb = BasicBlock::Create(func);
    // basic block
    bb->setName("entry");
    if (this->global.is_set == 0)
    {
        for (int i = 0; i < this->global.global_vars.size(); i++)
        {
            if (this->global.global_vals[i] != nullptr)
                auto store = StoreInst::Create(this->global.global_vals[i], this->global.global_vars[i], bb);
        }
        this->global.is_set = 1;
    }

    auto ret_al = AllocaInst::Create(ty, num, bb);
    auto retbb = BasicBlock::Create(func);
    auto retinfo = ret_info(ret_al, retbb);
    if (func->getReturnType()->isUnitTy())
    {
        auto voidval = ConstantUnit::Create();
        auto retinst = RetInst::Create(voidval, retinfo.bb);
    }
    else
    {
        auto load = LoadInst::Create(retinfo.addr, retinfo.bb);
        auto retinst = RetInst::Create(load, retinfo.bb);
    }

    if (node->func_fparams != nullptr)
    {
        for (int i = 0; i < paramty.types.size(); i++)
        {
            auto addrname = paramty.names[i] + ".addr";

            // alloca and store param
            int size = 1;
            for (auto j : paramty.dims[i])
                size *= j.value(); // size of this param


            if (paramty.types[i]->isIntegerTy())
            {
                func->getArg(i)->setName(paramty.names[i]);
                auto inst = AllocaInst::Create(paramty.types[i], size, bb);

                inst->setName(addrname);
                auto store = StoreInst::Create(func->getArg(i), inst, bb);
                // update symbol table
                symtable->insert_or_assign(addrname, inst);
                symtable->set_dims(addrname, paramty.dims[i]);
            }
            else
            {
                func->getArg(i)->setName(addrname);
                if (symtable->find(addrname) == nullptr)
                {
                    auto temp = paramty.dims[i];
                    temp[0] = std::nullopt; ///
                    symtable->set_both(addrname, func->getArg(i), temp);
                }
            }
        }
    }
    irBlockAST(dc(BlockAST, node->block), module, bb, symtable, &retinfo);
}

param_list MakeIR::irFuncFParamsAST(FuncFParamsAST *node, Module *module, symtab *symtable)
{
    std::cout << "debug:irFuncFParamsAST" << std::endl;
    std::vector<std::vector<std::optional<std::size_t>>> dims;
    std::vector<Type *> types;
    std::vector<std::string> names;

    auto param = irFuncFParamAST(dc(FuncFParamAST, node->param), module, symtable);

    if (node->params)
    {
        auto tmp = irFuncFParamsAST(dc(FuncFParamsAST, node->params), module, symtable);
        dims.insert(dims.begin(), tmp.dims.begin(), tmp.dims.end());
        types.insert(types.begin(), tmp.types.begin(), tmp.types.end());
        names.insert(names.begin(), tmp.names.begin(), tmp.names.end());
    }
    dims.push_back(param->dim);
    types.push_back(param->type);
    names.push_back(param->name);
    return param_list(dims, types, names);
}

param_info *MakeIR::irFuncFParamAST(FuncFParamAST *node, Module *module, symtab *symtable)
{
    std::cout << "debug:irFuncFParamAST" << std::endl;
    std::vector<std::optional<std::size_t>> dim;
    if (node->unit)
        dim = irConstUnitAST(dc(ConstUnitAST, node->unit), module, symtable);
    if (!node->unit && node->type != 0)
        dim.push_back(-1);
    Type *type = Type::getIntegerTy();
    if (node->type != 0)
        type = PointerType::get(Type::getIntegerTy());
    auto name = node->ident;
    auto res = new param_info(name, type, dim);
    return res;
}

void MakeIR::irMulVarDefAST(MulVarDefAST *node, Module *module, symtab *symtable, BasicBlock *bb)
{
    std::cout << "debug:irMulVarDefAST" << std::endl;
    if (node->var_def)
        irVarDefAST(dc(VarDefAST, node->var_def), module, bb, symtable);
    if (node->mul_var_def)
        irMulVarDefAST(dc(MulVarDefAST, node->mul_var_def), module, symtable, bb);
}

void MakeIR::irVarDefAST(VarDefAST *node, Module *module, BasicBlock *bb, symtab *symtable)
{
    std::cout << "debug:irVarDefAST" << std::endl;
    auto ty = typehandler("int");
    int nums = 1;
    std::vector<std::optional<std::size_t>> units;
    if (node->unit)
    {
        units = irConstUnitAST(dc(ConstUnitAST, node->unit), module, symtable);
        for (auto i : units)
            nums *= i.value(); // 计算数组大小
    }
    if (node->isgloble)
    {
        auto var = GlobalVariable::Create(ty, nums, false, node->ident, module);
        var->setName(node->ident + ".addr");
        symtable->set_both(node->ident + ".addr", var, units);

        if (node->exp)
        {
            auto exp = irExpAST(dc(ExpAST, node->exp), module, bb, symtable);
            this->global.global_vals.push_back(exp);
        }
        else
        {
            auto exp = nullptr;
            this->global.global_vals.push_back(exp);
        }
        this->global.global_vars.push_back(var);
        this->global.global_dims.push_back(units);
    }
    else
    {
        auto var = AllocaInst::Create(ty, nums, bb);
        var->setName(node->ident + ".addr");
        symtable->set_both(std::string(var->getName()), var, units);
        if (node->exp)
        {
            auto exp = irExpAST(dc(ExpAST, node->exp), module, bb, symtable);
            auto storevar = StoreInst::Create(exp, var, bb);
        }
    }
}

std::vector<std::optional<std::size_t>> MakeIR::irConstUnitAST(ConstUnitAST *node, Module *module, symtab *symtable)
{
    std::cout << "debug:irConstUnitAST" << std::endl;
    std::vector<std::optional<std::size_t>>res;
    res.push_back(node->int_const);
    if (node->unit)
    {
        auto tmp = irConstUnitAST(dc(ConstUnitAST, node->unit), module, symtable);
        res.insert(res.end(), tmp.begin(), tmp.end());
    }
    return res;
}

BasicBlock *MakeIR::irBlockAST(BlockAST *node, Module *module, BasicBlock *bb, symtab *symtable, ret_info *ret, while_info *whi)
{
    std::cout << "debug:irBlockAST" << std::endl;

    if (node->item)
    {
        auto res = irBlockItemAST(dc(BlockItemAST, node->item), module, bb, symtable, ret);
        return res;
    }
    return bb;
}

BasicBlock *MakeIR::irBlockItemAST(BlockItemAST *node, Module *module, BasicBlock *bb, symtab *symtable, ret_info *ret, while_info *whi)
{
    std::cout << "debug:irBlockItemAST" << std::endl;
    BasicBlock *res = bb;
    if (node->block_item)
        res = irBlockItemAST(dc(BlockItemAST, node->block_item), module, bb, symtable, ret);
    // if cur bb changed in the block item , update the bb as res
    if (node->stmt)
        res = irStmtAST(dc(StmtAST, node->stmt), module, res, symtable, ret);
    if (node->mul_var_def)
        irMulVarDefAST(dc(MulVarDefAST, node->mul_var_def), module, symtable, res);
    return res;
}

BasicBlock *MakeIR::irStmtAST(StmtAST *node, Module *module, BasicBlock *bb, symtab *symtable, ret_info *ret, while_info *whi)
{
    std::cout << "debug:irStmtAST" << std::endl;
    switch (node->type)
    {
    case 1: // assign
    {
        auto lval = irLvalAST(dc(LvalAST, node->lval), module, bb, symtable); // addr
        auto exp = irExpAST(dc(ExpAST, node->exp), module, bb, symtable);

        if (lval.second.size() != 0)
        {
            auto addrname = std::string(lval.first->getName());
            auto vec = symtable->find_dims(addrname);
            auto bound = get_bounds(vec);
            auto offset = OffsetInst::Create(Type::getIntegerTy(), lval.first, lval.second, bound, bb);
            auto store = StoreInst::Create(exp, offset, bb);
        }
        else
        {
            auto store = StoreInst::Create(exp, lval.first, bb);
        }

        return bb;
    }
    case 2: // exp
    {
        irExpAST(dc(ExpAST, node->exp), module, bb, symtable);
        return bb;
    }
    case 3: // block
    {
        symtable->enter();
        auto res = irBlockAST(dc(BlockAST, node->block), module, bb, symtable, ret);
        symtable->exit();
        return res;
    }
    case 4: // if
    {

        auto func = bb->getParent();
        auto truebb = BasicBlock::Create(func, ret->bb);
        // auto falsebb = BasicBlock::Create(func);
        auto endbb = BasicBlock::Create(func, ret->bb);
        auto cond = irExpAST(dc(ExpAST, node->exp), module, bb, symtable, truebb, endbb);
        if (!short_circuit)
        {
            auto br = BranchInst::Create(truebb, endbb, cond, bb);
        }
        else
            this->short_circuit = 0;
        auto tempbb = irStmtAST(dc(StmtAST, node->stmt), module, truebb, symtable, ret);
        if (tempbb != nullptr)
            auto jp = JumpInst::Create(endbb, tempbb);
        // irStmtAST(dc(StmtAST,node->stmt2),module,falsebb,symtable);
        return endbb;
    }
    case 5: // if else
    {

        auto func = bb->getParent();
        auto truebb = BasicBlock::Create(func, ret->bb);
        auto falsebb = BasicBlock::Create(func, ret->bb);
        auto endbb = BasicBlock::Create(func, ret->bb);
        auto cond = irExpAST(dc(ExpAST, node->exp), module, bb, symtable, truebb, falsebb);
        if (!short_circuit)
        {
            auto br = BranchInst::Create(truebb, falsebb, cond, bb);
        }
        else
            this->short_circuit = 0;

        auto tempbb1 = irStmtAST(dc(StmtAST, node->stmt), module, truebb, symtable, ret);
        if (tempbb1 != nullptr)
            auto jp1 = JumpInst::Create(endbb, tempbb1);

        auto tempbb2 = irStmtAST(dc(StmtAST, node->stmt2), module, falsebb, symtable, ret);
        if (tempbb2 != nullptr)
            auto jp2 = JumpInst::Create(endbb, tempbb2);
        return endbb;
    }
    case 6: // while
    {
        auto func = bb->getParent();
        auto entrybb = BasicBlock::Create(func, ret->bb);
        auto truebb = BasicBlock::Create(func, ret->bb);
        auto endbb = BasicBlock::Create(func, ret->bb);
        auto whileinfo = while_info(entrybb, truebb, endbb);
        auto jp = JumpInst::Create(entrybb, bb);
        auto cond = irExpAST(dc(ExpAST, node->exp), module, entrybb, symtable, truebb, endbb);
        if (!short_circuit)
        {
            auto br = BranchInst::Create(truebb, endbb, cond, entrybb);
        }
        else
            this->short_circuit = 0;

        auto tempbb = irStmtAST(dc(StmtAST, node->stmt), module, truebb, symtable, ret, &whileinfo);
        if (tempbb != nullptr)
            auto jp2 = JumpInst::Create(entrybb, tempbb);

        return endbb;
    }
    case 7: // break uncertain
    {
        auto jp = JumpInst::Create(whi->end, bb);
        return bb;
    }
    case 8: // continue uncertain
    {
        auto jp = JumpInst::Create(whi->entry, bb);
        return bb;
    }
    case 9: // return
    {
        auto jp = JumpInst::Create(ret->bb, bb);

        return nullptr;
    }
    case 10: // return exp
    {
        auto retexp = irExpAST(dc(ExpAST, node->exp), module, bb, symtable);
        auto store = StoreInst::Create(retexp, ret->addr, bb);
        auto jp = JumpInst::Create(ret->bb, bb);
        return nullptr;
    }
    }
}

lvalpair MakeIR::irLvalAST(LvalAST *node, Module *module, BasicBlock *bb, symtab *symtable)
{
    std::cout << "debug:irLvalAST" << std::endl;
    auto res = symtable->find(node->ident + ".addr");
    std::vector<Value *> units;
    if (node->unit)
        units = irLvalUnitAST(dc(LvalUnitAST, node->unit), module, bb, symtable);
    else
        units.resize(0);
    return lvalpair(res, units);
}

Value *MakeIR::irExpAST(ExpAST *node, Module *module, BasicBlock *bb, symtab *symtable, BasicBlock *truebb, BasicBlock *falsebb)
{
    std::cout << "debug:irExpAST" << std::endl;

    return irLOrExpAST(dc(LOrExpAST, node->lorexp), module, bb, symtable, truebb, falsebb);
}

Value *MakeIR::irLOrExpAST(LOrExpAST *node, Module *module, BasicBlock *bb, symtab *symtable, BasicBlock *truebb, BasicBlock *falsebb)
{
    std::cout << "debug:irLOrExpAST" << std::endl;

    if (node->lor_exp)
    {
        auto ty = typehandler("int");
        this->short_circuit = 1;
        auto fbb = BasicBlock::Create(bb->getParent(), truebb);

        auto l = irLOrExpAST(dc(LOrExpAST, node->lor_exp), module, bb, symtable, truebb, fbb);
        if (!bb->back().isTerminator())
        {
            BranchInst::Create(truebb, fbb, l, bb);
        }

        auto r = irLAndExpAST(dc(LAndExpAST, node->land_exp), module, fbb, symtable, truebb, falsebb);
        if (!fbb->back().isTerminator())
        {
            BranchInst::Create(truebb, falsebb, r, fbb);
        }

        return l; // 由于短路，这里的返回值没有意义
    }
    else
        return irLAndExpAST(dc(LAndExpAST, node->land_exp), module, bb, symtable, truebb, falsebb);
}

Value *MakeIR::irLAndExpAST(LAndExpAST *node, Module *module, BasicBlock *bb, symtab *symtable, BasicBlock *truebb, BasicBlock *falsebb)
{
    std::cout << "debug:irLAndExpAST" << std::endl;
    if (node->land_exp)
    {
        this->short_circuit = 1;
        auto ty = typehandler("int");
        auto tbb = BasicBlock::Create(bb->getParent(), falsebb);
        auto l = irLAndExpAST(dc(LAndExpAST, node->land_exp), module, bb, symtable, tbb, falsebb);
        if (!bb->back().isTerminator())
        {
            BranchInst::Create(tbb, falsebb, l, bb);
        }
        auto r = irEqExpAST(dc(EqExpAST, node->eq_exp), module, tbb, symtable);
        auto res = BinaryInst::Create(BinaryInst::And, l, r, ty, tbb);
        auto br2 = BranchInst::Create(truebb, falsebb, res, tbb);
        return res;
    }
    else
        return irEqExpAST(dc(EqExpAST, node->eq_exp), module, bb, symtable);
}

Value *MakeIR::irEqExpAST(EqExpAST *node, Module *module, BasicBlock *bb, symtab *symtable)
{
    std::cout << "debug:irEqExpAST" << std::endl;

    if (node->eq_exp)
    {
        auto ty = typehandler("int");
        auto l = irEqExpAST(dc(EqExpAST, node->eq_exp), module, bb, symtable);
        auto r = irRelExpAST(dc(RelExpAST, node->rel_exp), module, bb, symtable);
        if (node->op_type == 1)
            return BinaryInst::Create(BinaryInst::Eq, l, r, ty, bb);
        else
            return BinaryInst::Create(BinaryInst::Ne, l, r, ty, bb);
    }
    else
        return irRelExpAST(dc(RelExpAST, node->rel_exp), module, bb, symtable);
}

Value *MakeIR::irRelExpAST(RelExpAST *node, Module *module, BasicBlock *bb, symtab *symtable)
{
    std::cout << "debug:irRelExpAST" << std::endl;
    if (node->rel_exp)
    {
        auto l = irRelExpAST(dc(RelExpAST, node->rel_exp), module, bb, symtable);
        auto r = irAddExpAST(dc(AddExpAST, node->add_exp), module, bb, symtable);
        auto ty = typehandler("int");
        if (node->op_type == 1)
            return BinaryInst::Create(BinaryInst::Lt, l, r, ty, bb);
        else if (node->op_type == 2)
            return BinaryInst::Create(BinaryInst::Gt, l, r, ty, bb);
        else if (node->op_type == 3)
            return BinaryInst::Create(BinaryInst::Le, l, r, ty, bb);
        else
            return BinaryInst::Create(BinaryInst::Ge, l, r, ty, bb);
    }
    else
        return irAddExpAST(dc(AddExpAST, node->add_exp), module, bb, symtable);
}

Value *MakeIR::irAddExpAST(AddExpAST *node, Module *module, BasicBlock *bb, symtab *symtable)
{
    std::cout << "debug:irAddExpAST" << std::endl;
    if (node->add_exp)
    {

        auto l = irAddExpAST(dc(AddExpAST, node->add_exp), module, bb, symtable);
        auto r = irMulExpAST(dc(MulExpAST, node->mul_exp), module, bb, symtable);
        auto ty = typehandler("int");

        if (node->op_type == 1)
            return BinaryInst::Create(BinaryInst::Add, l, r, ty, bb);
        else
            return BinaryInst::Create(BinaryInst::Sub, l, r, ty, bb);
    }
    else
        return irMulExpAST(dc(MulExpAST, node->mul_exp), module, bb, symtable);
}

Value *MakeIR::irMulExpAST(MulExpAST *node, Module *module, BasicBlock *bb, symtab *symtable)
{
    std::cout << "debug:irMulExpAST" << std::endl;
    if (node->mul_exp)
    {
        auto l = irMulExpAST(dc(MulExpAST, node->mul_exp), module, bb, symtable);
        auto r = irUnaryExpAST(dc(UnaryExpAST, node->unary_exp), module, bb, symtable);
        auto ty = typehandler("int");
        if (node->op_type == 1)
            return BinaryInst::Create(BinaryInst::Mul, l, r, ty, bb);
        else if (node->op_type == 2)
            return BinaryInst::Create(BinaryInst::Div, l, r, ty, bb);
        else
            return BinaryInst::Create(BinaryInst::Mod, l, r, ty, bb);
    }
    else
        return irUnaryExpAST(dc(UnaryExpAST, node->unary_exp), module, bb, symtable);
}

Value *MakeIR::irUnaryExpAST(UnaryExpAST *node, Module *module, BasicBlock *bb, symtab *symtable)
{
    std::cout << "debug:irUnaryExpAST" << std::endl;
    switch (node->type)
    {
    case 1:
    {
        return irPrimaryExpAST(dc(PrimaryExpAST, node->primary_exp), module, bb, symtable);
    }
    case 2: // call
    {
        std::vector<Value *> args; // none args
        args.resize(0);
        auto func = module->getFunction(std::string_view(node->ident));
        auto call = CallInst::Create(func, args, bb);
        return call;
    }
    case 3: // call and args
    {
        auto args = irFuncRParamsAST(dc(FuncRParamsAST, node->params), module, bb, symtable);

        auto func = module->getFunction(node->ident);
        auto call = CallInst::Create(func, args, bb);
        return call;
    }
    case 4: // unary op
    {
        auto exp = irUnaryExpAST(dc(UnaryExpAST, node->unary_exp), module, bb, symtable);
        auto ty = typehandler("int");
        auto zero = ConstantInt::Create(0);

        if (node->op_type == 1)
            return BinaryInst::Create(BinaryInst::Add, zero, exp, ty, bb);
        else if (node->op_type == 2)
            return BinaryInst::Create(BinaryInst::Sub, zero, exp, ty, bb);
        else if (node->op_type == 3)
            return BinaryInst::Create(BinaryInst::Eq, zero, exp, ty, bb);
    }
    }
}

std::vector<Value *> MakeIR::irFuncRParamsAST(FuncRParamsAST *node, Module *module, BasicBlock *bb, symtab *symtable)
{
    std::cout << "debug:irFuncRParamsAST" << std::endl;
    std::vector<Value *> res;
    if (node->params)
    {
        auto tmp = irFuncRParamsAST(dc(FuncRParamsAST, node->params), module, bb, symtable);
        res.insert(res.begin(), tmp.begin(), tmp.end());
    }
    auto exp = irExpAST(dc(ExpAST, node->exp), module, bb, symtable);
    res.push_back(exp);
    return res;
}

Value *MakeIR::irPrimaryExpAST(PrimaryExpAST *node, Module *module, BasicBlock *bb, symtab *symtable)
{
    std::cout << "debug:irPrimaryExpAST" << std::endl;

    if (node->exp)
        return irExpAST(dc(ExpAST, node->exp), module, bb, symtable);
    else if (node->lval)
    {
        auto lval = irLvalAST(dc(LvalAST, node->lval), module, bb, symtable);
        if (lval.second.size() == get_bounds(symtable->find_dims(std::string(lval.first->getName()))).size() && lval.second.size() != 0)
        {
            auto bound = get_bounds(symtable->find_dims(std::string(lval.first->getName())));

            auto offset = OffsetInst::Create(Type::getIntegerTy(), lval.first, lval.second, bound, bb);
            auto load = LoadInst::Create(offset, bb);
            return load;
        }

        else if (lval.second.size() < get_bounds(symtable->find_dims(std::string(lval.first->getName()))).size())
        {
            if (lval.second.size() == 0)
            {
                return lval.first;
            }

            auto bound = get_bounds(symtable->find_dims(std::string(lval.first->getName())));
            if (bound.size() > lval.second.size())
            {
                for (int i = 0; i < bound.size() - lval.second.size(); i++)
                    lval.second.insert(lval.second.end(), ConstantInt::Create(0));
            }

            auto offset = OffsetInst::Create(Type::getIntegerTy(), lval.first, lval.second, bound, bb);
            return offset;
        }
        else
        {
            auto load = LoadInst::Create(lval.first, bb);
            return load;
        }
    }
    else
    {
        auto constint = ConstantInt::Create((uint32_t)node->int_const);
        return constint;
    }
    return nullptr;
}

std::vector<Value *> MakeIR::irLvalUnitAST(LvalUnitAST *node, Module *module, BasicBlock *bb, symtab *symtable)
{
    std::cout << "debug:irLvalUnitAST" << std::endl;
    std::vector<Value *> res;
    auto exp = irExpAST(dc(ExpAST, node->exp), module, bb, symtable);
    if (node->unit)
    {
        auto tmp = irLvalUnitAST(dc(LvalUnitAST, node->unit), module, bb, symtable);
        res.insert(res.begin(), tmp.begin(), tmp.end());
    }
    res.push_back(exp);
    return res;
}

std::vector<std::optional<std::size_t>> get_bounds(std::vector<std::optional<std::size_t>> dims)
{
    std::vector<std::optional<std::size_t>> res;
    for (auto i : dims)
    {
        if (i == -1)
            res.push_back(std::nullopt);
        else
            res.push_back(i);
    }
    return res;
}