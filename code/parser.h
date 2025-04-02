#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *column;
    char *table;
    char *condition_col;
    int condition_val;
} Query;

extern Query *query;

typedef struct Node {
    char *operation;
    char *arg1;
    char *arg2;
    struct Node *child;
} Node;

Node *build_relational_algebra(Query *q);
void print_tree(Node *node, int depth);

#endif