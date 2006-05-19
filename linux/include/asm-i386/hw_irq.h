// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004.

#ifndef _ASM_HW_IRQ_H
#define _ASM_HW_IRQ_H

/*
 *	linux/include/asm/hw_irq.h
 *
 *	(C) 1992, 1993 Linus Torvalds, (C) 1997 Ingo Molnar
 *
 *	moved some of the old arch/i386/kernel/irq.h to here. VY
 *
 *	IRQ/IPI changes taken from work by Thomas Radke
 *	<tomsoft@informatik.tu-chemnitz.de>
 */

#include <linux/config.h>
#include <asm/atomic.h>
#include <asm/irq.h>
#include <asm/current.h>
#include <linux/sched.h>

/*
 * IDT vectors usable for external interrupt sources start
 * at 0x20:
 */
#define FIRST_EXTERNAL_VECTOR	0x20

#define SYSCALL_VECTOR		0x80
#define VMSSYSCALL_VECTOR       0x81
#define VMSSYSCALL_VECTOR1       0x82
#define VMSSYSCALL_VECTOR2       0x83
#define VMSSYSCALL_VECTOR3       0x84

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

#define SOFTINT_TEST_VECTOR do { __asm__ __volatile__ ("int $0x88\n"); } while (0);

#define DOSOFTINT(ipl,vector) \
do { __asm__ __volatile__ ( \
"pushl %ebx\n\t" \
"movl ctl$gl_pcb, %ebx\n\t" \
"movl 2032(%ebx), %ebx\n\t" \
"sarl $0x4, %ebx\n\t" \
"andl $0x1f, %ebx\n\t" \
"cmpl " #ipl ", %ebx\n\t" \
"jae 1f\n\t" \
"popl %ebx\n\t" \
"int " #vector "\n\t" \
"jmp 2f\n\t" \
"1:\n\t" \
"movl smp$gl_cpu_data, %ebx\n\t" \
"btsw " #ipl ", 0x6(%ebx)\n\t" /* sisr */ \
"popl %ebx\n\t" \
"2:\n\t" \
); \
} while (0);

#if defined(CONFIG_VMS) && !defined(__arch_um__)
#if 0
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
#define SOFTINT_POWER_VECTOR DOSOFTINT($0x0,$0xaf)
#define SOFTINT_EMB_VECTOR DOSOFTINT($0x0,$0xae)
#define SOFTINT_MCHECK_VECTOR DOSOFTINT($0x0,$18)
#define SOFTINT_MEGA_VECTOR DOSOFTINT($0x0,$0xac)
#define SOFTINT_IPINTR_VECTOR DOSOFTINT($0x0,$0xab)
#define SOFTINT_VIRTCONS_VECTOR DOSOFTINT($0x0,$0xaa)
#define SOFTINT_HWCLK_VECTOR DOSOFTINT($0x0,$0xa9)
#define SOFTINT_INVALIDATE_VECTOR DOSOFTINT($0x0,$0xa8)
#define SOFTINT_PERFMON_VECTOR DOSOFTINT($0x0,$0xa7)
#define SOFTINT_MAILBOX_VECTOR DOSOFTINT($0x0,$0xa6)
#define SOFTINT_POOL_VECTOR DOSOFTINT($0x0,$0xa5)
#define SOFTINT_IOLOCK11_VECTOR DOSOFTINT($0xb,$0xa4)
#define SOFTINT_IOLOCK10_VECTOR DOSOFTINT($0xa,$0xa3)
#define SOFTINT_IOLOCK9_VECTOR DOSOFTINT($0x9,$0xa2)
#define SOFTINT_SYNCH_VECTOR DOSOFTINT($0x8,$0xa1)
#define SOFTINT_TIMER_VECTOR DOSOFTINT($0x8,$0xa0)
#define SOFTINT_SCS_VECTOR DOSOFTINT($0x8,$0x9f)
#define SOFTINT_SCHED_VECTOR DOSOFTINT($0x8,$0x9e)
#define SOFTINT_MMG_VECTOR DOSOFTINT($0x8,$14)
#define SOFTINT_IO_MISC_VECTOR DOSOFTINT($0x8,$0x9c)
#define SOFTINT_FILSYS_VECTOR DOSOFTINT($0x8,$0x9b)
#define SOFTINT_TX_SYNCH_VECTOR DOSOFTINT($0x8,$0x9a)
#define SOFTINT_LCKMGR_VECTOR DOSOFTINT($0x8,$0x99)
#define SOFTINT_IOLOCK8_VECTOR DOSOFTINT($0x8,$0x98)
#define SOFTINT_PORT_VECTOR DOSOFTINT($0x0,$0x97)
#define SOFTINT_TIMERFORK_VECTOR DOSOFTINT($0x7,$0x96)
#define SOFTINT_QUEUEAST_VECTOR DOSOFTINT($0x6,$0x95)
#define SOFTINT_IOPOST_VECTOR DOSOFTINT($0x4,$0x94)
#define SOFTINT_RESCHED_VECTOR DOSOFTINT($0x3,$0x93)
#define SOFTINT_ASTDEL_VECTOR DOSOFTINT($0x2,$0x92)
#endif
#else

