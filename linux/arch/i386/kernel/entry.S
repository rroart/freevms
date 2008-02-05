# $Id$
# $Locker$

# Author. Roar Thronæs.
# Modified Linux source file, 2001-2004  

/*
 *  linux/arch/i386/entry.S
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

/*
 * entry.S contains the system-call and fault low-level handling routines.
 * This also contains the timer-interrupt handler, as well as all interrupts
 * and faults that can result in a task-switch.
 *
 * NOTE: This code handles signal-recognition, which happens every time
 * after a timer-interrupt and after each system call.
 *
 * I changed all the .align's to 4 (16 byte alignment), as that's faster
 * on a 486.
 *
 * Stack layout in 'ret_from_system_call':
 * 	ptrace needs to have all regs on the stack.
 *	if the order here is changed, it needs to be
 *	updated in fork.c:copy_process, signal.c:do_signal,
 *	ptrace.c and ptrace.h
 *
 *	 0(%esp) - %ebx
 *	 4(%esp) - %ecx
 *	 8(%esp) - %edx
 *       C(%esp) - %esi
 *	10(%esp) - %edi
 *	14(%esp) - %ebp
 *	18(%esp) - %eax
 *	1C(%esp) - %ds
 *	20(%esp) - %es
 *	24(%esp) - orig_eax
 *	28(%esp) - %eip
 *	2C(%esp) - %cs
 *	30(%esp) - %eflags
 *	34(%esp) - %oldesp
 *	38(%esp) - %oldss
 *
 * "current" is in register %ebx during any slow entries.
 */

#include <linux/config.h>
#include <linux/sys.h>
#include <linux/linkage.h>
#include <asm/segment.h>
#include <asm/smp.h>
#include <asm/offset.h>	

EBX		= 0x00
ECX		= 0x04
EDX		= 0x08
ESI		= 0x0C
EDI		= 0x10
EBP		= 0x14
EAX		= 0x18
DS		= 0x1C
ES		= 0x20
ORIG_EAX	= 0x24
EIP		= 0x28
CS		= 0x2C
EFLAGS		= 0x30
OLDESP		= 0x34
OLDSS		= 0x38

CF_MASK		= 0x00000001
IF_MASK		= 0x00000200
NT_MASK		= 0x00004000
VM_MASK		= 0x00020000

/*
 * these are offsets into the task-struct.
 */
#if 0
state		= 12    /* was 0 */
flags		= 16
sigpending	= 20
addr_limit	= 24
exec_domain	= 28
tsk_ptrace	= 36
#endif	
/*
processor	= 52
*/
ENOSYS = 38
		
#define PUSHR_ALL \
	cld; \
	pushl %es; \
	pushl %ds; \
	pushl %eax; \
	pushl %ebp; \
	pushl %edi; \
	pushl %esi; \
	pushl %edx; \
	pushl %ecx; \
	pushl %ebx;

#define POPR_ALL	\
	popl %ebx;	\
	popl %ecx;	\
	popl %edx;	\
	popl %esi;	\
	popl %edi;	\
	popl %ebp;	\
	popl %eax;	\
	popl %ds;	\
	popl %es;

#define PUSHPSL \
        PUSHR_ALL; \
        call pushpsl; \
        POPR_ALL; 

#define PUSHPSLINTR \
        PUSHR_ALL; \
        call pushpsl; \
	call setpsli; \
        POPR_ALL; 

#define PSLINTR \
        PUSHR_ALL; \
	call setpsli; \
        POPR_ALL; 

#define CLIPAN	\
	pushl %edi 		; \
	pushfl			; \
	popl %edi		; \
	andl $0x200, %edi	; \
	je 11f			; \
	call panic 		; \
11:				; \
	popl %edi
	
#define REI \
	CLIPAN			; \
        PUSHR_ALL		; \
        call myrei		; \
        POPR_ALL		; \
	CLIPAN			; \
	MAYBEDOSOFTINT($0xb,$0xa4) ; \
	MAYBEDOSOFTINT($0xa,$0xa3) ; \
	MAYBEDOSOFTINT($0x9,$0xa2) ; \
	MAYBEDOSOFTINT($0x8,$0x98) ; \
	MAYBEDOSOFTINT($0x7,$0x96) ; \
	MAYBEDOSOFTINT($0x6,$0x95) ; \
	MAYBEDOSOFTINT($0x4,$0x94) ; \
	MAYBEDOSOFTINT($0x3,$0x93) ; \
	MAYBEDOSOFTINT($0x2,$0x92) ; \
	CLIPAN			; \
	pushl %eax		; \
	pushl %edi		; \
	pushl %edx		; \
	movl %cs, %eax		; \
	andl $0x3, %eax		; \
	shl $0x2,%eax		; \
	movl ctl$gl_pcb, %edi	; \
	movl tsk_pcbl_cpu_id(%edi), %edx ; \
	shl $0x8, %edx 		; \
	addl $init_tss, %edx	; \
	addl $0x4, %edx		; /* yes, 4 */ \
	addl %eax, %edx		; /* handle mode */ \
	addl %eax, %edx		; /* handle mode */ \
	movl %esp, (%edx)	; \
	addl $0x18, (%edx)	; \
	addl $tsk_ipr_sp, %edi	; \
	addl %eax, %edi		; /* handle mode */ \
	movl %esp, (%edi)	; \
	addl $0x18, (%edi)	; \
	cmpl $0x10, 0x10(%esp)	; \
	movw %cs, %ax		; \
	cmpw %ax, 0x10(%esp)	; \
	je 3f			; \
	addl $0x8, (%edx)	; \
	addl $0x8, (%edi)	; \
3:				; \
	popl %edx		; \
	popl %edi		; \
	popl %eax		; \
	iret			; \
/* the rest made stack problems */ \
	pushl %eax		; \
	pushl %edx		; \
	call do_sw_int		; \
	popl %edx		; \
	popl %eax		; \
        iret;
/* also need to do stack changes */
/* and astlvl checks */
  
#define SAVE_ALL \
	cld; \
	pushl %es; \
	pushl %ds; \
	pushl %eax; \
	pushl %ebp; \
	pushl %edi; \
	pushl %esi; \
	pushl %edx; \
	pushl %ecx; \
	pushl %ebx; \
	movl $(__KERNEL_DS),%edx; \
	movl %edx,%ds; \
	movl %edx,%es;

#define SAVE_ALL_EXE \
	cld; \
	pushl %es; \
	pushl %ds; \
	pushl %eax; \
	pushl %ebp; \
	pushl %edi; \
	pushl %esi; \
	pushl %edx; \
	pushl %ecx; \
	pushl %ebx; \
	movl $(__EXECUTIVE_DS),%edx; \
	movl %edx,%ds; \
	movl %edx,%es;

#define RESTORE_ALL	\
	popl %ebx;	\
	popl %ecx;	\
	popl %edx;	\
	popl %esi;	\
	popl %edi;	\
	popl %ebp;	\
	popl %eax;	\
