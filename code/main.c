#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "ra_tree.h"
#include "lex.yy.c"

// extern int yyparse();
extern FILE* yyin;
extern AstNode* ast_root;

int main(int argc, char** argv) {
    // Open input file or use stdin
    if (argc > 1) {
        FILE* input = fopen(argv[1], "r");
        if (!input) {
            printf("Cannot open input file %s\n", argv[1]);
            return 1;
        }
        yyin = input;
    }
    
    // Parse input
    yyparse();
    
    if (!ast_root) {
        printf("Parsing failed to generate an AST\n");
        return 1;
    }
    
    // Convert AST to RA tree
    RANode* ra_tree = ast_to_ra(ast_root);
    
    // Print the RA tree
    printf("Relational Algebra Tree:\n");
    print_ra_tree(ra_tree, 0);
    
    // Cleanup would go here
    
    return 0;
}