// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004.

/*
 *  linux/init/main.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 *  GK 2/5/95  -  Changed to support mounting root fs via NFS
 *  Added initrd & change_root: Werner Almesberger & Hans Lermen, Feb '96
 *  Moan early if gcc is old, avoiding bogus kernels - Paul Gortmaker, May '96
 *  Simplified starting of init:  Michael A. Griffith <grif@acm.org> 
 */

#define __KERNEL_SYSCALLS__

#define errno kernel_errno

#include <system_data_cells.h>
#include <descrip.h>
#include <prcdef.h>

#include <linux/config.h>
#include <linux/proc_fs.h>
#include <linux/devfs_fs_kernel.h>
#include <linux/unistd.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/delay.h>
#include <linux/utsname.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/smp_lock.h>
#include <linux/blk.h>
#include <linux/hdreg.h>
#include <linux/iobuf.h>
#include <linux/bootmem.h>
#include <linux/tty.h>
#ifdef __arch_um__
#include <asm-i386/hw_irq.h>
#endif

#include <asm/io.h>
#include <asm/bugs.h>

#if defined(CONFIG_ARCH_S390)
#include <asm/s390mach.h>
#include <asm/ccwcache.h>
#endif

#ifdef CONFIG_PCI
#include <linux/pci.h>
#endif

#ifdef CONFIG_DIO
#include <linux/dio.h>
#endif

#ifdef CONFIG_ZORRO
#include <linux/zorro.h>
#endif

#ifdef CONFIG_MTRR
#  include <asm/mtrr.h>
#endif

#ifdef CONFIG_NUBUS
#include <linux/nubus.h>
#endif

#ifdef CONFIG_ISAPNP
#include <linux/isapnp.h>
#endif

#ifdef CONFIG_IRDA
extern int irda_proto_init(void);
extern int irda_device_init(void);
#endif

#ifdef CONFIG_X86_LOCAL_APIC
#include <asm/smp.h>
#endif

#include <starlet.h>
#include <misc_routines.h>
#include <exe_routines.h>

void __init vms_init(void);
void __init vms_init2(void);
void __init vms_init4(void);
void __init vms_init3(void);
int /*__init*/ scs_init(void);
int scs_init2(void);
void vms_mount(void);
void vms2_mount(void);
int dlminit(void);
init_cwps();
void cre_syscommon(char * name);
probe_units();
void lnm_init_sys(void);
open_tty();
int mscp(void);
void early_printk(const char *fmt, ...);

/*
 * Versions of gcc older than that listed below may actually compile
 * and link okay, but the end product can have subtle run time bugs.
 * To avoid associated bogus bug reports, we flatly refuse to compile
 * with a gcc that is known to be too old from the very beginning.
 */
#if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 91)
#error Sorry, your GCC is too old. It builds incorrect kernels.
#endif

#ifndef __arch_um__
extern char _stext, _etext;
#endif
extern char *linux_banner;

static int init(void *);

extern void init_IRQ(void);
extern void init_modules(void);
extern void sock_init(void);
extern void fork_init(unsigned long);
extern void mca_init(void);
extern void sbus_init(void);
extern void ppc_init(void);
extern void sysctl_init(void);
extern void signals_init(void);
extern int init_pcmcia_ds(void);

extern void free_initmem(void);

#ifdef CONFIG_TC
extern void tc_init(void);
#endif

extern void ecard_init(void);

#if defined(CONFIG_SYSVIPC)
extern void ipc_init(void);
#endif
#ifdef CONFIG_PERFMON
extern void perfmon_init(void);
#endif

/*
 * Boot command-line arguments
 */
#define MAX_INIT_ARGS 8
#define MAX_INIT_ENVS 8

extern void time_init(void);
extern void softirq_init(void);

int rows, cols;

#ifdef CONFIG_BLK_DEV_INITRD
unsigned int real_root_dev;	/* do_proc_dointvec cannot handle kdev_t */
#endif

int root_mountflags = MS_RDONLY;
char *execute_command;
char root_device_name[64];


static char * argv_init[MAX_INIT_ARGS+2] = { "init", NULL, };
static char * envp_init[MAX_INIT_ENVS+2] = { "HOME=/", "TERM=linux", NULL, };

static int __init profile_setup(char *str)
{
    int par;
    if (get_option(&str,&par)) prof_shift = par;
	return 1;
}

__setup("profile=", profile_setup);