1:	popl %ds;	\
2:	popl %es;	\
	addl $4,%esp;	\
3:	REI;		\
.section .fixup,"ax";	\
4:	movl $0,(%esp);	\
	jmp 1b;		\
5:	movl $0,(%esp);	\
	jmp 2b;		\
6:	pushl %ss;	\
	popl %ds;	\
	pushl %ss;	\
	popl %es;	\
	pushl $11;	\
	call do_exit;	\
.previous;		\
.section __ex_table,"a";\
	.align 4;	\
	.long 1b,4b;	\
	.long 2b,5b;	\
	.long 3b,6b;	\
.previous

#define GET_CURRENT(reg) \
	movl ctl$gl_pcb, reg; \

#define CHM_FIX_SP(x) \
	pushl %eax		; \
	pushl %edi		; \
	pushl %edx		; \
	movl ctl$gl_pcb, %edi	; \
	movl tsk_pcbl_cpu_id(%edi), %edx ; \
	shl $0x8, %edx		; \
	addl $init_tss, %edx	; \
	addl $0x4, %edx		; \
	addl $tsk_ipr_sp, %edi	; \
	movl x+0x10(%esp), %eax	; \
	andl $0x3, %eax		; \
	je 2f			; \
	salw $2,%ax		; \
	addl %eax, %edi		; \
	addl %eax, %edx		; \
	addl %eax, %edx		; \
	cmpl $0xc, %eax 	; \
	movl x+0x18(%esp),%eax	; \
	je 4f			; \
	jmp 3f			; \
2:				; \
	movl %esp, %eax		; \
	addl $0xc, %eax		; \
3:				; \
	movl %eax, (%edx)	; \
4:				; \
	movl %eax, (%edi)	; \
	popl %edx		; \
	popl %edi		; \
	popl %eax; 

#define INTEXC_FIX_SP(x) \
	pushl %eax		; \
	pushl %edi		; \
	pushl %edx		; \
	movl ctl$gl_pcb, %edi	; \
	movl tsk_pcbl_cpu_id(%edi), %edx ; \
	shl $0x8, %edx		; \
	addl $init_tss, %edx	; \
	addl $0x4, %edx		; \
	addl $tsk_ipr_sp, %edi	; \
	/*movl %esp, (%edi);*/ /* check ekstra */ \
	/*movl %esp, (%edx);*/ /* check ekstra */ \
	/*addl $0xc, (%edi);*/ /* check ekstra */ \
	/*addl $0xc, (%edx);*/ /* check ekstra */ \
	movl x+0x10(%esp), %eax	; \
	andl $0x3, %eax		; \
	je 2f			; \
	salw $2,%ax		; \
	addl %eax, %edi		; \
	addl %eax, %edx		; \
	addl %eax, %edx		; \
	cmpl $0xc, %eax 	; \
	movl x+0x18(%esp),%eax	; \
	je 4f			; \
	jmp 3f			; \
2:				; \
	movl %esp, %eax		; \
	addl $0xc, %eax		; \
3:				; \
	movl %eax, (%edx)	; \
4:				; \
	movl %eax, (%edi)	; \
	popl %edx		; \
	popl %edi		; \
	popl %eax;
	
#define SOFTINT(name,ipl) \
	INTEXC_FIX_SP(0)	; \
        PUSHR_ALL		; \
	pushl ipl		; \
	pushl $0x0		; \
	call regtrap		; \
	addl $0x8, %esp		; \
        call SYMBOL_NAME(name)	; \
	cli			; /* temp workaround? */ \
        POPR_ALL		; \
        REI

#define LOOPCHECK \
	pushl %eax		; \
	movl 0x4(%esp), %eax	; \
	cmpl %eax, 0x34(%esp)	; \
	jne 43f			; \
	cmpl %eax, 0x64(%esp)	; \
	jne 43f			; \
	cmpl %eax, 0x94(%esp)	; \
	jne 43f			; \
	cmpl %eax, 0xc4(%esp)	; \
	jne 43f			; \
	cmpl %eax, 0xf4(%esp)	; \
	jne 43f			; \
	nop			; \
	43:			; \
	popl %eax		; 
		
#define MAYBEDOSOFTINT(ipl,vector) \
	pushl %edi		; \
	movl ctl$gl_pcb, %edi	; \
	movl tsk_pcbl_cpu_id(%edi), %edi ; \
	shl $0x2, %edi		; \
	addl $smp$gl_cpu_data, %edi; \
	movl (%edi), %edi	; \
	btw ipl, 0x6(%edi)	; /* sisr */ \
	jnc 1f			; \
	movl ctl$gl_pcb, %edi	; \
	movl tsk_psl(%edi), %edi	; \
	sarl $0x4, %edi		; \
	andl $0x1f, %edi	; \
	cmpl ipl, %edi		; \
	jae 1f			; \
	popl %edi		; \
	int vector 		; \
	jmp 2f			; \
1:				; \
	popl %edi		; \
2:				;

	
ENTRY(lcall7)
        PUSHPSL
        pushfl			# We get a different stack layout with call gates,
	pushl %eax		# which has to be cleaned up later..
	SAVE_ALL
	movl EIP(%esp),%eax	# due to call gates, this is eflags, not eip..
	movl CS(%esp),%edx	# this is eip..
	movl EFLAGS(%esp),%ecx	# and this is cs..
	movl %eax,EFLAGS(%esp)	#
	movl %edx,EIP(%esp)	# Now we move them to their "normal" places
	movl %ecx,CS(%esp)	#
	movl %esp,%ebx
	pushl %ebx
	movl ctl$gl_pcb,%ebx	# GET_CURRENT
	movl tsk_exec_domain(%ebx),%edx	# Get the execution domain
	movl 4(%edx),%edx	# Get the lcall7 handler for the domain
	pushl $0x7
	call *%edx
	addl $4, %esp
	popl %eax
	jmp ret_from_sys_call

ENTRY(lcall27)
        PUSHPSL
	pushfl			# We get a different stack layout with call gates,
	pushl %eax		# which has to be cleaned up later..
	SAVE_ALL
	movl EIP(%esp),%eax	# due to call gates, this is eflags, not eip..
	movl CS(%esp),%edx	# this is eip..
	movl EFLAGS(%esp),%ecx	# and this is cs..
	movl %eax,EFLAGS(%esp)	#
	movl %edx,EIP(%esp)	# Now we move them to their "normal" places
	movl %ecx,CS(%esp)	#
	movl %esp,%ebx
	pushl %ebx
	movl ctl$gl_pcb,%ebx	# GET_CURRENT
	movl tsk_exec_domain(%ebx),%edx	# Get the execution domain
	movl 4(%edx),%edx	# Get the lcall7 handler for the domain
	pushl $0x27
	call *%edx
	addl $4, %esp
	popl %eax
	jmp ret_from_sys_call

