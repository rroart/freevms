// $Id$
// $Locker$

// Author. Roar Thronæs.

#include"../../freevms/lib/src/cdtdef.h"
#include"../../freevms/lib/src/pdtdef.h"

#include<acbdef.h>
#include<aqbdef.h>
#include<cdrpdef.h>
#include<ddbdef.h>
#include<dyndef.h>
#include<descrip.h>
#include<iodef.h>
#include<iosbdef.h>
#include<hrbdef.h>
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

unsigned long long hrbq = 0;

void * find_hrb(int rspid) {
  struct _hrb * head=&hrbq;
  struct _hrb * tmp=head->hrb$l_flink;
  while (tmp!=head) {
    if (rspid==tmp->hrb$l_cmd_time) return tmp;
    tmp=tmp->hrb$l_flink;
  }
  return 0;
}

int mscplisten(void * packet, struct _cdt * c, struct _pdt * p) {
  int sts;
  struct _iosb * iosb=vmalloc(sizeof(struct _iosb));
  struct _cdrp * cdrp;
  struct _scs * scs = packet;
  struct _scs1 * scs1 = scs;
  struct _ppd * ppd = scs;
  struct _cdt * cdt = &cdtl[scs->scs$l_dst_conid];
  struct _mscp_basic_pkt * basic = ((unsigned long)packet) + sizeof(*scs);
  void * next = basic;
  struct _transfer_commands * trans = basic;
  int chan=get_mscp_chan(cdt->cdt$l_condat);

  if (hrbq==0) qhead_init(&hrbq);

  switch (ppd->ppd$b_opc) {
  case PPD$C_REQDAT:
    goto read;
    break;
  case PPD$C_SNDDAT:
    goto write;
    break;
  case PPD$C_SNDDG:
    goto mscp;
    break;
  default:
    panic("ppdopc\n");

  }

 mscp:
  {
    unsigned long lbn=trans->mscp$l_lbn;
    struct _hrb * hrb=vmalloc(sizeof(struct _hrb));
    cdrp = vmalloc(sizeof(struct _cdrp));
    bzero(cdrp,sizeof(struct _cdrp));
    bzero(hrb,sizeof(struct _hrb));
    hrb->hrb$l_lbn=lbn;
    hrb->hrb$l_cmd_time=scs1->scs$l_rspid; // wrong, but have to have some id...
    insque(hrb,&hrbq);
    if (basic->mscp$b_opcode == MSCP$K_OP_WRITE) {
      basic->mscp$b_opcode = MSCP$K_OP_END;
      cdrp->cdrp$w_cdrpsize=600;
      cdrp->cdrp$l_cdt=c;
      cdrp->cdrp$l_msg_buf=basic;
      cdrp->cdrp$l_xct_len=512;
      //scs_std$senddg(0,600,cdrp);
    }
    if (basic->mscp$b_opcode == MSCP$K_OP_READ) {
    }
  }
  return;

 read:
  {
    char * buf = vmalloc(1024);
    struct _irp * i=kmalloc(sizeof(struct _irp),GFP_KERNEL);
    struct _ucb * u;
    struct _hrb * hrb = find_hrb(scs1->scs$l_rspid);
    remque(hrb,hrb);
    bzero(i,sizeof(struct _irp));
    iosb->iosb$w_status=0;
    i->irp$b_type=DYN$C_IRP;
    i->irp$w_chan=chan;
    i->irp$l_func=IO$_READLBLK;
    i->irp$l_iosb=iosb;
    i->irp$l_qio_p1=buf;
    i->irp$l_qio_p2=512;
    i->irp$l_qio_p3=hrb->hrb$l_lbn;
    ((struct _acb *)i)->acb$b_rmod|=ACB$M_NODELETE; // bad idea to free this
    //printk("chan %x\n",chan);
    u=ctl$gl_ccbbase[chan].ccb$l_ucb;
    i->irp$l_ucb=u;
    i->irp$l_pid=current->pcb$l_pid;
    i->irp$l_sts|=IRP$M_BUFIO;
    i->irp$l_astprm=i;
    i->irp$l_ast=returnsome;
    i->irp$l_cdt=c;
    i->irp$l_rspid=scs1->scs$l_rspid;
    exe$insioq(i,u);
  }
  return;
 write:
  {
    struct _irp * i=kmalloc(sizeof(struct _irp),GFP_KERNEL);
    struct _ucb * u;
    struct _hrb * hrb = find_hrb(scs1->scs$l_rspid);
    remque(hrb,hrb);
    bzero(i,sizeof(struct _irp));
    iosb->iosb$w_status=0;
    i->irp$b_type=DYN$C_IRP;
    i->irp$w_chan=chan;
    i->irp$l_func=IO$_WRITELBLK;
    i->irp$l_iosb=iosb;
    i->irp$l_qio_p1=next;
    i->irp$l_qio_p2=512;
    i->irp$l_qio_p3=hrb->hrb$l_lbn;
    //printk("chan %x\n",chan);
    u=ctl$gl_ccbbase[chan].ccb$l_ucb;
    i->irp$l_ucb=u;
    i->irp$l_pid=current->pcb$l_pid;
    i->irp$l_sts|=IRP$M_BUFIO;
    i->irp$l_astprm=i;
    i->irp$l_ast=returnsome;
    i->irp$l_cdt=c;
    i->irp$l_rspid=scs1->scs$l_rspid;
    exe$insioq(i,u);
  }
  return;
}

extern struct _pdt dupdt;

void returnsome(struct _irp * i) {
  struct _cdrp * cdrp = vmalloc(sizeof(struct _cdrp));
  struct _mscp_basic_pkt * basic = vmalloc(sizeof(struct _mscp_basic_pkt));
  bzero(cdrp,sizeof(struct _cdrp));
  bzero(basic,sizeof(struct _mscp_basic_pkt));
  //basic->mscp$b_caa=MSCP$K_OP_READ;
  basic->mscp$b_opcode = MSCP$K_OP_END;
  basic->mscp$l_cmd_ref=0;
  //bcopy(i->irp$l_qio_p1,(unsigned long)basic + sizeof(struct _transfer_commands),512);
  cdrp->cdrp$w_cdrpsize=600;
  cdrp->cdrp$l_rspid=i->irp$l_rspid;
  cdrp->cdrp$l_cdt=i->irp$l_cdt;
  cdrp->cdrp$l_msg_buf=i->irp$l_qio_p1;
  cdrp->cdrp$l_xct_len=512;
  scs_std$senddata(&dupdt,cdrp,0);
  return;
  cdrp->cdrp$l_msg_buf=basic;
  cdrp->cdrp$l_xct_len=sizeof(struct _mscp_basic_pkt);
  scs_std$senddg(0,600,cdrp);
}

void mscpdaemonize() { }

/* mscp.exe mscp$disk let it be a kernel_thread? maybe not... */
int mscp(void) {
  char myname[]="mscp$disk";
  char myinfo[]="mscp disk";
  mscpdaemonize(); /* find out what this does */

  //  listen(msgbuf,err,cdt,pdt,cdt);
  scs_std$listen(mscplisten,mscpmyerr,myname,myinfo,0);
}


