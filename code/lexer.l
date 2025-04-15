%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.tab.h"

void count();
extern int debug;  /* Set to 1 to enable debug prints, 0 to disable */
%}

%option yylineno
%option case-insensitive  

%%
[ \t\n\r]+      {}

"SELECT"    { if (debug) printf("Matched: SELECT\n"); count(); return SELECT; }
"FROM"      { if (debug) printf("Matched: FROM\n"); count(); return FROM; }
"WHERE"     { if (debug) printf("Matched: WHERE\n"); count(); return WHERE; }
"JOIN"      { if (debug) printf("Matched: JOIN\n"); count(); return JOIN; }
"INNER"     { if (debug) printf("Matched: INNER\n"); count(); return INNER; }
"ON"        { if (debug) printf("Matched: ON\n"); count(); return ON; }
"AND"       { if (debug) printf("Matched: AND\n"); count(); return AND; }
"IN"        { if (debug) printf("Matched: IN\n"); count(); return IN; }
"COUNT"     { if (debug) printf("Matched: COUNT\n"); count(); return COUNT; }
"MAX"       { if (debug) printf("Matched: MAX\n"); count(); return MAX; }
"MIN"       { if (debug) printf("Matched: MIN\n"); count(); return MIN; }
"AVG"       { if (debug) printf("Matched: AVG\n"); count(); return AVG; }
"."         { if (debug) printf("Matched: DOT\n"); count(); return DOT; }

[a-zA-Z_][a-zA-Z0-9_]* { 
    if (debug) printf("Matched IDENTIFIER: %s\n", yytext);
    count(); 
    yylval.str = strdup(yytext);
    return IDENTIFIER; 
}

[0-9]+(\.[0-9]+)? { 
    if (debug) printf("Matched NUMBER: %s\n", yytext);
    count(); 
    yylval.num = atoi(yytext);
    return NUMBER; 
}

"="         { if (debug) printf("Matched: EQ\n"); count(); return EQ; }
"<"         { if (debug) printf("Matched: LT\n"); count(); return LT; }
">"         { if (debug) printf("Matched: GT\n"); count(); return GT; }
","         { if (debug) printf("Matched: COMMA\n"); count(); return COMMA; }
";"         { if (debug) printf("Matched: SEMICOLON\n"); count(); return SEMICOLON; }
"("         { if (debug) printf("Matched: LPAREN\n"); count(); return LPAREN; }
")"         { if (debug) printf("Matched: RPAREN\n"); count(); return RPAREN; }

.           { if (debug) printf("Unexpected character: %s\n", yytext); }

%%

void count() {
    
}

int yywrap() {
    return 1;
}