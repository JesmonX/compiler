#include "ast/ast.h"
#include "ir/ir.h"
#include <vector>

//symtab for values and dimensions
using allocmap = std::unordered_map<std::string,std::pair<Value*,std::vector<int>>>;
//stack of symtabs
using symtab_stack = std::vector<allocmap>;
//pair of value and vector of values(dims)
using lvalpair = std::pair<Value*,std::vector<Value*>>;


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
        auto newmap = allocmap();
        for (auto& [name, val] : this->cur()){
            newmap[name] = val;
        }
        ss.push_back(allocmap(newmap));
    }
    void exit(){
        ss.pop_back();
    }
    void insert_or_assign(std::string name, Value* val){
        this->cur()[name].first = val;
    }
    void set_dims(std::string name, std::vector<int> dims){
        this->cur()[name].second = dims;
    }
    void set_both(std::string name, Value* val, std::vector<int> dims){
        this->cur()[name] = std::make_pair(val,dims);
    }
    Value* find(std::string name){
        if(this->cur().find(name) == this->cur().end()){
            return nullptr;
        }
        return (this->cur())[name].first;
    }
    std::vector<int> find_dims(std::string name){
        return (this->cur())[name].second;
    }
    
};


class param_info {
public:
    std::string name;
    Type* type;
    std::vector<int> dim;
    param_info(std::string name, Type* type, std::vector<int> dim) : name(name), type(type), dim(dim) {}

};

class param_list {
public:
    std::vector<std::vector<int>> dims;
    std::vector<Type*> types;
    std::vector<std::string> names;
    param_list(std::vector<std::vector<int>> dims, std::vector<Type*> types, std::vector<std::string> names) : dims(dims), types(types), names(names) {}
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

class global_info {
public:
    std::vector<Value*> global_vars;
    std::vector<Value*> global_vals;
    std::vector<std::vector<int>> global_dims;
    bool is_set = 0;
    global_info(std::vector<Value*> global_vars, std::vector<Value*> global_vals,std::vector<std::vector<int>> global_dims) : global_vars(global_vars), global_vals(global_vals),global_dims(global_dims) {}
};
std::vector<std::optional<std::size_t>> get_bounds(std::vector<int> dims);

class MakeIR{
    public:
        bool short_circuit = 0;
        global_info global;
        //construct
        MakeIR(std::vector<Value*> global_vars, std::vector<Value*> global_vals,std::vector<std::vector<int>> global_dims) 
        : global(global_info(global_vars, global_vals,global_dims)) {}
    

        void irCompUnitAST(CompUnitAST* node, Module* module, symtab* symtable);
        void irCompUnitsAST(CompUnitsAST* node, Module* module, symtab* symtable);
        void irFuncDefAST(FuncDefAST* node, Module* module, symtab* symtable);
        void irVarDefAST(VarDefAST* node, Module* module, BasicBlock* bb, symtab* symtable);
        void irDeclAST(DeclAST* node, Module* module, symtab* symtable);
        void irMulVarDefAST(MulVarDefAST* node, Module* module, symtab* symtable, BasicBlock* bb = nullptr);
        std::vector<int> irConstUnitAST(ConstUnitAST* node, Module* module, symtab* symtable);
        param_list irFuncFParamsAST(FuncFParamsAST* node, Module* module, symtab* symtable);
        param_info* irFuncFParamAST(FuncFParamAST* node, Module* module, symtab* symtable);
        BasicBlock* irBlockAST(BlockAST* node, Module* module, BasicBlock* bb, symtab* symtable, ret_info* ret, while_info* whi = nullptr);
        BasicBlock* irBlockItemAST(BlockItemAST* node, Module* module, BasicBlock* bb, symtab* symtable, ret_info* ret, while_info* whi = nullptr);
        BasicBlock* irStmtAST(StmtAST* node, Module* module, BasicBlock* bb, symtab* symtable, ret_info* ret, while_info* whi = nullptr); 
        Value* irExpAST(ExpAST* node, Module* module, BasicBlock* bb, symtab* symtable,BasicBlock* truebb = nullptr, BasicBlock* falsebb = nullptr);
        lvalpair irLvalAST(LvalAST* node, Module* module, BasicBlock* bb, symtab* symtable);
        std::vector<Value*> irLvalUnitAST(LvalUnitAST* node, Module* module, BasicBlock* bb, symtab* symtable);
        Value* irPrimaryExpAST(PrimaryExpAST* node, Module* module, BasicBlock* bb, symtab* symtable);
        Value* irUnaryExpAST(UnaryExpAST* node, Module* module, BasicBlock* bb, symtab* symtable);
        std::vector<Value*> irFuncRParamsAST(FuncRParamsAST* node, Module* module, BasicBlock* bb, symtab* symtable);
        Value* irMulExpAST(MulExpAST* node, Module* module, BasicBlock* bb, symtab* symtable);
        Value* irAddExpAST(AddExpAST* node, Module* module, BasicBlock* bb, symtab* symtable);
        Value* irRelExpAST(RelExpAST* node, Module* module, BasicBlock* bb, symtab* symtable);
        Value* irEqExpAST(EqExpAST* node, Module* module, BasicBlock* bb, symtab* symtable);
        Value* irLAndExpAST(LAndExpAST* node, Module* module, BasicBlock* bb, symtab* symtable, BasicBlock* truebb = nullptr, BasicBlock* falsebb = nullptr);
        Value* irLOrExpAST(LOrExpAST* node, Module* module, BasicBlock* bb, symtab* symtable, BasicBlock* truebb = nullptr, BasicBlock* falsebb = nullptr);
};