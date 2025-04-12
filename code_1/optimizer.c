#include "optimizer.h"
#include "stats.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Flag to enable/disable optimizations
int enable_selection_pushdown = 0;
int enable_projection_pushdown = 1;
int enable_join_reordering = 0;
int debugkaru = 0;

Node* optimize_query(Node *root) {
    if (!root) return NULL;
    
    printf("\nOptimizing query...\n");
    
    init_stats();
    
    printf("\nOriginal Execution Plan:\n");
    CostMetrics original_cost = estimate_cost(root);
    print_execution_plan(root, "Original Plan");
    
    Node *optimized_root = root;
    if (enable_selection_pushdown) {
        printf("Applying selection push-down...\n");
        optimized_root = push_down_selections(optimized_root);
    }

    if (enable_projection_pushdown) {
        printf("Applying projection push-down...\n");
        optimized_root = push_down_projections(optimized_root);
    }
    
    printf("\nOptimized Execution Plan:\n");
    CostMetrics optimized_cost = estimate_cost(optimized_root);
    print_execution_plan(optimized_root, "Optimized Plan");
    
    printf("\nCost Comparison:\n");
    printf("Metric          | Original      | Optimized     | Change\n");
    printf("----------------|---------------|---------------|----------------\n");
    printf("Result Size     | %-13d | %-13d | %s%d (%.1f%%)\n",
           original_cost.result_size, optimized_cost.result_size,
           (optimized_cost.result_size <= original_cost.result_size) ? "-" : "+",
           abs(optimized_cost.result_size - original_cost.result_size),
           original_cost.result_size ? 
           (double)abs(optimized_cost.result_size - original_cost.result_size) / original_cost.result_size * 100 : 0.0);
    printf("I/O Cost        | %-13.1f | %-13.1f | %s%.1f (%.1f%%)\n",
           original_cost.io_cost, optimized_cost.io_cost,
           (optimized_cost.io_cost <= original_cost.io_cost) ? "-" : "+",
           fabs(optimized_cost.io_cost - original_cost.io_cost),
           original_cost.io_cost ? 
           fabs(optimized_cost.io_cost - original_cost.io_cost) / original_cost.io_cost * 100 : 0.0);
    printf("CPU Cost        | %-13.1f | %-13.1f | %s%.1f (%.1f%%)\n",
           original_cost.cpu_cost, optimized_cost.cpu_cost,
           (optimized_cost.cpu_cost <= original_cost.cpu_cost) ? "-" : "+",
           fabs(optimized_cost.cpu_cost - original_cost.cpu_cost),
           original_cost.cpu_cost ? 
           fabs(optimized_cost.cpu_cost - original_cost.cpu_cost) / original_cost.cpu_cost * 100 : 0.0);
    
    return optimized_root;
}

void extract_table_column(const char *expr, char **table, char **column) {
    char *expr_copy = strdup(expr);
    char *start = expr_copy;
    while (isspace(*start)) start++;
    char *end = start + strlen(start) - 1;
    while (end > start && isspace(*end)) *end-- = '\0';
    
    char *dot = strchr(start, '.');
    if (dot) {
        int table_len = dot - start;
        *table = malloc(table_len + 1);
        strncpy(*table, start, table_len);
        (*table)[table_len] = '\0';
        *column = strdup(dot + 1);
    } else {
        *table = NULL;
        *column = strdup(start);
    }
    free(expr_copy);
}

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

    if (debugkaru) {
        printf("Checking if '%s' can push to '%s': table=%s, column=%s\n", 
               condition, table_name, table ? table : "NULL", column ? column : "NULL");
    }

    int result = 0;
    if (table && column) {
        result = strcmp(table, table_name) == 0 && get_column_stats(table_name, column) != NULL;
    } else if (column) {
        ColumnStats *stats = get_column_stats(table_name, column);
        if (stats) {
            result = 1;
            if (debugkaru) printf("push down possible: column %s exists in table %s\n", column, table_name);
        }
    }
    
    if (debugkaru) printf("Result: %d\n", result);
    if (table) free(table);
    if (column) free(column);
    if (op) free(op);
    return result;
}

