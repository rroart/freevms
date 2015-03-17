#ifndef _ASM_HW_IRQ_H
#define _ASM_HW_IRQ_H

// check

#ifndef __ASSEMBLY__
/*
 *	linux/include/asm/hw_irq.h
 *
 *	(C) 1992, 1993 Linus Torvalds, (C) 1997 Ingo Molnar
 *
 *	moved some of the old arch/i386/kernel/irq.h to here. VY
 *
 *	IRQ/IPI changes taken from work by Thomas Radke
 *	<tomsoft@informatik.tu-chemnitz.de>
 *
 *	hacked by Andi Kleen for x86-64.
 *
 *  $Id$
 */

#include <linux/config.h>
#include <linux/stddef.h>
#include <asm/atomic.h>
#include <asm/irq.h>
#ifndef ASM_OFFSET_H
#include <asm/offset.h>
#endif

#endif

/*
 * IDT vectors usable for external interrupt sources start
 * at 0x20:
 */
#define FIRST_EXTERNAL_VECTOR	0x20

#define IA32_SYSCALL_VECTOR	0x80
#define IA32_VMSSYSCALL_VECTOR       0x81
#define IA32_VMSSYSCALL_VECTOR1       0x82
#define IA32_VMSSYSCALL_VECTOR2       0x83
#define IA32_VMSSYSCALL_VECTOR3       0x84
#define IA32_VMSSYSCALL_VECTOR0       0x85

#define FORKABLE_IPL_VECTOR 3904
#define POWER_VECTOR 0xaf
#define EMB_VECTOR 0xae
#define MCHECK_VECTOR 18
#define MEGA_VECTOR 0xac
#define IPINTR_VECTOR 0xab
#define VIRTCONS_VECTOR 0xaa
#define HWCLK_VECTOR 0xa9
#define INVALIDATE_VECTOR 0xa8
#define PERFMON_VECTOR 0xa7
#define MAILBOX_VECTOR 0xa6
#define POOL_VECTOR 0xa5
#define IOLOCK11_VECTOR 0xa4
#define IOLOCK10_VECTOR 0xa3
#define IOLOCK9_VECTOR 0xa2
#define SYNCH_VECTOR 0xa1
#define TIMER_VECTOR 0xa0
#define SCS_VECTOR 0x9f
#define SCHED_VECTOR 0x9e
#define MMG_VECTOR 14
#define IO_MISC_VECTOR 0x9c
#define FILSYS_VECTOR 0x9b
#define TX_SYNCH_VECTOR 0x9a
#define LCKMGR_VECTOR 0x99
#define IOLOCK8_VECTOR 0x98
#define PORT_VECTOR 0x97
#define TIMERFORK_VECTOR 0x96
#define QUEUEAST_VECTOR 0x95
#define IOPOST_VECTOR 0x94
#define RESCHED_VECTOR 0x93
#define ASTDEL_VECTOR 0x92

#define WRAPSTR2(z)  WRAPSTR(z)
#define WRAPSTR(z) #z

#define tsk_psl_wrap WRAPSTR2(tsk_psl)

#define DOSOFTINT(ipl,vector,psl) \
do { __asm__ __volatile__ ( \
"pushq %rbx\n\t" \
"movq 0x7ffff000, %rbx\n\t" \
"movq " psl "(%rbx), %rbx\n\t" \
"sarq $0x4, %rbx\n\t" \
"andq $0x1f, %rbx\n\t" \
"cmpq " #ipl ", %rbx\n\t" \
"jae 1f\n\t" \
"popq %rbx\n\t" \
"int " #vector "\n\t" \
"jmp 2f\n\t" \
"1:\n\t" \
"movq smp$gl_cpu_data, %rbx\n\t" \
"btsw " #ipl ", 0xa(%rbx)\n\t" /* sisr */ \
"popq %rbx\n\t" \
"2:\n\t" \
); \
} while (0);

#if 0
#define SOFTINT_TEST_VECTOR do { __asm__ __volatile__ ("int $0x88\n"); } while (0);

