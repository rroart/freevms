// $Id$
// $Locker$

// Author. Roar Thronæs.

#define GTY(x)
#define HOST_BITS_PER_WIDE_INT 64
#define POINTER_SIZE 32
#define true 1
#define false 0

#include<stdio.h>

#include "tree.h"

cdumain(int argc, char ** argv) {
  if (argc==2) cli$cli(argv[1]);
  return 0;
}

extern void fancy_abort (const char * a, int b, const char * c){
  abort();
}

int ggc_calloc (int s1, int s2) {
  int i= malloc(s1*s2);
  memset(i,0,s1*s2);
  return i;
}

int ggc_alloc(int x) {
  return malloc(x);
}

int xcalloc (int s, int t) {
  return malloc(s*t);
}

int xrealloc(int x, int y) {
  return xrealloc(x,y);
}

extern void layout_type (tree y){
}

int ggc_alloc_tree(length) {
  // check
  return malloc(length);
}

