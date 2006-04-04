// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <linux/init.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>

#include <ccbdef.h>
#include <crbdef.h>
#include <dcdef.h>
#include <ddbdef.h>
#include <ddtdef.h>
#include <devdef.h>
#include <dptdef.h>
#include <dyndef.h>
#include <fdtdef.h>
#include <internals.h>
#include <iodef.h>
#include <ipldef.h>
#include <irpdef.h>
#include <rsndef.h>
#include <ssdef.h>
#include <system_data_cells.h>
#include <ucbdef.h>
#include <descrip.h>
#include <vms_drivers.h>
#include <iosbdef.h>
#include <com_routines.h>
#include <queue.h>
#include <exe_routines.h>
#include <ioc_routines.h>
#include <misc_routines.h>
#include <sch_routines.h>
#include <linux/slab.h>

#define irp$l_nopartnerqfl irp$l_fqfl
#define irp$l_nopartnerqbl irp$l_fqbl

void mb$finishread(struct _ucb * u);

struct _dpt mb_dpt = { };

struct __mmb {
  struct __mmb * mmb$l_msgqfl;
  struct __mmb * mmb$l_msgqbl;
  unsigned short int mmb$w_size;
  unsigned char mmb$b_type;
  unsigned char mmb$b_func;
  struct _irp * mmb$l_irp;
  unsigned long mmb$l_pid;
  void * mmb$l_noreaderwaitqfl;
  void * mmb$l_noreaderwaitqbl;
  void * mmb$l_datastart;
  unsigned short mmb$w_datasize;
  short mmb$w_bufquochrg;
  char mmb$t_data[0];
};

struct __srb {
  void * srb$l_addr;
  void * srb$l_virtaddr;
  unsigned short int srb$w_size;
  unsigned char srb$b_type;
  unsigned char srb$b_func;
  void * srb$l_datastart;
  unsigned short int srb$w_datasize;
  unsigned short srb$w_reqsiz;
  short srb$w_bufquochrg;
  char srb$t_data[0];
};

int mb$fdt_setmode(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) { 
  int func=i->irp$l_func;
  struct _mb_ucb * mu=u;
  if (i->irp$l_func&IO$M_READERWAIT) {
    if (mu->ucb$l_mb_readerrefc==0)
      return exe$finishioc(SS$_NORMAL,i,p,u);
    insque(i,mu->ucb$l_mb_readerwaitqfl);
    return SS$_NORMAL;
  }
  if (i->irp$l_func&IO$M_WRITERWAIT) {
    if (mu->ucb$l_mb_writerrefc==0)
      return exe$finishioc(SS$_NORMAL,i,p,u);
    insque(i,mu->ucb$l_mb_writerwaitqfl);
    return SS$_NORMAL;
  }
  if (i->irp$l_func&IO$M_MB_ROOM_NOTIFY) {
    com_std$setattnast(i,p,u,c,mu->ucb$l_mb_room_notify);
    return exe$finishioc(SS$_NORMAL,i,p,u);
  }
  if (i->irp$l_func&IO$M_READATTN) {
    com_std$setattnast(i,p,u,c,&u->ucb$l_mb_r_ast);
    if (!aqempty(mu->ucb$l_mb_readqfl))
      com_std$delattnast(&u->ucb$l_mb_r_ast,u);
  } else {
    com_std$setattnast(i,p,u,c,&u->ucb$l_mb_w_ast);
    if (u->ucb$w_msgcnt)
      com_std$delattnast(&u->ucb$l_mb_w_ast,u);
  }

  return exe$finishioc(SS$_NORMAL,i,p,u);
}

int mb$fdt_sensemode(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) { 
  int retsts=SS$_NORMAL;
  int func=i->irp$l_func;
  struct _mb_ucb * mu=u;
  struct __mmb * head=&u->ucb$l_mb_msgqfl;
  struct __mmb * tmp=head->mmb$l_msgqfl;
  int sum=0;
  if (i->irp$l_func&IO$M_WRITERWAIT && mu->ucb$l_mb_writerrefc==0)
    retsts=SS$_NOWRITER;
  if (i->irp$l_func&IO$M_READERWAIT && mu->ucb$l_mb_readerrefc==0)
    retsts=SS$_NOREADER;
  while (tmp!=head) {
    sum+=tmp->mmb$w_datasize;
    tmp=tmp->mmb$l_msgqfl;
  }
  retsts=(retsts)+(u->ucb$w_msgcnt<<16);
  return exe$finishio(retsts,sum,i,p,u);
}