static struct dev_name_struct {
	const char *name;
	const int num;
} root_dev_names[] __initdata = {
	{ "nfs",     0x00ff },
	{ "hda",     0x0300 },
	{ "hdb",     0x0340 },
	{ "loop",    0x0700 },
	{ "hdc",     0x1600 },
	{ "hdd",     0x1640 },
	{ "hde",     0x2100 },
	{ "hdf",     0x2140 },
	{ "hdg",     0x2200 },
	{ "hdh",     0x2240 },
	{ "hdi",     0x3800 },
	{ "hdj",     0x3840 },
	{ "hdk",     0x3900 },
	{ "hdl",     0x3940 },
	{ "hdm",     0x5800 },
	{ "hdn",     0x5840 },
	{ "hdo",     0x5900 },
	{ "hdp",     0x5940 },
	{ "hdq",     0x5A00 },
	{ "hdr",     0x5A40 },
	{ "hds",     0x5B00 },
	{ "hdt",     0x5B40 },
	{ "sda",     0x0800 },
	{ "sdb",     0x0810 },
	{ "sdc",     0x0820 },
	{ "sdd",     0x0830 },
	{ "sde",     0x0840 },
	{ "sdf",     0x0850 },
	{ "sdg",     0x0860 },
	{ "sdh",     0x0870 },
	{ "sdi",     0x0880 },
	{ "sdj",     0x0890 },
	{ "sdk",     0x08a0 },
	{ "sdl",     0x08b0 },
	{ "sdm",     0x08c0 },
	{ "sdn",     0x08d0 },
	{ "sdo",     0x08e0 },
	{ "sdp",     0x08f0 },
	{ "ada",     0x1c00 },
	{ "adb",     0x1c10 },
	{ "adc",     0x1c20 },
	{ "add",     0x1c30 },
	{ "ade",     0x1c40 },
	{ "fd",      0x0200 },
	{ "md",      0x0900 },	     
	{ "xda",     0x0d00 },
	{ "xdb",     0x0d40 },
	{ "ram",     0x0100 },
	{ "scd",     0x0b00 },
	{ "mcd",     0x1700 },
	{ "cdu535",  0x1800 },
	{ "sonycd",  0x1800 },
	{ "aztcd",   0x1d00 },
	{ "cm206cd", 0x2000 },
	{ "gscd",    0x1000 },
	{ "sbpcd",   0x1900 },
	{ "eda",     0x2400 },
	{ "edb",     0x2440 },
	{ "pda",	0x2d00 },
	{ "pdb",	0x2d10 },
	{ "pdc",	0x2d20 },
	{ "pdd",	0x2d30 },
	{ "pcd",	0x2e00 },
	{ "pf",		0x2f00 },
	{ "apblock", APBLOCK_MAJOR << 8},
	{ "ddv", DDV_MAJOR << 8},
       { "ubd", UBD_MAJOR << 8 },
	{ "jsfd",    JSFD_MAJOR << 8},
#if defined(CONFIG_ARCH_S390)
	{ "dasda", (DASD_MAJOR << MINORBITS) },
	{ "dasdb", (DASD_MAJOR << MINORBITS) + (1 << 2) },
	{ "dasdc", (DASD_MAJOR << MINORBITS) + (2 << 2) },
	{ "dasdd", (DASD_MAJOR << MINORBITS) + (3 << 2) },
	{ "dasde", (DASD_MAJOR << MINORBITS) + (4 << 2) },
	{ "dasdf", (DASD_MAJOR << MINORBITS) + (5 << 2) },
	{ "dasdg", (DASD_MAJOR << MINORBITS) + (6 << 2) },
	{ "dasdh", (DASD_MAJOR << MINORBITS) + (7 << 2) },
#endif
#if defined(CONFIG_BLK_CPQ_DA) || defined(CONFIG_BLK_CPQ_DA_MODULE)
	{ "ida/c0d0p",0x4800 },
	{ "ida/c0d1p",0x4810 },
	{ "ida/c0d2p",0x4820 },
	{ "ida/c0d3p",0x4830 },
	{ "ida/c0d4p",0x4840 },
	{ "ida/c0d5p",0x4850 },
	{ "ida/c0d6p",0x4860 },
	{ "ida/c0d7p",0x4870 },
	{ "ida/c0d8p",0x4880 },
	{ "ida/c0d9p",0x4890 },
	{ "ida/c0d10p",0x48A0 },
	{ "ida/c0d11p",0x48B0 },
	{ "ida/c0d12p",0x48C0 },
	{ "ida/c0d13p",0x48D0 },
	{ "ida/c0d14p",0x48E0 },
	{ "ida/c0d15p",0x48F0 },
#endif
#if defined(CONFIG_BLK_CPQ_CISS_DA) || defined(CONFIG_BLK_CPQ_CISS_DA_MODULE)
	{ "cciss/c0d0p",0x6800 },
	{ "cciss/c0d1p",0x6810 },
	{ "cciss/c0d2p",0x6820 },
	{ "cciss/c0d3p",0x6830 },
	{ "cciss/c0d4p",0x6840 },
	{ "cciss/c0d5p",0x6850 },
	{ "cciss/c0d6p",0x6860 },
	{ "cciss/c0d7p",0x6870 },
	{ "cciss/c0d8p",0x6880 },
	{ "cciss/c0d9p",0x6890 },
	{ "cciss/c0d10p",0x68A0 },
	{ "cciss/c0d11p",0x68B0 },
	{ "cciss/c0d12p",0x68C0 },
	{ "cciss/c0d13p",0x68D0 },
	{ "cciss/c0d14p",0x68E0 },
	{ "cciss/c0d15p",0x68F0 },
#endif
	{ "nftla", 0x5d00 },
	{ "nftlb", 0x5d10 },
	{ "nftlc", 0x5d20 },
	{ "nftld", 0x5d30 },
	{ "ftla", 0x2c00 },
	{ "ftlb", 0x2c08 },
	{ "ftlc", 0x2c10 },
	{ "ftld", 0x2c18 },
	{ "mtdblock", 0x1f00 },
	{ NULL, 0 }
};

