// $Id$
// $Locker$

// Author. Roar Thronæs.
/**
   \file syspcntrl.c
   \brief system process controll
   \author Roar Thronæs
*/

#include<linux/unistd.h>
#include<linux/linkage.h>
#include<linux/sched.h>
#include<asm/bitops.h>
#include<starlet.h>
#include<ssdef.h>
#include<pridef.h>
#include<statedef.h>
#include<evtdef.h>
#include<ipldef.h>
#include<descrip.h>
#include <system_data_cells.h>
#include <exe_routines.h>
#include <sch_routines.h>
#include <misc_routines.h>
#include <ipl.h>
#include <internals.h>
#include <prvdef.h>

/* Author: Roar Thronæs */

unsigned long maxprocesscnt=MAXPROCESSCNT;

/**
   \brief determine whether request process has privs to examine or modify
   \param p pcb
*/

int exe$process_check_priv(struct _pcb * p) {
  int priv = ctl$gl_pcb->pcb$l_priv;
  /** if on the same job tree, success - MISSING */
  /** if the same uic, success - MISSING */
  /** if WORLD priv, success TODO fix this */
  if (p != ctl$gl_pcb && (priv & PRV$M_WORLD) == 0) {
    return SS$_NOPRIV;
  /** if group priv and group, success - MISSING */
  }
  return SS$_NORMAL;
}

int inline process_bit_shift() {
  return sch$gl_pixwidth;
}

int inline process_index_mask() {
  return (1 << sch$gl_pixwidth)-1;
}

int inline process_seq_mask() {
  return 0x1ffff & (~process_index_mask());
}

int alloc_ipid() {
  int i;
  unsigned long *vec=sch$gl_pcbvec;
  unsigned long *seq=sch$gl_seqvec;
  for (i=2;i<MAXPROCESSCNT;i++) {
    if (vec[i]==0) { // change to nullpcb later
      return i| ((seq[i]++)<<16);
    }
  }
  printk(KERN_EMERG "alloc_ipid 0\n");
  return 0;
}

void * exe$ipid_to_pcb(unsigned long pid) {
  int i;
  if ((pid&0xffff)>MAXPROCESSCNT) printk(KERN_EMERG "EXE %x\n",pid);
  if ((pid&0xffff)>MAXPROCESSCNT) {
    unsigned long * x=&i;
    printk(KERN_EMERG "FIND2 %d %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n",pid,pid,x[1],x[2],x[3],x[4],x[5],x[6],x[7],x[8],x[9],x[10],x[11],x[12],x[13],x[14],x[15],x[16]);
  }
  if ((pid&0xffff)>sch$gl_maxpix) return 0;
  unsigned long *vec=sch$gl_pcbvec;
  if (vec[pid&0xffff] && ((struct _pcb *)vec[pid&0xffff])->pcb$l_pid==pid)
    return vec[pid&0xffff];
  return 0;
  return find_process_by_pid(pid); // linux pid compatibility? may bug...
}

void * exe$epid_to_pcb(unsigned long pid) {
  int ipid=exe$epid_to_ipid(pid);
  if (ipid) return exe$ipid_to_pcb(ipid);
  return 0;
}
extern long csid;
int exe$epid_to_ipid(unsigned long pid) {
  if (pid == INIT_PID_REAL) return INIT_PID; // handle init 1
  unsigned long *vec=sch$gl_pcbvec;
  long mask = process_index_mask();
  struct _pcb * p;
  if ((csid&0xff)==(pid>>21)) {
    int ipid=(pid&mask)|(((pid&0x1ffff)>>process_bit_shift())<<16);
    if (vec[ipid&0xffff] && ((struct _pcb *)vec[ipid&0xffff])->pcb$l_pid==ipid)
      { } else {
	if (vec[ipid&0xffff])
	  printk(KERN_EMERG "panic? ipid %x %x %x %x\n",pid,ipid,((struct _pcb *)vec[ipid&0xffff])->pcb$l_pid,((struct _pcb *)vec[ipid&0xffff])->pcb$l_epid);
	else
	  printk(KERN_EMERG "panic? no vec ipid %x %x %x %x\n",pid,ipid,vec[ipid&0xffff],0);
      }
      return ipid;
  }
  if ((pid&0xfffe0000))
    return 0;
  return 0;
  p = find_process_by_pid(pid); // linux pid compatibility? may bug...
  if (p) return p->pcb$l_pid;
  return 0;
}

