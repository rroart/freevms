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
#define CONFIG_SYSCTL 1
#define CONFIG_BINFMT_AOUT 1
#define CONFIG_BINFMT_ELF 1
#define CONFIG_KCORE_ELF 1
#define CONFIG_UNIX98_PTYS 1
#define CONFIG_UNIX98_PTY_COUNT (256)

/*
 * Networking options
 */
#define CONFIG_PACKET 1
#define CONFIG_UNIX 1
#define CONFIG_INET 1
#define CONFIG_IP_MULTICAST 1
#define CONFIG_SYN_COOKIES 1

/*
 * File systems
 */
#define CONFIG_PROC_FS 1
#define CONFIG_DEVPTS_FS 1

#define CONFIG_EXTTWO_FS 1

/*
 * Partition Types
 */
#define CONFIG_MSDOS_PARTITION 1

#endif
