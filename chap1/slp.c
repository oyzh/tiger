#include "slp.h"
#include<stdio.h>

A_stm A_CompoundStm(A_stm stm1, A_stm stm2) {
  A_stm s = checked_malloc(sizeof *s);
  s->kind=A_compoundStm; s->u.compound.stm1=stm1; s->u.compound.stm2=stm2;
  return s;
}


A_stm A_AssignStm(string id, A_exp exp) {
  A_stm s = checked_malloc(sizeof *s);
  s->kind=A_assignStm; s->u.assign.id=id; s->u.assign.exp=exp;
  return s;
}

A_stm A_PrintStm(A_expList exps) {
  A_stm s = checked_malloc(sizeof *s);
  s->kind=A_printStm; s->u.print.exps=exps;
  return s;
}

A_exp A_IdExp(string id) {
  A_exp e = checked_malloc(sizeof *e);
  e->kind=A_idExp; e->u.id=id;
  return e;
}

A_exp A_NumExp(int num) {
  A_exp e = checked_malloc(sizeof *e);
  e->kind=A_numExp; e->u.num=num;
  return e;
}

A_exp A_OpExp(A_exp left, A_binop oper, A_exp right) {
  A_exp e = checked_malloc(sizeof *e);
  e->kind=A_opExp; e->u.op.left=left; e->u.op.oper=oper; e->u.op.right=right;
  return e;
}

A_exp A_EseqExp(A_stm stm, A_exp exp) {
  A_exp e = checked_malloc(sizeof *e);
  e->kind=A_eseqExp; e->u.eseq.stm=stm; e->u.eseq.exp=exp;
  return e;
}

A_expList A_PairExpList(A_exp head, A_expList tail) {
  A_expList e = checked_malloc(sizeof *e);
  e->kind=A_pairExpList; e->u.pair.head=head; e->u.pair.tail=tail;
  return e;
}

A_expList A_LastExpList(A_exp last) {
  A_expList e = checked_malloc(sizeof *e);
  e->kind=A_lastExpList; e->u.last=last;
  return e;
}



/*Author: Zhenhuan Ouyang
 *Data  : 09/22/2015
 *Zhejiang University
 */
#include<string.h>
#include"util.h"

Table_ update(Table_ t,string index,int valueble){
  Table_ p;
  p = t;
  while(p){
    if(strcmp(index,p->id) == 0){
      p->value = valueble;
      return t;
    }
    p = p->tail;
  }
  p = Table(index,valueble,t);
  return p;
}

int lookup(Table_ t,string key){
  Table_ p;
  p = t;
  while(p){
    if(strcmp(p->id,key) == 0){
      return p->value;
    }
  }
  printf("No varable:%s",key);
  exit(0);
}


    
/*
int maxargs(A_stm stm){
}
*/
struct IntAndTable interpExp(A_exp e, Table_ t){
  struct IntAndTable itb;
  int le,ri;
  switch(e->kind){
  case A_idExp:
    itb.i = lookup(t, e->u.id);
    itb.t = t;
    return itb;
    break;
  case A_numExp:
    itb.i = e->u.num;
    itb.t = t;
    return itb;
    break;
  case A_opExp:
    itb = interpExp(e->u.op.left, t);
    le = itb.i;
    itb = interpExp(e->u.op.right, itb.t);
    ri = itb.i;
    {
      switch(e->u.op.oper){
      case A_plus:
	itb.i = le + ri;
	break;
      case A_minus:
	itb.i = le - ri;
	break;
      case A_times:
	itb.i = le * ri;
	break;
      case A_div:
	if(ri == 0){
	  printf("Div 0 error!");
	  exit(0);
	}
	itb.i = le / ri;
	break;
      }
    }
    return itb;
    break;
  case A_eseqExp:
    t = interpStm(e->u.eseq.stm, t);
    itb = interpExp(e->u.eseq.exp, t);
    return itb;
    break;
  default:
    break;
  }
  return itb;
}

struct IntAndTable interpExps(A_expList el, Table_ t){
  struct IntAndTable itb;
  if(el->kind == A_pairExpList){
    itb = interpExp(el->u.pair.head, t);
    printf("%d ",itb.i);
    itb = interpExps(el->u.pair.tail, itb.t);
    return itb;
  }
  else{
    itb = interpExp(el->u.last,t);
    printf("%d ",itb.i);
    return itb;
  }
  return itb;
}


Table_ interpStm(A_stm s,Table_ t){
  Table_ tb_;
  struct IntAndTable itb;
  switch(s->kind){
  case A_compoundStm:
    tb_ = interpStm(s->u.compound.stm1, t);
    tb_ = interpStm(s->u.compound.stm2, tb_);
    return tb_; 
    break;
  case A_assignStm:
    itb = interpExp(s->u.assign.exp, t);
    tb_ = itb.t;
    tb_ = update(tb_, s->u.assign.id, itb.i);
    return tb_;
    break;
  case A_printStm:
    itb = interpExps(s->u.print.exps, t);
    printf("\n");
    return itb.t;
    break;
  default:
    break;
  }
  return NULL;
}

void interp(A_stm s){
  Table_ t;
  t = NULL;
  interpStm(s,t);
}
