#ifndef i386_autoconf_h
#define i386_autoconf_h

#define CONFIG_X86 1
#define CONFIG_ISA 1
#define CONFIG_UID16 1

/*
 * Processor type and features
 */
#define CONFIG_M386 1
#undef  CONFIG_X86_CMPXCHG
#define CONFIG_X86_L1_CACHE_SHIFT (4)
#undef  CONFIG_X86_CPUID
#define CONFIG_SMP 1
#define CONFIG_X86_UP_APIC 1
#define CONFIG_X86_GOOD_APIC 1
#define CONFIG_X86_IO_APIC 1
#define CONFIG_X86_LOCAL_APIC 1

/*
 * General setup
 */
#define CONFIG_PCI 1
#define CONFIG_PCI_BIOS 1
#define CONFIG_PCI_DIRECT 1
#define CONFIG_PCI_NAMES 1

/*
 * ATA/IDE/MFM/RLL support
 */
#define CONFIG_IDE 1

/*
 * IDE, ATA and ATAPI Block devices
 */
#define CONFIG_BLK_DEV_IDE 1

/*
 * Please see Documentation/ide.txt for help/info on IDE drives
 */
#define CONFIG_BLK_DEV_IDEDISK 1
#define CONFIG_IDEDISK_MULTI_MODE 1

/*
 * IDE chipset support/bugfixes
 */
#define CONFIG_BLK_DEV_IDEPCI 1
#define CONFIG_BLK_DEV_IDEDMA_PCI 1
#define CONFIG_IDEDMA_PCI_AUTO 1
#define CONFIG_BLK_DEV_IDEDMA 1
#define CONFIG_IDEDMA_AUTO 1
#define CONFIG_IDEDMA_NEW_DRIVE_LISTINGS 1
#define CONFIG_BLK_DEV_IDE_MODES 1
#define CONFIG_BLK_DEV_PIIX 1
#define CONFIG_PIIX_TUNING 1

/*
 * Network device support
 */
#define CONFIG_NETDEVICES 1
#define CONFIG_DUMMY 1

/*
 * Ethernet (10 or 100Mbit)
 */
#define CONFIG_NET_ETHERNET 1
#define CONFIG_NET_VENDOR_3COM 1
#define CONFIG_VORTEX 1
#define CONFIG_NET_ISA 1
#define CONFIG_NE2000 1
#define CONFIG_NET_PCI 1
#define CONFIG_NE2K_PCI 1
#define CONFIG_8139CP 1
#define CONFIG_8139TOO 1

/*
 * Ethernet (1000 Mbit)
 */
#define CONFIG_E1000

/*
 * Character devices
 */
#define CONFIG_VT 1
#define CONFIG_VT_CONSOLE 1
#define CONFIG_SERIAL 1
#define  CONFIG_SERIAL_CONSOLE 1

/*
 * Console drivers
 */
#define CONFIG_VGA_CONSOLE 1
#define CONFIG_VIDEO_SELECT 1

/*
 * Kernel hacking
 */
#define CONFIG_DEBUG_KERNEL 1
#define CONFIG_MAGIC_SYSRQ 1
#define CONFIG_DEBUG_BUGVERBOSE 1

#endif
