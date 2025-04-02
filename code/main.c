#include "parser.h"
#include "parser.tab.h"

Query *query = NULL;

Node *new_node(char *op, char *arg1, char *arg2) {
    Node *n = malloc(sizeof(Node));
    n->operation = op;
    n->arg1 = arg1;
    n->arg2 = arg2;
    n->child = NULL;
    return n;
}

void print_tree(Node *node, int depth) {
    if (!node) return;
    for (int i = 0; i < depth; i++) printf("  ");
    printf("%s(%s, %s)\n", node->operation, node->arg1, node->arg2);
    print_tree(node->child, depth + 1);
}

Node *build_relational_algebra(Query *q) {
    Node *root = NULL;
    if (q->condition_col) {
        // σ(age > 18) (students)
        Node *selection = new_node("σ", strdup(q->condition_col), q->table);
        char condition[50];
        sprintf(condition, "%s > %d", q->condition_col, q->condition_val);
        selection->arg1 = strdup(condition);

        // π(name) (σ(age > 18) (students))
        root = new_node("π", q->column, q->table);
        root->child = selection;
    } else {
        // π(a) (b) for simple queries
        root = new_node("π", q->column, q->table);
    }
    return root;
}

int main() {
    printf("Enter query (e.g., SELECT name FROM students WHERE age > 18; or SELECT a FROM b;):\n");
    yyparse();

    if (query) {
        printf("\nParsed Query:\n");
        if (query->condition_col) {
            printf("Column: %s, Table: %s, Condition: %s > %d\n",
                   query->column, query->table, query->condition_col, query->condition_val);
        } else {
            printf("Column: %s, Table: %s, No condition\n",
                   query->column, query->table);
        }

        printf("\nRelational Algebra Tree:\n");
        Node *tree = build_relational_algebra(query);
        print_tree(tree, 0);
    }
    return 0;
}