#define SOFTINT_IOLOCK11_VECTOR do { exe$frkipl11dsp(); myrei(); } while (0);
#define SOFTINT_IOLOCK10_VECTOR do { exe$frkipl10dsp(); myrei(); } while (0);
#define SOFTINT_IOLOCK9_VECTOR do { exe$frkipl9dsp(); myrei(); } while (0);
#define SOFTINT_IOLOCK8_VECTOR do { exe$frkipl8dsp(); myrei(); } while (0);
#define SOFTINT_TIMERFORK_VECTOR do { exe$swtimint(); myrei(); } while (0);
#define SOFTINT_QUEUEAST_VECTOR do { exe$frkipl6dsp(); myrei(); } while(0);
#define SOFTINT_IOPOST_VECTOR do { ioc$iopost(); myrei(); } while (0);
#define SOFTINT_RESCHED_VECTOR do { sch$resched(); myrei(); } while (0);
#define SOFTINT_ASTDEL_VECTOR do { sch$astdel(); myrei(); } while (0);
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
 *  Vectors 0xf0-0xfa are free (reserved for future Linux use).
 */
#define SPURIOUS_APIC_VECTOR	0xff
#define ERROR_APIC_VECTOR	0xfe
#define INVALIDATE_TLB_VECTOR	0xfd
#define RESCHEDULE_VECTOR	0xfc
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

#ifndef __arch_um__
extern atomic_t irq_err_count;
#endif
extern atomic_t irq_mis_count;

#ifdef __arch_um__
extern unsigned long _stext, _etext;
#else
extern char _stext, _etext;
#endif

/* PUSHR_ALL to PUSHPSL corresponds to stuff in entry.S */

#define PUSHR_ALL \
	"cld\n\t" \
	"pushl %es\n\t" \
	"pushl %ds\n\t" \
	"pushl %eax\n\t" \
	"pushl %ebp\n\t" \
	"pushl %edi\n\t" \
	"pushl %esi\n\t" \
	"pushl %edx\n\t" \
	"pushl %ecx\n\t" \
	"pushl %ebx\n\t"

#define POPR_ALL	\
	"popl %ebx\n\t"	\
	"popl %ecx\n\t"	\
	"popl %edx\n\t"	\
	"popl %esi\n\t"	\
	"popl %edi\n\t"	\
	"popl %ebp\n\t"	\
	"popl %eax\n\t"	\
	"popl %ds\n\t"	\
	"popl %es\n\t"

#define PUSHPSL \
        PUSHR_ALL \
        "call pushpsl\n\t" \
        POPR_ALL 

#define PUSHPSLI \
        PUSHR_ALL \
        "call pushpsli\n\t" \
        POPR_ALL 

#define REGTRAP \
	"pushl %eax\n\t" \
	"pushl $0x10\n\t" \
	"pushl $0x0\n\t" \
	"call regtrap\n\t" \
	"call setpsli\n\t" \
	"popl %eax\n\t" \
	"popl %eax\n\t" \
	"popl %eax\n\t"

#define INTEXC_FIX_SP(x) \
	"pushl %eax; \n\t" \
	"pushl %edi; \n\t" \
	"pushl %edx; \n\t" \
	"movl ctl$gl_pcb, %edi	; \n\t" \
	"movl 0x7a0(%edi), %edx ; \n\t" /* tsk_pcbl_cpu_id */ \
	"shl $0x8, %edx; \n\t" \
	"addl $init_tss, %edx	; \n\t" \
	"addl $0x4, %edx	; \n\t" \
	"addl $2112, %edi; \n\t" /* ipr_sp */ \
	"movl 0x4+0x10(%esp), %eax; \n\t" /* check. manual 4 */ \
	"andl $0x3, %eax; \n\t" \
	"je 2f			; \n\t" \
	/*"movl %esp, (%edi); \n\t"*/ /* check ekstra */ \
	/*"movl %esp, (%edx); \n\t"*/ /* check ekstra */ \
	/*"addl $0xc, (%edi); \n\t"*/ /* check ekstra */ \
	/*"addl $0xc, (%edx); \n\t"*/ /* check ekstra */ \
	"salw $2,%ax; \n\t" \
	"addl %eax, %edi; \n\t" \
	"addl %eax, %edx; \n\t" \
	"addl %eax, %edx; \n\t" \
	"cmpl $0xc, %eax; \n\t" \
	"movl 0x4+0x18(%esp),%eax; \n\t" /* check. manual 4 */ \
	"je 4f			; \n\t" \
	"jmp 3f			; \n\t" \
