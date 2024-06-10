%{
#include <stdio.h>
#include <ast/ast.h>
void yyerror(const char *s);
extern int yylex(void);
using std::unique_ptr;
using std::make_unique;
using std::move;
extern unique_ptr<BaseAST> ast;
%}

/// types
%union {
    int ival;
    BaseAST *node;
    char str[64];
    std::string *str_val;
}

%start CompUnit

%token <ival> INT_CONST
%token <str_val> IDENT
%token VOID INT
%token IF ELSE WHILE BREAK CONTINUE RETURN
%token LTE GTE EQ NEQ AND OR

%type <ival> UnaryOp
%type <node> CompUnit   VarDef  FuncDef  FuncFParams FuncFParam MulVarDef CompUnits Decl VarDefunit
%type <node> Block BlockItem Stmt LVal PrimaryExp FuncFParamUnit LValunit FuncRParams
%type <node> UnaryExp   MulExp AddExp RelExp EqExp LAndExp LOrExp Exp

%precedence THEN
%precedence ELSE
%%

CompUnit
    : CompUnits{
        auto comp_unit = make_unique<CompUnitAST>();
        comp_unit->comp_units = unique_ptr<BaseAST>($1);
        ast = move(comp_unit);
    }
    ;

CompUnits 
    : Decl{
        auto a = new CompUnitsAST();
        a->def = unique_ptr<BaseAST>($1);
        a->comp_unit = NULL;
        $$ = a;
    }   
    | CompUnits Decl{
        auto a = new CompUnitsAST();
        a->def = unique_ptr<BaseAST>($2);
        a->comp_unit = unique_ptr<BaseAST>($1);
        $$ = a;
    } 
    ;

Decl
    : INT MulVarDef ';'{
        auto a = new DeclAST();
        a->func_def = NULL;
        a->var_def = unique_ptr<BaseAST>($2);
        $$ = a;

    }
    | FuncDef{
        auto a = new DeclAST();
        a->func_def = unique_ptr<BaseAST>($1);
        a->var_def = NULL;
        $$ = a;
    }
    ;

MulVarDef 
    : MulVarDef ',' VarDef{
        auto a = new MulVarDefAST();
        a->mul_var_def = unique_ptr<BaseAST>($1);
        a->var_def = unique_ptr<BaseAST>($3);
        $$ = a;
    } 
    | VarDef{
        auto a = new MulVarDefAST();
        a->mul_var_def = NULL;
        a->var_def = unique_ptr<BaseAST>($1);
        $$ = a;
    } 
    ;

VarDef 
    : IDENT '=' Exp{
        auto var_def = new VarDefAST();
        var_def->ident = *($1);
        var_def->exp = unique_ptr<BaseAST>($3);
        var_def->type = 1;
        $$ = var_def;
    }
    | IDENT{
        auto var_def = new VarDefAST();
        var_def->ident = *($1);
        var_def->type = 2;
        $$ = var_def;
    }
    | IDENT VarDefunit{
        auto a = new VarDefAST();
        a->ident = *($1);
        a->type = 3;
        a->unit = unique_ptr<BaseAST>($2);
        $$ = a;
    }
    ;

VarDefunit 
    : '[' INT_CONST ']'{
        auto var_def_unit = new ConstUnitAST();
        var_def_unit->int_const = $2;
        var_def_unit->unit = NULL;
        $$ = var_def_unit;
    }
    |  '[' INT_CONST ']' VarDefunit {
        auto var_def_unit = new ConstUnitAST();
        var_def_unit->int_const = $2;
        var_def_unit->unit = unique_ptr<BaseAST>($4);
        $$ = var_def_unit;
    }
    ;

