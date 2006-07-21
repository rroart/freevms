// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source, 2001-2004. Parts from fault.c and trap_kern.c

#include <linux/config.h>
#include <linux/slab.h>
#include <linux/shm.h>
#include <linux/mman.h>
#include <linux/pagemap.h>
#include <linux/swap.h>
#include <linux/swapctl.h>
#include <linux/smp_lock.h>
#include <linux/init.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/personality.h>

#include <asm/uaccess.h>
#include <asm/pgalloc.h>

#include<linux/unistd.h>
#include<linux/linkage.h>
#include<linux/sched.h>
#include <system_data_cells.h>
#include <linux/mm.h>
#include <acbdef.h>
#include <fcbdef.h>
#include <ipl.h>
#include <ipldef.h>
#include <mmgdef.h>
#include <pfndef.h>
#include <phddef.h>
#include <rdedef.h>
#include <secdef.h>
#include <ssdef.h>
#include <vmspte.h>
#include <wcbdef.h>
#include <wsldef.h>
#include <va_rangedef.h>
#include <pfldef.h>
#include <mmg_functions.h>

#include <linux/vmalloc.h>

#ifdef __x86_64__
#include <asm/hardirq.h>
#endif

#include <misc_routines.h>
#include <mmg_routines.h>
#include <sch_routines.h>
#include <internals.h>

int do_wp_page(struct mm_struct *mm, struct _rde * vma, unsigned long address, pte_t *page_table, pte_t pte);

#ifdef __x86_64__
#include <asm/kdebug.h>
#else
extern void die(const char *,struct pt_regs *,long);
asmlinkage void do_invalid_op(struct pt_regs *, unsigned long);
#endif

#undef OLDINT
#define OLDINT

#ifdef __x86_64__
#undef OLDINT
#endif

unsigned long findpte_new(struct mm_struct *mm, unsigned long address);

#ifdef __arch_um__
#include "user_util.h"
//#define yield() do { SOFTINT_RESCHED_VECTOR; } while(0)
#define yield() do { sch$resched(); myrei(); } while(0)
#endif

//mmg$pagefault()
#ifdef CONFIG_MM_VMS

signed int mmg$ininewpfn(struct _pcb * p, struct _phd * phd, void * va, struct _mypte * pte) {
  signed long pfn=mmg$allocpfn();
  struct _pfn * page;
  if (pfn&0x80000000) return pfn;
  if ((((int)va)&WSL$M_PAGTYP)>=WSL$C_GLOBAL) {
    phd=mmg$gl_sysphd;
    pte=&((struct _mypte *)mmg$gq_gpt_base)[pte->pte$v_gptx];
    // not implemented yet
  }
  if ((((unsigned long)va)&0x80000000) == 0) {
    mmg$incptref(p->pcb$l_phd,pte);
  }
  // wrong page=&((struct _pfn *)pfn$al_head[PFN$C_FREPAGLST])[pfn];
  // also set page type
  mem_map[pfn].pfn$v_pagtyp=((unsigned long)va)&PFN$M_PAGTYP;
  //  mem_map[pfn].virtual=__va(pfn*PAGE_SIZE); // not necessary
  //mem_map[pfn].count.counter=1;
  mem_map[pfn].pfn$l_pt_pfn=0;
  mem_map[pfn].pfn$q_pte_index=0;
  mem_map[pfn].pfn$q_pte_index=pte; // hope it's the right one?

  page=&mem_map[pfn];
  //set_page_count(page, 1);
  mem_map[pfn].pfn$l_refcnt=1; // aah bug

  mmg$makewsle(p,p->pcb$l_phd,va,pte,pfn);
  return pfn;
}

int mmg$incptref(struct _phd * phd, struct _mypte * pte) {
#ifdef __arch_um__
  signed long pfn=__pa(pte->pte$v_pfn << PAGE_SHIFT) >> PAGE_SHIFT ;
#else
  signed long pfn=pte->pte$v_pfn;
#endif
  if (mem_map[pfn].pfn$v_pagtyp==PFN$C_GLOBAL || mem_map[pfn].pfn$v_pagtyp==PFN$C_GBLWRT) {
    mem_map[pfn].pfn$l_shrcnt++;
    if (mem_map[pfn].pfn$l_shrcnt==1) {
      // locate wsle and set its wsl$v_wslock
    }
  }
  mem_map[pfn].pfn$l_refcnt++;
}

int mmg$decptref(struct _phd * phd, struct _mypte * pte) {
  signed long pfn=pte->pte$v_pfn;
  if (mem_map[pfn].pfn$v_pagtyp==PFN$C_GLOBAL || mem_map[pfn].pfn$v_pagtyp==PFN$C_GBLWRT) {
    mem_map[pfn].pfn$l_shrcnt--;
    if (mem_map[pfn].pfn$l_shrcnt==0) {
      // locate wsle and clear its wsl$v_wslock
    }
  }
  mem_map[pfn].pfn$l_refcnt--;
}

int mmg$makewsle(struct _pcb * p, struct _phd * phd, void * va, void * pte, signed int pfn) {
  int new=p->pcb$l_phd->phd$l_wsnext++; // maybe not ++ here?
  struct _wsl * wsl = p->pcb$l_phd->phd$l_wslist;
  struct _wsl * wsle = &wsl[new];
  struct _pfn * page;
  if (wsle->wsl$v_valid) panic("should be invalid\n");
  wsle->wsl$v_valid=1;
  wsle->wsl$v_pagtyp=mem_map[pfn].pfn$v_pagtyp;
  wsle->wsl$pq_va=((unsigned long)wsle->wsl$pq_va)|(unsigned long)va;
  // p->pcb$l_phd->phd$l_ptwsleval++
  //  page=&((struct _pfn *)pfn$al_head[PFN$C_FREPAGLST])[pfn];
  page=&mem_map[pfn];
  page->pfn$l_wslx_qw=new; // hope it's the right one?

  if ((((unsigned long)wsle->wsl$pq_va)&WSL$M_PAGTYP)==WSL$C_GLOBAL)
    p->pcb$l_gpgcnt++;
  if ((((unsigned long)wsle->wsl$pq_va)&WSL$M_PAGTYP)==WSL$C_PROCESS)
    p->pcb$l_ppgcnt++;

}

/* vma is the first one with  address < vma->vm_end,
 * and even  address < vma->vm_start. Have to extend vma. */
static inline int expand_stack2(struct _rde * vma, unsigned long address)
{
	unsigned long grow;

	/*
	 * vma->vm_start/vm_end cannot change under us because the caller is required
	 * to hold the mmap_sem in write mode. We need to get the spinlock only
	 * before relocating the vma range ourself.
	 */
	address &= PAGE_MASK;
 	//spin_lock(&vma->vm_mm->page_table_lock);
	grow = ((int)(vma->rde$pq_start_va - address)) >> PAGE_SHIFT;
	if (vma->rde$pq_start_va + vma->rde$q_region_size - address > current->rlim[RLIMIT_STACK].rlim_cur || 1 ) {
	    // ((vma->vm_mm->total_vm + grow) << PAGE_SHIFT) > current->rlim[RLIMIT_AS].rlim_cur) { // check exact bounds later?
	  //spin_unlock(&vma->vm_mm->page_table_lock);
		return -ENOMEM;
	}
	vma->rde$pq_start_va = address;
	//vma->vm_pgoff -= grow;
	current->active_mm->total_vm += grow;
	//if (vma->vm_flags & VM_LOCKED)
	//	vma->vm_mm->locked_vm += grow;
	//spin_unlock(&vma->vm_mm->page_table_lock);
	return 0;
}

struct pfast {
  struct _wcb * window;
  unsigned long offset;
  unsigned long pfn;
  unsigned long address;
  pte_t * pte;
  unsigned long pteentry;
  struct _rde * rde;
};

void pagefaultast(struct pfast * p) {
  int res;
  if (p->window->wcb$l_fcb->fcb$l_fill_5)
  block_read_full_page3(p->window->wcb$l_fcb, &mem_map[p->pfn], p->offset);
  else
  ods2_block_read_full_page3(p->window, &mem_map[p->pfn], p->offset);
  *(unsigned long *)(p->pte)&=0xfffff000;
  *(unsigned long *)(p->pte)|=p->pteentry;

#if 0
  // if 0ing: no shortcut, want full compatibility because mm is vulnerable
  if ((p->rde->rde$l_flags)&VM_WRITE)
    *(unsigned long *)(p->pte)|=_PAGE_RW|_PAGE_DIRTY;
#endif

#ifdef __arch_um__
  *(unsigned long *)(p->pte)|=_PAGE_NEWPAGE;
#endif
  flush_tlb_range(current->mm, p->address, p->address + PAGE_SIZE);

  extern int myswapfile;
  struct _pfl * pfl = myswapfile;
  if (pfl && p->window==pfl->pfl$l_window)
    mmg$dallocpagfil1(p->offset>>PAGE_SHIFT);

  kfree(p);
}

extern int astdeb;

extern int in_atomic;

