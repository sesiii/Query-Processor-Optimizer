
#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "parser.hpp"

typedef struct JoinOrderNode {
    char *left_table;
    char *right_table;
    char *join_condition;
    double cost;
    struct JoinOrderNode *left;
    struct JoinOrderNode *right;
} JoinOrderNode;

typedef struct CostMetrics {
    int result_size;    // Estimated number of rows
    int num_columns;    // Number of columns
    double cost;        // Total cost = rows × columns
} CostMetrics;


// Main function to optimize a query plan
Node* optimize_query(Node *root);

// Helper functions
Node* push_down_selections(Node *node);
Node* push_down_projections(Node *node);
Node* reorder_joins(Node *node);

JoinOrderNode* find_optimal_join_order(Node *join_node);
CostMetrics estimate_cost(Node *node);

// Print the execution plan
void print_execution_plan(Node *node, const char *title);

#endif