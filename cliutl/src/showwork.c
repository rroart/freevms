// $Id$
// $Locker$

// Author. Roar Thron�s.

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include <ssdef.h>
#include <descrip.h>
#include <jpidef.h>
#include <starlet.h>
#include <misc.h>

void show_working_set(void)
{
    struct item_list_3 lst[14];
    char procname[15];
    char proclen;
    unsigned long upid, epid;
    unsigned long upidlen, epidlen;
    unsigned long wssize, wsquota, wsextent, wsauth, wsauthext;
    unsigned long wssizelen, wsquotalen, wsextentlen, wsauthlen, wsauthextlen;
    int jpistatus;
    int mypid = getpid();
    if (mypid == 1)
        mypid = 2;

    do
    {
        lst[0].buflen = 15;
        lst[0].item_code = JPI$_PRCNAM;
        lst[0].bufaddr = procname;
        lst[0].retlenaddr = &proclen;
        lst[1].buflen = sizeof(epid);
        lst[1].item_code = JPI$_PID;
        lst[1].bufaddr = &epid;
        lst[1].retlenaddr = &epidlen;
        lst[2].buflen = sizeof(upid);
        lst[2].item_code = JPI$_MASTER_PID;
        lst[2].bufaddr = &upid;
        lst[2].retlenaddr = &upidlen;
        lst[3].buflen = sizeof(wsauth);
        lst[3].item_code = JPI$_WSAUTH;
        lst[3].bufaddr = &wsauth;
        lst[3].retlenaddr = &wsauthlen;
        lst[4].buflen = sizeof(wsquota);
        lst[4].item_code = JPI$_WSQUOTA;
        lst[4].bufaddr = &wsquota;
        lst[4].retlenaddr = &wsquotalen;
        lst[5].buflen = sizeof(wssize);
        lst[5].item_code = JPI$_WSSIZE;
        lst[5].bufaddr = &wssize;
        lst[5].retlenaddr = &wssizelen;
        lst[6].buflen = sizeof(wsextent);
        lst[6].item_code = JPI$_WSEXTENT;
        lst[6].bufaddr = &wsextent;
        lst[6].retlenaddr = &wsextentlen;
        lst[7].buflen = sizeof(wsauthext);
        lst[7].item_code = JPI$_WSAUTHEXT;
        lst[7].bufaddr = &wsauthext;
        lst[7].retlenaddr = &wsauthextlen;
        lst[8].buflen = 0;
        lst[8].item_code = 0;

        jpistatus = sys$getjpi(0, 0, 0, lst, 0, 0, 0);
        if (jpistatus == SS$_NORMAL)
            if (mypid == upid)
            {
                printf("  Working Set      /Limit=%5lx /Quota=%5lx   /Extent=%5lx\n", wssize, wsquota, wsextent);
                printf("  Adjustment enabled      Authorized Quota=%5lx Authorized Extent=%5lx\n", wsauth, wsauthext);
            }
    }
    while (jpistatus == SS$_NORMAL);
//} while (jpistatus != SS$_NOMOREPROC);
}
