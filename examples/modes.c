#ifdef __i386__
extern int errno;
#else
#include<errno.h>
#endif

#define LOADARGS_0
#define LOADARGS_1 \
"bpushl .L__X'%k2, %k2\n\t"                                               \
"bmovl .L__X'%k2, %k2\n\t"
#define LOADARGS_2      LOADARGS_1
#define LOADARGS_3      LOADARGS_1
#define LOADARGS_4      LOADARGS_1
#define LOADARGS_5      LOADARGS_1

#define RESTOREARGS_0
#define RESTOREARGS_1 \
"bpopl .L__X'%k2, %k2\n\t"
#define RESTOREARGS_2   RESTOREARGS_1
#define RESTOREARGS_3   RESTOREARGS_1
#define RESTOREARGS_4   RESTOREARGS_1
#define RESTOREARGS_5   RESTOREARGS_1

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


#define INLINE_SYSCALL(name, nr, args...) \
({                                                                          \
unsigned int resultvar;                                                   \
asm volatile (                                                            \
LOADARGS_##nr                                                             \
"movl %1, %%eax\n\t"                                                      \
"int $0xb0\n\t"                                                           \
RESTOREARGS_##nr                                                          \
: "=a" (resultvar)                                                        \
: "i" (__NR_##name) ASMFMT_##nr(args) : "memory", "cc");                  \
if (resultvar >= 0xfffff001)                                              \
{                                                                       \
errno= (-resultvar);                                          \
resultvar = 0xffffffff;                                               \
}                                                                       \
(int) resultvar; })

#define INLINE_SYSCALL2(name, nr, args...) \
({                                                                          \
unsigned int resultvar;                                                   \
asm volatile (                                                            \
LOADARGS_##nr                                                             \
"movl %1, %%eax\n\t"                                                      \
"int $0xb1\n\t"                                                           \
RESTOREARGS_##nr                                                          \
: "=a" (resultvar)                                                        \
: "i" (__NR_##name) ASMFMT_##nr(args) : "memory", "cc");                  \
if (resultvar >= 0xfffff001)                                              \
{                                                                       \
errno= (-resultvar);                                          \
resultvar = 0xffffffff;                                               \
}                                                                       \
(int) resultvar; })

#define __NR_$setprn 1

int sys$testcode(void) {
	  return INLINE_SYSCALL($setprn,0);
}

int sys$testcode2(void) {
	  return INLINE_SYSCALL2($setprn,0);
}

main(){
sys$testcode();
sys$testcode2();
}
