#ifndef TREE_H
#define TREE_H
#define TRUE 1
#define FALSE 0
typedef struct tree *T_tree;
typedef char * String;
typedef int bool;
struct tree{
  T_tree left;
  String key;
  T_tree right;
};

T_tree Tree(T_tree l, String K, T_tree r);
T_tree insert(String key, T_tree t);
bool member(String, T_tree t);
#endif
