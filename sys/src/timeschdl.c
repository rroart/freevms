#include <linux/linkage.h>
#include <linux/sched.h>

asmlinkage void exe$swtimint(void) {
  if (current->phd$w_quant>=0 && current->phd$w_quant<128) 
    sch$qend(current);
  /* check tqe from EXE$GL_TQFL */
}
