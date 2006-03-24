// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<cpudef.h>
#include<ipldef.h>
#include<ipl.h>
#include<system_data_cells.h>
#include<system_service_setup.h>
#include<linux/kernel.h>
#include<linux/smp.h>
#include<asm/hw_irq.h>
#include<linux/sched.h>
#include<asm/current.h>
#include<queue.h>
long inline mycli(void);

int in_atomic=0;
unsigned long prev1[20];

extern int timer_on;

static mydebugi = 0;  // should have no printk in a non-interruptable zone

#define IPL_DEBUG
#undef IPL_DEBUG

#ifdef IPL_DEBUG
static long stk[1024];
#endif

inline asmlinkage void pushpsl(void);
void inline mysti(int long);
asmlinkage void myrei (void);

inline asmlinkage void setpsli(void) {
  ctl$gl_pcb->psl_intr=1;
}

inline void setpslmod(int mod) {
  ctl$gl_pcb->psl_cur_mod=ctl$gl_pcb->psl_prv_mod=mod&3; // check. pretend user mode
}

inline asmlinkage void pushpsli(void) {
  pushpsl();
#if 0
#ifdef __arch_um__
  current->psl_cur_mod=0;
  current->psl_prv_mod=0;
  current->psl_is=1;
  current->psl_ipl=22;
  smp$gl_cpu_data[0]->cpu$b_ipl=current->psl_ipl;
#endif
#endif
}

inline asmlinkage void pushpsl(void) {
  int this_cpu=smp_processor_id();
  //  if (current->pslindex>1)
  //  panic("xyz\n");
  current->pslstk[current->pslindex++]=current->psl;
  if(current->pslindex<0 || current->pslindex>30) {
    printk("push %x %x %x\n",current->pcb$l_pid,current->pslindex,&this_cpu);
    insque(33333,44343);
    panic("push\n");
  }
#if 0
  show_trace(&this_cpu);
  show_trace_task(current);
  panic("no reason");
#endif
}

inline asmlinkage void poppsl(void) {
  int this_cpu=smp_processor_id();
  current->oldpsl=current->psl;
  current->psl=current->pslstk[--(current->pslindex)];
  smp$gl_cpu_data[this_cpu]->cpu$b_ipl=current->psl_ipl;
  if(current->pslindex<0 || current->pslindex>30) {
    printk("pop %x %x\n",current->pcb$l_pid,current->pslindex);
    if (0) { int dummy;
    unsigned char *i;
    int j=0;
    for(i=&dummy;j<400;i++,j++) printk("%x ",*i);
    }
#if 0
    show_trace(&this_cpu);
    show_trace_task(current);
#endif
    panic("pop\n");
  }
}

#if 0
inline int savipl(void) {
  return smp$gl_cpu_data[smp_processor_id()]->cpu$b_ipl;
}
#endif

inline int getipl() {
  return current->psl_ipl;
}

inline int __PAL_MFPR_IPL() {
  return getipl();
}

/* no smp yet */
inline void setipl(unsigned char i) {
  int this_cpu = smp_processor_id();
#ifdef IPL_DEBUG
  int this=ctl$gl_pcb->psl_ipl; 
#if 0
  if (i==2 && i<ctl$gl_pcb->psl_ipl)
    printk("ouch %x\n",ctl$gl_pcb->psl_ipl);
#endif
#endif
  if (current->psl_cur_mod)
    panic("set ipl with no kernel mode\n");
  smp$gl_cpu_data[this_cpu]->cpu$b_ipl=i;
  current->psl_ipl=i;
  // softints and others?
#ifdef IPL_DEBUG
  int addr=&this_cpu;
  if (i==0)
    memcpy((long)stk+0,addr,128);
  if (i==2)
    memcpy((long)stk+64*10,addr,128);
  if (i==8 && i!=this)
    memcpy((long)stk+64*12,addr,128);
#endif
}

/* no smp yet */
inline void setipl2(unsigned char i) {
  int this_cpu = smp_processor_id();
  smp$gl_cpu_data[this_cpu]->cpu$b_ipl=i;
  current->psl_ipl=i;
  // softints and others?
}

