// $Id$
// $Locker$

// Author. Paul Nankervis.
// Author. Roar Thronæs.

#include <linux/config.h>

#include<mytypes.h>
#include<ccbdef.h>
#include<cpudef.h>
#include<descrip.h>
#include<fatdef.h>
#include<fcbdef.h>
#include<fibdef.h>
#include<fiddef.h>
#include<iodef.h>
#include<iosbdef.h>
#include<ipldef.h>
#include<irpdef.h>
#include<ssdef.h>
#include<ucbdef.h>
#include<uicdef.h>
#include<vcbdef.h>
#include<fh2def.h>
#include"../../f11x/src/xqp.h"

#include<linux/init.h>
#include<linux/sched.h>
#include<asm/current.h>

void f11b$dispatcher(void);
unsigned f11b_modify(struct _vcb * vcb, struct _irp * irp);

struct __xqp xqps[1]; // number of pids 

struct __xqp * xqp=&xqps[0];

void __init xqp_init(void) {
  int i;
  for(i=0;i<1;i++) {
    qhead_init(&xqps[i].xqp_head);
  }
}

void f11b$dispatch(struct _irp * i) {
  insque(i, &xqp->xqp_head);
  //  insque(i->irp$l_ioqfl, &xqps[current->pid].xqp_queue);
  f11b$dispatcher();
}

void f11b$dispatcher(void) {
  int pid=current->pcb$l_pid;
  struct _irp * i;
  int sts;
  int fcode, fmode;
  pid=0;
  while (!aqempty(&xqp->xqp_head)) {
    i=remque(xqp->xqp_head,0);
    xqp->io_channel=i->irp$w_chan;
    fcode=i->irp$v_fcode;
    fmode=i->irp$v_fmod;
    iosbret(i,SS$_NORMAL);

    switch (fcode) {
    case IO$_ACCESS:
      {
	struct _vcb * vcb = i->irp$l_ucb->ucb$l_vcb;
	struct dsc$descriptor * fibdsc=i->irp$l_qio_p1;
	struct dsc$descriptor * filedsc=i->irp$l_qio_p2;
	unsigned short *reslen=i->irp$l_qio_p3;
	struct dsc$descriptor * resdsc=i->irp$l_qio_p4;
	void * atrp=i->irp$l_qio_p5;
	struct _fibdef * fib=(struct _fibdef *)fibdsc->dsc$a_pointer;
	unsigned action=0;
	if (i->irp$l_func & IO$M_ACCESS) action=0;
	if (i->irp$l_func & IO$M_CREATE) action=2;
	if (i->irp$l_func & IO$M_DELETE) action=1;
	//if (fib->fib$w_did_num>0) 
	sts=f11b_access(vcb,i);
	if (sts==SS$_NOSUCHFILE && (i->irp$l_func & IO$M_CREATE))
	  goto create;
      }
      break;
    case IO$_READVBLK:
    case IO$_WRITEVBLK:
      {
	char * buffer;
	f11b_read_writevb(i);
	//return; // too early, maybe, but because of io_done
	//accesschunk(0,i->irp$l_qio_p3,&buffer,0,0,i);
	//memcpy(i->irp$l_qio_p1,buffer,512);
      }
      break;
    case IO$_CREATE:
      {
	struct _vcb * vcb = i->irp$l_ucb->ucb$l_vcb;
	//f11b$create(vcb,i);
      create:
	f11b_create(vcb,i);
      }
      break;
    case IO$_DELETE:
      {
	struct _vcb * vcb = i->irp$l_ucb->ucb$l_vcb;
	f11b_delete(vcb,i);
      }
      break;
    case IO$_MODIFY:
      {
	struct _vcb * vcb = i->irp$l_ucb->ucb$l_vcb;
	f11b_modify(vcb,i);
      }
      break;
    default:
      printk ("xqp %x code not implemented yet\n",fcode);
      break;
    }
    if (i) 
      f11b_io_done(i);
  }
}

/* module data
rwvb:
read_writevb





 */

