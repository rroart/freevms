// $Id$
// $Locker$

// Author. Paul Nankervis.
// Author. Roar Thronæs.

/* Access.c v1.3 */

/*
        This is part of ODS2 written by Paul Nankervis,
        email address:  Paulnank@au1.ibm.com

        ODS2 is distributed freely for all members of the
        VMS community to use. However all derived works
        must maintain comments in their source to acknowledge
        the contibution of the original author.
*/

/*
        This module implements 'accessing' files on an ODS2
        disk volume. It uses its own low level interface to support
        'higher level' APIs. For example it is called by the
        'RMS' routines.
*/

#include<linux/vmalloc.h>
#include<linux/linkage.h>
#include<linux/mm.h>

#include <stdio.h>
#include <linux/string.h>
//#include <memory.h>
//#include "ssdef.h"
//#include "phyio.h"

#include <mytypes.h>
#include <aqbdef.h>
#include <atrdef.h>
#include <ccbdef.h>
#include <fatdef.h>
#include <vcbdef.h>
#include <descrip.h>
#include <dyndef.h>
#include <ssdef.h>
#include <uicdef.h>
#include <namdef.h>
#include <fabdef.h>
#include <rabdef.h>
#include <fi2def.h>
#include <fi5def.h>
#include <fibdef.h>
#include <fiddef.h>
#include <iodef.h>
#include <iosbdef.h>
#include <irpdef.h>
#include <rmsdef.h>
#include <sbkdef.h>
#include <xabdef.h>
#include <xabdatdef.h>
#include <xabfhcdef.h>
#include <xabprodef.h>
#include <ucbdef.h>
#include <ddbdef.h>
#include <fh2def.h>
#include <fm2def.h>
#include <hm2def.h>
#include <fcbdef.h>
#include <scbdef.h>
#include <wcbdef.h>
#include <vmstime.h>
#include "../../f11x/src/xqp.h"
#include "cache.h"
#include "access.h"

#define DEBUGx

int f11b_read_writevb(struct _irp * i) {
  int lbn;
  char * buffer;
  struct _iosb iosb;
  struct _vcb * vcb = i->irp$l_ucb->ucb$l_vcb;
  struct _fcb * fcb = xqp->primary_fcb; // ???? is this right
  struct _wcb * wcb = &fcb->fcb$l_wlfl;
  int blocks=(i->irp$l_qio_p2+511)/512;
  lbn=f11b_map_vbn(i->irp$l_qio_p3,wcb);
  if (i->irp$v_fcode==IO$_WRITEVBLK) {
    f11b_write_block(vcb,i->irp$l_qio_p1,lbn,blocks,&iosb);
  } else {
    buffer=f11b_read_block(vcb,lbn,blocks,&iosb);
    memcpy(i->irp$l_qio_p1,buffer,512);
    kfree(buffer);
  }
  //f11b_io_done(i);
}

signed int f11b_map_vbn(unsigned int vbn,struct _wcb *wcb) {
  // thing there should be more here?
  signed int lbn=-1;
  ioc_std$mapvblk(vbn,0,wcb,0,0,&lbn,0,0);
  return lbn;
}

signed int f11b_map_idxvbn(struct _vcb * vcb, unsigned int vbn) {
  // thing there should be more here?
  signed int lbn=-1;
  struct _wcb * wcb=&((struct _fcb *) getidxfcb(vcb))->fcb$l_wlfl;
  ioc_std$mapvblk(vbn,0,wcb,0,0,&lbn,0,0);
  return lbn;
}

void * f11b_read_block(struct _vcb * vcb, unsigned long lbn, unsigned long count, struct _iosb * iosb) {
  struct _iosb myiosb;
  unsigned char * buf = kmalloc(512*count,GFP_KERNEL);
  unsigned long phyblk=lbn; // one to one
  unsigned long sts=sys$qiow(0,xqp->io_channel,IO$_READLBLK,&myiosb,0,0,buf,512*count,phyblk,0,0,0);
  if (iosb) iosb->iosb$w_status=myiosb.iosb$w_status;
  return buf;
}

void * f11b_write_block(struct _vcb * vcb, unsigned char * buf, unsigned long lbn, unsigned long count, struct _iosb * iosb) {
  struct _iosb myiosb;
  unsigned long phyblk=lbn; // one to one
  unsigned long sts=sys$qiow(0,xqp->io_channel,IO$_WRITELBLK,&myiosb,0,0,buf,512*count,phyblk,0,0,0);
  if (iosb) iosb->iosb$w_status=myiosb.iosb$w_status;
  return buf;
}

void * f11b_search_fcb(struct _vcb * vcb,struct _fiddef * fid)
{
    struct _fcb * head = &vcb->vcb$l_fcbfl;
    struct _fcb * tmp = head->fcb$l_fcbfl;
    while(tmp!=head) {
      if ((tmp->fcb$b_fid_nmx==fid->fid$b_nmx) && (tmp->fcb$w_fid_num==fid->fid$w_num)) return tmp;
      tmp=tmp->fcb$l_fcbfl;
    }
    return 0;
}

void iosbret(struct _irp * i,int sts) {
  if (i && i->irp$l_iosb) {
    struct _iosb * iosb=i->irp$l_iosb;
    iosb->iosb$w_status=(unsigned short) sts;
    //    exe$setef(i->irp$b_efn);
  }
}

