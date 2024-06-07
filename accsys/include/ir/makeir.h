#include "ast/ast.h"
#include "ir/ir.h"


void irCompUnitAST(CompUnitAST* node, Module* module);
void irCompUnitsAST(CompUnitsAST* node, Module* module);
void irFuncDefAST(FuncDefAST* node, Module* module);
void irVarDefAST(VarDefAST* node, Module* module, BasicBlock* bb);
void irDeclAST(DeclAST* node, Module* module);
void irMulVarDefAST(MulVarDefAST* node, Module* module, BasicBlock* bb = nullptr);
int irConstUnitAST(ConstUnitAST* node, Module* module);
std::vector<Type*> irFuncFParamsAST(FuncFParamsAST* node, Module* module);
Type* irFuncFParamAST(FuncFParamAST* node, Module* module);
void irBlockAST(BlockAST* node, Module* module, BasicBlock* bb);
void irBlockItemAST(BlockItemAST* node, Module* module, BasicBlock* bb);
void irStmtAST(StmtAST* node, Module* module, BasicBlock* bb);
Value* irExpAST(ExpAST* node, Module* module, BasicBlock* bb);
Value* irLvalAST(LvalAST* node, Module* module, BasicBlock* bb);
Value* irLvalUnitAST(LvalUnitAST* node, Module* module, BasicBlock* bb);
Value* irPrimaryExpAST(PrimaryExpAST* node, Module* module, BasicBlock* bb);
Value* irUnaryExpAST(UnaryExpAST* node, Module* module, BasicBlock* bb);
Value* irFuncRParamsAST(FuncRParamsAST* node, Module* module, BasicBlock* bb);
Value* irMulExpAST(MulExpAST* node, Module* module, BasicBlock* bb);
Value* irAddExpAST(AddExpAST* node, Module* module, BasicBlock* bb);
Value* irRelExpAST(RelExpAST* node, Module* module, BasicBlock* bb);
Value* irEqExpAST(EqExpAST* node, Module* module, BasicBlock* bb);
Value* irLAndExpAST(LAndExpAST* node, Module* module, BasicBlock* bb);
Value* irLOrExpAST(LOrExpAST* node, Module* module, BasicBlock* bb);