// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <linux/config.h>
#include<linux/linkage.h>
#include<linux/sched.h>

#include<ssdef.h>
#include<va_rangedef.h>
#include <system_data_cells.h>
#include<rdedef.h>
#include<phddef.h>

mmg$imgreset() {
  // rm$reset (in rmsreset)
  
  // exe$pscan_imgreset (process_can)

  // ws peak check to prev state

  // release icbs

  // now delete all P0 space
  // but just up to 0-0x8000000 for now
  struct _va_range va;
  va.va_range$ps_start_va=0;
  va.va_range$ps_end_va=0x08000000;
  exe$deltva(&va,0,0);

  // should also remove related rdes?
  struct _rde * rde_head = ctl$gl_pcb->pcb$l_phd->phd$ps_p0_va_list_flink;
  struct _rde * tmp = rde_head->rde$ps_va_list_flink;
  while (tmp!=rde_head) {
    struct _rde * next=tmp->rde$ps_va_list_flink;
    if (tmp->rde$pq_start_va<0x08000000) {
      if (tmp->rde$pq_start_va==0 && tmp->rde$q_region_size==0) {
	printk("can not figure out where this gets inserted; can not kfree it either\n");
	printk("rde %x %x %x %x %x\n",rde_head, tmp,tmp->rde$pq_start_va,tmp->rde$q_region_size);
      } else {
	remque(tmp,0);
	kfree(tmp);
      }
    }
    tmp=next;
  }

  // delete nonpermanent P1

  // set ws list to default value

  // ipl to 2
  // mmg$sectblrst

  // reset priv mask
  ctl$gl_pcb->pcb$l_priv=ctl$gq_procpriv;

  ctl$gl_imghdrbf=0;

  // if last release more

  // set phd$w_wslast
}

mmg$dalcstxscn(void) {
}