void f11b_read_attrib(struct _fcb * fcb,struct _atrdef * atrp) {
  struct _iosb iosb;
  int sts;
  struct _fh2 * head;
  unsigned long fi;
  head = f11b_read_header (xqp->current_vcb, 0, fcb, &iosb);  
  sts=iosb.iosb$w_status;
  fi=(unsigned short *)head+head->fh2$b_idoffset;

  while(atrp->atr$w_type!=0) {
    switch (atrp->atr$w_type) {
    case ATR$C_RECATTR:
      {
	memcpy(atrp->atr$l_addr,&head->fh2$w_recattr,atrp->atr$w_size);
      }
      break;
    case ATR$C_STATBLK:
      {
	struct _sbkdef * s=atrp->atr$l_addr;
	s->sbk$l_fcb=fcb;
      }
      break;
    case ATR$C_HEADER:
      {
	struct _fh2 * head;
	head = f11b_read_header (xqp->current_vcb, 0, fcb, &iosb);  
	sts=iosb.iosb$w_status;
	memcpy(atrp->atr$l_addr,head,atrp->atr$w_size);
	kfree(head); // wow. freeing something
      }
      break;
    case ATR$C_CREDATE:
      {
	if (head->fh2$w_struclev==0x501)
	  memcpy(atrp->atr$l_addr,&((struct _fi5 *)fi)->fi5$q_credate,atrp->atr$w_size);
	else
	  memcpy(atrp->atr$l_addr,&((struct _fi2 *)fi)->fi2$q_credate,atrp->atr$w_size);
      }  
	break;
    case ATR$C_REVDATE:
      {
	if (head->fh2$w_struclev==0x501)
	  memcpy(atrp->atr$l_addr,&((struct _fi5 *)fi)->fi5$q_revdate,atrp->atr$w_size);
	else
	  memcpy(atrp->atr$l_addr,&((struct _fi2 *)fi)->fi2$q_revdate,atrp->atr$w_size);
      }  
	break;
    case ATR$C_EXPDATE:
      {
	if (head->fh2$w_struclev==0x501)
	  memcpy(atrp->atr$l_addr,&((struct _fi5 *)fi)->fi5$q_expdate,atrp->atr$w_size);
	else
	  memcpy(atrp->atr$l_addr,&((struct _fi2 *)fi)->fi2$q_expdate,atrp->atr$w_size);
      }  
	break;
    case ATR$C_BAKDATE:
      {
	if (head->fh2$w_struclev==0x501)
	  memcpy(atrp->atr$l_addr,&((struct _fi5 *)fi)->fi5$q_bakdate,atrp->atr$w_size);
	else
	  memcpy(atrp->atr$l_addr,&((struct _fi2 *)fi)->fi2$q_bakdate,atrp->atr$w_size);
      }  
	break;
	
    case ATR$C_UIC:
	memcpy(atrp->atr$l_addr,&head->fh2$l_fileowner,atrp->atr$w_size);
	break;

    case ATR$C_FPRO:
	memcpy(atrp->atr$l_addr,&head->fh2$w_fileprot,atrp->atr$w_size);
	break;

    default:
      printk("atr %x not supported\n",atrp->atr$w_type);
      break;
    }
    atrp++;
  }
  kfree(head);
}

void f11b_write_attrib(struct _fcb * fcb,struct _atrdef * atrp) {
  struct _iosb iosb;
  int sts;
  struct _atrdef atr[2];
  struct _fatdef recattr;
  struct _fh2 * head;
  unsigned long fi;
  head = f11b_read_header (xqp->current_vcb, 0, fcb, &iosb);  
  sts=iosb.iosb$w_status;
  fi=(unsigned short *)head+head->fh2$b_idoffset;

  while(atrp->atr$w_type!=0) {
    switch (atrp->atr$w_type) {
    case ATR$C_RECATTR:
      {
	memcpy(&head->fh2$w_recattr,atrp->atr$l_addr,atrp->atr$w_size);
      }
      break;
    case ATR$C_STATBLK:
      {
	printk("statblk attribute is read-only\n");
      }
      break;
    case ATR$C_HEADER:
      {
	printk("header attribute is read-only\n");
      }
      break;
    case ATR$C_CREDATE:
      {
	if (head->fh2$w_struclev==0x501)
	  memcpy(&((struct _fi5 *)fi)->fi5$q_credate,atrp->atr$l_addr,atrp->atr$w_size);
	else
	  memcpy(&((struct _fi2 *)fi)->fi2$q_credate,atrp->atr$l_addr,atrp->atr$w_size);
      }  
	break;
    case ATR$C_REVDATE:
      {
	if (head->fh2$w_struclev==0x501)
	  memcpy(&((struct _fi5 *)fi)->fi5$q_revdate,atrp->atr$l_addr,atrp->atr$w_size);
	else
	  memcpy(&((struct _fi2 *)fi)->fi2$q_revdate,atrp->atr$l_addr,atrp->atr$w_size);
      }  
	break;
    case ATR$C_EXPDATE:
      {
	if (head->fh2$w_struclev==0x501)
	  memcpy(&((struct _fi5 *)fi)->fi5$q_expdate,atrp->atr$l_addr,atrp->atr$w_size);
	else
	  memcpy(&((struct _fi2 *)fi)->fi2$q_expdate,atrp->atr$l_addr,atrp->atr$w_size);
      }  
	break;
    case ATR$C_BAKDATE:
      {
	if (head->fh2$w_struclev==0x501)
	  memcpy(&((struct _fi5 *)fi)->fi5$q_bakdate,atrp->atr$l_addr,atrp->atr$w_size);
	else
	  memcpy(&((struct _fi2 *)fi)->fi2$q_bakdate,atrp->atr$l_addr,atrp->atr$w_size);
      }  
	break;
	
    case ATR$C_UIC:
	memcpy(&head->fh2$l_fileowner,atrp->atr$l_addr,atrp->atr$w_size);
	break;

    case ATR$C_FPRO:
	memcpy(&head->fh2$w_fileprot,atrp->atr$l_addr,atrp->atr$w_size);
	break;

    default:
      printk("atr %x not supported\n",atrp->atr$w_type);
      break;
    }
    atrp++;
  }
  {
    unsigned short check = checksum((vmsword *) head);
    head->fh2$w_checksum = VMSWORD(check);
  }
  writehead(fcb,head);
  kfree(head);
}

void * getvcb(void) {
  return xqp->current_vcb;
}

