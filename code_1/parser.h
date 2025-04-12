#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char *operation;  // e.g., "π", "σ", "⨝", "table", "cond", "expr"
    char *arg1;       // Column, condition, or value
    char *arg2;       // Secondary argument (e.g., table name)
    struct Node *child;  // Child node (e.g., for WHERE or JOIN)
    struct Node *next;   // For sibling nodes (e.g., join’s left child)
} Node;

extern Node *root;

Node *new_node(char *op, char *arg1, char *arg2);
void print_tree(Node *node, int depth);

#endif