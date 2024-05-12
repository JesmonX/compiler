#include "SemanticAnalysis.h"
#include "ast/ast.h"
#include "enum.h"
#include <cassert>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <vector>
// 递归遍历AST树的函数

// std::vector<typename Tp, typename Alloc>
using runnerr = std::runtime_error;
using std::endl;
void traverseAST(const ASTPtr& node) {
    if (!node) return;
    std::cout << "Visiting Node Type: " << typeid(*node).name() << std::endl;  // Print the type of the node

    if (auto funcDef = dynamic_cast<FuncDefAST*>(node.get())) {
        std::cout << "Function Definition: " << funcDef->ident << std::endl;
        traverseAST(funcDef->func_fparams);  // 遍历函数参数
        traverseAST(funcDef->block);         // 遍历函数体
    } else if (auto varDef = dynamic_cast<VarDefAST*>(node.get())) {
        std::cout << "Variable Definition: " << varDef->ident;
        std::cout << " of type " << (varDef->type == 3 ? "array" : "int") << std::endl;

        // 根据变量定义的类型决定遍历方式
        if (varDef->type == 1) {
            traverseAST(varDef->exp);
        } else if (varDef->type == 3) {
            traverseAST(varDef->unit);
        }
    } else if (auto block = dynamic_cast<BlockAST*>(node.get())) {
        std::cout << "Block" << std::endl;
        traverseAST(block->item);
    } else if (auto compUnit = dynamic_cast<CompUnitAST*>(node.get())) {
        std::cout << "Compilation Unit" << std::endl;
        traverseAST(compUnit->comp_units);
    } else if (auto compUnits = dynamic_cast<CompUnitsAST*>(node.get())) {
        std::cout << "Compilation Units" << std::endl;
        traverseAST(compUnits->comp_unit);
        traverseAST(compUnits->def);
    } else if (auto mulVarDef = dynamic_cast<MulVarDefAST*>(node.get())) {
        std::cout << "Multiple Variable Definitions" << std::endl;
        traverseAST(mulVarDef->mul_var_def);
        traverseAST(mulVarDef->var_def);
    } else if (auto constUnit = dynamic_cast<ConstUnitAST*>(node.get())) {
        std::cout << "Constant Unit: " << constUnit->int_const << std::endl;
        traverseAST(constUnit->unit);
    } else if (auto funcFParams = dynamic_cast<FuncFParamsAST*>(node.get())) {
        std::cout << "Function Formal Parameters" << std::endl;
        traverseAST(funcFParams->param);
        traverseAST(funcFParams->params);
    } else if (auto funcFParam = dynamic_cast<FuncFParamAST*>(node.get())) {
        std::cout << "Function Formal Parameter: " << funcFParam->ident << std::endl;
        traverseAST(funcFParam->unit);
    } else if (auto decl = dynamic_cast<DeclAST*>(node.get())) {
    }
    // 添加其他节点类型的处理
}
void SmAnalysis::traverseCompUnitAST(CompUnitAST* node) {
    traverse_by_node(node->comp_units);
}
// dzk
void SmAnalysis::traverseCompUnitsAST(CompUnitsAST* node) {
    if (node->comp_unit == nullptr)
        traverse_by_node(node->def);
    else {
        traverse_by_node(node->def);
        traverse_by_node(node->comp_unit);
    }
}
void SmAnalysis::traverseFuncDefAST(FuncDefAST* node) {
    auto func_name = node->ident;
    auto func_type = node->func_type;
    cur_func       = func_name;
    if (symbol_table.search_func(func_name)) throw runnerr("Function defined: " + func_name);
    symbol_table.insert_func(func_name, func_type);
    traverseFuncFParamsAST(dynamic_cast<FuncFParamsAST*>(node->func_fparams.get()), func_name);
    func_block = func_name;
    traverseBlockAST(dynamic_cast<BlockAST*>(node->block.get()));
    cur_func = "";
}
void SmAnalysis::traverseFuncFParamsAST(FuncFParamsAST* node, std::string func_name) {
    if (!node) return;
    traverseFuncFParamsAST(dynamic_cast<FuncFParamsAST*>(node->params.get()), func_name);
    traverseFuncFParamAST(dynamic_cast<FuncFParamAST*>(node->param.get()), func_name);
}
template <class T> int get_node_unit_dim(T* node) {
    int dim_num = 1;
    if (node == nullptr) return dim_num;
    while (node->unit != nullptr) {
        node = dynamic_cast<T*>(node->unit.get());
        dim_num++;
    }
    return dim_num;
}
void SmAnalysis::traverseFuncFParamAST(FuncFParamAST* node, std::string func_name) {
    assert(node);
    auto  var_name     = node->ident;
    auto  param_type   = node->type;
    auto& param_table  = symbol_table.get_func_param_table(func_name);
    auto& param_vector = symbol_table.get_func_param_vector(func_name);
    if (param_table.count(var_name) > 0) throw runnerr("Func Param defined: " + var_name);
    switch (param_type) {
        case 0:
            param_table[var_name].type = "int";
            param_vector.push_back(param_table[var_name]);
            break;
        case 1:
            param_table[var_name].type     = "int";
            param_table[var_name].is_array = true;
            param_table[var_name].dimension_num = get_node_unit_dim<ConstUnitAST>(dynamic_cast<ConstUnitAST*>(node->unit.get()));
            if (param_table[var_name].dimension_num > 1) { traverseConstUnitAST(node->unit, param_table[var_name].dimensions, 0); }
            param_vector.push_back(param_table[var_name]);
            break;
        case 2:
            param_table[var_name].type     = "int";
            param_table[var_name].is_array = true;
            // + 1 because when its unit's unit is null, should be 2
            param_table[var_name].dimension_num = 1 + get_node_unit_dim<ConstUnitAST>(dynamic_cast<ConstUnitAST*>(node->unit.get()));
            if (param_table[var_name].dimension_num > 1) { traverseConstUnitAST(node->unit, param_table[var_name].dimensions, 0); }
            param_vector.push_back(param_table[var_name]);
            break;
        default: std::cout << "goddamnit wtf from FParam" << std::endl;
    }
}
void SmAnalysis::traverseFuncRParamsAST(FuncRParamsAST* node, std::string func_name, int& index) {
    if (node->params != NULL) { traverseFuncRParamsAST(dynamic_cast<FuncRParamsAST*>(node->params.get()), func_name, index); }
    if (index > (symbol_table.func_table[func_name].func_param_vector.size() - 1))
        throw runnerr("too many arguments to "+func_name);
    const VarDefinition& hope_def    = symbol_table.func_table[func_name].func_param_vector[index];
    auto&                exp_var_def = traverseExpAST(node->exp);
    if (!compare_var_def(hope_def, exp_var_def)) throw runnerr("type not compatible: " + func_name);
    index++;
}
void SmAnalysis::traverseVarDefAST(VarDefAST* node) {
    auto var_name = node->ident;
    auto var_type = node->type;
    switch (var_type) {
        case 1: {  // IDENT '=' Exp
            if (symbol_table.search_var4def(var_name)) throw runnerr("var defined: " + var_name);
            hope_exp_to_comply.type = "int";
            traverseExpAST(node->exp);
            hope_exp_to_comply.type = "";
            auto var_def            = VarDefinition("int", false, 0);
            symbol_table.insert_var(var_name, var_def);
            // TO CHECK
            break;
        }
        case 2: {  // IDENT
            if (symbol_table.search_var4def(var_name)) throw runnerr("var defined: " + var_name);
            auto var_def = VarDefinition("int", false, 0);
            symbol_table.insert_var(var_name, var_def);
            break;
        }
        case 3: {  // IDENT VarDefunit
            if (symbol_table.search_var4def(var_name)) throw runnerr("var defined: " + var_name);
            int  dim_num = get_node_unit_dim<ConstUnitAST>(dynamic_cast<ConstUnitAST*>(node->unit.get()));
            auto var_def = VarDefinition("int", true, dim_num);

            if (dim_num > 1) {
                traverseConstUnitAST(node->unit, var_def.dimensions, 0);
                var_def.dimensions.erase(var_def.dimensions.begin());
            }
            symbol_table.insert_var(var_name, var_def);
            break;
        }
        default: std::cout << "shit happens when vardefast" << std::endl;
    }
}
void SmAnalysis::traverseDeclAST(DeclAST* node) {
    if (node->func_def) traverse_by_node(node->func_def);
    if (node->var_def) traverse_by_node(node->var_def);
}
void SmAnalysis::traverseMulVarDefAST(MulVarDefAST* node) {
    if (node->mul_var_def) traverseMulVarDefAST(node->mul_var_def);
    if (node->var_def)
        traverseVarDefAST(node->var_def);
    else
        std::cout << "wtf you tell me mulvardef.var_def is null?" << std::endl;
}