kdev_t __init name_to_kdev_t(char *line)
{
	int base = 0;

	if (strncmp(line,"/dev/",5) == 0) {
		struct dev_name_struct *dev = root_dev_names;
		line += 5;
		do {
			int len = strlen(dev->name);
			if (strncmp(line,dev->name,len) == 0) {
				line += len;
				base = dev->num;
				break;
			}
			dev++;
		} while (dev->name);
	}
	return to_kdev_t(base + simple_strtoul(line,NULL,base?10:16));
}

static int __init root_dev_setup(char *line)
{
	int i;
	char ch;

	ROOT_DEV = name_to_kdev_t(line);
	memset (root_device_name, 0, sizeof root_device_name);
	if (strncmp (line, "/dev/", 5) == 0) line += 5;
	for (i = 0; i < sizeof root_device_name - 1; ++i)
	{
	    ch = line[i];
	    if ( isspace (ch) || (ch == ',') || (ch == '\0') ) break;
	    root_device_name[i] = ch;
	}
	return 1;
}

__setup("root=", root_dev_setup);

static int __init checksetup(char *line)
{
	struct kernel_param *p;

	p = &__setup_start;
	do {
		int n = strlen(p->str);
		if (!strncmp(line,p->str,n)) {
			if (p->setup_func(line+n))
				return 1;
		}
		p++;
	} while (p < &__setup_end);
	return 0;
}

/* this should be approx 2 Bo*oMips to start (note initial shift), and will
   still work even if initially too large, it will just take slightly longer */
unsigned long loops_per_jiffy = (1<<12);

/* This is the number of bits of precision for the loops_per_jiffy.  Each
   bit takes on average 1.5/HZ seconds.  This (like the original) is a little
   better than 1% */
#define LPS_PREC 8

void __init calibrate_delay(void)
{
	unsigned long ticks, loopbit;
	int lps_precision = LPS_PREC;

	loops_per_jiffy = (1<<12);

	printk("Calibrating delay loop... ");
	while (loops_per_jiffy <<= 1) {
		/* wait for "start of" clock tick */
		ticks = jiffies;
		while (ticks == jiffies)
			/* nothing */;
		/* Go .. */
		ticks = jiffies;
		__delay(loops_per_jiffy);
		ticks = jiffies - ticks;
		if (ticks)
			break;
	}

/* Do a binary approximation to get loops_per_jiffy set to equal one clock
   (up to lps_precision bits) */
	loops_per_jiffy >>= 1;
	loopbit = loops_per_jiffy;
	while ( lps_precision-- && (loopbit >>= 1) ) {
		loops_per_jiffy |= loopbit;
		ticks = jiffies;
		while (ticks == jiffies);
		ticks = jiffies;
		__delay(loops_per_jiffy);
		if (jiffies != ticks)	/* longer than 1 tick */
			loops_per_jiffy &= ~loopbit;
	}

/* Round the value and print it */	
	printk("%lu.%02lu BogoMIPS\n",
		loops_per_jiffy/(500000/HZ),
		(loops_per_jiffy/(5000/HZ)) % 100);
}

