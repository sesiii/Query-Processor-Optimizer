// #include "parser.h"
// #include "parser.tab.h"

// extern void yy_scan_string(const char *str);

// Node *root = NULL;

// Node *new_node(char *op, char *arg1, char *arg2) {
//     Node *n = malloc(sizeof(Node));
//     n->operation = op ? strdup(op) : NULL;
//     n->arg1 = arg1 ? strdup(arg1) : NULL;
//     n->arg2 = arg2 ? strdup(arg2) : NULL;
//     n->child = NULL;
//     n->next = NULL;
//     return n;
// }

// void print_tree(Node *node, int depth) {
//     if (!node) return;
//     for (int i = 0; i < depth; i++) printf("  ");
//     if (node->arg1 && node->arg2)
//         printf("%s(%s AS %s)\n", node->operation, node->arg1, node->arg2);
//     else if (node->arg1)
//         printf("%s(%s)\n", node->operation, node->arg1);
//     else
//         printf("%s\n", node->operation);
//     // Print child first (subquery or from_clause)
//     print_tree(node->child, depth + 1);
//     // Then print next (siblings like join nodes)
//     print_tree(node->next, depth);
// }

// int main() {
//     FILE *file = fopen("query.sql", "r");
//     if (!file) {
//         perror("Failed to open query.sql");
//         return 1;
//     }

//     char *line = NULL;
//     size_t len = 0;
//     ssize_t read;

//     if ((read = getline(&line, &len, file)) != -1) {
//         if (line[read - 1] == '\n') {
//             line[read - 1] = '\0';
//         }
//         printf("Parsing query: %s\n", line);
//         yy_scan_string(line);
//     } else {
//         printf("No query found in query.sql\n");
//         free(line);
//         fclose(file);
//         return 1;
//     }

//     free(line);
//     fclose(file);
//     yyparse();

//     if (root) {
//         printf("\nAbstract Syntax Tree:\n");
//         print_tree(root, 0);
//     } else {
//         printf("No AST generated.\n");
//     }
//     return 0;
// }

//
//new
#include "parser.h"
#include "parser.tab.h"
#include "optimizer.h"

extern void yy_scan_string(const char *str);

Node *root = NULL;

Node *new_node(char *op, char *arg1, char *arg2) {
    Node *n = malloc(sizeof(Node));
    n->operation = op ? strdup(op) : NULL;
    n->arg1 = arg1 ? strdup(arg1) : NULL;
    n->arg2 = arg2 ? strdup(arg2) : NULL;
    n->child = NULL;
    n->next = NULL;
    return n;
}

void print_tree(Node *node, int depth) {
    if (!node) return;
    for (int i = 0; i < depth; i++) printf("  ");
    if (node->arg1 && node->arg2)
        printf("%s(%s AS %s)\n", node->operation, node->arg1, node->arg2);
    else if (node->arg1)
        printf("%s(%s)\n", node->operation, node->arg1);
    else
        printf("%s\n", node->operation);
    // Print child first (subquery or from_clause)
    print_tree(node->child, depth + 1);
    // Then print next (siblings like join nodes)
    print_tree(node->next, depth);
}

int main() {
    FILE *file = fopen("query.sql", "r");
    if (!file) {
        perror("Failed to open query.sql");
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    if ((read = getline(&line, &len, file)) != -1) {
        if (line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        printf("Parsing query: %s\n", line);
        yy_scan_string(line);
    } else {
        printf("No query found in query.sql\n");
        free(line);
        fclose(file);
        return 1;
    }

    free(line);
    fclose(file);
    yyparse();

    if (root) {
        printf("\nOriginal Abstract Syntax Tree:\n");
        print_tree(root, 0);
        
        // Optimize the query
        root = optimize_query(root);
    } else {
        printf("No AST generated.\n");
    }
    return 0;
}