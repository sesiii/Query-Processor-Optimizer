#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

ColumnList* create_column_list(char* name, ColumnList* next) {
    ColumnList* list = (ColumnList*)malloc(sizeof(ColumnList));
    list->name = strdup(name);
    list->next = next;
    return list;
}

AstNode* create_select_node(ColumnList* columns) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    node->type = NODE_SELECT;
    node->data.select.columns = columns;
    return node;
}

AstNode* create_from_node(AstNode* table) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    node->type = NODE_FROM;
    node->data.from.table = table;
    return node;
}

AstNode* create_table_node(char* name) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    node->type = NODE_TABLE;
    node->data.table.name = strdup(name);
    return node;
}

AstNode* create_join_node(AstNode* left, char* right, AstNode* condition) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    node->type = NODE_JOIN;
    node->data.join.left = left;
    node->data.join.right_table = strdup(right);
    node->data.join.condition = condition;
    return node;
}

AstNode* create_where_node(AstNode* condition) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    node->type = NODE_WHERE;
    node->data.where.condition = condition;
    return node;
}

AstNode* create_condition_node(char* left, char* op, int right) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    node->type = NODE_CONDITION;
    node->data.condition.left = strdup(left);
    node->data.condition.op = strdup(op);
    node->data.condition.right_val = right;
    node->data.condition.right_id = NULL;
    node->data.condition.left_cond = NULL;
    node->data.condition.right_cond = NULL;
    return node;
}

AstNode* create_eq_condition_node(char* left, char* right) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    node->type = NODE_CONDITION;
    node->data.condition.left = strdup(left);
    node->data.condition.op = strdup("=");
    node->data.condition.right_id = strdup(right);
    node->data.condition.left_cond = NULL;
    node->data.condition.right_cond = NULL;
    return node;
}

AstNode* create_and_condition_node(AstNode* left, AstNode* right) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    node->type = NODE_CONDITION;
    node->data.condition.left = strdup("AND");
    node->data.condition.left_cond = left;
    node->data.condition.right_cond = right;
    return node;
}

AstNode* create_or_condition_node(AstNode* left, AstNode* right) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    node->type = NODE_CONDITION;
    node->data.condition.left = strdup("OR");
    node->data.condition.left_cond = left;
    node->data.condition.right_cond = right;
    return node;
}

AstNode* create_query_node(AstNode* select, AstNode* from, AstNode* where) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    node->type = NODE_QUERY;
    node->data.query.select = select;
    node->data.query.from = from;
    node->data.query.where = where;
    return node;
}