#include"../../freevms/pal/src/ipl.h"
#include"../../freevms/sys/src/system_data_cells.h"
#include<linux/smp.h>

 inline void splx(void) {
  int this_cpu = smp_processor_id();
  int i, tmp;
  tmp=vmscpus[this_cpu].cpu$b_ipl;
  vmscpus[this_cpu].cpu$b_ipl=vmscpus[this_cpu].previpl[vmscpus[this_cpu].iplnr--];
  for(i=0;i<256;i++)
    if (vmscpus[this_cpu].cpu$t_ipending[i].interrupt>=tmp) { ; }
  /*
    do the interrupt?
   */

  /* check and do asts */
}

 inline char spl(unsigned char new) {
  int this_cpu = smp_processor_id();
  if (new<=vmscpus[this_cpu].cpu$b_ipl) {
    /* mark ipending */
    return 1;
  }
  vmscpus[this_cpu].previpl[vmscpus[this_cpu].iplnr++]=vmscpus[this_cpu].cpu$b_ipl;
  vmscpus[this_cpu].cpu$b_ipl=new;
  return 0;
}

inline int prespl_not(unsigned char new) {
  int this_cpu = smp_processor_id();
  printk("prespl: should not get here now\n");
  if (new<=vmscpus[this_cpu].cpu$b_ipl) {
    /* mark ipending */
    return 1;
  }
  return 0;
}