FuncDef 
    : INT IDENT '(' FuncFParams ')' Block{}
    {
        auto func_def = new FuncDefAST();
        func_def->ident = *($2);
        func_def->func_type = "int";
        func_def->func_fparams = unique_ptr<BaseAST>($4);
        func_def->block = unique_ptr<BaseAST>($6);
        $$ = func_def;
    }
    | VOID IDENT '(' FuncFParams ')' Block{
        auto func_def = new FuncDefAST();
        func_def->ident = *($2);
        func_def->func_type = "void";
        func_def->func_fparams = unique_ptr<BaseAST>($4);
        func_def->block = unique_ptr<BaseAST>($6);
        $$ = func_def;
    }
    | INT IDENT '(' ')' Block{
        auto func_def = new FuncDefAST();
        func_def->ident = *($2);
        func_def->func_type = "int";
        func_def->func_fparams = NULL;
        func_def->block = unique_ptr<BaseAST>($5);
        $$ = func_def;
    }
    | VOID IDENT '(' ')' Block{
        auto func_def = new FuncDefAST();
        func_def->ident = *($2);
        func_def->func_type = "void";
        func_def->func_fparams = NULL;
        func_def->block = unique_ptr<BaseAST>($5);
        $$ = func_def;
    }
    ;

FuncFParams 
    : FuncFParam{
        auto func_f_params = new FuncFParamsAST();
        func_f_params->param = unique_ptr<BaseAST>($1);
        func_f_params->params = NULL;
        $$ = func_f_params;
    }
    | FuncFParams ',' FuncFParam{
        auto func_f_params = new FuncFParamsAST();
        func_f_params->param = unique_ptr<BaseAST>($3);
        func_f_params->params = unique_ptr<BaseAST>($1);
        $$ = func_f_params;
    }
    ;

FuncFParam 
    : INT IDENT{
        auto func_f_param = new FuncFParamAST();
        func_f_param->ident = *($2);
        func_f_param->type = 0;
        func_f_param->unit = NULL;
        $$ = func_f_param;
    }
    | INT IDENT '[' ']'{
        auto func_f_param = new FuncFParamAST();
        func_f_param->ident = *($2);
        func_f_param->type = 1;
        func_f_param->unit = NULL;
        $$ = func_f_param;
    }
    | INT IDENT '[' ']' FuncFParamUnit{
        auto func_f_param = new FuncFParamAST();
        func_f_param->ident = *($2);
        func_f_param->type = 2;
        func_f_param->unit = unique_ptr<BaseAST>($5);
        $$ = func_f_param;
    }
    ;

FuncFParamUnit 
    : '[' INT_CONST ']'{
        auto a = new ConstUnitAST();
        a->int_const = $2;
        a->unit = NULL;
        $$ = a;
    }
    |  '[' INT_CONST ']' FuncFParamUnit{
        auto a = new ConstUnitAST();
        a->int_const = $2;
        a->unit = unique_ptr<BaseAST>($4);
        $$ = a;
    }
    ;

Block 
    : '{' '}'{
        auto a = new BlockAST();
        a->item = NULL;
        $$ = a;
    }
    | '{' BlockItem '}'{
        auto a = new BlockAST();
        a->item = unique_ptr<BaseAST>($2);
        $$ = a;
    }
    ;

BlockItem 
    : INT MulVarDef ';'{
        auto a = new BlockItemAST();
        a->mul_var_def = unique_ptr<BaseAST>($2);
        a->stmt = NULL;
        a->block_item = NULL;
        $$ = a;
    }
    | Stmt{
        auto a = new BlockItemAST();
        a->stmt = unique_ptr<BaseAST>($1);
        a->mul_var_def = NULL;
        a->block_item = NULL;
        $$ = a;
    }
    | BlockItem INT MulVarDef ';'{
        auto a = new BlockItemAST();
        a->mul_var_def = unique_ptr<BaseAST>($3);
        a->stmt = NULL;
        a->block_item = unique_ptr<BaseAST>($1);
        $$ = a;
    }
    | BlockItem Stmt{
        auto a = new BlockItemAST();
        a->stmt = unique_ptr<BaseAST>($2);
        a->mul_var_def = NULL;
        a->block_item = unique_ptr<BaseAST>($1);
        $$ = a;
    }
    ;

