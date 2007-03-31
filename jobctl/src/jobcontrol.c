// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <linux/sched.h>

#include <system_data_cells.h>
#include <lnmdef.h>
#include <msgdef.h>
#include <descrip.h>
#include <iodef.h>
#include <ssdef.h>
#include<starlet.h>
#include <exe_routines.h>
#include <misc_routines.h>

#ifdef CONFIG_VMS

// do this as an ordinary process after we get privileged images

short int mbx_buffer[128];
short int mailbox_channel=0;
static unsigned long long iosb;
do_mbx_read();

mbx_read_ast() {
  int unit = mbx_buffer[1];
  char * s = &mbx_buffer[sizeof(long)>>1];
  if (((iosb&0xffff)&1)==0)
    printk("iosb error\n");
  switch(mbx_buffer[0]) {
  case MSG$_TRMUNSOLIC:
    {
      char t[4];
      struct dsc$descriptor term;
      term.dsc$w_length=4;
      term.dsc$a_pointer=t;
      memcpy(t,s,4);
      printk("s is %s\n",s);
      jobctl_unsolicit(&term);
    }
    break;
  default:
    printk("unrecognized mbx\n");
  }
  do_mbx_read();
}

do_mbx_read() {
  int status;
  status = sys$qio (0, (int) mailbox_channel, IO$_READVBLK, &iosb, mbx_read_ast, 0, mbx_buffer, 256, 0, 0, 0, 0);
#if 0
  if ((status&1)==0) signal(status);
#endif

  return SS$_NORMAL;
}

int job_control(void *unused)
{
  int sts;
  struct _pcb *tsk = ctl$gl_pcb;
  strcpy(tsk->pcb$t_lname, "JOB_CONTROL");
  sts=exe$crembx(1,&mailbox_channel,512,512,0,0,0,0);
  sys$ar_jobctlmb=ctl$gl_ccbbase[mailbox_channel].ccb$l_ucb;
  do_mbx_read();
  sys$hiber();
  printk("job control should not get past sys$hiber\n");
}

#endif