int mb$fdt_write (struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  int savipl;
  struct __mmb * m;
  struct _mb_ucb * mu=u
;

  if (c->ccb$l_sts&CCB$M_NOWRITEACC)
    return exe_std$abortio(i,p,u,SS$_ILLIOFUNC);

  if (!test_and_set_bit(CCB$V_WRTCHKDON,&c->ccb$l_sts)) {
    int sts=exe_std$chkwrtacces(0,0,p,u);
    if ((sts&1)==0)
      return exe_std$abortio(sts,i,p,u);
  }

  if (i->irp$l_qio_p2>u->ucb$w_devbufsiz)
    return exe_std$abortio(i,p,u,SS$_MBTOOSML);

  int func=i->irp$l_func;

  if ((func&IO$M_READERCHECK) && (mu->ucb$l_mb_readerrefc==0))
    return exe$finishioc(SS$_NOREADER,i,p,u);

  if ((func&IO$M_FCODE)==IO$_WRITEOF) {
    i->irp$l_boff=0;
    i->irp$l_bcnt=0;
    i->irp$l_media=&i->irp$l_media;
  } else {
    i->irp$l_bcnt=i->irp$l_qio_p2;
    i->irp$l_media=i->irp$l_qio_p1;
  }

  exe_std$writechk(i,p,u,i->irp$l_qio_p1, i->irp$l_qio_p2); 

  m=kmalloc(sizeof(struct __mmb)+i->irp$l_qio_p2,GFP_KERNEL);
  if (m) {
    memset(m,0,sizeof(struct __mmb)+i->irp$l_qio_p2);
    m->mmb$w_size=sizeof(struct __mmb)+i->irp$l_qio_p2;
    m->mmb$b_type=DYN$C_BUFIO;
    m->mmb$b_func=func;
    m->mmb$w_datasize=i->irp$l_qio_p2;
    qhead_init(&m->mmb$l_noreaderwaitqfl);
    if ((func&IO$M_NOW)==0) m->mmb$l_irp=i;
    m->mmb$l_pid=ctl$gl_pcb->pcb$l_pid;
    m->mmb$w_datasize=i->irp$l_bcnt;
    m->mmb$l_datastart=&m->mmb$t_data;
    if (m->mmb$w_datasize)
      memcpy(m->mmb$t_data,i->irp$l_qio_p1,i->irp$l_qio_p2);
  } else {
    if (i->irp$l_func & IO$M_NORSWAIT)
      return exe_std$iorsnwait(i,p,u,c,SS$_INSFMEM, RSN$_NPDYNMEM);
    else
      return exe_std$abortio(i,p,u,SS$_INSFMEM);
  }
  
  savipl=vmslock(&SPIN_MAILBOX,IPL$_MAILBOX);

  if (i->irp$l_qio_p2>u->ucb$w_bufquo && aqempty(mu->ucb$l_mb_readqfl)) { // or msgqfl?

    kfree(m);

    if (i->irp$l_bcnt > u->ucb$w_iniquo) {
      return exe_std$abortio(i,p,u,SS$_MBTOOSML);
    }
    
    if (i->irp$l_func & IO$M_NORSWAIT)
      return exe_std$iorsnwait(i,p,u,c,SS$_MBFULL, RSN$_MAILBOX);
    else
      return exe_std$abortio(i,p,u,SS$_MBFULL);
    
  }

  u->ucb$w_msgcnt++;
  u->ucb$l_devdepend=u->ucb$w_msgcnt;

  u->ucb$w_bufquo-=m->mmb$w_datasize;
  if (m->mmb$w_datasize==0 || ((func&IO$M_FCODE)==IO$_WRITEOF))
    u->ucb$w_bufquo--;

  insque(m,u->ucb$l_mb_msgqbl);

  //  if (u->ucb$l_sts & UCB$M_BSY)
  if (!aqempty(mu->ucb$l_mb_readqfl))
    mb$finishread(u);
  else
    {
      if ((func&IO$M_NOW)==0 || (func&IO$M_READERCHECK && mu->ucb$l_mb_readerrefc==0))
	insque(&m->mmb$l_noreaderwaitqfl,mu->ucb$l_mb_noreaderwaitqfl);
      com_std$delattnast(&u->ucb$l_mb_w_ast, u);
    }

  vmsunlock(&SPIN_MAILBOX,savipl);
      
  if (func&IO$M_NOW)
    return exe$finishioc(SS$_NORMAL|(i->irp$l_bcnt<<16),i,p,u);
  else
    return SS$_NORMAL; // really exe$qioreturn
}