// replace this by building irp packet and using it. Plus fault wait state.
int makereadast(unsigned long window, unsigned long pfn, unsigned long address, unsigned long pte, unsigned long offset, unsigned long write_flag) {
  //mem_map[pfn].pfn$v_loc=PFN$C_RDINPROG;
  struct _acb * a=kmalloc(sizeof(struct _acb),GFP_KERNEL);
  struct pfast * pf=kmalloc(sizeof(struct pfast),GFP_KERNEL);
  struct _rde * rde;
  pf->window=window;
  pf->pfn=pfn;
  pf->address=address&0xfffff000;
  pf->offset=offset;
  pf->pte=pte;
  rde=mmg$lookup_rde_va(address, current->pcb$l_phd, LOOKUP_RDE_EXACT, IPL$_ASTDEL);
  pf->pteentry=rde->rde$r_regprot.regprt$l_region_prot;
  if (write_flag) {
    pf->pteentry|=_PAGE_RW|_PAGE_DIRTY;
    mem_map[pfn].pfn$l_page_state=PFN$M_MODIFY;
  }
  pf->rde=rde;
  a->acb$b_rmod=0;
  a->acb$l_kast=0;
  a->acb$l_ast=pagefaultast;
  a->acb$l_astprm=pf;
  astdeb=1;
  sch$qast(current->pcb$l_pid,0,a);
}


#ifdef __i386__

#define _PAGE_NEWPAGE 0

extern unsigned long idt;

