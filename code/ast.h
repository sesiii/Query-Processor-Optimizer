#ifndef AST_H
#define AST_H

// Column list structure
typedef struct ColumnList {
    char* name;
    struct ColumnList* next;
} ColumnList;

// AST node types
typedef enum {
    NODE_SELECT,
    NODE_FROM,
    NODE_WHERE,
    NODE_JOIN,
    NODE_TABLE,
    NODE_CONDITION,
    NODE_QUERY
} NodeType;

// AST node structure
typedef struct AstNode {
    NodeType type;
    union {
        struct {
            struct ColumnList* columns;
        } select;
        
        struct {
            struct AstNode* table;
        } from;
        
        struct {
            char* name;
        } table;
        
        struct {
            struct AstNode* left;
            char* right_table;
            struct AstNode* condition;
        } join;
        
        struct {
            struct AstNode* condition;
        } where;
        
        struct {
            char* left;
            char* op;
            int right_val;
            char* right_id;
            struct AstNode* left_cond;
            struct AstNode* right_cond;
        } condition;
        
        struct {
            struct AstNode* select;
            struct AstNode* from;
            struct AstNode* where;
        } query;
    } data;
} AstNode;

// AST node creation functions
ColumnList* create_column_list(char* name, ColumnList* next);
AstNode* create_select_node(ColumnList* columns);
AstNode* create_from_node(AstNode* table);
AstNode* create_table_node(char* name);
AstNode* create_join_node(AstNode* left, char* right, AstNode* condition);
AstNode* create_where_node(AstNode* condition);
AstNode* create_condition_node(char* left, char* op, int right);
AstNode* create_eq_condition_node(char* left, char* right);
AstNode* create_and_condition_node(AstNode* left, AstNode* right);
AstNode* create_or_condition_node(AstNode* left, AstNode* right);
AstNode* create_query_node(AstNode* select, AstNode* from, AstNode* where);

#endif