int mb$fdt_read (struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  if (c->ccb$l_sts&CCB$M_NOREADACC)
    return exe_std$abortio(i,p,u,SS$_ILLIOFUNC);

  if (i->irp$l_qio_p2>u->ucb$w_devbufsiz)
    return exe_std$abortio(i,p,u,SS$_MBTOOSML);

  if (i->irp$l_qio_p2)
    exe_std$readchk(i,p,u,i->irp$l_qio_p1,i->irp$l_qio_p2);

  int func=i->irp$l_func;
  struct _mb_ucb * mu=u;
  struct __srb * s = 0;
  int savipl;
  int skip=0;
  
  if (i->irp$l_qio_p2) {
    i->irp$l_bcnt=i->irp$l_qio_p2;
    i->irp$l_media=i->irp$l_qio_p1;
  } else {
    i->irp$l_bcnt=0;
    i->irp$l_media=&i->irp$l_media;
  }

  if ((i->irp$l_bcnt == 0) && (i->irp$l_func & IO$M_STREAM)) 
    return exe$finishioc(SS$_NORMAL,i,p,u);

  if ((func&IO$M_WRITERCHECK) && (u->ucb$w_msgcnt==0) && (mu->ucb$l_mb_writerrefc==0))
    return exe$finishioc(SS$_NOWRITER,i,p,u);

  i->irp$l_sts|=IRP$M_BUFIO; //? what
  i->irp$l_sts|=IRP$M_MBXIO;

  if (i->irp$l_qio_p2>u->ucb$w_bufquo || i->irp$l_qio_p2>u->ucb$w_iniquo)
    return exe_std$abortio(i,p,u,SS$_EXQUOTA);

  savipl=vmslock(&SPIN_MAILBOX,IPL$_MAILBOX);

  if ((func&IO$M_STREAM) && !aqempty(u->ucb$l_mb_msgqfl)) {
    struct __mmb * tmp = u->ucb$l_mb_msgqfl;
    if (tmp->mmb$w_datasize == i->irp$l_bcnt)
      skip = 1;
  }

  if ((func & IO$M_STREAM) && ! skip) {
    s=kmalloc(sizeof(struct __srb)+i->irp$l_qio_p2,GFP_KERNEL);
    if (s) {
      memset(s,0,sizeof(struct __srb)+i->irp$l_qio_p2);
      s->srb$w_size=sizeof(struct __srb)+i->irp$l_qio_p2;
      s->srb$b_type=DYN$C_BUFIO;
      s->srb$b_func=func;
      s->srb$w_datasize=0;
      s->srb$w_reqsiz=i->irp$l_qio_p2;
      s->srb$w_bufquochrg=1;
      s->srb$l_datastart=&s->srb$t_data;
      s->srb$l_virtaddr=i->irp$l_media;
      s->srb$l_addr=&s->srb$t_data;
      i->irp$l_svapte=s;
    } else {
      return exe_std$iorsnwait(i,p,u,c,SS$_INSFMEM, RSN$_NPDYNMEM);
    }

    u->ucb$w_bufquo-=i->irp$l_qio_p2;
  }

  if ((func & IO$M_STREAM) && ! skip) {
    if (u->ucb$w_bufquo<s->srb$w_reqsiz) {
      if (u->ucb$w_iniquo<s->srb$w_reqsiz && aqempty(u->ucb$l_mb_msgqfl))
	return exe_std$abortio(i,p,u,SS$_MBFULL);
    }
  }

  if ((func & IO$M_NOW) && (u->ucb$w_msgcnt==0)) {
    vmsunlock(&SPIN_MAILBOX,savipl);
    if (i->irp$l_svapte) {
      kfree(s);
      if (s->srb$w_bufquochrg)
	u->ucb$w_bufquo+=i->irp$l_qio_p2;
    }
    i->irp$l_svapte=0;
    return exe$finishioc(SS$_ENDOFFILE,i,p,u);
  }

  if ((func&IO$M_WRITERCHECK) && mu->ucb$l_mb_writerrefc && u->ucb$w_msgcnt==0)
    insque(&i->irp$l_nopartnerqfl,mu->ucb$l_mb_nowriterwaitqfl);
  else
    i->irp$l_nopartnerqfl=0; // quick and dirty?

  //  if (m->mmb$w_msgsize)
  //    memcpy(m->mmb$t_data,i->irp$l_qio_p2,m->mmb$w_size);
  // ?  u->ucb$w_msgcnt++;
  insque(i,mu->ucb$l_mb_readqbl);

  if (!aqempty(u->ucb$l_mb_msgqfl))
    mb$finishread(u);
  else
    com_std$delattnast(&u->ucb$l_mb_r_ast ,u);
  vmsunlock(&SPIN_MAILBOX,savipl);
  return SS$_NORMAL; // or qioreturn? (forklock held, etc
}

