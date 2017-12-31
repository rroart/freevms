#include <linux/kernel.h>

struct _fab;

int  exe$delete  (struct _fab * fab, void * err, void * suc)
{
    printk("not implemented\n");
}
int  exe$enter  (struct _fab * fab, void * err, void * suc)
{
    printk("not implemented\n");
}
//int  exe$erase  (struct _fab * fab, void * err, void * suc) { printk("not implemented\n"); }
int  exe$find  (struct _fab * fab, void * err, void * suc)
{
    printk("not implemented\n");
}
int  exe$flush  (struct _fab * fab, void * err, void * suc)
{
    printk("not implemented\n");
}
int  exe$free  (struct _fab * fab, void * err, void * suc)
{
    printk("not implemented\n");
}
int  exe$modify  (struct _fab * fab, void * err, void * suc)
{
    printk("not implemented\n");
}
int  exe$nxtvol  (struct _fab * fab, void * err, void * suc)
{
    printk("not implemented\n");
}
#if 0
int  exe$read  (struct _fab * fab, void * err, void * suc)
{
    printk("not implemented\n");
}
#endif
int  exe$release  (struct _fab * fab, void * err, void * suc)
{
    printk("not implemented\n");
}
int  exe$remove  (struct _fab * fab, void * err, void * suc)
{
    printk("not implemented\n");
}
int  exe$rename  (struct _fab * fab, void * err, void * suc)
{
    printk("not implemented\n");
}
int  exe$rewind  (struct _fab * fab, void * err, void * suc)
{
    printk("not implemented\n");
}
int  exe$space  (struct _fab * fab, void * err, void * suc)
{
    printk("not implemented\n");
}
int  exe$truncate  (struct _fab * fab, void * err, void * suc)
{
    printk("not implemented\n");
}
int  exe$update  (struct _fab * fab, void * err, void * suc)
{
    printk("not implemented\n");
}
int  rms$wait  (struct _fab * fab, void * err, void * suc)
{
    printk("not implemented\n");
}
#if 0
int  exe$write  (struct _fab * fab, void * err, void * suc)
{
    printk("not implemented\n");
}
#endif
int  exe$filescan  (struct _fab * fab, void * err, void * suc)
{
    printk("not implemented\n");
}
//int  exe$setddir  (struct _fab * fab, void * err, void * suc) { printk("not implemented\n"); }
int  exe$setdfprot  (struct _fab * fab, void * err, void * suc)
{
    printk("not implemented\n");
}
int  exe$ssvexc  (struct _fab * fab, void * err, void * suc)
{
    printk("not implemented\n");
}
int  exe$rmsrundwn  (struct _fab * fab, void * err, void * suc)
{
    printk("not implemented\n");
}

// workaround for bug or something

#ifdef __i386__
char * strcpy(char * dest,const char *src)
{
    int d0, d1, d2;
    __asm__ __volatile__(
        "1:\tlodsb\n\t"
        "stosb\n\t"
        "testb %%al,%%al\n\t"
        "jne 1b"
        : "=&S" (d0), "=&D" (d1), "=&a" (d2)
        :"0" (src),"1" (dest) : "memory");
    return dest;
}
#endif
