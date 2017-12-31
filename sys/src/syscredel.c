// $Id$
// $Locker$

// Author. Roar Thronæs.
/**
   \file syscredel.c
   \brief MM create or delete
   \author Roar Thronæs
*/

#include<linux/config.h>
#include<linux/unistd.h>
#include<linux/linkage.h>
#include<linux/sched.h>
#include<linux/vmalloc.h>
#include <system_data_cells.h>
#include <linux/mm.h>
#include <ipldef.h>
#include <mmgdef.h>
#include <pfndef.h>
#include <phddef.h>
#include <rdedef.h>
#include <ssdef.h>
#include <starlet.h>
#include <va_rangedef.h>
#include <vmspte.h>
#include <wsldef.h>
#include <ipl.h>
#include <queue.h>
#include <mmg_functions.h>
#include <exe_routines.h>
#include <mmg_routines.h>
#include <linux/slab.h>
#include <internals.h>

#undef OLDINT
#define OLDINT

#ifdef __x86_64__
#undef OLDINT
#endif

// mmg$credel
// mmg$crepag
// mmg$cretva
// mmg$delpag
// mmg$fast_create
// mmg$try_all
// create_bufobj

/** note that adjstk - 5.2 15.3.3 - is MISSING */

inline struct _rde * mmg$lookup_rde_va (void * va, struct _phd * const phd, int function, int ipl);
inline struct _rde * mmg$search_rde_va (void * va, struct _rde *head, struct _rde **prev, struct _rde **next);

asmlinkage void exe$cntreg(void)
{
}

int vava=0;

int mmg$delpag(int acmode, void * va, struct _pcb * p, signed int pagedirection, struct _rde * rde, unsigned long newpte)
{
    //spinlock  , too
    int savipl=getipl();
    setipl(IPL$_MMG);
    if ((unsigned long)va>=0x3ff000)
    {
        //   vava=1;
    }

    pgd_t *pgd = 0;
    pud_t *pud = 0;
    pmd_t *pmd = 0;
    pte_t *pte = 0;
    unsigned long address=va;

    struct mm_struct * mm = p->mm;
    if (mm==0)
        mm = p->active_mm; // workaround for do_exit

    if (va>=rde->rde$pq_first_free_va)
    {

    }

    pgd = pgd_offset(mm, address);
    pud = pud_offset(pgd, address);
    pmd = pmd_offset(pud, address);
    if (pmd && *(long *)pmd)
    {
        pte = pte_offset(pmd, address);
    }

    if (vava)
        printk("va %lx %lx %lx %lx\n",pgd,pud,pmd,pte);

    if (pte==0)
    {
        setipl(savipl);
        return SS$_NORMAL;
    }

    if (*(unsigned long*)pte==0)
    {
        setipl(savipl);
        return SS$_NORMAL;
    }

    struct _mypte * mypte = pte;

    if (*(long *)pte&_PAGE_PRESENT)
        goto valid;
    if (mypte->pte$v_typ1)   // page or image file
    {
        if (mypte->pte$v_typ0)   // image file
        {
            *(unsigned long *)mypte=0;
        }
        else     // page file
        {
            mmg$dallocpagfil1(mypte->pte$v_pgflpag);
            // restore job page file quota
            // fix phd$l_ppgflva
            *(unsigned long *)mypte=0;
            // if last page remove null pages
            // the book also said something about it coming from a demand zero...
        }
    }
    if (mypte->pte$v_typ1==0)   //zero transition or global
    {
        if (mypte->pte$v_typ0==0)
        {
            if (mypte->pte$v_pfn)   //transition
            {
                if (mem_map[mypte->pte$v_pfn].pfn$v_loc==PFN$C_FREPAGLST)
                {
                    int ipl = vmslock(&SPIN_MMG, IPL$_MMG);
                    mmg$delpfnlst(PFN$C_FREPAGLST,mypte->pte$v_pfn);
                    vmsunlock(&SPIN_MMG, ipl);
                    // do something with pfn shrcnt
                    goto skipthis;
                }

                if (mem_map[mypte->pte$v_pfn].pfn$v_loc==PFN$C_MFYPAGLST)
                {
                    //also check if it has page file backing store
                    mem_map[mypte->pte$v_pfn].pfn$l_page_state&=~PFN$M_MODIFY;
                    int ipl = vmslock(&SPIN_MMG, IPL$_MMG);
                    mmg$delpfnlst(PFN$C_MFYPAGLST,mypte->pte$v_pfn);
                    vmsunlock(&SPIN_MMG, ipl);
                    goto skipthis;
                }

                // there also is a couple of more steps and checks

skipthis:
                {}
            }
            else     // zero page demand?
            {
                // do not do anything?
            }
        }
        else
        {
        }
    }
    goto out;

valid:
    {}
    struct _phd * phd = p->pcb$l_phd;
    struct _wsl * wsl = phd->phd$l_wslist;
    struct _pfn * pfn = &mem_map[mypte->pte$v_pfn];
    struct _wsl * wsle = &wsl[pfn->pfn$l_wslx_qw];
    if (wsle->wsl$v_pagtyp==WSL$C_PROCESS)
    {
        p->pcb$l_ppgcnt--;
    }
    wsle->wsl$pq_va=0;
    *(unsigned long*)pte=0;
    int ipl = vmslock(&SPIN_MMG, IPL$_MMG);
#ifdef OLDINT
    mmg$dallocpfn(pfn);
#else
    mmg$dallocpfn(pfn-mem_map);
#endif
    vmsunlock(&SPIN_MMG, ipl);

out:

    __flush_tlb(); //flush_tlb_range(current->mm, page, page + PAGE_SIZE);
    setipl(savipl);
    return SS$_NORMAL;
}