create_fork_thread() { 
  sch_std$ravail(RSN$_MAILBOX);
}

enum { read_equal, read_less, read_more };

void mb$finishread(struct _ucb * u) {
  struct _mb_ucb * mu = u;
  struct __srb * s = 0;
  int room_ast=0;
  int not_done;

  do {

    not_done=0;
    struct _irp * i = mu->ucb$l_mb_readqfl; //not? u->ucb$l_irp;?
    struct __mmb * msg = u->ucb$l_mb_msgqfl;
    long read_status=read_equal;
    short func = i->irp$l_func;
    int retstatus=SS$_NORMAL;

    if (func & IO$M_STREAM) {
      if (msg->mmb$w_datasize < i->irp$l_bcnt)
	read_status=read_more;
      if (msg->mmb$w_datasize > i->irp$l_bcnt)
	read_status=read_less;
    }
    if ((func&IO$M_FCODE)==IO$_WRITEOF)
      read_status=read_equal;

    switch (read_status) {
    case read_equal:
      remque(i,0);
      msg=remque(msg,0);
      if (i->irp$l_svapte==0)
	i->irp$l_svapte=msg;
      else {
	s = i->irp$l_svapte;
	if (s->srb$w_datasize==0) {
	  i->irp$l_svapte=msg;
	  if (s->srb$w_bufquochrg) {
	    room_ast=1;
	    u->ucb$w_bufquo+=s->srb$w_reqsiz;
	  }
	  com_std$drvdealmem(s);
	}
      }

      if (i->irp$l_nopartnerqfl) // quick and dirty?
	remque(i->irp$l_nopartnerqfl,0);
      if (!aqempty(msg->mmb$l_noreaderwaitqfl))
	remque(msg->mmb$l_noreaderwaitqfl,0);
#if 0
      if (msg->mmb$l_nowriterwaitqfl)
	remque(msg->mmb$l_nowriterwaitqfl,0);
#endif

      if (i->irp$l_svapte!=msg) {
	memcpy(s->srb$l_datastart,msg->mmb$l_datastart,msg->mmb$w_datasize);
	s->srb$w_datasize+=s->srb$w_datasize;
      } else {
	msg->mmb$l_msgqfl=msg->mmb$l_datastart;
	msg->mmb$l_msgqbl=i->irp$l_media;
      }

      if (msg->mmb$w_bufquochrg) {
	room_ast=1;
	if (msg->mmb$w_datasize==0)
          u->ucb$w_bufquo++;
        else
	  u->ucb$w_bufquo+=msg->mmb$w_datasize;
      }

      if (sch$gl_resmask & (1<<RSN$_MAILBOX))
	fork(create_fork_thread,0,0,u);

      if (i->irp$l_svapte!=msg) {
	if (s->srb$w_bufquochrg) {
	  room_ast=1;
	  u->ucb$w_bufquo+=s->srb$w_reqsiz;
	}
      }

      if (i->irp$l_bcnt<msg->mmb$w_datasize)
	retstatus=SS$_BUFFEROVF;

      if (i->irp$l_svapte!=msg)
	i->irp$l_bcnt=s->srb$w_datasize;

      if ((func&IO$M_FCODE)==IO$_WRITEOF)
	retstatus=SS$_ENDOFFILE;
    
      u->ucb$w_msgcnt--;
      u->ucb$l_devdepend=u->ucb$w_msgcnt;

      if (msg->mmb$l_irp) {
	struct _irp * wirp=msg->mmb$l_irp;
	wirp->irp$l_iost1= (wirp->irp$l_bcnt<<16) + SS$_NORMAL;
	wirp->irp$l_iost2 = exe$ipid_to_epid(msg->mmb$l_pid);
	com$post(wirp,u);
      }

      if (i->irp$l_svapte!=msg)
	com_std$drvdealmem(msg);

      i->irp$l_iost1= (i->irp$l_bcnt<<16) + retstatus;
      i->irp$l_iost2 = exe$ipid_to_epid(msg->mmb$l_pid);

      com$post(i,u);

      break;
    case read_less:
      remque(i,0);

      if (i->irp$l_nopartnerqfl) // quick and dirty
	remque(i->irp$l_nopartnerqfl,0);
#if 0
      if (msg->mmb$l_nowriterwaitqfl)
	remque(msg->mmb$l_nowriterwaitqfl,0);
#endif

      if (s->srb$w_bufquochrg) {
	room_ast=1;
	u->ucb$w_bufquo+=s->srb$w_reqsiz;
      }

      if (msg->mmb$w_bufquochrg) {
	room_ast=1;
	if (i->irp$l_bcnt==0)
	  u->ucb$w_bufquo++;
	else
	  u->ucb$w_bufquo+=i->irp$l_bcnt;
      }

      if (sch$gl_resmask & (1<<RSN$_MAILBOX))
	fork(create_fork_thread,0,0,u);

      s=i->irp$l_svapte;

      memcpy(s->srb$l_datastart,msg->mmb$l_datastart,i->irp$l_bcnt);
      s->srb$w_datasize+=i->irp$l_bcnt;
      msg->mmb$w_datasize-=i->irp$l_bcnt;
      msg->mmb$l_datastart+=i->irp$l_bcnt;
    
      i->irp$l_bcnt=s->srb$w_datasize;
      i->irp$l_iost1= (i->irp$l_bcnt<<16) + SS$_NORMAL;
      i->irp$l_iost2 = exe$ipid_to_epid(msg->mmb$l_pid);

      com$post(i,u);

      not_done=aqempty(mu->ucb$l_mb_readqfl)==0;

      break;
    case read_more:
      msg=remque(msg,0);
      if (!aqempty(msg->mmb$l_noreaderwaitqfl))
	remque(msg->mmb$l_noreaderwaitqfl,0);

      msg->mmb$l_msgqfl=msg->mmb$l_datastart;
      msg->mmb$l_msgqbl=i->irp$l_media;

      if(msg->mmb$w_bufquochrg) {
	room_ast=1;
	if (msg->mmb$w_datasize==0)
	  u->ucb$w_bufquo++;
	else
	  u->ucb$w_bufquo+=msg->mmb$w_datasize;
      }

      if (sch$gl_resmask & (1<<RSN$_MAILBOX))
	fork(create_fork_thread,0,0,u);

      s=i->irp$l_svapte;
      memcpy(s->srb$l_datastart,msg->mmb$l_datastart,msg->mmb$w_datasize);

      i->irp$l_bcnt-=msg->mmb$w_datasize;
      s->srb$w_datasize+=msg->mmb$w_datasize;
      s->srb$l_datastart+=msg->mmb$w_datasize;
    
      u->ucb$w_msgcnt--;
      u->ucb$l_devdepend=u->ucb$w_msgcnt;

      if (msg->mmb$l_irp) {
	struct _irp * wirp=msg->mmb$l_irp;
	wirp->irp$l_iost1= (wirp->irp$l_bcnt<<16) + SS$_NORMAL;
	wirp->irp$l_iost2 = exe$ipid_to_epid(msg->mmb$l_pid);
	com$post(wirp,u);
      } 
      com_std$drvdealmem(msg);

      struct __mmb * next = u->ucb$l_mb_msgqfl;

      if (!aqempty(&u->ucb$l_mb_msgqfl) && (next->mmb$b_func&IO$M_FCODE)!=IO$_WRITEOF) {
	  not_done=1;
      } else {

	  if (s->srb$w_bufquochrg) {
	    room_ast=1;
	    u->ucb$w_bufquo+=s->srb$w_reqsiz;
	  }

	  i->irp$l_bcnt=s->srb$w_datasize;
	  remque(i,0);
    
	  i->irp$l_iost1=  (s->srb$w_datasize<<16) + SS$_NORMAL;
	  i->irp$l_iost2 = exe$ipid_to_epid(msg->mmb$l_pid);

	  com$post(i,u);
      }
      break;
    }
  } while (not_done);

  if (room_ast)
    com_std$delattnast(&mu->ucb$l_mb_room_notify, u);

}

