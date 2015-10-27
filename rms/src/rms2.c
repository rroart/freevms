#include <linux/kernel.h>

#include <fabdef.h>
#include <rabdef.h>

int  exe$delete  (struct _rabdef * rab, void * err, void * suc)
{
    printk("not implemented\n");
    return 0;
}
int  exe$enter  (struct _fabdef * fab, void * err, void * suc)
{
    printk("not implemented\n");
    return 0;
}
int  exe$find  (struct _rabdef * rab, void * err, void * suc)
{
    printk("not implemented\n");
    return 0;
}
int  exe$flush  (struct _rabdef * rab, void * err, void * suc)
{
    printk("not implemented\n");
    return 0;
}
int  exe$free  (struct _rabdef * rab, void * err, void * suc)
{
    printk("not implemented\n");
    return 0;
}
int  exe$modify  (struct _fabdef * fab, void * err, void * suc)
{
    printk("not implemented\n");
    return 0;
}
int  exe$nxtvol  (struct _rabdef * rab, void * err, void * suc)
{
    printk("not implemented\n");
    return 0;
}
int  exe$release  (struct _rabdef * rab, void * err, void * suc)
{
    printk("not implemented\n");
    return 0;
}
int  exe$remove  (struct _fabdef * fab, void * err, void * suc)
{
    printk("not implemented\n");
    return 0;
}
int  exe$rename  (struct _fabdef * oldfab, void * err, void * suc, struct _fabdef * newfab)
{
    printk("not implemented\n");
    return 0;
}
int  exe$rewind  (struct _rabdef * rab, void * err, void * suc)
{
    printk("not implemented\n");
    return 0;
}
int  exe$space  (struct _rabdef * rab, void * err, void * suc)
{
    printk("not implemented\n");
    return 0;
}
int  exe$truncate  (struct _rabdef * rab, void * err, void * suc)
{
    printk("not implemented\n");
    return 0;
}
int  exe$update  (struct _rabdef * rab, void * err, void * suc)
{
    printk("not implemented\n");
    return 0;
}
int  rms$wait  (void * control_block)
{
    printk("not implemented\n");
    return 0;
}
int  exe$filescan  (void *srcstr, void *valuelst, unsigned int *fldflags, void *auxout, unsigned short int *retlen)
{
    printk("not implemented\n");
    return 0;
}
int  exe$setdfprot  (unsigned short int *newdefprotaddr, unsigned short int *curdefprotaddr)
{
    printk("not implemented\n");
    return 0;
}
int  exe$ssvexc  (void)
{
    printk("not implemented\n");
    return 0;
}
int  exe$rmsrundwn  (void *buffer_address, unsigned char *type_value)
{
    printk("not implemented\n");
    return 0;
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
