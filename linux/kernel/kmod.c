// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004.

/*
    kmod, the new module loader (replaces kerneld)
    Kirk Petersen

    Reorganized not to be a daemon by Adam Richter, with guidance
    from Greg Zornetzer.

    Modified to avoid chroot and file sharing problems.
    Mikael Pettersson

    Limit the concurrent number of kmod modprobes to catch loops from
    "modprobe needs a service that is in a module".
    Keith Owens <kaos@ocs.com.au> December 1999

    Unblock all signals when we exec a usermode process.
    Shuu Yamaguchi <shuu@wondernetworkresources.com> December 2000
*/

#ifdef __x86_64__
#define errno kernel_errno
#else
#define errno kernel_errno
#endif

#define __KERNEL_SYSCALLS__

#include <linux/config.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <linux/kmod.h>
#include <linux/smp_lock.h>
#include <linux/completion.h>

#include <asm/uaccess.h>

extern int max_threads;

int exec_usermodehelper(char *program_path, char *argv[], char *envp[])
{
    return 0;
}

#ifdef CONFIG_KMOD

/*
    modprobe_path is set via /proc/sys.
*/
char modprobe_path[256] = "/sbin/modprobe";

static int exec_modprobe(void * module_name)
{
    static char * envp[] = { "HOME=/", "TERM=linux", "PATH=/sbin:/usr/sbin:/bin:/usr/bin", NULL };
    char *argv[] = { modprobe_path, "-s", "-k", "--", (char*)module_name, NULL };
    int ret;

    ret = exec_usermodehelper(modprobe_path, argv, envp);
    if (ret)
    {
        printk(KERN_ERR
               "kmod: failed to exec %s -s -k %s, errno = %d\n",
               modprobe_path, (char*) module_name, errno);
    }
    return ret;
}

/**
 * request_module - try to load a kernel module
 * @module_name: Name of module
 *
 * Load a module using the user mode module loader. The function returns
 * zero on success or a negative errno code on failure. Note that a
 * successful module load does not mean the module did not then unload
 * and exit on an error of its own. Callers must check that the service
 * they requested is now available not blindly invoke it.
 *
 * If module auto-loading support is disabled then this function
 * becomes a no-operation.
 */
int request_module(const char * module_name)
{
    pid_t pid;
    int waitpid_result;
    sigset_t tmpsig;
    int i;
    static atomic_t kmod_concurrent = ATOMIC_INIT(0);
#define MAX_KMOD_CONCURRENT 50  /* Completely arbitrary value - KAO */
    static int kmod_loop_msg;

    /* Don't allow request_module() before the root fs is mounted!  */
    if ( ! current->fs->root )
    {
        printk(KERN_ERR "request_module[%s]: Root fs not mounted\n",
               module_name);
        return -EPERM;
    }

    /* If modprobe needs a service that is in a module, we get a recursive
     * loop.  Limit the number of running kmod threads to max_threads/2 or
     * MAX_KMOD_CONCURRENT, whichever is the smaller.  A cleaner method
     * would be to run the parents of this process, counting how many times
     * kmod was invoked.  That would mean accessing the internals of the
     * process tables to get the command line, proc_pid_cmdline is static
     * and it is not worth changing the proc code just to handle this case.
     * KAO.
     */
    i = max_threads/2;
    if (i > MAX_KMOD_CONCURRENT)
        i = MAX_KMOD_CONCURRENT;
    atomic_inc(&kmod_concurrent);
    if (atomic_read(&kmod_concurrent) > i)
    {
        if (kmod_loop_msg++ < 5)
            printk(KERN_ERR
                   "kmod: runaway modprobe loop assumed and stopped\n");
        atomic_dec(&kmod_concurrent);
        return -ENOMEM;
    }

    pid = kernel_thread(exec_modprobe, (void*) module_name, 0);
    if (pid < 0)
    {
        printk(KERN_ERR "request_module[%s]: fork failed, errno %d\n", module_name, -pid);
        atomic_dec(&kmod_concurrent);
        return pid;
    }

    /* Block everything but SIGKILL/SIGSTOP */
    spin_lock_irq(&current->sigmask_lock);
    tmpsig = current->blocked;
    siginitsetinv(&current->blocked, sigmask(SIGKILL) | sigmask(SIGSTOP));
    recalc_sigpending(current);
    spin_unlock_irq(&current->sigmask_lock);

    waitpid_result = waitpid(pid, NULL, __WCLONE);
    atomic_dec(&kmod_concurrent);

    /* Allow signals again.. */
    spin_lock_irq(&current->sigmask_lock);
    current->blocked = tmpsig;
    recalc_sigpending(current);
    spin_unlock_irq(&current->sigmask_lock);

    if (waitpid_result != pid)
    {
        printk(KERN_ERR "request_module[%s]: waitpid(%d,...) failed, errno %d\n",
               module_name, pid, -waitpid_result);
    }
    return 0;
}
#endif /* CONFIG_KMOD */