void mb_cancel (struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  int func=i->irp$l_func;

}

void mb_aux_routine (struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  int func=i->irp$l_func;

}

static struct _fdt mb$fdt = {
  fdt$q_valid:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_WRITECHECK|IO$_READPBLK|IO$_WRITELBLK|IO$_DSE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_READRCT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_SETPRFPATH|IO$_FORMAT,
  fdt$q_buffered:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_DSE|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_FORMAT
};

/* more yet undefined dummies */
static void  startio () { };
static void  unsolint (void) { };
static void  cancel (void) { };
static void  regdump (void) { };
static void  diagbuf (void) { };
static void  errorbuf (void) { };
static void  unitinit (void) { };
static void  altstart (void) { };
static void  mntver (void) { };
static void  cloneducb (void) { };
static void  mntv_sssc (void) { };
static void  mntv_for (void) { };
static void  mntv_sqd (void) { };
static void  aux_storage (void) { };
static void  aux_routine (void) { };

static struct _ddt mb$ddt = {
  ddt$l_start: startio,
  ddt$l_unsolint: unsolint,
  ddt$l_fdt: &mb$fdt,
  ddt$l_cancel: cancel,
  ddt$l_regdump: regdump,
  ddt$l_diagbuf: diagbuf,
  ddt$l_errorbuf: errorbuf,
  ddt$l_unitinit: unitinit,
  ddt$l_altstart: altstart,
  ddt$l_mntver: mntver,
  ddt$l_cloneducb: cloneducb,
  ddt$w_fdtsize: 0,
  ddt$ps_mntv_sssc: mntv_sssc,
  ddt$ps_mntv_for: mntv_for,
  ddt$ps_mntv_sqd: mntv_sqd,
  ddt$ps_aux_storage: aux_storage,
  ddt$ps_aux_routine: aux_routine
};

