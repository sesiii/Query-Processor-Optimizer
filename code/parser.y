%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

extern int yylex();
extern int yyparse();
extern FILE* yyin;
void yyerror(const char* s);

AstNode* ast_root = NULL;
%}

%union {
    int intval;
    char* strval;
    struct AstNode* node;
    struct ColumnList* col_list;
}

%token <strval> IDENTIFIER
%token <intval> NUMBER
%token SELECT FROM WHERE JOIN ON AND OR COMMA
%token EQ LT GT

%type <node> query select_clause from_clause where_clause condition
%type <node> table_ref join_clause
%type <col_list> column_list

%%
query: select_clause from_clause where_clause
    {
        $$ = create_query_node($1, $2, $3);
        ast_root = $$;
    }
    ;

select_clause: SELECT column_list
    {
        $$ = create_select_node($2);
    }
    ;

column_list: IDENTIFIER
    {
        $$ = create_column_list($1, NULL);
    }
    | IDENTIFIER COMMA column_list
    {
        $$ = create_column_list($1, $3);
    }
    ;

from_clause: FROM table_ref
    {
        $$ = create_from_node($2);
    }
    ;

table_ref: IDENTIFIER
    {
        $$ = create_table_node($1);
    }
    | table_ref JOIN IDENTIFIER ON condition
    {
        $$ = create_join_node($1, $3, $5);
    }
    ;

where_clause: /* empty */
    {
        $$ = NULL;
    }
    | WHERE condition
    {
        $$ = create_where_node($2);
    }
    ;

condition: IDENTIFIER EQ NUMBER
    {
        $$ = create_condition_node($1, "=", $3);
    }
    | IDENTIFIER EQ IDENTIFIER
    {
        $$ = create_eq_condition_node($1, $3);
    }
    | condition AND condition
    {
        $$ = create_and_condition_node($1, $3);
    }
    | condition OR condition
    {
        $$ = create_or_condition_node($1, $3);
    }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Parse error: %s\n", s);
    exit(1);
}