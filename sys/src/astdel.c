// $Id$
// $Locker$

// Author. Roar Thronæs.
// Author. Roger Tucker.

#include <linux/linkage.h>
#include <system_data_cells.h>
#include <ssdef.h>
#include <pridef.h>
#include <acbdef.h>
#include <evtdef.h>
#include <statedef.h>
#include <queue.h>
#include <ipldef.h>
#include <ipl.h>
#include <linux/sched.h>
#include <linux/smp.h>

#undef OLDAST
#define OLDAST

#undef ASTDEBUG
#define ASTDEBUG

extern int mydebug5;

void sch$newlvl(struct _pcb *p);

static int checkq(struct _acb * head) {
  struct _acb * tmp = head->acb$l_astqfl;
  signed char tmpmode1, tmpmode2;
  
  while (tmp!=head && tmp->acb$l_astqfl!=head) {
    struct _acb * next = tmp->acb$l_astqfl;
    tmpmode1 = tmp->acb$b_rmod & 0x93;
    tmpmode2 = next->acb$b_rmod & 0x93;
    if (tmpmode1>tmpmode2)
      panic("acb %x %x %x %x\n", tmp, next, tmpmode1, tmpmode2);
    tmp = next;
  }

}

int sch$qast(unsigned long pid, int priclass, struct _acb * a) {
  int savipl;
  int status=SS$_NORMAL;
  int kernelmode;
  /* lock */
  savipl=getipl();
  setipl(IPL$_SYNCH);
  spin_lock(&SPIN_SCHED);
  struct _pcb * p=exe$ipid_to_pcb(pid);
  if (!p) {
    spin_unlock(&SPIN_SCHED);
    setipl(savipl);
    return SS$_NONEXPR;
  }
#ifdef ASTDEBUG
  checkq(&p->pcb$l_astqfl);
#endif
  struct _acb * tmp =  p->pcb$l_astqbl;
  if (!aqempty(&p->pcb$l_astqfl)) {
    signed char mode = a->acb$b_rmod & 0x93;
    struct _acb * head = &p->pcb$l_astqfl;
    tmp = head->acb$l_astqfl;
    signed char tmpmode = tmp->acb$b_rmod & 0x93;; 

    while (tmp!=head && mode>=tmpmode) {
      tmp=tmp->acb$l_astqfl;
      tmpmode = tmp->acb$b_rmod & 0x93;
    }
    tmp = tmp->acb$l_astqbl;
  }
  insque(a,tmp);
#ifdef ASTDEBUG
  checkq(&p->pcb$l_astqfl);
#endif
  if ((a->acb$b_rmod & ACB$M_KAST)==0)
    kernelmode=a->acb$b_rmod & 3;
  else
    kernelmode=0;
  p->phd$b_astlvl=kernelmode;
  /* just simple insert , no pris yet */
  //printk("bef rse\n");
  if (p->pcb$w_state!=SCH$C_CUR)
    status=sch$rse(p, priclass, EVT$_AST);
  else {
    struct _cpu * cpu=smp$gl_cpu_data[smp_processor_id()];
    struct _pcb * curp=ctl$gl_pcb;
    // if (p==curp) etc // smp not enabled
    p->pr_astlvl=p->phd$b_astlvl;
  }
  //printk("aft rse\n");
  /* unlock */
  spin_unlock(&SPIN_SCHED);
  setipl(savipl);
  return status;
}

printast(struct _acb * acb) {
  printk("acb %x %x %x %x %x\n",acb,acb->acb$l_pid,acb->acb$l_ast,acb->acb$l_astprm,acb->acb$l_kast);
}

/*
  ecx scratch
  ebx acb
  original call stack: esi
  0xac:
  0: ret eip
  4: ret cs
  8: flags
  c: ret sp
  10: ret ss

  add to kernel stack: edi
  exe$astdel
  user cs
  fl
  new user stack
  user ss

  add to new user stack: edx
  acb
  kernel routine return
  kernel cs
  fl
  new kernel stack
  kernel ss
*/