int getchan(struct _vcb * v) {
  return ((struct _ucb *)v->vcb$l_rvt)->ucb$ps_adp;
}

extern struct _ucb * myfilelist[50];
extern char * myfilelists[50];
extern int myfilelistptr;

// really really bad

struct _ucb * finducb(struct _fcb * fcb) {
  int i;
  struct _ucb *dev;
  struct _fcb * head, * tmp;
  for (i=0;i<myfilelistptr;i++) {
    dev=myfilelist[i];
    head=&dev->ucb$l_vcb->vcb$l_fcbfl;
    tmp=head->fcb$l_fcbfl;
    while (tmp!=head) {
      if (tmp==fcb) return dev;
      tmp=tmp->fcb$l_fcbfl;
    }
  }
  return 0;
}

struct _fcb * getidxfcb(struct _vcb * vcb) {
  struct _fcb * head = &vcb->vcb$l_fcbfl;
  struct _fcb * tmp = head->fcb$l_fcbfl;
  while (tmp!=head) {
    if (tmp->fcb$w_fid[0]==1 && tmp->fcb$w_fid[0]==1) return tmp;
    tmp=tmp->fcb$l_fcbfl;
  }
  return 0;
}

struct _fcb * getmapfcb(struct _vcb * vcb) {
  struct _fcb * head = &vcb->vcb$l_fcbfl;
  struct _fcb * tmp = head->fcb$l_fcbfl;
  while (tmp!=head) {
    if (tmp->fcb$w_fid[0]==2 && tmp->fcb$w_fid[0]==2) return tmp;
    tmp=tmp->fcb$l_fcbfl;
  }
  return 0;
}

/* checksum() to produce header checksum values... */

unsigned short checksum(vmsword *block)
{
  int count = 255;
  unsigned result = 0;
  unsigned short *ptr = block;
  do {
    unsigned data = *ptr++;
    result += VMSWORD(data);
  } while (--count > 0);
  return result;
}


/* rvn_to_dev() find device from relative volume number */

// half broken. is this switch_volume?

struct _vcb *rvn_to_dev(struct _vcb *vcb,unsigned rvn)
{
  return vcb;
}

/* fid_copy() copy fid from file header with default rvn! */

void fid_copy(struct _fiddef *dst,struct _fiddef *src,unsigned rvn)
{
  dst->fid$w_num = VMSWORD(src->fid$w_num);
  dst->fid$w_seq = VMSWORD(src->fid$w_seq);
  if (src->fid$b_rvn == 0) {
    dst->fid$b_rvn = rvn;
  } else {
    dst->fid$b_rvn = src->fid$b_rvn;
  }
  dst->fid$b_nmx = src->fid$b_nmx;
}

/* deaccesshead() release header from INDEXF... */

unsigned deaccesshead(struct _fh2 *head,unsigned idxblk)
{
  if (head && idxblk) {
    unsigned short check = checksum((vmsword *) head);
    head->fh2$w_checksum = VMSWORD(check);
  }
  return deaccesschunk(idxblk,1,1);
}

unsigned writechunk(struct _fcb * fcb,unsigned long vblock, char * buff)
{
  struct _iosb iosb;
  struct _vcb * vcb = xqp->current_vcb;
  struct _ucb * ucb = vcb->vcb$l_rvt; //was:  struct _ucb * ucb=finducb(fcb);
  int pbn;
  int sts=ioc_std$mapvblk(vblock,0,&fcb->fcb$l_wlfl,0,0,&pbn,0,0);
  sts=sys$qiow(0,xqp->io_channel,IO$_WRITELBLK,&iosb,0,0,buff,512,pbn,0,0,0);
  return iosb.iosb$w_status;
}

static unsigned gethead(struct _fcb * fcb,struct _fh2 **headbuff)
{
  struct _iosb iosb;
  struct _vcb * vcb = xqp->current_vcb;
  struct _ucb * ucb = vcb->vcb$l_rvt; //was:  struct _ucb * ucb=finducb(fcb);
  int vbn;
  int sts;
  struct _fiddef fid;
  fid.fid$w_num=fcb->fcb$w_fid[0];
  fid.fid$w_seq=fcb->fcb$w_fid[1];
  fid.fid$w_rvn=0;
  *headbuff=f11b_read_header(ucb->ucb$l_vcb,&fid,fcb,&iosb);
  return iosb.iosb$w_status;
}

unsigned writehead(struct _fcb * fcb,struct _fh2 *headbuff)
{
  struct _vcb * vcb = xqp->current_vcb;
  struct _ucb * ucb = vcb->vcb$l_rvt; //was:  struct _ucb * ucb=finducb(fcb);
  struct _fiddef * fid = &headbuff->fh2$w_fid.fid$w_num;
  unsigned short check = checksum((vmsword *) headbuff);
  int vbn=fid->fid$w_num + (fid->fid$b_nmx << 16) - 1 +
    VMSWORD(ucb->ucb$l_vcb->vcb$l_ibmapvbn) + VMSWORD(ucb->ucb$l_vcb->vcb$l_ibmapsize);;
  //if (headbuff->fh2$w_checksum == VMSWORD(check)) return 1;
  headbuff->fh2$w_checksum = VMSWORD(check);
  return writechunk(getidxfcb(ucb->ucb$l_vcb),vbn, headbuff);

}