#undef ini_fdt_act
extern void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn, unsigned long type);

void mb$struc_init (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb) {
  ucb->ucb$b_flck=IPL$_IOLOCK8;
  ucb->ucb$b_dipl=IPL$_IOLOCK8;

  ucb->ucb$l_devchar = DEV$M_REC | DEV$M_AVL | DEV$M_CCL | DEV$M_FOD/*| DEV$M_OOV*/;

  ucb->ucb$l_devchar2 = DEV$M_NNM;
  ucb->ucb$b_devclass = DC$_MAILBOX;
#if 0
  ucb->ucb$b_devtype = DT$_TTYUNKN;
  ucb->ucb$w_devbufsiz = 132;
#endif

  ucb->ucb$l_devdepend = 99; // just something to fill

  // dropped the mutex stuff

  return;
}

void mb$struc_reinit (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb) {
  ddb->ddb$ps_ddt=&mb$ddt;
  //dpt_store_isr(crb,nl_isr);
  return;
}

int mb$unit_init (struct _idb * idb, struct _ucb * ucb) {
  ucb->ucb$v_online = 0;
  //ucb->ucb$l_lr_msg_tmo = 0 ; // or offline? // where did this go?

  // idb->idb$ps_owner=&(ucb->ucb$r_ucb); // this is mailbox?
  // no adp or cram stuff

  // or ints etc
  
  ucb->ucb$v_online = 1;

  return SS$_NORMAL;
}

struct _dpt mb$dpt;
struct _ddb mb$ddb;
struct _mb_ucb mb$ucb;
struct _crb mb$crb;

struct _ucb * mbucb0;

