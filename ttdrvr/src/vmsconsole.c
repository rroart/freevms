// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004. Based on console.c.

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

#include<linux/blkdev.h>
#include<linux/console.h>

static struct _irp * globali;
static struct _ucb * globalu;

static void  startio3 (struct _irp * i, struct _ucb * u) { 
  ioc$reqcom(SS$_NORMAL,0,u);
  return;
};

static void  startio2 (struct _irp * i, struct _ucb * u) { 
  u->ucb$l_fpc=startio3;
  exe$iofork(i,u);
  return;
}

static void ubd_intr2(int irq, void *dev, struct pt_regs *unused)
{
  struct _irp * i;
  struct _ucb * u;
  void (*func)();

  if (intr_blocked(20))
    return;
  regtrap(REG_INTR,20);
  setipl(20);
  /* have to do this until we get things more in order */
  i=globali;
  u=globalu;

  func=u->ucb$l_fpc;
  func(i,u);
  myrei();
}

static struct _fdt op$fdt = {
  fdt$q_valid:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_WRITECHECK|IO$_READPBLK|IO$_WRITELBLK|IO$_DSE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_READRCT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_SETPRFPATH|IO$_FORMAT,
  fdt$q_buffered:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_DSE|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_FORMAT
};

/* more yet undefined dummies */
int con$startio (int a,int b ) { }
static void  unsolint (void) { };
static void  cancel (void) { };
static void  ioc_std$cancelio (void) { };
static void  regdump (void) { };
static void  diagbuf (void) { };
static void  errorbuf (void) { };
static void  unitinit (void) { };
int  con$wrtstartio (int a,int b) { }
static void  mntver (void) { };
static void  cloneducb (void) { };
static void  mntv_sssc (void) { };
static void  mntv_for (void) { };
static void  mntv_sqd (void) { };
static void  aux_storage (void) { };
static void  aux_routine (void) { };

static struct _ddt op$ddt = {
  ddt$l_start: con$startio,
  ddt$l_unsolint: unsolint,
  ddt$l_fdt: &op$fdt,
  ddt$l_cancel: cancel,
  ddt$l_regdump: regdump,
  ddt$l_diagbuf: diagbuf,
  ddt$l_errorbuf: errorbuf,
  ddt$l_unitinit: unitinit,
  ddt$l_altstart: con$wrtstartio,
  ddt$l_mntver: mntver,
  ddt$l_cloneducb: cloneducb,
  ddt$w_fdtsize: 0,
  ddt$l_mntv_sssc: mntv_sssc,
  ddt$l_mntv_for: mntv_for,
  ddt$l_mntv_sqd: mntv_sqd,
  ddt$l_aux_storage: aux_storage,
  ddt$l_aux_routine: aux_routine
};

#if 0
static char *vidmem = (char *)0xb8000;
static int vidport=0x3d4;
static int lines=25, mycols=80, orig_x=1, orig_y=24;

static void scroll(void)
{
  int i;

  memcpy ( vidmem, vidmem + mycols * 2, ( lines - 1 ) * mycols * 2 );
  for ( i = ( lines - 1 ) * mycols * 2; i < lines * mycols * 2; i += 2 )
    vidmem[i] = ' ';
}

static void puts(const char *s)
{
  int x,y,pos;
  char c;

  x=orig_x;
  y=orig_y;

  while ( ( c = *s++ ) != '\0' ) {
    if ( c == '\n' ) {
      x = 0;
      if ( ++y >= lines ) {
	scroll();
	y--;
      }
    } else {
      vidmem [ ( x + mycols * y ) * 2 ] = c; 
      if ( ++x >= mycols ) {
	x = 0;
	if ( ++y >= lines ) {
	  scroll();
	  y--;
	}
      }
    }
  }

  orig_x = x;
  orig_y = y;

  pos = (x + mycols * y) * 2;       /* Update cursor position */
  outb_p(14, vidport);
  outb_p(0xff & (pos >> 9), vidport+1);
  outb_p(15, vidport);
  outb_p(0xff & (pos >> 1), vidport+1);
}
#endif

extern struct tty_driver console_driver;

long pidtab[1024];
long pididx=0;

int con$fdtread(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  //  return read(0,i->irp$l_qio_p1,i->irp$l_qio_p2);
  return kbd$fdtread(i,p,u,c);
}

int con$fdtwrite(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  //  return write(1,i->irp$l_qio_p1,i->irp$l_qio_p2);
  //puts("test\n");
  //puts(i->irp$l_qio_p1);
  struct tty_struct * tty ;

  //  init_dev2(chan2dev(i->irp$w_chan ,&tty));
  //printk("dev %x %x\n",chan2dev(i->irp$w_chan),i->irp$w_chan);
  //init_dev2(0x0401,&tty);
  console_driver.write(i,p,u,c);
#if 1
  // this must be put back sometime
  u->ucb$l_irp=i;
  u->ucb$l_irp->irp$l_iost1 = SS$_NORMAL;
  com$post(u->ucb$l_irp,u);
#endif
  return SS$_NORMAL;
}

extern void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn, unsigned long type);