void * f11b_read_header(struct _vcb *vcb,struct _fiddef *fid,struct _fcb * fcb,
                    unsigned long * retsts)
{
  struct _iosb iosb;
  unsigned sts;
  char * headbuff;
  struct _vcb *vcbdev;
  unsigned idxvblk;
  signed long idxlblk;
  struct _fh2 idxfh;
  struct _fiddef * locfid;
  vcbdev = rvn_to_dev(vcb,0);
  if (vcbdev == NULL) {   
    if (retsts) *retsts = SS$_DEVNOTMOUNT; 
    return 0; 
  }
  //if (wrtflg && ((vcb->vcb$b_status & VCB$M_WRITE_IF) == 0)) return SS$_WRITLCK;
  if (fcb) {
    idxlblk = fcb->fcb$l_hdlbn;
    locfid=&fcb->fcb$w_fid_num;
  } else {
    idxvblk = fid->fid$w_num + (fid->fid$b_nmx << 16) - 1 +
      VMSWORD(vcbdev->vcb$l_ibmapvbn) + VMSWORD(vcbdev->vcb$l_ibmapsize);
    idxlblk = f11b_map_idxvbn(vcb,idxvblk);
    locfid=fid;
  }
  xqp->header_lbn=idxlblk;
#if 0
  if (vcbdev->idxfcb->head != NULL) 
    if (idxvblk >= VMSSWAP(vcbdev->idxfcb->head->fh2$w_recattr.fat$l_efblk)) 
      sys$qiow(0,irp->irp$w_chan,IO$_READLBLK,&iosb,0,0,(char *)&idxfh,sizeof(struct _fh2),vcb->vcb$l_ibmaplbn,0,0,0);
#endif
#if 0
  not yet
    if (idxvblk >= VMSSWAP(idxfh.fh2$w_recattr.fat$l_efblk)) {
      {
	printk("Not in index file\n");
	return SS$_NOSUCHFILE;
      }
    }
#endif
  //  sts = accesschunk(getidxfcb(vcb),idxvblk,(char **) headbuff,NULL, 0,0);

  headbuff = f11b_read_block(vcb,idxlblk,1,&iosb);
  sts=iosb.iosb$w_status;

  if (sts & 1) {
    struct _fh2 *head = headbuff; 
    if (VMSWORD(head->fh2$w_fid.fid$w_num) != locfid->fid$w_num ||
	head->fh2$w_fid.fid$b_nmx != locfid->fid$b_nmx ||
	VMSWORD(head->fh2$w_fid.fid$w_seq) != locfid->fid$w_seq ||
	(head->fh2$w_fid.fid$b_rvn != locfid->fid$b_rvn &&
	 head->fh2$w_fid.fid$b_rvn != 0)) {
      /* lib$signal(SS$_NOSUCHFILE); */
      sts = SS$_NOSUCHFILE;
    } else {
      if (head->fh2$b_idoffset < 38 ||
	  head->fh2$b_idoffset > head->fh2$b_mpoffset ||
	  head->fh2$b_mpoffset > head->fh2$b_acoffset ||
	  head->fh2$b_acoffset > head->fh2$b_rsoffset ||
	  head->fh2$b_map_inuse > head->fh2$b_acoffset - head->fh2$b_mpoffset ||
	  checksum((vmsword *) head) != VMSWORD(head->fh2$w_checksum)) {
	sts = SS$_DATACHECK;
      } else {
	//if (VMSWORD(head->fh2$w_seg_num) != seg_num) sts = SS$_FILESEQCHK;
      }
    }
    if ((sts & 1) == 0) deaccesschunk(0,0,0);
  }
  if (retsts) *retsts=sts;
  return headbuff;
}





struct WCBKEY_NOT {
  unsigned vbn;
  struct _fcb *fcb;
  struct _wcb *prevwcb;
};                              /* WCBKEY passes info to compare/create routines... */

/* premap_indexf() called to physically read the header for indexf.sys
   so that indexf.sys can be mapped and read into virtual cache.. */

struct _fh2 *premap_indexf(struct _fcb *fcb,struct _ucb *ucb,unsigned *retsts)
{
  struct _iosb iosb;
  struct _fh2 *head;
  struct _vcb *vcbdev = rvn_to_dev(ucb->ucb$l_vcb,fcb->fcb$b_fid_rvn);
  if (vcbdev == NULL) {
    *retsts = SS$_DEVNOTMOUNT;
    return NULL;
  }
  head = (struct _fh2 *) kmalloc(sizeof(struct _fh2),GFP_KERNEL);
  //  *(unsigned long *)head=0; not need when no vmalloc
  if (head == NULL) {
    *retsts = SS$_INSFMEM;
  } else {
    int sts;
#if 0
    struct _hm2 home;
    sts = sys$qiow(0,irp->irp$w_chan,IO$_READLBLK,&iosb,0,0,(char *) &home,sizeof(struct _hm2),vcbdev->vcb$l_homelbn,0,0,0);
#endif
    *retsts = sys$qiow(0,xqp->io_channel,IO$_READLBLK,&iosb,0,0, (char *) head,sizeof(struct _fh2),VMSLONG(vcbdev->vcb$l_ibmaplbn) + VMSWORD(vcbdev->vcb$l_ibmapsize),0,0,0);
    *retsts = iosb.iosb$w_status;
    if (!(*retsts & 1)) {
      kfree(head);
      head = NULL;
    } else {
      if (VMSWORD(head->fh2$w_fid.fid$w_num) != 1 ||
	  head->fh2$w_fid.fid$b_nmx != 0 ||
	  VMSWORD(head->fh2$w_fid.fid$w_seq) != 1 ||
	  VMSWORD(head->fh2$w_checksum) != checksum((unsigned short *) head)) {
	*retsts = SS$_DATACHECK;
	kfree(head);
	head = NULL;
      }
    }
  }
  return head;
}

