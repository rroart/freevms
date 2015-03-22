// $Id$
// $Locker$

// Author. Roar Thron�s.
/**
   \file shell.c
   \brief for pagetable and user space - TODO still more doc
   \author Roar Thron�s
 */

#include <linux/linkage.h>
#include <linux/sched.h>
#include <linux/completion.h>
#include <linux/personality.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/bootmem.h>
#include <linux/mman.h>
#include <linux/slab.h>

#include <descrip.h>
#include <ipldef.h>
#include <pcbdef.h>
#include <phddef.h>
#include <pqbdef.h>
#include <prcdef.h>
#include <pridef.h>
#include <rdedef.h>
#include <secdef.h>
#include <ssdef.h>
#include <starlet.h>
#include <system_data_cells.h>

#include <asm/pgalloc.h>
#include <asm/mmu_context.h>
#include <misc_routines.h>
#include <mmg_routines.h>

#include <internals.h>

long phys_pte_addr(struct _pcb * pcb)
{
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte = 0;

    long page=0x7ffdf000;

    struct mm_struct * mm=pcb->mm;

    pgd = pgd_offset(mm, page);

    pud = pgd;
    pmd = pud;

    if (pmd)
    {
        pte = pte_offset(pmd, page);
    }

    return (*(unsigned long*)pte)&0xfffff000;

}

void init_p1pp_long(unsigned long addr, signed long offset, signed long val)
{
    signed long * valp=addr+offset;
    *valp=val;
}

void init_p1pp_long_long(unsigned long addr, signed long offset, signed long long val)
{
    signed long long * valp=addr+offset;
    *valp=val;
}

/**
   \brief initialization of p1 data
   \param pcb process pcb
   \param phd process phd
   \param offset an offset
*/

void init_p1pp_data(struct _pcb * pcb, struct _phd * phd,signed long offset)
{
    init_p1pp_long(&ctl$gl_pcb,offset,pcb);
    init_p1pp_long(&ctl$gl_phd,offset,phd);
    init_p1pp_long(&ctl$gl_ccbbase,offset,&ctl$ga_ccb_table);
    init_p1pp_long(&ctl$gl_chindx,offset,CHANNELCNT);
}

// do this after init_fork_p1pp

/**
   \brief initialization of p1 space when not(?) forking
   \param pcb new process pcb
   \param phd new process phd
   \param pcb old process pcb
   \param phd old process phd
*/

