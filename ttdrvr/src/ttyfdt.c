#include<crbdef.h>
#include<cdtdef.h>
#include<dcdef.h>
#include<ddtdef.h>
#include<dptdef.h>
#include<fdtdef.h>
#include<pdtdef.h>
#include<idbdef.h>
#include<irpdef.h>
#include<ucbdef.h>
#include<ddbdef.h>
#include<ipldef.h>
#include<dyndef.h>
#include<ssdef.h>
#include<iodef.h>
#include<devdef.h>
#include<system_data_cells.h>
#include<ipl.h>
#include<linux/vmalloc.h>
#include<linux/pci.h>
#include<system_service_setup.h>
#include<descrip.h>
#include<ttyucbdef.h>

#include<linux/blkdev.h>

// maybe use _twp and _tt_readbuf for these two?
int xyz=0;
int tty$fdtread(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c){
  int sts;

  int *buf,size;
  size=i->irp$l_qio_p2;
  buf = i->irp$l_qio_p1;
  exe_std$readchk(i,p,u,buf,size);

  struct _ltrm_ucb * lt = u;

  // check whether something we set earlier is consistent
  if (lt->ucb$l_tl_phyucb) {
    struct _tty_ucb * tty = lt->ucb$l_tl_phyucb;
    if (u!=tty->ucb$l_tt_logucb)
      printk("<0>" "tty error in read\n");
  }

  sts = exe$qiodrvpkt (i,p,u);
  return sts;
}

int tty$fdtwrite(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c){
  int sts;

  int *buf,size;
  size=i->irp$l_qio_p2;
  buf = i->irp$l_qio_p1;
  exe_std$writechk(i,p,u,buf,size);

  struct _ltrm_ucb * lt = u;

  // check whether something we set earlier is consistent
  if (lt->ucb$l_tl_phyucb) {
    struct _tty_ucb * tty = lt->ucb$l_tl_phyucb;
    if (u!=tty->ucb$l_tt_logucb)
      printk("<0>" "tty error in write\n");
  }

  // not yet
  // no full duplex with untested altquepkt, using qiodrvpkt instead
  //  sts = exe$altquepkt (i,p,u);

  sts = exe$qiodrvpkt (i,p,u);
  return sts;
}

