#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#define MAX_TOKEN_LENGTH 256

typedef enum {
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_OPERATOR,
    TOKEN_LITERAL,
    TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LENGTH];
} Token;

void add_token(TokenType type, const char* value);

#endif // LEXICAL_ANALYZER_H