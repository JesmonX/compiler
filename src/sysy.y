%{
#include <stdio.h>
#include <ast/ast.h>
void yyerror(const char *s);
extern int yylex(void);
extern NodePtr root;
%}

/// types
%union {
    int ival;
    ExprPtr expr;
    OpType op;
    NodePtr node;
    char str[64];
}

%token <ival> INT_CONST
%token <str> IDENT
%token VOID INT
%token IF ELSE WHILE BREAK CONTINUE RETURN
%token LTE GTE EQ NEQ
%token AND OR


%start CompUnit

%type <expr> Exp
%type <node> CompUnit Unit Decl VarDecl VarDef InitVal FuncDef  FuncFParams FuncFParam
%type <node> Block BlockItem Stmt LVal PrimaryExp
%type <node> UnaryExp UnaryOp FuncRParams MulExp AddExp RelExp EqExp LAndExp LOrExp


%%
CompUnit : Unit
         | CompUnit Unit
         ;

Unit : Decl
     | FuncDef
     ;

Decl : VarDecl
     ;

VarDecl : INT VarDef ';'
        ;

VarDef : IDENT '=' InitVal
       | IDENT
       | IDENT VarDefunit
       | VarDef ',' VarDef
       ;

VarDefunit : '[' INT_CONST ']'
           | VarDefunit '[' INT_CONST ']'
           ;

InitVal : Exp
        ;

FuncDef : INT IDENT '(' FuncFParams ')' Block
        | VOID IDENT '(' FuncFParams ')' Block
        | INT IDENT '(' ')' Block
        | VOID IDENT '(' ')' Block
        ;

FuncFParams : FuncFParam
            | FuncFParams ',' FuncFParam
            ;

FuncFParam : INT IDENT
           | INT IDENT '[' ']'
           | INT IDENT '[' ']' FuncFParamUnit
           ;

FuncFParamUnit : '[' INT_CONST ']'
               | FuncFParamUnit '[' INT_CONST ']'
               ;

Block : '{' '}'
      | '{' BlockItem '}'
      ;

BlockItem : Decl
          | Stmt
          | BlockItem Decl
          | BlockItem Stmt
          ;

Stmt : LVal '=' Exp ';'
     | Exp ';'
     | Block
     | IF '(' Exp ')' Stmt
     | IF '(' Exp ')' Stmt ELSE Stmt
     | WHILE '(' Exp ')' Stmt
     | BREAK ';'
     | CONTINUE ';'
     | RETURN ';'
     | RETURN Exp ';'
     ;

Exp : LOrExp
    ;

LVal : IDENT
     | IDENT LValunit
     ;

LValunit : '[' Exp ']'
         | LValunit '[' Exp ']'
         ;

PrimaryExp : '(' Exp ')'
           | LVal
           | INT_CONST
           ;

UnaryExp : PrimaryExp
         | IDENT '(' ')'
         | IDENT '(' FuncRParams ')'
         | UnaryOp UnaryExp
         ;

UnaryOp : '+'
        | '-'
        | '!'
        ;

FuncRParams : Exp
            | FuncRParams ',' Exp
            ;

MulExp : UnaryExp
       | MulExp '*' UnaryExp
       | MulExp '/' UnaryExp
       | MulExp '%' UnaryExp
       ;

AddExp : MulExp
       | AddExp '+' MulExp
       | AddExp '-' MulExp
       ;

RelExp : AddExp
       | RelExp '<' AddExp
       | RelExp '>' AddExp
       | RelExp LTE AddExp
       | RelExp GTE AddExp
       ;

EqExp : RelExp
      | EqExp EQ RelExp
      | EqExp NEQ RelExp
      ;

LAndExp : EqExp
        | LAndExp AND EqExp
        ;

LOrExp : LAndExp
       | LOrExp OR LAndExp
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