ENTRY(ret_from_fork)
	GET_CURRENT(%ebx)
	testb $0x02,tsk_ptrace(%ebx)	# PT_TRACESYS
	jne tracesys_exit
	jmp	ret_from_sys_call

/*
 * Return to user mode is not as complex as all this looks,
 * but we want the default path for a system call return to
 * go as quickly as possible which is why some of this is
 * less clear than it otherwise should be.
 */

ENTRY(system_call)
#        PUSHPSLINTR
	CHM_FIX_SP(0)
	pushl %eax			# save orig_eax
	SAVE_ALL
	GET_CURRENT(%ebx)
	testb $0x02,tsk_ptrace(%ebx)	# PT_TRACESYS
	jne tracesys
	cmpl $(NR_syscalls),%eax
	jae badsys
	pushl %eax
	pushl %edx
	pushl $0x0
	pushl $0x1
	call regtrap
	call setpsli	
	popl %eax
	popl %eax
	popl %edx
	popl %eax
	call *SYMBOL_NAME(sys_call_table)(,%eax,4)
	movl %eax,EAX(%esp)		# save the return value
ENTRY(ret_from_sys_call)
	cli				# need_resched and signals atomic test
	cmpl $0,tsk_sigpending(%ebx)
	jne signal_return
restore_all:
	RESTORE_ALL

	ALIGN
signal_return:
	sti				# we can get here from an interrupt handler
	testl $(VM_MASK),EFLAGS(%esp)
	movl %esp,%eax
	jne v86_signal_return
	xorl %edx,%edx
	call SYMBOL_NAME(do_signal)
	jmp restore_all

	ALIGN
v86_signal_return:
	call SYMBOL_NAME(save_v86_state)
	movl %eax,%esp
	xorl %edx,%edx
	call SYMBOL_NAME(do_signal)
	jmp restore_all

	ALIGN
tracesys:
	movl $-ENOSYS,EAX(%esp)
	call SYMBOL_NAME(syscall_trace)
	movl ORIG_EAX(%esp),%eax
	cmpl $(NR_syscalls),%eax
	jae tracesys_exit
	call *SYMBOL_NAME(sys_call_table)(,%eax,4)
	movl %eax,EAX(%esp)		# save the return value
tracesys_exit:
	call SYMBOL_NAME(syscall_trace)
	jmp ret_from_sys_call
badsys:
	movl $-ENOSYS,EAX(%esp)
	jmp ret_from_sys_call

	ALIGN
#if 0
ENTRY(common_interrupt)
	INTEXC_FIX_ISP(4)
	SAVE_ALL
	pushl %eax
	pushl %edx
	pushl $0x10
	pushl $0x0
	call regtrap
	call setpsli	
	popl %eax
	popl %eax
	popl %edx
	popl %eax
call_do_IRQ:	
	call do_IRQ
#endif	
ENTRY(ret_from_intr)
#        PUSHPSL                         # bad place. do better for new I/O
        GET_CURRENT(%ebx)
ret_from_exception:
	movl EFLAGS(%esp),%eax		# mix EFLAGS and CS
	movb CS(%esp),%al
	testl $(VM_MASK | 3),%eax	# return to VM86 mode or non-supervisor?
	jne ret_from_sys_call
	jmp restore_all

ENTRY(exe_sti)
	orl $0x200, 0x8(%esp)
	iret

ENTRY(exe_cli)
	andl $0xfffffdff, 0x8(%esp)
	iret

ENTRY(sup_sti)
	orl $0x200, 0x8(%esp)
	iret

ENTRY(sup_cli)
	andl $0xfffffdff, 0x8(%esp)
	iret

ENTRY(vmssystem_call3)
#        PUSHPSLINTR
	CHM_FIX_SP(0)
	pushl %eax			# save orig_eax
	SAVE_ALL
	GET_CURRENT(%ebx)
	testb $0x02,tsk_ptrace(%ebx)	# PT_TRACESYS
	jne vmstracesys
	cmpl $(NR_vmssyscalls3),%eax
	jae vmsbadsys
	pushl %eax
	pushl %edx
	pushl $0x3
	pushl $0x1
	call regtrap
	call setpsli	
	popl %eax
	popl %eax
	popl %edx
	popl %eax
	call *SYMBOL_NAME(vmssys_call_table3)(,%eax,4)
        jmp syscall_common

ENTRY(vmssystem_call1)
#        PUSHPSLINTR
	CHM_FIX_SP(0)
	pushl %eax			# save orig_eax
	SAVE_ALL_EXE
	GET_CURRENT(%ebx)
	testb $0x02,tsk_ptrace(%ebx)	# PT_TRACESYS
	jne vmstracesys
	cmpl $(NR_vmssyscalls1),%eax
	jae vmsbadsys
	pushl %eax
	pushl %edx
	pushl $0x1
	pushl $0x1
	call regtrap
	call setpsli	
	popl %eax
	popl %eax
	popl %edx
	popl %eax
	call *SYMBOL_NAME(vmssys_call_table1)(,%eax,4)
        jmp syscall_common

ENTRY(vmssystem_call)
#        PUSHPSLINTR
	CHM_FIX_SP(0x0)
	pushl %eax			# save orig_eax
	SAVE_ALL
	GET_CURRENT(%ebx)
	testb $0x02,tsk_ptrace(%ebx)	# PT_TRACESYS
	jne vmstracesys
	cmpl $(NR_vmssyscalls),%eax
	jae vmsbadsys
	/*
	cmpl $15,%eax
	je do_minus2
	cmpl $6,%eax
	je do_minus2
	cmpl $7,%eax
	je do_minus2
	cmpl $8,%eax
	je do_minus2
	jmp normal_call
do_minus2:
	subl $0x7,0x28(%esp)
normal_call:		
	*/
	pushl %eax
	pushl %edx
	pushl $0x0
	pushl $0x1
	call regtrap
	call setpsli	
	popl %eax
	popl %eax
	popl %edx
	popl %eax
	call *SYMBOL_NAME(vmssys_call_table)(,%eax,4)
syscall_common:
        movl %eax,EAX(%esp)		# save the return value
ENTRY(vmsret_from_sys_call)
	movl %cs, %eax
	test $3, %eax
	je 1f
	int $0xb2
	jmp 2f
1:	
	cli				# need_resched and signals atomic test
2:	
	cmpl $0,tsk_sigpending(%ebx)
	jne vmssignal_return
vmsrestore_all:
	RESTORE_ALL

	ALIGN
vmssignal_return:
	sti				# we can get here from an interrupt handler
	testl $(VM_MASK),EFLAGS(%esp)
	movl %esp,%eax
	jne vmsv86_signal_return
	xorl %edx,%edx
	call SYMBOL_NAME(do_signal)
	jmp vmsrestore_all

	ALIGN
vmsv86_signal_return:
	call SYMBOL_NAME(save_v86_state)
	movl %eax,%esp
	xorl %edx,%edx
	call SYMBOL_NAME(do_signal)
	jmp vmsrestore_all

	ALIGN