Stmt 
    : LVal '=' Exp ';'{
        auto a = new StmtAST();
        a->lval = unique_ptr<BaseAST>($1);
        a->exp = unique_ptr<BaseAST>($3);
        a->type = 1;
        $$ = a;
    }
    | Exp ';'{
        auto a = new StmtAST();
        a->exp = unique_ptr<BaseAST>($1);
        a->type = 2;
        $$ = a;
    }
    | Block{
        auto a = new StmtAST();
        a->block = unique_ptr<BaseAST>($1);
        a->type = 3;
        $$ = a;
    }
    | IF '(' Exp ')' Stmt %prec THEN{
        auto a = new StmtAST();
        a->exp = unique_ptr<BaseAST>($3);
        a->stmt = unique_ptr<BaseAST>($5);
        a->type = 4;
        $$ = a;
    }
    | IF '(' Exp ')' Stmt ELSE Stmt{
        auto a = new StmtAST();
        a->exp = unique_ptr<BaseAST>($3);
        a->stmt = unique_ptr<BaseAST>($5);
        a->type = 5;
        a->stmt2 = unique_ptr<BaseAST>($7);
        $$ = a;
    }
    | WHILE '(' Exp ')' Stmt{
        auto a = new StmtAST();
        a->exp = unique_ptr<BaseAST>($3);
        a->stmt = unique_ptr<BaseAST>($5);
        a->type = 6;
        $$ = a;
    }
    | BREAK ';'{
        auto a = new StmtAST();
        a->type = 7;
        $$ = a;
    }
    | CONTINUE ';'{
        auto a = new StmtAST();
        a->type = 8;
        $$ = a;
    }
    | RETURN ';'{
        auto a = new StmtAST();
        a->type = 9;
        $$ = a;
    }
    | RETURN Exp ';'{
        auto a = new StmtAST();
        a->exp = unique_ptr<BaseAST>($2);
        a->type = 10;
        $$ = a;
    }
    ;

Exp 
    : LOrExp{
        auto a = new ExpAST();
        a->lorexp = unique_ptr<BaseAST>($1);
        $$ = a;
    }
    ;

LVal 
    : IDENT{
        auto a = new LvalAST();
        a->ident = *($1);
        a->unit = NULL;
        $$ = a;
    }
    | IDENT LValunit{
        auto a = new LvalAST();
        a->ident = *($1);
        a->unit = unique_ptr<BaseAST>($2);
        $$ = a;
    }
    ;

LValunit 
    : '[' Exp ']'{
        auto a = new LvalUnitAST();
        a->exp = unique_ptr<BaseAST>($2);
        a->unit = NULL;
        $$ = a;
    }
    | LValunit '[' Exp ']'{
        auto a = new LvalUnitAST();
        a->exp = unique_ptr<BaseAST>($3);
        a->unit = unique_ptr<BaseAST>($1);
        $$ = a;
    }
    ;

PrimaryExp 
    : '(' Exp ')'{
        auto a = new PrimaryExpAST();
        a->exp = unique_ptr<BaseAST>($2);
        a->lval = NULL;
        a->type = 1;
        $$ = a;
    }
    | LVal{
        auto a = new PrimaryExpAST();
        a->lval = unique_ptr<BaseAST>($1);
        a->exp = NULL;
        a->type = 2;
        $$ = a;
    }
    | INT_CONST{
        auto a = new PrimaryExpAST();
        a->int_const = $1;
        a->lval = NULL;
        a->exp = NULL;
        a->type = 3;
        $$ = a;
    }
    ;