int exe$ipid_to_epid(unsigned long pid) {
  int i;
  unsigned long *vec=sch$gl_pcbvec;
  unsigned long *seq=sch$gl_seqvec;
  long shift = process_bit_shift();
  long mask = process_index_mask();
  long seqno = seq[pid&0xffff]-1;
  return ((csid&0xff)<<21) | (seqno<<shift) | (pid&0xffff);
}

int exe$a_pid_to_ipid(unsigned long pid) {// remove this
  int i;
  unsigned long *vec=sch$gl_pcbvec;
  unsigned long *seq=sch$gl_seqvec;
  long shift = process_bit_shift();
  long mask = process_index_mask();
  long seqno = seq[pid&0xffff]-1;
  return ((csid&0xff)<<21) | (seqno<<shift) | (pid&0xffff);
}

#if 0
long fixi=0;
long fixs[1024];
#endif

/**
   \brief modifies the stack so that it will call exe$wait again.
*/

void fixup_hib_pc(void * dummy) {
#ifdef __i386__
  char ** addr = dummy + 0x28;
  (*addr)-=7;
#else
  char ** addr = dummy + 0xa0;
#if 0
  fixs[fixi++]=addr;
  fixs[fixi++]=*addr;
  if(fixi>1000)fixi=0;
#endif
  (*addr)-=9;
#endif
}

/**
   \brief set process in hibernate state - see 5.2 13.3.1
*/

asmlinkage int exe$hiber(long dummy) {
  /** some tricks for the return stack */
#ifdef __x86_64__
  __asm__ __volatile__ ("movq %%rsp,%0; ":"=r" (dummy) );
#endif
  /** spinlock sched */
  struct _pcb * p=current;
  vmslock(&SPIN_SCHED,IPL$_SCHED);
#if 0
  if (p->pcb$l_sts & PCB$M_WAKEPEN) {
    p->pcb$l_sts&=~PCB$M_WAKEPEN;
  }
#endif

  /** test and clear, bbcci equivalent, on wakepen bit */
  if (test_and_clear_bit(12,&p->pcb$l_sts)) { // PCB$M_WAKEPEN 0x1000 12 bits
    /** if wake request preceding */
    /** release spin */
    vmsunlock(&SPIN_SCHED,IPL$_ASTDEL);
    /** set ipl 0 ? */
    setipl(0);
    return SS$_NORMAL;
  }
  /* cwps stuff not yet */
  /** otherwise */
  /** some tricks for the return stack */
#ifdef __i386__
  fixup_hib_pc(&dummy);
#else
  fixup_hib_pc(dummy-0x10/*-0x30*/);
#endif
  /** go into hibernate wait */
  int ret = sch$wait(p,sch$gq_hibwq);
  setipl(0); // unstandard, but sch$wait might leave at ipl 8
  return ret;
}

/**
   \brief locating the process and checking privileges - see 5.2 13.1.2
   \param p pcb
   \param pidadr epid
   \param prcnam process name
   \param retipid return val ipid
   \param retepid return val epid
   \details with no cwps
*/

   /** return params not as specified */
int exe$nampid(struct _pcb *p, unsigned long *pidadr, void *prcnam, struct _pcb ** retpcb, unsigned long * retipid, unsigned long *retepid) {
  int sts = SS$_NORMAL;
  int ipid = 0;
  unsigned long *vec=sch$gl_pcbvec;
  /** determine if requesting process is target process - MISSING */
  /** if so do priv checks - MISSING partially */
  /** sched spinlock, and on success do not unlock */
  vmslock(&SPIN_SCHED,IPL$_SCHED);
  *retpcb=0;
  *retipid=0;
  *retepid=0;
  if (pidadr==0 && prcnam==0) {
    *retipid=p->pcb$l_pid;
    *retepid=p->pcb$l_epid;
    *retpcb=p;
    return SS$_NORMAL;
  }
  /** try to locate using the epid */
  if (pidadr && *pidadr) {
    ipid=exe$epid_to_ipid(*pidadr);
    /** indications about remote pcb, give REMOTE_PROC, TODO reimplement */
    if (ipid==0 && is_cluster_on())
      return SS$_REMOTE_PROC;
    /** if unknown node, return NONEXPR - MISSING */
    if (ipid == 0)
      goto not_found;
    /** check privs for examining or modifying process */
    sts = exe$process_check_priv (vec[ipid&0xffff]);
    if ((sts & 1) == 0)
      goto error;
    *retipid=ipid;
    *retpcb=vec[ipid&0xffff];
    *retepid=*pidadr;
    return SS$_NORMAL;
  }
  /** try to locate using the process name */
  if (prcnam) {
    int i;
    struct dsc$descriptor * d = prcnam;
    // compare stuff etc
    /** indications about remote process, give REMOTE_PROC, TODO reimplement */
    /** if process name indicates unknown node, return NOSUCHNODE - MISSING */
    /** if process name uses incorrectly formated nodename, return IVLOGNAM - MISSING */
    for (i=2;i<MAXPROCESSCNT;i++) {
      if (vec[i]!=0) { // change to nullpcb later
	struct _pcb * p=vec[i];
	if (strncmp(d->dsc$a_pointer,p->pcb$t_lname,d->dsc$w_length)==0) {
	  /** check privs for examining or modifying process */
	  sts = exe$process_check_priv (vec[ipid&0xffff]);
	  if ((sts & 1) == 0)
	    goto error;
	  *retipid=vec[i];
	  *retpcb=p;
	  return SS$_NORMAL;
	}
      }
      // not clusterable yet?
    }
    goto not_found;
    return SS$_NORMAL;
  }
  /* should not get here */
  /** return NOPRIV - MISSING mostly */
 not_found:
  vmsunlock(&SPIN_SCHED,IPL$_ASTDEL);
  return 0;
 error:
  vmsunlock(&SPIN_SCHED,IPL$_ASTDEL);
  return sts;
}