// seems like no need to handle this situation
void SmAnalysis::traverseConstUnitAST(ConstUnitAST* node, std::vector<int>& dims, int index) {
    if (nullptr == node)
        ;
    else {
        int int_const = node->int_const;
        dims.push_back(int_const);
        traverseConstUnitAST(node->unit, dims, index + 1);
    }
}

void SmAnalysis::traverseBlockAST(BlockAST* node) {
    if (node->item == nullptr) return;
    symbol_table.enter_block(func_block);
    func_block = "";
    traverseBlockItemAST(dynamic_cast<BlockItemAST*>(node->item.get()));
    symbol_table.exit_block();
}

void SmAnalysis::traverseBlockItemAST(BlockItemAST* node) {
    if (node->block_item) traverse_by_node(node->block_item);
    if (node->mul_var_def) traverse_by_node(node->mul_var_def);
    if (node->stmt) traverse_by_node(node->stmt);
}
void SmAnalysis::traverseStmtAST(StmtAST* node) {
    switch (node->type) {
        case 1: {  // LVal '=' Exp ';'
            auto& lval_def = traverseLvalAST(node->lval);
            // Exp must be int (for function specifically)
            hope_exp_to_comply.type = "int";
            auto& exp_def           = traverseExpAST(node->exp);
            hope_exp_to_comply.type = "";
            if (lval_def.is_array) throw runnerr("Array type is not assignable");
            if (exp_def.is_array) throw runnerr("SHITSHITSHIT");
            if (exp_def.type == "void") throw runnerr("Assigning to 'int' from incompatible type 'void'");
            break;
        }
        case 2:  // Exp ';'
            // do nothing currently
            traverseExpAST(node->exp);
            break;
        case 3:  // Block
            traverse_by_node(node->block);
            break;
        case 4:  // IF '(' Exp ')' Stmt
            hope_exp_to_comply.type = "int";
            traverseExpAST(node->exp);
            hope_exp_to_comply.type = "";
            traverseStmtAST(node->stmt);
            break;
        case 5:  // IF '(' Exp ')' Stmt ELSE Stmt
            hope_exp_to_comply.type = "int";
            traverseExpAST(node->exp);
            hope_exp_to_comply.type = "";
            traverseStmtAST(node->stmt);
            traverseStmtAST(node->stmt2);
            break;
        case 6:  // WHILE '(' Exp ')' Stmt
            hope_exp_to_comply.type = "int";
            traverseExpAST(node->exp);
            hope_exp_to_comply.type = "";
            traverseStmtAST(node->stmt);
            break;
        case 7:  // BREAK ';'
            break;
        case 8:  // CONTINUE ';'
            break;
        case 9: {  // RETURN ';'
            if (symbol_table.func_table[cur_func].func_type != "void") throw runnerr("Return type mismatch");
            break;
        }
        case 10: {  // RETURN Exp ';'
            hope_exp_to_comply.type = "int";
            auto& exp_def           = traverseExpAST(node->exp);
            hope_exp_to_comply.type = "";
            break;
        }
        default: break;
    }
}