int get_fm2_val(unsigned short ** mpp, unsigned long * phyblk, unsigned long *phylen) {
  unsigned short *mp=*mpp;
  if (phyblk==0 || phylen==0) return SS$_BADPARAM;
	switch (VMSWORD(*mp) >> 14) {
	case FM2$C_PLACEMENT:
	  *phylen = 0;
	  (*mpp)++;
	  break;
	case FM2$C_FORMAT1:
	  *phylen = (VMSWORD(*mp) & 0377) + 1;
	  *phyblk = ((VMSWORD(*mp) & 037400) << 8) | VMSWORD(mp[1]);
	  (*mpp) += 2;
	  break;
	case FM2$C_FORMAT2:
	  *phylen = (VMSWORD(*mp) & 037777) + 1;
	  *phyblk = (VMSWORD(mp[2]) << 16) | VMSWORD(mp[1]);
	  (*mpp) += 3;
	  break;
	case FM2$C_FORMAT3:
	  *phylen = ((VMSWORD(*mp) & 037777) << 16) + VMSWORD(mp[1]) + 1;
	  *phyblk = (VMSWORD(mp[3]) << 16) | VMSWORD(mp[2]);
	  (*mpp) += 4;
	default:
	  return SS$_FORMAT;
	}
	return SS$_NORMAL;
}

int add_wcb(struct _fcb * fcb, unsigned short * map)
{
  struct _iosb iosb;
  unsigned int retsts;
  unsigned curvbn=1;
  unsigned extents = 0;
  struct _fh2 *head = 0;

  struct _vcb * vcb = xqp->current_vcb;
  struct _ucb * ucb = vcb->vcb$l_rvt; //was:  struct _ucb * ucb = finducb(fcb); // bad bad
  unsigned short *mp;
  unsigned phyblk, phylen;
  struct _wcb *wcb;

  mp = map;
  get_fm2_val(&mp,&phyblk,&phylen);
  if (phylen!=0) {

    wcb = (struct _wcb *) kmalloc(sizeof(struct _wcb), GFP_KERNEL);
    bzero(wcb,sizeof(struct _wcb));
    if (wcb == NULL) {
      retsts = SS$_INSFMEM;
      return retsts;
    }

    wcb->wcb$b_type=DYN$C_WCB;
    wcb->wcb$l_orgucb=ucb;
    insque(wcb,&fcb->fcb$l_wlfl);
    wcb->wcb$l_fcb=fcb;
    
    if (fcb->fcb$l_efblk) curvbn=fcb->fcb$l_efblk+1;

    wcb->wcb$l_stvbn=curvbn;
    wcb->wcb$l_p1_count=phylen;
    wcb->wcb$l_p1_lbn=phyblk;

    curvbn += phylen;

  }
  retsts = SS$_NORMAL;
  return retsts;
}

/* wcb_create() creates a window control block by reading appropriate
   file headers... */

int wcb_create_all(struct _fcb * fcb, struct _fh2 * fh2)
{
  struct _iosb iosb;
  unsigned int retsts;
  unsigned curvbn=1;
  unsigned extents = 0;
  struct _fh2 *head = 0;

  struct _vcb * vcb = xqp->current_vcb;
  struct _ucb * ucb = vcb->vcb$l_rvt; // was: finducb(fcb); // bad bad
  unsigned short *mp;
  unsigned short *me;

  if (head == NULL) {
    head = premap_indexf(fcb,ucb,&retsts);
    if (head == NULL) return NULL;
    head->fh2$w_ext_fid.fid$w_num = 0;
    head->fh2$w_ext_fid.fid$b_nmx = 0;
  }
  if (fh2)
    head=fh2;
  else {
    head = f11b_read_header(ucb->ucb$l_vcb,&fcb->fcb$w_fid,0,&iosb);
    retsts = iosb.iosb$w_status;
  }
#if 0
  if (fcb->fcb$w_fid[0]>1)
    retsts=gethead(0,ucb->ucb$l_vcb,fcb->fcb$w_fid,0,&head,NULL,0);
#endif
  mp = (unsigned short *) head + head->fh2$b_mpoffset;
  me = mp + head->fh2$b_map_inuse;
  while (mp < me) {
    unsigned phyblk, phylen;
    struct _wcb *wcb;
    get_fm2_val(&mp,&phyblk,&phylen);
    if (phylen!=0) {

      wcb = (struct _wcb *) kmalloc(sizeof(struct _wcb),GFP_KERNEL);
      bzero(wcb,sizeof(struct _wcb));
      if (wcb == NULL) {
	retsts = SS$_INSFMEM;
	return retsts;
      }

      wcb->wcb$b_type=DYN$C_WCB;
      wcb->wcb$l_orgucb=ucb;
      insque(wcb,&fcb->fcb$l_wlfl);
      wcb->wcb$l_fcb=fcb;

      wcb->wcb$l_stvbn=curvbn;
      wcb->wcb$l_p1_count=phylen;
      wcb->wcb$l_p1_lbn=phyblk;

      curvbn += phylen;

    }
  }
  retsts = SS$_NORMAL;
  return retsts;
}


/* getwindow() find a window to map VBN to LBN ... */

unsigned getwindow(struct _fcb * fcb,unsigned vbn,struct _vcb **devptr,
		   unsigned *phyblk,unsigned *phylen,struct _fiddef *hdrfid,
		   unsigned *hdrseq)
{
  unsigned sts=SS$_NORMAL;
  struct _wcb *wcb;
#ifdef DEBUG
  printk("Accessing window for vbn %d, file (%x)\n",vbn,fcb->cache.hashval);
#endif

  wcb = fcb->fcb$l_wlfl;

  *devptr = rvn_to_dev(fcb->fcb$l_wlfl->wcb$l_orgucb->ucb$l_vcb,0);
  sts=ioc_std$mapvblk(vbn,0,&fcb->fcb$l_wlfl,0,0,phyblk,0,0);
  *phylen = 1;
  if (hdrfid != NULL) memcpy(hdrfid,&wcb->wcb$l_fcb->fcb$w_fid,sizeof(struct _fiddef));
  //    if (hdrseq != NULL) *hdrseq = wcb->hd_seg_num;
#ifdef DEBUG
  printk("Mapping vbn %d to %d (%d -> %d)[%d] file (%x)\n",
	 vbn,*phyblk,wcb->loblk,wcb->hiblk,wcb->hd_basevbn,fcb->cache.hashval);
#endif

  if (*devptr == NULL) return SS$_DEVNOTMOUNT;
  return SS$_NORMAL;
}

