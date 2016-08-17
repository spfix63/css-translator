/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

%pointer
%option nomain
%option noyywrap

%{
#include "yacc.tab.h"
%}

number [0-9]+
identifier [a-zA-Z][a-zA-Z0-9_\-]*
urlSymbol [a-zA-Z0-9]|"-"|"."|"_"|"~"|":"|"/"|"?"|"#"|"["|"]"|"@"|"!"|"$"|"&"|"'"|"("|")"|"*"|"+"|","|";"|"="
url "url("({urlSymbol})*")"
size [0-9]+{sizeType}
sizeType "px"|"%"|"em"|"ex"
symbol {urlSymbol}|" "
string \"({symbol})*\"
eqOperator "="|"~="|"|="
id "#"[a-zA-Z0-9]*
class "."{identifier}
pseudoclass ":"{identifier}
mediaBlock "@media "{identifier}

%%

{mediaBlock}    { yylval.text = strdup(yytext+strlen("@media ")); return MEDIA;}
{class}         { yylval.text = strdup(yytext+strlen(".")); return CLASS;}
{id}            { yylval.text = strdup(yytext+strlen("#")); return ID;}
{eqOperator}    { return EQ_OP;}
{string}        { yylval.text = strdup(yytext); return STRING;}
{size}          { yylval.text = strdup(yytext); return SIZE;}
{url}           { yylval.text = strdup(yytext + strlen("url(")); yylval.text[strlen(yylval.text) - 1] = 0; return URL;}
{identifier}    { yylval.text = strdup(yytext); return IDENTIFIER;}
"{"             { return BRACE_OPEN;}
"}"             { return BRACE_CLOSE;}
"["             { return BRACK_OPEN;}
"]"             { return BRACK_CLOSE;}
{number}        { yylval.intval = atoi(yytext); return NUMBER;}
"("             { return PAREN_OPEN; }
")"             { return PAREN_CLOSE; }
"+"             { return ADD; }
"*"             { return MULT; }
[ \t\n\r]      /* white space */;
","         	{ return COMMA;}
";"         	{ return SEMICOLON;}
":"         	{ return COLON;}
.               { yyerror("Unknown character");}

%%
/*
int main() 
{
	yyin = fopen("Sample38.trm", "r");
	if (yyin) 
		yyparse();
	else
		printf("Couldn't open file\n"); 
	fclose (yyin);
	return 0;
}
*/