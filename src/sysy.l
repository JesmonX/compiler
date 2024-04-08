%option noinput
%option nounput
%option noyywrap

%{
#include <ast/ast.h>
#include "sysy.tab.hh"
%}

dec         [1-9][0-9]*
blank       [ \t\n]
oct         0[0-7]*   
hex         0(x|X)[0-9a-fA-F]*
ident       [_a-zA-Z][_a-zA-Z0-9]*
comment     \/\/(.)* 
comments    \/\*([^*]|\*+[^*/])*\*+\/


%%


"+"             { return '+'; }
"-"             { return '-'; }
"*"             { return '*'; }
"/"             { return '/'; }
"%"             { return '%'; }
{blank}         { }
"int"           { return INT; }
"void"          { return VOID; }
"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"break"         { return BREAK; }
"continue"      { return CONTINUE; }
"return"        { return RETURN; }
"<"             { return '<'; }
"<="            { return LTE; }
">"             { return '>'; }
">="            { return GTE; }
"=="            { return EQ; }
"!="            { return NEQ; }
"="             { return '='; }
"&&"            { return AND; }
"||"            { return OR; }
"!"             { return '!'; }
"{"             { return '{'; }
"}"             { return '}'; }
"("             { return '('; }
")"             { return ')'; }
"["             { return '['; }
"]"             { return ']'; }
","             { return ','; }
":"             { return ':'; }
";"             { return ';'; }
{comment}       { }
{comments}      { }
{ident}         { yylval.str_val = new std::string(yytext); return IDENT; }


{dec}           { yylval.ival = atoi(yytext); return INT_CONST; }
{oct}           { yylval.ival = atoi(yytext); return INT_CONST; }
{hex}           { yylval.ival = atoi(yytext); return INT_CONST; }

.               { printf("ERROR(%s)\n", yytext); }

%%