Node* push_down_selections(Node *node) {
    if (!node) return NULL;
    if (debugkaru) printf("Pushing down selections...%s\n", node->operation ? node->operation : "NULL");
    
    if (node->operation && strcmp(node->operation, "σ") == 0) {
        Node *child = node->child;
        if (!child) return node;
        
        if (child->operation && strcmp(child->operation, "table") == 0 && 
            child->next && child->next->operation && strcmp(child->next->operation, "⨝") == 0) {
            
            Node *left_table = child;
            Node *join_node = child->next;
            Node *right_table = NULL;
            
            if (left_table->child && left_table->child->operation && 
                strcmp(left_table->child->operation, "table") == 0) {
                right_table = left_table->child;
            }
            
            if (right_table && can_push_to_table(node->arg1, right_table->arg1, node)) {
                if (debugkaru) printf("Pushing condition '%s' down to table '%s'\n", node->arg1, right_table->arg1);
                
                Node *new_selection = new_node("σ", strdup(node->arg1), NULL);
                new_selection->child = right_table;
                
                left_table->child = NULL;
                
                join_node->child = left_table;
                left_table->next = new_selection;
                
                node->child = NULL;
                free(node->operation);
                free(node->arg1);
                free(node);
                
                return join_node;
            } 
            else if (can_push_to_table(node->arg1, left_table->arg1, node)) {
                if (debugkaru) printf("Pushing condition '%s' down to table '%s'\n", node->arg1, left_table->arg1);
                
                Node *new_selection = new_node("σ", strdup(node->arg1), NULL);
                new_selection->child = left_table;
                
                left_table->child = NULL;
                
                join_node->child = new_selection;
                new_selection->next = right_table;
                
                node->child = NULL;
                free(node->operation);
                free(node->arg1);
                free(node);
                
                return join_node;
            }
        }
    }
    
    if (node->child) node->child = push_down_selections(node->child);
    if (node->next) node->next = push_down_selections(node->next);
    
    return node;
}

void parse_join_condition(const char *condition, char **left_table, char **left_col, 
                         char **right_table, char **right_col) {
    char condition_copy[100];
    strcpy(condition_copy, condition);
    
    char *left_side = strtok(condition_copy, "=");
    char *right_side = strtok(NULL, "=");
    
    if (left_side && right_side) {
        char *left_clean = left_side;
        char *right_clean = right_side;
        while (isspace(*left_clean)) left_clean++;
        while (isspace(*right_clean)) right_clean++;
        char *left_end = left_clean + strlen(left_clean) - 1;
        char *right_end = right_clean + strlen(right_clean) - 1;
        while (left_end > left_clean && isspace(*left_end)) *left_end-- = '\0';
        while (right_end > right_clean && isspace(*right_end)) *right_end-- = '\0';
        
        if (debugkaru) printf("Parsing join: left='%s', right='%s'\n", left_clean, right_clean);
        extract_table_column(left_clean, left_table, left_col);
        extract_table_column(right_clean, right_table, right_col);
    } else {
        *left_table = *left_col = *right_table = *right_col = NULL;
    }
}

double estimate_join_cost(const char *left_table, const char *right_table, const char *condition) {
    TableStats *left_stats = get_table_stats(left_table);
    TableStats *right_stats = get_table_stats(right_table);
    
    if (!left_stats || !right_stats) {
        if (debugkaru) printf("No stats for %s or %s\n", left_table, right_table);
        return 1000.0;
    }
    
    char *left_table_name, *left_col, *right_table_name, *right_col;
    parse_join_condition(condition, &left_table_name, &left_col, &right_table_name, &right_col);
    
    double selectivity = 1.0 / fmax(left_stats->row_count, right_stats->row_count);
    if (left_table_name && right_table_name && left_col && right_col) {
        double sel = calculate_join_selectivity(left_table_name, left_col, right_table_name, right_col);
        if (sel > 0.0 && sel < 1.0) {
            selectivity = sel;
            if (debugkaru) printf("Join selectivity for %s.%s = %s.%s: %f\n", 
                                 left_table_name, left_col, right_table_name, right_col, selectivity);
        }
    }
    
    double result_size = left_stats->row_count * right_stats->row_count * selectivity;
    
    if (left_table_name) free(left_table_name);
    if (left_col) free(left_col);
    if (right_table_name) free(right_table_name);
    if (right_col) free(right_col);
    
    return result_size;
}

JoinOrderNode* find_optimal_join_order(Node *join_node) {
    if (debugkaru) printf("Finding optimal join order (simplified implementation)\n");
    return NULL;
}