vmstracesys:
	movl $-ENOSYS,EAX(%esp)
	call SYMBOL_NAME(syscall_trace)
	movl ORIG_EAX(%esp),%eax
	cmpl $(NR_vmssyscalls),%eax
	jae vmstracesys_exit
	call *SYMBOL_NAME(vmssys_call_table)(,%eax,4)
	movl %eax,EAX(%esp)		# save the return value
vmstracesys_exit:
	call SYMBOL_NAME(syscall_trace)
	jmp vmsret_from_sys_call
vmsbadsys:
	movl $-ENOSYS,EAX(%esp)
	jmp vmsret_from_sys_call

	ALIGN
ENTRY(vmsret_from_intr)
	GET_CURRENT(%ebx)
vmsret_from_exception:
	movl EFLAGS(%esp),%eax		# mix EFLAGS and CS
	movb CS(%esp),%al
	testl $(VM_MASK | 3),%eax	# return to VM86 mode or non-supervisor?
	jne vmsret_from_sys_call
	jmp vmsrestore_all

ENTRY(divide_error)
#        PUSHPSL
	pushl $0		# no error code
	pushl $ SYMBOL_NAME(do_divide_error)
	ALIGN
error_code:
	pushl %ds
	pushl %eax
	xorl %eax,%eax
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	decl %eax			# eax = -1
	pushl %ecx
	pushl %ebx
	cld
	movl %es,%ecx
	movl ORIG_EAX(%esp), %esi	# get the error code
	movl ES(%esp), %edi		# get the function address
	movl %eax, ORIG_EAX(%esp)
	movl %ecx, ES(%esp)
	movl %esp,%edx
	pushl %eax
	pushl %edx
	pushl $0x10
	pushl $0x2
	call regtrap
	call setpsli
	popl %eax
	popl %eax
	popl %edx
	popl %eax
	pushl %esi			# push the error code
	pushl %edx			# push the pt_regs pointer
	movl $(__KERNEL_DS),%edx
	movl %edx,%ds
	movl %edx,%es
	GET_CURRENT(%ebx)
	call *%edi
	addl $8,%esp
	cli # check
	jmp ret_from_exception

ENTRY(coprocessor_error)
#        PUSHPSL
	pushl $0
	pushl $ SYMBOL_NAME(do_coprocessor_error)
	jmp error_code

ENTRY(simd_coprocessor_error)
#        PUSHPSL
	pushl $0
	pushl $ SYMBOL_NAME(do_simd_coprocessor_error)
	jmp error_code

ENTRY(device_not_available)
	cli # check
        PUSHPSL
	pushl $-1		# mark this as an int
	SAVE_ALL
	GET_CURRENT(%ebx)
	movl %cr0,%eax
	testl $0x4,%eax			# EM (math emulation bit)
	jne device_not_available_emulate
	call SYMBOL_NAME(math_state_restore)
	jmp ret_from_exception
device_not_available_emulate:
	pushl $0		# temporary storage for ORIG_EIP
	call  SYMBOL_NAME(math_emulate)
	addl $4,%esp
	jmp ret_from_exception

ENTRY(debug)
#        PUSHPSL
	pushl $0
	pushl $ SYMBOL_NAME(do_debug)
	jmp error_code

ENTRY(nmi)
        PUSHPSL
	pushl %eax
	SAVE_ALL
	movl %esp,%edx
	pushl $0
	pushl %edx
	call SYMBOL_NAME(do_nmi)
	addl $8,%esp
	RESTORE_ALL

ENTRY(int3)
#        PUSHPSL
	pushl $0
	pushl $ SYMBOL_NAME(do_int3)
	jmp error_code

ENTRY(overflow)
#        PUSHPSL
	pushl $0
	pushl $ SYMBOL_NAME(do_overflow)
	jmp error_code

ENTRY(bounds)
#        PUSHPSL
	pushl $0
	pushl $ SYMBOL_NAME(do_bounds)
	jmp error_code

ENTRY(invalid_op)
#        PUSHPSL
	pushl $0
	pushl $ SYMBOL_NAME(do_invalid_op)
	jmp error_code

ENTRY(coprocessor_segment_overrun)
#        PUSHPSL
	pushl $0
	pushl $ SYMBOL_NAME(do_coprocessor_segment_overrun)
	jmp error_code

ENTRY(double_fault)
#        PUSHPSL
	pushl $ SYMBOL_NAME(do_double_fault)
	jmp error_code

ENTRY(invalid_TSS)
#        PUSHPSL
	pushl $ SYMBOL_NAME(do_invalid_TSS)
	jmp error_code

ENTRY(segment_not_present)
#        PUSHPSL
	pushl $ SYMBOL_NAME(do_segment_not_present)
	jmp error_code

ENTRY(stack_segment)
#        PUSHPSL
	pushl $ SYMBOL_NAME(do_stack_segment)
	jmp error_code

ENTRY(general_protection)
#        PUSHPSL
	pushl $ SYMBOL_NAME(do_general_protection)
	jmp error_code

ENTRY(alignment_check)
#        PUSHPSL
	pushl $ SYMBOL_NAME(do_alignment_check)
	jmp error_code

ENTRY(page_fault)
#        PUSHPSLINTR
	INTEXC_FIX_SP(4)
	pushl $ SYMBOL_NAME(do_page_fault)
	jmp error_code

ENTRY(machine_check)
#        PUSHPSL
	pushl $0
	pushl $ SYMBOL_NAME(do_machine_check)
	jmp error_code

ENTRY(spurious_interrupt_bug)
#        PUSHPSL
	pushl $0
	pushl $ SYMBOL_NAME(do_spurious_interrupt_bug)
	jmp error_code

ENTRY(test_code)
        PUSHR_ALL
        call pushpsl
        call SYMBOL_NAME(do_test_code)
        POPR_ALL
        REI

ENTRY(sched_vector)
	SOFTINT(schedule,$0x0)
        PUSHR_ALL
#        call pushpsl
#        call SYMBOL_NAME(schedule)
	call panic
        POPR_ALL
        REI

ENTRY(resched_vector)
	SOFTINT(sch$resched,$0x3)
        PUSHR_ALL
#        call pushpsl
        call SYMBOL_NAME(sch$resched)
        POPR_ALL
        REI

ENTRY(timer_vector)
	SOFTINT(exe$swtimint,$0x7)
        PUSHR_ALL
#        call pushpsl
        call SYMBOL_NAME(exe$swtimint)
        POPR_ALL
        REI

ENTRY(timerfork_vector)
	SOFTINT(exe$swtimint,$0x7)
        PUSHR_ALL
#        call pushpsl
        call SYMBOL_NAME(exe$swtimint)
        POPR_ALL
        REI

ENTRY(iopost_vector)
	SOFTINT(ioc$iopost,$0x4)
        PUSHR_ALL