#if 0
/* return params not as specified */
void * exe$nampid2(struct _pcb *p, unsigned long *pidadr, void *prcnam) {
  /* sched spinlock */
  vmslock(&SPIN_SCHED,IPL$_SCHED);
  if (pidadr==0 && prcnam==0) {
    return p;
  }
  if (pidadr) {
    struct _pcb * tmp, **htable = &pidhash[pid_hashfn(*pidadr)];
    //printk("bef for\n");
    for(tmp = *htable; tmp && tmp->pid != *pidadr; tmp = tmp->pidhash_next) ;
    //printk("aft for\n");
    return tmp;
  }
  if (prcnam) {
    /* not yet */
    return;
  }
  /* should not get here */
  vmsunlock(&SPIN_SCHED,IPL$_ASTDEL);
  return 0;
}
#endif

/**
   \brief wake system service - see 5.2 13.3.1
*/

asmlinkage int exe$wake(unsigned long *pidadr, void *prcnam) {
  struct _pcb * retpcb, *p;
  unsigned long retipid, retepid;
  /** invoke nampid translation */
  int sts=exe$nampid(current,pidadr,prcnam,&retpcb,&retipid,&retepid);
  p=retpcb;
  if (p) {
    /** if found */
    /** invoke wake */
    sch$wake(p->pcb$l_pid);
    /** unlock spin sched */
    vmsunlock(&SPIN_SCHED,IPL$_ASTDEL);
    return;
  }
  /** cwps stuff - MISSING */
  setipl(0);
}

/**
   \brief suspend process MISSING - see 5.2 13.3.2
*/

asmlinkage int exe$suspnd(unsigned int *pidadr, void *prcnam, unsigned int flags ) {
  /** MISSING */
}

/**
   \brief resume suspended process MISSING - see 5.2 13.3.2.2
*/

asmlinkage int exe$resume (unsigned int *pidadr, void *prcnam) {
  struct _pcb * retpcb, *p;
  unsigned long retipid, retepid;
  /** invoke nampid translation */
  int sts=exe$nampid(current,pidadr,prcnam,&retpcb,&retipid,&retepid);
  p=retpcb;
  /** unlock spin */
  vmsunlock(&SPIN_SCHED,0);
  if (p) {
    /** if found */
    /** report scheduling event */
    sch$rse(p,PRI$_RESAVL,EVT$_RESUME);
    return;
  }
  /** cwps - MISSING */
  return SS$_NORMAL;
}

//asmlinkage int sys_$setpri(unsigned int *pidadr, void *prcnam, unsigned int pri, unsigned int *prvpri, unsigned int*pol, unsigned int *prvpol) {

/**
   \brief set process name - see 5.2 13.4.3
*/

asmlinkage int exe$setprn(struct dsc$descriptor *s) {
  struct _pcb *p=0;

  if (!p) p=current;
  p=current;
  /** put name into lname field */
  strncpy(p->pcb$t_lname,s->dsc$a_pointer,s->dsc$w_length);
  p->pcb$t_lname[s->dsc$w_length]=0;
  return SS$_NORMAL;
}

inline void *find_task_by_pid(int pid) {
  int tsk;
  tsk=exe$ipid_to_pcb(pid);
  if (tsk) return tsk;
  long *x=&tsk;
  printk(KERN_EMERG "FIND %d %x %x %x %x %x %x %x %x %x %x\n",pid,pid,x[1],x[2],x[3],x[4],x[5],x[6],x[7],x[8]);
  return 0;
}