Node* reorder_joins(Node *node) {
    if (!node) return NULL;
    
    if (node->operation && strcmp(node->operation, "⨝") == 0) {
        if (debugkaru) printf("Join reordering would be applied here\n");
        if (node->child) node->child = reorder_joins(node->child);
        if (node->next) node->next = reorder_joins(node->next);
    } else {
        if (node->child) node->child = reorder_joins(node->child);
        if (node->next) node->next = reorder_joins(node->next);
    }
    
    return node;
}

// CostMetrics estimate_cost(Node *node) {
//     CostMetrics metrics = {0, 0.0, 0.0};
    
//     if (!node) return metrics;
    
//     if (strcmp(node->operation, "table") == 0) {
//         TableStats *stats = get_table_stats(node->arg1);
//         if (stats) {
//             metrics.result_size = stats->row_count;
//             metrics.io_cost = stats->row_count * 1.0;
//             metrics.cpu_cost = 0.0;
//         } else {
//             metrics.result_size = 1000;
//             metrics.io_cost = 1000.0;
//             metrics.cpu_cost = 0.0;
//         }
//         return metrics;
//     }
    
//     if (strcmp(node->operation, "σ") == 0) {
//         CostMetrics child_metrics = estimate_cost(node->child);
//         double selectivity = 0.1;
        
//         char *table = NULL, *column = NULL, *op = NULL;
//         int value = 0;
//         extract_condition_components(node->arg1, &table, &column, &op, &value);
        
//         if (table && column && op) {
//             selectivity = calculate_condition_selectivity(table, column, op, value);
//             if (debugkaru) printf("Selection %s.%s %s %d: selectivity=%f\n", 
//                                  table, column, op, value, selectivity);
//         } else if (column && op) {
//             Node *current = node->child;
//             while (current) {
//                 if (current->operation && strcmp(current->operation, "table") == 0) {
//                     if (get_column_stats(current->arg1, column)) {
//                         selectivity = calculate_condition_selectivity(current->arg1, column, op, value);
//                         if (debugkaru) printf("Selection %s.%s %s %d: selectivity=%f\n", 
//                                              current->arg1, column, op, value, selectivity);
//                         break;
//                     }
//                 }
//                 current = current->child ? current->child : current->next;
//             }
//         }
        
//         metrics.result_size = (int)(child_metrics.result_size * selectivity);
//         metrics.io_cost = child_metrics.io_cost;
//         metrics.cpu_cost = child_metrics.cpu_cost + child_metrics.result_size * 0.01;
        
//         if (table) free(table);
//         if (column) free(column);
//         if (op) free(op);
//         return metrics;
//     }
    
//     if (strcmp(node->operation, "⨝") == 0) {
//         Node *left_table = node->child;
//         Node *right_table = NULL;
        
//         if (left_table && left_table->next) {
//             right_table = left_table->next;
//         } else if (left_table && left_table->child && 
//                    left_table->child->operation && strcmp(left_table->child->operation, "table") == 0) {
//             right_table = left_table->child;
//             left_table = node->child;
//         }
        
//         if (!left_table || !right_table) {
//             if (debugkaru) printf("Join missing inputs: left=%p, right=%p\n", left_table, right_table);
//             return metrics;
//         }
        
//         CostMetrics left_metrics = estimate_cost(left_table);
//         CostMetrics right_metrics = estimate_cost(right_table);
        
//         const char *left_name = NULL;
//         if (left_table->operation && strcmp(left_table->operation, "table") == 0) {
//             left_name = left_table->arg1;
//         } else if (left_table->operation && strcmp(left_table->operation, "σ") == 0) {
//             Node *current = left_table->child;
//             while (current && current->operation && strcmp(current->operation, "table") != 0) {
//                 current = current->child;
//             }
//             left_name = current ? current->arg1 : NULL;
//         }
        
//         const char *right_name = NULL;
//         if (right_table->operation && strcmp(right_table->operation, "table") == 0) {
//             right_name = right_table->arg1;
//         } else if (right_table->operation && strcmp(right_table->operation, "σ") == 0) {
//             Node *current = right_table->child;
//             while (current && current->operation && strcmp(current->operation, "table") != 0) {
//                 current = current->child;
//             }
//             right_name = current ? current->arg1 : NULL;
//         }
        