#        call pushpsl
        call SYMBOL_NAME(ioc$iopost)
        POPR_ALL
        REI

ENTRY(astdel_vector)
	SOFTINT(sch$astdel,$0x2)
        PUSHR_ALL
#        call pushpsl
        call SYMBOL_NAME(sch$astdel)
        POPR_ALL
        REI

ENTRY(queueast_vector)
	SOFTINT(exe$frkipl6dsp,$0x6)
        PUSHR_ALL
#        call pushpsl
        call SYMBOL_NAME(exe$frkipl6dsp)
        POPR_ALL
        REI

ENTRY(iolock8_vector)
	SOFTINT(exe$frkipl8dsp,$0x8)
        PUSHR_ALL
#        call pushpsl
        call SYMBOL_NAME(exe$frkipl8dsp)
        POPR_ALL
        REI

ENTRY(iolock9_vector)
	SOFTINT(exe$frkipl9dsp,$0x9)
        PUSHR_ALL
#        call pushpsl
        call SYMBOL_NAME(exe$frkipl9dsp)
        POPR_ALL
        REI

ENTRY(iolock10_vector)
	SOFTINT(exe$frkipl10dsp,$0xa)
        PUSHR_ALL
#        call pushpsl
        call SYMBOL_NAME(exe$frkipl10dsp)
        POPR_ALL
        REI

ENTRY(iolock11_vector)
	SOFTINT(exe$frkipl11dsp,$0xb)
        PUSHR_ALL
#        call pushpsl
        call SYMBOL_NAME(exe$frkipl11dsp)
        POPR_ALL
        REI
	
ENTRY(test_exe)
	iret

ENTRY(test_sup)
	iret

ENTRY(setipl)
	pushfl
	pushl %eax
	movl 0xc(%esp), %eax
	pushl %eax
	call setipl_inner
	popfl
	and %eax, %eax
	je 3f
	MAYBEDOSOFTINT($0xb,$0xa4)
	MAYBEDOSOFTINT($0xa,$0xa3)
	MAYBEDOSOFTINT($0x9,$0xa2)
	MAYBEDOSOFTINT($0x8,$0x98)
	MAYBEDOSOFTINT($0x7,$0x96)
	MAYBEDOSOFTINT($0x6,$0x95)
	MAYBEDOSOFTINT($0x4,$0x94)
	MAYBEDOSOFTINT($0x3,$0x93)
	MAYBEDOSOFTINT($0x2,$0x92)
3:
	popl %eax
	popfl
	ret
				
