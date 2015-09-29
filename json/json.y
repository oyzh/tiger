%{
#include<stdio.h>
#include"json.h"

int yylex(void);
J_Value root;

void yyerror(char *s)
{
  printf("error:%s",s);
}
%}

%union{
	int ival;
	double dval;
	String sval;
	J_Value vval;
	J_Object oval;
	J_Array aval;
	J_Number nval;
	
	}
%token <ival> INT
%token <dval> DOUBLE
%token <sval> STRING

%token FALSEE NULLL TRUEE
       LBRACE RBRACE LBRAKE RBRAKE COLON COMMA

%type <vval> value
%type <oval> object
%type <aval> array
%type <nval> number
%start text

%%
text  : value {root = $1;}
      ;
      
value : FALSEE                {$$ = V_False();}
      | NULLL                 {$$ = V_Null();}
      | TRUEE		      {$$ = V_True();}
      | LBRACE object RBRACE  {$$ = V_Object($2);}
      | LBRAKE array RBRAKE   {$$ = V_Array($2);}
      | number 	     	      {$$ = V_Number($1);}
      | STRING		      {$$ = V_String($1);}
      ;

object: STRING COLON value COMMA object {$$ = O_Object(P_Pair($1, $3), $5);}
      | STRING COLON value 	 	{$$ = O_Object(P_Pair($1, $3), NULL);}
      |	       	     			{$$ = NULL;}
      ;

array : value COMMA array		{$$ = A_Array($1, $3);}	
      | value 	    			{$$ = A_Array($1, NULL);}
      |					{$$ = NULL;}
      ;

number: DOUBLE				{$$ = N_Double($1);}
      | INT				{$$ = N_Int($1);}
      ;
