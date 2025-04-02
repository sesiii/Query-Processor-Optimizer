#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ra_tree.h"

RANode* create_ra_project(char** columns, int count, RANode* input) {
    RANode* node = (RANode*)malloc(sizeof(RANode));
    node->type = RA_PROJECT;
    node->data.project.columns = columns;
    node->data.project.column_count = count;
    node->data.project.input = input;
    return node;
}

RANode* create_ra_select(char* condition, RANode* input) {
    RANode* node = (RANode*)malloc(sizeof(RANode));
    node->type = RA_SELECT;
    node->data.select.condition = strdup(condition);
    node->data.select.input = input;
    return node;
}

RANode* create_ra_join(char* condition, RANode* left, RANode* right) {
    RANode* node = (RANode*)malloc(sizeof(RANode));
    node->type = RA_JOIN;
    node->data.join.condition = strdup(condition);
    node->data.join.left = left;
    node->data.join.right = right;
    return node;
}

RANode* create_ra_relation(char* name) {
    RANode* node = (RANode*)malloc(sizeof(RANode));
    node->type = RA_RELATION;
    node->data.relation.name = strdup(name);
    return node;
}

char* condition_to_string(AstNode* condition) {
    if (!condition) return NULL;
    
    char buffer[1024] = {0};
    
    if (condition->data.condition.left_cond && condition->data.condition.right_cond) {
        // This is a complex condition (AND/OR)
        char* left_str = condition_to_string(condition->data.condition.left_cond);
        char* right_str = condition_to_string(condition->data.condition.right_cond);
        sprintf(buffer, "(%s %s %s)", left_str, condition->data.condition.left, right_str);
        free(left_str);
        free(right_str);
    } else if (condition->data.condition.right_id) {
        // This is an equality condition between two columns
        sprintf(buffer, "%s %s %s", condition->data.condition.left, 
                condition->data.condition.op, condition->data.condition.right_id);
    } else {
        // This is a condition with a literal value
        sprintf(buffer, "%s %s %d", condition->data.condition.left, 
                condition->data.condition.op, condition->data.condition.right_val);
    }
    
    return strdup(buffer);
}

RANode* ast_to_ra(AstNode* ast) {
    if (!ast) return NULL;
    
    switch (ast->type) {
        case NODE_QUERY: {
            // Start with base relations from FROM clause
            RANode* ra = ast_to_ra(ast->data.query.from);
            
            // Apply WHERE clause conditions
            if (ast->data.query.where) {
                char* cond_str = condition_to_string(ast->data.query.where->data.where.condition);
                RANode* select_node = create_ra_select(cond_str, ra);
                free(cond_str);
                ra = select_node;
            }
            
            // Apply SELECT clause projections
            int col_count = 0;
            ColumnList* curr = ast->data.query.select->data.select.columns;
            while (curr) {
                col_count++;
                curr = curr->next;
            }
            
            char** columns = (char**)malloc(sizeof(char*) * col_count);
            curr = ast->data.query.select->data.select.columns;
            for (int i = 0; i < col_count; i++) {
                columns[i] = strdup(curr->name);
                curr = curr->next;
            }
            
            RANode* project_node = create_ra_project(columns, col_count, ra);
            
            return project_node;
        }
        
        case NODE_FROM: {
            return ast_to_ra(ast->data.from.table);
        }
        
        case NODE_TABLE: {
            return create_ra_relation(ast->data.table.name);
        }
        
        case NODE_JOIN: {
            RANode* left = ast_to_ra(ast->data.join.left);
            RANode* right = create_ra_relation(ast->data.join.right_table);
            
            char* cond_str = condition_to_string(ast->data.join.condition);
            RANode* join_node = create_ra_join(cond_str, left, right);
            free(cond_str);
            
            return join_node;
        }
        
        default:
            return NULL;
    }
}

void print_ra_tree(RANode* node, int indent) {
    if (!node) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    
    switch (node->type) {
        case RA_PROJECT:
            printf("π_{");
            for (int i = 0; i < node->data.project.column_count; i++) {
                printf("%s", node->data.project.columns[i]);
                if (i < node->data.project.column_count - 1) printf(", ");
            }
            printf("}\n");
            print_ra_tree(node->data.project.input, indent + 1);
            break;
            
        case RA_SELECT:
            printf("σ_{%s}\n", node->data.select.condition);
            print_ra_tree(node->data.select.input, indent + 1);
            break;
            
        case RA_JOIN:
            printf("⋈_{%s}\n", node->data.join.condition);
            print_ra_tree(node->data.join.left, indent + 1);
            print_ra_tree(node->data.join.right, indent + 1);
            break;
            
        case RA_RELATION:
            printf("%s\n", node->data.relation.name);
            break;
    }
}