#ifndef autoconf_h
#define autoconf_h

#undef CONFIG_VMS
#define CONFIG_VMS

#ifdef CONFIG_VMS
#define CONFIG_IO_VMS
#define CONFIG_MM_VMS
#endif

#define AUTOCONF_INCLUDED

#include <asm/autoconf.h>

/*
 * Code maturity level options
 */
#define CONFIG_EXPERIMENTAL 1

/*
 * Loadable module support
 */
#define CONFIG_MODULES 1
#define CONFIG_MODVERSIONS 1
#define CONFIG_KMOD 1

/*
 * General Setup
 */
#define CONFIG_NET 1
#define CONFIG_SYSVIPC 1
#undef  CONFIG_BSD_PROCESS_ACCT
#define CONFIG_SYSCTL 1
#define CONFIG_BINFMT_AOUT 1
#define CONFIG_BINFMT_ELF 1
#define CONFIG_BINFMT_MISC 1
#define CONFIG_KCORE_ELF 1
#define CONFIG_UNIX98_PTYS 1
#define CONFIG_UNIX98_PTY_COUNT (256)

/*
 * Block devices
 */
#undef  CONFIG_BLK_DEV_INITRD

/*
 * Networking options
 */
#define CONFIG_PACKET 1
#undef  CONFIG_NETFILTER
#define CONFIG_UNIX 1
#define CONFIG_INET 1
#define CONFIG_IP_MULTICAST 1
#define CONFIG_SYN_COOKIES 1
#undef  CONFIG_ATM
#undef  CONFIG_X25
#undef  CONFIG_NET_DIVERT
#undef  CONFIG_ECONET
#undef  CONFIG_WAN_ROUTER
#undef  CONFIG_NET_FASTROUTE

/*
 * File systems
 */
#define CONFIG_PROC_FS 1
#define CONFIG_DEVPTS_FS 1

#define CONFIG_EXTTWO_FS 1

/*
 * Partition Types
 */
#define CONFIG_PARTITION_ADVANCED 1
#undef  CONFIG_ACORN_PARTITION
#undef  CONFIG_OSF_PARTITION
#undef  CONFIG_AMIGA_PARTITION
#undef  CONFIG_ATARI_PARTITION
#undef  CONFIG_MAC_PARTITION
#define CONFIG_MSDOS_PARTITION 1
#undef  CONFIG_BSD_DISKLABEL
#undef  CONFIG_MINIX_SUBPARTITION
#undef  CONFIG_SOLARIS_X86_PARTITION
#undef  CONFIG_UNIXWARE_DISKLABEL
#undef  CONFIG_LDM_PARTITION
#undef  CONFIG_SGI_PARTITION
#undef  CONFIG_ULTRIX_PARTITION
#undef  CONFIG_SUN_PARTITION

#endif