int mb$init_tables() {
  ini_dpt_name(&mb$dpt, "MBDRIVER");
  ini_dpt_adapt(&mb$dpt, 0);
  ini_dpt_defunits(&mb$dpt, 1);
  ini_dpt_ucbsize(&mb$dpt,sizeof(struct _ucb));
  ini_dpt_struc_init(&mb$dpt, mb$struc_init);
  ini_dpt_struc_reinit(&mb$dpt, mb$struc_reinit);
  ini_dpt_ucb_crams(&mb$dpt, 1/*NUMBER_CRAMS*/);
  ini_dpt_end(&mb$dpt);

  ini_ddt_unitinit(&mb$ddt, mb$unit_init);
  ini_ddt_start(&mb$ddt, startio);
  ini_ddt_cancel(&mb$ddt, ioc_std$cancelio);
  ini_ddt_end(&mb$ddt);

  /* for the fdt init part */
  /* a lot of these? */
  ini_fdt_act(&mb$fdt,IO$_READLBLK,mb$fdt_read,1);
  ini_fdt_act(&mb$fdt,IO$_READPBLK,mb$fdt_read,1);
  ini_fdt_act(&mb$fdt,IO$_READVBLK,mb$fdt_read,1);
  ini_fdt_act(&mb$fdt,IO$_WRITELBLK,mb$fdt_write,1);
  ini_fdt_act(&mb$fdt,IO$_WRITEPBLK,mb$fdt_write,1);
  ini_fdt_act(&mb$fdt,IO$_WRITEVBLK,mb$fdt_write,1);
  ini_fdt_act(&mb$fdt,IO$_WRITEOF, mb$fdt_write, 1);
  ini_fdt_act(&mb$fdt,IO$_SETMODE, mb$fdt_setmode, 1);
  ini_fdt_act(&mb$fdt,IO$_SENSEMODE, mb$fdt_sensemode, 1);
  ini_fdt_end(&mb$fdt);

  return SS$_NORMAL;
}

long mb_iodb_vmsinit(void) {
#if 0
  struct _ucb * ucb=&mb$ucb;
  struct _ddb * ddb=&mb$ddb;
  struct _crb * crb=&mb$crb;
#endif 
  struct _ucb * ucb=kmalloc(sizeof(struct _ucb),GFP_KERNEL);
  struct _ddb * ddb=kmalloc(sizeof(struct _ddb),GFP_KERNEL);
  struct _crb * crb=kmalloc(sizeof(struct _crb),GFP_KERNEL);
  unsigned long idb=0,orb=0;

  mbucb0=ucb;

  memset(ucb,0,sizeof(struct _ucb));
  memset(ddb,0,sizeof(struct _ddb));
  memset(crb,0,sizeof(struct _crb));

#if 0
  init_ddb(&mb$ddb,&mb$ddt,&mb$ucb,"mba");
  init_ucb(&mb$ucb, &mb$ddb, &mb$ddt, &mb$crb);
  init_crb(&mb$crb);
#endif

  ucb -> ucb$w_size = sizeof(struct _mb_ucb); // temp placed

  init_ddb(ddb,&mb$ddt,ucb,"mba");
  init_ucb(ucb, ddb, &mb$ddt, crb);
  init_crb(crb);

//  ioc_std$clone_ucb(&mb$ucb,&ucb);
  mb$init_tables();
  mb$struc_init (crb, ddb, idb, orb, ucb);
  mb$struc_reinit (crb, ddb, idb, orb, ucb);
  mb$unit_init (idb, ucb);

  insertdevlist(ddb);

  return ddb;

}

long mb_iodbunit_vmsinit(struct _ddb * ddb,int unitno,void * dsc) {
  unsigned short int chan;
  struct _ucb * newucb;
  ioc_std$clone_ucb(ddb->ddb$ps_ucb/*&mb$ucb*/,&newucb);
#if 0
  exe$assign(dsc,&chan,0,0,0);
  registerdevchan(MKDEV(MB0_MAJOR,unitno),chan);
#endif

  return newucb;
}

int mb_vmsinit(void) {
  //struct _ucb * u=makeucbetc(&ddb,&ddt,&dpt,&fdt,"hda","hddriver");

  unsigned short chan0, chan1, chan2;
  $DESCRIPTOR(u0,"mba0");
  unsigned long idb=0,orb=0;
  struct _ccb * ccb;
  struct _ucb * newucb0,*newucb1,*newucb2;
  struct _ddb * ddb;

  printk(KERN_INFO "dev here pre\n");

  ddb=mb_iodb_vmsinit();

  /* for the fdt init part */
  /* a lot of these? */

#if 0
  mb_iodbunit_vmsinit(ddb,0,&u0);
#endif

  printk(KERN_INFO "dev here\n");

  // return chan0;

}