static int __init readonly(char *str)
{
	if (*str)
		return 0;
	root_mountflags |= MS_RDONLY;
	return 1;
}

static int __init readwrite(char *str)
{
	if (*str)
		return 0;
	root_mountflags &= ~MS_RDONLY;
	return 1;
}

static int __init debug_kernel(char *str)
{
	if (*str)
		return 0;
	console_loglevel = 10;
	return 1;
}

static int __init quiet_kernel(char *str)
{
	if (*str)
		return 0;
	console_loglevel = 4;
	return 1;
}

int mount_root_vfs=0;
int mount_root_ext2=1;
int install_ods2=0;

static int __init novfs(char *str)
{
	if (*str)
		return 0;
	mount_root_vfs=0;
	mount_root_ext2=0;
	return 1;
}

__setup("ro", readonly);
__setup("rw", readwrite);
__setup("debug", debug_kernel);
__setup("quiet", quiet_kernel);
__setup("novfs", novfs);

/*
 * This is a simple kernel command line parsing function: it parses
 * the command line, and fills in the arguments/environment to init
 * as appropriate. Any cmd-line option is taken to be an environment
 * variable if it contains the character '='.
 *
 * This routine also checks for options meant for the kernel.
 * These options are not given to init - they are for internal kernel use only.
 */
static void __init parse_options(char *line)
{
	char *next,*quote;
	int args, envs;

	if (!*line)
		return;
	args = 0;
	envs = 1;	/* TERM is set to 'linux' by default */
	next = line;
	while ((line = next) != NULL) {
                quote = strchr(line,'"');
                next = strchr(line, ' ');
                while (next != NULL && quote != NULL && quote < next) {
                        /* we found a left quote before the next blank
                         * now we have to find the matching right quote
                         */
                        next = strchr(quote+1, '"');
                        if (next != NULL) {
                                quote = strchr(next+1, '"');
                                next = strchr(next+1, ' ');
                        }
                }
                if (next != NULL)
                        *next++ = 0;
		if (!strncmp(line,"init=",5)) {
			line += 5;
			execute_command = line;
			/* In case LILO is going to boot us with default command line,
			 * it prepends "auto" before the whole cmdline which makes
			 * the shell think it should execute a script with such name.
			 * So we ignore all arguments entered _before_ init=... [MJ]
			 */
			args = 0;
			continue;
		}
		if (!strncmp(line,"install",7)) {
			line += 7;
			install_ods2 = 1;
			continue;
		}
		if (checksetup(line))
			continue;
		
		/*
		 * Then check if it's an environment variable or
		 * an option.
		 */
		if (strchr(line,'=')) {
			if (envs >= MAX_INIT_ENVS)
				break;
			envp_init[++envs] = line;
		} else {
			if (args >= MAX_INIT_ARGS)
				break;
			if (*line)
				argv_init[++args] = line;
		}
	}
	argv_init[args+1] = NULL;
	envp_init[envs+1] = NULL;
}


extern void setup_arch(char **);
extern void cpu_idle(void);

unsigned long wait_init_idle;

#ifndef CONFIG_SMP

#ifdef CONFIG_X86_LOCAL_APIC
static void __init smp_init(void)
{
	APIC_init_uniprocessor();
}
#else
#define smp_init()	do { } while (0)
#endif

#else


/* Called by boot processor to activate the rest. */
static void __init smp_init(void)
{
	/* Get other processors into their bootup holding patterns. */
	smp_boot_cpus();
	wait_init_idle = cpu_online_map;
	clear_bit(current->pcb$l_cpu_id, &wait_init_idle); /* Don't wait on me! */

	smp_threads_ready=1;
	smp_commence();

	/* Wait for the other cpus to set up their idle processes */
	printk("Waiting on wait_init_idle (map = 0x%lx)\n", wait_init_idle);
	while (wait_init_idle) {
		cpu_relax();
		barrier();
	}
	printk("All processors have done init_idle\n");
}

#endif

/*
 * We need to finalize in a non-__init function or else race conditions
 * between the root thread and the init thread may cause start_kernel to
 * be reaped by free_initmem before the root thread has proceeded to
 * cpu_idle.
 */

int fix_init_thread=1;

static void rest_init(void)
{
  printk("%%KERNEL-I-DEBUG, Before first kernel_thread\n");
	kernel_thread(init, NULL, CLONE_FS | CLONE_FILES | CLONE_SIGNAL);
	// { int j; for(j=0;j<1000000000;j++) ; } 
  printk("%%KERNEL-I-DEBUG, After first kernel_thread\n");
  fix_init_thread=0;
	unlock_kernel();
#if 0
	current->need_resched = 1;
#endif
 	cpu_idle();
} 