/* deaccesschunk() to deaccess a VIOC (chunk of a file) */

unsigned deaccesschunk(unsigned wrtvbn,
		       int wrtblks,int reuse)
{
#ifdef DEBUG
  printk("Deaccess chunk %8x\n",vioc->cache.hashval);
#endif
  if (wrtvbn) {
    unsigned modmask;
#if 0
    if (wrtvbn <= vioc->cache.hashval ||
	wrtvbn + wrtblks > vioc->cache.hashval + VIOC_CHUNKSIZE + 1) {
      return SS$_BADPARAM;
    }
#endif
#if 0
    modmask = 1 << (wrtvbn - vioc->cache.hashval - 1);
    while (--wrtblks > 0) modmask |= modmask << 1;
    if ((vioc->wrtmask | modmask) != vioc->wrtmask) return SS$_WRITLCK;
    vioc->modmask |= modmask;
    if (vioc->cache.refcount == 1) vioc->wrtmask = 0;
    vioc->cache.objmanager = vioc_manager;
#endif
  }
  //cache_untouch(&vioc->cache,reuse);
  return SS$_NORMAL;
}

void *f11b_readvblock(struct _fcb * fcb,unsigned curvbn,unsigned *retsts)
{
  struct _iosb iosb;
  int length;
  char *address;
  length = fcb->fcb$l_efblk - curvbn + 1;
  length = 1;
  do {
    // no breaking of the water for now?
    if (fcb->fcb$l_highwater != 0 && curvbn >= fcb->fcb$l_highwater) {
      //memset(address,0,length * 512);
      break;
    } else {
      unsigned sts;
      unsigned phyblk,phylen;
      struct _vcb *vcbdev;
      sts = getwindow(fcb,curvbn,&vcbdev,&phyblk,&phylen,NULL,NULL);
      if (sts & 1) {
	if (phylen > length) phylen = length;
	if (fcb->fcb$l_highwater != 0 && curvbn + phylen > fcb->fcb$l_highwater) {
	  phylen = fcb->fcb$l_highwater - curvbn;
	}
	address = f11b_read_block(vcbdev,phyblk,phylen,&iosb);
	sts = iosb.iosb$w_status;
      }
      if ((sts & 1) == 0) {
	*retsts = sts;
	return NULL;
      }
      length -= phylen;
      curvbn += phylen;
      //address += phylen * 512;
    }
  } while (length > 0);
  *retsts = SS$_NORMAL;
  return address;
}

/* accesschunk() return pointer to a 'chunk' of a file ... */

unsigned accesschunk(struct _fcb *fcb,unsigned vbn,
		     char **retbuff,unsigned *retblocks,unsigned wrtblks, struct _irp * irp)
{
  unsigned sts;
  int blocks;
  if (!fcb) fcb=xqp->primary_fcb;
  //  if (vbn < 1 || vbn > fcb->fcb$l_efblk) return SS$_ENDOFFILE;
  if (vbn < 1 || vbn > 100000) { iosbret(irp, SS$_ENDOFFILE); return SS$_ENDOFFILE; } // for second read

  *retbuff = f11b_readvblock(fcb,vbn,&sts);

  if (retblocks) *retblocks=1;
  iosbret(irp,SS$_NORMAL);
  return SS$_NORMAL;
}


unsigned deallocfile(struct _fcb *fcb);

/* deaccessfile() finish accessing a file.... */

unsigned deaccessfile(struct _fcb *fcb)
{
  struct _iosb iosb;
  int sts;
  struct _fh2 * head;
  return SS$_NORMAL;
#ifdef DEBUG
  printk("Deaccessing file (%x) reference %d\n",fcb->cache.hashval,fcb->cache.refcount);
#endif
  head = f11b_read_header (fcb->fcb$l_wlfl->wcb$l_orgucb->ucb$l_vcb, 0, fcb, &iosb);  
  sts=iosb.iosb$w_status;
  if (VMSLONG(head->fh2$l_filechar) & FH2$M_MARKDEL) {
    return deallocfile(fcb);
  }

  return SS$_NORMAL;
}

static void *fcb_create(unsigned filenum,unsigned *retsts)
{
  struct _fcb *fcb = (struct _fcb *) kmalloc(sizeof(struct _fcb),GFP_KERNEL);
  if (fcb == NULL) {
    if (retsts) *retsts = SS$_INSFMEM;
  } else {
    qhead_init(&fcb->fcb$l_wlfl);
    fcb->fcb$l_efblk = 100000;
    fcb->fcb$l_highwater = 0;
    fcb->fcb$l_status = 0;
    fcb->fcb$b_fid_rvn = 0;
  }
  return fcb;
}

void *fcb_create2(struct _fh2 * head,unsigned *retsts)
{
  struct _vcb * vcb=xqp->current_vcb;
  struct _fcb *fcb = (struct _fcb *) kmalloc(sizeof(struct _fcb),GFP_KERNEL);
  bzero(fcb,sizeof(struct _fcb));
  if (fcb == NULL) {
    if (retsts) *retsts = SS$_INSFMEM;
    return;
  } 
  fcb->fcb$b_type=DYN$C_FCB;
  qhead_init(&fcb->fcb$l_wlfl);

  fcb->fcb$w_fid_num=head->fh2$w_fid.fid$w_num;
  fcb->fcb$w_fid_seq=head->fh2$w_fid.fid$w_seq;

  insque(fcb,&vcb->vcb$l_fcbfl);

  fcb->fcb$l_efblk = VMSSWAP(head->fh2$w_recattr.fat$l_hiblk);
  if (head->fh2$b_idoffset > 39) {
    fcb->fcb$l_highwater = VMSLONG(head->fh2$l_highwater);
  } else {
    fcb->fcb$l_highwater = 0;
  }

  wcb_create_all(fcb,head);

  if (head->fh2$l_filechar & FH2$M_CONTIG)
    fcb->fcb$l_stlbn=f11b_map_vbn(1,&fcb->fcb$l_wlfl);
  fcb->fcb$l_hdlbn=f11b_map_idxvbn(vcb,head->fh2$w_fid.fid$w_num + (head->fh2$w_fid.fid$b_nmx << 16) - 1 + VMSWORD(vcb->vcb$l_ibmapvbn) + VMSWORD(vcb->vcb$l_ibmapsize));

  *retsts=SS$_NORMAL;
  return fcb;
}


