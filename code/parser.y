%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

void yyerror(const char *s);
int yylex();
extern int yylineno;

%}

%union {
    char *str;
    int num;
    struct Node *node;
}

%token SELECT FROM WHERE JOIN INNER ON AND DOT
%token EQ LT GT COMMA SEMICOLON LPAREN RPAREN
%token <str> IDENTIFIER
%token <num> NUMBER

%type <node> query select_clause from_clause where_clause join_clause condition expr table_ref
%type <str> column

%%

query: select_clause SEMICOLON
    { 
        root = $1;
    }
    ;

select_clause: SELECT column FROM from_clause where_clause
    {
        $$ = new_node("π", $2, NULL);  // Projection node
        if ($5) {                      // If WHERE exists
            $5->child = $4;            // σ’s child is the from_clause
            $$->child = $5;            // π’s child is σ
        } else {
            $$->child = $4;            // π’s child is from_clause directly
        }
    }
    ;

from_clause: table_ref
    { $$ = $1; }
    | table_ref join_clause
    {
        //$$ = $2;
        //$$->child = $1;  // Table_ref as left child of join
        $$=$1;
        $$->child=$2->child;
        $2->child=NULL;
        $$->next=$2;
    }
    ;

table_ref: IDENTIFIER
    
    { printf("%s\n",$1);
    $$ = new_node("table", $1, NULL); }
    ;

join_clause: JOIN table_ref ON condition
    {
        $$ = new_node("⨝", $4->arg1, NULL);  // Join node with condition as argument
        $$->child = $2;// Right table as child
                          
    }
    ;

where_clause: WHERE condition
    { $$ = new_node("σ", $2->arg1, NULL); }
    | /* empty */
    { $$ = NULL; }
    ;

condition: expr EQ expr
    {
        char cond[100];
        sprintf(cond, "%s = %s", $1->arg1, $3->arg1);
        $$ = new_node("cond", strdup(cond), NULL);
    }
    | expr LT expr
    {
        char cond[100];
        sprintf(cond, "%s < %s", $1->arg1, $3->arg1);
        $$ = new_node("cond", strdup(cond), NULL);
    }
    | expr GT expr
    {
        char cond[100];
        sprintf(cond, "%s > %s", $1->arg1, $3->arg1);
        $$ = new_node("cond", strdup(cond), NULL);
    }
    ;

expr: IDENTIFIER
    { $$ = new_node("expr", $1, NULL); }
    | IDENTIFIER DOT IDENTIFIER
    {
        char combined[100];
        sprintf(combined, "%s.%s", $1, $3);
        $$ = new_node("expr", strdup(combined), NULL);
    }
    | NUMBER
    {
        char num[20];
        sprintf(num, "%d", $1);
        $$ = new_node("expr", strdup(num), NULL);
    }
    

column: IDENTIFIER { $$ = $1; }
    | column COMMA IDENTIFIER { 
        char *combined = malloc(strlen($1) + strlen($3) + 2);
        sprintf(combined, "%s,%s", $1, $3);
        $$ = combined;
    }

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error at line %d: %s\n", yylineno, s);
}