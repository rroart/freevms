// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/vmalloc.h>

#include<acbdef.h>
#include<cddbdef.h>
#include<cdrpdef.h>
#include<cdtdef.h>
#include<crbdef.h>
#include<ddbdef.h>
#include<ddtdef.h>
#include<devdef.h>
#include<dptdef.h>
#include<dyndef.h>
#include<fdtdef.h>
#include<iodef.h>
#include<iosbdef.h>
#include<ipldef.h>
#include<irpdef.h>
#include<mscpdef.h>
#include<pbdef.h>
#include<pdtdef.h>
#include<pridef.h>
#include<sbdef.h>
#include<scsdef.h>
#include<ssdef.h>
#include<ucbdef.h>

int acp_std$modify(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int acp_std$mount(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int acp_std$access(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int acp_std$deaccess(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

void dumyerr(void) {
  /* do nothing yet */
}

int dulisten(void * packet, struct _cdt * c, struct _pdt * p) {

}

void dudaemonize(void) { }

/* mscp.exe mscp$disk let it be a kernel_thread? maybe not... */
int dumscp(void) {
  char myname[]="mscp$disk";
  char myinfo[]="mscp disk";
  dudaemonize(); /* find out what this does */
  
  //  listen(msgbuf,err,cdt,pdt,cdt);
  scs$listen(dulisten,dumyerr,myname,myinfo,0);
}

struct _pdt dupdt;

struct _cddb ducddb;

struct _fdt fdt_du = {
  fdt$q_valid:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_WRITECHECK|IO$_READPBLK|IO$_WRITELBLK|IO$_DSE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_READRCT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_SETPRFPATH|IO$_FORMAT,
  fdt$q_buffered:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_DSE|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_FORMAT
};

void  du_startio3 (struct _irp * i, struct _ucb * u) { 
  //printk("thirdtime %x %x\n",i,u);
  ioc$reqcom(SS$_NORMAL,0,u);
  return;
};

void  du_startio2 (struct _irp * i, struct _ucb * u) { 
  //printk("secondtime\n");

  u->ucb$l_fpc=du_startio3;
  exe$iofork(i,u);
  return;
}

void  du_startio (struct _irp * i, struct _ucb * u) { 
  struct _transfer_commands * m;
  if (1 || u->ucb$l_devchar2&DEV$M_CDP) { // does this anyway
    /* not local? */
    struct _cdrp * c;
    c=(struct _cdrp *) & i->irp$l_fqfl;
    c->cdrp$l_rwcptr=&u->ucb$w_rwaitcnt;
    c->cdrp$l_cdt=((struct _mscp_ucb *)u)->ucb$l_cdt;
    c->cdrp$l_rspid=rspid_alloc(c);
    m=vmalloc(1000 /*sizeof(struct _transfer_commands)*/);
    bzero(m,1000 /*sizeof(struct _transfer_commands)*/);
    ((struct _mscp_basic_pkt *)m)->mscp$l_cmd_ref=c->cdrp$l_rspid;
    ((struct _mscp_basic_pkt *)m)->mscp$w_unit=((struct _mscp_ucb *)u)->ucb$w_mscpunit;
    c->cdrp$w_cdrpsize=600; //wrong, but we do not use a bufferdescriptor
    c->cdrp$l_msg_buf=m; // ??
    c->cdrp$l_cdt=((struct _mscp_ucb *)u)->ucb$l_cdt;
    ioc$wfikpch(du_startio2,0,i,current,u,2,0);
    switch (i->irp$v_fcode) {

    case IO$_READLBLK :
      return du_readblk(i,u,m);
      break;
    case IO$_READPBLK :
      return du_readblk(i,u,m);
      break;
    case IO$_READVBLK :
      printk("should not be here in startio\n");
      break;
    case IO$_WRITELBLK :
      return du_writeblk(i,u,m);
      break;
    case IO$_WRITEPBLK :
      return du_writeblk(i,u,m);
      break;
    case IO$_WRITEVBLK :
      printk("should not be here in startio\n");
      break;
    case IO$_ACCESS :
      printk("should not be here in startio\n");
      break;
    case IO$_CREATE :
      printk("should not be here in startio\n");
      break;
    case IO$_DEACCESS :
      printk("should not be here in startio\n");
      break;
    case IO$_DELETE :
      printk("should not be here in startio\n");
      break;
    case IO$_MODIFY :
      printk("should not be here in startio\n");
      break;
    case IO$_ACPCONTROL :
      printk("should not be here in startio\n");
      break;
    case IO$_MOUNT :
      printk("not ready yet in startio\n");
      break;

    default:
      printk("du startio unknown %x\n",i->irp$v_fcode);
      break;
    }
  } else {
    /* local */
    /* error if it gets here, got no real mscp */
    panic("no real mscp\n");
  }
};

/* more yet undefined dummies */
void  du_unsolint (void) { };
void  du_functb (void) { };
void  du_cancel (void) { };
void  du_regdump (void) { };
void  du_diagbuf (void) { };
void  du_errorbuf (void) { };
void  du_unitinit (void) { };
void  du_altstart (void) { };
void  du_mntver (void) { };
void  du_cloneducb (void) { };
void  du_mntv_sssc (void) { };
void  du_mntv_for (void) { };
void  du_mntv_sqd (void) { };
void  du_aux_storage (void) { };
void  du_aux_routine (void) { };


struct _ddt ddt_du = {
  ddt$l_start: du_startio,
  ddt$l_unsolint: du_unsolint,
  ddt$l_functb: du_functb,
  ddt$l_cancel: du_cancel,
  ddt$l_regdump: du_regdump,
  ddt$l_diagbuf: du_diagbuf,
  ddt$l_errorbuf: du_errorbuf,
  ddt$l_unitinit: du_unitinit,
  ddt$l_altstart: du_altstart,
  ddt$l_mntver: du_mntver,
  ddt$l_cloneducb: du_cloneducb,
  ddt$w_fdtsize: 0,
  ddt$l_mntv_sssc: du_mntv_sssc,
  ddt$l_mntv_for: du_mntv_for,
  ddt$l_mntv_sqd: du_mntv_sqd,
  ddt$l_aux_storage: du_aux_storage,
  ddt$l_aux_routine: du_aux_routine
};

extern int acp_std$readblk();
extern int acp_std$writeblk();
extern int scs_std$sendmsg();
extern int scs_std$allocmsg();
extern int scs_std$deallomsg();
extern int scs_std$reqdata();
extern int scs_std$senddata();
extern int scs_std$sendmsg();
extern int scs_std$senddg();
extern int scs_std$connect();
extern int scs_std$dconnect();

struct _dpt du_dpt;

void * find_free_cdt(void);

extern struct _pb mypb;
extern struct _sb mysb;

void du_msg(void * packet, struct _cdt * c, struct _pdt * p) {
  du_dg(packet,c,p);
}
void du_dg(void * packet, struct _cdt * c, struct _pdt * p) {
  int sts;
  struct _iosb iosb;
  struct _cdrp * cdrp;
  struct _scs * scs = packet;
  struct _mscp_basic_pkt * basic = ((unsigned long)packet) + sizeof(*scs);
  struct _transfer_commands * trans = basic;
  unsigned long lbn=trans->mscp$l_lbn;
  char * buf;
  struct _acb * a=vmalloc(sizeof(struct _acb));
  bzero(a,sizeof(struct _acb));
  if (basic->mscp$b_caa==MSCP$K_OP_READ) {
    bcopy((unsigned long)basic+sizeof(*trans),c->cdt$l_reserved4,512);
  }
  a->acb$l_ast=((struct _cdrp *)c->cdt$l_fp_scs_norecv)->cdrp$l_fpc;
  a->acb$l_astprm=((struct _cdrp *)c->cdt$l_fp_scs_norecv)->cdrp$l_fr3;
  sch$qast(c->cdt$l_reserved3,PRI$_IOCOM,a);
}

void du_err() {
}

#if 0
int mscpcli(void) {
  while (1) {
    if (mypb.pb$w_state==PB$C_OPEN) goto out;
    sleep(10);
  }
 out:
  du_init(0);
 out2:
  sleep(1000);
  goto out2;
}
#endif

extern inline void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn, unsigned long);

void * du_init(char *s) {
  struct _ucb * u;
  struct _ddb * d;
  struct _crb * c;
  struct _cddb * cddb;
  struct _pb * pb;
  struct _cdt * cdt;

  s++;

  u=vmalloc(sizeof(struct _mscp_ucb));
  bzero(u,sizeof(struct _mscp_ucb));
  d=vmalloc(sizeof(struct _ddb));
  bzero(d,sizeof(struct _ddb));
  c=vmalloc(sizeof(struct _crb));
  bzero(c,sizeof(struct _crb));
  bzero(u,sizeof(struct _ucb));
  bzero(d,sizeof(struct _ddb));
  bzero(c,sizeof(struct _crb));

  pb=vmalloc(sizeof(struct _pb));
  bzero(pb,sizeof(struct _pb));

  cddb=vmalloc(sizeof(struct _cddb));
  bzero(u,sizeof(struct _cddb));
  
  qhead_init(&cddb->cddb$l_cdrpqfl);
  cddb->cddb$l_pdt=&dupdt;
  ((struct _mscp_ucb *)u)->ucb$l_cddb=c;
  u->ucb$l_pdt=&dupdt;
  pb->pb$l_pdt=&dupdt;
  ((struct _mscp_ucb *)u)->ucb$l_cdt=find_mscp_cdt(); // should be find_free_cdt();
  cdt=((struct _mscp_ucb *)u)->ucb$l_cdt;
  cdt->cdt$l_pb=pb;

  dupdt.pdt$l_ucb0=u;
  dupdt.pdtvec$l_sendmsg=scs_std$sendmsg;
  dupdt.pdtvec$l_senddg=scs_std$senddg;
  dupdt.pdtvec$l_allocmsg=scs_std$allocmsg;
  dupdt.pdtvec$l_deallomsg=scs_std$deallomsg;
  dupdt.pdtvec$l_reqdata=scs_std$reqdata;
  dupdt.pdtvec$l_senddata=scs_std$senddata;
  dupdt.pdtvec$l_sendmsg=scs_std$sendmsg;
  dupdt.pdt$l_connect=scs_std$connect;
  dupdt.pdt$l_dconnect=scs_std$dconnect;
  qhead_init(&dupdt.pdt$q_comql);
  qhead_init(&dupdt.pdt$q_comqh);
  qhead_init(&dupdt.pdt$q_comq2);
  qhead_init(&dupdt.pdt$q_comq3);

  insertdevlist(d);
  insertfillist(u);

  /* this is a all-in-one, should be split later */

  /* for the dpt part */
  bcopy("DUDRIVER",du_dpt.dpt$t_name,8);
  du_dpt.dpt$ps_ddt=&ddt_du;

  /* for the ddb init part */
  //  d->ddb$ps_link=d;
  //  d->ddb$ps_blink=d;
  d->ddb$b_type=DYN$C_DDB;
  d->ddb$l_ddt=&ddt_du;
  d->ddb$ps_ucb=u;
  bcopy(s,d->ddb$t_name,strlen(s));

  /* for the ucb init part */
  qhead_init(&u->ucb$l_ioqfl);
  u->ucb$b_type=DYN$C_UCB;
  u->ucb$b_flck=IPL$_IOLOCK8;
  /* devchars? */
  u->ucb$b_devclass=DEV$M_RND; /* just maybe? */
  u->ucb$b_dipl=IPL$_IOLOCK8;
  //  bcopy("nla0",u->ucb$t_name,4);
  u->ucb$l_ddb=d;
  u->ucb$l_crb=c;
  u->ucb$l_ddt=&ddt_du;

  /* for the crb init part */
  c->crb$b_type=DYN$C_CRB;

  /* and for the ddt init part */
  ddt_du.ddt$l_fdt=&fdt_du;
  ddt_du.ddt$l_functb=&fdt_du;

  ((struct _mscp_ucb *)u)->ucb$l_cddb=cddb;
  qhead_init(&cddb->cddb$l_cdrpqfl);

  /* a lot of these? */
  ini_fdt_act(&fdt_du,IO$_READLBLK,acp_std$readblk,1);
  ini_fdt_act(&fdt_du,IO$_READPBLK,acp_std$readblk,1);
  ini_fdt_act(&fdt_du,IO$_READVBLK,acp_std$readblk,1);
  ini_fdt_act(&fdt_du,IO$_WRITELBLK,acp_std$writeblk,1);
  ini_fdt_act(&fdt_du,IO$_WRITEPBLK,acp_std$writeblk,1);
  ini_fdt_act(&fdt_du,IO$_WRITEVBLK,acp_std$writeblk,1);
  ini_fdt_act(&fdt_du,IO$_ACCESS,acp_std$access,1);
  ini_fdt_act(&fdt_du,IO$_CREATE,acp_std$access,1);
  ini_fdt_act(&fdt_du,IO$_DEACCESS,acp_std$deaccess,1);
  ini_fdt_act(&fdt_du,IO$_DELETE,acp_std$modify,1);
  ini_fdt_act(&fdt_du,IO$_MODIFY,acp_std$modify,1);
  ini_fdt_act(&fdt_du,IO$_ACPCONTROL,acp_std$modify,1);
  ini_fdt_act(&fdt_du,IO$_MOUNT,acp_std$mount,1);

  mypb.pb$b_type=DYN$C_SCS_PB;
  mypb.pb$w_state=PB$C_CLOSED;
  mysb.sb$b_type=DYN$C_SCS_SB;

  scs$connect(du_msg,du_dg,du_err,0,0,"mscp$disk","vms$disk_cl_drvr",0,0,0,0,s);

  return u;
}

char dudriverstring[]="DUDRIVER";

int testme = 3;

struct _dpt du_dpt = {
  //  dpt$t_name:"DUDRIVER"
  //  dpt$b_type:&testme,
  // dpt$t_name:dudriverstring  // have no idea why this won't compile
};

int du_readblk(struct _irp * i, struct _ucb * u, struct _mscp_basic_pkt * m) {
  m->mscp$b_opcode=MSCP$K_OP_READ;
  return du_rw(i,u,m);
}

int du_writeblk(struct _irp * i, struct _ucb * u, struct _mscp_basic_pkt * m) {
  m->mscp$b_opcode=MSCP$K_OP_WRITE;
  return du_rw(i,u,m);
}

void du_rw_more(struct _irp * i);

int du_rw(struct _irp * i, struct _mscp_ucb * u, struct _transfer_commands * m) {
  int sts;
  unsigned long *l = &m->mscp$b_buffer;
  m->mscp$l_byte_cnt = i->irp$l_qio_p2;  // change later
  *l= i->irp$l_qio_p1;
  m->mscp$l_lbn = i->irp$l_qio_p3;
  insque(&i->irp$l_fqfl,&u->ucb$l_cddb->cddb$l_cdrpqfl);
  i->irp$l_fpc=du_rw_more;
  i->irp$l_fr3=i;
  u->ucb$l_cddb->cddb$l_pdt->pdtvec$l_senddg(0,0,&i->irp$l_fqfl);
}

void du_rw_more(struct _irp * i) {
  struct _cdrp * cdrp = &i->irp$l_fqfl;
  unsigned int sts;
  void (*func)(void *,void *);
  bcopy((unsigned long)cdrp->cdrp$l_msg_buf, i->irp$l_qio_p1,i->irp$l_qio_p2);
  // receive something
  func=i->irp$l_ucb->ucb$l_fpc;
  func(i,i->irp$l_ucb);
  sts=SS$_NORMAL;
  return sts;
}
