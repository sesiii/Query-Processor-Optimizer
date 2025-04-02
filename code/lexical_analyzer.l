%{
#include <stdio.h>
#include "lexical_analyzer.h"
int yylex();

void add_token(TokenType type, const char* value);
%}

DIGIT   [0-9]
LETTER  [a-zA-Z_]
ID      {LETTER}({LETTER}|{DIGIT})*

%%

"SELECT"               { add_token(TOKEN_KEYWORD, yytext); }
"FROM"                 { add_token(TOKEN_KEYWORD, yytext); }
"WHERE"                { add_token(TOKEN_KEYWORD, yytext); }
"AND"                  { add_token(TOKEN_KEYWORD, yytext); }
"OR"                   { add_token(TOKEN_KEYWORD, yytext); }
"JOIN"                 { add_token(TOKEN_KEYWORD, yytext); }
{ID}                   { add_token(TOKEN_IDENTIFIER, yytext); }
{DIGIT}+               { add_token(TOKEN_LITERAL, yytext); }
"="|"!="|"<"|"<="|">"|">=" { add_token(TOKEN_OPERATOR, yytext); }
[ \t\n]+               { /* ignore whitespace */ }
.                      { add_token(TOKEN_UNKNOWN, yytext); }

%%

int yywrap(void) {
    return 1;
}


// int main(int argc, char** argv) {
//     if (argc > 1) {
//         FILE* file = fopen(argv[1], "r");
//         if (!file) {
//             perror("fopen");
//             return 1;
//         }
//         yyin = file;
//     }

//     yylex();

//     return 0;
// }