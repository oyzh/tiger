#include "json.h"
#include <stdlib.h>
#include <assert.h>

J_Value V_False(void){
  J_Value v = (J_Value)malloc(sizeof(struct J_Value_));
  if(v){
    v->kind = K_False;
    return v;
  }
  assert(0);
}

J_Value V_Null(void){
  J_Value v = (J_Value)malloc(sizeof(struct J_Value_));
  if(v){
    v->kind = K_Null;
    return v;
  }
  assert(0);
}

J_Value V_True(void){
  J_Value v = (J_Value)malloc(sizeof(struct J_Value_));
  if(v){
    v->kind = K_True;
    return v;
  }
  assert(0);
}

J_Value V_Object(J_Object o){
  J_Value v = (J_Value)malloc(sizeof(struct J_Value_));
  if(v){
    v->kind = K_Object;
    v->u.obj = o;
    return v;
  }
  assert(0);
}

J_Value V_Array(J_Array a ){
  J_Value v = (J_Value)malloc(sizeof(struct J_Value_));
  if(v){
    v->kind = K_Object;
    v->u.arr = a;
    return v;
  }
  assert(0);
}

J_Value V_Number(J_Number n){
  J_Value v = (J_Value)malloc(sizeof(struct J_Value_));
  if(v){
    v->kind = K_Object;
    v->u.num = n;
    return v;
  }
  assert(0);
}

J_Value V_String(String s){
  J_Value v = (J_Value)malloc(sizeof(struct J_Value_));
  if(v){
    v->kind = K_Object;
    v->u.str = s;
    return v;
  }
  assert(0);
}

J_Object O_Object(J_Pair h, J_Object t){
  J_Object v= (J_Object)malloc(sizeof(struct J_Object_));
  if(v){
    v->head = h;
    v->tail = t;
    return v;
  }
  assert(0);
}

J_Pair P_Pair(String k,J_Value v){
  J_Pair p = (J_Pair)malloc(sizeof(struct J_Pair_));
  if(p){
    p->key = k;
    p->value = v;
    return p;
  }
  assert(0);
}

J_Array A_Array(J_Value h, J_Array t){
  J_Array a = (J_Array)malloc(sizeof(struct J_Array_));
  if(a){
    a->head = h;
    a->tail = t;
    return a;
  }
  assert(0);
}

J_Number N_Double(double d){
  J_Number n = (J_Number)malloc(sizeof(struct J_Number_));
  if(n){
    n->kind = Double;
    n->u.d = d;
    return n;
  }
  assert(0);
}

J_Number N_Int(int i){
  J_Number n = (J_Number)malloc(sizeof(struct J_Number_));
  if(n){
    n->kind = Int;
    n->u.i = i;
    return n;
  }
  assert(0);
}