//         double selectivity = 0.0001; // Default based on primary key
//         if (node->arg1 && left_name && right_name) {
//             char *left_table_name, *left_col, *right_table_name, *right_col;
//             parse_join_condition(node->arg1, &left_table_name, &left_col, &right_table_name, &right_col);
//             if (left_table_name && right_table_name && left_col && right_col) {
//                 double sel = calculate_join_selectivity(left_table_name, left_col, right_table_name, right_col);
//                 if (sel > 0.0 && sel < 1.0) {
//                     selectivity = sel;
//                 } else {
//                     TableStats *left_stats = get_table_stats(left_name);
//                     TableStats *right_stats = get_table_stats(right_name);
//                     selectivity = 1.0 / fmax(left_stats ? left_stats->row_count : 1000, 
//                                            right_stats ? right_stats->row_count : 1000);
//                     if (debugkaru) printf("Default selectivity for %s.%s = %s.%s: %f\n", 
//                                          left_table_name, left_col, right_table_name, right_col, selectivity);
//                 }
//             } else {
//                 if (debugkaru) printf("Failed to parse join condition: %s\n", node->arg1);
//             }
//             if (left_table_name) free(left_table_name);
//             if (left_col) free(left_col);
//             if (right_table_name) free(right_table_name);
//             if (right_col) free(right_col);
//         }
        
//         metrics.result_size = (int)(left_metrics.result_size * right_metrics.result_size * selectivity);
//         metrics.io_cost = left_metrics.io_cost + right_metrics.io_cost;
//         metrics.cpu_cost = left_metrics.cpu_cost + right_metrics.cpu_cost + 
//                           (left_metrics.result_size * right_metrics.result_size * 0.01);
        
//         if (debugkaru) {
//             printf("Join cost: size=%d, io=%.1f, cpu=%.1f (left_size=%d, right_size=%d, sel=%f)\n",
//                    metrics.result_size, metrics.io_cost, metrics.cpu_cost,
//                    left_metrics.result_size, right_metrics.result_size, selectivity);
//         }
        
//         return metrics;
//     }
    
//     if (strcmp(node->operation, "π") == 0) {
//         CostMetrics child_metrics = estimate_cost(node->child);
//         metrics.result_size = child_metrics.result_size;
//         metrics.io_cost = child_metrics.io_cost;
//         metrics.cpu_cost = child_metrics.cpu_cost + child_metrics.result_size * 0.005;
//         return metrics;
//     }
    
//     return metrics;
// }

// CostMetrics estimate_cost(Node *node) {
//     CostMetrics metrics = {0, 0.0, 0.0};
    
//     if (!node) return metrics;
    
//     if (strcmp(node->operation, "table") == 0) {
//         TableStats *stats = get_table_stats(node->arg1);
//         if (stats) {
//             metrics.result_size = stats->row_count;
//             // Base I/O cost for full table scan
//             metrics.io_cost = stats->row_count * 1.0;
//             // CPU cost proportional to row count
//             metrics.cpu_cost = stats->row_count * 0.1;
//         } else {
//             metrics.result_size = 1000;
//             metrics.io_cost = 1000.0;
//             metrics.cpu_cost = 100.0;
//         }
//         return metrics;
//     }
    
//     if (strcmp(node->operation, "σ") == 0) {
//         CostMetrics child_metrics = estimate_cost(node->child);
//         double selectivity = 0.1;
        
//         char *table = NULL, *column = NULL, *op = NULL;
//         int value = 0;
//         extract_condition_components(node->arg1, &table, &column, &op, &value);
        
//         if (table && column && op) {
//             selectivity = calculate_condition_selectivity(table, column, op, value);
//         } else if (column && op) {
//             Node *current = node->child;
//             while (current) {
//                 if (current->operation && strcmp(current->operation, "table") == 0) {
//                     if (get_column_stats(current->arg1, column)) {
//                         selectivity = calculate_condition_selectivity(current->arg1, column, op, value);
//                         break;
//                     }
//                 }
//                 current = current->child ? current->child : current->next;
//             }
//         }
        
//         metrics.result_size = (int)(child_metrics.result_size * selectivity);
//         // I/O cost remains same as child (selection doesn't add I/O)
//         metrics.io_cost = child_metrics.io_cost;
//         // CPU cost adds filtering cost (0.01 per row)
//         metrics.cpu_cost = child_metrics.cpu_cost + child_metrics.result_size * 0.01;
        