int shell_init_other(struct _pcb * pcb, struct _pcb * oldpcb, long addr, long oldaddr)
{
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte = 0;
    pgd_t *oldpgd;
    pud_t *oldpud;
    pmd_t *oldpmd;
    pte_t *oldpte = 0;

    long page=addr;
    long oldpage=oldaddr;

    struct mm_struct * mm=pcb->mm;
    struct mm_struct * oldmm=oldpcb->mm;

    if (oldmm==0)
        oldmm=&init_mm;

    spin_lock(&mm->page_table_lock);
    pgd = pgd_offset(mm, page);
#ifdef __i386__
    oldpgd = pgd_offset(oldmm, oldpage);
#else
#if 0
    if (oldmm->pgd)
        oldpgd = pgd_offset(oldmm, oldpage);
    else
        oldpgd = pgd_offset_k(oldpage);
#else
    oldpgd = pgd_offset(oldmm, oldpage);
#endif
#endif
    //printk(KERN_EMERG "mm %lx %lx %lx %lx\n",mm,mm->pgd,oldmm,oldmm->pgd);
    //printk(KERN_EMERG "pgd %lx %lx\n",pgd,oldpgd);

    pud = pud_alloc(mm, pgd, page);
#ifdef __i386__
    oldpud = pud_alloc(oldmm, oldpgd, oldpage);
#else
    oldpud = pud_alloc(oldmm, oldpgd, oldpage);
#endif

    pmd = pmd_alloc(mm, pud, page);
#ifdef __i386__
    oldpmd = pmd_alloc(oldmm, oldpud, oldpage);
#else
    oldpmd = pmd_alloc(oldmm, oldpud, oldpage);
#endif
    //printk(KERN_EMERG "pmd %lx %lx\n",pmd,oldpmd);

    if (pmd)
    {
        pte = pte_alloc(mm, pmd, page);
#if 0
        if (pte)
            *(long*)pte=0;
#endif
    }
    if (oldpmd)
    {
        oldpte = pte_alloc(oldmm, oldpmd, oldpage);
#if 0
        if (oldpte)
            *(long*)oldpte=0;
#endif
    }
    spin_unlock(&mm->page_table_lock);

    int ipl = vmslock(&SPIN_MMG, IPL$_MMG);
    int pfn=mmg$ininewpfn(pcb,pcb->pcb$l_phd,page,pte);
    vmsunlock(&SPIN_MMG, ipl);
    mem_map[pfn].pfn$q_bak=*(unsigned long *)pte;
#define _PAGE_NEWPAGE 0
    *(unsigned long *)pte=((unsigned long)/*__va*/(pfn*PAGE_SIZE))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED|_PAGE_DIRTY;
    *(unsigned long *)oldpte=((unsigned long)/*__va*/(pfn*PAGE_SIZE))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED|_PAGE_DIRTY;
    flush_tlb_range(pcb->mm, page, page + PAGE_SIZE);
    flush_tlb_range(oldpcb->mm, oldpage, oldpage + PAGE_SIZE);
    memset(oldpage,0,PAGE_SIZE); // must zero content also
}

