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

#include <stdio.h>
#include <linux/string.h>
//#include <memory.h>
//#include "ssdef.h"
//#include "phyio.h"

#include <mytypes.h>
#include <aqbdef.h>
#include <fatdef.h>
#include <vcbdef.h>
#include <descrip.h>
#include <ssdef.h>
#include <uicdef.h>
#include <namdef.h>
#include <fabdef.h>
#include <rabdef.h>
#include <fibdef.h>
#include <fiddef.h>
#include <rmsdef.h>
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

#include "cache.h"
#include "access.h"

#define DEBUGx

extern struct _ucb * myfilelist[50];
extern int myfilelistptr;

// really really bad

static struct _ucb * finducb(struct _fcb * fcb) {
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

// half broken

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
  struct _ucb * ucb=finducb(fcb);
  int pbn;
  int sts=ioc_std$mapvblk(vblock,0,&fcb->fcb$l_wlfl,0,0,&pbn,0,0);
  return phyio_write(ucb->ucb$l_vcb->vcb$l_aqb->aqb$l_acppid,pbn,512,buff);
}

/* accesshead() find file or extension header from INDEXF... */

unsigned gethead(struct _fcb * fcb,struct _fh2 **headbuff)
{
  struct _ucb * ucb=finducb(fcb);
  int vbn;
  int sts;
  struct _fiddef fid;
  fid.fid$w_num=fcb->fcb$w_fid[0];
  fid.fid$w_seq=fcb->fcb$w_fid[1];
  fid.fid$w_rvn=0;
  return accesshead(ucb->ucb$l_vcb,&fid,0,headbuff, &vbn, 0);
}

unsigned writehead(struct _fcb * fcb,struct _fh2 *headbuff)
{
  struct _ucb * ucb=finducb(fcb);
  struct _fiddef * fid = &headbuff->fh2$w_fid.fid$w_num;
  int vbn=fid->fid$w_num + (fid->fid$b_nmx << 16) - 1 +
    VMSWORD(ucb->ucb$l_vcb->vcb$l_ibmapvbn) + VMSWORD(ucb->ucb$l_vcb->vcb$l_ibmapsize);;
  return writechunk(getidxfcb(ucb->ucb$l_vcb),vbn, headbuff);

}