int exe_std$wrtmailbox (struct _mb_ucb *mb_ucb, int msgsiz, void *msg,...) {
  struct __mmb * m;
  struct _ucb * u = mb_ucb;

  if (msgsiz>u->ucb$w_bufquo) {
    //    kfree(m);
    return SS$_MBFULL;
    //or return exe$abortio(SS$_MBFULL,i,p,u);
  }
  if (msgsiz>u->ucb$w_devbufsiz)
    return SS$_MBTOOSML;
  // orreturn exe$abortio(SS$_MBTOOSML,i,p,u);
  //  int savipl=vmslock(&SPIN_MAILBOX,IPL$_MAILBOX);
  m=kmalloc(sizeof(struct __mmb)+msgsiz,GFP_KERNEL);
  memset(m,0,sizeof(struct __mmb)+msgsiz);
  m->mmb$w_size=sizeof(struct __mmb)+msgsiz;
  m->mmb$b_type=DYN$C_BUFIO;
  m->mmb$b_func=0;
  m->mmb$w_datasize=msgsiz;
  qhead_init(&m->mmb$l_noreaderwaitqfl);
  m->mmb$l_irp=0;
  m->mmb$l_pid=ctl$gl_pcb->pcb$l_pid;
  m->mmb$l_datastart=&m->mmb$t_data;
  if (m->mmb$w_datasize)
    memcpy(m->mmb$t_data,msg,msgsiz);

  u->ucb$w_msgcnt++;
  u->ucb$l_devdepend=u->ucb$w_msgcnt;

  if (m->mmb$w_bufquochrg) {
    //    room_ast=1;
    if (m->mmb$w_datasize==0)
      u->ucb$w_bufquo++;
    else
      u->ucb$w_bufquo-=m->mmb$w_datasize;
  }

  insque(m,u->ucb$l_mb_msgqbl);

  if (!aqempty(mb_ucb->ucb$l_mb_readqfl))
    mb$finishread(u);
  else
    {
      com_std$delattnast(&u->ucb$l_mb_w_ast, u);
    }

  //  vmsunlock(&SPIN_MAILBOX,savipl);

  return SS$_NORMAL;
}

int exe_std$sndevmsg(struct _mb_ucb *mb_ucb, int msgtyp, struct _ucb *ucb) {
  struct _ucb * u=mb_ucb;
  long message[8];
  //spinlock
  if (u->ucb$b_devclass != DC$_MAILBOX)
    return SS$_DEVNOTMBX;
  message[0]=msgtyp + (ucb->ucb$w_unit << 16);
  message[1]=0;

  // too lazy to do ioc$cvt_devnam
  sprintf(&message[1],"%s%d",&ucb->ucb$l_ddb->ddb$t_name[1],ucb->ucb$w_unit);

  return exe_std$wrtmailbox(mb_ucb,8*sizeof(long),message);
}

int mb$chanunwait(struct _ucb * u, struct _ccb * c) {
  // make ccb copy on stack
  // spinlock etc
  struct _mb_ucb * mu = u;
  if ((c->ccb$l_sts&CCB$M_NOREADACC)==0)
    mu->ucb$l_mb_readerrefc++;
  if ((c->ccb$l_sts&CCB$M_NOWRITEACC)==0)
    mu->ucb$l_mb_writerrefc++;
  if ((c->ccb$l_sts&CCB$M_NOREADACC)==0) {
    struct _irp * i=mu->ucb$l_mb_readerwaitqfl;
    while (!aqempty(mu->ucb$l_mb_readerwaitqfl)) { 
      i->irp$l_iost1=SS$_NORMAL;
      i->irp$l_iost2=0;
      com$post(i,u);
      remque(i,0);
      i=mu->ucb$l_mb_readerwaitqfl;
    }
  }
  if ((c->ccb$l_sts&CCB$M_NOWRITEACC)==0) {
    struct _irp * i=mu->ucb$l_mb_writerwaitqfl;
    while (!aqempty(mu->ucb$l_mb_writerwaitqfl)) { 
      i->irp$l_iost1=SS$_NORMAL;
      i->irp$l_iost2=0;
      com$post(i,u);
      remque(i,0);
      i=mu->ucb$l_mb_writerwaitqfl;
    }
  }
  return SS$_NORMAL;
}