.data
ENTRY(sys_call_table)
	.long SYMBOL_NAME(sys_ni_syscall)	/* 0  -  old "setup()" system call*/
	.long SYMBOL_NAME(sys_exit)
	.long SYMBOL_NAME(sys_fork)
	.long SYMBOL_NAME(sys_read)
	.long SYMBOL_NAME(sys_write)
	.long SYMBOL_NAME(sys_open)		/* 5 */
	.long SYMBOL_NAME(sys_close)
	.long SYMBOL_NAME(sys_waitpid)
	.long SYMBOL_NAME(sys_creat)
	.long SYMBOL_NAME(sys_link)
	.long SYMBOL_NAME(sys_unlink)		/* 10 */
	.long SYMBOL_NAME(sys_execve)
	.long SYMBOL_NAME(sys_chdir)
	.long SYMBOL_NAME(sys_time)
	.long SYMBOL_NAME(sys_mknod)
	.long SYMBOL_NAME(sys_chmod)		/* 15 */
	.long SYMBOL_NAME(sys_lchown16)
	.long SYMBOL_NAME(sys_ni_syscall)				/* old break syscall holder */
	.long SYMBOL_NAME(sys_stat)
	.long SYMBOL_NAME(sys_lseek)
	.long SYMBOL_NAME(sys_getpid)		/* 20 */
	.long SYMBOL_NAME(sys_mount)
	.long SYMBOL_NAME(sys_oldumount)
	.long SYMBOL_NAME(sys_setuid16)
	.long SYMBOL_NAME(sys_getuid16)
	.long SYMBOL_NAME(sys_stime)		/* 25 */
	.long SYMBOL_NAME(sys_ptrace)
	.long SYMBOL_NAME(sys_alarm)
	.long SYMBOL_NAME(sys_fstat)
	.long SYMBOL_NAME(sys_pause)
	.long SYMBOL_NAME(sys_utime)		/* 30 */
	.long SYMBOL_NAME(sys_ni_syscall)				/* old stty syscall holder */
	.long SYMBOL_NAME(sys_ni_syscall)				/* old gtty syscall holder */
	.long SYMBOL_NAME(sys_access)
	.long SYMBOL_NAME(sys_nice)
	.long SYMBOL_NAME(sys_ni_syscall)	/* 35 */		/* old ftime syscall holder */
	.long SYMBOL_NAME(sys_sync)
	.long SYMBOL_NAME(sys_kill)
	.long SYMBOL_NAME(sys_rename)
	.long SYMBOL_NAME(sys_mkdir)
	.long SYMBOL_NAME(sys_rmdir)		/* 40 */
	.long SYMBOL_NAME(sys_dup)
	.long SYMBOL_NAME(sys_pipe)
	.long SYMBOL_NAME(sys_times)
	.long SYMBOL_NAME(sys_ni_syscall)				/* old prof syscall holder */
	.long SYMBOL_NAME(sys_brk)		/* 45 */
	.long SYMBOL_NAME(sys_setgid16)
	.long SYMBOL_NAME(sys_getgid16)
	.long SYMBOL_NAME(sys_signal)
	.long SYMBOL_NAME(sys_geteuid16)
	.long SYMBOL_NAME(sys_getegid16)	/* 50 */
	.long SYMBOL_NAME(sys_acct)
	.long SYMBOL_NAME(sys_umount)					/* recycled never used phys() */
	.long SYMBOL_NAME(sys_ni_syscall)				/* old lock syscall holder */
	.long SYMBOL_NAME(sys_ioctl)
	.long SYMBOL_NAME(sys_fcntl)		/* 55 */
	.long SYMBOL_NAME(sys_ni_syscall)				/* old mpx syscall holder */
	.long SYMBOL_NAME(sys_setpgid)
	.long SYMBOL_NAME(sys_ni_syscall)				/* old ulimit syscall holder */
	.long SYMBOL_NAME(sys_olduname)
	.long SYMBOL_NAME(sys_umask)		/* 60 */
	.long SYMBOL_NAME(sys_chroot)
	.long SYMBOL_NAME(sys_ustat)
	.long SYMBOL_NAME(sys_dup2)
	.long SYMBOL_NAME(sys_getppid)
	.long SYMBOL_NAME(sys_getpgrp)		/* 65 */
	.long SYMBOL_NAME(sys_setsid)
	.long SYMBOL_NAME(sys_sigaction)
	.long SYMBOL_NAME(sys_sgetmask)
	.long SYMBOL_NAME(sys_ssetmask)
	.long SYMBOL_NAME(sys_setreuid16)	/* 70 */
	.long SYMBOL_NAME(sys_setregid16)
	.long SYMBOL_NAME(sys_sigsuspend)
	.long SYMBOL_NAME(sys_sigpending)
	.long SYMBOL_NAME(sys_sethostname)
	.long SYMBOL_NAME(sys_setrlimit)	/* 75 */
	.long SYMBOL_NAME(sys_old_getrlimit)
	.long SYMBOL_NAME(sys_getrusage)
	.long SYMBOL_NAME(sys_gettimeofday)
	.long SYMBOL_NAME(sys_settimeofday)
	.long SYMBOL_NAME(sys_getgroups16)	/* 80 */
	.long SYMBOL_NAME(sys_setgroups16)
	.long SYMBOL_NAME(old_select)
	.long SYMBOL_NAME(sys_symlink)
	.long SYMBOL_NAME(sys_lstat)
	.long SYMBOL_NAME(sys_readlink)		/* 85 */
	.long SYMBOL_NAME(sys_uselib)
	.long SYMBOL_NAME(sys_swapon)
	.long SYMBOL_NAME(sys_reboot)
	.long SYMBOL_NAME(old_readdir)
	.long SYMBOL_NAME(old_mmap)		/* 90 */
	.long SYMBOL_NAME(sys_munmap)
	.long SYMBOL_NAME(sys_truncate)
	.long SYMBOL_NAME(sys_ftruncate)
	.long SYMBOL_NAME(sys_fchmod)
	.long SYMBOL_NAME(sys_fchown16)		/* 95 */
	.long SYMBOL_NAME(sys_getpriority)
	.long SYMBOL_NAME(sys_setpriority)
	.long SYMBOL_NAME(sys_ni_syscall)				/* old profil syscall holder */
	.long SYMBOL_NAME(sys_statfs)
	.long SYMBOL_NAME(sys_fstatfs)		/* 100 */
	.long SYMBOL_NAME(sys_ioperm)
	.long SYMBOL_NAME(sys_socketcall)
	.long SYMBOL_NAME(sys_syslog)
	.long SYMBOL_NAME(sys_setitimer)
	.long SYMBOL_NAME(sys_getitimer)	/* 105 */
	.long SYMBOL_NAME(sys_newstat)
	.long SYMBOL_NAME(sys_newlstat)
	.long SYMBOL_NAME(sys_newfstat)
	.long SYMBOL_NAME(sys_uname)
	.long SYMBOL_NAME(sys_iopl)		/* 110 */
	.long SYMBOL_NAME(sys_vhangup)
	.long SYMBOL_NAME(sys_ni_syscall)	/* old "idle" system call */
	.long SYMBOL_NAME(sys_vm86old)
	.long SYMBOL_NAME(sys_wait4)
	.long SYMBOL_NAME(sys_swapoff)		/* 115 */
	.long SYMBOL_NAME(sys_sysinfo)
	.long SYMBOL_NAME(sys_ipc)
	.long SYMBOL_NAME(sys_fsync)
	.long SYMBOL_NAME(sys_sigreturn)
	.long SYMBOL_NAME(sys_clone)		/* 120 */
	.long SYMBOL_NAME(sys_setdomainname)
	.long SYMBOL_NAME(sys_newuname)
	.long SYMBOL_NAME(sys_modify_ldt)
	.long SYMBOL_NAME(sys_adjtimex)
	.long SYMBOL_NAME(sys_mprotect)		/* 125 */
	.long SYMBOL_NAME(sys_sigprocmask)
	.long SYMBOL_NAME(sys_create_module)
	.long SYMBOL_NAME(sys_init_module)
	.long SYMBOL_NAME(sys_delete_module)
	.long SYMBOL_NAME(sys_get_kernel_syms)	/* 130 */
	.long SYMBOL_NAME(sys_quotactl)
	.long SYMBOL_NAME(sys_getpgid)
	.long SYMBOL_NAME(sys_fchdir)
	.long SYMBOL_NAME(sys_bdflush)
	.long SYMBOL_NAME(sys_sysfs)		/* 135 */
	.long SYMBOL_NAME(sys_personality)
	.long SYMBOL_NAME(sys_ni_syscall)	/* for afs_syscall */
	.long SYMBOL_NAME(sys_setfsuid16)
	.long SYMBOL_NAME(sys_setfsgid16)
	.long SYMBOL_NAME(sys_llseek)		/* 140 */
	.long SYMBOL_NAME(sys_getdents)
	.long SYMBOL_NAME(sys_select)
	.long SYMBOL_NAME(sys_flock)
	.long SYMBOL_NAME(sys_msync)
	.long SYMBOL_NAME(sys_readv)		/* 145 */
	.long SYMBOL_NAME(sys_writev)
	.long SYMBOL_NAME(sys_getsid)
	.long SYMBOL_NAME(sys_fdatasync)
	.long SYMBOL_NAME(sys_sysctl)
	.long SYMBOL_NAME(sys_mlock)		/* 150 */
	.long SYMBOL_NAME(sys_munlock)
	.long SYMBOL_NAME(sys_mlockall)
	.long SYMBOL_NAME(sys_munlockall)
	.long SYMBOL_NAME(sys_sched_setparam)
	.long SYMBOL_NAME(sys_sched_getparam)   /* 155 */
	.long SYMBOL_NAME(sys_sched_setscheduler)
	.long SYMBOL_NAME(sys_sched_getscheduler)
	.long SYMBOL_NAME(sys_sched_yield)
	.long SYMBOL_NAME(sys_sched_get_priority_max)
	.long SYMBOL_NAME(sys_sched_get_priority_min)  /* 160 */
	.long SYMBOL_NAME(sys_sched_rr_get_interval)
	.long SYMBOL_NAME(sys_nanosleep)
	.long SYMBOL_NAME(sys_mremap)
	.long SYMBOL_NAME(sys_setresuid16)
	.long SYMBOL_NAME(sys_getresuid16)	/* 165 */
	.long SYMBOL_NAME(sys_vm86)
	.long SYMBOL_NAME(sys_query_module)
	.long SYMBOL_NAME(sys_poll)
	.long SYMBOL_NAME(sys_nfsservctl)
	.long SYMBOL_NAME(sys_setresgid16)	/* 170 */
	.long SYMBOL_NAME(sys_getresgid16)
	.long SYMBOL_NAME(sys_prctl)
	.long SYMBOL_NAME(sys_rt_sigreturn)
	.long SYMBOL_NAME(sys_rt_sigaction)
	.long SYMBOL_NAME(sys_rt_sigprocmask)	/* 175 */
	.long SYMBOL_NAME(sys_rt_sigpending)
	.long SYMBOL_NAME(sys_rt_sigtimedwait)
	.long SYMBOL_NAME(sys_rt_sigqueueinfo)
	.long SYMBOL_NAME(sys_rt_sigsuspend)
	.long SYMBOL_NAME(sys_pread)		/* 180 */
	.long SYMBOL_NAME(sys_pwrite)
	.long SYMBOL_NAME(sys_chown16)
	.long SYMBOL_NAME(sys_getcwd)
	.long SYMBOL_NAME(sys_capget)
	.long SYMBOL_NAME(sys_capset)           /* 185 */
	.long SYMBOL_NAME(sys_sigaltstack)
	.long SYMBOL_NAME(sys_sendfile)
	.long SYMBOL_NAME(sys_ni_syscall)		/* streams1 */
	.long SYMBOL_NAME(sys_ni_syscall)		/* streams2 */
	.long SYMBOL_NAME(sys_vfork)            /* 190 */
	.long SYMBOL_NAME(sys_getrlimit)
	.long SYMBOL_NAME(sys_mmap2)
	.long SYMBOL_NAME(sys_truncate64)
	.long SYMBOL_NAME(sys_ftruncate64)
	.long SYMBOL_NAME(sys_stat64)		/* 195 */
	.long SYMBOL_NAME(sys_lstat64)
	.long SYMBOL_NAME(sys_fstat64)
	.long SYMBOL_NAME(sys_lchown)
	.long SYMBOL_NAME(sys_getuid)
	.long SYMBOL_NAME(sys_getgid)		/* 200 */
	.long SYMBOL_NAME(sys_geteuid)
	.long SYMBOL_NAME(sys_getegid)
	.long SYMBOL_NAME(sys_setreuid)
	.long SYMBOL_NAME(sys_setregid)
	.long SYMBOL_NAME(sys_getgroups)	/* 205 */
	.long SYMBOL_NAME(sys_setgroups)
	.long SYMBOL_NAME(sys_fchown)
	.long SYMBOL_NAME(sys_setresuid)
	.long SYMBOL_NAME(sys_getresuid)
	.long SYMBOL_NAME(sys_setresgid)	/* 210 */
	.long SYMBOL_NAME(sys_getresgid)
	.long SYMBOL_NAME(sys_chown)
	.long SYMBOL_NAME(sys_setuid)
	.long SYMBOL_NAME(sys_setgid)
	.long SYMBOL_NAME(sys_setfsuid)		/* 215 */
	.long SYMBOL_NAME(sys_setfsgid)
	.long SYMBOL_NAME(sys_pivot_root)
	.long SYMBOL_NAME(sys_mincore)
	.long SYMBOL_NAME(sys_madvise)
	.long SYMBOL_NAME(sys_getdents64)	/* 220 */
	.long SYMBOL_NAME(sys_fcntl64)
	.long SYMBOL_NAME(sys_ni_syscall)	/* reserved for TUX */
	.long SYMBOL_NAME(sys_ni_syscall)	/* Reserved for Security */
	.long SYMBOL_NAME(sys_gettid)
	.long SYMBOL_NAME(sys_readahead)	/* 225 */
	.long SYMBOL_NAME(sys_ni_syscall)	/* reserved for setxattr */
	.long SYMBOL_NAME(sys_ni_syscall)	/* reserved for lsetxattr */
	.long SYMBOL_NAME(sys_ni_syscall)	/* reserved for fsetxattr */
	.long SYMBOL_NAME(sys_ni_syscall)	/* reserved for getxattr */
	.long SYMBOL_NAME(sys_ni_syscall)	/* 230 reserved for lgetxattr */
	.long SYMBOL_NAME(sys_ni_syscall)	/* reserved for fgetxattr */
	.long SYMBOL_NAME(sys_ni_syscall)	/* reserved for listxattr */
	.long SYMBOL_NAME(sys_ni_syscall)	/* reserved for llistxattr */
	.long SYMBOL_NAME(sys_ni_syscall)	/* reserved for flistxattr */
	.long SYMBOL_NAME(sys_ni_syscall)	/* 235 reserved for removexattr */
	.long SYMBOL_NAME(sys_ni_syscall)	/* reserved for lremovexattr */
	.long SYMBOL_NAME(sys_ni_syscall)	/* reserved for fremovexattr */

	.rept NR_syscalls-(.-sys_call_table)/4
		.long SYMBOL_NAME(sys_ni_syscall)
