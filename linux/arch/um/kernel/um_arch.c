/* 
 * Copyright (C) 2000 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include "linux/config.h"
#include "linux/sched.h"
#include "linux/mm.h"
#include "linux/types.h"
#include "linux/tty.h"
#include "linux/init.h"
#include "linux/bootmem.h"
#include "linux/spinlock.h"
#include "linux/utsname.h"
#include <linux/seq_file.h>
#include "asm/page.h"
#include "asm/pgtable.h"
#include "asm/ptrace.h"
#include "asm/elf.h"
#include "asm/user.h"
#include "asm/delay.h"
#include "ubd_user.h"
#include "asm/current.h"
#include "user_util.h"
#include "kern_util.h"
#include "kern.h"
#include "mprot.h"
#include "mem_user.h"
#include "umid.h"
#include "initrd.h"
#include "init.h"

#define DEFAULT_COMMAND_LINE "root=/dev/ubd0"

int dump_fpu(struct pt_regs *regs, elf_fpregset_t *fpu)
{
	return(0);
}

unsigned long thread_saved_pc(struct thread_struct *thread)
{
	panic("Someone should implement thread_saved_pc");
	return(0);
}

/*
 * get_cpuinfo - Get information on one CPU for use by procfs.
 *
 *	Prints info on the next CPU into buffer.  Beware, doesn't check for
 *	buffer overflow.  Current implementation of procfs assumes that the
 *	resulting data is <= 1K.
 *
 * Args:
 *	buffer	-- you guessed it, the data buffer
 *	cpu_np	-- Input: next cpu to get (start at 0).  Output: Updated.
 *
 *	Returns number of bytes written to buffer.
 */

static int show_cpuinfo(struct seq_file *m, void *v)
{
	int index;

	index = (struct cpuinfo_um *)v - cpu_data;
#ifdef CONFIG_SMP
	if (!(cpu_online_map & (1 << index)))
		return 0;
#endif

	seq_printf(m, "processor\t: user-mode\n");
	seq_printf(m, "bogomips\t: %lu.%02lu\n",
		   loops_per_jiffy/(500000/HZ),
		   (loops_per_jiffy/(5000/HZ)) % 100);
	seq_printf(m, "host\t\t: %s\n", host_info);

	return(0);
}

static void *c_start(struct seq_file *m, loff_t *pos)
{
	return *pos < NR_CPUS ? cpu_data + *pos : NULL;
}

static void *c_next(struct seq_file *m, void *v, loff_t *pos)
{
	++*pos;
	return c_start(m, pos);
}

static void c_stop(struct seq_file *m, void *v)
{
}

struct seq_operations cpuinfo_op = {
	start:	c_start,
	next:	c_next,
	stop:	c_stop,
	show:	show_cpuinfo,
};

pte_t * __bad_pagetable(void)
{
	panic("Someone should implement __bad_pagetable");
	return(NULL);
}

extern void start_kernel(void);

extern int debug;
extern int debug_stop;

static int start_kernel_proc(void *unused)
{
	int pid;

	block_signals();
	pid = getpid();

	cpu_tasks[0].pid = pid;
	cpu_tasks[0].task = &init_task_union; // was: current;
#ifdef CONFIG_SMP
 	cpu_online_map = 1;
#endif
	if(debug) stop_pid(pid);
	start_kernel();
	return(0);
}

extern unsigned long high_physmem;

#ifdef CONFIG_HOST_2G_2G
#define START 0x60000000
#else
#define START 0x80000000
#endif

unsigned long host_task_size;
unsigned long task_size;

void set_task_sizes(int arg)
{
	/* Round up to the nearest 4M */
	host_task_size = ROUND_4M((unsigned long) &arg);
	task_size = START;
}

unsigned long uml_physmem;

unsigned long start_vm;
unsigned long end_vm;

int ncpus = 1;

#define PFN_UP(x)	(((x) + PAGE_SIZE-1) >> PAGE_SHIFT)
#define PFN_DOWN(x)	((x) >> PAGE_SHIFT)
#define PFN_PHYS(x)	((x) << PAGE_SHIFT)

static char *argv1_begin = NULL;
static char *argv1_end = NULL;

static int have_root __initdata = 0;
long physmem_size = 32 * 1024 * 1024;

void set_cmdline(char *cmd)
{
	if(honeypot) return;
	strcpy(argv1_begin, "[");
	strncat(argv1_begin, cmd, argv1_end - argv1_begin - strlen("[]"));
	strcat(argv1_begin, "]");
	memset(argv1_begin + strlen(argv1_begin), '\0', 
	       argv1_end - argv1_begin - strlen(argv1_begin));  
}

static char *usage_string = 
"User Mode Linux v%s\n"
"	available at http://user-mode-linux.sourceforge.net/\n\n";

static int __init uml_version_setup(char *line, int *add)
{
	printf("%s\n", system_utsname.release);
	exit(0);
}

__uml_setup("--version", uml_version_setup,
"--version\n"
"    Prints the version number of the kernel.\n\n"
);

static int __init uml_root_setup(char *line, int *add)
{
	have_root = 1;
	return 0;
}