#define SOFTINT_POWER_VECTOR do { __asm__ __volatile__ ("int $0xaf\n"); } while (0);
#define SOFTINT_EMB_VECTOR do { __asm__ __volatile__ ("int $0xae\n"); } while (0);
#define SOFTINT_MCHECK_VECTOR do { __asm__ __volatile__ ("int 18\n"); } while (0);
#define SOFTINT_MEGA_VECTOR do { __asm__ __volatile__ ("int $0xac\n"); } while (0);
#define SOFTINT_IPINTR_VECTOR do { __asm__ __volatile__ ("int $0xab\n"); } while (0);
#define SOFTINT_VIRTCONS_VECTOR do { __asm__ __volatile__ ("int $0xaa\n"); } while (0);
#define SOFTINT_HWCLK_VECTOR do { __asm__ __volatile__ ("int $0xa9\n"); } while (0);
#define SOFTINT_INVALIDATE_VECTOR do { __asm__ __volatile__ ("int $0xa8\n"); } while (0);
#define SOFTINT_PERFMON_VECTOR do { __asm__ __volatile__ ("int $0xa7\n"); } while (0);
#define SOFTINT_MAILBOX_VECTOR do { __asm__ __volatile__ ("int $0xa6\n"); } while (0);
#define SOFTINT_POOL_VECTOR do { __asm__ __volatile__ ("int $0xa5\n"); } while (0);
#define SOFTINT_IOLOCK11_VECTOR do { __asm__ __volatile__ ("int $0xa4\n"); } while (0);
#define SOFTINT_IOLOCK10_VECTOR do { __asm__ __volatile__ ("int $0xa3\n"); } while (0);
#define SOFTINT_IOLOCK9_VECTOR do { __asm__ __volatile__ ("int $0xa2\n"); } while (0);
#define SOFTINT_SYNCH_VECTOR do { __asm__ __volatile__ ("int $0xa1\n"); } while (0);
#define SOFTINT_TIMER_VECTOR do { __asm__ __volatile__ ("int $0xa0\n"); } while (0);
#define SOFTINT_SCS_VECTOR do { __asm__ __volatile__ ("int $0x9f\n"); } while (0);
#define SOFTINT_SCHED_VECTOR do { __asm__ __volatile__ ("int $0x9e\n"); } while (0);
#define SOFTINT_MMG_VECTOR do { __asm__ __volatile__ ("int 14\n"); } while (0);
#define SOFTINT_IO_MISC_VECTOR do { __asm__ __volatile__ ("int $0x9c\n"); } while (0);
#define SOFTINT_FILSYS_VECTOR do { __asm__ __volatile__ ("int $0x9b\n"); } while (0);
#define SOFTINT_TX_SYNCH_VECTOR do { __asm__ __volatile__ ("int $0x9a\n"); } while (0);
#define SOFTINT_LCKMGR_VECTOR do { __asm__ __volatile__ ("int $0x99\n"); } while (0);
#define SOFTINT_IOLOCK8_VECTOR do { __asm__ __volatile__ ("int $0x98\n"); } while (0);
#define SOFTINT_PORT_VECTOR do { __asm__ __volatile__ ("int $0x97\n"); } while (0);
#define SOFTINT_TIMERFORK_VECTOR do { __asm__ __volatile__ ("int $0x96\n"); } while (0);
#define SOFTINT_QUEUEAST_VECTOR do { __asm__ __volatile__ ("int $0x95\n"); } while (0);
#define SOFTINT_IOPOST_VECTOR do { __asm__ __volatile__ ("int $0x94\n"); } while (0);
#define SOFTINT_RESCHED_VECTOR do { __asm__ __volatile__ ("int $0x93\n"); } while (0);
#define SOFTINT_ASTDEL_VECTOR do { __asm__ __volatile__ ("int $0x92\n"); } while (0);
#else
#define SOFTINT_POWER_VECTOR DOSOFTINT($0x0,$0xaf,tsk_psl_wrap)
#define SOFTINT_EMB_VECTOR DOSOFTINT($0x0,$0xae,tsk_psl_wrap)
#define SOFTINT_MCHECK_VECTOR DOSOFTINT($0x0,$18,tsk_psl_wrap)
#define SOFTINT_MEGA_VECTOR DOSOFTINT($0x0,$0xac,tsk_psl_wrap)
#define SOFTINT_IPINTR_VECTOR DOSOFTINT($0x0,$0xab,tsk_psl_wrap)
#define SOFTINT_VIRTCONS_VECTOR DOSOFTINT($0x0,$0xaa,tsk_psl_wrap)
#define SOFTINT_HWCLK_VECTOR DOSOFTINT($0x0,$0xa9,tsk_psl_wrap)
#define SOFTINT_INVALIDATE_VECTOR DOSOFTINT($0x0,$0xa8,tsk_psl_wrap)
#define SOFTINT_PERFMON_VECTOR DOSOFTINT($0x0,$0xa7,tsk_psl_wrap)
#define SOFTINT_MAILBOX_VECTOR DOSOFTINT($0x0,$0xa6,tsk_psl_wrap)
#define SOFTINT_POOL_VECTOR DOSOFTINT($0x0,$0xa5,tsk_psl_wrap)
#define SOFTINT_IOLOCK11_VECTOR DOSOFTINT($0x11,$0xa4,tsk_psl_wrap)
#define SOFTINT_IOLOCK10_VECTOR DOSOFTINT($0x10,$0xa3,tsk_psl_wrap)
#define SOFTINT_IOLOCK9_VECTOR DOSOFTINT($0x9,$0xa2,tsk_psl_wrap)
#define SOFTINT_SYNCH_VECTOR DOSOFTINT($0x8,$0xa1,tsk_psl_wrap)
#define SOFTINT_TIMER_VECTOR DOSOFTINT($0x8,$0xa0,tsk_psl_wrap)
#define SOFTINT_SCS_VECTOR DOSOFTINT($0x8,$0x9f,tsk_psl_wrap)
#define SOFTINT_SCHED_VECTOR DOSOFTINT($0x8,$0x9e,tsk_psl_wrap)
#define SOFTINT_MMG_VECTOR DOSOFTINT($0x8,$14,tsk_psl_wrap)
#define SOFTINT_IO_MISC_VECTOR DOSOFTINT($0x8,$0x9c,tsk_psl_wrap)
#define SOFTINT_FILSYS_VECTOR DOSOFTINT($0x8,$0x9b,tsk_psl_wrap)
#define SOFTINT_TX_SYNCH_VECTOR DOSOFTINT($0x8,$0x9a,tsk_psl_wrap)
#define SOFTINT_LCKMGR_VECTOR DOSOFTINT($0x8,$0x99,tsk_psl_wrap)
#define SOFTINT_IOLOCK8_VECTOR DOSOFTINT($0x8,$0x98,tsk_psl_wrap)
#define SOFTINT_PORT_VECTOR DOSOFTINT($0x0,$0x97,tsk_psl_wrap)
#define SOFTINT_TIMERFORK_VECTOR DOSOFTINT($0x7,$0x96,tsk_psl_wrap)
#define SOFTINT_QUEUEAST_VECTOR DOSOFTINT($0x6,$0x95,tsk_psl_wrap)
#define SOFTINT_IOPOST_VECTOR DOSOFTINT($0x4,$0x94,tsk_psl_wrap)
#define SOFTINT_RESCHED_VECTOR DOSOFTINT($0x3,$0x93,tsk_psl_wrap)
#define SOFTINT_ASTDEL_VECTOR DOSOFTINT($0x2,$0x92,tsk_psl_wrap)
#endif

