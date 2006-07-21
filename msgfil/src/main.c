// $Id$
// $Locker$

// Author. Roar Thronæs.

#define GTY(x)
#define HOST_BITS_PER_WIDE_INT 64
#define POINTER_SIZE 32
#define true 1
#define false 0

#include <stdio.h>
#include <string.h>
#include <libgen.h>

int vms_mm=0;

#include "tree.h"

char * module_name;

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
  name = basename(name);
  int newlen = (long)strchr(name,'.')-(long)name;
  module_name = strndup(name, newlen);
  genwrite();
  yyparse();
  extern int out;
  fclose(out);
  extern tree root_tree;
  //  gencode(root_tree);
  return 0;
}

int get_cli_int() {
  return 0;
}