void init_sys_p1pp()
{
    extern struct _phd system_phd;
    struct _pcb * pcb = &init_task;
    pcb->pcb$l_phd=&system_phd;
    long phd=pcb->pcb$l_phd;

    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    pte_t *pte2;
    pte_t *pte3;

    long page=0x7ffff000;
    long x2p=0x7fff0000;
    long e2p=0x7fff1000;

    struct mm_struct * mm=&init_mm;

    long pt=alloc_bootmem_pages(4096);
    long pa=alloc_bootmem_pages(4096);
    long x2=alloc_bootmem_pages(4096);
    long e2=alloc_bootmem_pages(4096);

#ifdef __x86_64__
    long pm=alloc_bootmem_pages(4096);
#if 0
    long pu=alloc_bootmem_pages(4096); // already allocated
#else
    //  long pu = pud_offset(((pgd_t*)/*__pa*/(pgd)), page);
#endif
#endif

    kernel_puts("alloced pt pa\n");

    memset(pt,0,4096);
    memset(pa,0,4096);
    memset(x2,0,4096);
    memset(e2,0,4096);
#ifdef __x86_64__
    memset(pm,0,4096);
#endif

    kernel_puts("memset them\n");

#ifdef __i386__
    pgd = pgd_offset(mm, page);
#else
    pgd = pgd_offset_k(page);
#endif

#ifdef __i386__
    *(unsigned long *)pgd=__pa(pt)|_PAGE_PRESENT|_PAGE_RW;
#else
#if 0
    *(unsigned long *)pgd=__pa(pu)|_PAGE_PRESENT|_PAGE_RW; // already there
#endif
#endif

#ifdef __i386__
    kernel_puts("set pgd\n");
#endif

#ifdef __i386__
    pud = pgd;
    pmd = pud; //pmd_offset(pgd, page);
#else
    pud = pud_offset(((pgd_t*)/*__pa*/(pgd)), page);
    *(unsigned long *)pud=__pa(pm)|_PAGE_PRESENT|_PAGE_RW;
    pmd = pmd_offset(((pud_t*)/*__pa*/(pud)), page);
    *(unsigned long *)pmd=__pa(pt)|_PAGE_PRESENT|_PAGE_RW;
#endif

#ifdef __i386__
    pte = pte_offset(((pmd_t*)__pa(pmd)), page);
    pte2 = pte_offset(((pmd_t*)__pa(pmd)), x2p);
    pte3 = pte_offset(((pmd_t*)__pa(pmd)), e2p);
#else
    pte = pte_offset(((pmd_t*)/*__pa*/(pmd)), page);
    pte2 = pte_offset(((pmd_t*)/*__pa*/(pmd)), x2p);
    pte3 = pte_offset(((pmd_t*)/*__pa*/(pmd)), e2p);
#endif

    char c[40];
    sprintf(c,"%lx %lx %lx %lx %lx\n",swapper_pg_dir,pgd,pud,pmd,pte);

    kernel_puts("pte\n");
    kernel_puts(c);

    long pfn=__pa(pa)>>12;
    long pfn2=__pa(x2)>>12;
    long pfn3=__pa(e2)>>12;

#define _PAGE_NEWPAGE 0
    *(unsigned long *)pte=((unsigned long)/*__va*/(pfn*PAGE_SIZE))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED|_PAGE_DIRTY;
    *(unsigned long *)pte2=((unsigned long)/*__va*/(pfn2*PAGE_SIZE))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED|_PAGE_DIRTY;
    *(unsigned long *)pte3=((unsigned long)/*__va*/(pfn3*PAGE_SIZE))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED|_PAGE_DIRTY;
    kernel_puts("pte content\n");
    ctl$gl_pcb=&init_task_union;
    kernel_puts("ctl$gl_pcb set\n");
    flush_tlb_range(pcb->mm, page, page + PAGE_SIZE);
    flush_tlb_range(pcb->mm, x2p, x2p + PAGE_SIZE);
    flush_tlb_range(pcb->mm, e2p, e2p + PAGE_SIZE);
    kernel_puts("flush\n");
    ctl$gl_chindx=42;
    kernel_puts("42\n");
    if ( (*(unsigned long*)0x7ffff000)==42)
        kernel_puts("still 42\n");
    memset(page,0,PAGE_SIZE); // must zero content also
    memset(x2p,0,PAGE_SIZE); // must zero content also
    memset(e2p,0,PAGE_SIZE); // must zero content also
    kernel_puts("memset page\n");
    if ( (*(unsigned long*)0x7ffff000)==0)
        kernel_puts("not 42\n");

    init_p1pp_data(pcb,phd,0);
    ctl$gq_procpriv=ctl$gl_pcb->pcb$l_priv;
}