asmlinkage void do_page_fault(struct pt_regs *regs, unsigned long error_code) {
	struct task_struct *tsk;
	struct mm_struct *mm;
	struct _rde * vma;
	unsigned long address;
	unsigned long page;
	signed long pfn;
	unsigned long fixup;
	int write;
	siginfo_t info;
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	struct _mypte * mypte;

	/* get the address */
	__asm__("movl %%cr2,%0":"=r" (address));

	/* It's safe to allow irq's after cr2 has been saved */
	if (regs->eflags & X86_EFLAGS_IF)
		local_irq_enable();

	//check if ipl>2 bugcheck

        if (address==0x5a5a5a5a)
          die("5a\n",regs,error_code);
	if (address==0x5a5a5a5a)
	  panic("poisoned\n");

	if (in_atomic) { 
	  printk("atomic addr %x\n",address);
	  address=0x11111111;
	}

#if 0
	if (intr_blocked(IPL$_MMG))
	  return;

	regtrap(REG_INTR,IPL$_MMG);

	setipl(IPL$_MMG);
#endif
	
	vmslock(&SPIN_MMG, IPL$_MMG);
	//vmslock(&SPIN_SCHED,-1);

	//some linux stuff
	tsk = current;

	if (in_atomic) { 
	  printk("atomic addr %x\n",address);
	  address=0x11111111;
	}

	tsk->pcb$l_phd->phd$l_pageflts++;

	//printk("fault %x ",address);
	//printk(":");

	/*
	 * We fault-in kernel-space virtual memory on-demand. The
	 * 'reference' page table is init_mm.pgd.
	 *
	 * NOTE! We MUST NOT take any locks for this case. We may
	 * be in an interrupt or a critical region, and should
	 * only copy the information from the master page table,
	 * nothing more.
	 *
	 * This verifies that the fault happens in kernel space
	 * (error_code & 4) == 0, and that the fault was not a
	 * protection error (error_code & 1) == 0.
	 */
	if (address >= TASK_SIZE && !(error_code & 5))
		goto vmalloc_fault;

	mm = tsk->mm;
	info.si_code = SEGV_MAPERR;

	/*
	 * If we're in an interrupt or have no user
	 * context, we must not take the fault..
	 */
	//	if (in_interrupt() || !mm)
	if (mm==&init_mm)
		goto no_context;
	if (!mm)
		goto no_context;

	if (address<PAGE_SIZE)
	  goto bad_area;

	if (address>0x90000000)
	  goto bad_area;

	page = address & PAGE_MASK;
	pgd = pgd_offset(mm, page);
	pud = pud_offset(pgd, page);
	pmd = pmd_offset(pud, page);

	if (0) /* not yet (!(pte_present(pmd))) */ { 
	  // transform it
	  printk("transform it\n");
	}
	pte = pte_offset(pmd, page);
	mypte=pte;
	//printk("fault3 %x %x %x %x",page,pgd,pmd,pte);
	mmg$frewsle(current,address);

	//printk(" pte %x ",*(unsigned long *)pte);

	// different forms of invalid ptes?
	// 0 valid bit, and...?
	// how to differentiate between
	// demandzeropage, transitionpage, invalidglobalpage, pageinpfile or
	//   in image file
	// Use bit 9-11, they are available for os use.
	// 11 and 10 reflect vax pte 26 22  (and 9 for 21)
	// 0 0 pfn=0 demand zero page
	// 0 0 pfn!=0 page in transition
	// 0 1 pfn=gpti invalid global page
	// 1 0 pfn=misc page is in page file
	// 1 1 page is in image file

	if ((*(unsigned long *)pte)&_PAGE_TYP1) { // page or image file
	  if ((*(unsigned long *)pte)&_PAGE_TYP0) { // image file
	    unsigned long index=(*(unsigned long *)pte)>>PAGE_SHIFT;
	    struct _secdef *pstl=current->pcb$l_phd->phd$l_pst_base_offset;
#if 0
	    struct _secdef *sec=&pstl[index];
	    struct _wcb * window=sec->sec$l_window;
	    unsigned long vbn=sec->sec$l_vbn;
	    struct _rde * rde= mmg$lookup_rde_va(address, current->pcb$l_phd, LOOKUP_RDE_EXACT, IPL$_ASTDEL);
	    unsigned long offset;// in PAGE_SIZE units
#else
	    struct _secdef *sec;
	    struct _wcb * window;
	    unsigned long vbn;
	    struct _rde * rde;
	    unsigned long offset;// in PAGE_SIZE units

	    if (index>64) {
	      printk("wrong %x %x %x %x\n",address,page,pte,*pte);
	      die("Wrong\n",regs,error_code);
	      panic("Wrong\n");
	    }
	    sec=&pstl[index];
	    window=sec->sec$l_window;
	    vbn=sec->sec$l_vbn;
	    rde= mmg$lookup_rde_va(address, current->pcb$l_phd, LOOKUP_RDE_EXACT, IPL$_ASTDEL);
#endif
	    if (rde==0) printk("vma0 address %x\n",address);
	    //printk(" i pstl sec window vbn rde %x %x %x %x %x %x\n",index,pstl,sec,window,vbn,rde);
	    offset=((address-(unsigned long)rde->rde$pq_start_va)>>PAGE_SHIFT)+(vbn>>3);
	    //printk(" offs %x ",offset);
	    //page_cache_read(window, offset);
	    //file->f_dentry->d_inode->i_mapping->a_ops->readpage(file, page);

	    //printk(" a ");
	    {
	      pfn = mmg$ininewpfn(tsk,tsk->pcb$l_phd,page,pte);
	      mem_map[pfn].pfn$v_loc=PFN$C_ACTIVE;
	      mem_map[pfn].pfn$q_bak=*(unsigned long *)pte;
	      // *(unsigned long *)pte=pfn // in the future have transition
	      *(unsigned long *)pte=((unsigned long)(pfn<<PAGE_SHIFT))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED|_PAGE_DIRTY;
	      //printk(" pfn pte %x %x ",pfn,*(unsigned long *)pte);
	    }
	    //printk(" b ");
	    flush_tlb_range(tsk->mm, page, page + PAGE_SIZE);
	    vmsunlock(&SPIN_MMG, -1);
	    makereadast(window,pfn,address,pte,offset,error_code&2);	
	    //printk(" a ");
	    return;
	  } else { // page file
	    extern int myswapfile;
	    struct _pfl * pfl=myswapfile;
	    struct _wcb * window=pfl->pfl$l_window;
	    unsigned long vbn=mypte->pte$v_pgflpag;
	    struct _rde * rde;
	    unsigned long offset;// in PAGE_SIZE units

	    offset=vbn<<PAGE_SHIFT;
	    {
	      pfn = mmg$ininewpfn(tsk,tsk->pcb$l_phd,page,pte);
	      mem_map[pfn].pfn$v_loc=PFN$C_ACTIVE;
	      mem_map[pfn].pfn$q_bak=*(unsigned long *)pte;
	      // *(unsigned long *)pte=pfn // in the future have transition
	      *(unsigned long *)pte=((unsigned long)(pfn<<PAGE_SHIFT))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED|_PAGE_DIRTY;
	    }
	    flush_tlb_range(tsk->mm, page, page + PAGE_SIZE);
	    printk("soon reading pfl_page %x %x %x %x\n",vbn,pte,*(long*)pte,page);
	    vmsunlock(&SPIN_MMG, -1);
	    makereadast(window,pfn,address,pte,offset,error_code&2);	
	    return;
	  }
	}

	if (!((*(unsigned long *)pte)&_PAGE_TYP1)) { //zero transition or global
	  if (!((*(unsigned long *)pte)&_PAGE_TYP0)) {
	    if ((*(unsigned long *)pte)&0xffffffff) { //transition
	      long pfn=mypte->pte$v_pfn;
	      if (pfn>=num_physpages) goto notyet;
	      if (pte!=mem_map[pfn].pfn$q_pte_index) goto notyet;
	      if ((*(unsigned long *)pte)&_PAGE_PRESENT) goto notyet;
	      int loc=mem_map[pfn].pfn$v_loc;
	      if (loc<=PFN$C_BADPAGLST) {
#ifdef OLDINT
		mmg$rempfn(loc,&mem_map[pfn]);
#else
		mmg$rempfn(loc,pfn);
#endif
		mem_map[pfn].pfn$v_loc=PFN$C_ACTIVE;
		mmg$makewsle(tsk,tsk->pcb$l_phd,address,pte,pfn);
		*(unsigned long *)pte|=_PAGE_PRESENT;
		flush_tlb_range(current->mm, page, page + PAGE_SIZE);
	      }
	      if (loc==PFN$C_WRTINPROG ) {
		// mmg$rempfn(loc,&mem_map[pfn]); // not needed?
		mem_map[pfn].pfn$v_loc=PFN$C_ACTIVE;
		mmg$makewsle(tsk,tsk->pcb$l_phd,address,pte,pfn);
		*(unsigned long *)pte|=_PAGE_PRESENT;
		flush_tlb_range(current->mm, page, page + PAGE_SIZE);
	      }
	      //printk("put transition page back in %x %x %x\n",loc,pte,address);
	      vmsunlock(&SPIN_MMG, -1);
	      return;
	    notyet:
	      {}
	    } else { // zero page demand?
	      {
		struct _rde * rde= mmg$lookup_rde_va(address, current->pcb$l_phd, LOOKUP_RDE_HIGHER, IPL$_ASTDEL);
		if (address<rde->rde$ps_start_va && address>=(rde->rde$ps_start_va-PAGE_SIZE)) {
		  rde->rde$ps_start_va-=PAGE_SIZE;
		  rde->rde$l_region_size+=PAGE_SIZE;
		}
		{
		  pfn = mmg$ininewpfn(tsk,tsk->pcb$l_phd,page,pte);
		  mem_map[pfn].pfn$v_loc=PFN$C_ACTIVE;
		  mem_map[pfn].pfn$q_bak=*(unsigned long *)pte;
		  mem_map[pfn].pfn$l_page_state|=PFN$M_MODIFY;
		  *(unsigned long *)pte=((unsigned long)(pfn<<PAGE_SHIFT))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED|_PAGE_DIRTY;
		  if (page==0) {
		    printk("wrong %x %x %x %x\n",address,page,pte,*pte);
		    die("Wrong\n",regs,error_code);
		    panic("Wrong\n");
		  }
		  flush_tlb_range(tsk->mm, page, page + PAGE_SIZE);
		  memset(page,0,PAGE_SIZE); // must zero content also
		  if ((error_code&2)==0) {
		    *(unsigned long *)pte=((unsigned long)(pfn<<PAGE_SHIFT))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_USER|_PAGE_ACCESSED;
		    flush_tlb_range(tsk->mm, page, page + PAGE_SIZE);
		  }
		}
	      }
	      vmsunlock(&SPIN_MMG, -1);
	      return;
	    }
	  } else {
	  }
	  

	}

#if 0
	vma= mmg$lookup_rde_va(address, current->pcb$l_phd, LOOKUP_RDE_EXACT, IPL$_ASTDEL);
	//do {
	survive2:
	  if (error_code&2) {
	    if (!pte_write(*pte)) {
	      switch (do_wp_page(mm, vma, address, pte, *pte)) {
	      case 1:
		current->min_flt++;
		break;
	      case 2:
		current->maj_flt++;
		break;
	      default:
		  //goto survive2;
		break;
	      }
	    }
	  }
	  //} while(!pte_present(*pte));
	  //  *(unsigned long*)pte |= 1;
	*pte = pte_mkyoung(*pte);
        if(pte_write(*pte)) *pte = pte_mkdirty(*pte);
        //flush_tlb_page2(mm, page);
        //up_read(&mm->mmap_sem);
	vmsunlock(&SPIN_MMG, -1);
        return(0);

	return;
#endif

	//	printk("fault2 %x ",address);
	down_read(&mm->mmap_sem);

	//	vma = find_vma(mm, address);
	vma = mmg$lookup_rde_va(address, current->pcb$l_phd, LOOKUP_RDE_EXACT, IPL$_ASTDEL);
	//	printk("err %x vma %x\n",error_code,vma);
	if (!vma)
		goto bad_area;
	if (vma->rde$ps_start_va <= address)
		goto good_area;
	if (!(vma->rde$l_flags & VM_GROWSDOWN))
		goto bad_area;
	if (error_code & 4) {
		/*
		 * accessing the stack below %esp is always a bug.
		 * The "+ 32" is there due to some instructions (like
		 * pusha) doing post-decrement on the stack and that
		 * doesn't show up until later..
		 */
		if (address + 32 < regs->esp)
			goto bad_area;
	}
	if (expand_stack2(vma, address))
		goto bad_area;
/*
 * Ok, we have a good vm_area for this memory access, so
 * we can handle it..
 */
good_area:
	info.si_code = SEGV_ACCERR;
	write = 0;
	switch (error_code & 3) {
		default:	/* 3: write, present */
#ifdef TEST_VERIFY_AREA
			if (regs->cs == KERNEL_CS)
				printk("WP fault at %08lx\n", regs->eip);
#endif
			/* fall through */
		case 2:		/* write, not present */
			if (!(vma->rde$l_flags & VM_WRITE))
				goto bad_area;
			write++;
			pfn=mypte->pte$v_pfn;
			mem_map[pfn].pfn$l_page_state=PFN$M_MODIFY;
			break;
		case 1:		/* read, present */
			goto bad_area;
		case 0:		/* read, not present */
			if (!(vma->rde$l_flags & (VM_READ | VM_EXEC)))
				goto bad_area;
	}

 survive:
	/*
	 * If for any reason at all we couldn't handle the fault,
	 * make sure we exit gracefully rather than endlessly redo
	 * the fault.
	 */
	//	printk("ha mm 1\n");
	//	switch (handle_mm_fault(mm, vma, address, write))
	switch (do_wp_page(mm, vma, address, pte, *pte)) {
	case 1:
		tsk->min_flt++;
		break;
	case 2:
		tsk->maj_flt++;
		break;
	case 0:
		goto do_sigbus;
	default:
		goto out_of_memory;
	}

	/*
	 * Did it hit the DOS screen memory VA from vm86 mode?
	 */
	if (regs->eflags & VM_MASK) {
		unsigned long bit = (address - 0xA0000) >> PAGE_SHIFT;
		if (bit < 32)
			tsk->thread.screen_bitmap |= 1 << bit;
	}
	up_read(&mm->mmap_sem);
	vmsunlock(&SPIN_MMG, -1);
	return;

/*
 * Something tried to access memory that isn't in our memory map..
 * Fix it, but check if it's kernel or user first..
 */
bad_area:
	up_read(&mm->mmap_sem);

	/* User mode accesses just cause a SIGSEGV */
	if (error_code & 4) {
		tsk->thread.cr2 = address;
		tsk->thread.error_code = error_code;
		tsk->thread.trap_no = 14;
		info.si_signo = SIGSEGV;
		info.si_errno = 0;
		/* info.si_code has been set above */
		info.si_addr = (void *)address;
		force_sig_info(SIGSEGV, &info, tsk);
		vmsunlock(&SPIN_MMG, -1);
		return;
	}

	/*
	 * Pentium F0 0F C7 C8 bug workaround.
	 */
	if (boot_cpu_data.f00f_bug) {
		unsigned long nr;
		
		nr = (address - idt) >> 3;

		if (nr == 6) {
			do_invalid_op(regs, 0);
			vmsunlock(&SPIN_MMG, -1);
			return;
		}
	}

no_context:
	/* Are we prepared to handle this kernel fault?  */
	if ((fixup = search_exception_table(regs->eip)) != 0) {
		regs->eip = fixup;
		vmsunlock(&SPIN_MMG, -1);
		return;
	}

/*
 * Oops. The kernel tried to access some bad page. We'll have to
 * terminate things with extreme prejudice.
 */

	bust_spinlocks(1);

	if (address < PAGE_SIZE)
		printk(KERN_ALERT "Unable to handle kernel NULL pointer dereference");
	else
		printk(KERN_ALERT "Unable to handle kernel paging request");
	printk(" at virtual address %08lx\n",address);
	printk(" printing eip:\n");
	printk("%08lx\n", regs->eip);
	asm("movl %%cr3,%0":"=r" (page));
	page = ((unsigned long *) __va(page))[address >> 22];
	printk(KERN_ALERT "*pde = %08lx\n", page);
	if (page & 1) {
		page &= PAGE_MASK;
		address &= 0x003ff000;
		page = ((unsigned long *) __va(page))[address >> PAGE_SHIFT];
		printk(KERN_ALERT "*pte = %08lx\n", page);
	}
	die("Oops", regs, error_code);
	bust_spinlocks(0);
	do_exit(SIGKILL);

/*
 * We ran out of memory, or some other thing happened to us that made
 * us unable to handle the page fault gracefully.
 */
out_of_memory:
	up_read(&mm->mmap_sem);
	if (tsk->pcb$l_pid == INIT_PID) {
	  //		tsk->policy |= SCHED_YIELD;
#if 0
	  current->need_resched=1;
		schedule();
#endif
		down_read(&mm->mmap_sem);
		goto survive;
	}
	printk("VM: killing process %s\n", tsk->pcb$t_lname);
	if (error_code & 4)
		do_exit(SIGKILL);
	goto no_context;

do_sigbus:
	up_read(&mm->mmap_sem);

	/*
	 * Send a sigbus, regardless of whether we were in kernel
	 * or user mode.
	 */
	tsk->thread.cr2 = address;
	tsk->thread.error_code = error_code;
	tsk->thread.trap_no = 14;
	info.si_signo = SIGBUS;
	info.si_errno = 0;
	info.si_code = BUS_ADRERR;
	info.si_addr = (void *)address;
	force_sig_info(SIGBUS, &info, tsk);

	/* Kernel mode? Handle exceptions or die */
	if (!(error_code & 4))
		goto no_context;
	vmsunlock(&SPIN_MMG, -1);
	return;

vmalloc_fault:
	{
		/*
		 * Synchronize this task's top level page-table
		 * with the 'reference' page table.
		 *
		 * Do _not_ use "tsk" here. We might be inside
		 * an interrupt in the middle of a task switch..
		 */
		int offset = __pgd_offset(address);
		pgd_t *pgd, *pgd_k;
		pud_t *pud, *pud_k;
		pmd_t *pmd, *pmd_k;
		pte_t *pte_k;

		asm("movl %%cr3,%0":"=r" (pgd));
		pgd = offset + (pgd_t *)__va(pgd);
		pgd_k = init_mm.pgd + offset;

		if (!pgd_present(*pgd_k))
			goto no_context;
		set_pgd(pgd, *pgd_k);
		
		pud = pud_offset(pgd, address);
		pud_k = pud_offset(pgd_k, address);
		if (!pud_present(*pud_k))
			goto no_context;
		set_pud(pud, *pud_k);

		pmd = pmd_offset(pud, address);
		pmd_k = pmd_offset(pud_k, address);
		if (!pmd_present(*pmd_k))
			goto no_context;
		set_pmd(pmd, *pmd_k);

		pte_k = pte_offset(pmd_k, address);
		if (!pte_present(*pte_k))
			goto no_context;
		vmsunlock(&SPIN_MMG, -1);
		return;
	}
}
#endif

