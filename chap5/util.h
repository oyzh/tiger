#include <assert.h>

typedef char * string;
typedef char bool;

#define TRUE (char)1
#define FALSE (char)0
#define MAX_LENGTH 512
void * checked_malloc(int);
string String(char *);

typedef struct U_boolList_ * U_boolList;
struct U_boolList_ {bool head; U_boolList tail;};
U_boolList U_BoolList(bool head, U_boolList tail);

