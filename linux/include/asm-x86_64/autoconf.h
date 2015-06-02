#ifndef i386_autoconf_h
#define i386_autoconf_h

#define CONFIG_IA32_EMULATION 1
#define CONFIG_X86_64 1
#define CONFIG_X86 1
#undef  CONFIG_ISA
#undef  CONFIG_UID16
#define CONFIG_RWSEM_GENERIC_SPINLOCK 1
#define CONFIG_X86_CMPXCHG 1
#define ONFIG_EARLY_PRINTK 1

/*
 * Processor type and features
 */
#define CONFIG_GENERIC_CPU 1
#if (defined CONFIG_GENERIC_CPU) || (defined CONFIG_MPSC)
#define CONFIG_X86_L1_CACHE_BYTES (128)
#define CONFIG_X86_L1_CACHE_SHIFT (7)
#else
#define CONFIG_X86_L1_CACHE_BYTES (64)
#define CONFIG_X86_L1_CACHE_SHIFT (6)
#endif
#define CONFIG_X86_TSC 1
#define CONFIG_X86_GOOD_APIC 1
#undef  CONFIG_X86_CPUID
#undef  CONFIG_MATH_EMULATION
#undef  CONFIG_EISA
#undef  CONFIG_MCA
#define CONFIG_X86_IO_APIC 1
#define CONFIG_X86_LOCAL_APIC 1
#undef  CONFIG_MTRR
#define CONFIG_SMP 1
#undef  CONFIG_HPET_TIMER
#undef  CONFIG_GART_IOMMU
#ifndef CONFIG_GART_IOMMU
#define CONFIG_DUMMY_IOMMU 1
#endif
#ifndef CONFIG_SMP
#define CONFIG_X86_UP_IOAPIC 1
#endif
#define CONFIG_MCE 1
#ifdef CONFIG_SMP
#undef CONFIG_K8_NUMA
#ifdef CONFIG_K8_NUMA
#define CONFIG_DISCONTIGMEM 1
#define CONFIG_NUMA 1
#endif
#endif

#undef  CONFIG_PM
#undef  CONFIG_APM_IGNORE_USER_SUSPEND
#undef  CONFIG_APM_DO_ENABLE
#undef  CONFIG_APM_CPU_IDLE
#undef  CONFIG_APM_DISPLAY_BLANK
#undef  CONFIG_APM_RTC_IS_GMT
#undef  CONFIG_APM_ALLOW_INTS

/*
 * General setup
 */
#define CONFIG_PCI 1
#undef  CONFIG_PCI_GOANY
#undef  CONFIG_PCI_BIOS
#define CONFIG_PCI_DIRECT 1
#undef  CONFIG_PCI_NAMES
#undef  CONFIG_HOTPLUG_PCI

/*
 * Memory Technology Devices (MTD)
 */
#undef  CONFIG_MTD
/*
 * Block devices
 */
#undef  CONFIG_BLK_DEV_XD
#undef  CONFIG_PARIDE
#undef  CONFIG_BLK_CPQ_DA

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
#undef  CONFIG_BLK_DEV_HD_IDE
#undef  CONFIG_BLK_DEV_HD
#define CONFIG_BLK_DEV_IDEDISK 1
#define CONFIG_IDEDISK_MULTI_MODE 1

/*
 * IDE chipset support/bugfixes
 */
#undef  CONFIG_BLK_DEV_TRM290
#undef  CONFIG_BLK_DEV_VIA82CXXX
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
#undef CONFIG_E1000_NAPI

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
#undef  CONFIG_DEBUG_HIGHMEM
#undef  CONFIG_DEBUG_SLAB
#undef  CONFIG_DEBUG_IOVIRT
#define CONFIG_MAGIC_SYSRQ 1
#undef  CONFIG_DEBUG_SPINLOCK
#define CONFIG_DEBUG_BUGVERBOSE 1

#endif
