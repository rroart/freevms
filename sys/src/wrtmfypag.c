// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <linux/bitops.h>
#include <linux/config.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>

#include<dyndef.h>
#include<fcbdef.h>
#include<ipldef.h>
#include<irpdef.h>
#include<mpwdef.h>
#include<schdef.h>
#include<system_data_cells.h>
#include<vmspte.h> 
#include<ssdef.h>
#include<pfndef.h>
#include<pfldef.h>
#include<wcbdef.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <misc_routines.h>
#include <mmg_routines.h>

#if 0
struct _irp * mpw$gl_irpfl = &mpw$gl_irpfl;
struct _irp * mpw$gl_irpbl = &mpw$gl_irpfl;
#endif

unsigned char mpw$gb_state;

void mmg$purgempl(unsigned long command);

void mmg$wrtmfypag(void) {
#if 0
  // check this?
  if (mmg$gl_maxpfidx==0)
    return;
#endif
#if 0
  if (sch$gl_mfylim>sch$gl_mfycnt)
    return;
#else
  if (1>sch$gl_mfycnt)
    return;
#endif
  if (test_and_set_bit(SCH$V_MPW,&sch$gl_sip))
    return;
#if 0
  setipl(IPL$_MMG);
  vmslock(&SPIN_MMG,-1);
#endif
  mmg$purgempl(MPW$C_MAINTAIN);

  test_and_clear_bit(SCH$V_MPW,&sch$gl_sip);
}

void mmg$purgempl(unsigned long command) {
  if (1>sch$gl_mfycnt) // one extra check
    return;
  if (mpw$ar_perfstats==MPW$C_SVAPTE)
    return;
  mpw$gb_state=command;

  struct _irp * i=kmalloc(sizeof(struct _irp),GFP_KERNEL);
  memset(i,0,sizeof(struct _irp));
  // remember that cdrp place is borrowed to use as pte array etc
  i->irp$b_type=DYN$C_INIT;
#if 0
  i->irp$b_efn=efn;
  i->irp$l_ast=astadr;
  i->irp$l_astprm=astprm;
  i->irp$l_iosb=iosb;
  i->irp$w_chan=chan;
  i->irp$l_func=func;
  i->irp$b_pri=p->pcb$b_pri;
  i->irp$l_qio_p1=p1;
  i->irp$l_qio_p2=p2;
  i->irp$l_qio_p3=p3;
  i->irp$l_qio_p4=p4;
  i->irp$l_qio_p5=p5;
  i->irp$l_qio_p6=p6;
  i->irp$l_ucb=ctl$gl_ccbbase[chan].ccb$l_ucb;
  i->irp$l_pid=current->pcb$l_pid;
  i->irp$l_sts|=IRP$M_BUFIO; /* no DIRIO because of no mmg$svaptechk */
#endif

  long * apte = &i->irp_cdrp;
  long * aphv = apte + 1;

  struct _pfn * pfn = pfn$al_tail[PFN$C_MFYPAGLST];
  mmg$rempfn(PFN$C_MFYPAGLST,pfn);

#if 0
  long old_va=pfn->virtual;
#endif
  int pfl_page;
  int sts=mmg$allocpagfil1(1, &pfl_page);

  if (sts!=SS$_NORMAL)
    return;

  // try to replace this with 0x80000000 + otherpte->v_pfn?
#if 0
  int * mypage=vmalloc(4096/*,GFP_KERNEL*/);
  //  *mypage=42;
  pfn->virtual=mypage;
  unsigned long * pte = findpte_new(ctl$gl_pcb->mm,mypage);
  long oldpte = *pte;
#endif
  long pfnno = pfn-&mem_map[0];

  struct _mypte * otherpte=pfn->pfn$q_pte_index;
  struct _mypte * mypte = otherpte;
  struct _pte * pte = otherpte;
#if 0
  *(unsigned long *)pte=*(long*)otherpte;
#endif
  *(unsigned long *)pte|=_PAGE_PRESENT;

#ifndef __arch_um__
  __flush_tlb();
#endif

  extern int myswapfile;
  struct _pfl * pfl = myswapfile;

  if (pfl->pfl$l_window->wcb$l_fcb->fcb$l_fill_5)
  block_write_full_page3(pfl->pfl$l_window->wcb$l_fcb, pfn, pfl_page);
  else
  ods2_block_write_full_page3(pfl->pfl$l_window, pfn, pfl_page);
#if 0
  pfn->virtual=old_va;
#endif
  printk("Wrote to pfl_page %x %x %x\n",pfl_page,otherpte,*(long*)otherpte);

  *(unsigned long *)pte&=~_PAGE_PRESENT;
#if 0
  *pte=oldpte;
#endif
#ifndef __arch_um__
  __flush_tlb();
#endif
#if 0
  vfree(mypage);
#endif

  long * tmp=otherpte;
  *tmp=0;
  otherpte->pte$v_typ1=1;
  otherpte->pte$v_pgflpag=pfl_page;
  pfn->pfn$q_bak=*(long*)otherpte;

  mmg$inspfn(PFN$C_FREPAGLST,pfn,0/*pfn$al_head[PFN$C_FREPAGLST]*/);

  kfree(i);

}
