#ifndef uml_autoconf_h
#define uml_autoconf_h

#define CONFIG_USERMODE 1
#undef  CONFIG_ISA
#undef  CONFIG_SBUS
#undef  CONFIG_PCI
#define CONFIG_UID16 1
#define CONFIG_RWSEM_XCHGADD_ALGORITHM 1

#define CONFIG_STDIO_CONSOLE 1
#define CONFIG_SSL 1
#define CONFIG_HOSTFS 1
#define CONFIG_MCONSOLE 1
#define CONFIG_MAGIC_SYSRQ 1
#undef  CONFIG_HOST_2G_2G
#define CONFIG_CON_ZERO_CHAN "fd:0,fd:1"
#define CONFIG_CON_CHAN "xterm"
#define CONFIG_SSL_CHAN "pty"

/*
 * Devices
 */
#define CONFIG_BLK_DEV_UBD 1
#undef  CONFIG_BLK_DEV_UBD_SYNC
#undef  CONFIG_MMAPPER
#undef  CONFIG_UML_SOUND
#undef  CONFIG_SOUND
#undef  CONFIG_HOSTAUDIO
#define CONFIG_FD_CHAN 1
#define CONFIG_NULL_CHAN 1
#define CONFIG_PORT_CHAN 1
#define CONFIG_PTY_CHAN 1
#define CONFIG_TTY_CHAN 1
#define CONFIG_XTERM_CHAN 1

/*
 * Network device support
 */
#define CONFIG_UML_NET 1
#define CONFIG_UML_NET_ETHERTAP 1
#define CONFIG_UML_NET_TUNTAP 1
#define CONFIG_UML_NET_SLIP 1
#define CONFIG_UML_NET_DAEMON 1
#define CONFIG_UML_NET_MCAST 1
#define CONFIG_NETDEVICES 1
#define CONFIG_DUMMY 1
#undef  CONFIG_BONDING
#undef  CONFIG_EQUALIZER
#define CONFIG_TUN 1
#define CONFIG_ETHERTAP 1

/*
 * Ethernet (10 or 100Mbit)
 */
#undef  CONFIG_NET_ETHERNET

/*
 * Ethernet (1000 Mbit)
 */
#undef  CONFIG_ACENIC_OMIT_TIGON_I

/*
 * File systems
 */
#define CONFIG_DEVFS_FS 1
#define CONFIG_DEVFS_MOUNT 1
#undef  CONFIG_DEVFS_DEBUG

/*
 * Kernel hacking
 */
#undef  CONFIG_DEBUG_SLAB
#define CONFIG_DEBUGSYM 1
#define CONFIG_PT_PROXY 1
#undef  CONFIG_GPROF
#undef  CONFIG_GCOV

#endif