void vms_init(void) __init;
void vms_init2(void) __init;

/*
 *	Activate the first processor.
 */

asmlinkage void __init start_kernel(void)
{
	char * command_line;
	unsigned long mempages;
	extern char saved_command_line[];
/*
 * Interrupts are still disabled. Do necessary setups, then
 * enable them
 */
#if 0
	kernel_puts("puts 1\n");
	lock_kernel();
#endif
	kernel_puts("puts 2\n");
	printk(linux_banner);
	kernel_puts("puts 3\n");
	setup_arch(&command_line);
	kernel_puts("puts 4\n");
	printk("Kernel command line: %s\n", saved_command_line);
	kernel_puts("puts 5\n");
	parse_options(command_line);
	kernel_puts("puts 7.5\n");
	init_sys_p1pp();
#if 1
	kernel_puts("puts 1\n");
	lock_kernel();
#endif
	kernel_puts("puts 6\n");
	trap_init();
	kernel_puts("puts 7\n");
	init_IRQ();
	kernel_puts("puts 8\n");
	vms_init();
	kernel_puts("puts 9\n");
	sched_init();
	kernel_puts("puts 10\n");
#ifndef CONFIG_VMS
	softirq_init();
	kernel_puts("puts 11\n");
#endif
	time_init();
	kernel_puts("puts 12\n");
	vms_init2();
	kernel_puts("puts 13\n");
	/*
	 * HACK ALERT! This is early. We're enabling the console before
	 * we've done PCI setups etc, and console_init() must be aware of
	 * this. But we do want output early, in case something goes wrong.
	 */
	kernel_puts("puts 14\n");
	console_init();
	kernel_puts("puts 15\n");
#ifdef CONFIG_MODULES
#if 0
	init_modules();
#endif
#endif
	if (prof_shift) {
		unsigned int size;
		/* only text is profiled */
		prof_len = (unsigned long) &_etext - (unsigned long) &_stext;
		prof_len >>= prof_shift;
		
		size = prof_len * sizeof(unsigned int) + PAGE_SIZE-1;
		prof_buffer = (unsigned int *) alloc_bootmem(size);
	}

	kernel_puts("puts 16\n");
	kmem_cache_init();
	sti();
	pgtable_cache_init();

	calibrate_delay();
#ifdef CONFIG_PERFMON
	perfmon_init();
#endif
#ifdef CONFIG_BLK_DEV_INITRD
	if (initrd_start && !initrd_below_start_ok &&
			initrd_start < min_low_pfn << PAGE_SHIFT) {
		printk(KERN_CRIT "initrd overwritten (0x%08lx < 0x%08lx) - "
		    "disabling it.\n",initrd_start,min_low_pfn << PAGE_SHIFT);
		initrd_start = 0;
	}
#endif
	mem_init();
	printk("%%KERNEL-I-DEBUG, After mem_init, before lnm_init_sys\n");
	lnm_init_sys();
	printk("%%KERNEL-I-DEBUG, After lnm_init\n");
	kmem_cache_sizes_init();
	printk("%%KERNEL-I-DEBUG, After kmem_cache_size_init\n");
	mempages = num_physpages;

	fork_init(mempages);
	proc_caches_init();
#ifndef CONFIG_VMS
	vfs_caches_init(mempages);
#endif
	buffer_init(mempages);
	page_cache_init(mempages);
#if defined(CONFIG_ARCH_S390)
	ccwcache_init();
#endif
	signals_init();
#ifndef CONFIG_VMS
#ifdef CONFIG_PROC_FS
	proc_root_init();
#endif
#endif
#if defined(CONFIG_SYSVIPC)
	ipc_init();
#endif
	check_bugs();
	lnm_init_prc(current); // put this later than lnm_init_sys
	printk("No POSIX conformance testing by UNIFIX\n");
	printk("Popular Organization for the Suppression of Individual eXpression\n");

	/* 
	 *	We count on the initial thread going ok 
	 *	Like idlers init is an unlocked kernel thread, which will
	 *	make syscalls (and thus be locked).
	 */

	mb_vmsinit();
	vms_init4();
	vms_init3();
	smp_init();
	rest_init();
}

