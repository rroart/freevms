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
#include <com_routines.h>
#include <ioc_routines.h>

#include<linux/blkdev.h>

#include <exe_routines.h>
#include <misc_routines.h>

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

int tty$fdtset(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  // stripped down pn$fdtset
  short bufsiz;
  char * buf;
  int size;
  struct _acb ** acb_p = 0;
  int sts=SS$_NOSUCHDEV;	// Assume no TZ device
  struct _ltrm_ucb * lt = u;
  if (u==0)	goto SET_ABORT;		// No TZ UCB exit
#if 0
  switch (i->irp$l_qio_p4) { 	// Figure out what to set // check
  }
#endif
  int act=i->irp$l_func&IO$M_FMODIFIERS;
  switch (act) {
  case 0: goto SET_MODE;		// Page, type ....
  case IO$M_CTRLCAST: goto SET_CTRLC; 
  case IO$M_CTRLYAST: goto SET_CTRLY; 
  case IO$M_OUTBAND: goto SET_OUTBAND; 
  default:
    sts=SS$_BADPARAM;	// Assume we were passed something bad.
    return exe_std$abortio(i,p,u,sts);
  }		//
  //;;JC------------------- END Modified by J. Clement -------------------------

 BAD_SET:
  sts=SS$_BADPARAM;	// Assume we were passed something bad.
 SET_ABORT:
  return exe_std$abortio(i,p,u,sts);

  //
  //;;JC------------------- Modified by J. Clement -------------------------
  //	This will set page size, and dev characteristics.
  //
 SET_MODE:
  //
  //	Set the mode
  //
  buf	 = i->irp$l_qio_p1;		// ADDRESS USER BUFFER
  size=i->irp$l_qio_p2;		// GET SIZE ARGUMENT
  if (size>0 && size<8) goto	BAD_SET;// Too small ?
  bufsiz=((short *)buf)[1];		// Width
  if	(bufsiz==0 || bufsiz>=511)			// Test it
    goto	BAD_SET;		// Too small ? Too big ?
  if	(size>=12)			// DID HE ASK FOR 2ND ?
    { }		// Check content, maybe?

  if (size==0)
    size=12;
  if (size>20)
    size=20;
  memcpy(&i->irp$l_media, buf, size);

  // check whether something we set earlier is consistent
  if (lt->ucb$l_tl_phyucb) {
    struct _tty_ucb * tty = lt->ucb$l_tl_phyucb;
    if (u!=tty->ucb$l_tt_logucb)
      printk("<0>" "tty error in set\n");
  }
#if 0
  // not yet?
  sts = exe$altquepkt (i,p,u);
  return sts;
#endif
  return exe$qiodrvpkt (i,p,u); 

 SET_CTRLC:
  acb_p=&lt->ucb$l_tl_ctrlc;
  goto SET_CMN;
 SET_CTRLY:
  acb_p=&lt->ucb$l_tl_ctrly;
 SET_CMN:
  // check whether something we set earlier is consistent
  if (lt->ucb$l_tl_phyucb) {
    struct _tty_ucb * tty = lt->ucb$l_tl_phyucb;
    if (u!=tty->ucb$l_tt_logucb)
      printk("<0>" "tty error in set2\n");
  }
  sts = com_std$setattnast(i,p,u,c,acb_p);	// Insert into AST list
  return exe$finishioc(sts,i,p,u);		// Complete request

 SET_OUTBAND:
  acb_p=&lt->ucb$l_tl_bandque;
  // check whether something we set earlier is consistent
  if (lt->ucb$l_tl_phyucb) {
    struct _tty_ucb * tty = lt->ucb$l_tl_phyucb;
    if (u!=tty->ucb$l_tt_logucb)
      printk("<0>" "tty error in set3\n");
  }
  void * dummy;
  sts = com_std$setctrlast(i,p,u,c,acb_p,lt->ucb$l_tl_outband,&dummy);	// Insert into AST list
  return exe$finishioc(sts,i,p,u);		// Complete request
}