unsigned accesshead(struct _vcb *vcb,struct _fiddef *fid,unsigned seg_num,
                    struct _fh2 **headbuff,
                    unsigned *retidxblk,unsigned wrtflg)
{
  unsigned sts;
  struct _vcb *vcbdev;
  unsigned idxblk;
  struct _fh2 idxfh;
  vcbdev = rvn_to_dev(vcb,fid->fid$b_rvn);
  if (vcbdev == NULL) return SS$_DEVNOTMOUNT;
  if (wrtflg && ((vcb->vcb$b_status & VCB$M_WRITE_IF) == 0)) return SS$_WRITLCK;
  idxblk = fid->fid$w_num + (fid->fid$b_nmx << 16) - 1 +
    VMSWORD(vcbdev->vcb$l_ibmapvbn) + VMSWORD(vcbdev->vcb$l_ibmapsize);
#if 0
  if (vcbdev->idxfcb->head != NULL) 
    if (idxblk >= VMSSWAP(vcbdev->idxfcb->head->fh2$w_recattr.fat$l_efblk)) 
      phyio_read(vcb->vcb$l_aqb->aqb$l_acppid,vcb->vcb$l_ibmaplbn,sizeof(struct _fh2),(char *)&idxfh);
#endif
#if 0
  not yet
    if (idxblk >= VMSSWAP(idxfh.fh2$w_recattr.fat$l_efblk)) {
      {
	printk("Not in index file\n");
	return SS$_NOSUCHFILE;
      }
    }
#endif
  sts = accesschunk(getidxfcb(vcb),idxblk,(char **) headbuff,
		    NULL,wrtflg ? 1 : 0);
  if (sts & 1) {
    struct _fh2 *head = *headbuff;
    if (retidxblk) {
      if (1 /*wrtflg*/) {
	*retidxblk = idxblk;
      } else {
	*retidxblk = 0;
      }
    }
    if (VMSWORD(head->fh2$w_fid.fid$w_num) != fid->fid$w_num ||
	head->fh2$w_fid.fid$b_nmx != fid->fid$b_nmx ||
	VMSWORD(head->fh2$w_fid.fid$w_seq) != fid->fid$w_seq ||
	(head->fh2$w_fid.fid$b_rvn != fid->fid$b_rvn &&
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
	if (VMSWORD(head->fh2$w_seg_num) != seg_num) sts = SS$_FILESEQCHK;
      }
    }
    if ((sts & 1) == 0) deaccesschunk(0,0,0);
  }
  return sts;
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
  struct _fh2 *head;
  struct _vcb *vcbdev = rvn_to_dev(ucb->ucb$l_vcb,fcb->fcb$b_fid_rvn);
  if (vcbdev == NULL) {
    *retsts = SS$_DEVNOTMOUNT;
    return NULL;
  }
  head = (struct _fh2 *) vmalloc(sizeof(struct _fh2));
  if (head == NULL) {
    *retsts = SS$_INSFMEM;
  } else {
    int sts;
#if 0
    struct _hm2 home;
    sts = phyio_read(vcbdev->vcb$l_aqb->aqb$l_acppid,vcbdev->vcb$l_homelbn,sizeof(struct _hm2),(char *) &home);
#endif
    *retsts = phyio_read(vcbdev->vcb$l_aqb->aqb$l_acppid,VMSLONG(vcbdev->vcb$l_ibmaplbn) + VMSWORD(vcbdev->vcb$l_ibmapsize),sizeof(struct _fh2), (char *) head);
    if (!(*retsts & 1)) {
      vfree(head);
      head = NULL;
    } else {
      if (VMSWORD(head->fh2$w_fid.fid$w_num) != 1 ||
	  head->fh2$w_fid.fid$b_nmx != 0 ||
	  VMSWORD(head->fh2$w_fid.fid$w_seq) != 1 ||
	  VMSWORD(head->fh2$w_checksum) != checksum((unsigned short *) head)) {
	*retsts = SS$_DATACHECK;
	vfree(head);
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

/* wcb_create() creates a window control block by reading appropriate
   file headers... */

int wcb_create_all(struct _fcb * fcb, struct _fh2 * fh2)
{
  unsigned int retsts;
  unsigned curvbn=1;
  unsigned extents = 0;
  struct _fh2 *head = 0;

  struct _ucb * ucb = finducb(fcb); // bad bad
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
  else
    retsts = accesshead(ucb->ucb$l_vcb,&fcb->fcb$w_fid,0,&head,NULL,0);
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

      wcb = (struct _wcb *) vmalloc(sizeof(struct _wcb));
      if (wcb == NULL) {
	retsts = SS$_INSFMEM;
	return retsts;
      }

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

#if 0
unsigned getwindow(struct _fcb * fcb,unsigned vbn,struct _vcb **devptr,
		   unsigned *phyblk,unsigned *phylen,struct _fiddef *hdrfid,
		   unsigned *hdrseq)
{
  unsigned sts=SS$_NORMAL;
  struct _wcb *wcb;
  struct WCBKEY wcbkey;
#ifdef DEBUG
  printk("Accessing window for vbn %d, file (%x)\n",vbn,fcb->cache.hashval);
#endif
  wcbkey.vbn = vbn;
  wcbkey.fcb = fcb;
  wcbkey.prevwcb = NULL;
  //wcb = cache_find((void *) &fcb->fcb$l_wlfl,0,&wcbkey,&sts,wcb_compare,wcb_create);
  {
    struct _wcb * head=&fcb->fcb$l_wlfl;
    struct _wcb * tmp=head->wcb$l_wlfl;
    while(tmp!=head) {
      if (0==wcb_compare(0,&wcbkey,tmp)) goto end;
      tmp=tmp->wcb$l_wlfl;
    }
  end:
    if (tmp==head) { 
      wcb = wcb_create(0,&wcbkey,&sts);
      if (wcb) {
	//insque(wcb,&fcb->fcb$l_wlfl);
      }
    } else {
      wcb = tmp;
    }
  }
  if (wcb == NULL) return sts;
  {
    unsigned extent = 0;
    unsigned togo = vbn - wcb->loblk;

    while (togo >= wcb->phylen[extent]) {
      togo -= wcb->phylen[extent];
      if (++extent > wcb->extcount) return SS$_BUGCHECK;
    }

    *devptr = rvn_to_dev(fcb->fcb$l_wlfl->wcb$l_orgucb->ucb$l_vcb,0);
    *phyblk = wcb->phyblk[extent] + togo;
    *phylen = wcb->phylen[extent] - togo;
    if (hdrfid != NULL) memcpy(hdrfid,&wcb->wcb$l_fcb->fcb$w_fid,sizeof(struct _fiddef));
    if (hdrseq != NULL) *hdrseq = wcb->hd_seg_num;
#ifdef DEBUG
    printk("Mapping vbn %d to %d (%d -> %d)[%d] file (%x)\n",
	   vbn,*phyblk,wcb->loblk,wcb->hiblk,wcb->hd_basevbn,fcb->cache.hashval);
#endif
    //cache_untouch(&wcb->cache,1);
  }
  if (*devptr == NULL) return SS$_DEVNOTMOUNT;
  return SS$_NORMAL;
}
#endif

unsigned getwindow(struct _fcb * fcb,unsigned vbn,struct _vcb **devptr,
		   unsigned *phyblk,unsigned *phylen,struct _fiddef *hdrfid,
		   unsigned *hdrseq)
{
  unsigned sts=SS$_NORMAL;
  struct _wcb *wcb;
#ifdef DEBUG
  printk("Accessing window for vbn %d, file (%x)\n",vbn,fcb->cache.hashval);
#endif
  if (aqempty(&fcb->fcb$l_wlfl))
    sts = wcb_create_all(fcb,0);
  if (aqempty(&fcb->fcb$l_wlfl)) return sts;
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
  //cache_untouch(&wcb->cache,1);

  if (*devptr == NULL) return SS$_DEVNOTMOUNT;
  return SS$_NORMAL;
}

/* Object manager for VIOC objects:- if the object has been
   modified then we need to flush it to disk before we let
   the cache routines do anything to it... */

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


void *vioc_create(unsigned hashval,void *keyval,unsigned *retsts)
{
  int length;
  unsigned curvbn = hashval + 1;
  char *address;
  struct _fcb *fcb = (struct _fcb *) keyval;
  length = fcb->fcb$l_efblk - curvbn + 1;
  length = 1;
  address = (char *) vmalloc(512);
  do {
    // no breaking of the water for now?
    if (fcb->fcb$l_highwater != 0 && curvbn >= fcb->fcb$l_highwater) {
      memset(address,0,length * 512);
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
	sts = phyio_read(vcbdev->vcb$l_aqb->aqb$l_acppid,phyblk,phylen * 512,address);
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
		     char **retbuff,unsigned *retblocks,unsigned wrtblks)
{
  unsigned sts;
  int blocks;
#ifdef DEBUG
  printk("Access chunk %8x %d (%x)\n",base,vbn,fcb->cache.hashval);
#endif
  //  if (vbn < 1 || vbn > fcb->fcb$l_efblk) return SS$_ENDOFFILE;
  if (vbn < 1 || vbn > 100000) return SS$_ENDOFFILE; // for second read

  *retbuff = vioc_create(vbn-1,fcb,&sts);

  if (retblocks) *retblocks=1;
  return SS$_NORMAL;
}


unsigned deallocfile(struct _fcb *fcb);

/* deaccessfile() finish accessing a file.... */

unsigned deaccessfile(struct _fcb *fcb)
{
  int sts;
  struct _fh2 * head;
#ifdef DEBUG
  printk("Deaccessing file (%x) reference %d\n",fcb->cache.hashval,fcb->cache.refcount);
#endif
  sts=gethead(fcb,&head);
  if (VMSLONG(head->fh2$l_filechar) & FH2$M_MARKDEL) {
    return deallocfile(fcb);
  }

  return SS$_NORMAL;
}

void *fcb_create(unsigned filenum,void *keyval,unsigned *retsts)
{
  struct _fcb *fcb = (struct _fcb *) vmalloc(sizeof(struct _fcb));
  if (fcb == NULL) {
    *retsts = SS$_INSFMEM;
  } else {
    qhead_init(&fcb->fcb$l_wlfl);
    fcb->fcb$l_efblk = 100000;
    fcb->fcb$l_highwater = 0;
    fcb->fcb$l_status = 0;
    fcb->fcb$b_fid_rvn = 0;
  }
  return fcb;
}


/* accessfile() open up file for access... */

unsigned accessfile(struct _vcb * vcb,struct _fiddef * fid,struct _fcb **fcbadd,
		    unsigned wrtflg)
{
  unsigned sts;
  struct _fcb *fcb;
  struct _fh2 *head;
  unsigned filenum = (fid->fid$b_nmx << 16) + fid->fid$w_num;
#ifdef DEBUG
  printk("Accessing file (%d,%d,%d)\n",(fid->fid$b_nmx << 16) +
	 fid->fid$w_num,fid->fid$w_seq,fid->fid$b_rvn);
#endif
  if (filenum < 1) return SS$_BADPARAM;
  if (wrtflg && ((vcb->vcb$b_status & VCB$M_WRITE_IF) == 0)) return SS$_WRITLCK;
  if (fid->fid$b_rvn > 1) filenum |= fid->fid$b_rvn << 24;
  //fcb = cache_find((void *) &vcb->vcb$l_fcbfl,filenum,NULL,&sts,NULL,fcb_create);
  {
    struct _fcb * head = &vcb->vcb$l_fcbfl;
    struct _fcb * tmp = head->fcb$l_fcbfl;
    while(tmp!=head) {
      if (filenum==((tmp->fcb$b_fid_nmx << 16) + tmp->fcb$w_fid_num)) goto end;
      tmp=tmp->fcb$l_fcbfl;
    }
  end:
    if (tmp==head) {
      fcb=fcb_create(filenum,0,&sts);
      if (fcb) {
	fcb->fcb$w_fid_num=fid->fid$w_num;
	fcb->fcb$w_fid_seq=fid->fid$w_seq;
	insque(fcb,&vcb->vcb$l_fcbfl);
      }
    } else {
      fcb = tmp;
    }
  }
  if (fcb == NULL) return sts;
  /* If not found make one... */
  *fcbadd = fcb;
#if 0
  if (fcb->vcb == NULL) {
    fcb->fcb$b_fid_rvn = fid->fid$b_rvn;
    //        if (fcb->fcb$b_fid_rvn == 0 && vcb->devices > 1) fcb->fcb$b_fid_rvn = 1;
    fcb->vcb = vcb;
  }
#endif
  if (wrtflg) {
#if 0
    if (fcb->headvioc != NULL && (fcb->fcb$l_status & FCB_WRITE) == 0) {
      deaccesshead(fcb->headvioc,NULL,0);
      fcb->headvioc = NULL;
    }
#endif
  }
#if 0
  if (fcb->headvioc == NULL) 
#else
    if (1) {
#endif
      unsigned sts;
      //        sts = accesshead(vcb,fid,0,&fcb->headvioc,&fcb->head,&fcb->headvbn,wrtflg);
      sts = accesshead(vcb,fid,0,&head,0,wrtflg);
      if (sts & 1) {
	fcb->fcb$l_efblk = VMSSWAP(head->fh2$w_recattr.fat$l_hiblk);
	if (head->fh2$b_idoffset > 39) {
	  fcb->fcb$l_highwater = VMSLONG(head->fh2$l_highwater);
	} else {
	  fcb->fcb$l_highwater = 0;
	}
      } else {
	printk("Accessfile status %d\n",sts);
	return sts;
      }
    }
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
      vfree(vcb);
    }
  }
  return sts;
#endif
}

#define HOME_LIMIT 100

/* mount() make disk volume available for processing... */

unsigned mount(unsigned flags,unsigned devices,char *devnam[],char *label[],struct _vcb **retvcb)
{
  unsigned device,sts;
  struct _vcb *vcb;
  struct _vcb *vcbdev;
  struct _ucb *ucb;
  struct _hm2 home;
  struct _aqb *aqb;
  if (sizeof(struct _hm2) != 512 || sizeof(struct _fh2) != 512) return SS$_NOTINSTALL;
  for (device = 0; device < devices; device++) {
    ucb = fl_init(devnam[device]);
    vcb = (struct _vcb *) vmalloc(sizeof(struct _vcb));
    bzero(vcb,sizeof(struct _vcb));
    aqb = (struct _aqb *) vmalloc(sizeof(struct _aqb));
    bzero(aqb,sizeof(struct _aqb));
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
      sts = phyio_init(strlen(devnam[device])+1,ucb->ucb$l_ddb->ddb$t_name,&ucb->ucb$l_vcb->vcb$l_aqb->aqb$l_acppid,0);
      //sts = device_lookup(strlen(devnam[device]),devnam[device],1,&ucbret);
      //if (!(sts & 1)) break;
      //      ucb->handle=vcbdev->dev->handle;
      for (hba = 1; hba <= HOME_LIMIT; hba++) {
	sts = phyio_read(vcbdev->vcb$l_aqb->aqb$l_acppid,hba,sizeof(struct _hm2),(char *) &home);
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
	    if (vcbdev->vcb$l_aqb->aqb$l_acppid != NULL) {
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
	idxfid.fid$b_rvn = device + 1;
	//sts = accessfile(vcb,&idxfid,&idxfcb,flags & 1);
	idxfcb=fcb_create(1,0,&sts);
	idxfcb->fcb$w_fid[0]=1;
	idxfcb->fcb$w_fid[1]=1;
	insque(idxfcb,&vcb->vcb$l_fcbfl);
	idxhd = vmalloc (sizeof(struct _fh2));
	sts = phyio_read(vcbdev->vcb$l_aqb->aqb$l_acppid,VMSLONG(vcbdev->vcb$l_ibmaplbn) + VMSWORD(vcbdev->vcb$l_ibmapsize),sizeof(struct _fh2), (char *) idxhd);
	wcb_create_all(idxfcb,idxhd);
	if (!(sts & 1)) {
	  ucb->ucb$l_vcb = NULL;
	} else {
	  ucb->ucb$l_vcb = vcb;
	  vcb->vcb$l_rvt = ucb; // just single volume so far
	  //insque(idxfcb,&vcb->vcb$l_fcbfl);
	  if (1) {
	    struct _fiddef mapfid = {2,2,0,0};
	    mapfid.fid$b_rvn = device + 1;
	    sts = accessfile(vcb,&mapfid,&mapfcb,1);
	    if (sts & 1) {
	      struct _scbdef *scb;
	      //insque(mapfcb,&vcb->vcb$l_fcbfl);
	      sts=ioc_std$mapvblk(1,0,&mapfcb->fcb$l_wlfl,0,0,&vcb->vcb$l_sbmaplbn,0,0);
	      sts = accesschunk(mapfcb,1,(char **) &scb,NULL,0);
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
