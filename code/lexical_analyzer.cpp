#include <stdio.h>
#include <string.h>
#include "lexical_analyzer.h"
#include <FlexLexer.h> // Include this if yyin is defined by Flex
#include <iostream>

#define MAX_TOKENS 100
extern int yylex();

typedef struct
{
    Token tokens[MAX_TOKENS];
    int token_count;
} LexicalAnalyzerState;

LexicalAnalyzerState state;

void add_token(TokenType type, const char *value)
{
    if (state.token_count < MAX_TOKENS)
    {
        state.tokens[state.token_count].type = type;
        strncpy(state.tokens[state.token_count].value, value, MAX_TOKEN_LENGTH);
        state.token_count++;
    }
}

int main(int argc, char **argv)
{
    if (argc > 1)
    {
        FILE *file = fopen(argv[1], "r");
        std::cout<<argv[1]<<std::endl;
        if (!file)
        {
            printf("Entered\n");
            perror("fopen");
            return 1;
            
            // yyin = file;
        }
        extern FILE *yyin; // Declare yyin as an external variable
        yyin = file;
    }
    
    yylex();
    printf("came out..\n");

    for (int i = 0; i < state.token_count; i++)
    {
        printf("Token: %s, Type: %d\n", state.tokens[i].value, state.tokens[i].type);
    }

    return 0;
}