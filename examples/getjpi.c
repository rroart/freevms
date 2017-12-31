#include <stdio.h>
#include <ssdef.h>
#include <descrip.h>
#include <jpidef.h>
#include <starlet.h>
#include <misc.h>

main()
{
    int i;
    $DESCRIPTOR(gsdnam1,"gsd1");
    $DESCRIPTOR(gsdnam2,"gsd2");
    struct item_list_3 lst[4];
    char procname[15];
    char proclen;
    unsigned long upid,epid;
    unsigned long upidlen,epidlen;
    int jpistatus;

    lst[0].buflen=15;
    lst[0].item_code=JPI$_PRCNAM;
    lst[0].bufaddr=procname;
    lst[0].retlenaddr=&proclen;
    lst[1].buflen=4;
    lst[1].item_code=JPI$_PID;
    lst[1].bufaddr=&epid;
    lst[1].retlenaddr=&epidlen;
    lst[2].buflen=4;
    lst[2].item_code=JPI$_MASTER_PID;
    lst[2].bufaddr=&upid;
    lst[2].retlenaddr=&upidlen;
    lst[3].buflen=0;
    lst[3].item_code=0;
    do
    {
        jpistatus=sys$getjpi(0,0,0,lst,0,0,0);
        printf("%x %x %s\n",upid,epid,procname);
    }
    while (jpistatus == SS$_NORMAL);
//} while (jpistatus != SS$_NOMOREPROC);
    printf("jpistatus %x\n",jpistatus);
}