#ifdef CONFIG_BLK_DEV_INITRD
static int do_linuxrc(void * shell)
{
	static char *argv[] = { "linuxrc", NULL, };

	close(0);close(1);close(2);
	setsid();
	(void) open("/dev/console",O_RDWR,0);
	(void) dup(0);
	(void) dup(0);
	return execve(shell, argv, envp_init);
}

#endif

struct task_struct *child_reaper = &init_task;

static void __init do_initcalls(void)
{
	initcall_t *call;

	call = &__initcall_start;
	do {
	  //printk ("before call %x\n",call);
	  //{ int i; for (i=10000000;i;i--) ; }
		(*call)();
		//printk("after call\n");
		//{ int i; for (i=10000000;i;i--) ; }
		call++;
	} while (call < &__initcall_end);

	//printk("bef flush\n");
	/* Make sure there is no pending stuff from the initcall sequence */
#ifndef CONFIG_VMS
	flush_scheduled_tasks();
#endif
}

/*
 * Ok, the machine is now initialized. None of the devices
 * have been touched yet, but the CPU subsystem is up and
 * running, and memory and process management works.
 *
 * Now we can finally start doing some real work..
 */
static void __init do_basic_setup(void)
{

	/*
	 * Tell the world that we're going to be the grim
	 * reaper of innocent orphaned children.
	 *
	 * We don't want people to have to make incorrect
	 * assumptions about where in the task array this
	 * can be found.
	 */
	child_reaper = current;

#if defined(CONFIG_MTRR)	/* Do this after SMP initialization */
/*
 * We should probably create some architecture-dependent "fixup after
 * everything is up" style function where this would belong better
 * than in init/main.c..
 */
	mtrr_init();
#endif

#ifdef CONFIG_SYSCTL
	sysctl_init();
#endif

	/*
	 * Ok, at this point all CPU's should be initialized, so
	 * we can start looking into devices..
	 */
#if defined(CONFIG_ARCH_S390)
	s390_init_machine_check();
#endif

#ifdef CONFIG_PCI
	pci_init();
#endif
#ifdef CONFIG_SBUS
	sbus_init();
#endif
#if defined(CONFIG_PPC)
	ppc_init();
#endif
#ifdef CONFIG_MCA
	mca_init();
#endif
#ifdef CONFIG_ARCH_ACORN
	ecard_init();
#endif
#ifdef CONFIG_ZORRO
	zorro_init();
#endif
#ifdef CONFIG_DIO
	dio_init();
#endif
#ifdef CONFIG_NUBUS
	nubus_init();
#endif
#ifdef CONFIG_ISAPNP
	isapnp_init();
#endif
#ifdef CONFIG_TC
	tc_init();
#endif

#ifndef CONFIG_VMS
	/* Networking initialization needs a process context */ 
	sock_init();
	printk("%%KERNEL-I-DEBUG, After sock_init\n");
#endif
#ifndef CONFIG_VMS
	start_context_thread();
	printk("%%KERNEL-I-DEBUG, After start_context_thread\n");
#endif
	do_initcalls();
	printk("%%KERNEL-I-DEBUG, After do_initcalls\n");

#ifdef CONFIG_IRDA
	irda_proto_init();
	irda_device_init(); /* Must be done after protocol initialization */
#endif
#ifdef CONFIG_PCMCIA
	init_pcmcia_ds();		/* Do this last */
#endif
}

extern void rd_load(void);
extern void initrd_load(void);

#ifndef CONFIG_VMS
/*
 * Prepare the namespace - decide what/where to mount, load ramdisks, etc.
 */
static void prepare_namespace(void)
{
#ifdef CONFIG_BLK_DEV_INITRD
	int real_root_mountflags = root_mountflags;
	if (!initrd_start)
		mount_initrd = 0;
	if (mount_initrd)
		root_mountflags &= ~MS_RDONLY;
	real_root_dev = ROOT_DEV;
#endif

#ifdef CONFIG_BLK_DEV_RAM
#ifdef CONFIG_BLK_DEV_INITRD
	if (mount_initrd)
		initrd_load();
	else
#endif
	rd_load();
#endif

#ifndef CONFIG_VMS
	if (mount_root_vfs)
	/* Mount the root filesystem.. */
	mount_root();

	mount_devfs_fs ();
#endif

#ifdef CONFIG_BLK_DEV_INITRD
	root_mountflags = real_root_mountflags;
	if (mount_initrd && ROOT_DEV != real_root_dev
	    && MAJOR(ROOT_DEV) == RAMDISK_MAJOR && MINOR(ROOT_DEV) == 0) {
		int error;
		int i, pid;

		pid = kernel_thread(do_linuxrc, "/linuxrc", SIGCHLD);
		if (pid > 0) {
			while (pid != wait(&i)) {
			  //				current->policy |= SCHED_YIELD;
				//schedule();
			  SOFTINT_RESCHED_VECTOR;
			}
		}
		if (MAJOR(real_root_dev) != RAMDISK_MAJOR
		     || MINOR(real_root_dev) != 0) {
			error = change_root(real_root_dev,"/initrd");
			if (error)
				printk(KERN_ERR "Change root to /initrd: "
				    "error %d\n",error);
		}
	}
#endif
}
#endif

