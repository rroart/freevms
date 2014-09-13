// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<descrip.h>
#include<ssdef.h>
#include<lib$routines.h>
#include<cli$routines.h>

#include<stdio.h>
#include<string.h>

int main(int argc, char *argv[])
{
  setvbuf(stdout, NULL, _IONBF, 0);      // need this to see i/o at all

  unsigned long sts;
  $DESCRIPTOR(q, "directory");

  sts = cli$present(&q);

  if (sts&1) {
    $DESCRIPTOR(p, "p1");
    int retlen;
    char c[80];
    struct dsc$descriptor o;
    o.dsc$a_pointer=c;
    o.dsc$w_length=80;
    memset(c, 0, 80);

    sts = cli$get_value(&p, &o, &retlen);

    struct dsc$descriptor d;
    d.dsc$a_pointer=c;
    d.dsc$w_length=strlen(c);
    // not yet    lib$create_dir(&d);
    lib$create_dir2(c,1);
    return SS$_NORMAL;
  }
  fprintf(stderr, "%CREATE-E-Can not create file yet\n");
  return SS$_NORMAL;
}