int tty$fdtsensem (struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  int devclass;
  int devdep;
  int size, *buf, spec_chr, devdep2, speed, parity;
  struct _ucb * ucb;
  VERIFY_SENSE(&size,&buf,i,p,u,c);		// VERIFY USER STORAGE
  if (u==0) {		// Have UCB
    //	If no TZ device abort with SS$_NOSUCHDEV
    // Save error reason
    return exe_std$abortio(i,p,u,SS$_NOSUCHDEV);		// Abort request
  }
  ucb = u;		// Switch to logical device if one exists
  spec_chr=GET_DCL(u);			// BUILD SPECIAL CHARACTERISTICS

  devclass = *(int*)(&ucb->ucb$b_devclass);		// BUILD TYPE, AND BUFFER SIZE
  devdep = ucb->ucb$l_devdepend;		//RETURN 1ST CHARACTERISTICS LONGWORD
  devdep2=	ucb->ucb$l_devdepnd2&~spec_chr; // check //AND 2ND LONGWORD (IF REQUESTED)
  speed= ((struct _tty_ucb *)ucb)->ucb$w_tt_speed; // check // RETURN SPEED
  parity= ((struct _tty_ucb *)ucb)->ucb$b_tt_parity; // check // RETURN PARITY INFO
  parity=(long)parity&~0xFF000000;		// ZERO HIGH BYTE
  parity = ((struct _tty_ucb*)ucb)->ucb$b_tt_crfill;		// AND CR/LF FILL

  *buf=devclass;			// RETURN USER DATA
  buf[1]=devdep;		//
  if	(size>=12)			// DID HE ASK FOR 2ND ?
    buf[2]=devdep2;		//
  // RETURN IOSB DATA
  //expanded	goto	CMN_EXIT;		// EXIT RETURNING R0,R1	
  return exe$finishio(speed|SS$_NORMAL,parity,i,p,u);	// check	// COMPLETE REQUEST IOSB WORD 0,1
}

int tty$fdtsensec (struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  long devclass;
  long devdep;
  int size, *buf, spec_chr, devdep2, speed, parity;
  struct _ucb * ucb;
  VERIFY_SENSE(&size,&buf,i,p,u,c);		// VERIFY USER STORAGE
  if (u==0)	{		// Have UCB
    //	If no TZ device abort with SS$_NOSUCHDEV
    // Save error reason
    return exe_std$abortio(i,p,u,SS$_NOSUCHDEV);		// Abort request
  }
  ucb = u;
  if (((struct _ltrm_ucb *)u)->ucb$l_tl_phyucb)
    ucb = ((struct _ltrm_ucb *)u)->ucb$l_tl_phyucb;
  spec_chr=GET_DCL(ucb);			// BUILD SPECIAL CHARACTERISTICS

  devclass=*(int*)(&((struct _tty_ucb *)ucb)->ucb$w_tt_desize) & 0xffffff00; //BUILD TYPE, AND BUFFER SIZE
  devclass|=	DC$_TERM;		// BUILD DEVICE CLASS
  devdep = ((struct _tty_ucb *)ucb)->ucb$l_tt_dechar;		//RETURN 1ST CHARACTERISTICS LONGWORD
  devdep2=	((struct _tty_ucb *)ucb)->ucb$l_tt_decha1&~spec_chr; // check // AND 2ND LONGWORD (IF REQUESTED)
  speed=((struct _tty_ucb *)ucb)->	ucb$w_tt_despee; // check // RETURN SPEED
  parity=((struct _tty_ucb *)ucb)->	ucb$b_tt_depari; // RETURN PARITY INFO
  parity	&=~0xFF000000;		// ZERO HIGH BYTE
  parity = ((struct _tty_ucb *)ucb)->ucb$b_tt_decrf;		// AND CR/LF FILL

  *buf=devclass;			// RETURN USER DATA
  buf[1]=devdep;		//
  if 	(size>=12)			// DID HE ASK FOR 2ND ?
    buf[2]=devdep2;		;
  // RETURN IOSB DATA
  //expanded	goto	CMN_EXIT;		// EXIT RETURNING R0,R1	
  return exe$finishio(speed|SS$_NORMAL,parity,i,p,u);	// check 	// COMPLETE REQUEST IOSB WORD 0,1
}

