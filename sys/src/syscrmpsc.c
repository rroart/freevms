// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/sched.h>
#include <linux/vmalloc.h>
#include <linux/file.h>
#include <system_data_cells.h>
#include<descrip.h>
#include<fcbdef.h>
#include<gsddef.h>
#include<ipldef.h>
#include<phddef.h>
#include<rdedef.h>
#include<secdef.h>
#include<seciddef.h>
#include<starlet.h>
#include<va_rangedef.h>
#include<vmspte.h>
#include<wsldef.h>

asmlinkage int exe$crmpsc_wrap(struct struct_crmpsc * s) {
#ifdef CONFIG_VMS
  int ret;
  int chan=s->chan;
  struct file * file=0;
  struct _fcb * fcb = 0;
  if (chan) { 
    file=fget(chan);
    fcb=e2_search_fcb(file->f_dentry->d_inode);
  }
  
  ret=exe$crmpsc(s->inadr,s->retadr,s->acmode,s->flags,s->gsdnam,s->ident,s->relpag,fcb,s->pagcnt,s->vbn,s->prot,s->pfc);
  if (file) fput(file);
  return ret;
#endif
}

asmlinkage int exe$mgblsc_wrap(struct struct_mgblsc * s) {
  return exe$mgblsc(s->inadr,s->retadr,s->acmode,s->flags,s->gsdnam,s->ident,s->relpag);}

asmlinkage int exe$mgblsc(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode, unsigned int flags, void *gsdnam, struct _secid *ident, unsigned int relpag) {
  struct dsc$descriptor * dsc = gsdnam;
  char * name;
  struct _secdef * sec, * pstl;
  unsigned long long return_ident;
  char * return_gsnam;
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

/* no short int yet*/
asmlinkage int exe$crmpsc(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode, unsigned int flags, void *gsdnam, unsigned long long * ident, unsigned int relpag, unsigned /*short*/ int chan, unsigned int pagcnt, unsigned int vbn, unsigned int prot,unsigned int pfc) {
  // we will just have to pretend this fd is chan and offset i vbn (mmap)?
  // fd -> file, have a version with file = fget(fd);

  struct _secdef *sec, *pstl;
  void * first=inadr->va_range$ps_start_va;
  void * last=inadr->va_range$ps_end_va;
  struct _pcb * p=smp$gl_cpu_data[smp_processor_id()]->cpu$l_curpcb;
  unsigned long prot_pte;
  struct _rde * rde;
  int savipl=getipl();

  mmg$vfysecflg();
  // check channel
  // skip wcb for some time?
  setipl(IPL$_ASTDEL);
  mmg$dalcstxscn();

  if ((flags & SEC$M_GBL)==0) {
    prot_pte=p->pcb$l_phd->phd$l_pst_free<<PAGE_SHIFT;
    prot_pte |= 0xc00; // TYP1 and TYP0
    pstl=p->pcb$l_phd->phd$l_pst_base_offset;
    sec=&pstl[p->pcb$l_phd->phd$l_pst_free++];
    sec->sec$l_flags=flags;
    sec->sec$l_vbn=vbn;
    sec->sec$l_unit_cnt=pagcnt;
    sec->sec$l_vpx=first;

    sec->sec$l_ccb=chan;
    sec->sec$l_window=((struct _fcb*)chan)->fcb$l_wlfl;
    mmg$fast_create(p, 0, first, last, (last-first)>>PAGE_SHIFT, prot_pte);

    setipl(savipl);

  } else {
    int i;
    struct dsc$descriptor * dsc = gsdnam;
    char * name;
    unsigned long long return_ident;
    char * return_gsnam;
    struct _gsd * gsd;
    struct _mypte gpte;
    prot_pte=((struct _phd *)mmg$gl_sysphd)->phd$l_pst_free<<PAGE_SHIFT;
    prot_pte |= 0x400; // TYP1 and TYP0
    pstl=((struct _phd *)mmg$gl_sysphd)->phd$l_pst_base_offset;
    sec=&pstl[((struct _phd *)mmg$gl_sysphd)->phd$l_pst_free++];
    sec->sec$l_flags=flags;
    sec->sec$l_vbn=vbn;
    sec->sec$l_unit_cnt=pagcnt;
    sec->sec$l_vpx=mmg$gq_free_gpt;
    sec->sec$l_window=chan;

    mmg$gsdscn(gsdnam,ident,flags,return_gsnam,&return_ident,&gsd);
    if (gsd) {
      return exe$mgblsc(inadr,retadr,acmode,flags,gsdnam,ident,relpag);
    }
    gsd=kmalloc(sizeof(struct _gsd),GFP_KERNEL);
    bzero(gsd,sizeof(struct _gsd));
    name=kmalloc(dsc->dsc$w_length,GFP_KERNEL);
    bcopy(dsc->dsc$a_pointer,name,dsc->dsc$w_length);
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