"2:				; \n\t" \
	"movl %esp, %eax	; \n\t" \
	"addl $0xc, %eax	; \n\t" \
"3:				; \n\t" \
	"movl %eax, (%edx)	; \n\t" \
"4:				; \n\t" \
	"movl %eax, (%edi); \n\t" \
	"popl %edx; \n\t" \
	"popl %edi; \n\t" \
	"popl %eax; \n\t"
	
#define IO_APIC_IRQ(x) (((x) >= 16) || ((1<<(x)) & io_apic_irqs))

#define __STR(x) #x
#define STR(x) __STR(x)

#define SAVE_ALL \
	"cld\n\t" \
	"pushl %es\n\t" \
	"pushl %ds\n\t" \
	"pushl %eax\n\t" \
	"pushl %ebp\n\t" \
	"pushl %edi\n\t" \
	"pushl %esi\n\t" \
	"pushl %edx\n\t" \
	"pushl %ecx\n\t" \
	"pushl %ebx\n\t" \
	"movl $" STR(__KERNEL_DS) ",%edx\n\t" \
	"movl %edx,%ds\n\t" \
	"movl %edx,%es\n\t"

#define IRQ_NAME2(nr) nr##_interrupt(void)
#define IRQ_NAME(nr) IRQ_NAME2(IRQ##nr)

#define GET_CURRENT \
	"movl ctl$gl_pcb, %ebx\n\t"

/*
 *	SMP has a few special interrupts for IPI messages
 */

	/* there is a second layer of macro just to get the symbolic
	   name for the vector evaluated. This change is for RTLinux */
#define BUILD_SMP_INTERRUPT(x,v) XBUILD_SMP_INTERRUPT(x,v)
#define XBUILD_SMP_INTERRUPT(x,v)\
asmlinkage void x(void); \
asmlinkage void call_##x(void); \
__asm__( \
"\n"__ALIGN_STR"\n" \
SYMBOL_NAME_STR(x) ":\n\t" \
	"pushl $"#v"-256\n\t" \
	INTEXC_FIX_SP(0x4) \
	SAVE_ALL \
        REGTRAP \
	SYMBOL_NAME_STR(call_##x)":\n\t" \
	"call "SYMBOL_NAME_STR(smp_##x)"\n\t" \
	"jmp ret_from_intr\n");

#define BUILD_SMP_TIMER_INTERRUPT(x,v) XBUILD_SMP_TIMER_INTERRUPT(x,v)
#define XBUILD_SMP_TIMER_INTERRUPT(x,v) \
asmlinkage void x(struct pt_regs * regs); \
asmlinkage void call_##x(void); \
__asm__( \
"\n"__ALIGN_STR"\n" \
SYMBOL_NAME_STR(x) ":\n\t" \
	"pushl $"#v"-256\n\t" \
	INTEXC_FIX_SP(0x4) \
	SAVE_ALL \
        REGTRAP \
	"movl %esp,%eax\n\t" \
	"pushl %eax\n\t" \
	SYMBOL_NAME_STR(call_##x)":\n\t" \
	"call "SYMBOL_NAME_STR(smp_##x)"\n\t" \
	"addl $4,%esp\n\t" \
	"jmp ret_from_intr\n");

#define BUILD_COMMON_IRQ() \
asmlinkage void call_do_IRQ(void); \
__asm__( \
	"\n" __ALIGN_STR"\n" \
	"common_interrupt:\n\t" \
	INTEXC_FIX_SP(0x4) \
	SAVE_ALL \
        REGTRAP \
	SYMBOL_NAME_STR(call_do_IRQ)":\n\t" \
	"call " SYMBOL_NAME_STR(do_IRQ) "\n\t" \
	"jmp ret_from_intr\n");

/* 
 * subtle. orig_eax is used by the signal code to distinct between
 * system calls and interrupted 'random user-space'. Thus we have
 * to put a negative value into orig_eax here. (the problem is that
 * both system calls and IRQs want to have small integer numbers in
 * orig_eax, and the syscall code has won the optimization conflict ;)
 *
 * Subtle as a pigs ear.  VY
 */

#define BUILD_IRQ(nr) \
asmlinkage void IRQ_NAME(nr); \
__asm__( \
"\n"__ALIGN_STR"\n" \
SYMBOL_NAME_STR(IRQ) #nr "_interrupt:\n\t" \
	"pushl $"#nr"-256\n\t" \
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
static inline void hw_resend_irq(struct hw_interrupt_type *h, unsigned int i) {
	if (IO_APIC_IRQ(i))
		send_IPI_self(IO_APIC_VECTOR(i));
}
#else
static inline void hw_resend_irq(struct hw_interrupt_type *h, unsigned int i) {}
#endif

#endif /* _ASM_HW_IRQ_H */