inline void __PAL_MTPR_IPL(unsigned char i) {
  setipl(i);
}

 inline void splx(void) {
  int this_cpu = smp_processor_id();
  int i, tmp;
  tmp=smp$gl_cpu_data[this_cpu]->cpu$b_ipl;
  smp$gl_cpu_data[this_cpu]->cpu$b_ipl=smp$gl_cpu_data[this_cpu]->previpl[smp$gl_cpu_data[this_cpu]->iplnr--];
  for(i=0;i<256;i++)
    if (smp$gl_cpu_data[this_cpu]->cpu$t_ipending[i].interrupt>=tmp) { ; }
  /*
    do the interrupt?
   */

  /* check and do asts */
}

 inline char spl(unsigned char new) {
  int this_cpu = smp_processor_id();
  if (new<=smp$gl_cpu_data[this_cpu]->cpu$b_ipl) {
    /* mark ipending */
    return 1;
  }
  smp$gl_cpu_data[this_cpu]->previpl[smp$gl_cpu_data[this_cpu]->iplnr++]=smp$gl_cpu_data[this_cpu]->cpu$b_ipl;
  smp$gl_cpu_data[this_cpu]->cpu$b_ipl=new;
  return 0;
}

inline int prespl_not(unsigned char new) {
  int this_cpu = smp_processor_id();
  printk("prespl: should not get here now\n");
  if (new<=smp$gl_cpu_data[this_cpu]->cpu$b_ipl) {
    /* mark ipending */
    return 1;
  }
  return 0;
}

inline void chm(char mode) {
  int this_cpu=smp_processor_id();
  int cur_mod=current->psl_cur_mod;
  int priv=min(mode,cur_mod);
  /* illegal from istack */
  /* different stacks not implemented either */
}

inline void regtrap(char type, char param) {
  /* remember to disable interrupt during this */
  int flag=mycli();
  int cpu=smp_processor_id();
  int saved_psl_is=current->psl_is;
  int saved_psl_ipl=current->psl_ipl;
  pushpsl(); /* should be at the end of this  */
  switch (type) {
  case REG_INTR:
    current->psl_cur_mod=0;
    current->psl_prv_mod=0;
    if (param>3)
      current->psl_is=1;
#if 0
    // default behaviour
    else
      current->psl_is=old isl
#endif
    /*  not fully implemented */
    current->psl_ipl=param;
    smp$gl_cpu_data[cpu]->cpu$b_ipl=current->psl_ipl;
    break;
    
  case REG_CHMX:
    current->psl_prv_mod=current->psl_cur_mod;
    current->psl_cur_mod=param;
    /*  not fully implemented */
    current->psl_is=saved_psl_is;
    current->psl_ipl=saved_psl_ipl;
    smp$gl_cpu_data[cpu]->cpu$b_ipl=current->psl_ipl;
    break;
    
  default:
    current->psl_prv_mod=current->psl_cur_mod;
    current->psl_cur_mod=0;
    // psl is from vector 1:0 is1ipl31 or both saved
    /*  not fully implemented */
    break;
  }
  // if is 0 ipr old psl curmod iprsp = sp, sp = is or curmod
  /*  not fully implemented */
  if (mydebugi>1) printk("bef %x %x ",param,smp$gl_cpu_data[cpu]->cpu$w_sisr);
  if (type==REG_INTR && param<16) {
    // enable interrupt
    smp$gl_cpu_data[cpu]->cpu$w_sisr&=~(1<<param);
  }
  if (mydebugi>1) printk("%x\n",smp$gl_cpu_data[cpu]->cpu$w_sisr);
#ifdef IPL_DEBUG
  int addr=&flag;
  memcpy((long)stk+64*param,addr,64);
#endif
  mysti(flag);
}

int block3=0;

