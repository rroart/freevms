#include<linux/sched.h>

void ioc$iopost(void) {
}

void ioc$myiopost(struct _pcb * p,unsigned long priclass) {
  sch$postef(p,priclass);
}