//         if (table) free(table);
//         if (column) free(column);
//         if (op) free(op);
//         return metrics;
//     }
    
//     if (strcmp(node->operation, "⨝") == 0) {
//         Node *left = node->child;
//         Node *right = left ? left->next : NULL;
        
//         if (!left || !right) {
//             return metrics;
//         }

//         CostMetrics left_metrics = estimate_cost(left);
//         CostMetrics right_metrics = estimate_cost(right);
        
//         double selectivity = 0.0001; // Default selectivity
        
//         if (node->arg1) {
//             char *left_table, *left_col, *right_table, *right_col;
//             parse_join_condition(node->arg1, &left_table, &left_col, &right_table, &right_col);
//             if (left_table && right_table && left_col && right_col) {
//                 selectivity = calculate_join_selectivity(left_table, left_col, right_table, right_col);
//             }
//             if (left_table) free(left_table);
//             if (left_col) free(left_col);
//             if (right_table) free(right_table);
//             if (right_col) free(right_col);
//         }
        
//         metrics.result_size = (int)(left_metrics.result_size * right_metrics.result_size * selectivity);
//         // I/O cost is sum of children's I/O
//         metrics.io_cost = left_metrics.io_cost + right_metrics.io_cost;
//         // CPU cost adds join processing (0.001 per potential pair)
//         metrics.cpu_cost = left_metrics.cpu_cost + right_metrics.cpu_cost + 
//                           (left_metrics.result_size * right_metrics.result_size * 0.001);
        
//         return metrics;
//     }
    
//     if (strcmp(node->operation, "π") == 0) {
//         CostMetrics child_metrics = estimate_cost(node->child);
//         metrics.result_size = child_metrics.result_size;
//         // I/O cost remains same as child (projection doesn't add I/O)
//         metrics.io_cost = child_metrics.io_cost;
//         // CPU cost adds projection cost (0.001 per row)
//         metrics.cpu_cost = child_metrics.cpu_cost + child_metrics.result_size * 0.001;
//         return metrics;
//     }
    
//     return metrics;
// }

CostMetrics estimate_cost(Node *node) {
    CostMetrics metrics = {0, 0.0, 0.0};
    
    if (!node) return metrics;
    
    if (strcmp(node->operation, "table") == 0) {
        TableStats *stats = get_table_stats(node->arg1);
        if (stats) {
            metrics.result_size = stats->row_count;
            // Base I/O cost - full table scan
            metrics.io_cost = stats->size_in_bytes / 1000.0; // Convert bytes to KB
            // CPU cost - reading all rows
            metrics.cpu_cost = stats->row_count * 0.001;
        } else {
            metrics.result_size = 1000;
            metrics.io_cost = 1000.0;
            metrics.cpu_cost = 1.0;
        }
        return metrics;
    }
    
    if (strcmp(node->operation, "σ") == 0) {
        CostMetrics child_metrics = estimate_cost(node->child);
        double selectivity = 0.1;
        
        char *table = NULL, *column = NULL, *op = NULL;
        int value = 0;
        extract_condition_components(node->arg1, &table, &column, &op, &value);
        
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
        
        metrics.result_size = (int)(child_metrics.result_size * selectivity);
        // I/O cost remains same as child
        metrics.io_cost = child_metrics.io_cost;
        // CPU cost adds filtering (0.0005 per input row)
        metrics.cpu_cost = child_metrics.cpu_cost + (child_metrics.result_size * 0.0005);
        
        if (table) free(table);
        if (column) free(column);
        if (op) free(op);
        return metrics;
    }
    
    if (strcmp(node->operation, "⨝") == 0) {
        Node *left = node->child;
        Node *right = left ? left->next : NULL;
        
        if (!left || !right) {
            return metrics;
        }

        CostMetrics left_metrics = estimate_cost(left);
        CostMetrics right_metrics = estimate_cost(right);
        
        double selectivity = 0.0001; // Default selectivity
        
        if (node->arg1) {
            char *left_table, *left_col, *right_table, *right_col;
            parse_join_condition(node->arg1, &left_table, &left_col, &right_table, &right_col);
            if (left_table && right_table && left_col && right_col) {
                selectivity = calculate_join_selectivity(left_table, left_col, right_table, right_col);
            }
            if (left_table) free(left_table);
            if (left_col) free(left_col);
            if (right_table) free(right_table);
            if (right_col) free(right_col);
        }
        
        metrics.result_size = (int)(left_metrics.result_size * right_metrics.result_size * selectivity);
        // I/O cost is sum of children
        metrics.io_cost = left_metrics.io_cost + right_metrics.io_cost;
        // CPU cost adds join processing (0.00001 per potential pair)
        metrics.cpu_cost = left_metrics.cpu_cost + right_metrics.cpu_cost + 
                          (left_metrics.result_size * right_metrics.result_size * 0.00001);
        
        return metrics;
    }
    
    if (strcmp(node->operation, "π") == 0) {
        CostMetrics child_metrics = estimate_cost(node->child);
        metrics.result_size = child_metrics.result_size;
        
        // Projection I/O cost depends on whether we're reading projected columns only
        if (node->child && node->child->operation && strcmp(node->child->operation, "table") == 0) {
            // If projecting directly from table, we can reduce I/O
            TableStats *stats = get_table_stats(node->child->arg1);
            if (stats) {
                // Estimate 50% I/O reduction for projection (since we're reading fewer columns)
                metrics.io_cost = stats->size_in_bytes / 1000.0 * 0.5;
            } else {
                metrics.io_cost = child_metrics.io_cost * 0.5;
            }
        } else {
            // For intermediate projections, no I/O savings
            metrics.io_cost = child_metrics.io_cost;
        }
        
        // CPU cost adds projection (0.0001 per row)
        metrics.cpu_cost = child_metrics.cpu_cost + (child_metrics.result_size * 0.0001);
        return metrics;
    }
    
    return metrics;
}

