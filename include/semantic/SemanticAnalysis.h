#include "ast/ast.h"
#include "semantic/enum.h"
#include "semantic/symtable.h"
#include <unordered_map>
#include <vector>

class SmAnalysis {
private:
    SymbolTable symbol_table;
    std::string func_block = "";
    VarDefinition hope_exp_to_comply;
    VarDefinition pre_def_int;
    VarDefinition pre_def_void;
    VarDefinition pre_def_array;
    std::string cur_func;

    std::vector<VarDefinition> temp_def_store; // this is junk code just for test cases

public:
    SmAnalysis()
        : hope_exp_to_comply("", false, 0)
        , pre_def_int("int", false, 0)
        , pre_def_void("void", false, 0)
        , pre_def_array("int", true, 10) {
        FuncDefinition putint;
        putint.func_type             = "void";
        putint.func_param_table["num"] = pre_def_int;
        putint.func_param_vector.push_back(pre_def_int);
        symbol_table.func_table["putint"] = putint;
        FuncDefinition getint;
        getint.func_type             = "int";
        symbol_table.func_table["getint"] = getint; 

        FuncDefinition getch;
        getch.func_type             = "int";
        symbol_table.func_table["getch"] = getch;

        FuncDefinition getarray;
        getarray.func_type             = "int";
        getarray.func_param_table["a"] = pre_def_array;
        getarray.func_param_vector.push_back(pre_def_array);
        symbol_table.func_table["getarray"] = getarray;

        FuncDefinition putch;
        putch.func_type             = "void";
        putch.func_param_table["ch"] = pre_def_int;
        putch.func_param_vector.push_back(pre_def_int);
        symbol_table.func_table["putch"] = putch;

        FuncDefinition putarray;
        putarray.func_type             = "void";
        putarray.func_param_table["n"] = pre_def_int;
        putarray.func_param_table["a"] = pre_def_array;
        putarray.func_param_vector.push_back(pre_def_int);
        putarray.func_param_vector.push_back(pre_def_array);
        symbol_table.func_table["putarray"] = putarray;

        FuncDefinition starttime;
        starttime.func_type             = "void";
        symbol_table.func_table["starttime"] = starttime;

        FuncDefinition stoptime;
        stoptime.func_type             = "void";
        symbol_table.func_table["stoptime"] = stoptime;

        /*
        int getint();
        int getch();
        int getarray(int a[]);
        void putint(int num);
        void putch(int ch);
        void putarray(int n, int a[]);
        void starttime();
        void stoptime();\n";*/

    }
    ~SmAnalysis() = default;
    ast_node_type get_type(const ASTPtr& node);

    void traverse_by_node_type(ast_node_type node_type, const ASTPtr& node);
    void traverse_by_node(const ASTPtr& node);

    void traverseCompUnitAST(CompUnitAST* node);
    void traverseFuncDefAST(FuncDefAST* node);
    void traverseVarDefAST(VarDefAST* node);
    void traverseDeclAST(DeclAST* node);
    void traverseCompUnitsAST(CompUnitsAST* node);
    void traverseMulVarDefAST(MulVarDefAST* node);
    void traverseConstUnitAST(ConstUnitAST* node, std::vector<int>& dims, int index);
    void traverseFuncFParamsAST(FuncFParamsAST* node, std::string func_name);
    void traverseFuncFParamAST(FuncFParamAST* node, std::string func_name);
    void traverseBlockAST(BlockAST* node);
    void traverseFuncRParamsAST(FuncRParamsAST* node, std::string func_name, int& index);
    