int exe$astdel_old() {
  struct _acb * acb;
  __asm__ ( "movl 0x0(%%esp),%%eax\n\t" :"=a" (acb) );
  if(acb->acb$l_ast) acb->acb$l_ast(acb->acb$l_astprm); /* ? */
  sys$clrast();
  __asm__ __volatile__(
		       "addl $0x4,%esp\n\t"
		       "iret\n\t" // or myrei?
		       );
}

int exe$astdel_wrap(struct _pcb * p, struct _acb * acb) {
  __asm__ __volatile__(
		       "movl 0x8(%esp),%ebx\n\t" // acb
		       "movl %esp,%esi\n\t"
		       "addl $0xac,%esi\n\t" // get original call stack
		       "movl 0xc(%esi),%edx\n\t" // get user stack
		       "addl $-0x18,%edx\n\t" // get new user stack
		       "movl %esp,%edi\n\t"
		       "addl $-0x14,%edi\n\t" // get new kernel stack
		       "\n\t"
#if 0
		       "movl 0x10(%ebx),%ecx\n\t" // get acb$l_ast
		       "movl %ecx,0x0(%edi)\n\t" // put acb$l_ast on kstack
#else
		       "movl $exe$astdel,0x0(%edi)\n\t" // put astdel on kstack
#endif
		       "movl 0x4(%esi),%ecx\n\t" // get user cs
		       "movl %ecx,0x4(%edi)\n\t" // put user cs on kstack
		       "movl 0x8(%esi),%ecx\n\t" // get user flags
		       "movl %ecx,0x8(%edi)\n\t" // put user flags on kstack
		       "movl %edx,0xc(%edi)\n\t" // put new ustack on kstack
		       "movl 0x10(%esi),%ecx\n\t" // get user ss
		       "movl %ecx,0x10(%edi)\n\t" // put ss on kstack
		       "\n\t"
#if 0
		       "movl 0x14(%ebx),%ecx\n\t" // get acb$l_astprm
		       "movl %ecx,0x0(%edx)\n\t" // put acb$l_astprm on ustack
#else
		       "movl %ebx,0x0(%edx)\n\t" // put acb as parm on ustack
#endif
		       "movl 0x0(%esp),%ecx\n\t" // get return eip
		       "movl %ecx,0x4(%edx)\n\t" // put return eip on ustack
		       "movl $0x0,0x8(%edx)\n\t"
		       "movl %cs,0x8(%edx)\n\t"  // put kernel cs on ustack 
		       "movl 0x8(%esi),%ecx\n\t" // get user flags
		       "movl %ecx,0xc(%edx)\n\t" // put user flags on ustack
		       "movl %edi,0x10(%edx)\n\t" // put new kstack on ustack
		       "movl $0x0,0x14(%edx)\n\t"
		       "movl %ss,0x14(%edx)\n\t" // put ss on ustack
		       "\n\t"
		       "addl $-0x14,%esp\n\t" // set new kernel stack
		       "iret\n\t" // or myrei?
#if 0
		       "movl 0x20(%esp),%ebx\n\t"
		       "movl 0x10(%ebx),%edx\n\t"
		       "movl 0x14(%ebx),%ebx\n\t"
		       "pushl %ebx\n\t"
		       "call *%edx\n\t"
		       "popl %ebx\n\t"
#endif
);
}

int exe$astdel_wrap2(struct _pcb * p, struct _acb * acb) {
  __asm__ __volatile__(
		       "pushl %ebp\n\t"
		       "pushl %ebx\n\t"
                       "pushl %ecx\n\t"
                       "pushl %edx\n\t"
                       "pushl %edi\n\t"
                       "pushl %esi\n\t"
		       "pushl 0x20(%esp)\n\t"
		       "pushl 0x20(%esp)\n\t"
		       "call exe$astdel_wrap2\n\t"
		       "popl %esi\n\t"
		       "popl %esi\n\t"
		       "popl %esi\n\t"
		       "popl %edi\n\t"
		       "popl %edx\n\t"
		       "popl %ecx\n\t"
		       "popl %ebx\n\t"
		       "popl %ebp\n\t"
		       );
}