unsigned f11b_modify(struct _vcb * vcb, struct _irp * irp)
{
  struct _iosb iosb;
  unsigned sts=SS$_NORMAL;
  unsigned wrtflg=1;
  struct _fcb *fcb;
  struct _fh2 *head;
  struct dsc$descriptor * fibdsc=irp->irp$l_qio_p1;
  struct dsc$descriptor * filedsc=irp->irp$l_qio_p2;
  unsigned short *reslen=irp->irp$l_qio_p3;
  struct dsc$descriptor * resdsc=irp->irp$l_qio_p4;
  void * atrp=irp->irp$l_qio_p5;
  struct _fibdef * fib=(struct _fibdef *)fibdsc->dsc$a_pointer;
  struct _fiddef * fid=&((struct _fibdef *)fibdsc->dsc$a_pointer)->fib$w_fid_num;
  unsigned action=0;
  if (irp->irp$l_func & IO$M_ACCESS) action=0;
  if (irp->irp$l_func & IO$M_DELETE) action=1;
  if (irp->irp$v_fcode == IO$_CREATE) action=2;
  if (irp->irp$l_func & IO$M_CREATE) action=2;
  action=0;
#ifdef DEBUG
  printk("Accessing file (%d,%d,%d)\n",(fid->fid$b_nmx << 16) +
	 fid->fid$w_num,fid->fid$w_seq,fid->fid$b_rvn);
#endif

  xqp->current_vcb=vcb; // until I can place it somewhere else

  if (fib->fib$w_did_num) {
    struct _fh2 * head;
    struct _fcb * fcb=xqp->primary_fcb;
    head = f11b_read_header (vcb, 0, fcb, &iosb);  
    sts=iosb.iosb$w_status;
    if (VMSLONG(head->fh2$l_filechar) & FH2$M_DIRECTORY) {
      unsigned eofblk = VMSSWAP(head->fh2$w_recattr.fat$l_efblk);
      if (VMSWORD(head->fh2$w_recattr.fat$w_ffbyte) == 0) --eofblk;
      sts = search_ent(fcb,fibdsc,filedsc,reslen,resdsc,eofblk,action);
    } else {
      sts = SS$_BADIRECTORY;
    }
  }

  if ( (sts & 1) == 0) { iosbret(irp,sts); return sts; }

  if (irp->irp$l_qio_p5 == 0)
    return SS$_NORMAL;

  if (wrtflg && ((vcb->vcb$b_status & VCB$M_WRITE_IF) == 0)) { iosbret(irp,SS$_WRITLCK);  return SS$_WRITLCK; }

  if (fib->fib$w_fid_num == 0 && fib->fib$w_fid_seq == 0  && fib->fib$b_fid_nmx == 0) {
    fcb=xqp->primary_fcb;
    printk("in create dir workaround? %x\n",fcb);
  } else
    fcb=f11b_search_fcb(vcb,fid);
  head = f11b_read_header(vcb,fid,fcb,&iosb);
  sts=iosb.iosb$w_status;
  if (sts & 1) {
  } else {
    printk("Accessfile status %d\n",sts);
    iosbret(irp,sts);
    return sts;
  }

  if (fcb==NULL) {
    fcb=fcb_create2(head,&sts);
  }
  if (fcb == NULL) { iosbret(irp,sts); return sts; }

  xqp->primary_fcb=fcb;
  xqp->current_window=&fcb->fcb$l_wlfl;

  if (atrp) f11b_write_attrib(fcb,atrp);

  if ((fib->fib$w_exctl&FIB$M_EXTEND) && (sts & 1)) {
    struct _fcb * newfcb;
    newfcb=f11b_search_fcb(xqp->current_vcb,&fib->fib$w_fid_num);
    sts = f11b_extend(newfcb,fib->fib$l_exsz,0);
  }

  iosbret(irp,SS$_NORMAL);
  return SS$_NORMAL;
}

int f11b_io_done(struct _irp * i) {
  void (*func)(unsigned long);
  // move user_status into irp$l_media
  if (i->irp$l_iosb==0) { // not set, or something vipes the irp
    i->irp$l_iost1=SS$_NORMAL;  // temporary value, must be set
  } else {
    i->irp$l_iost1=((struct _iosb *)i->irp$l_iosb)->iosb$w_status;
  }
  // decr vcb transaction count
  // clear namestring
  // copy local fib back intro the complex buffer packet
  // set irp$l_bcnt to abd$c_attrib
  
  // call check-dismount

  setipl(IPL$_ASTDEL);
  ioc$bufpost(i);

  sch$postef(i->irp$l_pid,0,i->irp$b_efn);
  
  func=i->irp$l_wind; //really acb$l_kast
  func(i);

  setipl(0);
}