extern int mydebug5;
extern int mydebug6;

int scs_init_done = 0;

static int init(void * unused)
{
	lock_kernel();
#ifdef CONFIG_VMS
	user_spaceable();
#endif
#ifdef CONFIG_VMS
	xqp_init2();
	extern void * global_e2_vcb;
	exttwo_init2(global_e2_vcb);
#endif
	do_basic_setup();
	printk("%%KERNEL-I-DEBUG, After do_basic_setup\n");
#ifdef CONFIG_VMS
	if (mount_root_ext2)
	  vms_mount();
	else
	  vms2_mount();
	extern char * default_name;
	char * vmsdev = do_translate(root_device_name);
	memcpy(default_name,vmsdev,strlen(vmsdev));
	cre_syscommon(vmsdev);
	int __init kswapd_init(void);
	kswapd_init();
#endif
#ifndef CONFIG_VMS
	prepare_namespace();
	printk("%%KERNEL-I-DEBUG, After prepare_namspace\n");
#endif 

#ifdef CONFIG_VMS
	probe_units();
#endif

	/*
	 * Ok, we have completed the initial bootup, and
	 * we're essentially up and running. Get rid of the
	 * initmem segments and start the user-mode stuff..
	 */
	free_initmem();
	printk("%%KERNEL-I-DEBUG, After free_initmem\n");
	unlock_kernel();
	//	mydebug5=1;
	//	mydebug6=1;
	printk("%%KERNEL-I-DEBUG, After unlock_kernel\n");
#ifndef CONFIG_VMS
	if (mount_root_vfs && open("/dev/console", O_RDWR, 0) < 0)
		printk("Warning: unable to open an initial console.\n");
#else
#if 0
	open_tty();
#else
#if 0
	sys_open("opa0:", 0, 0);
#else
	void sys_open_term();
	sys_open_term("opa0:");
#endif
#endif
#endif
	printk("%%KERNEL-I-DEBUG, Before dup\n");
	(void) dup(0);
	(void) dup(0);
	printk("%%KERNEL-I-DEBUG, After dup\n");
	
	/*
	 * We try each of these until one succeeds.
	 *
	 * The Bourne shell can be used instead of init if we are 
	 * trying to recover a really broken machine.
	 */

#ifdef __i386__
	// temp disabled on x86_64 due to stack growth
       scs_init();
       mscp();
       //__du_init();
        dlminit();
        init_cwps();
#endif

#ifdef CONFIG_VMS
	extern void job_control();
	kernel_thread(job_control, NULL, CLONE_FS | CLONE_FILES | CLONE_SIGNAL);
	extern void Main(void);
	extern char * mydevice;
	probe_units();
	if (mydevice==0)
	  printk("%%KERNEL-I-DEBUG, No network module. Can not start IPACP.\n");
	else
#if 0
	  kernel_thread(Main, NULL, CLONE_FS | CLONE_FILES | CLONE_SIGNAL);
#else
	if (1) {
#if 0
	  $DESCRIPTOR(image,"[vms$common.sysexe]ipacp");
	  $DESCRIPTOR(out,"SYS$OUTPUT");
#else
	  $DESCRIPTOR(image_1,"[vms$common.sysexe]ipacp");
	  $DESCRIPTOR(image_2,"/vms$common/sysexe/ipacp");
	  struct dsc$descriptor * image;
	  if (mount_root_vfs)
	    image = &image_2;
	  else
	    image = &image_1;
	  $DESCRIPTOR(out,"opa0:");
#endif
	  $DESCRIPTOR(name,"IPACP");
	  
	  long long priv = -1;
	  exe$creprc(0,image,0,&out,&out,&priv,0,&name,8,0x40001,0,PRC$M_NOUAF);
	}
#endif
	if (mydevice==0)
	  printk("%%KERNEL-I-DEBUG, No network module. Can not start clustering.\n");
	else {
	  if (scs_init_done==0)
	    printk("%%KERNEL-I-DEBUG, Nothing in params.dat. Can not start clustering.\n");
#ifdef __i386__
	// temp disabled on x86_64 due to stack growth
	  else
	    scs_init2();
#endif
	}
#endif

#ifdef CONFIG_VMS
#ifdef __arch_um__
 go:
	exe$hiber();
	goto go;
#endif
#endif
#ifdef __i386__
	if (execute_command)
		execve(execute_command,argv_init,envp_init);
#else
	if (execute_command) {
#if 1
	  // not yet. related to P1 ksp
	  long * l = 0x7ffa0000;
	  l[-1]=0;
	  l[-1-512]=0;
	  l = 0x7ff90000;
	  l[-1]=0;
	  l[-1-512]=0;
	  l = 0x7ff80000;
	  l[-1]=0;
	  l[-1-512]=0;
	  struct tss_struct *tss = init_tss + smp_processor_id();
	  tss->rsp0 = l;
#if 1
	  write_pda(kernelstack, (long) l);
#endif
	  asm ("movq $0x7ffa0000, %rsp");
#if 1
	  asm ("subq $0x8, %rsp");  // to have 16-byte aligned with fake
#endif
#endif
		execve(execute_command,argv_init,envp_init);
	}
#endif
#if 0
	printk("Will try to start loginout.\nPress <enter> or something as usual (but within 60 seconds).\n");
	execve("/vms$common/sysexe/loginout",argv_init,envp_init);
#else
#if 0
	$DESCRIPTOR(image,"[vms$common.sysexe]loginout");
	$DESCRIPTOR(out,"SYS$OUTPUT");
#else
	$DESCRIPTOR(image_1,"[vms$common.sysexe]loginout");
	$DESCRIPTOR(image_2,"/vms$common/sysexe/loginout");
	struct dsc$descriptor * image;
	if (mount_root_vfs)
	  image = &image_2;
	else
	  image = &image_1;
	$DESCRIPTOR(out,"opa0:");
#endif
	$DESCRIPTOR(in,"[vms$common.sysexe]startup.com");
	$DESCRIPTOR(in2,"[vms$common.sysexe]install.com");
	$DESCRIPTOR(name,"STARTUP");

	long long priv = -1;
	if (install_ods2)
	  exe$creprc(0,image,&in2,&out,&out,&priv,0,&name,8,0x40001,0,PRC$M_NOUAF);
	else
	  exe$creprc(0,image,&in,&out,&out,&priv,0,&name,8,0x40001,0,PRC$M_NOUAF);
	sys$hiber();
#endif
#if 0
	// not anymore?
	execve("/sbin/init",argv_init,envp_init);
	execve("/etc/init",argv_init,envp_init);
	execve("/bin/init",argv_init,envp_init);
#endif
	printk("Did not find loginout\n");
	execve("/bin/sh",argv_init,envp_init);
	panic("No init found.  Try passing init= option to kernel.");
}

