// $Id$
// $Locker$

// Author. Roar Thronæs.

#define GTY(x)
#define HOST_BITS_PER_WIDE_INT 64
#define POINTER_SIZE 32
#define true 1
#define false 0

#include <stdio.h>

int vms_mm=0;

#include "tree.h"

main(int argc, char ** argv) {
  char * name = 0;
  if (argc==2) 
    name = argv[1];
  extern int yydebug;
  yydebug=1;
  extern FILE *yyin;
  init_stringpool();
  yyin=fopen(name, "r");
  if (yyin==0) {
    printf("could not fopen %s\n",name);
    return 1;
  }
  yyparse();
  extern tree root_tree;
  gencode(root_tree);
  genwrite();
  return 0;
}
