// $Id$
// $Locker$

// Author. Roar Thronæs.
/**
   \file syscrmspc.c
   \brief MM process private and global sections
   \author Roar Thronæs
*/

#include<linux/linkage.h>
#include<linux/sched.h>
#include <linux/vmalloc.h>
#include <linux/file.h>
#include <system_data_cells.h>
#include<descrip.h>
#include<gsddef.h>
#include<ipldef.h>
#include<ipl.h>
#include<phddef.h>
#include<rdedef.h>
#include<secdef.h>
#include<seciddef.h>
#include<starlet.h>
#include<va_rangedef.h>
#include<vmspte.h>
#include<wsldef.h>
#include <exe_routines.h>
#include <misc_routines.h>
#include <mmg_routines.h>
#include <queue.h>
#include <linux/slab.h>

asmlinkage int exe$crmpsc_wrap(struct struct_crmpsc * s) {
  int ret;
#if 0
  int chan=s->chan;
  struct file * file=0;
  struct _fcb * fcb = 0;
  if (chan) { 
    file=fget(chan);
    fcb=e2_search_fcb(file->f_dentry->d_inode);
  }
#endif  
  ret=exe$crmpsc(s->inadr,s->retadr,s->acmode,s->flags,s->gsdnam,s->ident,s->relpag,s->chan,s->pagcnt,s->vbn,s->prot,s->pfc);
#if 0
  if (file) fput(file);
#endif
  return ret;
}

asmlinkage int exe$mgblsc_wrap(struct struct_mgblsc * s) {
  return exe$mgblsc(s->inadr,s->retadr,s->acmode,s->flags,s->gsdnam,s->ident,s->relpag);}

asmlinkage int exe$mgblsc(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode, unsigned int flags, void *gsdnam, struct _secid *ident, unsigned int relpag) {
  struct dsc$descriptor * dsc = gsdnam;
  char * name;
  struct _secdef * sec, * pstl;
  unsigned long long return_ident;
  char * return_gsnam = 0; // not yet used?
  struct _gsd * gsd;
  unsigned long prot_pte=0x400; //get gpti here
  void * first=inadr->va_range$ps_start_va;
  void * last=inadr->va_range$ps_end_va;

  mmg$vfysecflg();
  //  mmg$dalcstxscn1();

  mmg$gsdscn(gsdnam,ident,flags,return_gsnam,&return_ident,&gsd);

  pstl=((struct _phd *)mmg$gl_sysphd)->phd$l_pst_base_offset;
  sec=&pstl[gsd->gsd$l_gstx];

  (*(struct _mypte *)&prot_pte).pte$v_gptx=sec->sec$l_vpx;

  mmg$fast_create_gptx(current, 0, first, last, (last-first)>>PAGE_SHIFT, prot_pte);
}

/**
   \brief system service for process-private section creation - see 5.2 15.4.1
   \param inadr suggested address range
   \param retadr returned address range
   \param acmode access mode
   \param gsd global section
*/

