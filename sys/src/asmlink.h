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
asmlinkage int system_call(void);
asmlinkage int vmssystem_call(void);
