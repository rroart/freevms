// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<cpudef.h>
#include<irpdef.h>

#include<linux/init.h>
#include<linux/sched.h>
#include<asm/current.h>

struct __xqp {
  unsigned long long xqp_queue;
} xqps[65000]; // number of pids 

void __init xqp_init(void) {
  int i;
  for(i=0;i++;i<65000) {
    qhead_init(&xqps[i].xqp_queue);
  }
}

void f11b$dispatch(struct _irp * i) {
  insque(i->irp$l_ioqfl, xqps[current->pid].xqp_queue);
}
