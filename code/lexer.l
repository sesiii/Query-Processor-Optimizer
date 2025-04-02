%{
#include "parser.h"
#include "parser.tab.h"
%}

%%

"SELECT"    { return SELECT; }
"FROM"      { return FROM; }
"WHERE"     { return WHERE; }
[a-zA-Z][a-zA-Z0-9]*  { yylval.str = strdup(yytext); return IDENTIFIER; }
">"         { return GT; }
[0-9]+      { yylval.num = atoi(yytext); return NUMBER; }
";"         { return SEMICOLON; }  // Recognize semicolon
[ \t\n]     ;  // Ignore whitespace
.           { /* Ignore unexpected characters silently */ }

%%

int yywrap() { return 1; }