UnaryExp 
    : PrimaryExp{
        auto a = new UnaryExpAST();
        a->primary_exp = unique_ptr<BaseAST>($1);
        a->type = 1;
        $$ = a;
    }
    | IDENT '(' ')'{
        auto a = new UnaryExpAST();
        a->ident = *($1);
        a->type = 2;
        $$ = a;
    }
    | IDENT '(' FuncRParams ')'{
        auto a = new UnaryExpAST();
        a->ident = *($1);
        a->params = unique_ptr<BaseAST>($3);
        a->type = 3;
        $$ = a;
    }
    | UnaryOp UnaryExp{
        auto a = new UnaryExpAST();
        a->op_type = $1;
        a->unary_exp = unique_ptr<BaseAST>($2);
        a->type = 4;
        $$ = a;
    }
    ;

UnaryOp 
    : '+'{
        $$ = 1;
    }
    | '-'{
        $$ = 2;
    }
    | '!'{
        $$ = 3;
    }
    ;

FuncRParams 
    : Exp{
        auto a = new FuncRParamsAST();
        a->exp = unique_ptr<BaseAST>($1);
        a->params = NULL;
        $$ = a;
    }
    | FuncRParams ',' Exp{
        auto a = new FuncRParamsAST();
        a->exp = unique_ptr<BaseAST>($3);
        a->params = unique_ptr<BaseAST>($1);
        $$ = a;
    }
    ;

MulExp 
    : UnaryExp{
        auto a = new MulExpAST();
        a->unary_exp = unique_ptr<BaseAST>($1);
        a->mul_exp = NULL;
        $$ = a;
    }
    | MulExp '*' UnaryExp{
        auto a = new MulExpAST();
        a->unary_exp = unique_ptr<BaseAST>($3);
        a->mul_exp = unique_ptr<BaseAST>($1);
        a->op_type = 1;
        $$ = a;
    }
    | MulExp '/' UnaryExp{
        auto a = new MulExpAST();
        a->unary_exp = unique_ptr<BaseAST>($3);
        a->mul_exp = unique_ptr<BaseAST>($1);
        a->op_type = 2;
        $$ = a;
    }
    | MulExp '%' UnaryExp{
        auto a = new MulExpAST();
        a->unary_exp = unique_ptr<BaseAST>($3);
        a->mul_exp = unique_ptr<BaseAST>($1);
        a->op_type = 3;
        $$ = a;
    }
    ;

AddExp 
    : MulExp{
        auto a = new AddExpAST();
        a->mul_exp = unique_ptr<BaseAST>($1);
        a->add_exp = NULL;
        $$ = a;
    }
    | AddExp '+' MulExp{
        auto a = new AddExpAST();
        a->mul_exp = unique_ptr<BaseAST>($3);
        a->add_exp = unique_ptr<BaseAST>($1);
        a->op_type = 1;
        $$ = a;
    }
    | AddExp '-' MulExp{
        auto a = new AddExpAST();
        a->mul_exp = unique_ptr<BaseAST>($3);
        a->add_exp = unique_ptr<BaseAST>($1);
        a->op_type = 2;
        $$ = a;
    }
    ;

RelExp 
    : AddExp{
        auto a = new RelExpAST();
        a->add_exp = unique_ptr<BaseAST>($1);
        a->rel_exp = NULL;
        $$ = a;
    }
    | RelExp '<' AddExp{
        auto a = new RelExpAST();
        a->add_exp = unique_ptr<BaseAST>($3);
        a->rel_exp = unique_ptr<BaseAST>($1);
        a->op_type = 1;
        $$ = a;
    }
    | RelExp '>' AddExp{
        auto a = new RelExpAST();
        a->add_exp = unique_ptr<BaseAST>($3);
        a->rel_exp = unique_ptr<BaseAST>($1);
        a->op_type = 2;
        $$ = a;
    }
    | RelExp LTE AddExp{
        auto a = new RelExpAST();
        a->add_exp = unique_ptr<BaseAST>($3);
        a->rel_exp = unique_ptr<BaseAST>($1);
        a->op_type = 3;
        $$ = a;
    }
    | RelExp GTE AddExp{
        auto a = new RelExpAST();
        a->add_exp = unique_ptr<BaseAST>($3);
        a->rel_exp = unique_ptr<BaseAST>($1);
        a->op_type = 4;
        $$ = a;
    }
    ;

