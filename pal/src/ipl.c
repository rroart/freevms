#include"../../freevms/lib/src/cpudef.h"
#include"../../freevms/lib/src/ipldef.h"
#include"../../freevms/pal/src/ipl.h"
#include"../../freevms/sys/src/system_data_cells.h"
#include"../../freevms/lib/src/system_service_setup.h"
#include<linux/kernel.h>
#include<linux/smp.h>
#include<asm/hw_irq.h>
#include<linux/sched.h>
#include<asm/current.h>

static mydebugi = 1;

inline asmlinkage void pushpsl(void) {
  int this_cpu=smp_processor_id();
  current->pslstk[current->pslindex++]=current->psl;
  if(current->pslindex<0 || current->pslindex>16) {
    printk("push %x %x\n",current->pid,current->pslindex);
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
  if(current->pslindex<0 || current->pslindex>16) {
    printk("pop %x %x\n",current->pid,current->pslindex);
    if (0) { int dummy;
    unsigned char *i;
    int j=0;
    for(i=&dummy;j<400;i++,j++) printk("%x ",*i);
    }
    show_trace(&this_cpu);
    show_trace_task(current);
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

/* no smp yet */
inline void setipl(unsigned char i) {
  int this_cpu = smp_processor_id();
  smp$gl_cpu_data[this_cpu]->cpu$b_ipl=i;
  current->psl_ipl=i;
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
  int cpu=smp_processor_id();
  pushpsl(); /* should be at the end of this  */
  switch (type) {
  case REG_INTR:
    current->psl_cur_mod=0;
    current->psl_prv_mod=0;
    /*  not fully implemented */
    current->psl_ipl=param;
    smp$gl_cpu_data[cpu]->cpu$b_ipl=current->psl_ipl;
    break;
    
  case REG_CHMX:
    current->psl_prv_mod=current->psl_cur_mod;
    current->psl_cur_mod=param;
    /*  not fully implemented */
    break;
    
  default:
    current->psl_prv_mod=current->psl_cur_mod;
    current->psl_cur_mod=0;    
    /*  not fully implemented */
    break;
  }
  /*  not fully implemented */
  if (mydebugi>1) printk("bef %x %x ",param,smp$gl_cpu_data[cpu]->cpu$w_sisr);
  if (type==REG_INTR && param<16) {
    smp$gl_cpu_data[cpu]->cpu$w_sisr&=~(1<<param);
  }
  if (mydebugi>1) printk("%x\n",smp$gl_cpu_data[cpu]->cpu$w_sisr);
}

int block3=0;

inline char intr_blocked(unsigned char this) {
  int this_cpu = smp_processor_id();
  struct _pcb * p=current;
  if (mydebugi>1) printk("bl %x %x %x %x\n",p->pid,this,smp$gl_cpu_data[this_cpu]->cpu$w_sisr,p->pslindex);
  if (this<=smp$gl_cpu_data[this_cpu]->cpu$b_ipl) {
    if (this<16) smp$gl_cpu_data[this_cpu]->cpu$w_sisr|=(1<<this);
    if (mydebugi>0) printk("blocked %x %x\n",this,smp$gl_cpu_data[this_cpu]->cpu$b_ipl);
    //          { long long i;    for(i=10000000;i>0;i--) ; }
    if (p->pslindex>20)
      { long long i;    for(i=1;i!=0;i++) ; }
      //  panic("pslindex\n");
    block3++;
    //    if (this!=8) block3=0;
    if (block3>90) mydebugi=3;
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
      setipl(0);  // a fix for an error of unknown origin
      printk("lockup fixed by setting ipl 0\n");
#endif
    }
    pushpsl();
    return 1;
  }
  block3=0;
  return 0;
}

asmlinkage void do_sw_int(void) {
  int this_cpu = smp_processor_id();
  int i, j, sisr=smp$gl_cpu_data[this_cpu]->cpu$w_sisr;
  if (mydebugi>1) printk("swint2 %x %x %x\n",current->pid,smp$gl_cpu_data[this_cpu]->cpu$b_ipl,smp$gl_cpu_data[this_cpu]->cpu$w_sisr);
  for(i=15,j=0x8000;i>smp$gl_cpu_data[this_cpu]->cpu$b_ipl;i--,j=j>>1) 
    if (sisr & j) {
      if (mydebugi>0) printk("swint %x %x %x\n",current->pid,smp$gl_cpu_data[this_cpu]->cpu$b_ipl,smp$gl_cpu_data[this_cpu]->cpu$w_sisr);
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
	/* some more iolock fork stuff? */
      case IPL$_TIMER:
	SOFTINT_TIMER_VECTOR;
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
  struct _pcb *p=current;
  int this_cpu=smp_processor_id();
  if (mydebugi>1) printk("bl %x %x %x\n",p->pid,smp$gl_cpu_data[this_cpu]->cpu$b_ipl,smp$gl_cpu_data[this_cpu]->cpu$w_sisr);
  if (mydebugi>1) printk("befpop %x %x ",p->pid,p->psl_ipl);
  poppsl();
  if (mydebugi>1) printk("%x\n",p->psl_ipl);
  if (p->psl_is==1 && p->psl_ipl==0) panic("is ipl\n");
  if (p->psl_ipl>0 && p->psl_cur_mod!=0) panic("ipl curmod\n");
  if (p->psl_prv_mod < p->psl_cur_mod) panic("prv curmod\n");
  if (p->psl_cur_mod < p->oldpsl_cur_mod) panic("rei to higher mode\n");
  if (p->psl_is==1 && p->oldpsl_is==0) panic("is stuff\n");
  if (p->psl_ipl > p->oldpsl_ipl) panic("rei to higher ipl\n");
  setipl(current->psl_ipl);
  if (!in_sw_ast) sw_ast();
  do_sw_int();
  /* also needs some changing mode stacks */
}