#ifdef __arch_um__

unsigned long segv(unsigned long address, unsigned long ip, int is_write, 
		   int is_user)
{
	struct mm_struct *mm = current->mm;
	struct _rde *vma;
	struct siginfo si;
	void *catcher;
	pgd_t *pgd;
	pmd_t *pmd;
	pte_t *pte;
	unsigned long page;
	signed long pfn;
	struct _mypte * mypte;
	struct _pcb * tsk=current;

	//check if ipl>2 bugcheck

	if (intr_blocked(IPL$_MMG))
	  return;

	regtrap(REG_INTR,IPL$_MMG);

	setipl(IPL$_MMG);
	//vmslock(&SPIN_SCHED,-1);

	//some linux stuff
	if((address >= start_vm) && (address < end_vm)){
		flush_tlb_kernel_vm();
		return(0);
	}
	if(mm == NULL) panic("Segfault with no mm");

	if (address&0x80000000) {
	  //check if another process phd
	}

	if (address<PAGE_SIZE)
	  goto skip;

	if (address>0x80000000)
	  goto skip;

	if (in_atomic) { 
	  printk("atomic addr %x\n",address);
	  address=0x11111111;
	}

	current->pcb$l_phd->phd$l_pageflts++;

	// locate pte of address
	page = address & PAGE_MASK;
	pgd = pgd_offset(mm, page);
	pmd = pmd_offset(pgd, page);
	if (0) /* not yet (!(pte_present(pmd))) */ { 
	  // transform it
	  printk("transform it\n");
	}
	pte = pte_offset(pmd, page);

	mypte = pte;
	if (((unsigned long)mypte)<0x80000000)
	  goto skip;

	mmg$frewsle(current,address);

	// different forms of invalid ptes?
	// 0 valid bit, and...?
	// how to differentiate between
	// demandzeropage, transitionpage, invalidglobalpage, pageinpfile or
	//   in image file
	// Use bit 9-11, they are available for os use.
	// 11 and 10 reflect vax pte 26 22  (and 9 for 21)
	// 0 0 pfn=0 demand zero page
	// 0 0 pfn!=0 page in transition
	// 0 1 pfn=gpti invalid global page
	// 1 0 pfn=misc page is in page file
	// 1 1 page is in image file

	if (mypte->pte$v_typ1) { // page or image file
	  if (mypte->pte$v_typ0) { // image file
	    unsigned long index=(*(unsigned long *)pte)>>PAGE_SHIFT;
	    struct _secdef *pstl=current->pcb$l_phd->phd$l_pst_base_offset;
	    struct _secdef *sec=&pstl[index];
	    struct _wcb * window=sec->sec$l_window;
	    unsigned long vbn=sec->sec$l_vbn;
	    struct _rde * rde= mmg$lookup_rde_va(address, current->pcb$l_phd, LOOKUP_RDE_EXACT, IPL$_ASTDEL);
	    unsigned long offset;// in PAGE_SIZE units
	    offset=((address-(unsigned long)rde->rde$pq_start_va)>>PAGE_SHIFT)+(vbn>>3);

	    pfn = mmg$ininewpfn(tsk,tsk->pcb$l_phd,page,pte);
	    mem_map[pfn].pfn$v_loc=PFN$C_ACTIVE;
	    mem_map[pfn].pfn$q_bak=*(unsigned long *)pte;
	    // *(unsigned long *)pte=pfn // in the future have transition
	    *(unsigned long *)pte=((unsigned long)__va(pfn*PAGE_SIZE))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED|_PAGE_DIRTY;
	    flush_tlb_range(current->mm, page, page + PAGE_SIZE);
	    
	    makereadast(window,pfn,address,pte,offset,is_write);
	    return;
	  } else { // page file
	    extern int myswapfile;
	    struct _pfl * pfl=myswapfile;
	    struct _wcb * window=pfl->pfl$l_window;
	    unsigned long vbn=mypte->pte$v_pgflpag;
	    struct _rde * rde;
	    unsigned long offset;// in PAGE_SIZE units

	    offset=vbn<<PAGE_SHIFT;
	    {
	      pfn = mmg$ininewpfn(tsk,tsk->pcb$l_phd,page,pte);
	      mem_map[pfn].pfn$v_loc=PFN$C_ACTIVE;
	      mem_map[pfn].pfn$q_bak=*(unsigned long *)pte;
	      // *(unsigned long *)pte=pfn // in the future have transition
	      *(unsigned long *)pte=((unsigned long)(pfn<<PAGE_SHIFT))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED|_PAGE_DIRTY;
	    }
	    flush_tlb_range(tsk->mm, page, page + PAGE_SIZE);
	    printk("soon reading pfl_page %x %x %x %x\n",vbn,pte,*(long*)pte,page);
	    makereadast(window,pfn,address,pte,offset,is_write);	
	    return;
	  }
	}

	if (mypte->pte$v_typ1==0) { //zero transition or global
	  if (mypte->pte$v_typ0==0) {
	    if (mypte->pte$v_pfn) { //transition
	      long pfn=mypte->pte$v_pfn;
	      if (pfn>=num_physpages) goto notyet;
	      if (pte!=mem_map[pfn].pfn$q_pte_index) goto notyet;
	      if ((*(unsigned long *)pte)&_PAGE_PRESENT) goto notyet;
	      int loc=mem_map[pfn].pfn$v_loc;
	      if (loc<=PFN$C_BADPAGLST) {
#ifdef OLDINT
		mmg$rempfn(loc,&mem_map[pfn]);
#else
		mmg$rempfn(loc,pfn);
#endif
		mem_map[pfn].pfn$v_loc=PFN$C_ACTIVE;
		mmg$makewsle(tsk,tsk->pcb$l_phd,address,pte,pfn);
		*(unsigned long *)pte|=_PAGE_PRESENT;
		flush_tlb_range(current->mm, page, page + PAGE_SIZE);
	      }
	      if (loc==PFN$C_WRTINPROG ) {
		// mmg$rempfn(loc,&mem_map[pfn]); // not needed?
		mem_map[pfn].pfn$v_loc=PFN$C_ACTIVE;
		mmg$makewsle(tsk,tsk->pcb$l_phd,address,pte,pfn);
		*(unsigned long *)pte|=_PAGE_PRESENT;
		flush_tlb_range(current->mm, page, page + PAGE_SIZE);
	      }
	      //printk("put transition page back in %x %x %x\n",loc,pte,address);
	      return;
	    notyet:
	    } else { // zero page demand?
	      struct _rde * rde= mmg$lookup_rde_va(address, current->pcb$l_phd, LOOKUP_RDE_HIGHER, IPL$_ASTDEL);
	      if (address<rde->rde$ps_start_va && address>=(rde->rde$ps_start_va-PAGE_SIZE)) {
		rde->rde$ps_start_va-=PAGE_SIZE;
		rde->rde$l_region_size+=PAGE_SIZE;
	      }
	      pfn = mmg$ininewpfn(tsk,tsk->pcb$l_phd,page,pte);
	      mem_map[pfn].pfn$v_loc=PFN$C_ACTIVE;
	      mem_map[pfn].pfn$q_bak=*(unsigned long *)pte;
	      mem_map[pfn].pfn$l_page_state|=PFN$M_MODIFY;
	      *(unsigned long *)pte=((unsigned long)__va(pfn*PAGE_SIZE))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED|_PAGE_DIRTY;
	      flush_tlb_range(current->mm, page, page + PAGE_SIZE);
	      bzero(page,PAGE_SIZE); // must zero content also
	      if (is_write==0) {
		*(unsigned long *)pte=((unsigned long)__va(pfn*PAGE_SIZE))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_USER|_PAGE_ACCESSED;
		flush_tlb_range(current->mm, page, page + PAGE_SIZE);
	      }
	      return;
	    }
	  } else {
	    struct _mypte * gpt = mmg$gq_gpt_base;
	    unsigned long index=mypte->pte$v_gptx;
	    struct _mypte * gpte = &gpt[index];

	    if (gpte->pte$v_valid) {
	      if (gpte->pte$v_typ1) { // global section, no pagefiling yet
		*mypte=*gpte;
		mypte->pte$v_global=0;	
		mypte->pte$v_gblwrt=0;	
		mmg$incptref(mmg$gl_sysphd,pte);
	      } else { // global zero
		*mypte=*gpte;
		mypte->pte$v_global=0;
		mypte->pte$v_gblwrt=0;
		mmg$incptref(mmg$gl_sysphd,pte);
	      }

	    } else {

	      if (gpte->pte$v_typ1) { // global section, no pagefiling yet
		unsigned long gptx=mypte->pte$v_gptx;
		struct _mypte * gpte=&((struct _mypte *)mmg$gq_gpt_base)[gptx];
		struct _secdef * pstl=((struct _phd *)mmg$gl_sysphd)->phd$l_pst_base_offset;
		unsigned long index=gpte->pte$v_stx;
		struct _secdef *sec=&pstl[index];
		struct _wcb * window=sec->sec$l_window;
		unsigned long vbn=sec->sec$l_vbn;
		unsigned long offset=gptx-sec->sec$l_vpx;// in PAGE_SIZE units
		pfn = mmg$ininewpfn(tsk,tsk->pcb$l_phd,page|PFN$C_GLOBAL,pte);
		mem_map[pfn].pfn$v_loc=PFN$C_ACTIVE;
		*(unsigned long *)gpte=((unsigned long)__va(pfn*PAGE_SIZE))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED|_PAGE_DIRTY;
		gpte->pte$v_global=1;
		*mypte=*gpte;
		mypte->pte$v_global=0;	
		mypte->pte$v_gblwrt=0;	
		*(unsigned long *)pte|=_PAGE_DIRTY;//collided with gblwrt
		flush_tlb_range(current->mm, page, page + PAGE_SIZE);
		makereadast(window,pfn,address,pte,offset,is_write);
	      } else { // global zero
		pfn = mmg$ininewpfn(tsk,tsk->pcb$l_phd,page|PFN$C_GLOBAL,pte);
		mem_map[pfn].pfn$v_loc=PFN$C_ACTIVE;
		*(unsigned long *)pte=((unsigned long)__va(pfn*PAGE_SIZE))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED|_PAGE_DIRTY;
		*(unsigned long *)gpte=((unsigned long)__va(pfn*PAGE_SIZE))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED|_PAGE_DIRTY;
		gpte->pte$v_global=1;
		flush_tlb_range(current->mm, page, page + PAGE_SIZE);
	      }
	    }


	  }
	  

	}

	vma= mmg$lookup_rde_va(address, current->pcb$l_phd, LOOKUP_RDE_EXACT, IPL$_ASTDEL);
	//do {
	survive2:
	  if (is_write) {
	    if (!pte_write(*pte)) {
	      pfn=mypte->pte$v_pfn;
	      mem_map[pfn].pfn$l_page_state=PFN$M_MODIFY;
	      switch (do_wp_page(mm, vma, address, pte, *pte)) {
	      case 1:
		current->min_flt++;
		break;
	      case 2:
		current->maj_flt++;
		break;
	      default:
		if (current->pcb$l_pid == INIT_PID) {
		  up_read(&mm->mmap_sem);
		  yield();
		  down_read(&mm->mmap_sem);
		  //goto survive2;
		}
	      }
	    }
	  }
	  //} while(!pte_present(*pte));
	  //  *(unsigned long*)pte |= 1;
	*pte = pte_mkyoung(*pte);
        if(pte_write(*pte)) *pte = pte_mkdirty(*pte);
        flush_tlb_page2(mm, page);
        //up_read(&mm->mmap_sem);
        return(0);

	return;

	if (is_write==0) {
	  pfn = mmg$ininewpfn(tsk,tsk->pcb$l_phd,page,pte);
	  mem_map[pfn].pfn$v_loc=PFN$C_ACTIVE;
	  *(unsigned long *)pte=((unsigned long)__va(pfn*PAGE_SIZE))|_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED|_PAGE_DIRTY;
	  flush_tlb_range(current->mm, page, page + PAGE_SIZE);
	  bcopy(page,__va(pfn*PAGE_SIZE),PAGE_SIZE);
	}

	return;
	// mmg$ininewpfn(tsk,va,pte); // put this somewhere?

 skip:
	// keep some linux stuff for now
	catcher = current->thread.fault_catcher;
	si.si_code = SEGV_MAPERR;
	down_read(&mm->mmap_sem);
	//vma = find_vma(mm, address);
	goto bad; // got straight to bad for now.
      
	vma = mmg$lookup_rde_va(address, current->pcb$l_phd, LOOKUP_RDE_EXACT, IPL$_ASTDEL);
	if(!vma) goto bad;
	else if(vma->rde$pq_start_va <= address) goto good_area;
//else if(!(vma->vm_flags & VM_GROWSDOWN)) goto bad;// -> DESCEND
	else if(expand_stack2(vma, address)) goto bad;

 good_area:
	si.si_code = SEGV_ACCERR;
//if(is_write && !(vma->vm_flags & VM_WRITE)) goto bad;

	page = address & PAGE_MASK;
	if(page == (unsigned long) current + PAGE_SIZE)
		panic("Kernel stack overflow");
	pgd = pgd_offset(mm, page);
	pmd = pmd_offset(pgd, page);
	do {
	survive:
	        switch (do_wp_page(mm, vma, address, pte, *pte)) {
		case 1:
			current->min_flt++;
			break;
		case 2:
			current->maj_flt++;
			break;
		default:
			if (current->pcb$l_pid == INIT_PID) {
				up_read(&mm->mmap_sem);
				yield();
				down_read(&mm->mmap_sem);
				goto survive;
			}
			/* Fall through to bad area case */
		case 0:
			goto bad;
		}
		pte = pte_offset(pmd, page);
	} while(!pte_present(*pte));
	*pte = pte_mkyoung(*pte);
	if(pte_write(*pte)) *pte = pte_mkdirty(*pte);
	flush_tlb_page2(mm, page);
	up_read(&mm->mmap_sem);
	return(0);
 bad:
	if(catcher != NULL){
		current->thread.fault_addr = (void *) address;
		up_read(&mm->mmap_sem);
		do_longjmp(catcher);
	} 
	else if(current->thread.fault_addr != NULL){
		panic("fault_addr set but no fault catcher");
	}
	if(!is_user) 
		panic("Kernel mode fault at addr 0x%lx, ip 0x%lx", 
		      address, ip);
	si.si_signo = SIGSEGV;
	si.si_addr = (void *) address;
	current->thread.cr2 = address;
	current->thread.err = is_write;
	force_sig_info(SIGSEGV, &si, current);
	up_read(&mm->mmap_sem);
	return(0);
}
#endif
#endif

