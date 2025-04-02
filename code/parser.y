%{
#include "parser.h"

void yyerror(const char *s);
int yylex();

Query *new_query(char *col, char *tbl, char *cond_col, int cond_val) {
    Query *q = malloc(sizeof(Query));
    q->column = col;
    q->table = tbl;
    q->condition_col = cond_col;
    q->condition_val = cond_val;
    return q;
}

%}

%union {
    char *str;
    int num;
}

%token SELECT FROM WHERE GT SEMICOLON
%token <str> IDENTIFIER
%token <num> NUMBER

%type <str> column table condition_col
%type <num> condition_val

%%

query: SELECT column FROM table where_clause SEMICOLON
    ;

where_clause: WHERE condition_col GT condition_val
    {
        query = new_query($<str>-4, $<str>-3, $2, $4);  // column, table from earlier
    }
    | /* empty */
    {
        query = new_query($<str>-2, $<str>-1, NULL, 0);  // No condition
    }
    ;

column: IDENTIFIER { $$ = $1; }
table: IDENTIFIER { $$ = $1; }
condition_col: IDENTIFIER { $$ = $1; }
condition_val: NUMBER { $$ = $1; }

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}