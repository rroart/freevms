// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <stdio.h>
#include <string.h>

#include <descrip.h>
#include <ssdef.h>
#include <starlet.h>
#include <cli$routines.h>
#include <lib$routines.h>

int set_symbol(char * symstr, char * valstr) {
  struct dsc$descriptor sym_, *sym=&sym_;
  struct dsc$descriptor val_, *val=&val_;
  sym->dsc$a_pointer=symstr;
  sym->dsc$w_length=strlen(symstr);
  val->dsc$a_pointer=valstr;
  val->dsc$w_length=strlen(valstr);
  return lib$set_symbol(sym, val, 0);
}

int show_symbol() {
  $DESCRIPTOR(p, "p1");
  char c[80];
  memset(c, 0, 80);
  struct dsc$descriptor o;
  o.dsc$a_pointer=c;
  o.dsc$w_length=80;
  char d[80];
  memset(d, 0, 80);
  struct dsc$descriptor q;
  q.dsc$a_pointer=d;
  q.dsc$w_length=80;
  int retlen;
  int sts;
  sts = cli$present(&p);
  sts = cli$get_value(&p, &o, &retlen);
  struct dsc$descriptor *sym=&o;
  struct dsc$descriptor *val=&q;
  sym->dsc$w_length=retlen;
#if 0
  sym->dsc$a_pointer=symstr;
  sym->dsc$w_length=strlen(symstr);
  val->dsc$a_pointer=valstr;
  val->dsc$w_length=strlen(valstr);
#endif
  sts = lib$get_symbol(sym, val, 0);
  printf("  %s == %s\n",c,d);
}

int delete_symbol() {
  $DESCRIPTOR(p, "p1");
  char c[80];
  struct dsc$descriptor o;
  o.dsc$a_pointer=c;
  o.dsc$w_length=80;
  int retlen;
  int sts;
  sts = cli$present(&p);
  sts = cli$get_value(&p, &o, &retlen);
  struct dsc$descriptor *sym=&o;
  sym->dsc$w_length=retlen;
#if 0
  sym->dsc$a_pointer=symstr;
  sym->dsc$w_length=strlen(symstr);
  val->dsc$a_pointer=valstr;
  val->dsc$w_length=strlen(valstr);
#endif
  sts = lib$delete_symbol(sym, 0);
  return sts;
}

