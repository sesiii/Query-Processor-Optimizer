%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SELECT 1
#define FROM 2
#define WHERE 3
int yylex(void);
#define JOIN 4
#define ON 5
#define AND 6
#define OR 7
#define IDENTIFIER 8
#define NUMBER 9
#define EQ 10
#define LT 11
#define GT 12
#define COMMA 13
#define YY_DECL int yylex(void)
#define YYSTYPE char*
#define CHARA 14

%}

%%
"SELECT"    { return SELECT; }
"FROM "     { return FROM; }
"WHERE"     { return WHERE; }
"JOIN"      { return JOIN; }
"ON"       { return ON; }
"AND"       { return AND; }
"OR "       { return OR; }
[a-zA-Z][a-zA-Z0-9_]*\.?[a-zA-Z0-9_]*  { 
    // yylval.strval = strdup(yytext);
    return IDENTIFIER; 
}
[0-9]+    { 
    // yylval.intval = atoi(yytext);
    return NUMBER; 
}
"="       { return EQ; }
"<"       { return LT; }
">"       { return GT; }
","       { return COMMA; }
[ \t\n]   { /* ignore whitespace */ }
.         { return CHARA; }
%%

int yywrap() {
    return 1;
}