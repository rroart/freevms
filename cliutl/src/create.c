// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<descrip.h>
#include<ssdef.h>

#include<stdio.h>

int main(int argc, char *argv[])
{
  if (argc>2 && 0==strncmp(argv[1],"/directory",strlen(argv[1]))) {
    struct dsc$descriptor d;
    d.dsc$a_pointer=argv[2];
    d.dsc$w_length=strlen(argv[2]);
    lib$create_dir(&d);
    return SS$_NORMAL;
  }
  fprintf(stderr, "%CREATE-E-Can not create file yet\n");
  return SS$_NORMAL;
}