/*
  ecx scratch
  original call stack: esi
  0xb8:
  0: original ret eip
  4: ret cs
  8: flags
  c: ret sp
  10: ret ss

  change kernel stack: edi
  0: ret eip to exe$astdel

  add to new user stack: edx
  0: ast
  4: astprm
  8: original eip
*/

int exe$astdel() {
#if 0
  long (*ast)();
  long astprm;
  __asm__ ( "movl 0x0(%%esp),%%eax\n\t" :"=a" (acb) );
  if(ast) ast(astprm); /* ? */
  sys$clrast();
#endif
  __asm__ __volatile__(
		       "pushfl\n\t"
		       "pushl %eax\n\t"
		       "pushl %ebp\n\t"
		       "pushl %ebx\n\t"
                       "pushl %ecx\n\t"
                       "pushl %edx\n\t"
                       "pushl %edi\n\t"
                       "pushl %esi\n\t"
		       "pushl 0x24(%esp)\n\t"
		       "call *0x24(%esp)\n\t"
		       "addl $0x4,%esp\n\t"
		       "call sys$clrast\n\t"
		       "movl 0x1c(%esp),%esi\n\t"
		       "movl %esi,0x20(%esp)\n\t"
		       "movl %esi,0x24(%esp)\n\t"
		       "popl %esi\n\t"
		       "popl %edi\n\t"
		       "popl %edx\n\t"
		       "popl %ecx\n\t"
		       "popl %ebx\n\t"
		       "popl %ebp\n\t"
		       "popl %eax\n\t"
		       "popfl\n\t"
		       "popfl\n\t"
		       "popfl\n\t"
		       );
}

int exe$astdel_prep2(long stack, long ast, long astprm) {
  __asm__ __volatile__(
		       "movl %esp,%esi\n\t"
		       "addl $0xb8,%esi\n\t" // get original call stack
		       "movl 0xc(%esi),%edx\n\t" // get user stack
		       "addl $-0xc,%edx\n\t" // get new user stack
		       "movl %esi,%edi\n\t" // get kernel stack
		       "\n\t"
		       "movl 0x8(%esp),%ecx\n\t" // get ast
		       "movl %ecx,0x0(%edx)\n\t" // put ast as parm on ustack
		       "movl 0xc(%esp),%ecx\n\t" // get astprm
		       "movl %ecx,0x4(%edx)\n\t" // put astprm on ustack
		       "movl 0x0(%esi),%ecx\n\t" // get return eip
		       "movl %ecx,0x8(%edx)\n\t" // put return eip on ustack
		       "\n\t"
		       "movl $exe$astdel,0x0(%edi)\n\t" // put astdel on kstack
		       "movl %edx,0xc(%edi)\n\t" // put new ustack on kstack
);
}

int exe$astdel_prep(long a, long b, long c) {
  __asm__ __volatile__(
		       "pushl %ebp\n\t"
		       "pushl %ebx\n\t"
                       "pushl %ecx\n\t"
                       "pushl %edx\n\t"
                       "pushl %edi\n\t"
                       "pushl %esi\n\t"
		       "pushl 0x24(%esp)\n\t"
		       "pushl 0x24(%esp)\n\t"
		       "pushl 0x24(%esp)\n\t"
		       "call exe$astdel_prep2\n\t"
		       "popl %esi\n\t"
		       "popl %esi\n\t"
		       "popl %esi\n\t"
		       "popl %esi\n\t"
		       "popl %edi\n\t"
		       "popl %edx\n\t"
		       "popl %ecx\n\t"
		       "popl %ebx\n\t"
		       "popl %ebp\n\t"
		       );
}