asmlinkage int exe$deltva(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode)
{
    // create and init stack scratch space

    setipl(IPL$_ASTDEL);

    void * first=inadr->va_range$ps_start_va;
    void * last=inadr->va_range$ps_end_va;
    struct _pcb * p=ctl$gl_pcb;
    struct _rde * rde;
    unsigned long numpages=(last-first)/PAGE_SIZE;
    acmode=0;
    mmg$credel(acmode, first, last, mmg$delpag, inadr, retadr, acmode, p, numpages);
}

/**
   \brief system service for expanding region with zero ptes - see 5.2 15.3.2
   note that this is not tested at all and probably does not work
   consider this as MISSING
   \param pagcnt page count
   \param retadr return address
   \param acmode access mode
   \param region which region
*/

asmlinkage int exe$expreg(unsigned int pagcnt, struct _va_range *retadr,unsigned int acmode, char region)
{
    int prot_pte=0x45|_PAGE_RW;
    int sts = SS$_NORMAL;
    // region is default 0, and region rde are not used like in vms

    // select corresponding rde...
    // here I have got to see or create a region starting at, like 0x20000000

    struct _rde * rde=mmg$lookup_rde_va(0x20000000, current->pcb$l_phd, LOOKUP_RDE_EXACT, IPL$_ASTDEL);
    if (rde==0)
    {
        int sts=exe$create_region_32(pagcnt*4096,prot_pte,0x187500,0,0,0,0x20000000);
        rde=mmg$lookup_rde_va(0x20000000, current->pcb$l_phd, LOOKUP_RDE_EXACT, IPL$_ASTDEL);
        rde->rde$pq_first_free_va=0x20000000;

        retadr->va_range$ps_start_va=0x20000000;
        retadr->va_range$ps_end_va=0x20000000+pagcnt*4096-1;

        goto out;
        return sts;
    }

out:
    rde->rde$pq_first_free_va+=pagcnt*4096;

    // if can create all at once:
    mmg$fast_create(ctl$gl_pcb,rde,retadr->va_range$ps_start_va,retadr->va_range$ps_end_va,pagcnt,prot_pte);
    // else when does this run?
    //mmg$credel(acmode, first, last, mmg$crepag, inadr, retadr, acmode, p, numpages);
    return sts;
}


struct _mmg mymmg;

int inline mmg$inadrini()
{
}

int inline mmg$retadrini()
{
}

int inline insrde(struct _rde * elem, struct _rde * head)
{
    struct _rde * tmp=head->rde$ps_va_list_flink;
    struct _rde * prev=head;
    while (tmp!=head && elem->rde$pq_start_va>tmp->rde$pq_start_va)
    {
        prev=tmp;
        tmp=tmp->rde$ps_va_list_flink;
    }
#if 0
    if (head==tmp)
    {
        if (elem->rde$pq_start_va<prev->rde$pq_start_va)
        {
            prev=head;
        }
        else
        {
            prev=prev->rde$ps_va_list_blink;
        }
    }
#endif
    insque(elem,prev); // ins at pred
    return 1;
}

