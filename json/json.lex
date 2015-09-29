%{
#include <string.h>
#include <stdio.h>
#include "json.h"/*json.h must on the prev of y.tab.h, so bad .*/
#include "y.tab.h"

#define MAXLEN 1024

int charPos=1;
/*The returned string is not in a unique memory.So when next string coming ,the last one will drop. Compiler's next step should save the string.*/
char str[MAXLEN];
int iden;
void addchar(char c){
  str[iden++] = c;
}
void addstr(char * s,int n){
  int i = 0;
  while(i < n)
    str[iden++] = s[i++];
}
 
int yywrap(void)
{
 charPos=1;
 return 1;
}


void adjust(void)
{
 charPos+=yyleng;
}

%}

%x string
digits [0-9]+
space [ \t\r]+

%%
{space}               {adjust(); printf("a blank"); continue;}
\n                    {adjust(); continue;}
","                   {adjust(); printf(",");return COMMA;}
":"                   {adjust(); return COLON;}
"["                   {adjust(); return LBRAKE;}
"]"                   {adjust(); return RBRAKE;}
"{"                   {adjust(); printf("a {");return LBRACE;}
"}"                   {adjust(); return RBRACE;}
"null"                  {adjust(); return NULLL;}
"false"                 {adjust(); return FALSEE;}
"true"                  {adjust(); return TRUEE;}
"\""                  {adjust(); iden = 0; BEGIN string;}
{digits}+             {adjust(); yylval.ival = atoi(yytext); return INT;}
<string>"\\\""        {adjust(); addstr(yytext,2); continue;}
<string>"\""          {adjust(); addchar('\0'); yylval.sval = str; BEGIN INITIAL; return STRING;}
<string>.             {adjust(); addstr(yytext,1); continue;}
.                     {exit(1);}
%%
