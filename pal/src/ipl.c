#include"../../freevms/pal/src/ipl.h"
#include"../../freevms/sys/src/system_data_cells.h"
#include<linux/smp.h>

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



