#include "optimizer.h"
#include "stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Flag to enable/disable optimizations
int enable_selection_pushdown = 1;
int enable_join_reordering = 1;

// Main optimization function
Node* optimize_query(Node *root) {
    if (!root) return NULL;
    
    printf("\nOptimizing query...\n");
    
    init_stats();
    
    if (enable_selection_pushdown) {
        printf("Applying selection push-down...\n");
        root = push_down_selections(root);
    }
    
    // if (enable_join_reordering) {
        // printf("Applying join reordering...\n");
        // root = reorder_joins(root);
    // }
    
    printf("\nOptimized Execution Plan:\n");
    print_execution_plan(root);
    
    return root;
}

// Helper function to extract table name and column from an expression
void extract_table_column(const char *expr, char **table, char **column) {
    char *dot = strchr(expr, '.');
    if (dot) {
        int table_len = dot - expr;
        *table = malloc(table_len + 1);
        strncpy(*table, expr, table_len);
        (*table)[table_len] = '\0';
        *column = strdup(dot + 1);
    } else {
        *table = NULL;
        *column = strdup(expr);
    }
}

// Helper function to extract table, column, operator, and value from a condition
void extract_condition_components(const char *condition, char **table, char **column, char **op, int *value) {
    char condition_copy[100];
    strcpy(condition_copy, condition);
    
    char *token = strtok(condition_copy, " ");
    if (token) {
        extract_table_column(token, table, column);
        token = strtok(NULL, " ");
        if (token) {
            *op = strdup(token);
            token = strtok(NULL, " ");
            if (token) {
                *value = atoi(token);
            } else {
                *value = 0;
            }
        } else {
            *op = NULL;
            *value = 0;
        }
    } else {
        *table = *column = *op = NULL;
        *value = 0;
    }
}


int can_push_to_table(const char *condition, const char *table_name, Node *context) {
    char *table = NULL, *column = NULL, *op = NULL;
    int value = 0;
    extract_condition_components(condition, &table, &column, &op, &value);

    printf("Checking if '%s' can push to '%s': table=%s, column=%s\n", 
           condition, table_name, table ? table : "NULL", column ? column : "NULL");

    int result = 0;
    if (table && column) {
        // Only allow push if table in condition matches table_name
        result = strcmp(table, table_name) == 0 && get_column_stats(table_name, column) != NULL;
    } else if (column) {
        // If table name is not specified, check if column exists in given table
        ColumnStats *stats = get_column_stats(table_name, column);
        if (stats) {
            result = 1;
            printf("push down possible: column %s exists in table %s\n", column, table_name);
        }
    }
    
    printf("Result: %d\n", result);
    if (table) free(table);
    if (column) free(column);
    if (op) free(op);
    return result;
}


// // Recursive function to push selection predicates down the tree
// Node* push_down_selections(Node *node) {
//     if (!node) return NULL;
//     printf("Pushing down selections...%s\n", node->operation);

//     if (node->operation && strcmp(node->operation, "σ") == 0) {
//         Node *child = node->child;
//         if (!child) return node;

//         // Check if child is a table with a join sibling
//         if (child->operation && strcmp(child->operation, "table") == 0 && 
//             child->next && child->next->operation && strcmp(child->next->operation, "⨝") == 0) {
//             Node *left_table = child;           // employees
//             Node *join_node = child->next;      // ⨝
//             Node *right_table = join_node->child; // salaries

//             if (right_table && can_push_to_table(node->arg1, right_table->arg1, node)) {
//                 printf("Pushing condition '%s' down to table '%s'\n", node->arg1, right_table->arg1);
//                 Node *new_selection = new_node("σ", node->arg1, NULL);
//                 new_selection->child = right_table;
//                 join_node->child = new_selection;
//                 left_table->next = join_node;
//                 free(node->operation);
//                 free(node->arg1);
//                 free(node);
//                 return left_table;
//             } else if (left_table && can_push_to_table(node->arg1, left_table->arg1, node)) {
//                 printf("Pushing condition '%s' down to table '%s'\n", node->arg1, left_table->arg1);
//                 Node *new_selection = new_node("σ", node->arg1, NULL);
//                 new_selection->child = left_table;
//                 new_selection->next = join_node;
//                 free(node->operation);
//                 free(node->arg1);
//                 free(node);
//                 return new_selection;
//             }
//         }
//     }
    
