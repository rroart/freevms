// $Id$
// $Locker$

// Author. Roar Thronæs.

// should be started by VMS$INITIAL-050_CONFIGURE.COM
// but for now we will use a kernel thread

// configure is supposed to use polling but if we use polling
// we might never be aware of the other devices

#include <linux/config.h>
#include <linux/mm.h>

#include <descrip.h>
#include <prcpoldef.h>
#include <cdldef.h>
#include <cdrpdef.h>
#include <cdtdef.h>
#include <chdef.h>
#include <cwpsdef.h>
#include <ddbdef.h>
#include <ddtdef.h>
#include <dptdef.h>
#include <dyndef.h>
#include <fdtdef.h>
#include <iodef.h>
#include <iosbdef.h>
#include <mscpdef.h> // does not belong here
#include <nisca.h>
#include <pbdef.h>
#include <pdtdef.h>
#include <rddef.h>
#include <rdtdef.h>
#include <sbdef.h>
#include <sbnbdef.h>
#include <scsdef.h>
#include <ssdef.h>
#include <system_data_cells.h>
#include <ucbdef.h>
#include <vcdef.h>

extern struct _pb mypb;
extern struct _sb mysb;
extern struct _pdt mypdt;
extern struct _pb otherpb;
extern struct _sb othersb;

int cluster_not_started = 1;

short int cf_mbxchan = 0;

void configure_init(void) {
  int promsk=0;
  int sts = exe$crembx(1,&cf_mbxchan,0,0,promsk,0,0,0);
}

void someone_wrote(void) {
  int sts;
  struct _iosb iosb;
  printk("someone wrote\n");
  struct _prcpol prcpol_, *prcpol=&prcpol_;
  memset(prcpol, 0, sizeof(struct _prcpol));
  sts = sys$qiow(0, cf_mbxchan, /*IO$M_NOW|*/IO$_READVBLK, &iosb, 0, 0, prcpol, sizeof(struct _prcpol), 0, 0, 0, 0);
  if (0==strncmp("mscp$disk",&prcpol->prcpol$b_prcnam[0],9)) {
    mscp_talk_with(&prcpol->prcpol$t_nodnam,"mscp$disk");
  } else {
    printk("prcnam not recognized: %s\n",&prcpol->prcpol$b_prcnam[0]);
  }
  sts = sys$qiow(0, cf_mbxchan, IO$_SETMODE|IO$M_WRTATTN, &iosb, 0, 0, someone_wrote, 0, 0, 0, 0, 0);
}

void configure(void) {

  struct _iosb iosb;
  signed long long sec=-10000000;
  signed long long tensec=-100000000;

  $DESCRIPTOR(process_name_,"CONFIGURE");
  struct dsc$descriptor * process_name = &process_name_;
  exe$setprn(process_name);

#if 0
    if (cluster_not_started) {
      exe$schdwk(0,0,&sec,0);
      sys$hiber();
    }
#endif
    int sts = sys$qiow(0, cf_mbxchan, IO$_SETMODE|IO$M_WRTATTN, &iosb, 0, 0, someone_wrote, 0, 0, 0, 0, 0);

  while (1) {
    sys$hiber();
  }
}