void init_p1pp(struct _pcb * pcb, struct _phd * phd)
{
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte = 0;

    long page=0x7ffff000;

    struct mm_struct * mm=pcb->mm;

    if (mm==0)
        mm=&init_mm;

    spin_lock(&mm->page_table_lock);
    pgd = pgd_offset(mm, page);

    pud = pud_alloc(mm, pgd, page);

    pmd = pmd_alloc(mm, pud, page);

    if (pmd)
    {
        pte = pte_alloc(mm, pmd, page);
#if 0
        // check. why was this done?
        if (pte)
            *(long*)pte=0;
#endif
    }
    spin_unlock(&mm->page_table_lock);

    int ipl = vmslock(&SPIN_MMG, IPL$_MMG);
    int pfn=mmg$ininewpfn(pcb,phd,page,pte);
    vmsunlock(&SPIN_MMG, ipl);
    mem_map[pfn].pfn$q_bak=*(unsigned long *)pte;
#define _PAGE_NEWPAGE 0
    *(unsigned long *)pte=((unsigned long)/*__va*/(pfn*PAGE_SIZE))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED|_PAGE_DIRTY;
    ctl$gl_pcb=pcb;
    ctl$gl_chindx=42;
    //printk("pte content\n");
    flush_tlb_range(pcb->mm, page, page + PAGE_SIZE);
    //printk("flush\n");
    ctl$gl_chindx=42;
    //printk("42\n");
#if 0
    if ( (*(unsigned long*)0x7ffff000)==42)
        printk("still 42\n");
#endif
#if 0
    //printk("ms %lx %lx %lx %lx %lx %lx\n",page,0,(long)&ctl$gl_pcb-page,(long)&ctl$gl_pcb+4,0,PAGE_SIZE+4-((long)&ctl$gl_pcb-page));
    printk("%lx %lx %lx %lx %lx\n",pcb,pgd,pud,pmd,pte);
    void release_console_sem(void);
    release_console_sem();
#endif
#ifdef __i386__
    memset(page,0,(long)&ctl$gl_pcb-page); // must zero content also, but not ctl$gl_pcb
    memset((long)&ctl$gl_pcb+4,0,PAGE_SIZE+4-((long)&ctl$gl_pcb-page)); // must zero content also
#else
    memset((long)&ctl$gl_pcb+8,0,PAGE_SIZE-8); // must zero content also
#endif
    ctl$gl_pcb=pcb;
#if 0
    printk("memset page\n");
    if ( (*(unsigned long*)0x7ffff000)==0)
        printk("not 42\n");
#endif
#if 0
    do_mmap_pgoff(0,page,4096,PROT_READ | PROT_WRITE,MAP_FIXED | MAP_PRIVATE,0);
#if 0
    long vma = find_vma(pcb->mm, page);
    handle_mm_fault(pcb->mm, vma, page, 1);
#endif
#endif

    init_p1pp_data(pcb,phd,0);
    ctl$gq_procpriv=ctl$gl_pcb->pcb$l_priv;
}

/**
   \brief initialization of p1 space when forking
   \param pcb new process pcb
   \param phd new process phd
   \param pcb old process pcb
   \param phd old process phd
*/