int tty$cancelio(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {					// Cancel PZ usage
  struct _ccb * R6=c;
  struct _acb ** acb_p;
  int R0,R1;
  struct _ucb * tz = u;
  struct _tz_ucb * real_tz;
  struct _tty_ucb * tty;
  signed int index=ctl$gl_ccbbase-c;
  int sts;
  ioc_std$cancelio(index,i,p,u);			// Call the cancel routine
  if	(UCB$M_CANCEL&tz->ucb$l_sts) {	// Branch if not for this guy
    R1=0;
    sts=	SS$_ABORT;			// Status is request canceled
    tz->ucb$l_sts&=~(UCB$M_BSY|UCB$M_CANCEL);			// Clear unit status flags
    ioc$reqcom(sts,R1,tz);			// Complete request
  }
  if	(tz->ucb$l_refc)			// Last Deassign
    return;				// No, just exit
  // wait with the rest
#if 0
  //
  // Last DEASSIGN we need to get rid of AST's 
  //
  R6			 = index;		// Save the Channel number
  real_tz=tz;
  struct _ltrm_ucb * lt = u;
  if (lt->ucb$l_tl_phyucb)
  tz=	real_tz->ucb$l_pz_xucb;		// Switch to TZ UCB
  if (tz==0) goto	l20;				// if not there, skip
  real_tz=tz;
  acb_p=&	real_tz->ucb$l_tz_xon_ast;		// Get XON list head address
  if	(*acb_p)			// Any ast to deliver
    // EQL 0 do not flush it
    com_std$flushattns(p,tz,index,acb_p);		// Flush it
  real_tz= tz; acb_p=&real_tz->ucb$l_tz_xoff_ast;	// Get XOFF list head address
  if	(*acb_p)			// Any ast to deliver
    // EQL 0 do not flush it
    com_std$flushattns(p,tz,index,acb_p);		// Flush it
  real_tz=tz; acb_p=&real_tz->ucb$l_tz_set_ast;		// Get SET_LINE list head address
  if	(*acb_p)			// Any ast to deliver
    // EQL 0 do not flush it
    com_std$flushattns(p,tz,index,acb_p);		// Flush it
  //
  // Do a DISCONNECT on the TZ device.
  //

  real_tz=tz; real_tz->ucb$l_tz_xucb=0;		// Clear backlink to PZ device
  tz->ucb$l_sts|=	UCB$M_DELETEUCB;  // Set it to go bye-bye
  tz->ucb$l_sts&=~UCB$M_ONLINE;	// Mark offline
  tz->ucb$l_sts&=~UCB$M_INT; 	// Don't expect interrupt
  tty=tz;
  R1=	tty->ucb$l_tt_logucb;		// Look at logical term UCB
  if	(((struct _ucb *)R1)->ucb$l_refc==0) {			// See if TZ has any references
    // If so, go and do disconnect
    ioc$delete_ucb();		// if not, delete the UCB
    goto	l20;
  }	
  R0=0;				// indicate that we must hangup
  tty=tz;
  R1=	tty->ucb$l_tt_class;
  ((struct _tt_class *)R1)->class_disconnect();		// Force disconnect
 l20:
  tz=u;
  real_tz=tz;
  real_tz->ucb$l_pz_xucb=0;		// Clear link to deleted TZ
  u->ucb$l_sts|=UCB$M_DELETEUCB;	// Set our own delete bit
  return;
#endif
}
