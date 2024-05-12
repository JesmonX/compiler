#ifndef ENUM_H
#define ENUM_H

// define node type
typedef enum _nodeType {
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_ID,
    TOKEN_TYPE,
    // TOKEN_COMMA,
    // TOKEN_SEMI,
    // TOKEN_ASSIGNOP,
    // TOKEN_RELOP,
    // TOKEN_PLUS,
    // TOKEN_MINUS,
    TOKEN_OTHER,
    NOT_A_TOKEN

} NodeType;

typedef enum _kind { BASIC, ARRAY, STRUCTURE, FUNCTION } Kind;
typedef enum _basicType { INT_TYPE, FLOAT_TYPE } BasicType;
typedef enum _errorType {
    UNDEF_VAR = 1,         // Undefined Variable
    UNDEF_FUNC,            // Undefined Function
    REDEF_VAR,             // Redefined Variable
    REDEF_FUNC,            // Redefined Function
    TYPE_MISMATCH_ASSIGN,  // Type mismatchedfor assignment.
    LEFT_VAR_ASSIGN,  // The left-hand side of an assignment must be a variable.
    TYPE_MISMATCH_OP,      // Type mismatched for operands.
    TYPE_MISMATCH_RETURN,  // Type mismatched for return.
    FUNC_AGRC_MISMATCH,    // Function is not applicable for arguments
    NOT_A_ARRAY,           // Variable is not a Array
    NOT_A_FUNC,            // Variable is not a Function
    NOT_A_INT,             // Variable is not a Integer
    ILLEGAL_USE_DOT,       // Illegal use of "."
    NONEXISTFIELD,         // Non-existentfield
    REDEF_FEILD,           // Redefined field
    DUPLICATED_NAME,       // Duplicated name
    UNDEF_STRUCT           // Undefined structure
} ErrorType;

enum ast_node_type {
    BaseAST_node,
    // normally not base
    FuncDefAST_node,
    VarDefAST_node,
    DeclAST_node,
    CompUnitAST_node,
    CompUnitsAST_node,
    MulVarDefAST_node,
    ConstUnitAST_node,
    FuncFParamsAST_node,
    FuncFParamAST_node,
    BlockAST_node,
    BlockItemAST_node,
    StmtAST_node,
    ExpAST_node,
    LvalAST_node,
    LvalUnitAST_node,
    PrimaryExpAST_node,
    UnaryExpAST_node,
    FuncRParamsAST_node,
    MulExpAST_node,
    AddExpAST_node,
    RelExpAST_node,
    EqExpAST_node,
    LAndExpAST_node,
    LOrExpAST_node
};
#endif