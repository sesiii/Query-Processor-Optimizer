#include "parser.h"
#include "parser.tab.h"

extern void yy_scan_string(const char *str);

Node *root = NULL;

Node *new_node(char *op, char *arg1, char *arg2) {
    Node *n = malloc(sizeof(Node));
    n->operation = op ? strdup(op) : NULL;
    n->arg1 = arg1 ? strdup(arg1) : NULL;
    n->arg2 = arg2 ? strdup(arg2) : NULL;
    n->child = NULL;
    n->next = NULL;
    // printf("Created node: %s, %s, %s\n", n->operation, n->arg1, n->arg2);  // Debug
    return n;
}

void print_tree(Node *node, int depth) {
    if (!node) return;
    for (int i = 0; i < depth; i++) printf("  ");
    if (node->arg1 && node->arg2)
        printf("%s(%s, %s)\n", node->operation, node->arg1, node->arg2);
    else if (node->arg1)
        printf("%s(%s)\n", node->operation, node->arg1);
    else
        printf("%s\n", node->operation);
    print_tree(node->child, depth + 1);
    print_tree(node->next, depth);
}

int main() {
    printf("Parsing hardcoded query:SELECT a, b FROM table1 JOIN table2 ON table1.id = table2.id WHERE c > 5;\n");
    yy_scan_string("SELECT a, b FROM table1 JOIN table2 ON table1.id = table2.id WHERE c > 5;");  // Hardcoded input
    yyparse();

    if (root) {
        printf("\nAbstract Syntax Tree:\n");
        print_tree(root, 0);
    } else {
        printf("No AST generated.\n");
    }
    return 0;
}