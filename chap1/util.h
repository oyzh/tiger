#ifndef UTIL_H
#define UTIL_H
#include <assert.h>

typedef char *string;
typedef char bool;

#define TRUE 1
#define FALSE 0

void *checked_malloc(int);
string String(char *);

typedef struct U_boolList_ *U_boolList;
struct U_boolList_ {bool head; U_boolList tail;};
U_boolList U_BoolList(bool head, U_boolList tail);

/*Author: Zhenhuan Ouyang
 *Data  : 09/22/2015
 *Zhejiang University
 */
#include<stdlib.h>

typedef struct table *Table_;

struct table {
  string id;
  int value;
  Table_ tail;
};
Table_ Table(string id, int value, struct table *tail);

#endif