#ifdef CONFIG_HOTPLUG
/*
    hotplug path is set via /proc/sys
    invoked by hotplug-aware bus drivers,
    with exec_usermodehelper and some thread-spawner

    argv [0] = hotplug_path;
    argv [1] = "usb", "scsi", "pci", "network", etc;
    ... plus optional type-specific parameters
    argv [n] = 0;

    envp [*] = HOME, PATH; optional type-specific parameters

    a hotplug bus should invoke this for device add/remove
    events.  the command is expected to load drivers when
    necessary, and may perform additional system setup.
*/
char hotplug_path[256] = "/sbin/hotplug";

EXPORT_SYMBOL(hotplug_path);

#endif /* CONFIG_HOTPLUG */

struct subprocess_info
{
    struct completion *complete;
    char *path;
    char **argv;
    char **envp;
    pid_t retval;
};

/*
 * This is the task which runs the usermode application
 */
static int ____call_usermodehelper(void *data)
{
    struct subprocess_info *sub_info = data;
    int retval;

    retval = -EPERM;
    if (current->fs->root)
        retval = exec_usermodehelper(sub_info->path, sub_info->argv, sub_info->envp);

    /* Exec failed? */
    sub_info->retval = (pid_t)retval;
    do_exit(0);
}

/*
 * This is run by keventd.
 */
static void __call_usermodehelper(void *data)
{
    struct subprocess_info *sub_info = data;
    pid_t pid;

    /*
     * CLONE_VFORK: wait until the usermode helper has execve'd successfully
     * We need the data structures to stay around until that is done.
     */
    pid = kernel_thread(____call_usermodehelper, sub_info, CLONE_VFORK | SIGCHLD);
    if (pid < 0)
        sub_info->retval = pid;
    complete(sub_info->complete);
}

/**
 * call_usermodehelper - start a usermode application
 * @path: pathname for the application
 * @argv: null-terminated argument list
 * @envp: null-terminated environment list
 *
 * Runs a user-space application.  The application is started asynchronously.  It
 * runs as a child of keventd.  It runs with full root capabilities.  keventd silently
 * reaps the child when it exits.
 *
 * Must be called from process context.  Returns zero on success, else a negative
 * error code.
 */
int call_usermodehelper(char *path, char **argv, char **envp)
{
    DECLARE_COMPLETION(work);
    struct subprocess_info sub_info =
    {
complete:
        &work,
path:
        path,
argv:
        argv,
envp:
        envp,
        retval:     0,
    };
    struct tq_struct tqs =
    {
routine:
        __call_usermodehelper,
data:
        &sub_info,
    };

    if (path[0] == '\0')
        goto out;

    __call_usermodehelper(&sub_info);
out:
    return sub_info.retval;
}

/*
 * This is for the serialisation of device probe() functions
 * against device open() functions
 */
static DECLARE_MUTEX(dev_probe_sem);

void dev_probe_lock(void)
{
    down(&dev_probe_sem);
}

void dev_probe_unlock(void)
{
    up(&dev_probe_sem);
}

EXPORT_SYMBOL(exec_usermodehelper);
EXPORT_SYMBOL(call_usermodehelper);

#ifdef CONFIG_KMOD
EXPORT_SYMBOL(request_module);
#endif