/** no short int yet*/
asmlinkage int exe$crmpsc(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode, unsigned int flags, void *gsdnam, unsigned long long * ident, unsigned int relpag, unsigned /*short*/ long chan, unsigned int pagcnt, unsigned int vbn, unsigned int prot,unsigned int pfc) {
  // we will just have to pretend this fd is chan and offset i vbn (mmap)?
  // fd -> file, have a version with file = fget(fd);
  /** initialize scratch space - MISSING ? */

  struct _secdef *sec, *pstl;
  void * first=inadr->va_range$ps_start_va;
  void * last=inadr->va_range$ps_end_va;
  struct _pcb * p=ctl$gl_pcb;
  unsigned long prot_pte;
  struct _rde * rde;
  int savipl=getipl();

  /** call mmg vfysecflg to test flag args - MISSING - empty routine */
  mmg$vfysecflg();
  // check channel
  /** confirm that the channel has been assigned etc - MISSING */
  /** check wheter the wcb maps the entire file - MISSING */
  // skip wcb for some time?
  /** if to be a copy-on-reference - MISSING */
  /** set ipl 2 to prevent asts */
  setipl(IPL$_ASTDEL);
  /** call mmg dalcstxscn to check process section table for deallocatable sections - MISSING */
  mmg$dalcstxscn();

  /** check for copy-on-reference - MISSING */
  /** check demand zero - MISSING */

  if ((flags & SEC$M_GBL)==0) {
    /** form a template pte */
    /** calculate protection and some more - MISSING */
    prot_pte=p->pcb$l_phd->phd$l_pst_free<<PAGE_SHIFT;
    prot_pte |= 0xc00; // TYP1 and TYP0
    pstl=p->pcb$l_phd->phd$l_pst_base_offset;
    sec=&pstl[p->pcb$l_phd->phd$l_pst_free++];
    /** set sec flags from param */
    sec->sec$l_flags=flags;
    /** set sec vbn from param */
    sec->sec$l_vbn=vbn;
    /** vbn 0 check - MISSING */
    /** copy to sec unit_cnt (pagcnt) from pagcnt param */
    sec->sec$l_unit_cnt=pagcnt;
    sec->sec$l_vpx=first;

    sec->sec$l_ccb=chan; // check. address later
    struct _ccb * ccb = &ctl$ga_ccb_table[chan];
    /** set in sec window the ccb window */
    sec->sec$l_window = ccb->ccb$l_wind;
    /** check whether file has been open consistently with the flags - MISSING */
    /** if this is the first section mapped - MISSING */
    /** set sec refcnt to 1 - MISSING */
    /** set sec pfc - MISSING */
    mmg$fast_create(p, 0, first, last, (last-first)>>PAGE_SHIFT, prot_pte);

    setipl(savipl);

  } else {
    int i;
    struct dsc$descriptor * dsc = gsdnam;
    char * name;
    unsigned long long return_ident;
    char * return_gsnam = 0; // not yet used?
    struct _gsd * gsd;
    struct _mypte gpte;
    /** form a template pte */
    /** calculate protection and some more - MISSING */
    prot_pte=((struct _phd *)mmg$gl_sysphd)->phd$l_pst_free<<PAGE_SHIFT;
    prot_pte |= 0x400; // TYP1 and TYP0
    pstl=((struct _phd *)mmg$gl_sysphd)->phd$l_pst_base_offset;
    sec=&pstl[((struct _phd *)mmg$gl_sysphd)->phd$l_pst_free++];
    /** set sec flags from param */
    sec->sec$l_flags=flags;
    /** set sec vbn from param */
    sec->sec$l_vbn=vbn;
    /** vbn 0 check - MISSING */
    /** copy to sec unit_cnt (pagcnt) from pagcnt param */
    sec->sec$l_unit_cnt=pagcnt;
    sec->sec$l_vpx=mmg$gq_free_gpt;
    /** set in sec window the ccb window */
    sec->sec$l_window=chan;
    /** check whether file has been open consistently with the flags - MISSING */
    /** if this is the first section mapped - MISSING */
    /** set sec refcnt to 1 - MISSING */
    /** set sec pfc - MISSING */

    mmg$gsdscn(gsdnam,ident,flags,return_gsnam,&return_ident,&gsd);
    if (gsd) {
      return exe$mgblsc(inadr,retadr,acmode,flags,gsdnam,ident,relpag);
    }
    gsd=kmalloc(sizeof(struct _gsd),GFP_KERNEL);
    memset(gsd,0,sizeof(struct _gsd));
    name=kmalloc(dsc->dsc$w_length,GFP_KERNEL);
    memcpy(name,dsc->dsc$a_pointer,dsc->dsc$w_length);
    gsd->gsd$l_basepfn=name; //really t_gsdnam
    gsd->gsd$l_ident=ident; //change later
    gsd->gsd$l_flags=flags;
    gsd->gsd$l_pcbuic=current->pcb$l_uic;
    gsd->gsd$l_gstx=(((struct _phd *)mmg$gl_sysphd)->phd$l_pst_free)-1;
    if (flags&SEC$M_SYSGBL) {
      insque(gsd,&exe$gl_gsdsysfl); 
    } else {
      insque(gsd,&exe$gl_gsdgrpfl);
    }
    sec->sec$l_gsd=gsd;
    if (chan) {
      gpte.pte$l_all=0;
      gpte.pte$v_typ1=1;
      gpte.pte$v_typ0=1;
      gpte.pte$v_stx=gsd->gsd$l_gstx;
      gpte.pte$v_global=1;
      //gpte.pte$v_gblwrt=1;
    } else {
      gpte.pte$l_all=0;
      gpte.pte$v_typ1=0;
      gpte.pte$v_typ0=0;
      gpte.pte$v_pfn=0;
      gpte.pte$v_global=1;
      gpte.pte$v_gblwrt=1;
    }
    for (i=mmg$gq_free_gpt;i<mmg$gq_free_gpt+((last-first)>>PAGE_SHIFT); i++) {
      ((unsigned long *)mmg$gq_gpt_base)[i]=*(unsigned long *)&gpte;
    }
      ((unsigned long *)mmg$gq_gpt_base)[i]=0;
    mmg$gq_free_gpt+=1+((last-first)>>PAGE_SHIFT);
    return exe$mgblsc(inadr,retadr,acmode,flags,gsdnam,ident,relpag);
  }

}