int astdeb=0;
#ifdef ASTDEBUG
int lastast=0;
int lastastprm=0;
int lastacb=0;
int myacbi=0;
long myacbs[1024];
#endif

asmlinkage void sch$astdel(/*int dummy*/) {
  struct _cpu * cpu=smp$gl_cpu_data[smp_processor_id()];
  struct _pcb * p=ctl$gl_pcb;
  struct _acb * dummy = 0, *acb;

  /*lock*/
  if (intr_blocked(IPL$_ASTDEL))
    return;

  regtrap(REG_INTR, IPL$_ASTDEL);

  spin_lock(&SPIN_SCHED);
#ifdef ASTDEBUG
  myacbs[myacbi++]=0;
  myacbs[myacbi++]=0x77777777;
  myacbs[myacbi++]=0;
  myacbs[myacbi++]=0x77777777;
#endif
 more:
#ifdef ASTDEBUG
  checkq(&p->pcb$l_astqfl);
#endif
  setipl(IPL$_SYNCH); // also IPL$_SCHED

  /* { int i;
     //printk("here ast\n");
     for (i=0; i<1000000; i++) ;
     } */
  if (aqempty(&p->pcb$l_astqfl)) {
    sch$newlvl(p);
    spin_unlock(&SPIN_SCHED);
    return;
  }
  /* { int i,j;
     //printk("here ast2 %x %x %x\n",p->pid,p->pcb$l_astqfl,&p->pcb$l_astqfl);
     for (j=0; j<20; j++) for (i=0; i<1000000000; i++) ;
     } */
  acb=remque(p->pcb$l_astqfl,dummy);
#ifdef ASTDEBUG
  myacbs[myacbi++]=acb;
  myacbs[myacbi++]=acb->acb$l_ast;
  myacbs[myacbi++]=acb->acb$l_astprm;
  myacbs[myacbi++]=acb->acb$b_rmod;
  if (acb->acb$b_rmod&3)
    printk("A %x %x\n",acb,acb->acb$b_rmod);
  if (myacbi>1000)
    myacbi=0;
  lastacb=acb;
  lastast=acb->acb$l_ast;
  lastastprm=acb->acb$l_astprm;
#endif
  //printk("here ast2 %x %x %x %x\n",p->pid,p->pcb$l_astqfl,&p->pcb$l_astqfl,acb);
#ifdef __i386__
  if (0 && astdeb /*&& ( ((unsigned long)acb->acb$l_ast)>0xd0000000 || ((unsigned long)acb->acb$l_kast)>0xd0000000)*/)   printast(acb);
  //printk(" a %x %x ",p->pcb$l_astqfl,&p->pcb$l_astqfl);
#endif
  //  printast(acb);
  //  mydebug5=1;
  //  printk(KERN_EMERG "astdel %x\n",acb);
  if (acb->acb$b_rmod & ACB$M_KAST) {
    acb->acb$b_rmod&=~ACB$M_KAST;
    /* unlock */
    //printk("astdel1 %x \n",acb->acb$l_kast);
    setipl(IPL$_ASTDEL);
    //p->pcb$b_astact=1;
    if (((unsigned long)acb->acb$l_kast<0x80000000)||((unsigned long)acb->acb$l_kast>0xb0000000)) {
      int i;
      printk("kast %x\n",acb->acb$l_kast);
      for(i=0;i<2000000000;i++) ;
    }
#ifdef __i386__
    if (0 && astdeb)
      printk(" a %x ",acb->acb$l_kast);
  if (0 && astdeb)
    printk(" a %x",*(unsigned long*)acb->acb$l_kast);
#endif
#ifdef __i386__
  //      printk("a1 ");
#endif
    acb->acb$l_kast(acb);
#ifdef __i386__
    //      printk("a2 ");
#endif
    //p->pcb$b_astact=0;
    // do not do this? if ((acb->acb$b_rmod&ACB$M_NODELETE)==0) kfree(acb);
    goto more;
  }
  //printk("astdel2 %x %x \n",acb->acb$l_ast,acb->acb$l_astprm);
  if (p->pcb$b_asten!=15 || p->pcb$b_astact) { // 15 because no modes yet
#ifdef ASTDEBUG
    myacbs[myacbi++]=-1;
    myacbs[myacbi++]=acb;
    myacbs[myacbi++]=acb->acb$l_ast;
    myacbs[myacbi++]=acb->acb$l_astprm;
#endif
    insque(acb,&p->pcb$l_astqfl);
    p->phd$b_astlvl=p->pr_astlvl=(acb->acb$b_rmod & 3) + 1;
    spin_unlock(&SPIN_SCHED);
    return;
  }
  setipl(IPL$_ASTDEL);
  p->pcb$b_astact=0; // 1; wait with this until we get modes
  setipl(0); // for kernel mode, I think. everything is in kernelmode yet.
  if (((unsigned long)acb->acb$l_ast<0x80000000)&&((unsigned long)acb->acb$l_ast>0xb0000000)) {
    int i;
    printk("kast %x\n",acb->acb$l_ast);
    for(i=0;i<2000000000;i++) ;
  }
#ifdef __i386__
  if (0 && astdeb)
    printk(" a %x",acb->acb$l_ast);
  if (0 && astdeb)
    printk(" a %x",*(unsigned long*)acb->acb$l_ast);
#endif
#ifdef __i386__
  //  printk("a3 ");
#endif
  if (acb->acb$b_rmod&ACB$M_PKAST) {
    acb->acb$b_rmod&=~ACB$M_PKAST;
    if(acb->acb$l_kast) acb->acb$l_kast(acb->acb$l_astprm); /* ? */
  }
#ifdef __arch_um__
  if (((unsigned long)acb->acb$l_ast<0x80000000)) {
    // funny, this printk resulted in tracing too
    // printk("no user ast, avoiding tracing against myself. ast astprm pid %x %x %x %x\n",acb->acb$l_ast,acb->acb$l_astprm,p->pcb$l_pid,p->pid);
  } else {
    if(acb->acb$l_ast) acb->acb$l_ast(acb->acb$l_astprm); /* ? */
  }
#else
  long (*ast)() = acb->acb$l_ast;
  long astprm = acb->acb$l_astprm;
  int rmod = acb->acb$b_rmod;
  if ((acb->acb$b_rmod&ACB$M_NODELETE)==0) kfree(acb);
#ifndef OLDAST
  if (ast && (((long)ast)&0x80000000)==0)/* not yet: (rmod&3) */ { // workaround
    int sts = exe$astdel_prep(0/*&dummy*/,ast,astprm);
  } else {
    if(ast) ast(astprm); /* ? */
    sch$newlvl(p);
  }
#else
  if(ast) ast(astprm); /* ? */
#endif
#endif
#ifdef __i386__
  //      printk("a4 ");
#endif
  p->pcb$b_astact=0;
  /*unlock*/
#ifdef OLDAST
  goto more;
#endif
}

void sch$newlvl(struct _pcb *p) {
  int newlvl;
  int oldipl=getipl();

  spin_lock(&SPIN_SCHED);
  setipl(IPL$_SYNCH);

  if (aqempty(p->pcb$l_astqfl))
    newlvl=4;
  else {
    if(p->pcb$l_astqfl->acb$b_rmod & ACB$M_KAST)
      newlvl=p->pcb$l_astqfl->acb$b_rmod & 3;
    else
      newlvl=0; // was: p->phd$b_astlvl; /* ? */
  }
    
  p->phd$b_astlvl=newlvl;
  p->pr_astlvl=newlvl;
  spin_unlock(&SPIN_SCHED);
  setipl(oldipl);
}


