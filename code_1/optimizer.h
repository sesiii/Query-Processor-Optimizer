// // #ifndef OPTIMIZER_H
// // #define OPTIMIZER_H

// // #include "parser.h"

// // typedef struct JoinOrderNode {
// //     char *left_table;
// //     char *right_table;
// //     char *join_condition;
// //     double cost;
// //     struct JoinOrderNode *left;
// //     struct JoinOrderNode *right;
// // } JoinOrderNode;

// // // Main function to optimize a query plan
// // Node* optimize_query(Node *root);

// // // Helper functions
// // Node* push_down_selections(Node *node);
// // Node* push_down_projections(Node *node);
// // Node* reorder_joins(Node *node);

// // JoinOrderNode* find_optimal_join_order(Node *join_node);
// // double estimate_cost(Node *node);

// // // Print the execution plan
// // void print_execution_plan(Node *node);

// // #endif


// #ifndef OPTIMIZER_H
// #define OPTIMIZER_H

// #include "parser.h"

// typedef struct JoinOrderNode {
//     char *left_table;
//     char *right_table;
//     char *join_condition;
//     double cost;
//     struct JoinOrderNode *left;
//     struct JoinOrderNode *right;
// } JoinOrderNode;

// typedef struct CostMetrics {
//     int result_size;    // Estimated number of output tuples
//     double io_cost;     // Cost of data access (row scans)
//     double cpu_cost;    // Cost of processing (comparisons)
// } CostMetrics;

// // Main function to optimize a query plan
// Node* optimize_query(Node *root);

// // Helper functions
// Node* push_down_selections(Node *node);
// Node* push_down_projections(Node *node);
// Node* reorder_joins(Node *node);

// JoinOrderNode* find_optimal_join_order(Node *join_node);
// CostMetrics estimate_cost(Node *node);

// // Print the execution plan
// void print_execution_plan(Node *node);

// #endif


#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "parser.h"

typedef struct JoinOrderNode {
    char *left_table;
    char *right_table;
    char *join_condition;
    double cost;
    struct JoinOrderNode *left;
    struct JoinOrderNode *right;
} JoinOrderNode;

typedef struct CostMetrics {
    int result_size;   
    double io_cost;    
    double cpu_cost;   
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