int init_fork_p1pp(struct _pcb * pcb, struct _phd * phd, struct _pcb * oldpcb, struct _phd * oldphd)
{
    int uml_map=0;

    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte = 0;
    pgd_t *oldpgd;
    pud_t *oldpud;
    pmd_t *oldpmd;
    pte_t *oldpte = 0;

    long oldpage=0x7fffe000;
    long page=0x7ffff000;

    struct mm_struct * mm=pcb->mm;
    struct mm_struct * oldmm=oldpcb->mm;

    //printk("%x %x %x %x ",page,pcb,phd,mm);
    //printk("%x %x %x %x\n",oldpage,oldpcb,oldphd,oldmm);

    if (oldmm==0)
        oldmm=&init_mm;

    if (mm==0)
    {
        int retval=0;
        struct _pcb * tsk = pcb;
        mm=&init_mm;
#define allocate_mm()   (kmem_cache_alloc(mm_cachep, SLAB_KERNEL))
        mm = allocate_mm();
        if (!mm)
            panic("goto fail_nomem\n");// was: goto fail_nomem;

        /* Copy the current MM stuff.. */
        memcpy(mm, oldmm, sizeof(*mm));
        if (!mm_init(mm))
            panic("goto fail_nomem\n");// was: goto fail_nomem;

        down_write(&oldmm->mmap_sem);
        // not needed? retval = dup_mmap(mm);
        retval = dup_stuff(mm,tsk->pcb$l_phd);
        up_write(&oldmm->mmap_sem);

        if (retval)
            goto free_pt;

        /*
         * child gets a private LDT (if there was an LDT in the parent)
         */
        copy_segments(tsk, mm);

        if (init_new_context(tsk,mm))
            goto free_pt;

good_mm:
        tsk->mm = mm;
        tsk->active_mm = mm;
        goto out;

free_pt:
        mmput(mm);
        return retval;
    }

out:

    spin_lock(&mm->page_table_lock);
    pgd = pgd_offset(mm, page);
#ifdef __i386__
    oldpgd = pgd_offset(oldmm, oldpage);
#else
#if 0
    if (oldmm->pgd)
        oldpgd = pgd_offset(oldmm, oldpage);
    else
        oldpgd = pgd_offset_k(oldpage);
#else
    oldpgd = pgd_offset(oldmm, oldpage);
#endif
#endif
    //printk(KERN_EMERG "mm %lx %lx %lx %lx\n",mm,mm->pgd,oldmm,oldmm->pgd);
    //printk(KERN_EMERG "pgd %lx %lx\n",pgd,oldpgd);

    pud = pud_alloc(mm, pgd, page);
#ifdef __i386__
    oldpud = pud_alloc(oldmm, oldpgd, oldpage);
#else
    oldpud = pud_alloc(oldmm, oldpgd, oldpage);
#endif
    //printk(KERN_EMERG "pmd %lx %lx\n",pmd,oldpmd);

    pmd = pmd_alloc(mm, pud, page);
#ifdef __i386__
    oldpmd = pmd_alloc(oldmm, oldpud, oldpage);
#else
    oldpmd = pmd_alloc(oldmm, oldpud, oldpage);
#endif
    //printk(KERN_EMERG "pmd %lx %lx\n",pmd,oldpmd);

    if (pmd)
    {
        pte = pte_alloc(mm, pmd, page);
#if 0
        if (pte)
            *(long*)pte=0;
#endif
    }
    if (oldpmd)
    {
        oldpte = pte_alloc(oldmm, oldpmd, oldpage);
#if 0
        if (oldpte)
            *(long*)oldpte=0;
#endif
    }
    spin_unlock(&mm->page_table_lock);

    //printk("%x %x %x %x\n",swapper_pg_dir,pgd,pmd,pte);
    //printk("%x %x %x %x\n",swapper_pg_dir,oldpgd,oldpmd,oldpte);

    int ipl = vmslock(&SPIN_MMG, IPL$_MMG);
    int pfn=mmg$ininewpfn(pcb,phd,page,pte);
    vmsunlock(&SPIN_MMG, ipl);
    mem_map[pfn].pfn$q_bak=*(unsigned long *)pte;
#define _PAGE_NEWPAGE 0
    *(unsigned long *)pte=((unsigned long)/*__va*/(pfn*PAGE_SIZE))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED|_PAGE_DIRTY;
    *(unsigned long *)oldpte=((unsigned long)/*__va*/(pfn*PAGE_SIZE))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED|_PAGE_DIRTY;
    //printk("pte content\n");
    flush_tlb_range(pcb->mm, page, page + PAGE_SIZE);
    flush_tlb_range(oldpcb->mm, oldpage, oldpage + PAGE_SIZE);
    //printk("flush\n");
    init_p1pp_long(&ctl$gl_chindx,-4096,42);
#if 0
    printk("42\n");
    if ( (*(unsigned long*)0x7fffe000)==42)
        printk("still 42\n");
#endif
#if 0
    printk("%lx %lx %lx %lx %lx\n",oldpcb,oldpgd,oldpud,oldpmd,oldpte);
    printk("%lx %lx %lx %lx %lx\n",pcb,pgd,pud,pmd,pte);
    void release_console_sem(void);
    release_console_sem();
#endif
    memset(oldpage,0,PAGE_SIZE); // must zero content also
#if 0
    printk("memset page\n");
    if ( (*(unsigned long*)0x7fffe000)==0)
        printk("not 42\n");
#endif

    init_p1pp_data(pcb,phd,-4096);
    init_p1pp_long_long(&ctl$gq_procpriv,-4096,ctl$gq_procpriv);
    init_p1pp_long(&ctl$gl_lnmhash,-4096,ctl$gl_lnmhash);
    init_p1pp_long(&ctl$gl_lnmdirect,-4096,ctl$gl_lnmdirect);
    //printk("init p1pp data\n");
    return uml_map;
}

/**
   \brief make address readable from user space
   \param addr address
*/

