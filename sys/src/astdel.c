// $Id$
// $Locker$

// Author. Roar Thronæs.
// Author. Roger Tucker.

#include <linux/linkage.h>
#include <system_data_cells.h>
#include <ssdef.h>
#include <pridef.h>
#include <cpudef.h>
#include <acbdef.h>
#include <evtdef.h>
#include <statedef.h>
#include <queue.h>
#include <ipldef.h>
#include <ipl.h>
#include <exe_routines.h>
#include <sch_routines.h>
#include <misc_routines.h>
#include <smp_routines.h>
#include <starlet.h>
#include <linux/sched.h>
#include <linux/smp.h>
#include <linux/slab.h>
#include <internals.h>

#define OLDAST
#undef OLDAST

#if 0
#ifdef __x86_64__
#define OLDAST
#endif
#endif

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
  vmslock(&SPIN_SCHED,-1);
  struct _pcb * p=exe$ipid_to_pcb(pid);
  if (!p) {
    vmsunlock(&SPIN_SCHED,-1);
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
    sch$rse(p, priclass, EVT$_AST);
  else {
    struct _cpu * cpu=smp$gl_cpu_data[smp_processor_id()];
    struct _pcb * curp=ctl$gl_pcb;
    if (p==cpu->cpu$l_curpcb) // etc // smp not enabled
      p->pr_astlvl=p->phd$b_astlvl;
    else
#if 0
      smp_send_work(CPU$M_UPDASTSR, p->pcb$l_cpu_id);
#else
      p->pr_astlvl=p->phd$b_astlvl;
      // come to think of it, it did not matter, since we emulate
      // the astlvl in the pcb. ipint not needed, then. 
#endif
  }
  //printk("aft rse\n");
  /* unlock */
  vmsunlock(&SPIN_SCHED,-1);
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

#ifdef __i386__
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
		       "mov %cs,0x8(%edx)\n\t"  // put kernel cs on ustack 
		       "movl 0x8(%esi),%ecx\n\t" // get user flags
		       "movl %ecx,0xc(%edx)\n\t" // put user flags on ustack
		       "movl %edi,0x10(%edx)\n\t" // put new kstack on ustack
		       "movl $0x0,0x14(%edx)\n\t"
		       "mov %ss,0x14(%edx)\n\t" // put ss on ustack
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
  0xb8/bc? 68/6c from dummy a:
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
#if 0
		       "movl %esp,%esi\n\t"
		       "addl $0xb8,%esi\n\t" // get original call stack
#else
		       "movl 0x4(%esp),%esi\n\t"
		       "addl $0x6c,%esi\n\t" // get original call stack
#endif
		       "back_here2:\n\t"
		       "cmpl $0x10,0x4(%esi)\n\t" // come from kernel mode?
		       "je from_kernel_mode\n\t" // then go to other code
		       "cmpl $0x23,0x4(%esi)\n\t"
		       "je back_here\n\t"
		       "jmp panic\n\t"
		       "back_here:\n\t"
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
		       "ret\n\t"
		       "\n\t"
		       "from_kernel_mode:\n\t"
		       "addl $0x48,%esi\n\t" // next stack, via do_sw_int
		       "cmpl $0x23,0x4(%esi)\n\t"
		       "je back_here2\n\t"
		       "cmpl $0x10,0x4(%esi)\n\t"
		       "je back_here2\n\t"
		       "subl $0x48,%esi\n\t" // back
		       "addl $0x54,%esi\n\t" // next stack, via sw_ast
		       "cmpl $0x23,0x4(%esi)\n\t"
		       "je back_here2\n\t"
		       "cmpl $0x10,0x4(%esi)\n\t"
		       "je back_here2\n\t"
		       "subl $0x54,%esi\n\t" // back
		       "addl $0x6c,%esi\n\t" // next stack, via cli mycli
		       "jmp back_here2\n\t"
		       "ret\n\t"
);
}

long cstab[4] = { __KERNEL_CS, __EXECUTIVE_CS, __SUPERVISOR_CS, __USER_CS };
long sstab[4] = { __KERNEL_CS | 8, __EXECUTIVE_CS | 8, __SUPERVISOR_CS | 8, __USER_CS | 8 };

