#ifndef RA_TREE_H
#define RA_TREE_H

#include "ast.h"

typedef enum {
    RA_PROJECT,
    RA_SELECT,
    RA_JOIN,
    RA_RELATION
} RANodeType;

typedef struct RANode {
    RANodeType type;
    union {
        struct {
            char** columns;
            int column_count;
            struct RANode* input;
        } project;
        
        struct {
            char* condition;
            struct RANode* input;
        } select;
        
        struct {
            char* condition;
            struct RANode* left;
            struct RANode* right;
        } join;
        
        struct {
            char* name;
        } relation;
    } data;
} RANode;

// Convert AST to RA tree
RANode* ast_to_ra(AstNode* ast);

// Helper functions
RANode* create_ra_project(char** columns, int count, RANode* input);
RANode* create_ra_select(char* condition, RANode* input);
RANode* create_ra_join(char* condition, RANode* left, RANode* right);
RANode* create_ra_relation(char* name);

// Print the RA tree
void print_ra_tree(RANode* node, int indent);

// Convert condition to string
char* condition_to_string(AstNode* condition);

#endif