const VarDefinition& SmAnalysis::traverseLvalAST(LvalAST* node) {
    auto lval_name = node->ident;
    std::cout << "lval_name: " << lval_name << std::endl;
    if (nullptr == node->unit) {  // IDENT
        if (false == symbol_table.search_var(lval_name)) throw runnerr("var not declared " + lval_name);
        return symbol_table.get_var_def(lval_name);
    } else {  // IDENT LValunit
        if (false == symbol_table.search_var(lval_name)) throw runnerr("var not declared " + lval_name);
        auto& var_def      = symbol_table.get_var_def(lval_name);
        int   dim_num      = get_node_unit_dim<LvalUnitAST>(dynamic_cast<LvalUnitAST*>(node->unit.get()));
        int   real_dim_num = var_def.dimension_num - dim_num;
        
        // traverse unit
        traverseLvalUnitAST(node->unit);
        
        if (real_dim_num > 0) {
            // shit code just for test cases
            VarDefinition result_var_def = var_def;
            result_var_def.dimension_num = real_dim_num;
            for (int i = 1; i <= dim_num; i++) {
                result_var_def.dimensions.erase(result_var_def.dimensions.begin());
            }
            temp_def_store.push_back(result_var_def);
            return temp_def_store.back();
        } else if (real_dim_num == 0) {
            return pre_def_int;
        } else
            throw runnerr("Subscripting a non-array");
    }
    assert(false);
}
void SmAnalysis::traverseLvalUnitAST(LvalUnitAST* node) {
    if (node->unit != NULL) { traverseLvalUnitAST(node->unit); }
    auto& exp_def = traverseExpAST(node->exp);
    if ((exp_def.type != "int") || (exp_def.is_array == true)) throw runnerr("array subscript is not an integer");
}
const VarDefinition& SmAnalysis::traversePrimaryExpAST(PrimaryExpAST* node) {
    if (nullptr == node->exp) {
        if (nullptr == node->lval) {  // INT_CONST
            // do nothing currently
            return pre_def_int;
        } else {  // LVal
            return traverseLvalAST(node->lval);
        }
    } else {  // '(' Exp ')'
        return traverseExpAST(node->exp);
    }
}
const VarDefinition& SmAnalysis::traverseUnaryExpAST(UnaryExpAST* node) {
    auto unary_exp_name = node->ident;
    switch (node->type) {
        case 1: {
            return traversePrimaryExpAST(node->primary_exp);
        }
        case 2: {  // IDENT '(' ')'
            if (!symbol_table.search_func(unary_exp_name)) { throw runnerr("not func thing get called: " + unary_exp_name); }
            if ("int" == symbol_table.func_table[unary_exp_name].func_type)
                return pre_def_int;
            else
                return pre_def_void;
        }
        case 3: {  // IDENT '(' FuncRParams ')'
            if (!symbol_table.search_func(unary_exp_name)) { throw runnerr("not func thing get called: " + unary_exp_name); }
            int index = 0;
            traverseFuncRParamsAST(dynamic_cast<FuncRParamsAST*>(node->params.get()), unary_exp_name, index);
            if ("int" == symbol_table.func_table[unary_exp_name].func_type)
                return pre_def_int;
            else
                return pre_def_void;
        }
        case 4:  // UnaryOp UnaryExp
            return traverseUnaryExpAST(node->unary_exp);
    }
    throw runnerr("shit happen in unaryexpast");
}

