// $Id$
// $Locker$

// Author. Roar Thronæs.

#include"../../freevms/lib/src/cdtdef.h"
#include"../../freevms/lib/src/pdtdef.h"

#include<aqbdef.h>
#include<cdrpdef.h>
#include<ddbdef.h>
#include<dyndef.h>
#include<descrip.h>
#include<iodef.h>
#include<iosbdef.h>
#include<irpdef.h>
#include<misc.h>
#include<mscpdef.h>
#include<scsdef.h>
#include<ssdef.h>
#include<ucbdef.h>
#include<vcbdef.h>
#include<system_data_cells.h>

#include<linux/vmalloc.h>

void mscpmyerr(void) {
  /* do nothing yet */
}

int get_mscp_chan(char * s) {
  int sts;
  unsigned short int chan;
  struct _ucb * ucb;
  struct dsc$descriptor dsc;
  struct return_values r;
  struct _vcb * vcb;
  struct _aqb * aqb;
  dsc.dsc$w_length=strlen(s);
  dsc.dsc$a_pointer=s;
  if (ioc$search(&r,&dsc)==SS$_NORMAL)
    return ((struct _ucb *)r.val1)->ucb$ps_adp;
  ucb = fl_init(s);
  vcb = (struct _vcb *) vmalloc(sizeof(struct _vcb));
  bzero(vcb,sizeof(struct _vcb));
  vcb->vcb$b_type=DYN$C_VCB;
  aqb = (struct _aqb *) vmalloc(sizeof(struct _aqb));
  bzero(aqb,sizeof(struct _aqb));
  aqb->aqb$b_type=DYN$C_AQB;
  qhead_init(&aqb->aqb$l_acpqfl);
  ucb->ucb$l_vcb=vcb;
  vcb->vcb$l_aqb=aqb;
  qhead_init(&vcb->vcb$l_fcbfl);
  vcb->vcb$l_cache = NULL; // ?
  sts = phyio_init(strlen(s),ucb->ucb$l_ddb->ddb$t_name,&ucb->ucb$l_vcb->vcb$l_aqb->aqb$l_mount_count,0);
  sts=exe$assign(&dsc,&chan,0,0,0);
  ucb->ucb$ps_adp=chan; //wrong field and use, but....
  return chan;
}

extern struct _cdt cdtl[1024];

static in_workaround=0;

void returnsome(struct _irp *i);

int mscplisten(void * packet, struct _cdt * c, struct _pdt * p) {
  int sts;
  struct _iosb * iosb=vmalloc(sizeof(struct _iosb));
  struct _cdrp * cdrp;
  struct _scs * scs = packet;
  struct _cdt * cdt = &cdtl[scs->scs$l_dst_conid];
  struct _mscp_basic_pkt * basic = ((unsigned long)packet) + sizeof(*scs);
  struct _transfer_commands * trans = basic;
  int chan=get_mscp_chan(cdt->cdt$l_condat);
  unsigned long lbn=trans->mscp$l_lbn;
  char * buf;
#if 0
  int savipl=setipl(0); // still something funny someplace
  int savis=current->psl_is;
  current->psl_is=0;
  in_workaround=1;
#endif
  cdrp = vmalloc(sizeof(struct _cdrp));
  bzero(cdrp,sizeof(struct _cdrp));
  if (basic->mscp$b_opcode == MSCP$K_OP_WRITE) {
    buf = trans + sizeof(*trans);
    sts = sys$qiow(0,chan,IO$_WRITELBLK,iosb,0,0,buf,512,lbn,0,0,0);
    basic->mscp$b_caa=basic->mscp$b_opcode;
    basic->mscp$b_opcode = MSCP$K_OP_END;
    cdrp->cdrp$w_cdrpsize=600;
    cdrp->cdrp$l_cdt=c;
    cdrp->cdrp$l_msg_buf=basic;
    scs_std$senddg(0,0,cdrp);
  }
  if (basic->mscp$b_opcode == MSCP$K_OP_READ) {
    buf = vmalloc(1024);
    trans = basic = buf;
#if 0
    sts = sys$qiow(0,chan,IO$_READLBLK,iosb,0,0,(unsigned long)buf+sizeof(*trans),512,lbn,0,0,0);
#else
    {
      struct _irp * i=vmalloc(sizeof(struct _irp));
      struct _ucb * u;
      bzero(i,sizeof(struct _irp));
      iosb->iosb$w_status=0;
      i->irp$b_type=DYN$C_IRP;
      i->irp$w_chan=chan;
      i->irp$l_func=IO$_READLBLK;
      i->irp$l_iosb=iosb;
      i->irp$l_qio_p1=(unsigned long)buf+sizeof(*trans);
      i->irp$l_qio_p2=512;
      i->irp$l_qio_p3=lbn;
      //printk("chan %x\n",chan);
      u=ctl$gl_ccbbase[chan].ccb$l_ucb;
      i->irp$l_ucb=u;
      i->irp$l_pid=current->pid;
      i->irp$l_sts|=IRP$M_BUFIO;
      i->irp$l_astprm=i;
      i->irp$l_ast=returnsome;
      i->irp$l_cdt=c;
      exe$insioq(i,u);
    }
#endif
  }
}

void returnsome(struct _irp * i) {
  struct _cdrp * cdrp = vmalloc(sizeof(struct _cdrp));
  struct _mscp_basic_pkt * basic = vmalloc(1024);
  bzero(cdrp,sizeof(struct _cdrp));
  bzero(basic,1024);
  basic->mscp$b_caa=MSCP$K_OP_READ;
  basic->mscp$b_opcode = MSCP$K_OP_END;
  bcopy(i->irp$l_qio_p1,(unsigned long)basic + sizeof(struct _transfer_commands),512);
  cdrp->cdrp$w_cdrpsize=600;
  cdrp->cdrp$l_cdt=i->irp$l_cdt;
  cdrp->cdrp$l_msg_buf=basic;
  scs_std$senddg(0,0,cdrp);
#if 0
  if (savis) current->psl_is=1;
  setipl(savipl);
  in_workaround=0;
#endif
}

void mscpdaemonize() { }

/* mscp.exe mscp$disk let it be a kernel_thread? maybe not... */
int mscp(void) {
  char myname[]="mscp$disk";
  char myinfo[]="mscp disk";
  mscpdaemonize(); /* find out what this does */

  //  listen(msgbuf,err,cdt,pdt,cdt);
  scs$listen(mscplisten,mscpmyerr,myname,myinfo,0);
}


