// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <stdio.h> 
#include <ssdef.h> 
#include <descrip.h> 
#include <lkidef.h>
#include <starlet.h>
#include <misc.h>

/*
Lock Database
-------------
   LKB Address     Lockid  ParentId   PID     BLKAST  SubLocks RQ GR  Queue     
RSB Address             Resource Name          Mode
----------------- -------- -------- -------- -------- -------- -- -- ------- ---
-------------- ------------------------------- ----
FFFFFFFF.79BD9E80 12000001 00000000 00000000 82B167A8        0    PR Granted FFF
FFFFF.7FCCBE80 F11B$aOBELIXSYS   ....          Kern
 */

// missing LKI codes, do this as privileged as show memory?

show_locks(){
  int i;
  struct item_list_3 lst[14];
  char rsbname[15];
  char rsblen;
  int lckstatus;

  long lkb_addr;
  long lkb_addr_len;
  long lkb_addr_retlen;

  int lockid;
  int lockid_len;
  int lockid_retlen;

  int parentid;
  int parentid_len;
  int parentid_retlen;

  int pid;
  int pid_len;
  int pid_retlen;

  long blkast;
  long blkast_len;
  long blkast_retlen;

  int sublocks;
  int sublocks_len;
  int sublocks_retlen;

  int rq;
  int rq_len;
  int rq_retlen;

  int gr;
  int gr_len;
  int gr_retlen;

  int queue;
  int queue_len;
  int queue_retlen;

  long rsbaddr;
  long rsbaddr_len;
  int rsbaddr_retlen;

  long lkbaddr;
  long lkbaddr_len;
  int lkbaddr_retlen;

  int mode;
  int mode_len;
  int mode_retlen;
  
  printf("Lock Database\n");
  printf("-------------\n");
  printf("   LKB Address     Lockid  ParentId   PID     BLKAST  SubLocks RQ GR  Queue     RSB Address             Resource Name          Mode\n");
  printf("----------------- -------- -------- -------- -------- -------- -- -- ------- ----------------- ------------------------------- ----");

  lst[0].buflen=&lkbaddr_len;
  lst[0].item_code=LKI$_NAMSPACE;
  lst[0].bufaddr=&lkbaddr;
  lst[0].retlenaddr=&lkbaddr_retlen;
  lst[1].buflen=&lockid_len;
  lst[1].item_code=LKI$_LKID;
  lst[1].bufaddr=&lockid;
  lst[1].retlenaddr=&lockid_retlen;
  lst[2].buflen=&parentid_len;
  lst[2].item_code=LKI$_PARENT;
  lst[2].bufaddr=&parentid;
  lst[2].retlenaddr=&parentid_retlen;
  lst[3].buflen=&blkast_len;
  lst[3].item_code=LKI$_BLOCKING;
  lst[3].bufaddr=&blkast;
  lst[3].retlenaddr=&blkast_retlen;
  lst[4].buflen=&sublocks_len;
  lst[4].item_code=LKI$_LOCKS; // check. don't know yet
  lst[4].bufaddr=&sublocks;
  lst[4].retlenaddr=&sublocks_retlen;
  lst[5].buflen=&rq_len;
  lst[5].item_code=LKI$_BLOCKER_BR;
  lst[5].bufaddr=&rq;
  lst[5].retlenaddr=&rq_retlen;
  lst[6].buflen=&gr_len;
  lst[6].item_code=LKI$_LOCKS_BR;
  lst[6].bufaddr=&gr;
  lst[6].retlenaddr=&gr_retlen;
  lst[7].buflen=&queue_len;
  lst[7].item_code=LKI$_MSTCSID;
  lst[7].bufaddr=&queue;
  lst[7].retlenaddr=&queue_retlen;
  lst[8].buflen=&queue_len;
  lst[8].item_code=LKI$_VALBLKST;
  lst[8].bufaddr=&queue;
  lst[8].retlenaddr=&queue_retlen;
  lst[9].buflen=&rsbaddr_len;
  lst[9].item_code=LKI$_RANGE;
  lst[9].bufaddr=&rsbaddr;
  lst[9].retlenaddr=&rsbaddr_retlen;
  lst[10].buflen=15;
  lst[10].item_code=LKI$_RESNAM;
  lst[10].bufaddr=rsbname;
  lst[10].retlenaddr=&rsblen;
  lst[11].buflen=&mode_len;
  lst[11].item_code=LKI$_SYSTEM;
  lst[11].bufaddr=&mode;
  lst[11].retlenaddr=&mode_retlen;
  lst[12].buflen=&pid_len;
  lst[12].item_code=LKI$_PID;
  lst[12].bufaddr=&pid;
  lst[12].retlenaddr=&pid_retlen;
  lst[13].buflen=0;
  lst[13].item_code=0;
  do {
    lckstatus=sys$getlki(0,0,lst,0,0,0,0);
    if (lckstatus&1) {
      printf("%16lx ", lkbaddr);
      printf("%8x ", lockid);
      printf("%8x ", parentid);
      printf("%8x ", pid);
      printf("%16lx ", blkast);
      printf("%8x ", sublocks);
      printf("%x ", rq);
      printf("%x ", gr);
      printf("%x ", queue);
      printf("%16lx ", rsbaddr);
      printf("%s",rsbname);
      printf("%x ", mode);
      printf("\n");
    }
  } while (lckstatus == SS$_NORMAL);
  //} while (jpistatus != SS$_NOMOREPROC);
  //printf("lckstatus %x\n",lckstatus);
}