void print_execution_plan_recursive(Node *node, int depth) {
    if (!node) return;
    
    for (int i = 0; i < depth; i++) printf("  ");
    
    CostMetrics metrics = estimate_cost(node);
    
    if (strcmp(node->operation, "table") == 0) {
        printf("table(%s) [size=%d, io=%.1f, cpu=%.1f]\n", 
               node->arg1, metrics.result_size, metrics.io_cost, metrics.cpu_cost);
    }
    else if (strcmp(node->operation, "σ") == 0) {
        printf("σ(%s) [size=%d, io=%.1f, cpu=%.1f]\n", 
               node->arg1, metrics.result_size, metrics.io_cost, metrics.cpu_cost);
    }
    else if (strcmp(node->operation, "⨝") == 0) {
        printf("⨝(%s) [size=%d, io=%.1f, cpu=%.1f]\n", 
               node->arg1, metrics.result_size, metrics.io_cost, metrics.cpu_cost);
    }
    else if (strcmp(node->operation, "π") == 0) {
        printf("π(%s) [size=%d, io=%.1f, cpu=%.1f]\n", 
               node->arg1, metrics.result_size, metrics.io_cost, metrics.cpu_cost);
    }
    else {
        printf("%s %s %s [size=%d, io=%.1f, cpu=%.1f]\n", 
               node->operation, 
               node->arg1 ? node->arg1 : "", 
               node->arg2 ? node->arg2 : "", 
               metrics.result_size, metrics.io_cost, metrics.cpu_cost);
    }
    
    print_execution_plan_recursive(node->child, depth + 1);
    print_execution_plan_recursive(node->next, depth);
}

void print_execution_plan(Node *node, const char *title) {
    printf("--- %s ---\n", title);
    print_execution_plan_recursive(node, 0);
}