inline char intr_blocked(unsigned char this) {
#if 0
  int flag=mycli();
  int this_cpu = smp_processor_id();
  struct _pcb * p=current;
  if (mydebugi>1) printk("bl %x %x %x %x\n",p->pcb$l_pid,this,smp$gl_cpu_data[this_cpu]->cpu$w_sisr,p->pslindex);
  if (this<=smp$gl_cpu_data[this_cpu]->cpu$b_ipl) {
    if (this<16) smp$gl_cpu_data[this_cpu]->cpu$w_sisr|=(1<<this);
    // if (mydebugi>0) printk("blocked %x %x\n",this,smp$gl_cpu_data[this_cpu]->cpu$b_ipl);
    //          { long long i;    for(i=10000000;i>0;i--) ; }
    if (p->pslindex>20)
      { long long i;    for(i=1;i!=0;i++) ; }
      //  panic("pslindex\n");
    block3++;
    //    if (this!=8) block3=0;
    if (block3>20)
      block3++;
    if (block3>80) {
      mydebugi=mydebugi;
      block3++;
    }
    if (block3>100) {
#if 0
      extern void show_trace_task(struct task_struct *tsk);
      extern int mydebug2, mydebug3, mydebug4, mydebug5, mydebug6;
      show_trace_task(p);
      { long long i;    for(i=1;i!=0;i++) ; }
      mydebug2=1;
      mydebug4=1;
      mydebug5=1;
      mydebug6=1;
#else
      mydebugi=1;
#ifdef __arch_um__
      setipl(0);  // a fix for an error of unknown origin
      printk("lockup fixed by setting ipl 0\n");
#endif
#endif
    }
    mysti(flag);
    p->psl_intr=0;
    return 1;
  }
  block3=0;
  p->psl_intr=1;
  mysti(flag);
  return 0;
#else
  int this_cpu = smp_processor_id();
  struct _pcb * p=current;
  if (mydebugi>1) printk("bl %x %x %x %x\n",p->pcb$l_pid,this,smp$gl_cpu_data[this_cpu]->cpu$w_sisr,p->pslindex);
  if (this<=smp$gl_cpu_data[this_cpu]->cpu$b_ipl) {
    if (this<16)
      panic("should not block\n");
    return 1;
  }
  return 0;
#endif
}

asmlinkage void do_sw_int(void) {
  int this_cpu = smp_processor_id();
  int i, j, sisr=smp$gl_cpu_data[this_cpu]->cpu$w_sisr;
  if (mydebugi>1) printk("swint2 %x %x %x\n",current->pcb$l_pid,smp$gl_cpu_data[this_cpu]->cpu$b_ipl,smp$gl_cpu_data[this_cpu]->cpu$w_sisr);
  for(i=15,j=0x8000;i>smp$gl_cpu_data[this_cpu]->cpu$b_ipl;i--,j=j>>1) 
    if (sisr & j) {
      if (mydebugi>0) printk("swint %x %x %x\n",current->pcb$l_pid,smp$gl_cpu_data[this_cpu]->cpu$b_ipl,smp$gl_cpu_data[this_cpu]->cpu$w_sisr);
      switch (i) {
	case 12:
	  /* IPC something */
	  break;
      case IPL$_IOLOCK11:
	SOFTINT_IOLOCK11_VECTOR;
	break;
      case IPL$_IOLOCK10:
	SOFTINT_IOLOCK10_VECTOR;
	break;
      case IPL$_IOLOCK9:
	SOFTINT_IOLOCK9_VECTOR;
	break;
      case IPL$_IOLOCK8:
	SOFTINT_IOLOCK8_VECTOR;
	break;
	/* some more iolock fork stuff? */
      case IPL$_TIMERFORK:
	SOFTINT_TIMERFORK_VECTOR;
	break;
      case IPL$_QUEUEAST:
	SOFTINT_QUEUEAST_VECTOR;
	break;
      case IPL$_IOPOST:
	SOFTINT_IOPOST_VECTOR;
	break;
      case IPL$_RESCHED:
	SOFTINT_RESCHED_VECTOR;
	break;
      case IPL$_ASTDEL:
	SOFTINT_ASTDEL_VECTOR;
	break;
      }
      if (mydebugi>1) printk("hereint\n");
    }
}

extern int in_sw_ast;