int mmg$crepag (int acmode, void * va, struct _pcb * p, signed int pagedirection, struct _rde * rde, unsigned long newpte);

/**
   \brief create or delete page - see 5.2 15.1
   \param first address
   \param last address
   \param pagesubr page crepag or credel subroutine
   \param inadr not yet used
   \param retadr return address not yet used
   \param acmodeagain not yet used
   \param p pcb struct
   \param numpager number of pages, not yet used
*/

int mmg$credel(int acmode, void * first, void * last, void (* pagesubr)(), struct _va_range *inadr, struct _va_range *retadr, unsigned int acmodeagain, struct _pcb * p, int numpages)
{
    unsigned long tmp=first;
    /** initialize template pte */
    int newpte=0; // lots of zeros for demand page zero
    /** test if in system space, and return error */
    if (((long)first & 0x80000000) || ((long)last & 0x80000000))
        return SS$_NOPRIV;
    /** set pagesubr in scratch space */
    mymmg.mmg$l_pagesubr=pagesubr;
    /** set svstartva in scratch - MISSING */
    /** iterate until last */
    while (tmp<last)
    {
        /** invoke per-page pagesubr */
        pagesubr(0,tmp,p,+PAGE_SIZE,0,newpte);      // crepag or del
        tmp=tmp+PAGE_SIZE;
    }
    return SS$_NORMAL;
}

/**
   \brief system service for virtual address space creation - see 5.2 15.2 15.3.1
   \param inadr address range
   \param retadr returned address range
   \param acmode access mode
*/

asmlinkage int exe$cretva (struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode)
{
    /** create stack scratch space - MISSING */
    /** template pte - (seems to be in credel) */
    /** set ipl 2 - MISSING */
    /** test page ownership and access mode - MISSING */
    /** call credel */
    struct _rde * tmprde = mmg$lookup_rde_va (inadr->va_range$ps_start_va, current->pcb$l_phd, 0, 2);
    void * first=inadr->va_range$ps_start_va;
    void * last=inadr->va_range$ps_end_va;
    /** test address range in system space, and return NOPRIV - (seems to be in credel) */
    /** test overlapping addresses - MISSING */
    /** does only credel, no fast_create */
    struct _pcb * p=current;
    struct _rde * rde;
    unsigned long numpages=(last-first)/PAGE_SIZE;
    mmg$credel(acmode, first, last, mmg$crepag, inadr, retadr, acmode, p, numpages);
    /** return pagefile quota, peak pagefile usage - MISSING */
    /** set ipl 0 - MISSING */
}

/**
   \brief routine for single virtual address space creation - see 5.2 15.3.1
*/

int mmg$crepag (int acmode, void * va, struct _pcb * p, signed int pagedirection, struct _rde * rde, unsigned long newpte)
{

    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte = 0;
    struct mm_struct * mm=current->mm;
    unsigned long address=va;

    /** test if it is within its address space - MISSING */
    if (va>=rde->rde$pq_first_free_va)
    {

    }

    pgd = pgd_offset(mm, address);
    pud = pud_alloc(mm, pgd, address);
    pmd = pmd_alloc(mm, pud, address);
    if (pmd)
    {
#ifdef __i386__
        // there is some lock prob for 64 bit
        spin_lock(&mm->page_table_lock);
#endif
        pte = pte_alloc(mm, pmd, address);
        spin_unlock(&mm->page_table_lock);
    }

    /** check if we should set the new pte */
    *(unsigned long *)pte=newpte; // do this anyway
    /** test whether page already exists and return va_in_use if so */
    /** test if overmap allowed and do eventual delpag - MISSING */
    if ((long)(*(long *)pte))
    {
        return SS$_VA_IN_USE;
    }
    else
    {
        /** charge pagefile quota - MISSING */
        /** store requested value into the pte */
        *(unsigned long *)pte=newpte;
    }

}

