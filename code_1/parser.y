%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.hpp"

void yyerror(const char *s);
int yylex();
extern int yylineno;

int debug = 0; // Set to 1 to enable debug prints, 0 to disable

%}

%union {
    char *str;
    int num;
    struct Node *node;
}

%token SELECT FROM WHERE JOIN INNER ON AND DOT IN
%token COUNT MAX MIN AVG
%token EQ LT GT COMMA SEMICOLON LPAREN RPAREN
%token <str> IDENTIFIER
%token <num> NUMBER

%type <node> query select_clause from_clause where_clause join_clause condition expr table_ref subquery
%type <str> column column_item

%%

query: select_clause SEMICOLON
    { 
        if (debug) printf("Parsed query: %s\n", $1->arg1);
        root = $1;
    }
    ;

select_clause: SELECT column FROM from_clause where_clause
    {
        $$ = new_node("π", $2, NULL);
        if ($5) {
            $5->child = $4;
            $$->child = $5;
        } else {
            $$->child = $4;
        }
    }
    ;

column: column_item
    { 
        if (debug) printf("Column: %s\n", $1);
        $$ = $1; 
    }
    | column COMMA column_item
    {
        char *combined = (char *)malloc(strlen($1) + strlen($3) + 2);
        sprintf(combined, "%s,%s", $1, $3);
        if (debug) printf("Combined columns: %s\n", combined);
        $$ = combined;
    }
    ;

column_item: IDENTIFIER
    { 
        if (debug) printf("Column item: %s\n", $1);
        $$ = $1; 
    }
    | IDENTIFIER DOT IDENTIFIER
    {
        char combined[100];
        sprintf(combined, "%s.%s", $1, $3);
        if (debug) printf("Column item with dot: %s\n", combined);
        $$ = strdup(combined);
    }
    | COUNT LPAREN column_item RPAREN
    {
        char agg[100];
        sprintf(agg, "COUNT(%s)", $3);
        if (debug) printf("Aggregate COUNT: %s\n", agg);
        $$ = strdup(agg);
    }
    | MAX LPAREN column_item RPAREN
    {
        char agg[100];
        sprintf(agg, "MAX(%s)", $3);
        if (debug) printf("Aggregate MAX: %s\n", agg);
        $$ = strdup(agg);
    }
    | MIN LPAREN column_item RPAREN
    {
        char agg[100];
        sprintf(agg, "MIN(%s)", $3);
        if (debug) printf("Aggregate MIN: %s\n", agg);
        $$ = strdup(agg);
    }
    | AVG LPAREN column_item RPAREN
    {
        char agg[100];
        sprintf(agg, "AVG(%s)", $3);
        if (debug) printf("Aggregate AVG: %s\n", agg);
        $$ = strdup(agg);
    }
    ;

from_clause: table_ref
    { 
        $$ = $1; 
    }
    | table_ref join_clause
    {
        $$ = $1;
        $$->child = $2->child;
        $2->child = NULL;
        $$->next = $2;
    }
    ;

table_ref: IDENTIFIER
    { 
        if (debug) printf("Table reference: %s\n", $1);
        $$ = new_node("table", $1, NULL); 
    }
    | IDENTIFIER IDENTIFIER
    {
        if (debug) printf("Table reference with alias: %s as %s\n", $1, $2);
        $$ = new_node("table", $1, $2); // arg1 = table name, arg2 = alias
    }
    ;

join_clause: JOIN table_ref ON condition
    {
        if (debug) printf("Join clause: %s\n", $2->arg1);
        $$ = new_node("⨝", $4->arg1, NULL);  // Join node with condition as argument
        $$->child = $2;              // Right table as child
    }
    ;

where_clause: WHERE condition
    { 
        if (debug) printf("Where clause: %s\n", $2->arg1);
        $$ = new_node("σ", $2->arg1, NULL); 
    }
    | /* empty */
    { 
        if (debug) printf("Empty where clause\n");
        $$ = NULL; 
    }
    ;

condition: expr EQ expr
    {
        char cond[100];
        sprintf(cond, "%s = %s", $1->arg1, $3->arg1);
        if (debug) printf("Condition: %s\n", cond);
        $$ = new_node("cond", strdup(cond), NULL);
    }
    | expr LT expr
    {
        char cond[100];
        sprintf(cond, "%s < %s", $1->arg1, $3->arg1);
        if (debug) printf("Condition: %s\n", cond);
        $$ = new_node("cond", strdup(cond), NULL);
    }
    | expr GT expr
    {
        char cond[100];
        sprintf(cond, "%s > %s", $1->arg1, $3->arg1);
        if (debug) printf("Condition: %s\n", cond);
        $$ = new_node("cond", strdup(cond), NULL);
    }
    | expr IN LPAREN subquery RPAREN
    {
        char cond[100];
        sprintf(cond, "%s IN (subquery)", $1->arg1);
        if (debug) printf("Condition with subquery: %s\n", cond);
        $$ = new_node("cond", strdup(cond), NULL);
        $$->child = $4; // Subquery as child
    }
    ;

subquery: select_clause
    { 
        if (debug) printf("Subquery\n");
        $$ = $1; 
    }
    ;

expr: IDENTIFIER
    { 
        if (debug) printf("Expression: %s\n", $1);
        $$ = new_node("expr", $1, NULL); 
    }
    | IDENTIFIER DOT IDENTIFIER
    {
        char combined[100];
        sprintf(combined, "%s.%s", $1, $3);
        if (debug) printf("Expression with dot: %s\n", combined);
        $$ = new_node("expr", strdup(combined), NULL);
    }
    | NUMBER
    {
        char num[20];
        sprintf(num, "%d", $1);
        if (debug) printf("Expression with number: %s\n", num);
        $$ = new_node("expr", strdup(num), NULL);
    }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error at line %d: %s\n", yylineno, s);
}