.endr

ENTRY(vmssys_call_table)
	.long SYMBOL_NAME(sys_$ni_syscall)	/* 0  -  old "setup()" system call*/
	.long SYMBOL_NAME(sys_$testtest)
	.long SYMBOL_NAME(exe$setpri_wrap)
        .long SYMBOL_NAME(exe$crelnt_wrap)
        .long SYMBOL_NAME(exe$setprn)
        .long SYMBOL_NAME(exe$dclast)
        .long SYMBOL_NAME(exe$waitfr)
        .long SYMBOL_NAME(exe$wflor)
        .long SYMBOL_NAME(exe$wfland)
        .long SYMBOL_NAME(exe$clref)
        .long SYMBOL_NAME(exe$setime)
        .long SYMBOL_NAME(exe$setimr)
        .long SYMBOL_NAME(exe$cantim)
        .long SYMBOL_NAME(exe$numtim)
        .long SYMBOL_NAME(exe$gettim)
        .long SYMBOL_NAME(exe$hiber)
        .long SYMBOL_NAME(exe$wake)
        .long SYMBOL_NAME(exe$schdwk)
        .long SYMBOL_NAME(exe$canwak)
        .long SYMBOL_NAME(exe$suspnd)
        .long SYMBOL_NAME(exe$resume)
        .long SYMBOL_NAME(exe$exit)
        .long SYMBOL_NAME(exe$forcex)
        .long SYMBOL_NAME(exe$setrwm)
        .long SYMBOL_NAME(exe$delprc)
        .long SYMBOL_NAME(exe$readef)
        .long SYMBOL_NAME(exe$setef)
        .long SYMBOL_NAME(exe$synch)
        .long SYMBOL_NAME(exe$enq_wrap)
        .long SYMBOL_NAME(exe$deq)
        .long SYMBOL_NAME(exe$assign)
        .long SYMBOL_NAME(exe$dassgn)
        .long SYMBOL_NAME(exe$qio_wrap)
        .long SYMBOL_NAME(exe$qiow_wrap)
        .long SYMBOL_NAME(exe$getlki_wrap)
        .long SYMBOL_NAME(exe$getlkiw_wrap)
        .long SYMBOL_NAME(exe$enqw_wrap) 
        .long SYMBOL_NAME(exe$crelnm) 
        .long SYMBOL_NAME(exe$trnlnm) 
        .long SYMBOL_NAME(exe$dellnm) 
        .long SYMBOL_NAME(exe$clrast) 
        .long SYMBOL_NAME(exe$setast) 
        .long SYMBOL_NAME(exe$ascefc) 
        .long SYMBOL_NAME(exe$dacefc) 
        .long SYMBOL_NAME(exe$dlcefc) 
        .long SYMBOL_NAME(exe$crembx_wrap) 
        .long SYMBOL_NAME(exe$delmbx) 
        .long SYMBOL_NAME(exe$mount) 
	.long SYMBOL_NAME(exe$cretva)
	.long SYMBOL_NAME(exe$expreg)
	.long SYMBOL_NAME(exe$crmpsc_wrap)
	.long SYMBOL_NAME(exe$mgblsc_wrap)
	.long SYMBOL_NAME(exe$deltva)
	.long SYMBOL_NAME(exe$cntreg)
	.long SYMBOL_NAME(exe$dgblsc)
	.long SYMBOL_NAME(exe$setswm)
	.long SYMBOL_NAME(exe$setprt)
	.long SYMBOL_NAME(exe$adjwsl)
	.long SYMBOL_NAME(exe$lkwset)
	.long SYMBOL_NAME(exe$lckpag)
	.long SYMBOL_NAME(exe$ulwset)
	.long SYMBOL_NAME(exe$ulkpag)
	.long SYMBOL_NAME(exe$purgws)
	.long SYMBOL_NAME(exe$creprc_wrap          )
	.long SYMBOL_NAME(exe$imgact_wrap          )
	.long SYMBOL_NAME(exe$imgfix          )
	.long SYMBOL_NAME(exe$imgsta_wrap          )
	.long SYMBOL_NAME(exe$dclexh          )
	.long SYMBOL_NAME(exe$rundwn          )
	.long SYMBOL_NAME(exe$sndjbc          )
	.long SYMBOL_NAME(exe$sndjbcw         )
	.long SYMBOL_NAME(exe$getqui          )
	.long SYMBOL_NAME(exe$getmsg          )
	.long SYMBOL_NAME(exe$putmsg          )
	.long SYMBOL_NAME(exe$excmsg          )
	.long SYMBOL_NAME(exe$getsyi_wrap          )
	.long SYMBOL_NAME(exe$getsyiw_wrap         )
	.long SYMBOL_NAME(exe$device_scan     )
	.long SYMBOL_NAME(exe$getdvi_wrap     )
	.long SYMBOL_NAME(exe$fao             )
	.long SYMBOL_NAME(exe$faol            )
	.long SYMBOL_NAME(exe$cmkrnl          )
	.long SYMBOL_NAME(exe$getjpi_wrap     )
	.long SYMBOL_NAME(exe$updsec          )
	.long SYMBOL_NAME(exe$resched         )
	.long SYMBOL_NAME(exe$setexv          )
	.long SYMBOL_NAME(exe$check_access    )
	.long SYMBOL_NAME(exe$getuai_wrap     )
	.long SYMBOL_NAME(exe$setuai          )
	.long SYMBOL_NAME(exe$idtoasc         )
	.long SYMBOL_NAME(exe$asctoid         )
	.long SYMBOL_NAME(exe$add_ident       )
	.long SYMBOL_NAME(exe$rem_ident       )
	.long SYMBOL_NAME(exe$find_held       )
	.long SYMBOL_NAME(exe$find_holder     )
	.long SYMBOL_NAME(exe$mod_ident       )
	.long SYMBOL_NAME(exe$mod_holder      )
	.long SYMBOL_NAME(exe$grantid      )
	.long SYMBOL_NAME(exe$revokid      )
	.long SYMBOL_NAME(exe$create_region_32_wrap)
	.long SYMBOL_NAME(exe$delete_region_32)
	.long SYMBOL_NAME(exe$get_region_info)
	.long SYMBOL_NAME(exe$getdviw_wrap)
	.long SYMBOL_NAME(exe$cli)
	.long SYMBOL_NAME(exe$setprv)
	.long SYMBOL_NAME(exe$cancel)
	.long SYMBOL_NAME(exe$getjpiw_wrap)
	.long SYMBOL_NAME(exe$sndopr)
	.long SYMBOL_NAME(exe$process_affinity_wrap)
	.long SYMBOL_NAME(exe$set_implicit_affinity)
	.long SYMBOL_NAME(exe$cpu_capabilities)
	.long SYMBOL_NAME(exe$process_capabilities_wrap)
	.long SYMBOL_NAME(exe$set_security_wrap)
	.long SYMBOL_NAME(exe$get_security_wrap)
	
	.rept NR_vmssyscalls-(.-vmssys_call_table)/4
		.long SYMBOL_NAME(sys_$ni_syscall)

  .endr