/*
 * Vectors 0x20-0x2f are used for ISA interrupts.
 */

/*
 * Special IRQ vectors used by the SMP architecture, 0xf0-0xff
 *
 *  some of the following vectors are 'rare', they are merged
 *  into a single vector (CALL_FUNCTION_VECTOR) to save vector space.
 *  TLB, reschedule and local APIC vectors are performance-critical.
 *
 *  Vectors 0xf0-0xf9 are free (reserved for future Linux use).
 */
#define SPURIOUS_APIC_VECTOR	0xff
#define ERROR_APIC_VECTOR	0xfe
#define INVALIDATE_TLB_VECTOR	0xfd
#define RESCHEDULE_VECTOR	0xfc
/* 0xfa free */
#define CALL_FUNCTION_VECTOR	0xfb
#define IPINT_VECTOR		0xfa

/*
 * Local APIC timer IRQ vector is on a different priority level,
 * to work around the 'lost local interrupt if more than 2 IRQ
 * sources per level' errata.
 */
#define LOCAL_TIMER_VECTOR	0xef

/*
 * First APIC vector available to drivers: (vectors 0x30-0xee)
 * we start at 0x31 to spread out vectors evenly between priority
 * levels. (0x80 is the syscall vector)
 */
#define FIRST_DEVICE_VECTOR	0x31
#define FIRST_SYSTEM_VECTOR	0xef

