// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <stdio.h> 
#include <ssdef.h> 
#include <descrip.h> 
#include <starlet.h>
#include <misc.h>

void stringify(char * src, char * dst, int size) {
  for ( ; size; size--, src++, dst++) {
    if ( *src >= 32 && *src < 127)
      *dst = *src;
    else
      *dst = '.';
  }
}

int sda$examine(int mask) {
  int retlen;
  $DESCRIPTOR(p, "p1");
  $DESCRIPTOR(p2, "p2"); // temp workaround
  char c[80];
  struct dsc$descriptor o;
  o.dsc$a_pointer=c;
  o.dsc$w_length=80;
  memset (c, 0, 80);
  char c2[80];
  struct dsc$descriptor o2;
  o2.dsc$a_pointer=c2;
  o2.dsc$w_length=80;
  memset (c2, 0, 80);
  int sts = cli$present(&p);
  if (sts&1) {
    sts = cli$get_value(&p, &o, &retlen);
    o.dsc$w_length=retlen;
  }

  char * semi = strchr (c, ';');
  if (semi)
    *semi = 0;
#if 0
  // not yet
  long addr = strtol (c, 0, 16);
#else
  long addr = strtol (c, 0, 10);
#endif
  long isnot_addr = addr;
  if (addr == 0) 
    sts = sda_find_addr (c, &addr);
  if ((sts&1)==0)
    return sts;
  long size = sizeof(long);
  if (semi) {
#if 0
    // not yet
    size = strtol (semi+1, 0, 16);
#else
    size = strtol (semi+1, 0, 10);
#endif
    size = ((size>>4)+1)<<4;
  } else {
    sts = cli$present(&p2);
    if (sts&1) {
      sts = cli$get_value(&p2, &o2, &retlen);
      o2.dsc$w_length=retlen;
      if (sts&1) {
#if 0
	// not yet
	size = strtol (c2, 0, 16);
#else
	size = strtol (c2, 0, 10);
#endif
	size = ((size>>4)+1)<<4;
	semi = 1;
      }
    }
  }
  char buf[size];
#if 0
  printf("\n\n%s %x %x %x %x %x\n\n",c,c, semi, isnot_addr, addr, size);
#endif
  sda$getmem(addr, buf, size);
  if (!semi) {
    char * name = 0;
    long offset;
    if (!isnot_addr)
      name = c;
    else 
      sts = sda_find_symbol(addr, &name, &offset);
#if 0
    printf("FIND %x %x %x %x\n",sts,addr,name,offset);
#endif
    long * buf2 = buf;
    printf("\n");
    printf("%s", name);
    if (offset)
      printf("+%x", offset);
    char str[size+1];
    str[size]=0;
    stringify(buf, str, size);
    printf(":  %lx  %s\n", *buf2, str);
  } else {
    char * name = 0;
    long offset;
    int * buf2 = buf;
    char str[size+1];
    str[size]=0;
    int i;
    printf("\n");
    for (i = 0; i < size; i+=0x10, buf2+=4) {
      stringify(buf2, str, 0x10);
      str[0x10]=0;
      printf("%8x %8x %8x %8x  %s    %lx\n", buf2[3], buf2[2], buf2[1], buf2[0], str, addr+i);
    }
  }
}
