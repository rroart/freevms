// $Id$
// $Locker$

// Author. Roar Thronæs.

// like memcpy

int memcpy(void *dest, const void *src, int n);

int cliutl$getmem(const void *src, void *dest, int n) {
  if (n==0)
    n = sizeof(void *);
  long arglst[4];
  arglst[0]=3;
  arglst[1]=dest;
  arglst[2]=src;
  arglst[3]=n;
  sys$cmkrnl(memcpy, &arglst[0]);
}

int cliutl$exit() {
  // not yet exit(0);
  extern int do_ret;
  do_ret = 1;
}