#ifdef CONFIG_VMS

#if 0
long myindex=0;
long mystack[1024];
long mystack2[1024];
long mystack3[1024];
long mystack4[1024];
#endif

int mmg$frewsle(struct _pcb * p, void * va) {
  struct _phd * phd = p->pcb$l_phd;
  struct _wsl * wsl = phd->phd$l_wslist;
  struct _wsl wsle ;
  unsigned long * pte;
  unsigned long index;
  unsigned long va2;
  unsigned long sts;

 again:
  wsle = wsl[phd->phd$l_wsnext];
  if (wsle.wsl$pq_va==0) { // empty
    if (phd->phd$l_wssize<p->pcb$l_ppgcnt+p->pcb$l_gpgcnt) {
      goto more;
    }
    // compare to wsquota
    if (phd->phd$l_wsquota<p->pcb$l_ppgcnt+p->pcb$l_gpgcnt) {
      goto more;
    }

    return SS$_NORMAL;

    // compare to growlim
    // this probably gets selected
  }
  if ((((int)wsle.wsl$pq_va)&WSL$M_PAGTYP)==WSL$C_PPGTBL||(((int)wsle.wsl$pq_va)&WSL$M_PAGTYP)==WSL$C_GPGTBL) {
    // can not be used
  }
  // check if it points to a dead page table
  // but empty but translation buffer, skip
  // if selected for reuse and non-empty
  // if not selected incr inde and start again

  index=phd->phd$l_wsnext;
  va2=((unsigned long)wsle.wsl$pq_va)&0xfffff000;
  pte=findpte_new(p->mm,va2);

  {
#ifdef __arch_um__
    signed long pfn=__pa(((struct _mypte*)pte)->pte$v_pfn << PAGE_SHIFT) >> PAGE_SHIFT ;
#else
    signed long pfn=((struct _mypte*)pte)->pte$v_pfn;
#endif

#if 0
    //if dem zero (data page)?
    if ((mem_map[pfn].pfn$q_bak&PTE$M_TYP0)==0)
      goto more;

    if (*pte&_PAGE_DIRTY)
      goto more;
#endif
    if ((unsigned long)va2>=0x70000000) goto more;
  }

  sts=mmg$frewslx(p, va2,pte,index);
  if (sts==SS$_NORMAL)
    return SS$_NORMAL;

 more:
  phd->phd$l_wsnext++;
  if (phd->phd$l_wsnext>phd->phd$l_wslast)
    phd->phd$l_wsnext=0; //? phd->phd$l_wsdyn;
  goto again;

}