EqExp 
    : RelExp{
        auto a = new EqExpAST();
        a->rel_exp = unique_ptr<BaseAST>($1);
        a->eq_exp = NULL;
        $$ = a;
    }
    | EqExp EQ RelExp{
        auto a = new EqExpAST();
        a->rel_exp = unique_ptr<BaseAST>($3);
        a->eq_exp = unique_ptr<BaseAST>($1);
        a->op_type = 1;
        $$ = a;
    }
    | EqExp NEQ RelExp{
        auto a = new EqExpAST();
        a->rel_exp = unique_ptr<BaseAST>($3);
        a->eq_exp = unique_ptr<BaseAST>($1);
        a->op_type = 2;
        $$ = a;
    }
    ;

LAndExp 
    : EqExp{
        auto a = new LAndExpAST();
        a->eq_exp = unique_ptr<BaseAST>($1);
        a->land_exp = NULL;
        $$ = a;
    }
    | LAndExp AND EqExp{
        auto a = new LAndExpAST();
        a->eq_exp = unique_ptr<BaseAST>($3);
        a->land_exp = unique_ptr<BaseAST>($1);
        $$ = a;
    }
    ;

LOrExp 
    : LAndExp{
        auto a = new LOrExpAST();
        a->land_exp = unique_ptr<BaseAST>($1);
        a->lor_exp = NULL;
        $$ = a;
    }
    | LOrExp OR LAndExp{
        auto a = new LOrExpAST();
        a->land_exp = unique_ptr<BaseAST>($3);
        a->lor_exp = unique_ptr<BaseAST>($1);
        $$ = a;
    }
    ;



%%

void yyerror(const char *s) {
    printf("error: %s\n", s);
}
/*
CompUnit      ::= [CompUnit] (Decl | FuncDef);

BType         ::= "int";
Decl          ::= VarDecl;


VarDecl       ::= BType VarDef {"," VarDef} ";";
VarDef        ::= IDENT "=" InitVal
                | IDENT {"[" INT_CONST "]"};
InitVal       ::= Exp;

FuncDef       ::= FuncType IDENT "(" [FuncFParams] ")" Block;
FuncType      ::= "void" | "int";
FuncFParams   ::= FuncFParam {"," FuncFParam};
FuncFParam    ::= BType IDENT ["[" "]" {"[" INT_CONST "]"}];

Block         ::= "{" {BlockItem} "}";
BlockItem     ::= Decl | Stmt;
Stmt          ::= LVal "=" Exp ";"
                | Exp ";"
                | Block
                | "if" "(" Exp ")" Stmt ["else" Stmt]
                | "while" "(" Exp ")" Stmt
                | "break" ";"
                | "continue" ";"
                | "return" [Exp] ";";

Exp           ::= LOrExp;
LVal          ::= IDENT {"[" Exp "]"};
PrimaryExp    ::= "(" Exp ")" | LVal | Number;
Number        ::= INT_CONST;
UnaryExp      ::= PrimaryExp | IDENT "(" [FuncRParams] ")" | UnaryOp UnaryExp;
UnaryOp       ::= "+" | "-" | "!";
FuncRParams   ::= Exp {"," Exp};

MulExp        ::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp;
AddExp        ::= MulExp | AddExp ("+" | "-") MulExp;

RelExp        ::= AddExp | RelExp ("<" | ">" | "<=" | ">=") AddExp;
EqExp         ::= RelExp | EqExp ("==" | "!=") RelExp;
LAndExp       ::= EqExp | LAndExp "&&" EqExp;
LOrExp        ::= LAndExp | LOrExp "||" LAndExp;
*/