// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<descrip.h>
#include<iledef.h>
#include<prvdef.h>
#include<uaidef.h>
#include<cli$routines.h>
#include<starlet.h>

#include<stdio.h>
#include<string.h>
#include <strings.h>

char * privs[] = {
  "",
  "CMKRNL",
  "CMEXEC",
  "SYSNAM",
  "GRPNAM",
  "ALLSPOOL",
  "IMPERSONATE",
  "DIAGNOSE",
  "LOG_IO",
  "GROUP",
  "NOACNT",
  "PRMCEB",
  "PRMMBX",
  "PSWAPM",
  "SETPRI",
  "SETPRV",
  "TMPMBX",
  "WORLD",
  "MOUNT",
  "OPER",
  "EXQUOTA",
  "NETMBX",
  "VOLPRO",
  "PHY_IO",
  "BUGCHK",
  "PRMGBL",
  "SYSGBL",
  "PFNMAP",
  "SHMEM",
  "SYSPRV",
  "BYPASS",
  "SYSLCK",
  "SHARE",
  "UPGRADE",
  "DOWNGRADE",
  "GRPPRV",
  "READALL",
  "IMPORT",
  "AUDIT",
  "SECURITY",
  ""
};

int inst$findpriv(const char * name) {
  int i;
  for (i=1;i<=PRV$K_NUMBER_OF_PRIVS;i++)
    if (0==strncasecmp(name,privs[i],strlen(privs[i])))
      return i;
  return 0;
}

int auth$show() {
  int retlen;
  $DESCRIPTOR(p, "p1");
  char c[80];
  struct dsc$descriptor o;
  o.dsc$a_pointer=c;
  o.dsc$w_length=80;
  memset (c, 0, 80);
  int sts = cli$present(&p);
  if (sts&1) {
    sts = cli$get_value(&p, &o, &retlen);
    if (c[retlen-1]==13)
      c[--retlen]=0;
    if (c[retlen-1]==10)
      c[--retlen]=0;
    o.dsc$w_length=retlen;
  }

  char owner[20];
  int ownlen=0;
  int mem=0;
  int memlen=0;
  int grp=0;
  int grplen=0;
  int priv=0;
  int privlen=0;

  struct _ile3 itmlst[9];
  itmlst[0].ile3$w_length=20;
  itmlst[0].ile3$w_code=UAI$_OWNER;
  itmlst[0].ile3$ps_bufaddr=owner;
  itmlst[0].ile3$ps_retlen_addr=&ownlen;
  itmlst[1].ile3$w_length=4;
  itmlst[1].ile3$w_code=UAI$_MEM;
  itmlst[1].ile3$ps_bufaddr=&mem;
  itmlst[1].ile3$ps_retlen_addr=&memlen;
  itmlst[2].ile3$w_length=4;
  itmlst[2].ile3$w_code=UAI$_GRP;
  itmlst[2].ile3$ps_bufaddr=&grp;
  itmlst[2].ile3$ps_retlen_addr=&grplen;
  itmlst[3].ile3$w_length=4;
  itmlst[3].ile3$w_code=UAI$_PRIV;
  itmlst[3].ile3$ps_bufaddr=&priv;
  itmlst[3].ile3$ps_retlen_addr=&privlen;
  itmlst[4].ile3$w_length=0;
  itmlst[4].ile3$w_code=0;
  sts = sys$getuai(0, 0, &o, &itmlst[0], 0, 0, 0);

  printf("Username: %s\n",o.dsc$a_pointer);
  printf("Owner: %s\n",owner);
  printf("UIC: [%o,%o]\n",grp,mem);

  printf("Privileges:\n");
  int i;
  for (i=0;i<PRV$K_NUMBER_OF_PRIVS;i++,priv=priv>>1)
    if (priv&1)
      printf("%16s",privs[i+1]);
  printf("\n\n");

  return sts;
}
