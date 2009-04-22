/* Copyright (C) 1992, 93, 95, 96, 97, 98, 99, 00 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper, <drepper@gnu.org>, August 1995.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#ifdef compile_um
#include "sysdepum.h"
#else

#ifndef _LINUX_I386_SYSDEP_H
#define _LINUX_I386_SYSDEP_H 1

/* There is some commonality.  */
//#include <sysdeps/unix/i386/sysdep.h>
//#include <bp-sym.h>
//#include <bp-asm.h>

#ifdef __i386__
/* For Linux we can use the system call table in the header file
	/usr/include/asm/unistd.h
   of the kernel.  But these symbols do not follow the SYS_* syntax
   so we have to redefine the `SYS_ify' macro here.  */
#undef SYS_ify
#define SYS_ify(syscall_name)	__NR_##syscall_name

/* ELF-like local names start with `.L'.  */
#undef L
#define L(name)	.L##name

#ifdef __ASSEMBLER__

/* Linux uses a negative return value to indicate syscall errors,
   unlike most Unices, which use the condition codes' carry flag.

   Since version 2.1 the return value of a system call might be
   negative even if the call succeeded.  E.g., the `lseek' system call
   might return a large offset.  Therefore we must not anymore test
   for < 0, but test for a real error by making sure the value in %eax
   is a real error number.  Linus said he will make sure the no syscall
   returns a value in -1 .. -4095 as a valid result so we can savely
   test with -4095.  */

/* We don't want the label for the error handle to be global when we define
   it here.  */
#ifdef PIC
# define SYSCALL_ERROR_LABEL 0f
#else
# define SYSCALL_ERROR_LABEL syscall_error
#endif

#undef	PSEUDO
#define	PSEUDO(name, syscall_name, args)				      \
  .text;								      \
  ENTRY (name)								      \
    DO_CALL (args, syscall_name);					      \
    cmpl $-4095, %eax;							      \
    jae SYSCALL_ERROR_LABEL;						      \
  L(pseudo_end):

#undef	PSEUDO_END
#define	PSEUDO_END(name)						      \
  SYSCALL_ERROR_HANDLER							      \
  END (name)

#ifndef PIC
#define SYSCALL_ERROR_HANDLER	/* Nothing here; code in sysdep.S is used.  */
#else
/* Store (- %eax) into errno through the GOT.  */
#ifdef _LIBC_REENTRANT
#define SYSCALL_ERROR_HANDLER						      \
0:pushl %ebx;								      \
  call 1f;								      \
1:popl %ebx;								      \
  xorl %edx, %edx;							      \
  addl $_GLOBAL_OFFSET_TABLE_+[.-1b], %ebx;				      \
  subl %eax, %edx;							      \
  pushl %edx;								      \
  PUSH_ERRNO_LOCATION_RETURN;						      \
  call BP_SYM (__errno_location)@PLT;					      \
  POP_ERRNO_LOCATION_RETURN;						      \
  popl %ecx;								      \
  popl %ebx;								      \
  movl %ecx, (%eax);							      \
  orl $-1, %eax;							      \
  jmp L(pseudo_end);
/* A quick note: it is assumed that the call to `__errno_location' does
   not modify the stack!  */
#else
#define SYSCALL_ERROR_HANDLER						      \
0:call 1f;								      \
1:popl %ecx;								      \
  xorl %edx, %edx;							      \
  addl $_GLOBAL_OFFSET_TABLE_+[.-1b], %ecx;				      \
  subl %eax, %edx;							      \
  movl errno@GOT(%ecx), %ecx;						      \
  movl %edx, (%ecx);							      \
  orl $-1, %eax;							      \
  jmp L(pseudo_end);
#endif	/* _LIBC_REENTRANT */
#endif	/* PIC */

/* Linux takes system call arguments in registers:

	syscall number	%eax	     call-clobbered
	arg 1		%ebx	     call-saved
	arg 2		%ecx	     call-clobbered
	arg 3		%edx	     call-clobbered
	arg 4		%esi	     call-saved
	arg 5		%edi	     call-saved

   The stack layout upon entering the function is:

	20(%esp)	Arg# 5
	16(%esp)	Arg# 4
	12(%esp)	Arg# 3
	 8(%esp)	Arg# 2
	 4(%esp)	Arg# 1
	  (%esp)	Return address

   (Of course a function with say 3 arguments does not have entries for
   arguments 4 and 5.)

   The following code tries hard to be optimal.  A general assumption
   (which is true according to the data books I have) is that

	2 * xchg	is more expensive than	pushl + movl + popl

   Beside this a neat trick is used.  The calling conventions for Linux
   tell that among the registers used for parameters %ecx and %edx need
   not be saved.  Beside this we may clobber this registers even when
   they are not used for parameter passing.

   As a result one can see below that we save the content of the %ebx
   register in the %edx register when we have less than 3 arguments
   (2 * movl is less expensive than pushl + popl).

   Second unlike for the other registers we don't save the content of
   %ecx and %edx when we have more than 1 and 2 registers resp.

   The code below might look a bit long but we have to take care for
   the pipelined processors (i586).  Here the `pushl' and `popl'
   instructions are marked as NP (not pairable) but the exception is
   two consecutive of these instruction.  This gives no penalty on
   other processors though.  */