const VarDefinition& SmAnalysis::traverseMulExpAST(MulExpAST* node) {
    if (nullptr == node->mul_exp) {
        return traverseUnaryExpAST(node->unary_exp);
    } else {
        auto& mul_def = traverseMulExpAST(node->mul_exp);
        auto& unary_def = traverseUnaryExpAST(node->unary_exp);
        if (!compare_var_def(mul_def, unary_def))
            throw runnerr("Invalid operands to "+mul_def.type+" "+unary_def.type);
        return mul_def;
    }
}
const VarDefinition& SmAnalysis::traverseAddExpAST(AddExpAST* node) {
    if (nullptr == node->add_exp) {
        return traverseMulExpAST(node->mul_exp);
    } else {
        auto& add_def = traverseAddExpAST(node->add_exp);
        auto& mul_def = traverseMulExpAST(node->mul_exp);
        if (!compare_var_def(add_def, mul_def))
            throw runnerr("Invalid operands to "+add_def.type+ " "+mul_def.type);
        return add_def;
    }
}
const VarDefinition& SmAnalysis::traverseRelExpAST(RelExpAST* node) {
    if (nullptr == node->rel_exp) {
        return traverseAddExpAST(node->add_exp);
    } else {
        auto& rel_def = traverseRelExpAST(node->rel_exp);
        auto& add_def =  traverseAddExpAST(node->add_exp);
        return rel_def;
    }
}
const VarDefinition& SmAnalysis::traverseEqExpAST(EqExpAST* node) {
    if (nullptr == node->eq_exp) {
        return traverseRelExpAST(node->rel_exp);
    } else {
        auto& eq_def = traverseEqExpAST(node->eq_exp);
        auto& rel_def = traverseRelExpAST(node->rel_exp);
        return eq_def;
    }
}
const VarDefinition& SmAnalysis::traverseLAndExpAST(LAndExpAST* node) {
    if (node->land_exp == nullptr) {
        traverseEqExpAST(node->eq_exp);
    } else {
        // TO DO
    }
}
const VarDefinition& SmAnalysis::traverseLOrExpAST(LOrExpAST* node) {
    if (node->lor_exp == nullptr) {  // LAndExp
        traverseLAndExpAST(dynamic_cast<LAndExpAST*>(node->land_exp.get()));
    } else {  // LOrExp OR LAndExp
              // TO DO
              // if (hope_exp_to_comply.type == "int")
              //     throw runnerr("Error in declaration (like int a = true ||
              //     bool)");
    }
}
const VarDefinition& SmAnalysis::traverseExpAST(ExpAST* node) {
    assert(node->lorexp != NULL);
    traverseLOrExpAST(node->lorexp);
}

