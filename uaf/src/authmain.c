// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<stdio.h>
#include<string.h>
#include<unistd.h>

#include<descrip.h> 
#include <cli$routines.h>
#include <lib$routines.h>

#ifdef __i386__
//int errno;
#endif

int auth$show(int mask);
int auth$exit(int mask);

struct cli_struct {
  char * cliroutine;
  void (*fn)();
};

struct cli_struct cliroutines[]={
  {  "auth_exit", auth$exit, }, 
  {  "auth_show", auth$show, }, 
  { 0, 0 , },
};

void * get_cli_int(char * c) {
  int i;
  for (i=0;cliroutines[i].cliroutine;i++) {
    if (0==strncmp(c,cliroutines[i].cliroutine,strlen(cliroutines[i].cliroutine)))
      return cliroutines[i].fn;
  }
  return 0;
}

int vms_mm = 1;

extern long auth_parse[];

int do_ret = 0;

main() {
  printf("\nFreeVMS authorize utility\n\n");

  while(!do_ret) {
    printf("UAF> "); 
    fflush(stdout);

    char command_str[256];
    int len = read (0, command_str, 255);

    struct dsc$descriptor command_line;
    command_line.dsc$a_pointer = command_str;
    command_line.dsc$w_length = len;

    int sts = cli$dcl_parse(&command_line, auth_parse, 0, 0, 0);
    if (sts&1) 
      cli$dispatch(0);
  }
}