int user_spaceable_addr(void * addr)
{
#ifdef __i386__
    struct _pcb * pcb = ctl$gl_pcb;
    pgd_t *pgd;
    pmd_t *pmd;
    pte_t *pte = 0;
    long page=addr;

    struct mm_struct * mm=pcb->mm;

    spin_lock(&mm->page_table_lock);
    pgd = pgd_offset(mm, page);
    pmd = pmd_offset(pgd, page);
    if (pmd)
    {
        pte = pte_offset(pmd, page);
    }
    spin_unlock(&mm->page_table_lock);
    if (pte)
    {
        long * ptep = pte;
        *ptep|=_PAGE_USER;
        ptep=pmd;
        *ptep|=_PAGE_USER;
        flush_tlb_range(pcb->mm, page, page + PAGE_SIZE);
    }
#else
    struct _pcb * pcb = ctl$gl_pcb;
    pgd_t *pgd;
    pmd_t *pmd;
    pte_t *pte = 0;
    long page=((long)addr) & ~0xfff;

    struct mm_struct * mm=pcb->mm;

    spin_lock(&mm->page_table_lock);
    pgd = pgd_offset(mm, page);
    pmd = pmd_alloc(mm, pgd, page);
    if (pmd)
    {
        pte = pte_alloc(mm, pmd, page);
    }
    spin_unlock(&mm->page_table_lock);
    if (pte)
    {
        long * ptep = pte;
        *ptep|=_PAGE_USER;
        ptep=pmd;
        *ptep|=_PAGE_USER;
        ptep=pgd;
        *ptep|=_PAGE_USER;
        flush_tlb_range(pcb->mm, page, page + PAGE_SIZE);
    }
#endif
}

/**
   \brief make systime readable from user space
*/

int user_spaceable()
{
#if 0
    extern int exe$astdel();
    user_spaceable_addr(exe$astdel);
#endif
#ifdef __i386__
    user_spaceable_addr(&exe$gq_systime);
#endif
#if 0
    extern struct desc_struct gdt_table[];
    long long * l=&gdt_table[2];
    *l|=0x0000600000000000;
#endif
}

#ifdef __i386__
#define P1PP __attribute__ ((section ("p1pp")))