ENTRY(vmssys_call_table1)
        .long SYMBOL_NAME(sys_$ni_syscall1)
	.long SYMBOL_NAME(exe$close)
	.long SYMBOL_NAME(exe$connect)
	.long SYMBOL_NAME(exe$create)
	.long SYMBOL_NAME(exe$delete)
	.long SYMBOL_NAME(exe$disconnect)
	.long SYMBOL_NAME(exe$display)
	.long SYMBOL_NAME(exe$enter)
	.long SYMBOL_NAME(exe$erase)
	.long SYMBOL_NAME(exe$extend)
	.long SYMBOL_NAME(exe$find)
	.long SYMBOL_NAME(exe$flush)
	.long SYMBOL_NAME(exe$free)
	.long SYMBOL_NAME(exe$get)
	.long SYMBOL_NAME(exe$modify)
	.long SYMBOL_NAME(exe$nxtvol)
	.long SYMBOL_NAME(exe$open)
	.long SYMBOL_NAME(exe$parse)
	.long SYMBOL_NAME(exe$put)
	.long SYMBOL_NAME(exe$read)
	.long SYMBOL_NAME(exe$release)
	.long SYMBOL_NAME(exe$remove)
	.long SYMBOL_NAME(exe$rename)
	.long SYMBOL_NAME(exe$rewind)
	.long SYMBOL_NAME(exe$search)
	.long SYMBOL_NAME(exe$space)
	.long SYMBOL_NAME(exe$truncate)
	.long SYMBOL_NAME(exe$update)
	.long SYMBOL_NAME(exe$wait)
	.long SYMBOL_NAME(exe$write)
	.long SYMBOL_NAME(exe$filescan)
	.long SYMBOL_NAME(exe$setddir)
	.long SYMBOL_NAME(exe$setdfprot)
	.long SYMBOL_NAME(exe$ssvexc)
	.long SYMBOL_NAME(exe$rmsrundwn)
	.long SYMBOL_NAME(exe$cmexec)
	.rept NR_vmssyscalls1-(.-vmssys_call_table1)/4
		.long SYMBOL_NAME(sys_$ni_syscall1)

  .endr


ENTRY(vmssys_call_table3)
	.long SYMBOL_NAME(sys_$ni_syscall3)	/* 0  -  old "setup()" system call*/
        .long SYMBOL_NAME(exe$asctim)
        .long SYMBOL_NAME(exe$bintim)

	.rept NR_vmssyscalls3-(.-vmssys_call_table3)/4
		.long SYMBOL_NAME(sys_$ni_syscall3)

  .endr



