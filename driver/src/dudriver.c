// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/vmalloc.h>

#include<acbdef.h>
#include<cddbdef.h>
#include<cdrpdef.h>
#include<cdtdef.h>
#include<crbdef.h>
#include<dcdef.h>
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
#include<rddef.h>

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
  scs_std$listen(dulisten,dumyerr,myname,myinfo,0);
}

struct _pdt dupdt;

struct _cddb ducddb;

struct _fdt du$fdt = {
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
    struct _scs_rd *r;
    c=(struct _cdrp *) & i->irp$l_fqfl;
    c->cdrp$l_rwcptr=&u->ucb$w_rwaitcnt;
    c->cdrp$l_cdt=((struct _mscp_ucb *)u)->ucb$l_cdt;
    c->cdrp$l_rspid=scs_std$alloc_rspid(0,0,c,0);
    scs_std$find_rdte( c->cdrp$l_rspid, &r);
    r->rd$l_cdrp=c;
    m=vmalloc(1000 /*sizeof(struct _transfer_commands)*/);
    bzero(m,1000 /*sizeof(struct _transfer_commands)*/);
    ((struct _mscp_basic_pkt *)m)->mscp$l_cmd_ref=c->cdrp$l_rspid;
    ((struct _mscp_basic_pkt *)m)->mscp$w_unit=((struct _mscp_ucb *)u)->ucb$w_mscpunit;
    ((struct _mscp_basic_pkt *)m)->mscp$b_caa=u->ucb$l_ddb->ddb$t_name[1]; // use this as unit type?
    c->cdrp$w_cdrpsize=600; //wrong, but we do not use a bufferdescriptor
    c->cdrp$l_msg_buf=m; // ??
    c->cdrp$l_xct_len=512;
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
void  du_cancel (void) { };
static void  ioc_std$cancelio (void) { };
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


struct _ddt du$ddt = {
  ddt$l_start: du_startio,
  ddt$l_unsolint: du_unsolint,
  ddt$l_fdt: &du$fdt,
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

void du$struc_init (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb) {
  ucb->ucb$b_flck=IPL$_IOLOCK8;
  ucb->ucb$b_dipl=IPL$_IOLOCK8;

  ucb->ucb$l_devchar = DEV$M_REC | DEV$M_AVL | DEV$M_CCL /*| DEV$M_OOV*/;

  ucb->ucb$l_devchar2 = DEV$M_NNM;
  ucb->ucb$b_devclass = DC$_MISC;
  ucb->ucb$b_devtype = DT$_TTYUNKN;
  ucb->ucb$w_devbufsiz = 132;

  ucb->ucb$l_devdepend = 99; // just something to fill

  // dropped the mutex stuff

  return;
}

void du$struc_reinit (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb) {
  ddb->ddb$ps_ddt=&du$ddt;
  //dpt_store_isr(crb,nl_isr);
  return;
}

int du$unit_init (struct _idb * idb, struct _ucb * ucb) {
  ucb->ucb$v_online = 0;
  //ucb->ucb$l_lr_msg_tmo = 0 ; // or offline? // where did this go?

  // idb->idb$ps_owner=&(ucb->ucb$r_ucb); // this is mailbox?
  // no adp or cram stuff

  // or ints etc
  
  ucb->ucb$v_online = 1;

  return SS$_NORMAL;
}

struct _dpt du$dpt;
struct _ddb du$ddb;
struct _mscp_ucb du$ucb;
struct _crb du$crb;

extern inline void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn, unsigned long);

int du$init_tables() {
  ini_dpt_name(&du$dpt, "DUDRIVER");
  ini_dpt_adapt(&du$dpt, 0);
  ini_dpt_defunits(&du$dpt, 1);
  ini_dpt_ucbsize(&du$dpt,sizeof(struct _ucb));
  ini_dpt_struc_init(&du$dpt, du$struc_init);
  ini_dpt_struc_reinit(&du$dpt, du$struc_reinit);
  ini_dpt_ucb_crams(&du$dpt, 1/*NUMBER_CRAMS*/);
  ini_dpt_end(&du$dpt);

  ini_ddt_unitinit(&du$ddt, du$unit_init);
  ini_ddt_start(&du$ddt, du_startio);
  ini_ddt_cancel(&du$ddt, ioc_std$cancelio);
  ini_ddt_end(&du$ddt);
 
  /* for the fdt init part */
  /* a lot of these? */
  ini_fdt_act(&du$fdt,IO$_ACCESS,acp_std$access,1);
  ini_fdt_act(&du$fdt,IO$_READLBLK,acp_std$readblk,1);
  ini_fdt_act(&du$fdt,IO$_READPBLK,acp_std$readblk,1);
  ini_fdt_act(&du$fdt,IO$_READVBLK,acp_std$readblk,1);
  ini_fdt_act(&du$fdt,IO$_WRITELBLK,acp_std$writeblk,1);
  ini_fdt_act(&du$fdt,IO$_WRITEPBLK,acp_std$writeblk,1);
  ini_fdt_act(&du$fdt,IO$_WRITEVBLK,acp_std$writeblk,1);
  ini_fdt_act(&du$fdt,IO$_CREATE,acp_std$access,1);
  ini_fdt_act(&du$fdt,IO$_DEACCESS,acp_std$deaccess,1);
  ini_fdt_act(&du$fdt,IO$_DELETE,acp_std$modify,1);
  ini_fdt_act(&du$fdt,IO$_MODIFY,acp_std$modify,1);
  ini_fdt_act(&du$fdt,IO$_ACPCONTROL,acp_std$modify,1);
  ini_fdt_act(&du$fdt,IO$_MOUNT,acp_std$mount,1);
  ini_fdt_end(&du$fdt);

  return SS$_NORMAL;
}

void * find_free_cdt(void);

extern struct _pb mypb;
extern struct _sb othersb;

void du_msg(void * packet, struct _cdt * c, struct _pdt * p) {
  du_dg(packet,c,p);
}

extern struct _scs_rd rdtl[128];

void du_dg(void * packet, struct _cdt * c, struct _pdt * p) {
  int sts;
  struct _iosb iosb;
  struct _scs * scs = packet;
  struct _scs1 * scs1 = scs;
  struct _ppd * ppd = scs;
  struct _mscp_basic_pkt * basic = ((unsigned long)packet) + sizeof(*scs);
  struct _transfer_commands * trans = basic;
  void * next = basic;
  unsigned long lbn=trans->mscp$l_lbn;
  char * buf;
  struct _scs_rd * rd=&rdtl[scs1->scs$l_rspid];
  struct _cdrp * cdrp = rd->rd$l_cdrp;
  struct _irp dummyirp;
  struct _irp * irp = ((unsigned long)cdrp)-((unsigned long)((unsigned long)&dummyirp.irp$l_fqfl-(unsigned long)&dummyirp));
  struct _acb * a=kmalloc(sizeof(struct _acb),GFP_KERNEL);
  bzero(a,sizeof(struct _acb));

  if (ppd->ppd$b_opc==PPD$C_SNDDAT) {
    struct _scs_rd * rd=&rdtl[scs1->scs$l_rspid];
    struct _cdrp * cdrp = rd->rd$l_cdrp;
    bcopy(next,cdrp->cdrp$l_msg_buf,512);
  }

  //a->acb$l_ast=((struct _cdrp *)c->cdt$l_fp_scs_norecv)->cdrp$l_fpc;
  //a->acb$l_astprm=((struct _cdrp *)c->cdt$l_fp_scs_norecv)->cdrp$l_fr3;
  //sch$qast(c->cdt$l_reserved3,PRI$_IOCOM,a);
  a->acb$l_ast=cdrp->cdrp$l_fpc;
  a->acb$l_astprm=cdrp->cdrp$l_fr3;
  sch$qast(irp->irp$l_pid,PRI$_IOCOM,a);
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

int du_iodb_vmsinit(void) {
#if 0
  struct _ucb * ucb=&du$ucb;
  struct _ddb * ddb=&du$ddb;
  struct _crb * crb=&du$crb;
#endif
  struct _ucb * ucb=kmalloc(sizeof(struct _mscp_ucb),GFP_KERNEL);
  struct _ddb * ddb=kmalloc(sizeof(struct _ddb),GFP_KERNEL);
  struct _crb * crb=kmalloc(sizeof(struct _crb),GFP_KERNEL);
  unsigned long idb=0,orb=0;
  struct _ccb * ccb;
  struct _ucb * newucb,newucb1;
//  ioc_std$clone_ucb(&du$ucb,&ucb);
  bzero(ucb,sizeof(struct _mscp_ucb));
  bzero(ddb,sizeof(struct _ddb));
  bzero(crb,sizeof(struct _crb));

#if 0
  init_ddb(&du$ddb,&du$ddt,&du$ucb,"dua");
  init_ucb(&du$ucb, &du$ddb, &du$ddt, &du$crb);
  init_crb(&du$crb);
#endif
  init_ddb(ddb,&du$ddt,ucb,"dua");
  init_ucb(ucb, ddb, &du$ddt, crb);
  init_crb(crb);

  du$init_tables();
  du$struc_init (crb, ddb, idb, orb, ucb);
  du$struc_reinit (crb, ddb, idb, orb, ucb);
  du$unit_init (idb, ucb);

  insertdevlist(ddb);

  return ddb;
}

void dupdt_init(void) {
  // not yet? dupdt.pdt$l_ucb0=u;
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
}

void  du_iodb_clu_vmsinit(struct _ucb * u) {
  struct _cddb * cddb;
  struct _pb * pb;
  struct _cdt * cdt;

  pb=kmalloc(sizeof(struct _pb),GFP_KERNEL);
  bzero(pb,sizeof(struct _pb));

  cddb=kmalloc(sizeof(struct _cddb),GFP_KERNEL);
  bzero(cddb,sizeof(struct _cddb));
  
  qhead_init(&cddb->cddb$l_cdrpqfl);
  cddb->cddb$l_pdt=&dupdt;
  ((struct _mscp_ucb *)u)->ucb$l_cddb=cddb;

  u->ucb$l_pdt=&dupdt;
  pb->pb$l_pdt=&dupdt;
  ((struct _mscp_ucb *)u)->ucb$l_cdt=find_mscp_cdt(); // should be find_free_cdt();
  cdt=((struct _mscp_ucb *)u)->ucb$l_cdt;
  cdt->cdt$l_pb=pb;
}

void __du_init(void) {
  struct _ddb * ddb;

  // specific cluster stuff here 

  ddb = du_iodb_vmsinit();
  du_iodb_clu_vmsinit(ddb->ddb$l_ucb);
  dupdt_init();
}

int du_iodbunit_vmsinit(struct _ddb * ddb,int unitno, void * d) {
  struct _ucb * newucb;

  ioc_std$clone_mscp_ucb(ddb->ddb$l_ucb/*&file$ucb*/,&newucb);
  //ioc_std$clone_mscp_ucb(/*ddb->ddb$l_ucb*/&du$ucb,&newucb);

  return newucb;
}

//extern struct _ucb file$ucb;

void * du_init(char *s) {
  struct _ucb * u=&du$ucb;
  struct _ddb * d=&du$ddb;
  struct _crb * c=&du$crb;
  struct _ucb * newucb;

  s++;

  /* this is a all-in-one, should be split later */

  mypb.pb$b_type=DYN$C_SCS_PB;
  mypb.pb$w_state=PB$C_CLOSED;
  othersb.sb$b_type=DYN$C_SCS_SB;

  ioc_std$clone_mscp_ucb(&du$ucb,&newucb);

  insertfillist(newucb,s);

  return newucb;
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
  u->ucb$l_cddb->cddb$l_pdt->pdtvec$l_senddg(0,600,&i->irp$l_fqfl);
  if (((struct _mscp_basic_pkt *)m)->mscp$b_opcode==MSCP$K_OP_WRITE) { 
    scs_std$senddata(&dupdt,&i->irp$l_fqfl,0);
  } else {
    scs_std$reqdata(&dupdt,&i->irp$l_fqfl,0);
  }
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
