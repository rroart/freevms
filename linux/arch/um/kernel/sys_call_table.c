/* 
 * Copyright (C) 2000 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include "linux/unistd.h"
#include "linux/version.h"
#include "linux/sys.h"
#include "asm/signal.h"
#include "sysdep/syscalls.h"
#include "kern_util.h"

extern syscall_handler_t sys_ni_syscall;
extern syscall_handler_t sys_exit;
extern syscall_handler_t sys_fork;
extern syscall_handler_t sys_read;
extern syscall_handler_t sys_write;
extern syscall_handler_t sys_creat;
extern syscall_handler_t sys_link;
extern syscall_handler_t sys_unlink;
extern syscall_handler_t sys_chdir;
extern syscall_handler_t sys_mknod;
extern syscall_handler_t sys_chmod;
extern syscall_handler_t sys_lchown16;
extern syscall_handler_t sys_ni_syscall;
extern syscall_handler_t sys_stat;
extern syscall_handler_t sys_lseek;
extern syscall_handler_t sys_getpid;
extern syscall_handler_t sys_oldumount;
extern syscall_handler_t sys_setuid16;
extern syscall_handler_t sys_getuid16;
extern syscall_handler_t sys_ptrace;
extern syscall_handler_t sys_alarm;
extern syscall_handler_t sys_fstat;
extern syscall_handler_t sys_pause;
extern syscall_handler_t sys_utime;
extern syscall_handler_t sys_ni_syscall;
extern syscall_handler_t sys_ni_syscall;
extern syscall_handler_t sys_access;
extern syscall_handler_t sys_nice;
extern syscall_handler_t sys_ni_syscall;
extern syscall_handler_t sys_sync;
extern syscall_handler_t sys_kill;
extern syscall_handler_t sys_rename;
extern syscall_handler_t sys_mkdir;
extern syscall_handler_t sys_rmdir;
extern syscall_handler_t sys_pipe;
extern syscall_handler_t sys_times;
extern syscall_handler_t sys_ni_syscall;
extern syscall_handler_t sys_brk;
extern syscall_handler_t sys_setgid16;
extern syscall_handler_t sys_getgid16;
extern syscall_handler_t sys_signal;
extern syscall_handler_t sys_geteuid16;
extern syscall_handler_t sys_getegid16;
extern syscall_handler_t sys_acct;
extern syscall_handler_t sys_umount;
extern syscall_handler_t sys_ni_syscall;
extern syscall_handler_t sys_ioctl;
extern syscall_handler_t sys_fcntl;
extern syscall_handler_t sys_ni_syscall;
extern syscall_handler_t sys_setpgid;
extern syscall_handler_t sys_ni_syscall;
extern syscall_handler_t sys_olduname;
extern syscall_handler_t sys_umask;
extern syscall_handler_t sys_chroot;
extern syscall_handler_t sys_ustat;
extern syscall_handler_t sys_dup2;
extern syscall_handler_t sys_getppid;
extern syscall_handler_t sys_getpgrp;
extern syscall_handler_t sys_sigaction;
extern syscall_handler_t sys_sgetmask;
extern syscall_handler_t sys_ssetmask;
extern syscall_handler_t sys_setreuid16;
extern syscall_handler_t sys_setregid16;
extern syscall_handler_t sys_sigsuspend;
extern syscall_handler_t sys_sigpending;
extern syscall_handler_t sys_sethostname;
extern syscall_handler_t sys_setrlimit;
extern syscall_handler_t sys_old_getrlimit;
extern syscall_handler_t sys_getrusage;
extern syscall_handler_t sys_gettimeofday;
extern syscall_handler_t sys_settimeofday;
extern syscall_handler_t sys_getgroups16;
extern syscall_handler_t sys_setgroups16;
extern syscall_handler_t sys_symlink;
extern syscall_handler_t sys_lstat;
extern syscall_handler_t sys_readlink;
extern syscall_handler_t sys_uselib;
extern syscall_handler_t sys_swapon;
extern syscall_handler_t sys_reboot;
extern syscall_handler_t old_readdir;
extern syscall_handler_t sys_munmap;
extern syscall_handler_t sys_truncate;
extern syscall_handler_t sys_ftruncate;
extern syscall_handler_t sys_fchmod;
extern syscall_handler_t sys_fchown16;
extern syscall_handler_t sys_getpriority;
extern syscall_handler_t sys_setpriority;
extern syscall_handler_t sys_ni_syscall;
extern syscall_handler_t sys_statfs;
extern syscall_handler_t sys_fstatfs;
extern syscall_handler_t sys_ni_syscall;
extern syscall_handler_t sys_socketcall;
extern syscall_handler_t sys_syslog;
extern syscall_handler_t sys_setitimer;
extern syscall_handler_t sys_getitimer;
extern syscall_handler_t sys_newstat;
extern syscall_handler_t sys_newlstat;
extern syscall_handler_t sys_newfstat;
extern syscall_handler_t sys_uname;
extern syscall_handler_t sys_ni_syscall;
extern syscall_handler_t sys_vhangup;
extern syscall_handler_t sys_ni_syscall;
extern syscall_handler_t sys_ni_syscall;
extern syscall_handler_t sys_swapoff;
extern syscall_handler_t sys_sysinfo;
extern syscall_handler_t sys_ipc;
extern syscall_handler_t sys_fsync;
extern syscall_handler_t sys_sigreturn;
extern syscall_handler_t sys_clone;
extern syscall_handler_t sys_setdomainname;
extern syscall_handler_t sys_newuname;
extern syscall_handler_t sys_ni_syscall;
extern syscall_handler_t sys_adjtimex;
extern syscall_handler_t sys_mprotect;
extern syscall_handler_t sys_sigprocmask;
extern syscall_handler_t sys_create_module;
extern syscall_handler_t sys_init_module;
extern syscall_handler_t sys_delete_module;
extern syscall_handler_t sys_get_kernel_syms;
extern syscall_handler_t sys_quotactl;
extern syscall_handler_t sys_getpgid;
extern syscall_handler_t sys_fchdir;
extern syscall_handler_t sys_bdflush;
extern syscall_handler_t sys_sysfs;
extern syscall_handler_t sys_personality;
extern syscall_handler_t sys_ni_syscall;
extern syscall_handler_t sys_setfsuid16;
extern syscall_handler_t sys_setfsgid16;
extern syscall_handler_t sys_llseek;
extern syscall_handler_t sys_getdents;
extern syscall_handler_t sys_flock;
extern syscall_handler_t sys_msync;
extern syscall_handler_t sys_readv;
extern syscall_handler_t sys_writev;
extern syscall_handler_t sys_getsid;
extern syscall_handler_t sys_fdatasync;
extern syscall_handler_t sys_sysctl;
extern syscall_handler_t sys_mlock;
extern syscall_handler_t sys_munlock;
extern syscall_handler_t sys_mlockall;
extern syscall_handler_t sys_munlockall;
extern syscall_handler_t sys_sched_setparam;
extern syscall_handler_t sys_sched_getparam;
extern syscall_handler_t sys_sched_setscheduler;
extern syscall_handler_t sys_sched_getscheduler;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 5, 0)
extern syscall_handler_t sys_sched_yield;
#endif
extern syscall_handler_t sys_sched_get_priority_max;
extern syscall_handler_t sys_sched_get_priority_min;
extern syscall_handler_t sys_sched_rr_get_interval;
extern syscall_handler_t sys_nanosleep;
extern syscall_handler_t sys_mremap;
extern syscall_handler_t sys_setresuid16;
extern syscall_handler_t sys_getresuid16;
extern syscall_handler_t sys_ni_syscall;
extern syscall_handler_t sys_query_module;
extern syscall_handler_t sys_poll;
extern syscall_handler_t sys_nfsservctl;
extern syscall_handler_t sys_setresgid16;
extern syscall_handler_t sys_getresgid16;
extern syscall_handler_t sys_prctl;
extern syscall_handler_t sys_ni_syscall;
extern syscall_handler_t sys_rt_sigaction;
extern syscall_handler_t sys_rt_sigprocmask;
extern syscall_handler_t sys_rt_sigpending;
extern syscall_handler_t sys_rt_sigtimedwait;
extern syscall_handler_t sys_rt_sigqueueinfo;
extern syscall_handler_t sys_rt_sigsuspend;
extern syscall_handler_t sys_pread;
extern syscall_handler_t sys_pwrite;
extern syscall_handler_t sys_chown16;
extern syscall_handler_t sys_getcwd;
extern syscall_handler_t sys_capget;
extern syscall_handler_t sys_capset;
extern syscall_handler_t sys_sigaltstack;
extern syscall_handler_t sys_sendfile;
extern syscall_handler_t sys_ni_syscall;
extern syscall_handler_t sys_ni_syscall;
extern syscall_handler_t sys_vfork;
extern syscall_handler_t sys_getrlimit;
extern syscall_handler_t sys_mmap2;
extern syscall_handler_t sys_truncate64;
extern syscall_handler_t sys_ftruncate64;
extern syscall_handler_t sys_stat64;
extern syscall_handler_t sys_lstat64;
extern syscall_handler_t sys_fstat64;
extern syscall_handler_t sys_lchown;
extern syscall_handler_t sys_getuid;
extern syscall_handler_t sys_getgid;
extern syscall_handler_t sys_geteuid;
extern syscall_handler_t sys_getegid;
extern syscall_handler_t sys_setreuid;
extern syscall_handler_t sys_setregid;
extern syscall_handler_t sys_getgroups;
extern syscall_handler_t sys_setgroups;
extern syscall_handler_t sys_fchown;
extern syscall_handler_t sys_setresuid;
extern syscall_handler_t sys_getresuid;
extern syscall_handler_t sys_setresgid;
extern syscall_handler_t sys_getresgid;
extern syscall_handler_t sys_chown;
extern syscall_handler_t sys_setuid;
extern syscall_handler_t sys_setgid;
extern syscall_handler_t sys_setfsuid;
extern syscall_handler_t sys_setfsgid;
extern syscall_handler_t sys_pivot_root;
extern syscall_handler_t sys_mincore;
extern syscall_handler_t sys_madvise;
extern syscall_handler_t sys_fcntl64;
extern syscall_handler_t sys_getdents64;
extern syscall_handler_t sys_gettid;
extern syscall_handler_t sys_readahead;

extern syscall_handler_t um_mount;
extern syscall_handler_t um_time;
extern syscall_handler_t um_stime;

extern syscall_handler_t  exe$testtest           ;
extern syscall_handler_t  exe$setpri		 ;
extern syscall_handler_t  exe$crelnt             ;
extern syscall_handler_t  exe$setprn             ;
extern syscall_handler_t  exe$dclast             ;
extern syscall_handler_t  exe$waitfr             ;
extern syscall_handler_t  exe$wflor              ;
extern syscall_handler_t  exe$wfland             ;
extern syscall_handler_t  exe$clref              ;
extern syscall_handler_t  exe$setime            ;
extern syscall_handler_t  exe$setimr            ;
extern syscall_handler_t  exe$cantim            ;
extern syscall_handler_t  exe$numtim ;
extern syscall_handler_t  exe$gettim            ;
extern syscall_handler_t  exe$hiber             ;
extern syscall_handler_t  exe$wake              ;
extern syscall_handler_t  exe$schdwk            ;
extern syscall_handler_t  exe$canwak            ;
extern syscall_handler_t  exe$suspnd            ;
extern syscall_handler_t  exe$resume            ;
extern syscall_handler_t  exe$exit              ;
extern syscall_handler_t  exe$forcex             ;
extern syscall_handler_t  exe$setrwm            ;
extern syscall_handler_t  exe$delprc            ;
extern syscall_handler_t  exe$readef            ;
extern syscall_handler_t  exe$setef             ;
extern syscall_handler_t  exe$synch             ;
extern syscall_handler_t  exe$enq               ;
extern syscall_handler_t  exe$deq               ;

extern syscall_handler_t  exe$close  ;
extern syscall_handler_t  exe$connect  ;
extern syscall_handler_t  exe$create  ;
extern syscall_handler_t  exe$delete  ;
extern syscall_handler_t  exe$disconnect  ;
extern syscall_handler_t  exe$display  ;
extern syscall_handler_t  exe$enter  ;
extern syscall_handler_t  exe$erase  ;
extern syscall_handler_t  exe$extend  ;
extern syscall_handler_t  exe$find  ;
extern syscall_handler_t  exe$flush  ;
extern syscall_handler_t  exe$free  ;
extern syscall_handler_t  exe$get  ;
extern syscall_handler_t  exe$modify  ;
extern syscall_handler_t  exe$nxtvol  ;
extern syscall_handler_t  exe$open  ;
extern syscall_handler_t  exe$parse  ;
extern syscall_handler_t  exe$put  ;
extern syscall_handler_t  exe$read  ;
extern syscall_handler_t  exe$release  ;
extern syscall_handler_t  exe$remove  ;
extern syscall_handler_t  exe$rename  ;
extern syscall_handler_t  exe$rewind  ;
extern syscall_handler_t  exe$search  ;
extern syscall_handler_t  exe$space  ;
extern syscall_handler_t  exe$truncate  ;
extern syscall_handler_t  exe$update  ;
extern syscall_handler_t  exe$wait  ;
extern syscall_handler_t  exe$write  ;
extern syscall_handler_t  exe$filescan  ;
extern syscall_handler_t  exe$setddir  ;
extern syscall_handler_t  exe$setdfprot  ;
extern syscall_handler_t  exe$ssvexc  ;
extern syscall_handler_t  exe$rmsrundwn  ;

extern syscall_handler_t  exe$asctim             ;
extern syscall_handler_t  exe$bintim             ;
extern syscall_handler_t  exe$assign             ;
extern syscall_handler_t  exe$dassgn             ;
extern syscall_handler_t  exe$qio             ;
extern syscall_handler_t  exe$qiow             ;
extern syscall_handler_t  exe$getlki ;
extern syscall_handler_t  exe$getlkiw ;
extern syscall_handler_t  exe$enqw ;
extern syscall_handler_t  exe$crelnm ;
extern syscall_handler_t  exe$trnlnm ;
extern syscall_handler_t  exe$dellnm ;
extern syscall_handler_t  exe$clrast ;
extern syscall_handler_t  exe$setast ;
extern syscall_handler_t  exe$ascefc ;
extern syscall_handler_t  exe$dacefc ;
extern syscall_handler_t  exe$dlcefc ;
extern syscall_handler_t  exe$crembx ;
extern syscall_handler_t  exe$delmbx ;
extern syscall_handler_t  exe$mount ;
extern syscall_handler_t exe$cretva ;
extern syscall_handler_t exe$expreg ;
extern syscall_handler_t exe$crmpsc ;
extern syscall_handler_t exe$mgblsc ;
extern syscall_handler_t exe$deltva ;
extern syscall_handler_t exe$cntreg ;
extern syscall_handler_t exe$dgblsc ;
extern syscall_handler_t exe$setswm ;
extern syscall_handler_t exe$setprt ;
extern syscall_handler_t exe$adjwsl ;
extern syscall_handler_t exe$lkwset ;
extern syscall_handler_t exe$lckpag ;
extern syscall_handler_t exe$ulwset ;
extern syscall_handler_t exe$ulkpag ;
extern syscall_handler_t exe$purgws ;
extern syscall_handler_t exe$creprc           ;
extern syscall_handler_t exe$imgact           ;
extern syscall_handler_t exe$imgfix           ;
extern syscall_handler_t exe$imgsta           ;
extern syscall_handler_t exe$dclexh           ;
extern syscall_handler_t exe$rundwn           ;
extern syscall_handler_t exe$sndjbc           ;
extern syscall_handler_t exe$sndjbcw          ;
extern syscall_handler_t exe$getqui           ;
extern syscall_handler_t exe$getmsg           ;
extern syscall_handler_t exe$putmsg           ;
extern syscall_handler_t exe$excmsg           ;
extern syscall_handler_t exe$getsyi           ;
extern syscall_handler_t exe$getsyiw          ;
extern syscall_handler_t exe$device_scan      ;
extern syscall_handler_t exe$getdvi           ;
extern syscall_handler_t exe$fao              ;
extern syscall_handler_t exe$faol             ;
extern syscall_handler_t exe$cmkrnl           ;
extern syscall_handler_t exe$getjpi           ;
extern syscall_handler_t exe$updsec           ;
extern syscall_handler_t exe$resched          ;
extern syscall_handler_t exe$setexv           ;
extern syscall_handler_t exe$check_access     ;
extern syscall_handler_t exe$getuai           ;
extern syscall_handler_t exe$setuai           ;
extern syscall_handler_t exe$idtoasc          ;
extern syscall_handler_t exe$asctoid          ;
extern syscall_handler_t exe$add_ident        ;
extern syscall_handler_t exe$rem_ident        ;
extern syscall_handler_t exe$find_held        ;
extern syscall_handler_t exe$find_holder      ;
extern syscall_handler_t exe$mod_ident        ;
extern syscall_handler_t exe$mod_holder       ;
extern syscall_handler_t exe$grantid       ;
extern syscall_handler_t exe$revokid       ;
extern syscall_handler_t exe$cmexec       ;

#define LAST_GENERIC_SYSCALL __NR_$cmexec

#if LAST_GENERIC_SYSCALL > LAST_ARCH_SYSCALL
#define LAST_SYSCALL LAST_GENERIC_SYSCALL
#else
#define LAST_SYSCALL LAST_ARCH_SYSCALL
#endif

syscall_handler_t *sys_call_table[] = {
	[ 0 ] = sys_ni_syscall,
	[ __NR_exit ] = sys_exit,
	[ __NR_fork ] = sys_fork,
	[ __NR_read ] = sys_read,
	[ __NR_write ] = sys_write,

	/* These three are declared differently in asm/unistd.h */
	[ __NR_open ] = (syscall_handler_t *) sys_open,
	[ __NR_close ] = (syscall_handler_t *) sys_close,
	[ __NR_waitpid ] = (syscall_handler_t *) sys_waitpid,
	[ __NR_creat ] = sys_creat,
	[ __NR_link ] = sys_link,
	[ __NR_unlink ] = sys_unlink,

	/* declared differently in kern_util.h */
	[ __NR_execve ] = (syscall_handler_t *) sys_execve,
	[ __NR_chdir ] = sys_chdir,
	[ __NR_time ] = um_time,
	[ __NR_mknod ] = sys_mknod,
	[ __NR_chmod ] = sys_chmod,
	[ __NR_lchown ] = sys_lchown16,
	[ __NR_break ] = sys_ni_syscall,
	[ __NR_oldstat ] = sys_stat,
	[ __NR_lseek ] = sys_lseek,
	[ __NR_getpid ] = sys_getpid,
	[ __NR_mount ] = um_mount,
	[ __NR_umount ] = sys_oldumount,
	[ __NR_setuid ] = sys_setuid16,
	[ __NR_getuid ] = sys_getuid16,
	[ __NR_stime ] = um_stime,
	[ __NR_ptrace ] = sys_ptrace,
	[ __NR_alarm ] = sys_alarm,
	[ __NR_oldfstat ] = sys_fstat,
	[ __NR_pause ] = sys_pause,
	[ __NR_utime ] = sys_utime,
	[ __NR_stty ] = sys_ni_syscall,
	[ __NR_gtty ] = sys_ni_syscall,
	[ __NR_access ] = sys_access,
	[ __NR_nice ] = sys_nice,
	[ __NR_ftime ] = sys_ni_syscall,
	[ __NR_sync ] = sys_sync,
	[ __NR_kill ] = sys_kill,
	[ __NR_rename ] = sys_rename,
	[ __NR_mkdir ] = sys_mkdir,
	[ __NR_rmdir ] = sys_rmdir,

	/* Declared differently in asm/unistd.h */
	[ __NR_dup ] = (syscall_handler_t *) sys_dup,
	[ __NR_pipe ] = sys_pipe,
	[ __NR_times ] = sys_times,
	[ __NR_prof ] = sys_ni_syscall,
	[ __NR_brk ] = sys_brk,
	[ __NR_setgid ] = sys_setgid16,
	[ __NR_getgid ] = sys_getgid16,
	[ __NR_signal ] = sys_signal,
	[ __NR_geteuid ] = sys_geteuid16,
	[ __NR_getegid ] = sys_getegid16,
	[ __NR_acct ] = sys_acct,
	[ __NR_umount2 ] = sys_umount,
	[ __NR_lock ] = sys_ni_syscall,
	[ __NR_ioctl ] = sys_ioctl,
	[ __NR_fcntl ] = sys_fcntl,
	[ __NR_mpx ] = sys_ni_syscall,
	[ __NR_setpgid ] = sys_setpgid,
	[ __NR_ulimit ] = sys_ni_syscall,
	[ __NR_oldolduname ] = sys_olduname,
	[ __NR_umask ] = sys_umask,
	[ __NR_chroot ] = sys_chroot,
	[ __NR_ustat ] = sys_ustat,
	[ __NR_dup2 ] = sys_dup2,
	[ __NR_getppid ] = sys_getppid,
	[ __NR_getpgrp ] = sys_getpgrp,
	[ __NR_setsid ] = (syscall_handler_t *) sys_setsid,
	[ __NR_sigaction ] = sys_sigaction,
	[ __NR_sgetmask ] = sys_sgetmask,
	[ __NR_ssetmask ] = sys_ssetmask,
	[ __NR_setreuid ] = sys_setreuid16,
	[ __NR_setregid ] = sys_setregid16,
	[ __NR_sigsuspend ] = sys_sigsuspend,
	[ __NR_sigpending ] = sys_sigpending,
	[ __NR_sethostname ] = sys_sethostname,
	[ __NR_setrlimit ] = sys_setrlimit,
	[ __NR_getrlimit ] = sys_old_getrlimit,
	[ __NR_getrusage ] = sys_getrusage,
	[ __NR_gettimeofday ] = sys_gettimeofday,
	[ __NR_settimeofday ] = sys_settimeofday,
	[ __NR_getgroups ] = sys_getgroups16,
	[ __NR_setgroups ] = sys_setgroups16,
	[ __NR_symlink ] = sys_symlink,
	[ __NR_oldlstat ] = sys_lstat,
	[ __NR_readlink ] = sys_readlink,
	[ __NR_uselib ] = sys_uselib,
	[ __NR_swapon ] = sys_swapon,
	[ __NR_reboot ] = sys_reboot,
	[ __NR_readdir ] = old_readdir,
	[ __NR_munmap ] = sys_munmap,
	[ __NR_truncate ] = sys_truncate,
	[ __NR_ftruncate ] = sys_ftruncate,
	[ __NR_fchmod ] = sys_fchmod,
	[ __NR_fchown ] = sys_fchown16,
	[ __NR_getpriority ] = sys_getpriority,
	[ __NR_setpriority ] = sys_setpriority,
	[ __NR_profil ] = sys_ni_syscall,
	[ __NR_statfs ] = sys_statfs,
	[ __NR_fstatfs ] = sys_fstatfs,
	[ __NR_ioperm ] = sys_ni_syscall,
	[ __NR_socketcall ] = sys_socketcall,
	[ __NR_syslog ] = sys_syslog,
	[ __NR_setitimer ] = sys_setitimer,
	[ __NR_getitimer ] = sys_getitimer,
	[ __NR_stat ] = sys_newstat,
	[ __NR_lstat ] = sys_newlstat,
	[ __NR_fstat ] = sys_newfstat,
	[ __NR_olduname ] = sys_uname,
	[ __NR_iopl ] = sys_ni_syscall,
	[ __NR_vhangup ] = sys_vhangup,
	[ __NR_idle ] = sys_ni_syscall,
	[ __NR_wait4 ] = (syscall_handler_t *) sys_wait4,
	[ __NR_swapoff ] = sys_swapoff,
	[ __NR_sysinfo ] = sys_sysinfo,
	[ __NR_ipc ] = sys_ipc,
	[ __NR_fsync ] = sys_fsync,
	[ __NR_sigreturn ] = sys_sigreturn,
	[ __NR_clone ] = sys_clone,
	[ __NR_setdomainname ] = sys_setdomainname,
	[ __NR_uname ] = sys_newuname,
	[ __NR_adjtimex ] = sys_adjtimex,
	[ __NR_mprotect ] = sys_mprotect,
	[ __NR_sigprocmask ] = sys_sigprocmask,
	[ __NR_create_module ] = sys_create_module,
	[ __NR_init_module ] = sys_init_module,
	[ __NR_delete_module ] = sys_delete_module,
	[ __NR_get_kernel_syms ] = sys_get_kernel_syms,
	[ __NR_quotactl ] = sys_quotactl,
	[ __NR_getpgid ] = sys_getpgid,
	[ __NR_fchdir ] = sys_fchdir,
	[ __NR_bdflush ] = sys_bdflush,
	[ __NR_sysfs ] = sys_sysfs,
	[ __NR_personality ] = sys_personality,
	[ __NR_afs_syscall ] = sys_ni_syscall,
	[ __NR_setfsuid ] = sys_setfsuid16,
	[ __NR_setfsgid ] = sys_setfsgid16,
	[ __NR__llseek ] = sys_llseek,
	[ __NR_getdents ] = sys_getdents,
	[ __NR__newselect ] = (syscall_handler_t *) sys_select,
	[ __NR_flock ] = sys_flock,
	[ __NR_msync ] = sys_msync,
	[ __NR_readv ] = sys_readv,
	[ __NR_writev ] = sys_writev,
	[ __NR_getsid ] = sys_getsid,
	[ __NR_fdatasync ] = sys_fdatasync,
	[ __NR__sysctl ] = sys_sysctl,
	[ __NR_mlock ] = sys_mlock,
	[ __NR_munlock ] = sys_munlock,
	[ __NR_mlockall ] = sys_mlockall,
	[ __NR_munlockall ] = sys_munlockall,
	[ __NR_sched_setparam ] = sys_sched_setparam,
	[ __NR_sched_getparam ] = sys_sched_getparam,
	[ __NR_sched_setscheduler ] = sys_sched_setscheduler,
	[ __NR_sched_getscheduler ] = sys_sched_getscheduler,
	[ __NR_sched_yield ] = sys_sched_yield,
	[ __NR_sched_get_priority_max ] = sys_sched_get_priority_max,
	[ __NR_sched_get_priority_min ] = sys_sched_get_priority_min,
	[ __NR_sched_rr_get_interval ] = sys_sched_rr_get_interval,
	[ __NR_nanosleep ] = sys_nanosleep,
	[ __NR_mremap ] = sys_mremap,
	[ __NR_setresuid ] = sys_setresuid16,
	[ __NR_getresuid ] = sys_getresuid16,
	[ __NR_vm86 ] = sys_ni_syscall,
	[ __NR_query_module ] = sys_query_module,
	[ __NR_poll ] = sys_poll,
	[ __NR_nfsservctl ] = sys_nfsservctl,
	[ __NR_setresgid ] = sys_setresgid16,
	[ __NR_getresgid ] = sys_getresgid16,
	[ __NR_prctl ] = sys_prctl,
	[ __NR_rt_sigreturn ] = sys_ni_syscall,
	[ __NR_rt_sigaction ] = sys_rt_sigaction,
	[ __NR_rt_sigprocmask ] = sys_rt_sigprocmask,
	[ __NR_rt_sigpending ] = sys_rt_sigpending,
	[ __NR_rt_sigtimedwait ] = sys_rt_sigtimedwait,
	[ __NR_rt_sigqueueinfo ] = sys_rt_sigqueueinfo,
	[ __NR_rt_sigsuspend ] = sys_rt_sigsuspend,
	[ __NR_pread ] = sys_pread,
	[ __NR_pwrite ] = sys_pwrite,
	[ __NR_chown ] = sys_chown16,
	[ __NR_getcwd ] = sys_getcwd,
	[ __NR_capget ] = sys_capget,
	[ __NR_capset ] = sys_capset,
	[ __NR_sigaltstack ] = sys_sigaltstack,
	[ __NR_sendfile ] = sys_sendfile,
	[ __NR_getpmsg ] = sys_ni_syscall,
	[ __NR_putpmsg ] = sys_ni_syscall,
	[ __NR_vfork ] = sys_vfork,
	[ __NR_ugetrlimit ] = sys_getrlimit,
	[ __NR_mmap2 ] = sys_mmap2,
	[ __NR_truncate64 ] = sys_truncate64,
	[ __NR_ftruncate64 ] = sys_ftruncate64,
	[ __NR_stat64 ] = sys_stat64,
	[ __NR_lstat64 ] = sys_lstat64,
	[ __NR_fstat64 ] = sys_fstat64,
	[ __NR_fcntl64 ] = sys_fcntl64,
	[ __NR_getdents64 ] = sys_getdents64,
        [ __NR_security ] = sys_ni_syscall,
	[ __NR_gettid ] = sys_gettid,
	[ __NR_readahead ] = sys_readahead,
	[ __NR_setxattr ] = sys_ni_syscall,
	[ __NR_lsetxattr ] = sys_ni_syscall,
	[ __NR_fsetxattr ] = sys_ni_syscall,
	[ __NR_getxattr ] = sys_ni_syscall,
	[ __NR_lgetxattr ] = sys_ni_syscall,
	[ __NR_fgetxattr ] = sys_ni_syscall,
	[ __NR_listxattr ] = sys_ni_syscall,
	[ __NR_llistxattr ] = sys_ni_syscall,
	[ __NR_flistxattr ] = sys_ni_syscall,
	[ __NR_removexattr ] = sys_ni_syscall,
	[ __NR_lremovexattr ] = sys_ni_syscall,
	[ __NR_fremovexattr ] = sys_ni_syscall,

	[ __NR_$testtest ] = sys_ni_syscall          ,
	[ __NR_$setpri ] = exe$setpri		,
	[ __NR_$crelnt ] = exe$crelnt            ,
	[ __NR_$setprn ] = exe$setprn            ,
	[ __NR_$dclast ] = exe$dclast            ,
	[ __NR_$waitfr ] = exe$waitfr            ,
	[ __NR_$wflor ] = exe$wflor             ,
	[ __NR_$wfland ] = exe$wfland            ,
	[ __NR_$clref ] = exe$clref             ,
	[ __NR_$setime ] = exe$setime           ,
	[ __NR_$setimr ] = exe$setimr           ,
	[ __NR_$cantim ] = exe$cantim           ,
	[ __NR_$numtim ] = exe$numtim,
	[ __NR_$gettim ] = exe$gettim           ,
	[ __NR_$hiber ] = exe$hiber            ,
	[ __NR_$wake ] = exe$wake             ,
	[ __NR_$schdwk ] = exe$schdwk           ,
	[ __NR_$canwak ] = exe$canwak           ,
	[ __NR_$suspnd ] = exe$suspnd           ,
	[ __NR_$resume ] = exe$resume           ,
	[ __NR_$exit ] = exe$exit             ,
	[ __NR_$forcex ] = exe$forcex            ,
	[ __NR_$setrwm ] = exe$setrwm           ,
	[ __NR_$delprc ] = exe$delprc           ,
	[ __NR_$readef ] = exe$readef           ,
	[ __NR_$setef ] = exe$setef            ,
	[ __NR_$synch ] = exe$synch            ,
	[ __NR_$enq ] = exe$enq              ,
	[ __NR_$deq ] = exe$deq              ,
	[ __NR_$close ] = exe$close,
	[ __NR_$connect ] = exe$connect,
	[ __NR_$create ] = exe$create,
	[ __NR_$delete ] = exe$delete,
	[ __NR_$disconnect ] = exe$disconnect,
	[ __NR_$display ] = exe$display,
	[ __NR_$enter ] = exe$enter,
	[ __NR_$erase ] = exe$erase,
	[ __NR_$extend ] = exe$extend,
	[ __NR_$find ] = exe$find,
	[ __NR_$flush ] = exe$flush,
	[ __NR_$free ] = exe$free,
	[ __NR_$get ] = exe$get,
	[ __NR_$modify ] = exe$modify,
	[ __NR_$nxtvol ] = exe$nxtvol,
	[ __NR_$open ] = exe$open,
	[ __NR_$parse ] = exe$parse,
	[ __NR_$put ] = exe$put,
	[ __NR_$read ] = exe$read,
	[ __NR_$release ] = exe$release,
	[ __NR_$remove ] = exe$remove,
	[ __NR_$rename ] = exe$rename,
	[ __NR_$rewind ] = exe$rewind,
	[ __NR_$search ] = exe$search,
	[ __NR_$space ] = exe$space,
	[ __NR_$truncate ] = exe$truncate,
	[ __NR_$update ] = exe$update,
	[ __NR_$wait ] = exe$wait,
	[ __NR_$write ] = exe$write,
	[ __NR_$filescan ] = exe$filescan,
	[ __NR_$setddir ] = exe$setddir,
	[ __NR_$setdfprot ] = exe$setdfprot,
	[ __NR_$ssvexc ] = exe$ssvexc,
	[ __NR_$rmsrundwn ] = exe$rmsrundwn,
	[ __NR_$asctim ] = exe$asctim            ,
	[ __NR_$bintim ] = exe$bintim            ,
	[ __NR_$assign ] = exe$assign            ,
	[ __NR_$dassgn ] = exe$dassgn            ,
	[ __NR_$qio ] = exe$qio            ,
	[ __NR_$qiow ] = exe$qiow            ,
	[ __NR_$getlki ] = exe$getlki            ,
	[ __NR_$getlkiw ] = exe$getlkiw            ,
	[ __NR_$enqw ] = exe$enqw ,
	[ __NR_$crelnm ] = exe$crelnm ,
	[ __NR_$trnlnm ] = exe$trnlnm ,
	[ __NR_$dellnm ] = exe$dellnm ,
	[ __NR_$clrast ] = exe$clrast ,
	[ __NR_$setast ] = exe$setast ,
	[ __NR_$ascefc ] = exe$ascefc ,
	[ __NR_$dacefc ] = exe$dacefc ,
	[ __NR_$dlcefc ] = exe$dlcefc ,
	[ __NR_$crembx ] = exe$crembx ,
	[ __NR_$delmbx ] = exe$delmbx ,
	[ __NR_$mount ] = exe$mount,
	[ __NR_$cretva ] = exe$cretva,
	[ __NR_$expreg ] = exe$expreg,
	[ __NR_$crmpsc ] = exe$crmpsc,
	[ __NR_$mgblsc ] = exe$mgblsc,
	[ __NR_$deltva ] = exe$deltva,
	[ __NR_$cntreg ] = exe$cntreg,
	[ __NR_$dgblsc ] = exe$dgblsc,
	[ __NR_$setswm ] = exe$setswm,
	[ __NR_$setprt ] = exe$setprt,
	[ __NR_$adjwsl ] = exe$adjwsl,
	[ __NR_$lkwset ] = exe$lkwset,
	[ __NR_$lckpag ] = exe$lckpag,
	[ __NR_$ulwset ] = exe$ulwset,
	[ __NR_$ulkpag ] = exe$ulkpag,
	[ __NR_$purgws ] = exe$purgws,
	[ __NR_$creprc           ] = exe$creprc          ,
	[ __NR_$imgact           ] = exe$imgact          ,
	[ __NR_$imgfix           ] = exe$imgfix          ,
	[ __NR_$imgsta           ] = exe$imgsta          ,
	[ __NR_$dclexh           ] = exe$dclexh          ,
	[ __NR_$rundwn           ] = exe$rundwn          ,
	[ __NR_$sndjbc           ] = exe$sndjbc          ,
	[ __NR_$sndjbcw          ] = exe$sndjbcw         ,
	[ __NR_$getqui           ] = exe$getqui          ,
	[ __NR_$getmsg           ] = exe$getmsg          ,
	[ __NR_$putmsg           ] = exe$putmsg          ,
	[ __NR_$excmsg           ] = exe$excmsg          ,
	[ __NR_$getsyi           ] = exe$getsyi          ,
	[ __NR_$getsyiw          ] = exe$getsyiw         ,
	[ __NR_$device_scan      ] = exe$device_scan     ,
	[ __NR_$getdvi           ] = exe$getdvi          ,
	[ __NR_$fao              ] = exe$fao             ,
	[ __NR_$faol             ] = exe$faol            ,
	[ __NR_$cmkrnl           ] = exe$cmkrnl          ,
	[ __NR_$getjpi           ] = exe$getjpi          ,
	[ __NR_$updsec           ] = exe$updsec          ,
	[ __NR_$resched          ] = exe$resched         ,
	[ __NR_$setexv           ] = exe$setexv          ,
	[ __NR_$check_access     ] = exe$check_access    ,
	[ __NR_$getuai           ] = exe$getuai          ,
	[ __NR_$setuai           ] = exe$setuai          ,
	[ __NR_$idtoasc          ] = exe$idtoasc         ,
	[ __NR_$asctoid          ] = exe$asctoid         ,
	[ __NR_$add_ident        ] = exe$add_ident       ,
	[ __NR_$rem_ident        ] = exe$rem_ident       ,
	[ __NR_$find_held        ] = exe$find_held       ,
	[ __NR_$find_holder      ] = exe$find_holder     ,
	[ __NR_$mod_ident        ] = exe$mod_ident       ,
	[ __NR_$mod_holder       ] = exe$mod_holder      ,
	[ __NR_$cmexec       ] = exe$cmexec      ,
	
	ARCH_SYSCALLS
	[ LAST_SYSCALL + 1 ... NR_syscalls ] = 
	        (syscall_handler_t *) sys_ni_syscall
};

/*
 * Overrides for Emacs so that we follow Linus's tabbing style.
 * Emacs will notice this stuff at the end of the file and automatically
 * adjust the settings for this buffer only.  This must remain at the end
 * of the file.
 * ---------------------------------------------------------------------------
 * Local variables:
 * c-file-style: "linux"
 * End:
 */