#ifndef __ASSEMBLY__
extern int irq_vector[NR_IRQS];
#define IO_APIC_VECTOR(irq)	irq_vector[irq]

/*
 * Various low-level irq details needed by irq.c, process.c,
 * time.c, io_apic.c and smp.c
 *
 * Interrupt entry/exit code at both C and assembly level
 */

extern void mask_irq(unsigned int irq);
extern void unmask_irq(unsigned int irq);
extern void disable_8259A_irq(unsigned int irq);
extern void enable_8259A_irq(unsigned int irq);
extern int i8259A_irq_pending(unsigned int irq);
extern void make_8259A_irq(unsigned int irq);
extern void init_8259A(int aeoi);
extern void FASTCALL(send_IPI_self(int vector));
extern void init_VISWS_APIC_irqs(void);
extern void setup_IO_APIC(void);
extern void disable_IO_APIC(void);
extern void print_IO_APIC(void);
extern int IO_APIC_get_PCI_irq_vector(int bus, int slot, int fn);
extern void send_IPI(int dest, int vector);

extern unsigned long io_apic_irqs;

extern atomic_t irq_err_count;
extern atomic_t irq_mis_count;

extern char _stext, _etext;

#define IO_APIC_IRQ(x) (((x) >= 16) || ((1<<(x)) & io_apic_irqs))

#include <asm/ptrace.h>

extern void reschedule_interrupt(void);
extern void invalidate_interrupt(void);
extern void call_function_interrupt(void);
extern void ipint_interrupt(void);
extern void apic_timer_interrupt(void);
extern void spurious_interrupt(void);
extern void error_interrupt(void);

#define IRQ_NAME2(nr) nr##_interrupt(void)
#define IRQ_NAME(nr) IRQ_NAME2(IRQ##nr)

#define BUILD_IRQ(nr) \
asmlinkage void IRQ_NAME(nr); \
__asm__( \
"\n"__ALIGN_STR "\n" \
SYMBOL_NAME_STR(IRQ) #nr "_interrupt:\n\t" \
	"push $" #nr "-256 ; " \
	"jmp common_interrupt");

extern unsigned long prof_cpu_mask;
extern unsigned int * prof_buffer;
extern unsigned long prof_len;
extern unsigned long prof_shift;

/*
 * x86 profiling function, SMP safe. We might want to do this in
 * assembly totally?
 */
static inline void x86_do_profile (unsigned long eip)
{
    if (!prof_buffer)
        return;

    /*
     * Only measure the CPUs specified by /proc/irq/prof_cpu_mask.
     * (default is all CPUs.)
     */
    if (!((1<<smp_processor_id()) & prof_cpu_mask))
        return;

    eip -= (unsigned long) &_stext;
    eip >>= prof_shift;
    /*
     * Don't ignore out-of-bounds EIP values silently,
     * put them into the last histogram slot, so if
     * present, they will show up as a sharp peak.
     */
    if (eip > prof_len-1)
        eip = prof_len-1;
    atomic_inc((atomic_t *)&prof_buffer[eip]);
}

#ifdef CONFIG_SMP /*more of this file should probably be ifdefed SMP */
static inline void hw_resend_irq(struct hw_interrupt_type *h, unsigned int i)
{
    if (IO_APIC_IRQ(i))
        send_IPI_self(IO_APIC_VECTOR(i));
}
#else
static inline void hw_resend_irq(struct hw_interrupt_type *h, unsigned int i) {}
#endif

#endif

#endif /* _ASM_HW_IRQ_H */