ast_node_type SmAnalysis::get_type(const ASTPtr& node) {
    if (dynamic_cast<FuncDefAST*>(node.get())) return FuncDefAST_node;
    if (dynamic_cast<VarDefAST*>(node.get())) return VarDefAST_node;
    if (dynamic_cast<DeclAST*>(node.get())) return DeclAST_node;
    if (dynamic_cast<CompUnitAST*>(node.get())) return CompUnitAST_node;
    if (dynamic_cast<CompUnitsAST*>(node.get())) return CompUnitsAST_node;
    if (dynamic_cast<MulVarDefAST*>(node.get())) return MulVarDefAST_node;
    if (dynamic_cast<ConstUnitAST*>(node.get())) return ConstUnitAST_node;
    if (dynamic_cast<FuncFParamsAST*>(node.get())) return FuncFParamsAST_node;
    if (dynamic_cast<FuncFParamAST*>(node.get())) return FuncFParamAST_node;
    if (dynamic_cast<BlockAST*>(node.get())) return BlockAST_node;
    if (dynamic_cast<BlockItemAST*>(node.get())) return BlockItemAST_node;
    if (dynamic_cast<StmtAST*>(node.get())) return StmtAST_node;
    if (dynamic_cast<ExpAST*>(node.get())) return ExpAST_node;
    if (dynamic_cast<LvalAST*>(node.get())) return LvalAST_node;
    if (dynamic_cast<LvalUnitAST*>(node.get())) return LvalUnitAST_node;
    if (dynamic_cast<PrimaryExpAST*>(node.get())) return PrimaryExpAST_node;
    if (dynamic_cast<UnaryExpAST*>(node.get())) return UnaryExpAST_node;
    if (dynamic_cast<FuncRParamsAST*>(node.get())) return FuncRParamsAST_node;
    if (dynamic_cast<MulExpAST*>(node.get())) return MulExpAST_node;
    if (dynamic_cast<AddExpAST*>(node.get())) return AddExpAST_node;
    if (dynamic_cast<RelExpAST*>(node.get())) return RelExpAST_node;
    if (dynamic_cast<EqExpAST*>(node.get())) return EqExpAST_node;
    if (dynamic_cast<LAndExpAST*>(node.get())) return LAndExpAST_node;
    if (dynamic_cast<LOrExpAST*>(node.get())) return LOrExpAST_node;
    std::cout << "wtf!" << std::endl;
    return BaseAST_node;
}
void SmAnalysis::traverse_by_node_type(ast_node_type node_type, const ASTPtr& node) {
    if (node_type == FuncDefAST_node) traverseFuncDefAST(dynamic_cast<FuncDefAST*>(node.get()));
    if (node_type == VarDefAST_node) traverseVarDefAST(dynamic_cast<VarDefAST*>(node.get()));
    if (node_type == DeclAST_node) traverseDeclAST(dynamic_cast<DeclAST*>(node.get()));
    if (node_type == CompUnitAST_node) traverseCompUnitAST(dynamic_cast<CompUnitAST*>(node.get()));
    if (node_type == CompUnitsAST_node) traverseCompUnitsAST(dynamic_cast<CompUnitsAST*>(node.get()));
    if (node_type == MulVarDefAST_node) traverseMulVarDefAST(dynamic_cast<MulVarDefAST*>(node.get()));
    // if (node_type == ConstUnitAST_node) traverseConstUnitAST(dynamic_cast<ConstUnitAST*>(node.get()));
    // if (node_type == FuncFParamsAST_node)
    // traverseFuncFParamsAST(dynamic_cast<FuncFParamsAST*>(node.get())); if
    // (node_type == FuncFParamAST_node)
    // traverseFuncFParamAST(dynamic_cast<FuncFParamAST*>(node.get()));
    if (node_type == BlockAST_node) traverseBlockAST(dynamic_cast<BlockAST*>(node.get()));
    if (node_type == BlockItemAST_node) traverseBlockItemAST(dynamic_cast<BlockItemAST*>(node.get()));
    if (node_type == StmtAST_node) traverseStmtAST(dynamic_cast<StmtAST*>(node.get()));
    if (node_type == ExpAST_node) traverseExpAST(dynamic_cast<ExpAST*>(node.get()));
    if (node_type == LvalAST_node) traverseLvalAST(dynamic_cast<LvalAST*>(node.get()));
    if (node_type == LvalUnitAST_node) traverseLvalUnitAST(dynamic_cast<LvalUnitAST*>(node.get()));
    if (node_type == PrimaryExpAST_node) traversePrimaryExpAST(dynamic_cast<PrimaryExpAST*>(node.get()));
    if (node_type == UnaryExpAST_node) traverseUnaryExpAST(dynamic_cast<UnaryExpAST*>(node.get()));
    // if (node_type == FuncRParamsAST_node)
    // traverseFuncRParamsAST(dynamic_cast<FuncRParamsAST*>(node.get()));
    if (node_type == MulExpAST_node) traverseMulExpAST(dynamic_cast<MulExpAST*>(node.get()));
    if (node_type == AddExpAST_node) traverseAddExpAST(dynamic_cast<AddExpAST*>(node.get()));
    if (node_type == RelExpAST_node) traverseRelExpAST(dynamic_cast<RelExpAST*>(node.get()));
    if (node_type == EqExpAST_node) traverseEqExpAST(dynamic_cast<EqExpAST*>(node.get()));
    if (node_type == LAndExpAST_node) traverseLAndExpAST(dynamic_cast<LAndExpAST*>(node.get()));
    if (node_type == LOrExpAST_node) traverseLOrExpAST(dynamic_cast<LOrExpAST*>(node.get()));
}
void SmAnalysis::traverse_by_node(const ASTPtr& node) {
    auto type = get_type(node);
    traverse_by_node_type(type, node);
}
