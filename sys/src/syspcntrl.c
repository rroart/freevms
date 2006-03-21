// $Id$
// $Locker$

// Author. Roar Thronæs.

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

/* Author: Roar Thronæs */

unsigned long maxprocesscnt=MAXPROCESSCNT;

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

void fixup_hib_pc(void * dummy) {
#ifdef __i386__
  char ** addr = dummy + 0x28;
  (*addr)-=7;
#else
  char ** addr = dummy + 0xa0;
  (*addr)-=9;
#endif
}

asmlinkage int exe$hiber(long dummy) {
#ifdef __x86_64__
  __asm__ __volatile__ ("movq %%rsp,%0; ":"=r" (dummy) );
#endif
  /* spinlock sched */
  struct _pcb * p=current;
  vmslock(&SPIN_SCHED,IPL$_SCHED);
#if 0
  if (p->pcb$l_sts & PCB$M_WAKEPEN) {
    p->pcb$l_sts&=~PCB$M_WAKEPEN;
  }
#endif
  /* release spin */
  /* set ipl 0 ? */

  if (test_and_clear_bit(12,&p->pcb$l_sts)) { // PCB$M_WAKEPEN 0x1000 12 bits
    vmsunlock(&SPIN_SCHED,IPL$_ASTDEL);
    setipl(0);
    return SS$_NORMAL;
  }
  /* cwps stuff not yet */
#ifdef __i386__
  fixup_hib_pc(&dummy);
#else
  fixup_hib_pc(dummy-0x30);
#endif
  int ret = sch$wait(p,sch$gq_hibwq);
  setipl(0); // unstandard, but sch$wait might leave at ipl 8
  return ret;
}

/* return params not as specified */
int exe$nampid(struct _pcb *p, unsigned long *pidadr, void *prcnam, struct _pcb ** retpcb, unsigned long * retipid, unsigned long *retepid) {
  int ipid;
  unsigned long *vec=sch$gl_pcbvec;
  /* sched spinlock */
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
  if (pidadr && *pidadr) {
    ipid=exe$epid_to_ipid(*pidadr);
    if (ipid==0 && is_cluster_on())
      return SS$_REMOTE_PROC;
    *retipid=ipid;
    *retpcb=vec[ipid&0xffff];
    *retepid=*pidadr;
    return SS$_NORMAL;
  }
  if (prcnam) {
    int i;
    struct dsc$descriptor * d = prcnam;
    // compare stuff etc
    for (i=2;i<MAXPROCESSCNT;i++) {
      if (vec[i]!=0) { // change to nullpcb later
	struct _pcb * p=vec[i];
	if (strncmp(d->dsc$a_pointer,p->pcb$t_lname,d->dsc$w_length)==0) {
	  *retipid=vec[i];
	  *retpcb=p;
	  return SS$_NORMAL;
	}
      }
      // not clusterable yet?
    }
    return SS$_NORMAL;
  }
  /* should not get here */
  vmsunlock(&SPIN_SCHED,IPL$_ASTDEL);
  return 0;
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

asmlinkage int exe$wake(unsigned long *pidadr, void *prcnam) {
  struct _pcb * retpcb, *p;
  unsigned long retipid, retepid;
  int sts=exe$nampid(current,pidadr,prcnam,&retpcb,&retipid,&retepid);
  p=retpcb;
  if (p) {
    sch$wake(p->pcb$l_pid);
    vmsunlock(&SPIN_SCHED,IPL$_ASTDEL);
    return;
  }
  /* no cwps stuff yet */
  setipl(0);
}

asmlinkage int exe$suspnd(unsigned int *pidadr, void *prcnam, unsigned int flags ) {

}

asmlinkage int exe$resume (unsigned int *pidadr, void *prcnam) {
  struct _pcb * retpcb, *p;
  unsigned long retipid, retepid;
  int sts=exe$nampid(current,pidadr,prcnam,&retpcb,&retipid,&retepid);
  p=retpcb;
  vmsunlock(&SPIN_SCHED,0);
  if (p) {
    sch$rse(p,PRI$_RESAVL,EVT$_RESUME);
    return;
  }
  /* no cwps here either */
  return SS$_NORMAL;
}

//asmlinkage int sys_$setpri(unsigned int *pidadr, void *prcnam, unsigned int pri, unsigned int *prvpri, unsigned int*pol, unsigned int *prvpol) {
asmlinkage int exe$setprn(struct dsc$descriptor *s) {
  struct _pcb *p=0;

  if (!p) p=current;
  p=current;
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