unsigned short ctl$gw_nmioch P1PP ;
short short_aligner;
unsigned long ctl$gl_chindx P1PP = 4;
unsigned long ctl$gl_lnmhash P1PP ;
unsigned long ctl$gl_lnmdirect P1PP ;
unsigned long ctl$al_stack[4] P1PP ;
unsigned long long ctl$gq_lnmtbl_cache P1PP ;
unsigned long ctl$gl_cmsupr P1PP ;
unsigned long ctl$gl_cmuser P1PP ;
unsigned long ctl$gl_cmhandler P1PP ;
unsigned long ctl$aq_excvec[8] P1PP ;
unsigned long ctl$gl_thexec P1PP = 0;
unsigned long ctl$gl_thsupr P1PP = 0;
unsigned long long ctl$gq_common P1PP ;
unsigned long ctl$gl_getmsg P1PP ;
unsigned long ctl$al_stacklim[4] P1PP ;
unsigned long ctl$gl_ctlbasva P1PP ;
unsigned long ctl$gl_imghdrbf P1PP ;
unsigned long ctl$gl_imglstptr P1PP ;
unsigned long ctl$gl_phd P1PP ;
unsigned long ctl$gq_allocreg[2] P1PP ;
unsigned long long ctl$gq_mountlst P1PP ;
unsigned char ctl$t_username[12] P1PP ;
unsigned char ctl$t_account[8] P1PP ;
unsigned long long ctl$gq_login P1PP ;
unsigned long ctl$gl_finalsts P1PP ;
unsigned long ctl$gl_wspeak P1PP ;
unsigned long ctl$gl_virtpeak P1PP ;
unsigned long ctl$gl_volumes P1PP ;
unsigned long long ctl$gq_istart P1PP ;
unsigned long ctl$gl_icputim P1PP ;
unsigned long long ctl$gq_ifaults P1PP ;
unsigned long ctl$gl_ifaultio P1PP ;
unsigned long ctl$gl_iwspeak P1PP ;
unsigned long ctl$gl_ipagefl P1PP ;
unsigned long ctl$gl_idiocnt P1PP ;
unsigned long ctl$gl_ibiocnt P1PP ;
unsigned long ctl$gl_ivolumes P1PP ;
unsigned char ctl$t_nodeaddr[7] P1PP ;
unsigned char ctl$t_nonename[7] P1PP ;
unsigned char ctl$t_remoteid[17] P1PP ;
unsigned char spare_for_alignment1 P1PP ;
unsigned long long ctl$gq_procpriv P1PP ;
unsigned long ctl$gl_usrchmk P1PP ;
unsigned long ctl$gl_usrchme P1PP ;
unsigned long ctl$gl_powerast P1PP ;
unsigned char ctl$gb_pwrmode P1PP ;
unsigned char ctl$gb_ssfilter P1PP ;
unsigned char ctl$gb_reenable_asts P1PP ;
unsigned char spare_for_alignment2 P1PP ;
unsigned long ctl$al_finalexc[4] P1PP ;
struct _ccb * /*unsigned long*/ ctl$gl_ccbbase P1PP ;
unsigned long long ctl$gq_dbgarea P1PP ;
unsigned long ctl$gl_rmsbase P1PP ;
unsigned long ctl$gl_ppmsg[2] P1PP ;
unsigned char ctl$gb_msgmask P1PP ;
unsigned char ctl$gb_deflang P1PP ;
unsigned short ctl$gw_ppmsgchn P1PP ;
unsigned long ctl$gl_usrundwn P1PP ;
struct _pcb * ctl$gl_pcb P1PP ;
unsigned long ctl$gl_ruf P1PP ;
unsigned long ctl$gl_sitespec P1PP ;
unsigned long ctl$gl_knownfil P1PP ;
unsigned long ctl$al_ipastvec[8] P1PP ;
unsigned long ctl$gl_cmcntx P1PP ;
unsigned long ctl$gl_aiflnkptr P1PP ;
unsigned long ctl$gl_f11bxqp P1PP ;
unsigned long long ctl$gq_p0alloc P1PP ;
unsigned long ctl$gl_prcallcnt P1PP ;
unsigned long ctl$gl_rdiptr P1PP ;
unsigned long ctl$gl_lnmdirseq P1PP ;
unsigned long long ctl$gq_helpflags P1PP ;
unsigned long long ctl$gq_termchar P1PP ;
unsigned long ctl$gl_krpfl P1PP ;
unsigned long ctl$gl_krpbl P1PP ;
unsigned long ctl$gl_creprc_flags P1PP ;
unsigned long ctl$gl_thcount[3] P1PP ;
unsigned long long ctl$gq_cwps_q1 P1PP ;
unsigned long long ctl$gq_cwps_q2 P1PP ;
unsigned long ctl$gl_cwps_l1 P1PP ;
unsigned long ctl$gl_cwps_l2 P1PP ;
unsigned long ctl$gl_cwps_l3 P1PP ;
unsigned long ctl$gl_cwps_l4 P1PP ;
unsigned long ctl$gl_prcprm_kdata2 P1PP ;
unsigned long ctl$gl_usrundwn_exec P1PP ;
unsigned long ctl$ag_clidata P1PP ;
unsigned long ctl$gl_fixuplnk P1PP ;
unsigned long ctl$gl_iaflnkptr P1PP ;
#endif

/**
   \brief syscall code to be used in the future?
*/

#ifdef __x86_64__
int __attribute__ ((section (".vsyscall_5"))) exe$syscall()
{
    __asm__ __volatile__(
        "pushq %r10\n\t"
        "movq %r10,%rcx\n\t"
        "int $0x85\n\t"
        "popq %r10\n\t"
        "ret\n\t"
    );
}
#endif
