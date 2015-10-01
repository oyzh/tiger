/*
 * table.c - Functions to manipulate generic tables.
 * Copyright (c) 1997 Andrew W. Appel.
 */

#include <stdio.h>
#include "util.h"
//#include "symbol.h"
//#include "types.h"
//#include "env.h"
#include "table.h"

#define TABSIZE 127

typedef struct binder_ *binder;
struct binder_ {void *key; void *value; binder next; void *prevtop;};
struct TAB_table_ {
  binder table[TABSIZE];
  void *top;
};


static binder Binder(void *key, void *value, binder next, void *prevtop)
{binder b = checked_malloc(sizeof(*b));
 b->key = key; b->value=value; b->next=next; b->prevtop = prevtop; 
 return b;
}
/* create a new S_table (author named is terrible) */
TAB_table TAB_empty(void)
{ 
 TAB_table t = checked_malloc(sizeof(*t));
 int i;
 t->top = NULL;
 for (i = 0; i < TABSIZE; i++)
   t->table[i] = NULL;
 return t;
}

/* The cast from pointer to integer in the expression
 *   ((unsigned)key) % TABSIZE
 * may lead to a warning message.  However, the code is safe,
 * and will still operate correctly.  This line is just hashing
 * a pointer value into an integer value, and no matter how the
 * conversion is done, as long as it is done consistently, a
 * reasonable and repeatable index into the table will result.
 */

void TAB_enter(TAB_table t, void *key, void *value)
{int index;
 assert(t && key);
 index = ((unsigned)key) % TABSIZE;
 t->table[index] = Binder(key, value,t->table[index], t->top);
 t->top = key;
}

void *TAB_look(TAB_table t, void *key)
{int index;
 binder b;
 assert(t && key);
 index=((unsigned)key) % TABSIZE;
 for(b=t->table[index]; b; b=b->next)
   if (b->key==key) return b->value;
 return NULL;
}

void *TAB_pop(TAB_table t) {
  void *k; binder b; int index;
  assert (t); /*table shoud has mem*/
  k = t->top;
  assert (k); /*init table should not pop*/ 
  index = ((unsigned)k) % TABSIZE;
  b = t->table[index];
  assert(b);
  t->table[index] = b->next;
  t->top=b->prevtop;
  return b->key;
}

void TAB_dump(TAB_table t, void (*show)(void *key, void *value)) {
  void *k = t->top;
  int index = ((unsigned)k) % TABSIZE;
  binder b = t->table[index];
  if (b==NULL) return;
  t->table[index]=b->next;
  t->top=b->prevtop;
  show(b->key,b->value);
  TAB_dump(t,show);
  assert(t->top == b->prevtop && t->table[index]==b->next);
  t->top=k;
  t->table[index]=b;
}
/*
void TAB_show_tenv(TAB_table t) {
	if (!t) {
		puts("empty table");
		return;
	}
	binder * i = t->table;
	for (; i < &t->table[TABSIZE]; i++) {
		if (*i) {
			char * s = "";
			char ext = '\0';
			Ty_ty tmp = (Ty_ty)(*i)->value;
			if (tmp) {
			printf("%d", tmp->kind);  	
			switch (tmp->kind) {
			case Ty_record:
				s = "reocord";
				break;
			case Ty_array:
				s = "arrry of";
				ext = '0' + tmp->u.array->kind;
				break;
			case Ty_int:
				s = "int";
				break;
			case Ty_string:
				s = "string";
				break;
			case Ty_nil:
				s = "nil";
				break;
			case Ty_void:
				s = "void";
				break;
			case Ty_name:
				s = S_name(tmp->u.name.sym);
				break;
			case Ty_double:
				s = "double";
				break;
			}
			
		}
		printf("%s => %s %c\n", S_name((S_symbol)(*i)->key), s, ext);	
		}
	}
}

void TAB_show_venv(TAB_table v) {
	
}
*/