#undef	DO_CALL
#define DO_CALL(args, syscall_name)			      		      \
    PUSHARGS_##args							      \
    DOARGS_##args							      \
    movl $SYS_ify (syscall_name), %eax;					      \
    int $0x81								      \
    POPARGS_##args

#define PUSHARGS_0	/* No arguments to push.  */
#define	DOARGS_0	/* No arguments to frob.  */
#define	POPARGS_0	/* No arguments to pop.  */
#define	_PUSHARGS_0	/* No arguments to push.  */
#define _DOARGS_0(n)	/* No arguments to frob.  */
#define	_POPARGS_0	/* No arguments to pop.  */

#define PUSHARGS_1	movl %ebx, %edx; PUSHARGS_0
#define	DOARGS_1	_DOARGS_1 (4)
#define	POPARGS_1	POPARGS_0; movl %edx, %ebx
#define	_PUSHARGS_1	pushl %ebx; _PUSHARGS_0
#define _DOARGS_1(n)	movl n(%esp), %ebx; _DOARGS_0(n-4)
#define	_POPARGS_1	_POPARGS_0; popl %ebx

#define PUSHARGS_2	PUSHARGS_1
#define	DOARGS_2	_DOARGS_2 (8)
#define	POPARGS_2	POPARGS_1
#define _PUSHARGS_2	_PUSHARGS_1
#define	_DOARGS_2(n)	movl n(%esp), %ecx; _DOARGS_1 (n-4)
#define	_POPARGS_2	_POPARGS_1

#define PUSHARGS_3	_PUSHARGS_2
#define DOARGS_3	_DOARGS_3 (16)
#define POPARGS_3	_POPARGS_3
#define _PUSHARGS_3	_PUSHARGS_2
#define _DOARGS_3(n)	movl n(%esp), %edx; _DOARGS_2 (n-4)
#define _POPARGS_3	_POPARGS_2

#define PUSHARGS_4	_PUSHARGS_4
#define DOARGS_4	_DOARGS_4 (24)
#define POPARGS_4	_POPARGS_4
#define _PUSHARGS_4	pushl %esi; _PUSHARGS_3
#define _DOARGS_4(n)	movl n(%esp), %esi; _DOARGS_3 (n-4)
#define _POPARGS_4	_POPARGS_3; popl %esi

#define PUSHARGS_5	_PUSHARGS_5
#define DOARGS_5	_DOARGS_5 (32)
#define POPARGS_5	_POPARGS_5
#define _PUSHARGS_5	pushl %edi; _PUSHARGS_4
#define _DOARGS_5(n)	movl n(%esp), %edi; _DOARGS_4 (n-4)
#define _POPARGS_5	_POPARGS_4; popl %edi

#else	/* !__ASSEMBLER__ */

/* We need some help from the assembler to generate optimal code.  We
   define some macros here which later will be used.  */
asm (".L__X'%ebx = 1\n\t"
     ".L__X'%ecx = 2\n\t"
     ".L__X'%edx = 2\n\t"
     ".L__X'%eax = 3\n\t"
     ".L__X'%esi = 3\n\t"
     ".L__X'%edi = 3\n\t"
     ".L__X'%ebp = 3\n\t"
     ".L__X'%esp = 3\n\t"
     ".macro bpushl name reg\n\t"
     ".if 1 - \\name\n\t"
     ".if 2 - \\name\n\t"
     "pushl %ebx\n\t"
     ".else\n\t"
     "xchgl \\reg, %ebx\n\t"
     ".endif\n\t"
     ".endif\n\t"
     ".endm\n\t"
     ".macro bpopl name reg\n\t"
     ".if 1 - \\name\n\t"
     ".if 2 - \\name\n\t"
     "popl %ebx\n\t"
     ".else\n\t"
     "xchgl \\reg, %ebx\n\t"
     ".endif\n\t"
     ".endif\n\t"
     ".endm\n\t"
     ".macro bmovl name reg\n\t"
     ".if 1 - \\name\n\t"
     ".if 2 - \\name\n\t"
     "movl \\reg, %ebx\n\t"
     ".endif\n\t"
     ".endif\n\t"
     ".endm\n\t");

//extern int errno;
#ifndef __KERNEL__
#include <errno.h>
#else
extern int kernel_errno;
#define errno kernel_errno
#endif

/* Define a macro which expands inline into the wrapper code for a system
   call.  */
