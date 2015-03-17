// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2006

/*
 *  linux/arch/x86-64/mm/fault.c
 *
 *  Copyright (C) 1995  Linus Torvalds
 *  Copyright (C) 2001,2002 Andi Kleen, SuSE Labs.
 */

#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/ptrace.h>
#include <linux/mman.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/smp_lock.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/vt_kern.h>		/* For unblank_screen() */
#include <linux/compiler.h>

#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/pgalloc.h>
#include <asm/hardirq.h>
#include <asm/smp.h>
#include <asm/proto.h>
#include <asm/kdebug.h>

extern spinlock_t console_lock, timerlist_lock;

void bust_spinlocks(int yes)
{
    spin_lock_init(&timerlist_lock);
    if (yes)
    {
        oops_in_progress = 1;
#ifdef CONFIG_SMP
        global_irq_lock = 0;	/* Many serial drivers do __global_cli() */
#endif
    }
    else
    {
        int loglevel_save = console_loglevel;
#ifdef CONFIG_VT
        unblank_screen();
#endif
        oops_in_progress = 0;
        /*
         * OK, the message is on the console.  Now we call printk()
         * without oops_in_progress set so that printk will give klogd
         * a poke.  Hold onto your hats...
         */
        console_loglevel = 15;		/* NMI oopser may have shut the console up */
        printk(" ");
        console_loglevel = loglevel_save;
    }
}

static int bad_address(void *p)
{
    unsigned long dummy;
    return __get_user(dummy, (unsigned long *)p);
}

void dump_pagetable(unsigned long address)
{
#if 0
    pml4_t *pml4;
    asm("movq %%cr3,%0" : "=r" (pml4));

    pml4 = __va((unsigned long)pml4 & PHYSICAL_PAGE_MASK);
    pml4 += pml4_index(address);
    printk("PML4 %lx ", pml4_val(*pml4));
    if (bad_address(pml4)) goto bad;
    if (!pml4_present(*pml4)) goto ret;

    pgd_t *pgd = __pgd_offset_k((pgd_t *)pml4_page(*pml4), address);
    if (bad_address(pgd)) goto bad;
    printk("PGD %lx ", pgd_val(*pgd));
    if (!pgd_present(*pgd))	goto ret;

    pmd_t *pmd = pmd_offset(pgd, address);
    if (bad_address(pmd)) goto bad;
    printk("PMD %lx ", pmd_val(*pmd));
    if (!pmd_present(*pmd))	goto ret;

    pte_t *pte = pte_offset(pmd, address);
    if (bad_address(pte)) goto bad;
    printk("PTE %lx", pte_val(*pte));
ret:
    printk("\n");
    return;
bad:
    printk("BAD\n");
#endif
}

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

    while (scan_more && instr < max_instr)
    {
        unsigned char opcode;
        unsigned char instr_hi;
        unsigned char instr_lo;

        if (__get_user(opcode, instr))
            break;

        instr_hi = opcode & 0xf0;
        instr_lo = opcode & 0x0f;
        instr++;

        switch (instr_hi)
        {
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