// really belongs in mmg_functions.h
inline struct _rde * mmg$lookup_rde_va (void * va, struct _phd * const phd, int function, int ipl)
{
    struct _rde * head=&phd->phd$ps_p0_va_list_flink;
    struct _rde * rde, *prev, *next, *newrde;
    int savipl=getipl();
    if (ipl < IPL$_ASTDEL) setipl(IPL$_ASTDEL);

    newrde = mmg$search_rde_va(va, head, &prev, &next);

    if (newrde)
    {
        if (ipl < IPL$_ASTDEL) setipl(savipl);
        return newrde;
    }

    if (function==LOOKUP_RDE_EXACT)
    {
        if (ipl < IPL$_ASTDEL) setipl(savipl);
        return 0;
    }

    //else function is LOOKUP_RDE_HIGHER

    if (head->rde$v_descend)
    {
        if (ipl < IPL$_ASTDEL) setipl(savipl);
        return (prev);
    }

    if (va < head->rde$pq_start_va)
    {
        if (ipl < IPL$_ASTDEL) setipl(savipl);
        return (head);
    }

    if (next != head)
    {
        if (ipl < IPL$_ASTDEL) setipl(savipl);
        return (next);
    }

    head++;
    if (!head->rde$v_descend)
    {
        if (ipl < IPL$_ASTDEL) setipl(savipl);
        return (head);
    }

    rde = head->rde$ps_va_list_blink;
    if (ipl < IPL$_ASTDEL) setipl(savipl);
    return (newrde);
}

// belongs same place.
// is like find_vma?
inline struct _rde * mmg$search_rde_va (void * va, struct _rde *head, struct _rde **prev, struct _rde **next)
{
    struct _rde * tmp=head;
    int dofirst=0;
    *prev=0;
    *next=head->rde$ps_va_list_flink;

    while (head!=*next)
    {
        if (!head->rde$v_descend)
        {
            if ((tmp->rde$pq_start_va+tmp->rde$q_region_size) > va) goto out;
        }
        else
        {
            if (tmp->rde$pq_start_va <= va) goto out;
        }
        *prev=tmp;
        tmp=*next;
        *next=tmp->rde$ps_va_list_flink;
    }
out:
    if (!head->rde$v_descend)
    {
        if ((tmp->rde$pq_start_va+tmp->rde$q_region_size) > va) dofirst=1;
    }
    else
    {
        if (tmp->rde$pq_start_va <= va) dofirst=1;
    }
    if (dofirst)
    {
        if (tmp->rde$pq_start_va<=va) return tmp;
        *next=tmp;
        return 0;
    }
    else
    {
        *prev=tmp;
        return 0;
    }
}

int mmg$fast_create(struct _pcb * p, struct _rde *rde, void * start_va, void * end_va, unsigned long pages, unsigned long prot_pte)
{
    unsigned long newpte;
    unsigned long page;
    unsigned long tmp=start_va;
    unsigned long new_pte;

    for(page=0; page<pages; page++)
    {
        newpte=prot_pte|0;//tmp;
        mmg$crepag(0,tmp,0,+PAGE_SIZE,0,newpte);
        tmp+=PAGE_SIZE;
    }
}

int mmg$fast_create_gptx(struct _pcb * p, struct _rde *rde, void * start_va, void * end_va, unsigned long pages, unsigned long prot_pte)
{
    unsigned long page;
    unsigned long tmp=start_va;
    struct _mypte newpte;

    newpte.pte$l_all=prot_pte;

    for(page=0; page<pages; page++)
    {
        mmg$crepag(0,tmp,0,+PAGE_SIZE,0,*(unsigned long *)&newpte);
        tmp+=PAGE_SIZE;
        newpte.pte$v_gptx++;
    }
}

asmlinkage int exe$create_region_32_wrap  (struct struct_args * s)
{
    return exe$create_region_32(s->s1,s->s2,s->s3,s->s4,s->s5,s->s6,s->s7);
}

asmlinkage int exe$create_region_32  ( unsigned long length, unsigned int region_prot, unsigned int flags, unsigned long long *return_region_id, void **return_va, unsigned long *return_length, unsigned long start_va)
{
    struct _rde * rde;
    rde=kmalloc(sizeof(struct _rde),GFP_KERNEL);
    memset(rde,0,sizeof(struct _rde));
    rde->rde$b_type=30; // fix later
    rde->rde$pq_start_va=start_va;
    rde->rde$q_region_size=length;
    rde->rde$l_flags=flags;
    rde->rde$r_regprot.regprt$l_region_prot = region_prot;
    insrde(rde,&current->pcb$l_phd->phd$ps_p0_va_list_flink);
    return SS$_NORMAL;
}

asmlinkage int exe$delete_region_32  (unsigned long long *region_id,  unsigned int acmode, void **return_va, unsigned long *return_length)
{

}

asmlinkage int exe$get_region_info  (unsigned int function_code, unsigned long long *region_id, void *start_va, void *reserved, unsigned int buffer_length, void *buffer_address, unsigned int *return_length)
{

}

