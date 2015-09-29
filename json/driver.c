#include <stdio.h>
#include <stdlib.h>
#include "json.h"

extern int yyparse(void);

void parse(char * fname) 
{
  FILE *f=fopen(fname,"r");
  if(!f)exit(1);
  printf("yes3\n");
  if (yyparse() == 0) /* parsing worked */
    fprintf(stderr,"Parsing successful!\n");
  else fprintf(stderr,"Parsing failed\n");
}


int main(int argc, char **argv) {
 if (argc!=2) {fprintf(stderr,"usage: a.out filename\n"); exit(1);}
 printf("yes1\n");
 parse(argv[1]);
 printf("yes2\n");
 return 0;
}