__uml_setup("root=", uml_root_setup,
"root=<file containing the root fs>\n"
"    This is actually used by the generic kernel in exactly the same\n"
"    way as in any other kernel. If you configure a number of block\n"
"    devices and want to boot off something other than ubd0, you \n"
"    would use something like:\n"
"        root=/dev/ubd5\n\n"
);

#ifdef CONFIG_SMP
static int __init uml_ncpus_setup(char *line, int *add)
{
       if (!sscanf(line, "%d", &ncpus)) {
               printk("Couldn't parse [%s]\n", line);
               return -1;
       }

       return 0;
}

__uml_setup("ncpus=", uml_ncpus_setup,
"ncpus=<# of desired CPUs>\n"
"    This tells an SMP kernel how many virtual processors to start.\n"
"    Currently, this has no effect because SMP isn't enabled.\n\n" 
);
#endif

static int __init Usage(char *line, int *add)
{
 	const char **p;

	printf(usage_string, system_utsname.release);
 	p = &__uml_help_start;
 	while (p < &__uml_help_end) {
 		printf("%s", *p);
 		p++;
 	}
	exit(0);
}

__uml_setup("--help", Usage,
"--help\n"
"    Prints this message.\n\n"
);

static int __init uml_checksetup(char *line, int *add)
{
	struct uml_param *p;

	p = &__uml_setup_start;
	while(p < &__uml_setup_end) {
		int n;

		n = strlen(p->str);
		if(!strncmp(line, p->str, n)){
			if (p->setup_func(line + n, add)) return 1;
		}
		p++;
	}
	return 0;
}

static void __init uml_postsetup(void)
{
	initcall_t *p;

	p = &__uml_postsetup_start;
	while(p < &__uml_postsetup_end){
		(*p)();
		p++;
	}
	return;
}

extern int debug_trace;
extern int jail;
void *brk_start;

int linux_main(int argc, char **argv)
{
	unsigned long start_pfn, end_pfn, bootmap_size;
	unsigned long virtmem_size;
	unsigned int i, add;
	void *sp;

	for (i = 1; i < argc; i++){
		if((i == 1) && (argv[i][0] == ' ')) continue;
		add = 1;
		uml_checksetup(argv[i], &add);
		if(add) add_arg(saved_command_line, argv[i]);
	}
	if(have_root == 0) add_arg(saved_command_line, DEFAULT_COMMAND_LINE);

	if(!jail)
		remap_data(ROUND_DOWN(&_stext), ROUND_UP(&_etext), 1);
	remap_data(ROUND_DOWN(&_sdata), ROUND_UP(&_edata), 1);
	brk_start = sbrk(0);
	remap_data(ROUND_DOWN(&__bss_start), ROUND_UP(brk_start), 1);

	/* Start physical memory at least 4M after the current brk */
	uml_physmem = ROUND_4M(brk_start) + (1 << 22);

	setup_machinename(system_utsname.machine);

	argv1_begin = argv[1];
	argv1_end = &argv[1][strlen(argv[1])];
  
	/* Kernel vm starts after physical memory and is either the size
	 * of physical memory or the remaining space left in the kernel
	 * area of the address space, whichever is smaller.
	 */
	start_vm = uml_physmem + physmem_size + VMALLOC_OFFSET;
	if(start_vm >= get_kmem_end())
		panic("Physical memory too large to allow any kernel "
		      "virtual memory");

	virtmem_size = physmem_size;
	if(physmem_size > get_kmem_end() - start_vm)
		virtmem_size = get_kmem_end() - start_vm;
	end_vm = start_vm + virtmem_size;

	if(virtmem_size < physmem_size)
		printk(KERN_INFO "Kernel virtual memory size shrunk to %ld "
		       "bytes\n", virtmem_size);

	setup_range(-1, NULL, uml_physmem, physmem_size, 
		    physmem_size + VMALLOC_OFFSET + virtmem_size);
	setup_memory();
	high_physmem = uml_physmem + physmem_size;

	start_pfn = PFN_UP(__pa(uml_physmem));
	end_pfn = PFN_DOWN(__pa(high_physmem));
	bootmap_size = init_bootmem(start_pfn, end_pfn - start_pfn);
	free_bootmem(__pa(uml_physmem) + bootmap_size, 
		     high_physmem - uml_physmem - bootmap_size);
  	uml_postsetup();

	init_task.thread.kernel_stack = (unsigned long) &init_task + 
		2 * PAGE_SIZE;

	task_protections((unsigned long) &init_task);
	sp = (void *) init_task.thread.kernel_stack + 2 * PAGE_SIZE - 
		sizeof(unsigned long);
	return(signals(start_kernel_proc, sp));
}

void __init setup_arch(char **cmdline_p)
{
	paging_init();
 	strcpy(command_line, saved_command_line);
 	*cmdline_p = command_line;
	setup_hostinfo();
}

void __init check_bugs(void)
{
	check_ptrace();
}

spinlock_t pid_lock = SPIN_LOCK_UNLOCKED;

void lock_pid(void)
{
	spin_lock(&pid_lock);
}

void unlock_pid(void)
{
	spin_unlock(&pid_lock);
}

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
