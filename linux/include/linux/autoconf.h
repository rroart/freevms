#ifndef autoconf_h
#define autoconf_h

#define CONFIG_VMS
#undef CONFIG_VMS

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
#undef  CONFIG_KCORE_AOUT
#define CONFIG_KCORE_ELF 1
#define CONFIG_UNIX98_PTYS 1
#define CONFIG_UNIX98_PTY_COUNT (256)

/*
 * Block devices
 */
#undef  CONFIG_BLK_DEV_INITRD
#undef  CONFIG_BLK_DEV_LOOP
#undef  CONFIG_BLK_DEV_NBD
#undef  CONFIG_BLK_DEV_RAM

/*
 * Networking options
 */
#define CONFIG_PACKET 1
#undef  CONFIG_PACKET_MMAP
#undef  CONFIG_NETLINK_DEV
#undef  CONFIG_NETFILTER
#undef  CONFIG_FILTER
#define CONFIG_UNIX 1
#define CONFIG_INET 1
#define CONFIG_IP_MULTICAST 1
#undef  CONFIG_IP_ADVANCED_ROUTER
#undef  CONFIG_IP_PNP
#undef  CONFIG_NET_IPIP
#undef  CONFIG_NET_IPGRE
#undef  CONFIG_IP_MROUTE
#undef  CONFIG_ARPD
#undef  CONFIG_INET_ECN
#define CONFIG_SYN_COOKIES 1
#undef  CONFIG_ATM
#undef  CONFIG_VLAN_8021Q
#undef  CONFIG_BRIDGE
#undef  CONFIG_X25
#undef  CONFIG_LAPB
#undef  CONFIG_LLC
#undef  CONFIG_NET_DIVERT
#undef  CONFIG_ECONET
#undef  CONFIG_WAN_ROUTER
#undef  CONFIG_NET_FASTROUTE
#undef  CONFIG_NET_HW_FLOWCONTROL

/*
 * QoS and/or fair queueing
 */
#undef  CONFIG_NET_SCHED

/*
 * Ethernet (1000 Mbit)
 */
#undef  CONFIG_FDDI
#undef  CONFIG_HIPPI
#undef  CONFIG_PLIP
#undef  CONFIG_PPP
#undef  CONFIG_SLIP

/*
 * Wireless LAN (non-hamradio)
 */
#undef  CONFIG_NET_RADIO
#undef  CONFIG_NET_FC
#undef  CONFIG_RCPCI
#undef  CONFIG_SHAPER

/*
 * File systems
 */
#define CONFIG_PROC_FS 1
#define CONFIG_DEVPTS_FS 1

#ifndef CONFIG_VMS
#define CONFIG_EXT2_FS 1
#else
#define CONFIG_EXTTWO_FS 1
#endif

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