int exe$astdel_prep2_new(long stack, long ast, long astprm, long cs, long ss) {
  __asm__ __volatile__(
		       "pushfl\n\t"
		       "popl %edi\n\t"
		       "andl $0x200, %edi \n\t"
		       "je 1f\n\t"
		       "call panic\n\t"
		       "1:\n\t"
		       "movl %esp,%edi\n\t"
		       "addl $-0x14,%edi\n\t" // get new kernel stack
		       "movl 0x4(%esp),%edx\n\t" // get user stack
		       "addl $-0x8,%edx\n\t"
		       "\n\t"
		       "movl $exe$astdel,0x0(%edi)\n\t" // put astdel on kstack
		       "movl 0x10(%esp),%ecx\n\t" // get mode cseg
		       "movl %ecx,0x4(%edi)\n\t" // put cseg on stack
		       "pushfl\n\t"
		       "popl %ecx\n\t"
		       "orl $0x200,%ecx\n\t" // intr on
		       "movl %ecx,0x8(%edi)\n\t" // check get a flag reg
		       "movl %edx,0xc(%edi)\n\t" // put new ustack on kstack
		       "movl 0x14(%esp),%ecx\n\t" // get mode sseg
		       "movl %ecx,0x10(%edi)\n\t" // put sseg on stack
		       "\n\t"
		       "movl 0x8(%esp),%ecx\n\t" // get ast
		       "movl %ecx,0x0(%edx)\n\t" // put ast as parm on ustack
		       "movl 0xc(%esp),%ecx\n\t" // get astprm
		       "movl %ecx,0x4(%edx)\n\t" // put astprm on ustack
#if 0
		       "movl %esp,0x8(%edx)\n\t" // put kstack on ustack
		       "movl 0x0(%esi),%ecx\n\t" // get return eip
		       "movl %ecx,0x8(%edx)\n\t" // put return eip on ustack
#endif
		       "\n\t"
		       "addl $-0x14, %esp\n\t" // stack move
		       // check. need myrei here?
		       "call myrei\n\t"
		       "pushfl\n\t"
		       "popl %edi\n\t"
		       "andl $0x200, %edi \n\t"
		       "je 1f\n\t"
		       "call panic\n\t"
		       "1:\n\t"
#if 1
		       "movl ctl$gl_pcb, %edi	;\n\t" \
		       "movl 0x7a0(%edi), %edx ;\n\t" /* cpuid */ \
		       "shl $0x8, %edx;\n\t" \
		       "addl $init_tss, %edx\n\t"
		       "addl $0x4, %edx\n\t"
		       "movl %esp, (%edx)\n\t"
		       "addl $0x14, (%edx)\n\t"
#endif
		       "movl 0x28(%esp),%ecx\n\t" // get mode sseg
		       "movl %ecx, %ds\n\t"
		       "movl %ecx, %es\n\t"
		       "iret\n\t"
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
#if 1
		       "call exe$astdel_prep2\n\t"
#else
		       "call exe$astdel_prep2_new\n\t"
#endif
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
#endif

#ifdef __x86_64__
int __attribute__ ((section (".vsyscall_4"))) exe$astdel() {
  __asm__ __volatile__(
		       "pushfq\n\t"
		       "pushq %rax\n\t"
		       "pushq %rbx\n\t"
		       "pushq %rcx\n\t"
		       "pushq %rdx\n\t"
		       "pushq %rbp\n\t"
		       "pushq %rdi\n\t"
		       "pushq %rsi\n\t"
		       "pushq %r8\n\t"
		       "pushq %r9\n\t"
		       "pushq %r10\n\t"
		       "pushq %r11\n\t"
		       "pushq %r12\n\t"
		       "pushq %r13\n\t"
		       "pushq %r14\n\t"
		       "pushq %r15\n\t"
		       "callq * %rax\n\t"
#if 0
		       "callq sys$clrast\n\t"
#else
		       "movq $0x28, %rax\n\t"
		       "int $0x81\n\t"
#endif
#if 0
		       "movl 0x1c(%esp),%esi\n\t"
		       "movl %esi,0x20(%esp)\n\t"
		       "movl %esi,0x24(%esp)\n\t"
#endif
		       "popq %r15\n\t"
		       "popq %r14\n\t"
		       "popq %r13\n\t"
		       "popq %r12\n\t"
		       "popq %r11\n\t"
		       "popq %r10\n\t"
		       "popq %r9\n\t"
		       "popq %r8\n\t"
		       "popq %rsi\n\t"
		       "popq %rdi\n\t"
		       "popq %rbp\n\t"
		       "popq %rdx\n\t"
		       "popq %rcx\n\t"
		       "popq %rbx\n\t"
		       "popq %rax\n\t"
		       "popfq\n\t"
		       "popfq\n\t"
		       "popfq\n\t"
		       );
}

long cstab[4] = { __KERNEL_CS, __EXECUTIVE_CS, __SUPERVISOR_CS, __USER_CS };
long sstab[4] = { __KERNEL_DS, __EXECUTIVE_DS , __SUPERVISOR_DS , __USER_DS };

int exe$astdel_prep2_new(long stack, long ast, long astprm, long dummy, long cs, long ss) {
  // rdi rsi rdx
  __asm__ __volatile__(
		       "movq %rsi,%rbp\n\t" // remember ast
		       "movq %rdx,%rbx\n\t" // remember astprm
		       "movq %rdi,%rdx\n\t" // get user stack
		       "movq %rsp,%rdi\n\t"
		       "addq $-0x28,%rdi\n\t" // get new kernel stack
#if 0
		       "addq $-0x10,%rdx\n\t"
#endif
		       "\n\t"
		       "movq $exe$astdel,0x0(%rdi)\n\t" // put astdel on kstack
		       "movq %r8,0x8(%rdi)\n\t" // user cseg
		       "pushfq\n\t"
		       "popq %rcx\n\t"
		       "orq $0x200,%rcx\n\t" // intr on
		       "movq %rcx,0x10(%rdi)\n\t" // check get a flag reg
		       "movq %rdx,0x18(%rdi)\n\t" // put new ustack on kstack
		       "movq %r9,0x20(%rdi)\n\t" // user sseg
		       "\n\t"
#if 0
		       "movq 0x10(%rsp),%rcx\n\t" // get ast
		       "movq %rcx,0x0(%rdx)\n\t" // put ast as parm on ustack
		       "movq 0x18(%rsp),%rcx\n\t" // get astprm
		       "movq %rcx,0x8(%rdx)\n\t" // put astprm on ustack
#endif
#if 0
		       "movq %rsp,0x10(%rdx)\n\t" // put kstack on ustack
		       "movq 0x0(%rsi),%rcx\n\t" // get return eip
		       "movq %rcx,0x10(%rdx)\n\t" // put return eip on ustack
#endif
		       "\n\t"
		       "addq $-0x28, %rsp\n\t" // stack move
		       // check. need myrei here?
		       "callq myrei\n\t"
#if 1
		       "movq 0x7ffff000, %rdi\n\t"
		       "movq 2744(%rdi), %rdx\n\t"
		       "shl $0x8, %rdx\n\t"
		       "addq $init_tss, %rdx\n\t"
		       "addq $0x4, %rdx\n\t" // yes, 4
		       "movq %rsp, (%rdx)\n\t"
		       "addq $0x28, (%rdx)\n\t"
#if 1
		       "movq %rsp, %gs:0\n\t"
		       "addq $0x28, %gs:0\n\t"
#endif
#endif
		       "movq %rbp, %rax\n\t" // ast
		       "movq %rbx, %rdi\n\t" // astprm
		       "cli\n\t"
		       "swapgs\n\t"
		       "iretq\n\t"
);
}
#endif

int astdeb=0;
#ifdef ASTDEBUG
int lastast=0;
int lastastprm=0;
int lastacb=0;
int myacbi=0;
long myacbs[1024];
#endif

asmlinkage void sch$astdel(int dummy) {
  struct _cpu * cpu=smp$gl_cpu_data[smp_processor_id()];
  struct _pcb * p=ctl$gl_pcb;
  struct _acb * acb;

  /*lock*/
#ifdef __x86_64__
  if (intr_blocked(IPL$_ASTDEL))
    return;
  regtrap(REG_INTR, IPL$_ASTDEL);
#endif

#ifdef ASTDEBUG
  myacbs[myacbi++]=0;
  myacbs[myacbi++]=0x77777777;
  myacbs[myacbi++]=0;
  myacbs[myacbi++]=0x77777777;
#endif
 more:
  setipl(IPL$_SYNCH); // also IPL$_SCHED
  vmslock(&SPIN_SCHED,-1);
#ifdef ASTDEBUG
  checkq(&p->pcb$l_astqfl);
#endif

  /* { int i;
     //printk("here ast\n");
     for (i=0; i<1000000; i++) ;
     } */
  if (aqempty(&p->pcb$l_astqfl)) {
    vmsunlock(&SPIN_SCHED,-1);
    sch$newlvl(p);
    return;
  }
  /* { int i,j;
     //printk("here ast2 %x %x %x\n",p->pid,p->pcb$l_astqfl,&p->pcb$l_astqfl);
     for (j=0; j<20; j++) for (i=0; i<1000000000; i++) ;
     } */
  acb=remque(p->pcb$l_astqfl,0);
#ifdef ASTDEBUG
  myacbs[myacbi++]=acb;
  myacbs[myacbi++]=acb->acb$l_ast;
  myacbs[myacbi++]=acb->acb$l_astprm;
  myacbs[myacbi++]=acb->acb$b_rmod;
#if 0
  if (acb->acb$b_rmod&3)
    printk("A %x %x\n",acb,acb->acb$b_rmod);
#endif
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
    vmsunlock(&SPIN_SCHED,-1);
    //printk("astdel1 %x \n",acb->acb$l_kast);
    setipl(IPL$_ASTDEL);
    //p->pcb$b_astact=1;
#ifdef __i386__
    if (((unsigned long)acb->acb$l_kast<0x80000000)||((unsigned long)acb->acb$l_kast>0xb0000000)) {
      int i;
      printk("kast %x\n",acb->acb$l_kast);
      for(i=0;i<2000000000;i++) ;
    }
#endif
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
  // avoid leftovers
  if ((current->pslstk[current->pslindex-1]&3)<(acb->acb$b_rmod&3)) goto out;
  // skip if disabled
  if (!test_bit(acb->acb$b_rmod&3,&p->pcb$b_asten)) goto out;
  // test if busy already
  if (test_and_set_bit(acb->acb$b_rmod&3,&p->pcb$b_astact)) {
  out:
#ifdef ASTDEBUG
    myacbs[myacbi++]=-1;
    myacbs[myacbi++]=acb;
    myacbs[myacbi++]=acb->acb$l_ast;
    myacbs[myacbi++]=acb->acb$l_astprm;
#endif
    insque(acb,&p->pcb$l_astqfl);
    p->phd$b_astlvl=p->pr_astlvl=(acb->acb$b_rmod & 3) + 1;
    vmsunlock(&SPIN_SCHED,-1);
    return;
  }
  p->phd$b_astlvl=p->pr_astlvl=(acb->acb$b_rmod & 3) + 1;
  //unlock
  vmsunlock(&SPIN_SCHED,-1);
  setipl(IPL$_ASTDEL);
#ifdef OLDAST
  p->pcb$b_astact=0; // 1; wait with this until we get modes
  setipl(0); // for kernel mode, I think. everything is in kernelmode yet.
#endif
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
    acb->acb$b_rmod&=~ACB$M_PKAST; // check
    if (acb->acb$b_rmod&3)
      printk("error: pkast not kernel mode\n"); 
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
#ifndef OLDAST
  if ((acb->acb$b_rmod&(ACB$M_NODELETE|ACB$M_PKAST))==0) kfree(acb);
  if (rmod&3) {
#ifdef __i386__
    user_spaceable_addr(exe$astdel); // bad temp hack
    user_spaceable_addr(&dummy); // bad temp hack
#endif
#if 0
    int sts = exe$astdel_prep(&dummy,ast,astprm);
#else
#ifdef __i386__
    int sts = exe$astdel_prep2_new(p->ipr_sp[rmod&3],ast,astprm,cstab[rmod&3],sstab[rmod&3]);
#else
    int sts = exe$astdel_prep2_new(p->ipr_sp[rmod&3],ast,astprm,0,cstab[rmod&3],sstab[rmod&3]);
#endif
#endif
  } else {
    setipl(0);
    if(ast) ast(astprm); /* ? */
    // simulate exe$astdel and cmod$astexit
    test_and_clear_bit(rmod&3, &p->pcb$b_astact); // check
    sch$newlvl(p);
  }
#else
  if(ast) ast(astprm); /* ? */
#endif
#endif
#ifdef __i386__
  //      printk("a4 ");
#endif
#ifdef OLDAST
  p->pcb$b_astact=0;
  /*unlock*/
  goto more;
#endif
}

void sch$newlvl(struct _pcb *p) {
  int newlvl;
  int oldipl=getipl();

  setipl(IPL$_SYNCH);
  vmslock(&SPIN_SCHED,-1);

  if (aqempty(p->pcb$l_astqfl))
    newlvl=4;
  else {
    if(p->pcb$l_astqfl->acb$b_rmod & ACB$M_KAST)
      newlvl=0; // was: p->phd$b_astlvl; /* ? */
    else
      newlvl=p->pcb$l_astqfl->acb$b_rmod & 3;
  }
    
  p->phd$b_astlvl=newlvl;
  p->pr_astlvl=newlvl;
  vmsunlock(&SPIN_SCHED,-1);
  setipl(oldipl);
}