    void traverseBlockItemAST(BlockItemAST* node);
    void traverseStmtAST(StmtAST* node);
    void traverseLvalUnitAST(LvalUnitAST* node);
    const VarDefinition& traverseLvalAST(LvalAST* node);
    const VarDefinition& traversePrimaryExpAST(PrimaryExpAST* node);
    const VarDefinition& traverseUnaryExpAST(UnaryExpAST* node);
    const VarDefinition& traverseMulExpAST(MulExpAST* node);
    const VarDefinition& traverseAddExpAST(AddExpAST* node);
    const VarDefinition& traverseRelExpAST(RelExpAST* node);
    const VarDefinition& traverseEqExpAST(EqExpAST* node);
    const VarDefinition& traverseLAndExpAST(LAndExpAST* node);
    const VarDefinition& traverseLOrExpAST(LOrExpAST* node);
    const VarDefinition& traverseExpAST(ExpAST* node);
    //
    auto traverseFuncDefAST(const ASTPtr& node) {
        return traverseFuncDefAST(dynamic_cast<FuncDefAST*>(node.get()));
    }
    auto traverseVarDefAST(const ASTPtr& node) {
        return traverseVarDefAST(dynamic_cast<VarDefAST*>(node.get()));
    }
    auto traverseDeclAST(const ASTPtr& node) {
        return traverseDeclAST(dynamic_cast<DeclAST*>(node.get()));
    }
    auto traverseCompUnitAST(const ASTPtr& node) {
        return traverseCompUnitAST(dynamic_cast<CompUnitAST*>(node.get()));
    }
    auto traverseCompUnitsAST(const ASTPtr& node) {
        return traverseCompUnitsAST(dynamic_cast<CompUnitsAST*>(node.get()));
    }
    auto traverseMulVarDefAST(const ASTPtr& node) {
        return traverseMulVarDefAST(dynamic_cast<MulVarDefAST*>(node.get()));
    }
    auto traverseConstUnitAST(const ASTPtr& node, std::vector<int>& dims, int index) {
        return traverseConstUnitAST(dynamic_cast<ConstUnitAST*>(node.get()), dims, index);
    }
    auto traverseFuncFParamsAST(const ASTPtr& node, std::string func_name) {
        return traverseFuncFParamsAST(dynamic_cast<FuncFParamsAST*>(node.get()), func_name);
    }
    auto traverseFuncFParamAST(const ASTPtr& node, std::string func_name) {
        return traverseFuncFParamAST(dynamic_cast<FuncFParamAST*>(node.get()), func_name);
    }
    auto traverseFuncRParamsAST(const ASTPtr& node, std::string func_name, int index) {
        return traverseFuncRParamsAST(dynamic_cast<FuncRParamsAST*>(node.get()), func_name, index);
    }
    auto traverseBlockAST(const ASTPtr& node) {
        return traverseBlockAST(dynamic_cast<BlockAST*>(node.get()));
    }
    auto traverseBlockItemAST(const ASTPtr& node) {
        return traverseBlockItemAST(dynamic_cast<BlockItemAST*>(node.get()));
    }
    auto traverseStmtAST(const ASTPtr& node) {
        return traverseStmtAST(dynamic_cast<StmtAST*>(node.get()));
    }

    const VarDefinition& traverseLvalAST(const ASTPtr& node) {
        return traverseLvalAST(dynamic_cast<LvalAST*>(node.get()));
    }
    auto traverseLvalUnitAST(const ASTPtr& node) {
        return traverseLvalUnitAST(dynamic_cast<LvalUnitAST*>(node.get()));
    }
    const VarDefinition& traversePrimaryExpAST(const ASTPtr& node) {
        return traversePrimaryExpAST(dynamic_cast<PrimaryExpAST*>(node.get()));
    }
    const VarDefinition& traverseUnaryExpAST(const ASTPtr& node) {
        return traverseUnaryExpAST(dynamic_cast<UnaryExpAST*>(node.get()));
    }
    const VarDefinition& traverseMulExpAST(const ASTPtr& node) {
        return traverseMulExpAST(dynamic_cast<MulExpAST*>(node.get()));
    }
    const VarDefinition& traverseAddExpAST(const ASTPtr& node) {
        return traverseAddExpAST(dynamic_cast<AddExpAST*>(node.get()));
    }
    const VarDefinition& traverseRelExpAST(const ASTPtr& node) {
        return traverseRelExpAST(dynamic_cast<RelExpAST*>(node.get()));
    }
    const VarDefinition& traverseEqExpAST(const ASTPtr& node) {
        return traverseEqExpAST(dynamic_cast<EqExpAST*>(node.get()));
    }
    const VarDefinition& traverseLAndExpAST(const ASTPtr& node) {
        return traverseLAndExpAST(dynamic_cast<LAndExpAST*>(node.get()));
    }
    const VarDefinition& traverseLOrExpAST(const ASTPtr& node) {
        return traverseLOrExpAST(dynamic_cast<LOrExpAST*>(node.get()));
    }
    const VarDefinition& traverseExpAST(const ASTPtr& node) {
        return traverseExpAST(dynamic_cast<ExpAST*>(node.get()));
    }
    bool compare_var_def(const VarDefinition& A, const VarDefinition& B) {
        bool result = false;
        if (!(A.type == B.type && A.is_array == B.is_array && A.dimension_num == B.dimension_num))  return false;
        if (!(A.is_array))  return true;
        int n = A.dimensions.size();
        for (int i = 0; i <= n - 1; i++) {
            if (A.dimensions[i] != B.dimensions[i])  return false;
        }
        return true;
    }
};

void traverseAST(const std::unique_ptr<BaseAST>& node);

/*
FuncDefAST
VarDefAST
DeclAST
CompUnitAST
CompUnitsAST
MulVarDefAST
ConstUnitAST
FuncFParamsAST
FuncFParamAST
BlockAST
BlockItemAST
StmtAST
ExpAST
LvalAST
LvalUnitAST
PrimaryExpAST
UnaryExpAST
FuncRParamsAST
MulExpAST
AddExpAST
RelExpAST
EqExpAST
LAndExpAST
LOrExpAST
*/