int mmg$frewslx(struct _pcb * p, void * va,unsigned long * pte, unsigned long index) {
#ifdef __arch_um__
  signed long pfn=__pa(((struct _mypte*)pte)->pte$v_pfn << PAGE_SHIFT) >> PAGE_SHIFT ;
#else
  signed long pfn=((struct _mypte*)pte)->pte$v_pfn;
#endif
  struct _wsl * wsl = p->pcb$l_phd->phd$l_wslist;

#if 0
  // not yet?
  if (*pte&_PAGE_DIRTY)
    mem_map[pfn].pfn$v_modify=1;
  else
    mem_map[pfn].pfn$v_modify=0;
  (*pte) &= ~(_PAGE_DIRTY|_PAGE_PRESENT);
#endif
  // how to fix tlb/tb?

  // has pagefile backing store?

#ifdef __arch_um__
  //forget it?
  *pte|=_PAGE_NEWPAGE;
#endif
  flush_tlb_range(p->mm, va, va + PAGE_SIZE);

  //wsl[index].wsl$pq_va=0;
  mmg$delwslx(p,p->pcb$l_phd,index,pte);

#if 0
  mystack[myindex]=va;
  mystack2[myindex]=pte;
  mystack3[myindex]=*(long*)pte;
  mystack4[myindex]=index|((mem_map[pfn].pfn$l_page_state&PFN$M_MODIFY)<<16);
  myindex++;
#endif
  mmg$relpfn(pfn);

  return SS$_NORMAL;
}

mmg$delwslx(struct _pcb * pcb, struct _phd * phd, int index,int pte) {
  struct _wsl * wsl = phd->phd$l_wslist;

  if ((((unsigned long)wsl[index].wsl$pq_va)&WSL$M_PAGTYP)==WSL$C_GLOBAL) {
    //*pte = mem_map[pfn].pfn$q_bak;
    mmg$decptref(pcb->pcb$l_phd,pte);
    pcb->pcb$l_gpgcnt--;
  }

  if ((((unsigned long)wsl[index].wsl$pq_va)&WSL$M_PAGTYP)==WSL$C_PROCESS) {
    //*pte = mem_map[pfn].pfn$q_bak; // need this  here too?
    pcb->pcb$l_ppgcnt--;
  }

  wsl[index].wsl$pq_va=0;

}

unsigned long findpte_new(struct mm_struct *mm, unsigned long address) {
  unsigned long page;
  pgd_t *pgd = 0;
  pud_t *pud = 0;
  pmd_t *pmd = 0;
  pte_t *pte = 0;
  page = address & PAGE_MASK;
  pgd = pgd_offset(mm, page);
  pud = pud_offset(pgd, page);
  pmd = pmd_offset(pud, page);
  if (pmd && *(long *)pmd)
    pte = pte_offset(pmd, page);
  return pte;
}

#ifdef __x86_64__

/* Sometimes the CPU reports invalid exceptions on prefetch.
   Check that here and ignore.
   Opcode checker based on code by Richard Brunner */
static int is_prefetch(struct pt_regs *regs, unsigned long addr)
{ 
	unsigned char *instr = (unsigned char *)(regs->rip);
	int scan_more = 1;
	int prefetch = 0; 
	unsigned char *max_instr = instr + 15;

	/* Avoid recursive faults for this common case */
	if (regs->rip == addr)
		return 0; 

	if (regs->cs & (1<<2))
		return 0;

	while (scan_more && instr < max_instr) { 
		unsigned char opcode;
		unsigned char instr_hi;
		unsigned char instr_lo;

		if (__get_user(opcode, instr))
			break; 

		instr_hi = opcode & 0xf0; 
		instr_lo = opcode & 0x0f; 
		instr++;

		switch (instr_hi) { 
		case 0x20:
		case 0x30:
			/* Values 0x26,0x2E,0x36,0x3E are valid x86
			   prefixes.  In long mode, the CPU will signal
			   invalid opcode if some of these prefixes are
			   present so we will never get here anyway */
			scan_more = ((instr_lo & 7) == 0x6);
			break;
			
		case 0x40:
			/* In AMD64 long mode, 0x40 to 0x4F are valid REX prefixes
			   Need to figure out under what instruction mode the
			   instruction was issued ... */
			/* Could check the LDT for lm, but for now it's good
			   enough to assume that long mode only uses well known
			   segments or kernel. */
			scan_more = ((regs->cs & 3) == 0) || (regs->cs == __USER_CS);
			break;
			
		case 0x60:
			/* 0x64 thru 0x67 are valid prefixes in all modes. */
			scan_more = (instr_lo & 0xC) == 0x4;
			break;		
		case 0xF0:
			/* 0xF0, 0xF2, and 0xF3 are valid prefixes in all modes. */
			scan_more = !instr_lo || (instr_lo>>1) == 1;
			break;			
		case 0x00:
			/* Prefetch instruction is 0x0F0D or 0x0F18 */
			scan_more = 0;
			if (__get_user(opcode, instr)) 
				break;
			prefetch = (instr_lo == 0xF) &&
				(opcode == 0x0D || opcode == 0x18);
			break;			
		default:
			scan_more = 0;
			break;
		} 
	}

#if 0
	if (prefetch)
		printk("%s: prefetch caused page fault at %lx/%lx\n", current->pcb$t_lname,
		       regs->rip, addr);
#endif
	return prefetch;
}

#define _PAGE_NEWPAGE 0

int page_fault_trace; 
int exception_trace = 1;

/*
 * This routine handles page faults.  It determines the address,
 * and the problem, and then passes it off to one of the appropriate
 * routines.
 *
 * error_code:
 *	bit 0 == 0 means no page found, 1 means protection fault
 *	bit 1 == 0 means read, 1 means write
 *	bit 2 == 0 means kernel, 1 means user-mode
 *      bit 3 == 1 means fault was an instruction fetch
 */
