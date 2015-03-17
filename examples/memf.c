#include <stdio.h>
#include <ssdef.h>
#include <descrip.h>
#include <va_rangedef.h>
#include <seciddef.h>
#include <secdef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


main()
{
    int i,fd;
    $DESCRIPTOR(gsdnam1,"gsd3");
    $DESCRIPTOR(gsdnam2,"gsd4");
    struct _va_range range1, range2;
    struct _secid id1, id2;
    range1.va_range$ps_start_va=0x10000000;
    range1.va_range$ps_end_va=0x10008000;
    range2.va_range$ps_start_va=0x20020000;
    range2.va_range$ps_end_va=0x20030000;
    id1.secid$l_version=0;
    id2.secid$l_version=0;
    fd=open("/file",O_RDONLY);

    sys$create_region_32(0x8000,0,0,0,0,0,range1.va_range$ps_start_va);
    sys$create_region_32(0x10000,0,0,0,0,0,range2.va_range$ps_start_va);

    sys$crmpsc(&range1,0,0,SEC$M_GBL,&gsdnam1,&id1,0,fd,2,0,0,0);
    sys$crmpsc(&range2,0,0,SEC$M_GBL,&gsdnam2,&id2,0,fd,2,1,0,0);

    for(i=0; i<4; i++)
    {
        volatile unsigned long * l1=range1.va_range$ps_start_va+0x2000;
        volatile unsigned long * l2=range2.va_range$ps_start_va+0x1000;
        (*l1)++;
        printf("%x %x\n",getpid(),*l1);
        sleep(2);
        (*l2)++;
        printf("%x %x\n",getpid(),*l2);
        sleep(2);
    }
    close(fd);
}