void op$struc_init (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb) {
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

void op$struc_reinit (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb) {
  ddb->ddb$ps_ddt=&op$ddt;
  //dpt_store_isr(crb,nl_isr);
  return;
}

int op$unit_init (struct _idb * idb, struct _ucb * ucb) {
  ucb->ucb$v_online = 0;
  //ucb->ucb$l_lr_msg_tmo = 0 ; // or offline? // where did this go?

  // idb->idb$ps_owner=&(ucb->ucb$r_ucb); // this is mailbox?
  // no adp or cram stuff

  // or ints etc
  
  ucb->ucb$v_online = 1;

  return SS$_NORMAL;
}

struct _dpt op$dpt;
struct _ddb op$ddb;
struct _ucb op$ucb;
struct _crb op$crb;

int op$init_tables() {
  ini_dpt_name(&op$dpt, "OPDRIVER");
  ini_dpt_adapt(&op$dpt, 0);
  ini_dpt_defunits(&op$dpt, 1);
  ini_dpt_ucbsize(&op$dpt,sizeof(struct _ucb));
  ini_dpt_struc_init(&op$dpt, op$struc_init);
  ini_dpt_struc_reinit(&op$dpt, op$struc_reinit);
  ini_dpt_ucb_crams(&op$dpt, 1/*NUMBER_CRAMS*/);
  ini_dpt_end(&op$dpt);

  ini_ddt_unitinit(&op$ddt, op$unit_init);
  ini_ddt_start(&op$ddt, con$startio);
  ini_ddt_cancel(&op$ddt, ioc_std$cancelio);
  ini_ddt_end(&op$ddt);

  /* for the fdt init part */
  /* a lot of these? */
  ini_fdt_act(&op$fdt,IO$_READLBLK,con$fdtread,1);
  ini_fdt_act(&op$fdt,IO$_READPBLK,con$fdtread,1);
  ini_fdt_act(&op$fdt,IO$_READVBLK,con$fdtread,1);
  ini_fdt_act(&op$fdt,IO$_WRITELBLK,con$fdtwrite,1);
  ini_fdt_act(&op$fdt,IO$_WRITEPBLK,con$fdtwrite,1);
  ini_fdt_act(&op$fdt,IO$_WRITEVBLK,con$fdtwrite,1);
  ini_fdt_end(&op$fdt);

  return SS$_NORMAL;
}

int con_iodb_vmsinit(void) {
#if 0
  struct _ucb * ucb=&op$ucb;
  struct _ddb * ddb=&op$ddb;
  struct _crb * crb=&op$crb;
#endif 
  struct _ucb * ucb=kmalloc(sizeof(struct _ucb),GFP_KERNEL);
  struct _ddb * ddb=kmalloc(sizeof(struct _ddb),GFP_KERNEL);
  struct _crb * crb=kmalloc(sizeof(struct _crb),GFP_KERNEL);
  unsigned long idb=0,orb=0;

  bzero(ucb,sizeof(struct _ucb));
  bzero(ddb,sizeof(struct _ddb));
  bzero(crb,sizeof(struct _crb));

#if 0
  init_ddb(&op$ddb,&op$ddt,&op$ucb,"dqa");
  init_ucb(&op$ucb, &op$ddb, &op$ddt, &op$crb);
  init_crb(&op$crb);
#endif

  init_ddb(ddb,&op$ddt,ucb,"opa");
  init_ucb(ucb, ddb, &op$ddt, crb);
  init_crb(crb);

//  ioc_std$clone_ucb(&op$ucb,&ucb);
  op$init_tables();
  op$struc_init (crb, ddb, idb, orb, ucb);
  op$struc_reinit (crb, ddb, idb, orb, ucb);
  op$unit_init (idb, ucb);

  insertdevlist(ddb);

  return ddb;

}

int con_iodbunit_vmsinit(struct _ddb * ddb,int unitno,void * dsc) {
  unsigned short int chan;
  struct _ucb * newucb;
  ioc_std$clone_ucb(ddb->ddb$ps_ucb/*&op$ucb*/,&newucb);
  exe$assign(dsc,&chan,0,0,0);
  registerucbchan(newucb,chan);
  registerdevchan(MKDEV(TTY_MAJOR,unitno),chan);


  return newucb;
}

int con_vmsinit(void) {
  //struct _ucb * u=makeucbetc(&ddb,&ddt,&dpt,&fdt,"hda","hddriver");

  unsigned short chan0, chan1, chan2;
  $DESCRIPTOR(dsc,"opa0");
  unsigned long idb=0,orb=0;
  struct _ccb * ccb;
  struct _ucb * newucb0,*newucb1,*newucb2;
  struct _ddb * ddb;

  printk(KERN_INFO "dev con here pre\n");

  ddb=con_iodb_vmsinit();

  /* for the fdt init part */
  /* a lot of these? */

  con_iodbunit_vmsinit(ddb,0,&dsc);
  con_iodbunit_vmsinit(ddb,1,&dsc);

  printk(KERN_INFO "dev con here\n");

  // return chan0;

}

unsigned int video_font_height;
unsigned int default_font_height;
unsigned int video_scan_lines;
       int sel_cons = 0;		/* must not be disallocated */

void clear_selection(void) { }

struct vt_struct *vt_cons[MAX_NR_CONSOLES];

int (*kbd_rate)(struct kbd_repeat *rep);
