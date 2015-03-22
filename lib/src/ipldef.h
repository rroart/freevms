#ifndef IPLDEF_H
#define IPLDEF_H

/* Author: Roar Thronæs */

#define IPL$_FORKABLE_IPL   3904
#define IPL$_POWER  31
#define IPL$_EMB    31
#define IPL$_MCHECK 31
#define IPL$_MEGA   31
/* 30 is powerfail, 30-24 are cpu-specific */
/* 23-20 are device interrupts */
#define IPL$_IPINTR 22
#define IPL$_VIRTCONS   22
#define IPL$_HWCLK  22
#define IPL$_INVALIDATE 21
#define IPL$_PERFMON    15
#define IPL$_MAILBOX    11
#define IPL$_POOL   11
#define IPL$_IOLOCK11   11
#define IPL$_IOLOCK10   10
#define IPL$_IOLOCK9    9
#define IPL$_SYNCH  8
#define IPL$_TIMER  8
#define IPL$_SCS    8
#define IPL$_SCHED  8
#define IPL$_MMG    8
#define IPL$_IO_MISC    8
#define IPL$_FILSYS 8
#define IPL$_TX_SYNCH   8
#define IPL$_LCKMGR 8
#define IPL$_IOLOCK8    8
#define IPL$_PORT   8
#define IPL$_TIMERFORK  7
#define IPL$_QUEUEAST   6
#define IPL$_IOPOST 4
#define IPL$_RESCHED    3
#define IPL$_ASTDEL 2

#endif

