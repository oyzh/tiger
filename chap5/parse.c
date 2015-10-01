/*
 * parse.c - Parse source file.
 */

#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "errormsg.h"
#include "parse.h"
#include "prabsyn.h"
#include "semant.h"

extern int yyparse(void);
extern A_exp absyn_root;

/* parse source file fname; 
   return abstract syntax data structure */
A_exp parse(string fname) 
{
	EM_reset(fname);
    if (yyparse() == 0) /* parsing worked */
		return absyn_root;
	else 
		printf("fuck! not pass syntax!\n");
		return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2){
		fprintf(stderr, "usage: a.out filename\n");
		exit(1);
	}
	A_exp temp = parse(argv[1]);
	if (temp) {
		//pr_exp(stdout, temp, 4);
		//printf("\n-------------------------\n");
		SEM_transProg(temp);
	}
	return 0;
}
