// $Id$
// $Locker$

// Author. Roar Thronæs.

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
#include<irpdef.h>
#include<ssdef.h>
#include<ucbdef.h>
#include<uicdef.h>
#include<vcbdef.h>
#include<fh2def.h>
#include"../../freevms/f11x/src/xqp.h"

#include<linux/init.h>
#include<linux/sched.h>
#include<linux/vmalloc.h>
#include<asm/current.h>

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
  int pid=current->pid;
  struct _irp * i;
  int sts;
  int fcode, fmode;
  pid=0;
  while (!aqempty(&xqp->xqp_head)) {
    i=remque(xqp->xqp_head,0);
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
	if (0) 
	  sts=direct(vcb,fibdsc,filedsc,reslen,resdsc,atrp,action,i);
	else
	  sts=f11b_access(vcb,i);
      }
      break;
    case IO$_READVBLK:
      {
	char * buffer;
	//f11b_read_writevb(i);
	accesschunk(0,i->irp$l_qio_p3,&buffer,0,0,i);
	memcpy(i->irp$l_qio_p1,buffer,512);
      }
      break;
    case IO$_CREATE:
      {
	struct _vcb * vcb = i->irp$l_ucb->ucb$l_vcb;
	//f11b$create(vcb,i);
	update_create(vcb,i->irp$l_qio_p1,i->irp$l_qio_p2,i);
      }
      break;
    default:
      printk ("xqp %x code not implemented yet\n",fcode);
      break;
    }
  }
}

void f11b$create(struct _vcb * v, struct _irp * i) {
  struct _fh2 * head=vmalloc(sizeof(struct _fh2));
  struct _fcb * fcb=vmalloc(sizeof(struct _fcb));
  
}

/* module data
rwvb:
read_writevb





 */
