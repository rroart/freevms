#include<linux/linkage.h>

asmlinkage void sched_vector(void);
asmlinkage void resched_vector(void);
asmlinkage void timer_vector(void);
asmlinkage void iopost_vector(void);
asmlinkage void exe$forkdspth(void);
asmlinkage void ioc$iopost(void);
asmlinkage void sch$astdel(int dummy);
asmlinkage void sw_ast(void);
asmlinkage void sch$sched(int);
asmlinkage void sch$resched(void);
asmlinkage void schedule(void);
asmlinkage void exe$swtimint(void);
asmlinkage void exe$forkdspth(void);
#ifdef __i386__
asmlinkage int system_call(void);
asmlinkage int vmssystem_call(void);
#endif
#ifdef __x86_64__
asmlinkage int ia32_vmssystem_call(void);
asmlinkage int ia32_vmssystem_call1(void);
asmlinkage int ia32_vmssystem_call2(void);
asmlinkage int ia32_vmssystem_call3(void);
asmlinkage int vms_system_call(void);
asmlinkage int vms_system_call1(void);
asmlinkage int vms_system_call2(void);
asmlinkage int vms_system_call3(void);
#endif