#undef INLINE_SYSCALL
#define INLINE_SYSCALL(name, nr, args...) \
  ({									      \
    unsigned int resultvar;						      \
    asm volatile (							      \
    LOADARGS_##nr							      \
    "movl %1, %%eax\n\t"						      \
    "int $0x81\n\t"							      \
    RESTOREARGS_##nr							      \
    : "=a" (resultvar)							      \
    : "i" (__NR_##name) ASMFMT_##nr(args) : "memory", "cc");		      \
    if (resultvar >= 0xfffff001)					      \
      {									      \
	errno= (-resultvar);					      \
	resultvar = 0xffffffff;						      \
      }									      \
    (int) resultvar; })

#undef INLINE_SYSCALL1
#define INLINE_SYSCALL1(name, nr, args...) \
  ({									      \
    unsigned int resultvar;						      \
    asm volatile (							      \
    LOADARGS_##nr							      \
    "movl %1, %%eax\n\t"						      \
    "int $0x82\n\t"							      \
    RESTOREARGS_##nr							      \
    : "=a" (resultvar)							      \
    : "i" (__NR_##name) ASMFMT_##nr(args) : "memory", "cc");		      \
    if (resultvar >= 0xfffff001)					      \
      {									      \
	errno= (-resultvar);					      \
	resultvar = 0xffffffff;						      \
      }									      \
    (int) resultvar; })

#undef INLINE_SYSCALL3
#define INLINE_SYSCALL3(name, nr, args...) \
  ({									      \
    unsigned int resultvar;						      \
    asm volatile (							      \
    LOADARGS_##nr							      \
    "movl %1, %%eax\n\t"						      \
    "int $0x84\n\t"							      \
    RESTOREARGS_##nr							      \
    : "=a" (resultvar)							      \
    : "i" (__NR_##name) ASMFMT_##nr(args) : "memory", "cc");		      \
    if (resultvar >= 0xfffff001)					      \
      {									      \
	errno= (-resultvar);					      \
	resultvar = 0xffffffff;						      \
      }									      \
    (int) resultvar; })

#undef INLINE_SYSCALLTEST
#define INLINE_SYSCALLTEST(name, nr, args...) \
  ({									      \
    unsigned int resultvar;						      \
    asm volatile (							      \
    LOADARGS_##nr							      \
    "movl %1, %%eax\n\t"						      \
    "call 0xc0100010\n\t"						      \
    RESTOREARGS_##nr							      \
    : "=a" (resultvar)							      \
    : "i" (__NR_##name) ASMFMT_##nr(args) : "memory", "cc");		      \
    if (resultvar >= 0xfffff001)					      \
      {									      \
	errno= (-resultvar);					      \
	resultvar = 0xffffffff;						      \
      }									      \
    (int) resultvar; })

#define LOADARGS_0
#define LOADARGS_1 \
    "bpushl .L__X'%k2, %k2\n\t"						      \
    "bmovl .L__X'%k2, %k2\n\t"
#define LOADARGS_2	LOADARGS_1
#define LOADARGS_3	LOADARGS_1
#define LOADARGS_4	LOADARGS_1
#define LOADARGS_5	LOADARGS_1

#define RESTOREARGS_0
#define RESTOREARGS_1 \
    "bpopl .L__X'%k2, %k2\n\t"
#define RESTOREARGS_2	RESTOREARGS_1
#define RESTOREARGS_3	RESTOREARGS_1
#define RESTOREARGS_4	RESTOREARGS_1
#define RESTOREARGS_5	RESTOREARGS_1

#define ASMFMT_0()
#define ASMFMT_1(arg1) \
	, "acdSD" (arg1)
#define ASMFMT_2(arg1, arg2) \
	, "adCD" (arg1), "c" (arg2)
#define ASMFMT_3(arg1, arg2, arg3) \
	, "aCD" (arg1), "c" (arg2), "d" (arg3)
#define ASMFMT_4(arg1, arg2, arg3, arg4) \
	, "aD" (arg1), "c" (arg2), "d" (arg3), "S" (arg4)
#define ASMFMT_5(arg1, arg2, arg3, arg4, arg5) \
	, "a" (arg1), "c" (arg2), "d" (arg3), "S" (arg4), "D" (arg5)

#endif	/* __ASSEMBLER__ */

#endif /* __i386__ */
#ifdef __x86_64__
#if 1
/* For Linux we can use the system call table in the header file
	/usr/include/asm/unistd.h
   of the kernel.  But these symbols do not follow the SYS_* syntax
   so we have to redefine the `SYS_ify' macro here.  */
#undef SYS_ify
#define SYS_ify(syscall_name)	__NR_##syscall_name

/* This is a kludge to make syscalls.list find these under the names
   pread and pwrite, since some kernel headers define those names
   and some define the *64 names for the same system calls.  */
#if !defined __NR_pread && defined __NR_pread64
# define __NR_pread __NR_pread64
#endif
#if !defined __NR_pwrite && defined __NR_pwrite64
# define __NR_pwrite __NR_pwrite64
#endif

/* This is to help the old kernel headers where __NR_semtimedop is not
   available.  */
#ifndef __NR_semtimedop
# define __NR_semtimedop 220
#endif


#ifdef __ASSEMBLER__

/* Linux uses a negative return value to indicate syscall errors,
   unlike most Unices, which use the condition codes' carry flag.

   Since version 2.1 the return value of a system call might be
   negative even if the call succeeded.	 E.g., the `lseek' system call
   might return a large offset.	 Therefore we must not anymore test
   for < 0, but test for a real error by making sure the value in %eax
   is a real error number.  Linus said he will make sure the no syscall
   returns a value in -1 .. -4095 as a valid result so we can savely
   test with -4095.  */

/* We don't want the label for the error handle to be global when we define
   it here.  */
#ifdef PIC
# define SYSCALL_ERROR_LABEL 0f
#else
# define SYSCALL_ERROR_LABEL syscall_error
#endif

#undef	PSEUDO
#define	PSEUDO(name, syscall_name, args)				      \
  .text;								      \
  ENTRY (name)								      \
    DO_CALL (syscall_name, args);					      \
    cmpq $-4095, %rax;							      \
    jae SYSCALL_ERROR_LABEL;						      \
  L(pseudo_end):

#undef	PSEUDO_END
#define	PSEUDO_END(name)						      \
  SYSCALL_ERROR_HANDLER							      \
  END (name)

#undef	PSEUDO_NOERRNO
#define	PSEUDO_NOERRNO(name, syscall_name, args) \
  .text;								      \
  ENTRY (name)								      \
    DO_CALL (syscall_name, args)

#undef	PSEUDO_END_NOERRNO
#define	PSEUDO_END_NOERRNO(name) \
  END (name)

#define ret_NOERRNO ret

#undef	PSEUDO_ERRVAL
#define	PSEUDO_ERRVAL(name, syscall_name, args) \
  .text;								      \
  ENTRY (name)								      \
    DO_CALL (syscall_name, args);					      \
    negq %rax

#undef	PSEUDO_END_ERRVAL
#define	PSEUDO_END_ERRVAL(name) \
  END (name)

#define ret_ERRVAL ret

#ifndef PIC
#define SYSCALL_ERROR_HANDLER	/* Nothing here; code in sysdep.S is used.  */
#elif RTLD_PRIVATE_ERRNO
# define SYSCALL_ERROR_HANDLER			\
0:						\
  leaq errno(%rip), %rcx;			\
  xorq %rdx, %rdx;				\
  subq %rax, %rdx;				\
  movl %edx, (%rcx);				\
  orq $-1, %rax;				\
  jmp L(pseudo_end);
#elif USE___THREAD
# ifndef NOT_IN_libc
#  define SYSCALL_ERROR_ERRNO __libc_errno
# else
#  define SYSCALL_ERROR_ERRNO errno
# endif
# define SYSCALL_ERROR_HANDLER			\
0:						\
  movq SYSCALL_ERROR_ERRNO@GOTTPOFF(%rip), %rcx;\
  xorq %rdx, %rdx;				\
  subq %rax, %rdx;				\
  movl %edx, %fs:(%rcx);			\
  orq $-1, %rax;				\
  jmp L(pseudo_end);
#elif defined _LIBC_REENTRANT
/* Store (- %rax) into errno through the GOT.
   Note that errno occupies only 4 bytes.  */
# define SYSCALL_ERROR_HANDLER			\
0:						\
  xorq %rdx, %rdx;				\
  subq %rax, %rdx;				\
  pushq %rdx;					\
  cfi_adjust_cfa_offset(8);			\
  PUSH_ERRNO_LOCATION_RETURN;			\
  call BP_SYM (__errno_location)@PLT;		\
  POP_ERRNO_LOCATION_RETURN;			\
  popq %rdx;					\
  cfi_adjust_cfa_offset(-8);			\
  movl %edx, (%rax);				\
  orq $-1, %rax;				\
  jmp L(pseudo_end);

/* A quick note: it is assumed that the call to `__errno_location' does
   not modify the stack!  */
#else /* Not _LIBC_REENTRANT.  */
# define SYSCALL_ERROR_HANDLER			\
0:movq errno@GOTPCREL(%RIP), %rcx;		\
  xorq %rdx, %rdx;				\
  subq %rax, %rdx;				\
  movl %edx, (%rcx);				\
  orq $-1, %rax;				\
  jmp L(pseudo_end);
#endif	/* PIC */

/* The Linux/x86-64 kernel expects the system call parameters in
   registers according to the following table:

    syscall number	rax
    arg 1		rdi
    arg 2		rsi
    arg 3		rdx
    arg 4		r10
    arg 5		r8
    arg 6		r9

    The Linux kernel uses and destroys internally these registers:
    return address from
    syscall		rcx
    additionally clobered: r12-r15,rbx,rbp
    eflags from syscall	r11

    Normal function call, including calls to the system call stub
    functions in the libc, get the first six parameters passed in
    registers and the seventh parameter and later on the stack.  The
    register use is as follows:

     system call number	in the DO_CALL macro
     arg 1		rdi
     arg 2		rsi
     arg 3		rdx
     arg 4		rcx
     arg 5		r8
     arg 6		r9

    We have to take care that the stack is aligned to 16 bytes.  When
    called the stack is not aligned since the return address has just
    been pushed.


    Syscalls of more than 6 arguments are not supported.  */

#undef	DO_CALL
#define DO_CALL(syscall_name, args)		\
    DOARGS_##args				\
    movq $SYS_ify (syscall_name), %rax;		\
    syscall;

#define DOARGS_0 /* nothing */
#define DOARGS_1 /* nothing */
#define DOARGS_2 /* nothing */
#define DOARGS_3 /* nothing */
#define DOARGS_4 movq %rcx, %r10;
#define DOARGS_5 DOARGS_4
#define DOARGS_6 DOARGS_5

#else	/* !__ASSEMBLER__ */
/* Define a macro which expands inline into the wrapper code for a system
   call.  */
#undef INLINE_SYSCALL
#define INLINE_SYSCALL(name, nr, args...) \
  ({									      \
    unsigned long resultvar = INTERNAL_SYSCALL (name, , nr, args);	      \
    if (__builtin_expect (INTERNAL_SYSCALL_ERROR_P (resultvar, ), 0))	      \
      {									      \
	__set_errno (INTERNAL_SYSCALL_ERRNO (resultvar, ));		      \
	resultvar = (unsigned long) -1;					      \
      }									      \
    (long) resultvar; })

#undef INTERNAL_SYSCALL_DECL
#define INTERNAL_SYSCALL_DECL(err) do { } while (0)

#undef INLINE_SYSCALL
#undef INTERNAL_SYSCALL
#define INLINE_SYSCALL(name, nr, args...) \
  ({									      \
    unsigned long resultvar;						      \
    LOAD_ARGS_##nr (args)						      \
    asm volatile (							      \
    "movq %1, %%rax\n\t"						      \
    "int $0x81\n\t"							      \
    : "=a" (resultvar)							      \
    : "i" (__NR_##name) ASM_ARGS_##nr : "memory", "cc", "r11", "cx");	      \
    (long) resultvar; })

#define INLINE_SYSCALL1(name, nr, args...) \
  ({									      \
    unsigned long resultvar;						      \
    LOAD_ARGS_##nr (args)						      \
    asm volatile (							      \
    "movq %1, %%rax\n\t"						      \
    "int $0x82\n\t"							      \
    : "=a" (resultvar)							      \
    : "i" (__NR_##name) ASM_ARGS_##nr : "memory", "cc", "r11", "cx");	      \
    (long) resultvar; })

#define INLINE_SYSCALL3(name, nr, args...) \
  ({									      \
    unsigned long resultvar;						      \
    LOAD_ARGS_##nr (args)						      \
    asm volatile (							      \
    "movq %1, %%rax\n\t"						      \
    "int $0x84\n\t"							      \
    : "=a" (resultvar)							      \
    : "i" (__NR_##name) ASM_ARGS_##nr : "memory", "cc", "r11", "cx");	      \
    (long) resultvar; })

#undef INTERNAL_SYSCALL_ERROR_P
#define INTERNAL_SYSCALL_ERROR_P(val, err) \
  ((unsigned long) (val) >= -4095L)

#undef INTERNAL_SYSCALL_ERRNO
#define INTERNAL_SYSCALL_ERRNO(val, err)	(-(val))

#define LOAD_ARGS_0()
#define ASM_ARGS_0

#define LOAD_ARGS_1(a1)					\
  register long int _a1 asm ("rdi") = (long) (a1);	\
  LOAD_ARGS_0 ()
#define ASM_ARGS_1	ASM_ARGS_0, "r" (_a1)

#define LOAD_ARGS_2(a1, a2)				\
  register long int _a2 asm ("rsi") = (long) (a2);	\
  LOAD_ARGS_1 (a1)
#define ASM_ARGS_2	ASM_ARGS_1, "r" (_a2)

#define LOAD_ARGS_3(a1, a2, a3)				\
  register long int _a3 asm ("rdx") = (long) (a3);	\
  LOAD_ARGS_2 (a1, a2)
#define ASM_ARGS_3	ASM_ARGS_2, "r" (_a3)

#define LOAD_ARGS_4(a1, a2, a3, a4)			\
  register long int _a4 asm ("r10") = (long) (a4);	\
  LOAD_ARGS_3 (a1, a2, a3)
#define ASM_ARGS_4	ASM_ARGS_3, "r" (_a4)

#define LOAD_ARGS_5(a1, a2, a3, a4, a5)			\
  register long int _a5 asm ("r8") = (long) (a5);	\
  LOAD_ARGS_4 (a1, a2, a3, a4)
#define ASM_ARGS_5	ASM_ARGS_4, "r" (_a5)

#define LOAD_ARGS_6(a1, a2, a3, a4, a5, a6)		\
  register long int _a6 asm ("r9") = (long) (a6);	\
  LOAD_ARGS_5 (a1, a2, a3, a4, a5)
#define ASM_ARGS_6	ASM_ARGS_5, "r" (_a6)

#endif	/* __ASSEMBLER__ */
#else 
/* For Linux we can use the system call table in the header file
	/usr/include/asm/unistd.h
   of the kernel.  But these symbols do not follow the SYS_* syntax
   so we have to redefine the `SYS_ify' macro here.  */
#undef SYS_ify
#define SYS_ify(syscall_name)	__NR_##syscall_name

/* ELF-like local names start with `.L'.  */
#undef L
#define L(name)	.L##name

#ifdef __ASSEMBLER__

/* Linux uses a negative return value to indicate syscall errors,
   unlike most Unices, which use the condition codes' carry flag.

   Since version 2.1 the return value of a system call might be
   negative even if the call succeeded.  E.g., the `lseek' system call
   might return a large offset.  Therefore we must not anymore test
   for < 0, but test for a real error by making sure the value in %rax
   is a real error number.  Linus said he will make sure the no syscall
   returns a value in -1 .. -4095 as a valid result so we can savely
   test with -4095.  */

/* We don't want the label for the error handle to be global when we define
   it here.  */
#ifdef PIC
# define SYSCALL_ERROR_LABEL 0f
#else
# define SYSCALL_ERROR_LABEL syscall_error
#endif

#undef	PSEUDO
#define	PSEUDO(name, syscall_name, args)				      \
  .text;								      \
  ENTRY (name)								      \
    DO_CALL (args, syscall_name);					      \
    cmpq $-4095, %rax;							      \
    jae SYSCALL_ERROR_LABEL;						      \
  L(pseudo_end):

#undef	PSEUDO_END
#define	PSEUDO_END(name)						      \
  SYSCALL_ERROR_HANDLER							      \
  END (name)

#ifndef PIC
#define SYSCALL_ERROR_HANDLER	/* Nothing here; code in sysdep.S is used.  */
#else
/* Store (- %rax) into errno through the GOT.  */
#ifdef _LIBC_REENTRANT
#define SYSCALL_ERROR_HANDLER						      \
0:pushq %rbx;								      \
  call 1f;								      \
1:popq %rbx;								      \
  xorq %rdx, %rdx;							      \
  addq $_GLOBAL_OFFSET_TABLE_+[.-1b], %rbx;				      \
  subq %rax, %rdx;							      \
  pushq %rdx;								      \
  PUSH_ERRNO_LOCATION_RETURN;						      \
  call BP_SYM (__errno_location)@PLT;					      \
  POP_ERRNO_LOCATION_RETURN;						      \
  popq %rcx;								      \
  popq %rbx;								      \
  movq %rcx, (%rax);							      \
  orq $-1, %rax;							      \
  jmp L(pseudo_end);
/* A quick note: it is assumed that the call to `__errno_location' does
   not modify the stack!  */
#else
#define SYSCALL_ERROR_HANDLER						      \
0:call 1f;								      \
1:popq %rcx;								      \
  xorq %rdx, %rdx;							      \
  addq $_GLOBAL_OFFSET_TABLE_+[.-1b], %rcx;				      \
  subq %rax, %rdx;							      \
  movq errno@GOT(%rcx), %rcx;						      \
  movq %rdx, (%rcx);							      \
  orq $-1, %rax;							      \
  jmp L(pseudo_end);
#endif	/* _LIBC_REENTRANT */
#endif	/* PIC */

/* Linux takes system call arguments in registers:

	syscall number	%rax	     call-clobbered
	arg 1		%rbx	     call-saved
	arg 2		%rcx	     call-clobbered
	arg 3		%rdx	     call-clobbered
	arg 4		%rsi	     call-saved
	arg 5		%rdi	     call-saved

   The stack layout upon entering the function is:

	40(%rsp)	Arg# 5
	32(%rsp)	Arg# 4
	24(%rsp)	Arg# 3
	16(%rsp)	Arg# 2
	 8(%rsp)	Arg# 1
	  (%rsp)	Return address

   (Of course a function with say 3 arguments does not have entries for
   arguments 4 and 5.)

   The following code tries hard to be optimal.  A general assumption
   (which is true according to the data books I have) is that

	2 * xchg	is more expensive than	pushq + movq + popq

   Beside this a neat trick is used.  The calling conventions for Linux
   tell that among the registers used for parameters %rcx and %rdx need
   not be saved.  Beside this we may clobber this registers even when
   they are not used for parameter passing.

   As a result one can see below that we save the content of the %rbx
   register in the %rdx register when we have less than 3 arguments
   (2 * movq is less expensive than pushq + popl).

   Second unlike for the other registers we don't save the content of
   %rcx and %rdx when we have more than 1 and 2 registers resp.

   The code below might look a bit long but we have to take care for
   the pipelined processors (i586).  Here the `pushl' and `popl'
   instructions are marked as NP (not pairable) but the exception is
   two consecutive of these instruction.  This gives no penalty on
   other processors though.  */

#undef	DO_CALL
#define DO_CALL(args, syscall_name)			      		      \
    PUSHARGS_##args							      \
    DOARGS_##args							      \
    movq $SYS_ify (syscall_name), %rax;					      \
    int $0x81								      \
    POPARGS_##args

#define PUSHARGS_0	/* No arguments to push.  */
#define	DOARGS_0	/* No arguments to frob.  */
#define	POPARGS_0	/* No arguments to pop.  */
#define	_PUSHARGS_0	/* No arguments to push.  */
#define _DOARGS_0(n)	/* No arguments to frob.  */
#define	_POPARGS_0	/* No arguments to pop.  */

#define PUSHARGS_1	movq %rbx, %rdx; PUSHARGS_0
#define	DOARGS_1	_DOARGS_1 (4)
#define	POPARGS_1	POPARGS_0; movq %rdx, %rbx
#define	_PUSHARGS_1	pushq %rbx; _PUSHARGS_0
#define _DOARGS_1(n)	movq n(%rsp), %rbx; _DOARGS_0(n-4)
#define	_POPARGS_1	_POPARGS_0; popq %rbx

#define PUSHARGS_2	PUSHARGS_1
#define	DOARGS_2	_DOARGS_2 (8)
#define	POPARGS_2	POPARGS_1
#define _PUSHARGS_2	_PUSHARGS_1
#define	_DOARGS_2(n)	movq n(%rsp), %rcx; _DOARGS_1 (n-4)
#define	_POPARGS_2	_POPARGS_1

#define PUSHARGS_3	_PUSHARGS_2
#define DOARGS_3	_DOARGS_3 (16)
#define POPARGS_3	_POPARGS_3
#define _PUSHARGS_3	_PUSHARGS_2
#define _DOARGS_3(n)	movq n(%rsp), %rdx; _DOARGS_2 (n-4)
#define _POPARGS_3	_POPARGS_2

#define PUSHARGS_4	_PUSHARGS_4
#define DOARGS_4	_DOARGS_4 (24)
#define POPARGS_4	_POPARGS_4
#define _PUSHARGS_4	pushq %rsi; _PUSHARGS_3
#define _DOARGS_4(n)	movq n(%rsp), %rsi; _DOARGS_3 (n-4)
#define _POPARGS_4	_POPARGS_3; popq %rsi

#define PUSHARGS_5	_PUSHARGS_5
#define DOARGS_5	_DOARGS_5 (32)
#define POPARGS_5	_POPARGS_5
#define _PUSHARGS_5	pushq %rdi; _PUSHARGS_4
#define _DOARGS_5(n)	movq n(%rsp), %rdi; _DOARGS_4 (n-4)
#define _POPARGS_5	_POPARGS_4; popq %rdi

#else	/* !__ASSEMBLER__ */

/* We need some help from the assembler to generate optimal code.  We
   define some macros here which later will be used.  */
asm (".L__X'%rbx = 1\n\t"
     ".L__X'%rcx = 2\n\t"
     ".L__X'%rdx = 2\n\t"
     ".L__X'%rax = 3\n\t"
     ".L__X'%rsi = 3\n\t"
     ".L__X'%rdi = 3\n\t"
     ".L__X'%rbp = 3\n\t"
     ".L__X'%rsp = 3\n\t"
#if 0
     ".L__X'%ebx = .L__X'%rbx\n\t"
     ".L__X'%ecx = .L__X'%rcx\n\t"
     ".L__X'%edx = .L__X'%rdx\n\t"
     ".L__X'%eax = .L__X'%rax\n\t"
     ".L__X'%esi = .L__X'%rsi\n\t"
     ".L__X'%edi = .L__X'%rdi\n\t"
     ".L__X'%ebp = .L__X'%rbp\n\t"
     ".L__X'%esp = .L__X'%rsp\n\t"
#endif
     ".macro bpushq name reg\n\t"
     ".if 1 - \\name\n\t"
     ".if 2 - \\name\n\t"
     "pushq %rbx\n\t"
     ".else\n\t"
     "xchgq \\reg, %rbx\n\t"
     ".endif\n\t"
     ".endif\n\t"
     ".endm\n\t"
     ".macro bpopq name reg\n\t"
     ".if 1 - \\name\n\t"
     ".if 2 - \\name\n\t"
     "popq %rbx\n\t"
     ".else\n\t"
     "xchgq \\reg, %rbx\n\t"
     ".endif\n\t"
     ".endif\n\t"
     ".endm\n\t"
     ".macro bmovq name reg\n\t"
     ".if 1 - \\name\n\t"
     ".if 2 - \\name\n\t"
     "movq \\reg, %rbx\n\t"
     ".endif\n\t"
     ".endif\n\t"
     ".endm\n\t");

extern int errno;

/* Define a macro which expands inline into the wrapper code for a system
   call.  */
#undef INLINE_SYSCALL
#define INLINE_SYSCALL(name, nr, args...) \
  ({									      \
    unsigned int resultvar;						      \
    asm volatile (							      \
    LOADARGS_##nr							      \
    "movq %1, %%rax\n\t"						      \
    "int $0x81\n\t"							      \
    RESTOREARGS_##nr							      \
    : "=a" (resultvar)							      \
    : "i" (__NR_##name) ASMFMT_##nr(args) : "memory", "cc");		      \
    if (resultvar >= 0xfffff001)					      \
      {									      \
	errno= (-resultvar);					      \
	resultvar = 0xffffffff;						      \
      }									      \
    (int) resultvar; })

#undef INLINE_SYSCALL1
#define INLINE_SYSCALL1(name, nr, args...) \
  ({									      \
    unsigned int resultvar;						      \
    asm volatile (							      \
    LOADARGS_##nr							      \
    "movq %1, %%rax\n\t"						      \
    "int $0x82\n\t"							      \
    RESTOREARGS_##nr							      \
    : "=a" (resultvar)							      \
    : "i" (__NR_##name) ASMFMT_##nr(args) : "memory", "cc");		      \
    if (resultvar >= 0xfffff001)					      \
      {									      \
	errno= (-resultvar);					      \
	resultvar = 0xffffffff;						      \
      }									      \
    (int) resultvar; })

#undef INLINE_SYSCALL3
#define INLINE_SYSCALL3(name, nr, args...) \
  ({									      \
    unsigned int resultvar;						      \
    asm volatile (							      \
    LOADARGS_##nr							      \
    "movq %1, %%rax\n\t"						      \
    "int $0x84\n\t"							      \
    RESTOREARGS_##nr							      \
    : "=a" (resultvar)							      \
    : "i" (__NR_##name) ASMFMT_##nr(args) : "memory", "cc");		      \
    if (resultvar >= 0xfffff001)					      \
      {									      \
	errno= (-resultvar);					      \
	resultvar = 0xffffffff;						      \
      }									      \
    (int) resultvar; })

#undef INLINE_SYSCALLTEST
#define INLINE_SYSCALLTEST(name, nr, args...) \
  ({									      \
    unsigned int resultvar;						      \
    asm volatile (							      \
    LOADARGS_##nr							      \
    "movq %1, %%rax\n\t"						      \
    "call 0xc0100010\n\t"						      \
    RESTOREARGS_##nr							      \
    : "=a" (resultvar)							      \
    : "i" (__NR_##name) ASMFMT_##nr(args) : "memory", "cc");		      \
    if (resultvar >= 0xfffff001)					      \
      {									      \
	errno= (-resultvar);					      \
	resultvar = 0xffffffff;						      \
      }									      \
    (int) resultvar; })

#define LOADARGS_0
#define LOADARGS_1 \
    "bpushq .L__X'%k2, %k2\n\t"						      \
    "bmovq .L__X'%k2, %k2\n\t"
#define LOADARGS_2	LOADARGS_1
#define LOADARGS_3	LOADARGS_1
#define LOADARGS_4	LOADARGS_1
#define LOADARGS_5	LOADARGS_1

#define RESTOREARGS_0
#define RESTOREARGS_1 \
    "bpopq .L__X'%k2, %k2\n\t"
#define RESTOREARGS_2	RESTOREARGS_1
#define RESTOREARGS_3	RESTOREARGS_1
#define RESTOREARGS_4	RESTOREARGS_1
#define RESTOREARGS_5	RESTOREARGS_1

#define ASMFMT_0()
#define ASMFMT_1(arg1) \
	, "acdSD" (arg1)
#define ASMFMT_2(arg1, arg2) \
	, "adCD" (arg1), "c" (arg2)
#define ASMFMT_3(arg1, arg2, arg3) \
	, "aCD" (arg1), "c" (arg2), "d" (arg3)
#define ASMFMT_4(arg1, arg2, arg3, arg4) \
	, "aD" (arg1), "c" (arg2), "d" (arg3), "S" (arg4)
#define ASMFMT_5(arg1, arg2, arg3, arg4, arg5) \
	, "a" (arg1), "c" (arg2), "d" (arg3), "S" (arg4), "D" (arg5)

#endif	/* __ASSEMBLER__ */
#endif
#endif /* __x86_64__ */
#endif /* linux/i386/sysdep.h */ /* linux/x86_64/sysdep.h */
#endif /* compile_um */