//     if (node->child) node->child = push_down_selections(node->child);
//     if (node->next) node->next = push_down_selections(node->next);
    
//     return node;
// }

Node* push_down_selections(Node *node) {
    if (!node) return NULL;
    printf("Pushing down selections...%s\n", node->operation);

    if (node->operation && strcmp(node->operation, "σ") == 0) {
        Node *child = node->child;
        if (!child) return node;

        // Check if child is a table with a join sibling
        if (child->operation && strcmp(child->operation, "table") == 0 && 
            child->next && child->next->operation && strcmp(child->next->operation, "⨝") == 0) {
            
            Node *left_table = child;           // employees
            Node *join_node = child->next;      // ⨝
            Node *right_table = NULL;
            
            // Find the right table - it could be a child of the left table
            if (left_table->child && left_table->child->operation && 
                strcmp(left_table->child->operation, "table") == 0) {
                right_table = left_table->child;  // salaries
            } else if (join_node->child && join_node->child->operation && 
                       strcmp(join_node->child->operation, "table") == 0) {
                right_table = join_node->child;   // alternative structure
            }

            // Try to push to right table first
            if (right_table && can_push_to_table(node->arg1, right_table->arg1, node)) {
                printf("Pushing condition '%s' down to table '%s'\n", node->arg1, right_table->arg1);
                
                // Create a new selection node for the right table
                Node *new_selection = new_node("σ", node->arg1, NULL);
                
                // If right table is child of left table
                if (left_table->child == right_table) {
                    new_selection->child = right_table;
                    left_table->child = new_selection;
                } 
                // If right table is child of join node
                else if (join_node->child == right_table) {
                    new_selection->child = right_table;
                    join_node->child = new_selection;
                }
                
                // Replace the original selection node with just the left table
                left_table->next = join_node;
                free(node->operation);
                free(node->arg1);
                free(node);
                return left_table;
            } 
            // Try to push to left table
            else if (can_push_to_table(node->arg1, left_table->arg1, node)) {
                printf("Pushing condition '%s' down to table '%s'\n", node->arg1, left_table->arg1);
                Node *new_selection = new_node("σ", node->arg1, NULL);
                new_selection->child = left_table;
                new_selection->next = join_node;
                free(node->operation);
                free(node->arg1);
                free(node);
                return new_selection;
            }
        }
    }
    
    if (node->child) node->child = push_down_selections(node->child);
    if (node->next) node->next = push_down_selections(node->next);
    
    return node;
}

// Parse a join condition to extract tables and columns
void parse_join_condition(const char *condition, char **left_table, char **left_col, 
                         char **right_table, char **right_col) {
    char condition_copy[100];
    strcpy(condition_copy, condition);
    
    char *left_side = strtok(condition_copy, "=");
    char *right_side = strtok(NULL, "=");
    
    if (left_side && right_side) {
        while (*left_side == ' ') left_side++;
        while (*right_side == ' ') right_side++;
        
        extract_table_column(left_side, left_table, left_col);
        extract_table_column(right_side, right_table, right_col);
    } else {
        *left_table = *left_col = *right_table = *right_col = NULL;
    }
}

// Estimate the cost of a join operation
double estimate_join_cost(const char *left_table, const char *right_table, const char *condition) {
    TableStats *left_stats = get_table_stats(left_table);
    TableStats *right_stats = get_table_stats(right_table);
    
    if (!left_stats || !right_stats) return 1000000;
    
    char *left_table_name, *left_col, *right_table_name, *right_col;
    parse_join_condition(condition, &left_table_name, &left_col, &right_table_name, &right_col);
    
    double selectivity = 1.0;
    if (left_table_name && right_table_name) {
        selectivity = calculate_join_selectivity(left_table_name, left_col, right_table_name, right_col);
    }
    
    double result_size = left_stats->row_count * right_stats->row_count * selectivity;
    
    if (left_table_name) free(left_table_name);
    if (left_col) free(left_col);
    if (right_table_name) free(right_table_name);
    if (right_col) free(right_col);
    
    return result_size;
}

// Find the optimal join order for a series of joins
JoinOrderNode* find_optimal_join_order(Node *join_node) {
    printf("Finding optimal join order (simplified implementation)\n");
    return NULL;
}