// debug additions

#ifdef __i386__
static char *vidmem = (char *)0xb8000;
static int vidport=0x3d4;
static int lines=25, mycols=80, orig_x=1, orig_y=10;

void scroll(void)
{
  int i;

  memcpy ( vidmem, vidmem + mycols * 2, ( lines - 1 ) * mycols * 2 );
  for ( i = ( lines - 1 ) * mycols * 2; i < lines * mycols * 2; i += 2 )
    vidmem[i] = ' ';
}

void kernel_puts(const char *s)
{
  int x,y,pos;
  char c;

  x=orig_x;
  y=orig_y;

  while ( ( c = *s++ ) != '\0' ) {
    if ( c == '\n' ) {
      x = 0;
      if ( ++y >= lines ) {
	scroll();
	y--;
      }
    } else {
      vidmem [ ( x + mycols * y ) * 2 ] = c; 
      if ( ++x >= mycols ) {
	x = 0;
	if ( ++y >= lines ) {
	  scroll();
	  y--;
	}
      }
    }
  }

  orig_x = x;
  orig_y = y;

  pos = (x + mycols * y) * 2;       /* Update cursor position */
  outb_p(14, vidport);
  outb_p(0xff & (pos >> 9), vidport+1);
  outb_p(15, vidport);
  outb_p(0xff & (pos >> 1), vidport+1);
}

#endif

#ifdef __arch_um__
void kernel_puts(const char *s) {
  printk("%s",s);
}
#endif

#ifdef __x86_64__
void kernel_puts(const char *s) {
  early_printk("%s",s);
}
#endif
