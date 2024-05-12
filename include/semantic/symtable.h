#include "ast/ast.h"
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "enum.h"

#define HASH_TABLE_SIZE 0x3fff
#define STACK_DEEP

typedef struct fieldList* pFieldList;
typedef struct tableItem* pItem;
typedef struct hashTable* pHash;
typedef struct stack* pStack;
typedef struct table* pTable;

#define HT_SIZE 233
#define HT_SEED 32

class VarDefinition {
public:
    std::string type = "";
    bool is_array = false;
    int dimension_num = 0;
    std::vector<int> dimensions;
    VarDefinition(std::string tp, bool ia, int dim_num)
        : type(tp)
        , is_array(ia)
        , dimension_num(dim_num) {}
    VarDefinition() = default;
};
using var_name2def = std::unordered_map<std::string, VarDefinition>;
class FuncDefinition {
public:
    std::string func_type;
    var_name2def func_param_table;
    std::vector<VarDefinition> func_param_vector;
    FuncDefinition() = default;
};
class SymbolTable {
public:
    std::unordered_map<std::string, FuncDefinition> func_table;
    std::vector<var_name2def> var_table_stack;

public:
    SymbolTable() {
        var_table_stack.push_back(std::unordered_map<std::string, VarDefinition>());
    }
    ~SymbolTable() = default;
    bool search_func(std::string func_name) {  //
        return func_table.count(func_name) > 0;
    }
    bool search_var(std::string var_name) {
        int n = var_table_stack.size();
        for (int i = n - 1; i >= 0; i--) {
            if (var_table_stack[i].count(var_name) > 0)  return true; 
        }
        return false;
    }
    bool search_var4def(std::string var_name) {
        return var_table_stack.back().count(var_name) > 0;
    }
    const VarDefinition& get_var_def(std::string var_name) {
        int n = var_table_stack.size();
        for (int i = n - 1; i >= 0; i--) {
            if (var_table_stack[i].count(var_name) > 0)  return var_table_stack[i][var_name]; 
        }
        throw std::runtime_error("this code should never be reached");
    }
    void insert_func(std::string func_name, std::string type) {  // must search before insert
        func_table[func_name].func_type = type;
    }
    void insert_var(std::string var_name, VarDefinition var_def) {  // must search before insert
        auto& var_table = var_table_stack.back();
        var_table[var_name].type = var_def.type;
        var_table[var_name].dimension_num = var_def.dimension_num;
        var_table[var_name].is_array = var_def.is_array;
        var_table[var_name].dimensions = var_def.dimensions;
    }
    void enter_block(std::string func_block) {
        auto var_def = var_name2def();
        if (func_block != "") {
            const auto& param_table = func_table[func_block].func_param_table;
            for (auto pair : param_table) {
                var_def[pair.first] = pair.second;
            }
        }
        var_table_stack.push_back(var_def);
    }
    void exit_block() {
        var_table_stack.pop_back();
    }
    std::unordered_map<std::string, VarDefinition>& get_func_param_table(std::string func_name) {
        return func_table[func_name].func_param_table;
    }
    std::vector<VarDefinition>& get_func_param_vector(std::string func_name) {
        return func_table[func_name].func_param_vector;
    }
};