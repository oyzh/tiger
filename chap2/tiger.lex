%{
#include <string.h>
#include "util.h"
#include "tokens.h"
#include "errormsg.h"
  
#define MAXLEN 1024
  
int charPos=1;
/*The returned string is not in a unique memory.So when next string coming ,the last one will drop. Compiler's next step should save the string.*/
char str[MAXLEN];
int iden;
void addchar(char c){
  str[iden++] = c;
}
void addstr(string s,int n){
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
 EM_tokPos=charPos;
 charPos+=yyleng;
}

%}
/*Lex Definitions*/

%x comment
%x string
digits [0-9]+
space [ \t\r]+
id [a-zA-Z][a-zA-Z1-9_]*

%%
{space}             {adjust(); continue;}
\n	            {adjust(); EM_newline(); continue;}
","                 {adjust(); return COMMA;}
":"                 {adjust(); return COLON;}
";"                 {adjust(); return SEMICOLON;}
"("                 {adjust(); return LPAREN;}
")"                 {adjust(); return RPAREN;}
"["                 {adjust(); return LBRACK;}
"]"                 {adjust(); return RBRACK;}
"{"                 {adjust(); return LBRACE;}
"}"                 {adjust(); return RBRACE;}
"."	            {adjust(); return DOT;}
"+"                 {adjust(); return PLUS;}
"-"		    {adjust(); return MINUS;}
"*"	            {adjust(); return TIMES;}
"/"	            {adjust(); return DIVIDE;}
"="	            {adjust(); return EQ;}
"<>"	            {adjust(); return NEQ;}
"<"                 {adjust(); return LT;}
"<="                {adjust(); return LE;}
">"	            {adjust(); return GT;}
">="	            {adjust(); return GE;}
"&"                 {adjust(); return AND;}
"|"                 {adjust(); return OR;}
":="                {adjust(); return ASSIGN;}
array               {adjust(); return ARRAY;}
if                  {adjust(); return IF;}
then                {adjust(); return THEN;}
else                {adjust(); return ELSE;}
while               {adjust(); return WHILE;}
for                 {adjust(); return FOR;}
to		    {adjust(); return TO;}
do		    {adjust(); return DO;}
let                 {adjust(); return LET;}
in	            {adjust(); return IN;}
end                 {adjust(); return END;}
of                  {adjust(); return OF;}
break               {adjust(); return BREAK;}
nil                 {adjust(); return NIL;}
function            {adjust(); return FUNCTION;}
var		    {adjust(); return VAR;}
type	            {adjust(); return TYPE;}
{id}                {adjust(); yylval.sval = yytext; return ID;}
{digits}	    {adjust(); yylval.ival=atoi(yytext); return INT;}
\/\*                {adjust(); BEGIN comment;}
<comment>\*\/       {adjust(); BEGIN INITIAL;}
<comment>.          {adjust(); continue;}
"\""                {adjust(); iden = 0; BEGIN string;}
<string>"\\\""      {adjust(); addstr(yytext,2); continue;}
<string>"\""        {adjust(); addchar('\0'); yylval.sval = str; BEGIN INITIAL; return STRING;}
<string>.           {adjust(); addstr(yytext,1); continue;}
.	            {adjust(); EM_error(EM_tokPos,"illegal token");continue;}
%%