// Reorder joins based on cost estimates
Node* reorder_joins(Node *node) {
    if (!node) return NULL;
    
    if (node->operation && strcmp(node->operation, "⨝") == 0) {
        printf("Join reordering would be applied here\n");
        if (node->child) node->child = reorder_joins(node->child);
        if (node->next) node->next = reorder_joins(node->next);
    } else {
        if (node->child) node->child = reorder_joins(node->child);
        if (node->next) node->next = reorder_joins(node->next);
    }
    
    return node;
}

// Estimate the cost of a node
double estimate_cost(Node *node) {
    if (!node) return 0;
    
    if (strcmp(node->operation, "⨝") == 0) {
        Node *left_table = node->child;
        Node *right_table = node->next;
        
        if (!left_table || !right_table) return 0;
        
        double left_cost = estimate_cost(left_table);
        double right_cost = estimate_cost(right_table);
        
        const char *left_name = left_table->operation && strcmp(left_table->operation, "table") == 0 
                              ? left_table->arg1 : "unknown";
        const char *right_name = right_table->operation && strcmp(right_table->operation, "table") == 0 
                               ? right_table->arg1 : "unknown";
        
        TableStats *left_stats = get_table_stats(left_name);
        TableStats *right_stats = get_table_stats(right_name);
        
        if (!left_stats || !right_stats) return left_cost + right_cost + 100000;
        
        double selectivity = node->arg1 ? estimate_join_cost(left_name, right_name, node->arg1) : 0.1;
        return left_cost + right_cost + (left_stats->row_count * right_stats->row_count * selectivity);
    }
    
    if (strcmp(node->operation, "table") == 0) {
        TableStats *stats = get_table_stats(node->arg1);
        return stats ? stats->row_count : 1000;
    } 
    else if (strcmp(node->operation, "σ") == 0) {
        double child_cost = estimate_cost(node->child);
        char *table = NULL, *column = NULL, *op = NULL;
        int value = 0;
        extract_condition_components(node->arg1, &table, &column, &op, &value);
        
        double selectivity = 0.5; // Default
        if (table && column && op) {
            selectivity = calculate_condition_selectivity(table, column, op, value);
        } else if (column && op) {
            Node *current = node->child;
            while (current) {
                if (current->operation && strcmp(current->operation, "table") == 0) {
                    if (get_column_stats(current->arg1, column)) {
                        selectivity = calculate_condition_selectivity(current->arg1, column, op, value);
                        break;
                    }
                }
                current = current->child ? current->child : current->next;
            }
        }
        if (table) free(table);
        if (column) free(column);
        if (op) free(op);
        return child_cost * selectivity + 10; // CPU cost
    }
    else if (strcmp(node->operation, "π") == 0) {
        double child_cost = estimate_cost(node->child);
        return child_cost + 5; // CPU cost
    }
    
    return 0;
}

void print_execution_plan_recursive(Node *node, int depth) {
    if (!node) return;
    
    for (int i = 0; i < depth; i++) printf("  ");
    
    if (strcmp(node->operation, "table") == 0) {
        if (node->arg2)
            printf("table(%s AS %s (cost: %.2f))\n", node->arg1, node->arg2, estimate_cost(node));
        else
            printf("table(%s (cost: %.2f))\n", node->arg1, estimate_cost(node));
    }
    else if (strcmp(node->operation, "σ") == 0) {
        printf("σ %s (cost: %.2f)\n", node->arg1, estimate_cost(node));
    }
    else if (strcmp(node->operation, "⨝") == 0) {
        printf("⨝ %s (cost: %.2f)\n", node->arg1, estimate_cost(node));
    }
    else if (strcmp(node->operation, "π") == 0) {
        printf("π %s (cost: %.2f)\n", node->arg1, estimate_cost(node));
    }
    else {
        printf("%s %s %s (cost: %.2f)\n", 
               node->operation, 
               node->arg1 ? node->arg1 : "", 
               node->arg2 ? node->arg2 : "", 
               estimate_cost(node));
    }
    
    print_execution_plan_recursive(node->child, depth + 1);
    print_execution_plan_recursive(node->next, depth);
}

void print_execution_plan(Node *node) {
    print_execution_plan_recursive(node, 0);
}