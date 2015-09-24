#include "tree.h"
#include <string.h>

T_tree Tree(T_tree l, String k, T_tree r){
  T_tree t = (T_tree)malloc(sizeof(*t));
  t->left = l;
  t->key = k;
  t-> right = r;
  return t;
}

T_tree insert(String key, T_tree t){
  if(t == NULL)
    return Tree(NULL, key, NULL);
  else if(strcmp(key, t->key) < 0)
    return Tree(insert(key, t->left), t->key, t->right);
  else if(strcmp(key, t->key) > 0)
    return Tree(t->left, t->key, insert(key, t->right));
  else return Tree(t->left ,key ,t->right);
}

bool member(String key, T_tree t){
  if(t == NULL)
    return FALSE;
  else if(strcmp(key, t->key) == 0)
    return TRUE;
  else if(strcmp(key, t->key) < 0)
    return member(key,t->left);
  else
    return member(key,t->right);
}