Node* push_down_projections(Node *node) {
    if (!node) {
        if (debugkaru) printf("\n[DEBUG] Node is NULL, returning NULL\n");
        return NULL;
    }
    
    if (debugkaru) printf("\n[DEBUG] ENTER push_down_projections for node type: %s, arg1: %s\n", 
           node->operation ? node->operation : "NULL", 
           node->arg1 ? node->arg1 : "NULL");
    
    if (node->operation && strcmp(node->operation, "π") == 0) {
        if (debugkaru) printf("\n[DEBUG] Found projection node with columns: %s\n", node->arg1);
        
        Node *child = node->child;
        if (!child) {
            if (debugkaru) printf("\n[DEBUG] Projection has no child, returning node\n");
            return node;
        }
        
        if (debugkaru) printf("\n[DEBUG] Child operation: %s\n", child->operation ? child->operation : "NULL");
        
        if (child->operation && strcmp(child->operation, "σ") == 0) {
            if (debugkaru) printf("\n[DEBUG] Case 1: Push projection through selection\n");
            
            if (debugkaru) printf("\n[DEBUG] Selection condition: %s\n", child->arg1 ? child->arg1 : "NULL");
            
            Node *new_projection = new_node("π", strdup(node->arg1), NULL);
            if (debugkaru) printf("\n[DEBUG] Created new projection with columns: %s\n", new_projection->arg1);
            
            new_projection->child = child->child;
            if (debugkaru) printf("\n[DEBUG] Connected new projection to selection's child\n");
            
            child->child = new_projection;
            if (debugkaru) printf("\n[DEBUG] Put new projection under selection\n");
            
            if (debugkaru) printf("\n[DEBUG] Returning selection node as new root\n");
            Node *result = child;
            node->child = NULL;
            free(node->operation);
            free(node->arg1);
            free(node);
            
            if (debugkaru) printf("\n[DEBUG] Recursively optimizing projection under selection\n");
            result->child = push_down_projections(result->child);
            
            if (debugkaru) printf("\n[DEBUG] EXIT push_down_projections with selection root\n");
            return result;
        }
        else if (child->operation && strcmp(child->operation, "table") == 0 && 
                 child->next && child->next->operation && strcmp(child->next->operation, "⨝") == 0) {
            
            if (debugkaru) printf("\n[DEBUG] Case 2: Push projection through join\n");
            
            Node *left_table = child;
            Node *join_node = child->next;
            
            if (debugkaru) printf("\n[DEBUG] Left table: %s\n", left_table->arg1);
            if (debugkaru) printf("\n[DEBUG] Join condition: %s\n", join_node->arg1);
            
            Node *right_table = NULL;
            if (left_table->child && left_table->child->operation && 
                strcmp(left_table->child->operation, "table") == 0) {
                right_table = left_table->child;
                if (debugkaru) printf("\n[DEBUG] Found right table as child of left table: %s\n", right_table->arg1);
            } else {
                if (debugkaru) printf("\n[DEBUG] Could not find right table, returning original node\n");
                if (debugkaru) printf("\n[DEBUG] EXIT push_down_projections with original node\n");
                return node;
            }
            
            if (debugkaru) printf("\n[DEBUG] Parsing projection columns: %s\n", node->arg1);
            char *columns = strdup(node->arg1);
            char *column_list[100];
            int column_count = 0;
            
            char *token = strtok(columns, ",");
            while (token) {
                while (*token == ' ') token++;
                column_list[column_count] = strdup(token);
                if (debugkaru) printf("\n[DEBUG] Column %d: %s\n", column_count, column_list[column_count]);
                column_count++;
                token = strtok(NULL, ",");
            }
            
            if (debugkaru) printf("\n[DEBUG] Parsing join condition: %s\n", join_node->arg1);
            char *left_table_name = NULL, *left_col = NULL;
            char *right_table_name = NULL, *right_col = NULL;
            parse_join_condition(join_node->arg1, &left_table_name, &left_col, 
                               &right_table_name, &right_col);
            
            if (debugkaru) printf("\n[DEBUG] Join parsed: %s.%s = %s.%s\n", 
                   left_table_name ? left_table_name : "NULL", 
                   left_col ? left_col : "NULL", 
                   right_table_name ? right_table_name : "NULL", 
                   right_col ? right_col : "NULL");
            
            char left_columns[512] = "";
            char right_columns[512] = "";
            int left_has_columns = 0;
            int right_has_columns = 0;
            
            if (debugkaru) printf("\n[DEBUG] Sorting columns by table\n");
            for (int i = 0; i < column_count; i++) {
                char *table = NULL, *column = NULL;
                extract_table_column(column_list[i], &table, &column);
                
                if (debugkaru) printf("\n[DEBUG] Column %d: table=%s, column=%s\n", 
                       i, table ? table : "NULL", column ? column : "NULL");
                
                if (table) {
                    if (strcmp(table, left_table->arg1) == 0) {
                        if (left_has_columns) strcat(left_columns, ",");
                        strcat(left_columns, column_list[i]);
                        left_has_columns = 1;
                        if (debugkaru) printf("\n[DEBUG] Added to left columns: %s\n", column_list[i]);
                    }
                    else if (strcmp(table, right_table->arg1) == 0) {
                        if (right_has_columns) strcat(right_columns, ",");
                        strcat(right_columns, column_list[i]);
                        right_has_columns = 1;
                        if (debugkaru) printf("\n[DEBUG] Added to right columns: %s\n", column_list[i]);
                    }
                }
                
                if (table) free(table);
                if (column) free(column);
            }
            
            if (left_table_name && left_col) {
                char full_col[100];
                sprintf(full_col, "%s.%s", left_table_name, left_col);
                if (!strstr(left_columns, full_col)) {
                    if (left_has_columns) strcat(left_columns, ",");
                    strcat(left_columns, full_col);
                    left_has_columns = 1;
                    if (debugkaru) printf("\n[DEBUG] Added join column to left: %s\n", full_col);
                }
            }
            
            if (right_table_name && right_col) {
                char full_col[100];
                sprintf(full_col, "%s.%s", right_table_name, right_col);
                if (!strstr(right_columns, full_col)) {
                    if (right_has_columns) strcat(right_columns, ",");
                    strcat(right_columns, full_col);
                    right_has_columns = 1;
                    if (debugkaru) printf("\n[DEBUG] Added join column to right: %s\n", full_col);
                }
            }
            
            if (debugkaru) printf("\n[DEBUG] Final left columns: %s\n", left_columns);
            if (debugkaru) printf("\n[DEBUG] Final right columns: %s\n", right_columns);
            
            Node *new_left_table = new_node("table", strdup(left_table->arg1), NULL);
            Node *new_right_table = new_node("table", strdup(right_table->arg1), NULL);
            
            Node *left_projection = NULL;
            if (left_has_columns) {
                left_projection = new_node("π", strdup(left_columns), NULL);
                left_projection->child = new_left_table;
                if (debugkaru) printf("\n[DEBUG] Created left projection: π(%s)\n", left_columns);
            } else {
                left_projection = new_left_table;
                if (debugkaru) printf("\n[DEBUG] No left projection needed\n");
            }
            
            Node *right_projection = NULL;
            if (right_has_columns) {
                right_projection = new_node("π", strdup(right_columns), NULL);
                right_projection->child = new_right_table;
                if (debugkaru) printf("\n[DEBUG] Created right projection: π(%s)\n", right_columns);
            } else {
                right_projection = new_right_table;
                if (debugkaru) printf("\n[DEBUG] No right projection needed\n");
            }
            
            Node *new_join = new_node("⨝", strdup(join_node->arg1), NULL);
            
            if (debugkaru) printf("\n[DEBUG] Rebuilding tree with clean structure\n");
            
            new_join->child = left_projection;
            left_projection->next = right_projection;
            
            Node *top_projection = new_node("π", strdup(node->arg1), NULL);
            top_projection->child = new_join;
            
            for (int i = 0; i < column_count; i++) {
                free(column_list[i]);
            }
            free(columns);
            if (left_table_name) free(left_table_name);
            if (left_col) free(left_col); 
            if (right_table_name) free(right_table_name);
            if (right_col) free(right_col);
            
            node->child = NULL;
            free(node->operation);
            free(node->arg1);
            free(node);
            
            if (debugkaru) printf("\n[DEBUG] Tree rebuilt with clean structure, returning new top projection\n");
            if (debugkaru) printf("\n[DEBUG] EXIT push_down_projections with rebuilt tree\n");
            return top_projection;
        }
    }
    
    if (node->child) {
        if (debugkaru) printf("\n[DEBUG] Recursively processing child of %s\n", node->operation);
        node->child = push_down_projections(node->child);
    }
    if (node->next) {
        if (debugkaru) printf("\n[DEBUG] Recursively processing next of %s\n", node->operation);
        node->next = push_down_projections(node->next);
    }
    
    if (debugkaru) printf("\n[DEBUG] EXIT push_down_projections returning node type: %s\n", 
           node->operation ? node->operation : "NULL");
    return node;
}

void print_ast(Node *node, int depth) {
    if (!node) return;
    
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    
    if (node->operation) {
        if (node->arg1) {
            printf("%s(%s)\n", node->operation, node->arg1);
        } else {
            printf("%s\n", node->operation);
        }
    } else {
        printf("NULL_NODE\n");
    }
    
    if (node->child) {
        print_ast(node->child, depth + 1);
    }
    
    if (node->next) {
        print_ast(node->next, depth);
    }
}