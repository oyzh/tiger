#include"prog1.h"
#include<stdio.h>

int main(){
  A_stm p = prog();

  /***********************************************
   *test maxargs
   **********************************************/
  int n = maxargs(p);
  interp(p);
  printf("\nmaxargs of print:%d\n",n);
  return 0;
}