asmlinkage void do_page_fault(struct pt_regs *regs, unsigned long error_code)
{
	struct task_struct *tsk;
	struct mm_struct *mm;
	struct _rde * vma;
	unsigned long address;
	unsigned long page;
	signed long pfn;
	unsigned long fixup;
	int write;
	siginfo_t info;
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	struct _mypte * mypte;

	/* get the address */
	__asm__("movq %%cr2,%0":"=r" (address));

	if (regs->eflags & X86_EFLAGS_IF)
		__sti();

#ifdef CONFIG_CHECKING
	if (1 ||page_fault_trace) 
		printk("pagefault rip:%lx rsp:%lx cs:%lu ss:%lu address %lx error %lx\n",
		       regs->rip,regs->rsp,regs->cs,regs->ss,address,error_code); 


	{ 
		unsigned long gs; 
		struct x8664_pda *pda = cpu_pda + safe_smp_processor_id(); 
		rdmsrl(MSR_GS_BASE, gs); 
		if (gs != (unsigned long)pda) { 
			wrmsrl(MSR_GS_BASE, pda); 
			printk("page_fault: wrong gs %lx expected %p\n", gs, pda);
		}
	}
#endif
			
	//check if ipl>2 bugcheck

	if (address==0x5a5a5a5a)
	  panic("poisoned\n");

	if (in_atomic) { 
	  printk("atomic addr %x\n",address);
	  address=0x11111111;
	}

#if 0
	if (intr_blocked(IPL$_MMG))
	  return;

	regtrap(REG_INTR,IPL$_MMG);

	setipl(IPL$_MMG);
	//vmslock(&SPIN_SCHED,-1);
#endif

	vmslock(&SPIN_MMG, IPL$_MMG);

	//some linux stuff
	tsk = current;
	mm = tsk->mm;
	info.si_code = SEGV_MAPERR;

	if (in_atomic) { 
	  printk("atomic addr %x\n",address);
	  address=0x11111111;
	}

	tsk->pcb$l_phd->phd$l_pageflts++;

	/* 5 => page not present and from supervisor mode */
	if (unlikely(!(error_code & 5) &&
		     ((address >= VMALLOC_START && address <= VMALLOC_END) ||
		      (address >= MODULES_VADDR && address <= MODULES_END))))
		goto vmalloc_fault;
  
	/*
	 * If we're in an interrupt or have no user
	 * context, we must not take the fault..
	 */
	if (mm==&init_mm)
		goto no_context;
	if (in_interrupt() || !mm)
		goto bad_area_nosemaphore;

	if (address<PAGE_SIZE)
	  goto bad_area;

	/* 
	 * Work around K8 errata #100. See the K8 specification update for 
	 * details. Any code segment in LDT is compatibility mode.
	 */
	if ((regs->cs == __USER32_CS || (regs->cs & (1<<2))) &&
		(address >> 32))
	  {
	    vmsunlock(&SPIN_MMG, -1);
		return;
	  }

#if 0
	page = address & PAGE_MASK;
	pgd = pgd_offset(mm, page);
	pmd = pmd_offset(pgd, page);
#else
	spin_unlock(&mm->page_table_lock); // check. not quite resolved
	page = address & PAGE_MASK;
	pgd = pgd_offset(mm, page);
	pud = pud_alloc(mm, pgd, page);
	pmd = pmd_alloc(mm, pud, page);
	pte = pte_alloc(mm, pmd, page);
	spin_unlock(&mm->page_table_lock);
#endif

	if (0) /* not yet (!(pte_present(pmd))) */ { 
	  // transform it
	  printk("transform it\n");
	}
#if 0
	pte = pte_offset(pmd, page);
#endif
	mypte=pte;
	//printk("fault3 %lx %lx %lx %lx %lx",page,pgd,pmd,pte,*(long*)pte);
	mmg$frewsle(current,address);

	//printk(" pte %x ",*(unsigned long *)pte);

	// different forms of invalid ptes?
	// 0 valid bit, and...?
	// how to differentiate between
	// demandzeropage, transitionpage, invalidglobalpage, pageinpfile or
	//   in image file
	// Use bit 9-11, they are available for os use.
	// 11 and 10 reflect vax pte 26 22  (and 9 for 21)
	// 0 0 pfn=0 demand zero page
	// 0 0 pfn!=0 page in transition
	// 0 1 pfn=gpti invalid global page
	// 1 0 pfn=misc page is in page file
	// 1 1 page is in image file

	if ((*(unsigned long *)pte)&_PAGE_TYP1) { // page or image file
	  if ((*(unsigned long *)pte)&_PAGE_TYP0) { // image file
	    unsigned long index=(*(unsigned long *)pte)>>PAGE_SHIFT;
	    struct _secdef *pstl=current->pcb$l_phd->phd$l_pst_base_offset;
#if 0
	    struct _secdef *sec=&pstl[index];
	    struct _wcb * window=sec->sec$l_window;
	    unsigned long vbn=sec->sec$l_vbn;
	    struct _rde * rde= mmg$lookup_rde_va(address, current->pcb$l_phd, LOOKUP_RDE_EXACT, IPL$_ASTDEL);
	    unsigned long offset;// in PAGE_SIZE units
#else
	    struct _secdef *sec;
	    struct _wcb * window;
	    unsigned long vbn;
	    struct _rde * rde;
	    unsigned long offset;// in PAGE_SIZE units

	    if (index>64) {
	      printk("wrong %x %x %x %x\n",address,page,pte,*pte);
	      die("Wrong\n",regs,error_code);
	      panic("Wrong\n");
	    }
	    sec=&pstl[index];
	    window=sec->sec$l_window;
	    vbn=sec->sec$l_vbn;
	    rde= mmg$lookup_rde_va(address, current->pcb$l_phd, LOOKUP_RDE_EXACT, IPL$_ASTDEL);
#endif
	    if (rde==0) printk("vma0 address %x\n",address);
	    //printk(" i pstl sec window vbn rde %x %x %x %x %x %x\n",index,pstl,sec,window,vbn,rde);
	    offset=((address-(unsigned long)rde->rde$pq_start_va)>>PAGE_SHIFT)+(vbn>>3);
	    //printk(" offs %x ",offset);
	    //page_cache_read(window, offset);
	    //file->f_dentry->d_inode->i_mapping->a_ops->readpage(file, page);

	    //printk(" a ");
	    {
	      pfn = mmg$ininewpfn(tsk,tsk->pcb$l_phd,page,pte);
	      mem_map[pfn].pfn$v_loc=PFN$C_ACTIVE;
	      mem_map[pfn].pfn$q_bak=*(unsigned long *)pte;
	      // *(unsigned long *)pte=pfn // in the future have transition
	      *(unsigned long *)pte=((unsigned long)(pfn<<PAGE_SHIFT))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED|_PAGE_DIRTY;
	      //printk(" pfn pte %x %x ",pfn,*(unsigned long *)pte);
	    }
	    //printk(" b ");
	    flush_tlb_range(tsk->mm, page, page + PAGE_SIZE);
	    vmsunlock(&SPIN_MMG, -1);
	    makereadast(window,pfn,address,pte,offset,error_code&2);	
	    //printk(" a ");
	    return;
	  } else { // page file
	    extern int myswapfile;
	    struct _pfl * pfl=myswapfile;
	    struct _wcb * window=pfl->pfl$l_window;
	    unsigned long vbn=mypte->pte$v_pgflpag;
	    struct _rde * rde;
	    unsigned long offset;// in PAGE_SIZE units

	    offset=vbn<<PAGE_SHIFT;
	    {
	      pfn = mmg$ininewpfn(tsk,tsk->pcb$l_phd,page,pte);
	      mem_map[pfn].pfn$v_loc=PFN$C_ACTIVE;
	      mem_map[pfn].pfn$q_bak=*(unsigned long *)pte;
	      // *(unsigned long *)pte=pfn // in the future have transition
	      *(unsigned long *)pte=((unsigned long)(pfn<<PAGE_SHIFT))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED|_PAGE_DIRTY;
	    }
	    flush_tlb_range(tsk->mm, page, page + PAGE_SIZE);
	    printk("soon reading pfl_page %x %x %x %x\n",vbn,pte,*(long*)pte,page);
	    vmsunlock(&SPIN_MMG, -1);
	    makereadast(window,pfn,address,pte,offset,error_code&2);	
	    return;
	  }
	}

	if (!((*(unsigned long *)pte)&_PAGE_TYP1)) { //zero transition or global
	  if (!((*(unsigned long *)pte)&_PAGE_TYP0)) {
	    if ((*(unsigned long *)pte)&0xffffffff) { //transition
	      long pfn=mypte->pte$v_pfn;
	      if (pfn>=num_physpages) goto notyet;
	      if (pte!=mem_map[pfn].pfn$q_pte_index) goto notyet;
	      if ((*(unsigned long *)pte)&_PAGE_PRESENT) goto notyet;
	      int loc=mem_map[pfn].pfn$v_loc;
	      if (loc<=PFN$C_BADPAGLST) {
#ifdef OLDINT
		mmg$rempfn(loc,&mem_map[pfn]);
#else
		mmg$rempfn(loc,pfn);
#endif
		mem_map[pfn].pfn$v_loc=PFN$C_ACTIVE;
		mmg$makewsle(tsk,tsk->pcb$l_phd,address,pte,pfn);
		*(unsigned long *)pte|=_PAGE_PRESENT;
		flush_tlb_range(current->mm, page, page + PAGE_SIZE);
	      }
	      if (loc==PFN$C_WRTINPROG ) {
		// mmg$rempfn(loc,&mem_map[pfn]); // not needed?
		mem_map[pfn].pfn$v_loc=PFN$C_ACTIVE;
		mmg$makewsle(tsk,tsk->pcb$l_phd,address,pte,pfn);
		*(unsigned long *)pte|=_PAGE_PRESENT;
		flush_tlb_range(current->mm, page, page + PAGE_SIZE);
	      }
	      //printk("put transition page back in %x %x %x\n",loc,pte,address);
	      vmsunlock(&SPIN_MMG, -1);
	      return;
	    notyet:
	      {}
	    } else { // zero page demand?
	      {
		struct _rde * rde= mmg$lookup_rde_va(address, current->pcb$l_phd, LOOKUP_RDE_HIGHER, IPL$_ASTDEL);
		if (address<rde->rde$ps_start_va && address>=(rde->rde$ps_start_va-PAGE_SIZE)) {
		  rde->rde$ps_start_va-=PAGE_SIZE;
		  rde->rde$l_region_size+=PAGE_SIZE;
		}
		{
		  pfn = mmg$ininewpfn(tsk,tsk->pcb$l_phd,page,pte);
		  mem_map[pfn].pfn$v_loc=PFN$C_ACTIVE;
		  mem_map[pfn].pfn$q_bak=*(unsigned long *)pte;
		  mem_map[pfn].pfn$l_page_state|=PFN$M_MODIFY;
		  *(unsigned long *)pte=((unsigned long)(pfn<<PAGE_SHIFT))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED|_PAGE_DIRTY;
		  if (page==0) {
		    printk("wrong %x %x %x %x\n",address,page,pte,*pte);
		    die("Wrong\n",regs,error_code);
		    panic("Wrong\n");
		  }
		  flush_tlb_range(tsk->mm, page, page + PAGE_SIZE);
		  memset(page,0,PAGE_SIZE); // must zero content also
		  if ((error_code&2)==0) {
		    *(unsigned long *)pte=((unsigned long)(pfn<<PAGE_SHIFT))|_PAGE_NEWPAGE|_PAGE_PRESENT|_PAGE_USER|_PAGE_ACCESSED;
		    flush_tlb_range(tsk->mm, page, page + PAGE_SIZE);
		  }
		}
	      }
	      vmsunlock(&SPIN_MMG, -1);
	      return;
	    }
	  } else {
	  }
	  

	}

again:
	down_read(&mm->mmap_sem);

	// vma = find_vma(mm, address);
	vma = mmg$lookup_rde_va(address, current->pcb$l_phd, LOOKUP_RDE_EXACT, IPL$_ASTDEL);
	if (!vma)
		goto bad_area;
	if (vma->rde$ps_start_va <= address)
		goto good_area;
	if (!(vma->rde$l_flags & VM_GROWSDOWN))
		goto bad_area;
	if (error_code & 4) {
		// XXX: align red zone size with ABI 
		if (address + 128 < regs->rsp)
			goto bad_area;
	}
	if (expand_stack2(vma, address))
		goto bad_area;
/*
 * Ok, we have a good vm_area for this memory access, so
 * we can handle it..
 */
good_area:
	info.si_code = SEGV_ACCERR;
	write = 0;
	switch (error_code & 3) {
		default:	/* 3: write, present */
			/* fall through */
		case 2:		/* write, not present */
			if (!(vma->rde$l_flags & VM_WRITE))
				goto bad_area;
			write++;
			pfn=mypte->pte$v_pfn;
                        mem_map[pfn].pfn$l_page_state=PFN$M_MODIFY;
			break;
		case 1:		/* read, present */
			goto bad_area;
		case 0:		/* read, not present */
			if (!(vma->rde$l_flags & (VM_READ | VM_EXEC)))
				goto bad_area;
	}

	/*
	 * If for any reason at all we couldn't handle the fault,
	 * make sure we exit gracefully rather than endlessly redo
	 * the fault.
	 */
	//switch (handle_mm_fault(mm, vma, address, write)) {
	switch (do_wp_page(mm, vma, address, pte, *pte)) {
	case 1:
		tsk->min_flt++;
		break;
	case 2:
		tsk->maj_flt++;
		break;
	case 0:
		goto do_sigbus;
	default:
		goto out_of_memory;
	}

	up_read(&mm->mmap_sem);
	vmsunlock(&SPIN_MMG, -1);
	return;

/*
 * Something tried to access memory that isn't in our memory map..
 * Fix it, but check if it's kernel or user first..
 */
bad_area:
	up_read(&mm->mmap_sem);

bad_area_nosemaphore:
	/* User mode accesses just cause a SIGSEGV */
	if (error_code & 4) {
		if (is_prefetch(regs, address))
		  {
	    vmsunlock(&SPIN_MMG, -1);
			return;
		  }

		if (exception_trace && !(tsk->ptrace & PT_PTRACED) && 
		    (tsk->sig->action[SIGSEGV-1].sa.sa_handler == SIG_IGN ||
		    (tsk->sig->action[SIGSEGV-1].sa.sa_handler == SIG_DFL)))
			printk(KERN_INFO 
		       "%s[%d]: segfault at %016lx rip %016lx rsp %016lx error %lx\n",
					tsk->pcb$t_lname, tsk->pcb$l_pid, address, regs->rip,
					regs->rsp, error_code);
	
		tsk->thread.cr2 = address;
		/* Kernel addresses are always protection faults */
		tsk->thread.error_code = error_code | (address >= TASK_SIZE);
		tsk->thread.trap_no = 14;
		info.si_signo = SIGSEGV;
		info.si_errno = 0;
		/* info.si_code has been set above */
		info.si_addr = (void *)address;
		force_sig_info(SIGSEGV, &info, tsk);
		vmsunlock(&SPIN_MMG, -1);
		return;
	}

no_context:
	
	/* Are we prepared to handle this kernel fault?  */
	if ((fixup = search_exception_table(regs->rip)) != 0) {
		regs->rip = fixup;
		if (0 && exception_trace) 
		printk(KERN_ERR 
		       "%s: fixed kernel exception at %lx address %lx err:%ld\n", 
		       tsk->pcb$t_lname, regs->rip, address, error_code);
		vmsunlock(&SPIN_MMG, -1);
		return;
	}

	if (is_prefetch(regs, address))
	  {
	    vmsunlock(&SPIN_MMG, -1);
		return;
	  }

/*
 * Oops. The kernel tried to access some bad page. We'll have to
 * terminate things with extreme prejudice.
 */

	unsigned long flags; 
	prepare_die(&flags);
	if (address < PAGE_SIZE)
		printk(KERN_ALERT "Unable to handle kernel NULL pointer dereference");
	else
		printk(KERN_ALERT "Unable to handle kernel paging request");
	printk(KERN_ALERT " at %016lx RIP: ", address); 
	printk_address(regs->rip);
	dump_pagetable(address);
	__die("Oops", regs, error_code);
	/* Executive summary in case the oops scrolled away */
	printk(KERN_EMERG "CR2: %016lx\n", address);
	exit_die(flags);
	do_exit(SIGKILL);

/*
 * We ran out of memory, or some other thing happened to us that made
 * us unable to handle the page fault gracefully.
 */
out_of_memory:
	up_read(&mm->mmap_sem);
	if (current->pcb$l_pid == 1) { 
#if 0
		tsk->policy |= SCHED_YIELD;
#endif
		schedule();
		goto again;
	}
	printk("VM: killing process %s\n", tsk->pcb$t_lname);
	if (error_code & 4)
		do_exit(SIGKILL);
	goto no_context;

do_sigbus:
	up_read(&mm->mmap_sem);

	/* Kernel mode? Handle exceptions or die */
	if (!(error_code & 4))
		goto no_context;
		
	if (is_prefetch(regs, address))
	  {
	    vmsunlock(&SPIN_MMG, -1);

		return;
	  }

	tsk->thread.cr2 = address;
	tsk->thread.error_code = error_code;
	tsk->thread.trap_no = 14;
	info.si_signo = SIGBUS;
	info.si_errno = 0;
	info.si_code = BUS_ADRERR;
	info.si_addr = (void *)address;
	force_sig_info(SIGBUS, &info, tsk);
	vmsunlock(&SPIN_MMG, -1);
	return;


vmalloc_fault:
	{
		pgd_t *pgd;
		pud_t *pud;
		pmd_t *pmd;
		pte_t *pte; 

		/* 
		 * x86-64 has the same kernel 3rd level pages for all CPUs.
		 * But for vmalloc/modules the TLB synchronization works lazily,
		 * so it can happen that we get a page fault for something
		 * that is really already in the page table. Just check if it
		 * is really there and when yes flush the local TLB. 
		 */
#if 0
		printk("vmalloc fault %lx index %lu\n",address,pml4_index(address));
		dump_pagetable(address);
#endif

		pgd = pgd_offset_k(address);
#if 0
		if (pgd != current_pgd_offset_k(address)) 
			goto bad_area_nosemaphore;	 
#endif
		if (!pgd_present(*pgd))
			goto bad_area_nosemaphore;
		pud = pud_offset(pgd, address);
		if (!pud_present(*pud))
			goto bad_area_nosemaphore;
		pmd = pmd_offset(pud, address);
		if (!pmd_present(*pmd))
			goto bad_area_nosemaphore;
		pte = pte_offset(pmd, address); 
		if (!pte_present(*pte))
			goto bad_area_nosemaphore; 

		__flush_tlb_all();		
		vmsunlock(&SPIN_MMG, -1);
		return;
	}
}
#endif
#endif
