#include"../../freevms/lbr/src/ipl.h"
#include"../../freevms/sys/src/system_data_cells.h"
#include<linux/smp.h>

 inline void splx(void) {
  int this_cpu = smp_processor_id();
  int i, tmp;
  tmp=vmscpus[this_cpu].cpu$b_ipl;
  vmscpus[this_cpu].cpu$b_ipl=old;
  for(i=0;i<256;i++)
    if (vmscpus[this_cpu].cpu$t_ipending[i].interrupt>=tmp) { ; }
  /*
    do the interrupt?
   */

  /* check and do asts */
}

 inline void spl(void) {
  int this_cpu = smp_processor_id();
  unsigned char tmp=vmscpus[this_cpu].cpu$b_ipl;
  vmscpus[this_cpu].cpu$b_ipl=new;
  return tmp;
}

inline int prespl(unsigned char new) {
  int this_cpu = smp_processor_id();
  printk("prespl: should not get here now\n");
  if (new<=vmscpus[this_cpu].cpu$b_ipl) {
    /* mark ipending */
    return 1;
  }
  return 0;
}