/* accessfile() open up file for access... */

unsigned f11b_access(struct _vcb * vcb, struct _irp * irp)
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

  if ((irp->irp$l_func & IO$M_ACCESS) == 0 && irp->irp$l_qio_p5 == 0)
    return SS$_NORMAL;

  if (wrtflg && ((vcb->vcb$b_status & VCB$M_WRITE_IF) == 0)) { iosbret(irp,SS$_WRITLCK);  return SS$_WRITLCK; }

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

  if (atrp) f11b_read_attrib(fcb,atrp);
  iosbret(irp,SS$_NORMAL);
  return SS$_NORMAL;
}




/* dismount() finish processing on a volume */

unsigned dismount(struct _vcb * vcb)
{
#if 0
  unsigned sts,device;
  struct _vcb *vcbdev;
  int expectfiles = vcb->devices;
  int openfiles = cache_refcount(&vcb->fcb->cache);
  if (vcb->vcb$b_status & VCB$M_WRITE_IF) expectfiles *= 2;
#ifdef DEBUG
  printk("Dismounting disk %d\n",openfiles);
#endif
  sts = SS$_NORMAL;
  if (openfiles != expectfiles) {
    sts = SS$_DEVNOTDISM;
  } else {
    vcbdev = vcb->vcbdev;
    for (device = 0; device < vcb->devices; device++) {
      if (vcbdev->dev != NULL) {
	if (vcb->vcb$b_status & VCB$M_WRITE_IF && getmapfcb(vcb) != NULL) {
	  sts = deaccessfile(vcbdev->getmapfcb(vcb));
	  if (!(sts & 1)) break;
	  vcbdev->idxfcb->fcb$l_status &= ~FCB_WRITE;
	  //                    vcbdev->mapfcb = NULL;
	}
	cache_remove(&vcb->fcb->cache);
	sts = deaccesshead(vcbdev->idxfcb->headvioc,vcbdev->idxfcb->head,vcbdev->idxfcb->headvbn);
	if (!(sts & 1)) break;
	cache_untouch(&vcbdev->idxfcb->cache,0);
	vcbdev->dev->vcb = NULL;
      }
      vcbdev++;
    }
    if (sts & 1) {
      cache_remove(&vcb->fcb->cache);
      while (vcb->dircache) cache_delete((struct CACHE *) vcb->dircache);
#ifdef DEBUG
      printk("Post close\n");
      cachedump();
#endif
      kfree(vcb);
    }
  }
  return sts;
#endif
}

#define HOME_LIMIT 100

/* mount() make disk volume available for processing... */

