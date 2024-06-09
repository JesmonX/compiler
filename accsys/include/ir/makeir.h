#include "ast/ast.h"
#include "ir/ir.h"
#include <vector>

using allocmap = std::unordered_map<std::string_view,Value*>;
using symtab_stack = std::vector<allocmap>;

class symtab{
    public:
    symtab_stack ss;
    symtab(){
        ss.push_back(allocmap());
    }
    allocmap& cur(){
        return ss.back();
    }
    void enter(){
        ss.push_back(allocmap(this->cur()));
    }
    void exit(){
        ss.pop_back();
    }
    void insert_or_assign(std::string_view name, Value* val){
        this->cur()[name] = val;
    }
    Value* find(std::string_view name){
        return (this->cur())[name];
    }
};


class ret_info {
public:
    Value* addr;//allca
    BasicBlock* bb;
    ret_info(Value* ret, BasicBlock* bb) : addr(ret), bb(bb) {}
};
class while_info {
public:
    BasicBlock* entry;
    BasicBlock* body;
    BasicBlock* end;
    while_info(BasicBlock* entry, BasicBlock* body, BasicBlock* end) : entry(entry), body(body), end(end) {}
};


void irCompUnitAST(CompUnitAST* node, Module* module, symtab* symtable);
void irCompUnitsAST(CompUnitsAST* node, Module* module, symtab* symtable);
void irFuncDefAST(FuncDefAST* node, Module* module, symtab* symtable);
void irVarDefAST(VarDefAST* node, Module* module, BasicBlock* bb, symtab* symtable);
void irDeclAST(DeclAST* node, Module* module, symtab* symtable);
void irMulVarDefAST(MulVarDefAST* node, Module* module, symtab* symtable, BasicBlock* bb = nullptr);
int irConstUnitAST(ConstUnitAST* node, Module* module, symtab* symtable);
std::vector<Type*> irFuncFParamsAST(FuncFParamsAST* node, Module* module, symtab* symtable);
Type* irFuncFParamAST(FuncFParamAST* node, Module* module, symtab* symtable);
BasicBlock* irBlockAST(BlockAST* node, Module* module, BasicBlock* bb, symtab* symtable, ret_info* ret, while_info* whi = nullptr);
BasicBlock* irBlockItemAST(BlockItemAST* node, Module* module, BasicBlock* bb, symtab* symtable, ret_info* ret, while_info* whi = nullptr);
BasicBlock* irStmtAST(StmtAST* node, Module* module, BasicBlock* bb, symtab* symtable, ret_info* ret, while_info* whi = nullptr); 
Value* irExpAST(ExpAST* node, Module* module, BasicBlock* bb, symtab* symtable);
Value* irLvalAST(LvalAST* node, Module* module, BasicBlock* bb, symtab* symtable);
Value* irLvalUnitAST(LvalUnitAST* node, Module* module, BasicBlock* bb, symtab* symtable);
Value* irPrimaryExpAST(PrimaryExpAST* node, Module* module, BasicBlock* bb, symtab* symtable);
Value* irUnaryExpAST(UnaryExpAST* node, Module* module, BasicBlock* bb, symtab* symtable);
std::vector<Value*> irFuncRParamsAST(FuncRParamsAST* node, Module* module, BasicBlock* bb, symtab* symtable);
Value* irMulExpAST(MulExpAST* node, Module* module, BasicBlock* bb, symtab* symtable);
Value* irAddExpAST(AddExpAST* node, Module* module, BasicBlock* bb, symtab* symtable);
Value* irRelExpAST(RelExpAST* node, Module* module, BasicBlock* bb, symtab* symtable);
Value* irEqExpAST(EqExpAST* node, Module* module, BasicBlock* bb, symtab* symtable);
Value* irLAndExpAST(LAndExpAST* node, Module* module, BasicBlock* bb, symtab* symtable);
Value* irLOrExpAST(LOrExpAST* node, Module* module, BasicBlock* bb, symtab* symtable);