// $Id$
// $Locker$

// Author. Roar Thronæs.

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
#include<ttydef.h>
#include<ttyrbdef.h>

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

  i->irp$q_tt_state |= TTY$M_ST_READ;

  struct _tty_ucb * tty = u;
  tty->ucb$q_tt_state = i->irp$q_tt_state; // here?

  struct _tt_readbuf * rb = kmalloc(size+sizeof(struct _tt_readbuf), GFP_KERNEL);
  memset(rb, 0, size+sizeof(struct _tt_readbuf));

  rb->tty$w_rb_type = DYN$C_BUFIO;
  rb->tty$w_rb_size = size+sizeof(struct _tt_readbuf);
  rb->tty$l_rb_txt = &rb->tty$l_rb_data;
  rb->tty$l_rb_uva = buf;
  rb->tty$l_rb_mod = i->irp$b_rmod;
  rb->tty$w_rb_txtsiz = i->irp$l_bcnt;

  i->irp$l_svapte = rb;

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

  i->irp$q_tt_state |= TTY$M_ST_WRITE;

  struct _tty_ucb * tty = u;
  tty->ucb$q_tt_state = i->irp$q_tt_state; // here?

  struct _twp * wb = kmalloc(size+sizeof(struct _twp), GFP_KERNEL);
  memset(wb, 0, sizeof(struct _twp));

  wb->tty$b_wb_type = DYN$C_TWP;
  wb->tty$l_wb_map = &wb->tty$l_wb_data; // start?
  wb->tty$l_wb_next = &wb->tty$l_wb_data;
  wb->tty$l_wb_end = (long)&wb->tty$l_wb_data+size;
  wb->tty$l_wb_irp = i;
  memcpy(&wb->tty$l_wb_data,buf,size);

  i->irp$l_svapte = wb;

  // not yet
  // no full duplex with untested altquepkt, using qiodrvpkt instead
  tty->ucb$l_tt_wrtbuf = wb;
  sts = exe$altquepkt (i,p,u);

  // sts = exe$qiodrvpkt (i,p,u); // seems duplex is needed to avoid crashes
  return sts;
}