asmlinkage void myrei (void) {
  /* look at REI for this */
  int flag, this_cpu;
  struct _pcb *p=current;
#if 0
  if (!p->psl_intr) {
    p->psl_intr=1;
    return; // return if not interrupt did happen
  }
#endif
  flag=mycli();
  this_cpu=smp_processor_id();
  if (mydebugi>1) printk("bl %x %x %x\n",p->pcb$l_pid,smp$gl_cpu_data[this_cpu]->cpu$b_ipl,smp$gl_cpu_data[this_cpu]->cpu$w_sisr);
  if (mydebugi>1) printk("befpop %x %x ",p->pcb$l_pid,p->psl_ipl);
  poppsl();
  if (mydebugi>1) printk("%x\n",p->psl_ipl);
  if (p->psl_is==1 && p->psl_ipl==0) panic("is ipl\n");
  if (p->psl_ipl>0 && p->psl_cur_mod!=0) panic("ipl curmod\n");
  if (p->psl_prv_mod < p->psl_cur_mod) panic("prv curmod\n");
  if (p->psl_cur_mod < p->oldpsl_cur_mod) panic("rei to higher mode\n");
  if (p->psl_is==1 && p->oldpsl_is==0) panic("is stuff\n");
#ifdef IPL_DEBUG
  if (p->psl_ipl > p->oldpsl_ipl && p->psl_ipl==2 && p->oldpsl_ipl==0) {
    int i;
    for (i=0;i<64;i++) printk("%4x ",(unsigned long)stk[i]);
    printk("\n");
  }
#endif
  if (p->psl_ipl > p->oldpsl_ipl) panic("rei to higher ipl %x %x\n",p->psl_ipl , p->oldpsl_ipl);
    //sickinsque(0x10000000+p->psl_ipl,0x20000000+p->oldpsl_ipl);
  // save old sp oldpsl is and 4 oth
  // set new pc psl
  // if is 0 change stack
  setipl2(current->psl_ipl);
  //  if (!in_sw_ast) sw_ast();
  mysti(flag);
  sw_ast();
#ifdef __x86_64__
  do_sw_int();
#endif
  /* also needs some changing mode stacks */
}

#if (defined __i386__) || (defined __x86_64__)
void inline mysti(long flags) {
  in_atomic=0;
  if (flags) __sti();
  spin_unlock(SPIN_ATOMIC);
  //printk("mysti\n");
}

void sickinsque(void * entry, void * pred) {
  *(void **)entry=*(void **)pred;
  *(void **)(entry+4)=pred;
  *(void **)((*(void **)pred)+4)=entry;
  *(void **)pred=entry;
}

long inline mycli(void) {
  long flags, retval;
  //printk("mycli\n");
  spin_lock(SPIN_ATOMIC);
  __save_flags(flags);
  retval=flags&0x00000200; /* interrupt enable/disable flag */
  if (in_atomic && in_atomic==current->pcb$l_pid) {
#ifdef __i386__
    long long l;
    for(l=0;l<20;l++) printk("%x ",prev1[l]);
    printk("\n");
    sickinsque(0x10000000,0x20000000);
    __cli();
    for(l=0;l;l++) ;
#endif
    sickinsque(0x10000000,0x20000000);
    panic("test\n");
  }
  if (in_atomic && in_atomic!=current->pcb$l_pid) printk("halfpancimycli\n");
  __cli();
  in_atomic=current->pcb$l_pid;
  { unsigned long *l=&flags;
  int i;
  for(i=0;i<20;i++) prev1[i]=l[i];
  }
  return retval;
}
#endif

#ifdef __arch_um__
void inline mysti(int flags) { 
  in_atomic=0;
  if (flags) unblock_signals();
  spin_unlock(SPIN_ATOMIC);
}

int inline mycli(void) {
  int retval=timer_on;
  spin_lock(SPIN_ATOMIC);
  if (in_atomic && in_atomic==current->pcb$l_pid) panic("test\n");
  if (in_atomic && in_atomic!=current->pcb$l_pid) printk("halfpancimycli\n");
  block_signals();
  in_atomic=current->pcb$l_pid;
  return retval;
}
#endif