unsigned mount(unsigned flags,unsigned devices,char *devnam[],char *label[],struct _vcb **retvcb)
{
  struct _iosb iosb;
  unsigned device,sts;
  struct _vcb *vcb;
  struct _vcb *vcbdev;
  struct _ucb *ucb;
  struct _hm2 home;
  struct _aqb *aqb;
  int islocal;
  if (sizeof(struct _hm2) != 512 || sizeof(struct _fh2) != 512) return SS$_NOTINSTALL;
  for (device = 0; device < devices; device++) {
    //printk("Trying to mount %s\n",devnam[device]);
    if (strchr(devnam[device],'_')) {
      ucb = du_init(devnam[device]);
      islocal=0;
    } else {
      struct dsc$descriptor dsc;
      int chan;
      char buf[5];
      int sts;
      memcpy(buf,devnam[device],4);
      buf[4]=0;
      dsc.dsc$a_pointer=buf;
      dsc.dsc$w_length=4;

      sts=exe$assign(&dsc,&chan,0,0,0);

      //printk("sts %x\n",sts);
      if ((sts & 1)==0) {
	ucb = fl_init(devnam[device]);
      } else {
	extern struct _ccb ctl$ga_ccb_table[];
	ucb = ctl$ga_ccb_table[chan].ccb$l_ucb;
      }
      islocal=1;
    }
    vcb = (struct _vcb *) kmalloc(sizeof(struct _vcb),GFP_KERNEL);
    bzero(vcb,sizeof(struct _vcb));
    vcb->vcb$b_type=DYN$C_VCB;
    xqp->current_vcb=vcb; // until I can place it somewhere else
    aqb = (struct _aqb *) kmalloc(sizeof(struct _aqb),GFP_KERNEL);
    bzero(aqb,sizeof(struct _aqb));
    aqb->aqb$b_type=DYN$C_AQB;
    qhead_init(&aqb->aqb$l_acpqfl);
    ucb->ucb$l_vcb=vcb;
    vcb->vcb$l_aqb=aqb;
    if (vcb == NULL) return SS$_INSFMEM;
    vcb->vcb$b_status = 0;
    if (flags & 1) vcb->vcb$b_status |= VCB$M_WRITE_IF;
    qhead_init(&vcb->vcb$l_fcbfl);
    vcb->vcb$l_cache = NULL; // ?
    vcbdev = vcb;
    sts = SS$_NOSUCHVOL;
    //    vcbdev->dev = NULL;
    if (strlen(devnam[device])) {
      int hba;
      int chan;
      struct dsc$descriptor dsc;
      if (islocal)
	sts = phyio_init(strlen(devnam[device])+1,devnam[device],&ucb->ucb$l_vcb->vcb$l_aqb->aqb$l_mount_count,0);
      dsc.dsc$a_pointer=do_file_translate(devnam[device]);
      dsc.dsc$w_length=strlen(dsc.dsc$a_pointer);
#if 0
      if (!islocal)
	dsc.dsc$a_pointer=((char *) dsc.dsc$a_pointer)+1;
#endif
      sts=exe$assign(&dsc,&chan,0,0,0);
      xqp->io_channel=chan;
      ucb->ucb$ps_adp=chan; //wrong field and use, but....
      //sts = device_lookup(strlen(devnam[device]),devnam[device],1,&ucbret);
      //if (!(sts & 1)) break;
      //      ucb->handle=vcbdev->dev->handle;
      for (hba = 1; hba <= HOME_LIMIT; hba++) {
	sts = sys$qiow(0,chan,IO$_READLBLK,&iosb,0,0,(char *) &home,sizeof(struct _hm2),hba,0,0,0);
	if (!(sts & 1)) break;
	if (hba == VMSLONG(home.hm2$l_homelbn) &&
	    memcmp(home.hm2$t_format,"DECFILE11B  ",12) == 0) break;
	sts = SS$_DATACHECK;
      }
      if (sts & 1) {
	if (VMSWORD(home.hm2$w_checksum2) != checksum((unsigned short *) &home)) {
	  sts = SS$_DATACHECK;
	} else {
	  if (VMSWORD(home.hm2$w_rvn) != device + 1)
	    if (VMSWORD(home.hm2$w_rvn) > 1 || device != 0)
	      sts = SS$_UNSUPVOLSET;
#if 0
	  move this
	    if (vcbdev->vcb$l_aqb->aqb$l_mount_count != NULL) {
	      sts = SS$_DEVMOUNT;
	    }
#endif
	}
      }
      if (!(sts & 1)) break;
    }
    if (sts & 1) {
      vcbdev = vcb;
      if (strlen(devnam[device])) {
	struct _fiddef idxfid = {1,1,0,0};
	struct _fcb * idxfcb;
	struct _fh2 * idxhd;
	struct _fcb * mapfcb;
	//	  vcb->vcb$ibmaplbn = idxfcb->stlbn;
	vcb->vcb$l_homelbn = home.hm2$l_homelbn;
	vcb->vcb$l_ibmaplbn = home.hm2$l_ibmaplbn;
	vcb->vcb$l_ibmapvbn = home.hm2$w_ibmapvbn;
	vcb->vcb$l_ibmapsize = home.hm2$w_ibmapsize;  // wrong use
	vcb->vcb$l_cluster = home.hm2$w_cluster;
	vcb->vcb$l_maxfiles = home.hm2$l_maxfiles;
	//vcb->vcb$l_free = 500; // how do we compute this?
	memcpy(&vcb->vcb$t_volname,home.hm2$t_volname,12);
	vcb->vcb$l_rvt = ucb; // just single volume so far
	idxfid.fid$b_rvn = device + 1;
	//sts = accessfile(vcb,&idxfid,&idxfcb,flags & 1);
	idxhd = f11b_read_block(vcbdev,VMSLONG(vcbdev->vcb$l_ibmaplbn) + VMSWORD(vcbdev->vcb$l_ibmapsize),1, &iosb);
	idxfcb=fcb_create2(idxhd,&sts);
#if 0
	idxfcb=vmalloc(sizeof(struct _fcb));
	bzero(idxfcb,sizeof(struct _fcb));
	qhead_init(&idxfcb->fcb$l_wlfl);
	idxfcb->fcb$w_fid[0]=1;
	idxfcb->fcb$w_fid[1]=1;
	insque(idxfcb,&vcb->vcb$l_fcbfl);
	wcb_create_all(idxfcb,idxhd);
#endif

	if (!(iosb.iosb$w_status & 1)) {
	  ucb->ucb$l_vcb = NULL;
	} else {
	  ucb->ucb$l_vcb = vcb;
	  //insque(idxfcb,&vcb->vcb$l_fcbfl);
	  if (1) {
	    struct _fibdef mapfib = {0,2,2,1,0};
	    struct dsc$descriptor mapdsc;
	    struct _irp * dummyirp = kmalloc(sizeof(struct _irp),GFP_KERNEL);
	    mapdsc.dsc$w_length=sizeof(struct _fibdef);
	    mapdsc.dsc$a_pointer=&mapfib;
	    bzero(dummyirp,sizeof(struct _irp));
	    dummyirp->irp$l_qio_p1=&mapdsc;
	    dummyirp->irp$l_ucb=ucb;
	    dummyirp->irp$l_func=IO$_ACCESS|IO$M_ACCESS;
	    sts = f11b_access(vcb,dummyirp);
	    mapfcb=getmapfcb(vcb);
	    if (sts & 1) {
	      struct _scbdef *scb;
	      //insque(mapfcb,&vcb->vcb$l_fcbfl);
	      sts=ioc_std$mapvblk(1,0,&mapfcb->fcb$l_wlfl,0,0,&vcb->vcb$l_sbmaplbn,0,0);
	      sts = accesschunk(mapfcb,1,(char **) &scb,NULL,0,0);
	      if (sts & 1) {
		vcbdev->vcb$l_sbmaplbn=mapfcb->fcb$l_wlfl->wcb$l_p1_lbn;
		vcbdev->vcb$l_sbmapsize=scb->scb$l_volsize/(512*8*scb->scb$w_cluster)+1;
		if (scb->scb$w_cluster == home.hm2$w_cluster) {
		  //vcbdev->max_cluster = (scb->scb$l_volsize + scb->scb$w_cluster - 1) / scb->scb$w_cluster;
		  sts = update_freecount(vcbdev,&vcbdev->vcb$l_free);
		  vcbdev->vcb$l_free*=vcbdev->vcb$l_cluster;
		  printk("Freespace is %d\n",vcbdev->vcb$l_free);
		}
	      }
	    }
	  }
	}
      }
    }
  }
  if (retvcb != NULL) *retvcb = vcb;
  return sts;
}
