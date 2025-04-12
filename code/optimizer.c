#include "optimizer.h"
#include "stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Flag to enable/disable optimizations
int enable_selection_pushdown = 1;
int enable_projection_pushdown = 1;
int enable_join_reordering = 0;
int debugkaru =0;

Node* optimize_query(Node *root) {
    if (!root) return NULL;
    
    printf("\nOptimizing query...\n");
    
    // Print initial AST
    if(debugkaru) printf("\n[DEBUG] Full AST before optimization:\n");
    // print_ast(root, 0);
    
    init_stats();
    
    if (enable_selection_pushdown) {
        printf("Applying selection push-down...\n");
        root = push_down_selections(root);
        
        if(debugkaru) printf("\n[DEBUG] AST after selection push-down:\n");
        // print_ast(root, 0);
    }

    if (enable_projection_pushdown) {
        printf("Applying projection push-down...\n");
        root = push_down_projections(root);
        
        if(debugkaru) printf("\n[DEBUG] AST after projection push-down:\n");
        // print_ast(root, 0);
    }
    
    // Print AST after projection push-down
    if(debugkaru) printf("\n[DEBUG] AST after projection push-down:\n");
    // print_ast(root, 0);
    
    // if (enable_join_reordering) {
    //     printf("Applying join reordering...\n");
    //     root = reorder_joins(root);
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

// Push down selections to the appropriate tables
Node* push_down_selections(Node *node) {
    if (!node) return NULL;
    printf("Pushing down selections...%s\n", node->operation ? node->operation : "NULL");
    
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
            }
            
            // Try to push to right table first
            if (right_table && can_push_to_table(node->arg1, right_table->arg1, node)) {
                // printf("Checking if '%s' can push to '%s': table=%s, column=%s\n", 
                //        node->arg1, right_table->arg1, right_table->arg1, 
                //        strchr(node->arg1, '.') ? strchr(node->arg1, '.') + 1 : "unknown");
                // printf("Result: %d\n", can_push_to_table(node->arg1, right_table->arg1, node));
                printf("Pushing condition '%s' down to table '%s'\n", node->arg1, right_table->arg1);
                
                // Create a new selection node for the right table
                Node *new_selection = new_node("σ", strdup(node->arg1), NULL);
                new_selection->child = right_table;
                
                // Detach right table from left table
                left_table->child = NULL;
                
                // Create a proper join structure
                join_node->child = left_table;
                left_table->next = new_selection;
                
                // We've pushed the selection down, so we can replace the original selection
                // with the join node
                
                // Clean up original selection node
                node->child = NULL;  // Prevent double free
                free(node->operation);
                free(node->arg1);
                free(node);
                
                return join_node;
            } 
            // Try to push to left table
            else if (can_push_to_table(node->arg1, left_table->arg1, node)) {
                printf("Pushing condition '%s' down to table '%s'\n", node->arg1, left_table->arg1);
                
                // Create a new selection node for the left table
                Node *new_selection = new_node("σ", strdup(node->arg1), NULL);
                new_selection->child = left_table;
                
                // Detach right table from left table
                left_table->child = NULL;
                
                // Create a proper join structure
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
    
    // Recursively process children and siblings
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
            printf("table(%s AS %s )\n", node->arg1, node->arg2, estimate_cost(node));
        else
            printf("table(%s)\n", node->arg1, estimate_cost(node));
    }
    else if (strcmp(node->operation, "σ") == 0) {
        printf("σ(%s )\n", node->arg1, estimate_cost(node));
    }
    else if (strcmp(node->operation, "⨝") == 0) {
        printf("⨝(%s)\n", node->arg1, estimate_cost(node));
    }
    else if (strcmp(node->operation, "π") == 0) {
        printf("π(%s)\n", node->arg1, estimate_cost(node));
    }
    else {
        printf("%s %s %s \n", 
               node->operation, 
               node->arg1 ? node->arg1 : "", 
               node->arg2 ? node->arg2 : "", 
               estimate_cost(node));
    }
    
    print_execution_plan_recursive(node->child, depth + 1);
    print_execution_plan_recursive(node->next, depth);
}
Node* push_down_projections(Node *node) {
    if (!node) {
        if(debugkaru) printf("\n[DEBUG] Node is NULL, returning NULL\n");
        return NULL;
    }
    
    if(debugkaru) printf("\n[DEBUG] ENTER push_down_projections for node type: %s, arg1: %s\n", 
           node->operation ? node->operation : "NULL", 
           node->arg1 ? node->arg1 : "NULL");
    
    // Print tree structure before modifications
    if(debugkaru) printf("\n[DEBUG] Current tree structure at this node:\n");
    if (node->operation) {
        if(debugkaru) printf("\n[DEBUG]   Node: %s(%s)\n", 
               node->operation, 
               node->arg1 ? node->arg1 : "");
        
        if (node->child) {
            if(debugkaru) printf("\n[DEBUG]     Child: %s(%s)\n", 
                   node->child->operation ? node->child->operation : "NULL", 
                   node->child->arg1 ? node->child->arg1 : "NULL");
            
            if (node->child->child) {
                if(debugkaru) printf("\n[DEBUG]       Child->Child: %s(%s)\n", 
                       node->child->child->operation ? node->child->child->operation : "NULL", 
                       node->child->child->arg1 ? node->child->child->arg1 : "NULL");
            }
        }
        
        if (node->next) {
            if(debugkaru) printf("\n[DEBUG]     Next: %s(%s)\n", 
                   node->next->operation ? node->next->operation : "NULL", 
                   node->next->arg1 ? node->next->arg1 : "NULL");
        }
    }
    
    // Process for projection nodes
    if (node->operation && strcmp(node->operation, "π") == 0) {
        if(debugkaru) printf("\n[DEBUG] Found projection node with columns: %s\n", node->arg1);
        
        Node *child = node->child;
        if (!child) {
            if(debugkaru) printf("\n[DEBUG] Projection has no child, returning node\n");
            return node;
        }
        
        if(debugkaru) printf("\n[DEBUG] Child operation: %s\n", child->operation ? child->operation : "NULL");
        
        // Case 1: Handle projection over selection
        if (child->operation && strcmp(child->operation, "σ") == 0) {
            if(debugkaru) printf("\n[DEBUG] Case 1: Push projection through selection\n");
            
            // Print selection details
            if(debugkaru) printf("\n[DEBUG] Selection condition: %s\n", child->arg1 ? child->arg1 : "NULL");
            
            // Create new projection with same columns but under the selection
            Node *new_projection = new_node("π", strdup(node->arg1), NULL);
            if(debugkaru) printf("\n[DEBUG] Created new projection with columns: %s\n", new_projection->arg1);
            
            // Connect new projection to selection's child
            new_projection->child = child->child;
            if(debugkaru) printf("\n[DEBUG] Connected new projection to selection's child\n");
            
            // Put projection under selection
            child->child = new_projection;
            if(debugkaru) printf("\n[DEBUG] Put new projection under selection\n");
            
            // Return the selection node as the new root
            if(debugkaru) printf("\n[DEBUG] Returning selection node as new root\n");
            Node *result = child;
            node->child = NULL; // Avoid double-free of child
            free(node->operation);
            free(node->arg1);
            free(node);
            
            // Recursively optimize the projection under the selection
            if(debugkaru) printf("\n[DEBUG] Recursively optimizing projection under selection\n");
            result->child = push_down_projections(result->child);
            
            if(debugkaru) printf("\n[DEBUG] EXIT push_down_projections with selection root\n");
            return result;
        }
        // Case 2: Handle projection over joined tables
        else if (child->operation && strcmp(child->operation, "table") == 0 && 
                 child->next && child->next->operation && strcmp(child->next->operation, "⨝") == 0) {
            
            if(debugkaru) printf("\n[DEBUG] Case 2: Push projection through join\n");
            
            Node *left_table = child;
            Node *join_node = child->next;
            
            if(debugkaru) printf("\n[DEBUG] Left table: %s\n", left_table->arg1);
            if(debugkaru) printf("\n[DEBUG] Join condition: %s\n", join_node->arg1);
            
            // Find right table
            Node *right_table = NULL;
            if (left_table->child && left_table->child->operation && 
                strcmp(left_table->child->operation, "table") == 0) {
                right_table = left_table->child;
                if(debugkaru) printf("\n[DEBUG] Found right table as child of left table: %s\n", right_table->arg1);
            } else {
                if(debugkaru) printf("\n[DEBUG] Could not find right table, returning original node\n");
                if(debugkaru) printf("\n[DEBUG] EXIT push_down_projections with original node\n");
                return node;
            }
            
            // Parse projection columns
            if(debugkaru) printf("\n[DEBUG] Parsing projection columns: %s\n", node->arg1);
            char *columns = strdup(node->arg1);
            char *column_list[100];
            int column_count = 0;
            
            char *token = strtok(columns, ",");
            while (token) {
                while (*token == ' ') token++;
                column_list[column_count] = strdup(token);
                if(debugkaru) printf("\n[DEBUG] Column %d: %s\n", column_count, column_list[column_count]);
                column_count++;
                token = strtok(NULL, ",");
            }
            
            // Parse join condition
            if(debugkaru) printf("\n[DEBUG] Parsing join condition: %s\n", join_node->arg1);
            char *left_table_name = NULL, *left_col = NULL;
            char *right_table_name = NULL, *right_col = NULL;
            parse_join_condition(join_node->arg1, &left_table_name, &left_col, 
                               &right_table_name, &right_col);
            
            if(debugkaru) printf("\n[DEBUG] Join parsed: %s.%s = %s.%s\n", 
                   left_table_name ? left_table_name : "NULL", 
                   left_col ? left_col : "NULL", 
                   right_table_name ? right_table_name : "NULL", 
                   right_col ? right_col : "NULL");
            
            // Prepare columns for left and right tables
            char left_columns[512] = "";
            char right_columns[512] = "";
            int left_has_columns = 0;
            int right_has_columns = 0;
            
            if(debugkaru) printf("\n[DEBUG] Sorting columns by table\n");
            for (int i = 0; i < column_count; i++) {
                char *table = NULL, *column = NULL;
                extract_table_column(column_list[i], &table, &column);
                
                if(debugkaru) printf("\n[DEBUG] Column %d: table=%s, column=%s\n", 
                       i, table ? table : "NULL", column ? column : "NULL");
                
                if (table) {
                    if (strcmp(table, left_table->arg1) == 0) {
                        if (left_has_columns) strcat(left_columns, ",");
                        strcat(left_columns, column_list[i]);
                        left_has_columns = 1;
                        if(debugkaru) printf("\n[DEBUG] Added to left columns: %s\n", column_list[i]);
                    }
                    else if (strcmp(table, right_table->arg1) == 0) {
                        if (right_has_columns) strcat(right_columns, ",");
                        strcat(right_columns, column_list[i]);
                        right_has_columns = 1;
                        if(debugkaru) printf("\n[DEBUG] Added to right columns: %s\n", column_list[i]);
                    } else {
                        if(debugkaru) printf("\n[DEBUG] Column doesn't match any table: %s\n", column_list[i]);
                    }
                }
                
                if (table) free(table);
                if (column) free(column);
            }
            
            // Add join columns if needed
            if (left_table_name && left_col) {
                char full_col[100];
                sprintf(full_col, "%s.%s", left_table_name, left_col);
                if (!strstr(left_columns, full_col)) {
                    if (left_has_columns) strcat(left_columns, ",");
                    strcat(left_columns, full_col);
                    left_has_columns = 1;
                    if(debugkaru) printf("\n[DEBUG] Added join column to left: %s\n", full_col);
                }
            }
            
            if (right_table_name && right_col) {
                char full_col[100];
                sprintf(full_col, "%s.%s", right_table_name, right_col);
                if (!strstr(right_columns, full_col)) {
                    if (right_has_columns) strcat(right_columns, ",");
                    strcat(right_columns, full_col);
                    right_has_columns = 1;
                    if(debugkaru) printf("\n[DEBUG] Added join column to right: %s\n", full_col);
                }
            }
            
            if(debugkaru) printf("\n[DEBUG] Final left columns: %s\n", left_columns);
            if(debugkaru) printf("\n[DEBUG] Final right columns: %s\n", right_columns);
            
            // Create new clean nodes for the optimized structure
            Node *new_left_table = new_node("table", strdup(left_table->arg1), NULL);
            Node *new_right_table = new_node("table", strdup(right_table->arg1), NULL);
            
            // Create projections for both tables
            Node *left_projection = NULL;
            if (left_has_columns) {
                left_projection = new_node("π", strdup(left_columns), NULL);
                left_projection->child = new_left_table;
                if(debugkaru) printf("\n[DEBUG] Created left projection: π(%s)\n", left_columns);
            } else {
                left_projection = new_left_table;
                if(debugkaru) printf("\n[DEBUG] No left projection needed\n");
            }
            
            Node *right_projection = NULL;
            if (right_has_columns) {
                right_projection = new_node("π", strdup(right_columns), NULL);
                right_projection->child = new_right_table;
                if(debugkaru) printf("\n[DEBUG] Created right projection: π(%s)\n", right_columns);
            } else {
                right_projection = new_right_table;
                if(debugkaru) printf("\n[DEBUG] No right projection needed\n");
            }
            
            // Create a new join node
            Node *new_join = new_node("⨝", strdup(join_node->arg1), NULL);
            
            // Build the optimized structure
            if(debugkaru) printf("\n[DEBUG] Rebuilding tree with clean structure\n");
            
            // Setting up child-next relationships
            new_join->child = left_projection;      // Left table projection is child of join
left_projection->next = right_projection; // Right table projection is next of left table projection

// Create a new top projection
Node *top_projection = new_node("π", strdup(node->arg1), NULL);
top_projection->child = new_join;        // Join is child of top projection
            
            // Clean up original structures we don't need anymore
            // Free up original nodes that are now replaced
            
            // Cleanup temporary arrays
            for (int i = 0; i < column_count; i++) {
                free(column_list[i]);
            }
            free(columns);
            if (left_table_name) free(left_table_name);
            if (left_col) free(left_col); 
            if (right_table_name) free(right_table_name);
            if (right_col) free(right_col);
            
            // Clean up the original node
            // We should free the original tree carefully to avoid double-free issues
            // This is a simplified approach; in production code, you'd need more careful cleanup
            node->child = NULL; // Detach children to avoid freeing them
            free(node->operation);
            free(node->arg1);
            free(node);
            
            if(debugkaru) printf("\n[DEBUG] Tree rebuilt with clean structure, returning new top projection\n");
            if(debugkaru) printf("\n[DEBUG] EXIT push_down_projections with rebuilt tree\n");
            return top_projection;
        } else {
            if(debugkaru) printf("\n[DEBUG] No special case matched for this projection\n");
        }
    }
    
    // Recursively process children
    if (node->child) {
        if(debugkaru) printf("\n[DEBUG] Recursively processing child of %s\n", node->operation);
        node->child = push_down_projections(node->child);
    }
    if (node->next) {
        if(debugkaru) printf("\n[DEBUG] Recursively processing next of %s\n", node->operation);
        node->next = push_down_projections(node->next);
    }
    
    if(debugkaru) printf("\n[DEBUG] EXIT push_down_projections returning node type: %s\n", 
           node->operation ? node->operation : "NULL");
    return node;
}
void print_ast(Node *node, int depth) {
    if (!node) return;
    
    // Print indentation
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    
    // Print node info
    if (node->operation) {
        if (node->arg1) {
            printf("%s(%s)\n", node->operation, node->arg1);
        } else {
            printf("%s\n", node->operation);
        }
    } else {
        printf("NULL_NODE\n");
    }
    
    // Print children
    if (node->child) {
        print_ast(node->child, depth + 1);
    }
    
    // Print siblings
    if (node->next) {
        print_ast(node->next, depth);
    }
}

// Add this to optimize_query function:
void print_ast_at_stages(Node *root) {
    if(debugkaru) printf("\n[DEBUG] Full AST before optimization:\n");
    // print_ast(root, 0);
    
    // Add after selection push-down:
    if(debugkaru) printf("\n[DEBUG] AST after selection push-down:\n");
    // print_ast(root, 0);
    
    // Add after projection push-down:
    if(debugkaru) printf("\n[DEBUG] AST after projection push-down:\n");
    // print_ast(root, 0);
}

void print_execution_plan(Node *node) {
    print_execution_plan_recursive(node, 0);
}