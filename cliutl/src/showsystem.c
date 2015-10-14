// $Id$
// $Locker$

// Author. Roar Thron�s.

#include <stdio.h>
#include <string.h>

#include <ssdef.h>
#include <descrip.h>
#include <jpidef.h>
#include <starlet.h>
#include <syidef.h>
#include <misc.h>

#include "../../linux/include/linux/version.h"

static char * states[] =
    { "NONE", "COLPG", "MWAIT", "CEF", "PFW", "LEF", "LEFO", "HIB", "HIBO", "SUSP", "SUSPO", "FPG", "COM", "COMO", "CUR" };

void show_system(void)
{
    struct item_list_3 lst[14], syilst[4];
    char scsnode[16];
    char procname[15];
    unsigned short proclen;
    char version[16];
    unsigned short versionlen;
    unsigned long long boottime;
    unsigned short boottimelen;
    unsigned long epid;
    unsigned short epidlen;
    unsigned long upid;
    unsigned short upidlen;
    unsigned short state;
    unsigned short statelen;
    unsigned char pri;
    unsigned short prilen;
    unsigned long pagep;
    unsigned short pageplen;
    unsigned long pageg;
    unsigned short pageglen;
    unsigned long pagef;
    unsigned short pageflen;
    unsigned int dirio;
    unsigned short diriolen;
    unsigned int bufio;
    unsigned short bufiolen;
    unsigned long long cputime;
    unsigned short cputimelen;

    int sts;
    int jpistatus;
    int retscsnodelen;

    struct _generic_64 now;
    char timestr[23];
    $DESCRIPTOR(atimenow, timestr);
    char timestr2[23];
    $DESCRIPTOR(atimenow2, timestr2);

    sys$gettim(&now);
    sys$asctim(0, &atimenow, &now, 0);

    syilst[0].buflen = 16;
    syilst[0].item_code = SYI$_VERSION;
    syilst[0].bufaddr = version;
    syilst[0].retlenaddr = &versionlen;
    syilst[1].buflen = sizeof(boottime);
    syilst[1].item_code = SYI$_BOOTTIME;
    syilst[1].bufaddr = &boottime;
    syilst[1].retlenaddr = &boottimelen;
    syilst[2].buflen = 16;
    syilst[2].item_code = SYI$_SCSNODE;
    syilst[2].bufaddr = scsnode;
    syilst[2].retlenaddr = &retscsnodelen;
    syilst[3].buflen = 0;
    syilst[3].item_code = 0;

    sts = sys$getsyi(0, 0, 0, syilst, 0, 0, 0);

    struct _generic_64 delta;
    delta.gen64$r_quad_overlay.gen64$q_quadword = boottime - now.gen64$r_quad_overlay.gen64$q_quadword;
    sys$asctim(0, &atimenow2, &delta, 0);

#ifdef __x86_64__
    int bits = 64;
#endif
#ifdef __i386__
    int bits = 32;
#endif
#ifndef FREEVMS_BUILD
#define FREEVMS_BUILD 1
#endif
    printf(" FreeVMS %d V%s build %d on node %6s  %s  Uptime  %s\n", bits, version, FREEVMS_BUILD, scsnode, timestr, timestr2);
    printf("  Pid    Process Name    State  Pri      I/O       CPU       Page flts  Pages\n");

    do
    {
        memset(procname, 0, 15);
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
        lst[3].buflen = sizeof(state);
        lst[3].item_code = JPI$_STATE;
        lst[3].bufaddr = &state;
        lst[3].retlenaddr = &statelen;
        lst[4].buflen = sizeof(pagef);
        lst[4].item_code = JPI$_PAGEFLTS;
        lst[4].bufaddr = &pagef;
        lst[4].retlenaddr = &pageflen;
        lst[5].buflen = sizeof(pri);
        lst[5].item_code = JPI$_PRI;
        lst[5].bufaddr = &pri;
        lst[5].retlenaddr = &prilen;
        lst[6].buflen = sizeof(pagep);
        lst[6].item_code = JPI$_PPGCNT;
        lst[6].bufaddr = &pagep;
        lst[6].retlenaddr = &pageplen;
        lst[7].buflen = sizeof(pageg);
        lst[7].item_code = JPI$_GPGCNT;
        lst[7].bufaddr = &pageg;
        lst[7].retlenaddr = &pageglen;
        lst[8].buflen = sizeof(dirio);
        lst[8].item_code = JPI$_DIRIO;
        lst[8].bufaddr = &dirio;
        lst[8].retlenaddr = &diriolen;
        lst[9].buflen = sizeof(bufio);
        lst[9].item_code = JPI$_BUFIO;
        lst[9].bufaddr = &bufio;
        lst[9].retlenaddr = &bufiolen;
        lst[10].buflen = sizeof(cputime);
        lst[10].item_code = JPI$_CPUTIM;
        lst[10].bufaddr = &cputime;
        lst[10].retlenaddr = &cputimelen;
        lst[11].buflen = 0;
        lst[11].item_code = 0;
        jpistatus = sys$getjpi(0, 0, 0, lst, 0, 0, 0);
        if (jpistatus == SS$_NORMAL)
        {
            delta.gen64$r_quad_overlay.gen64$q_quadword = -cputime * 100000; // check multiplication
            unsigned short timelen2 = 0;
            memset(timestr2, 0, 23);
            sts = sys$asctim(&timelen2, &atimenow2, &delta, 0);
            printf("%8lx %-15s %-6s %3x %9x %17s %6lx %6lx\n", epid, procname, states[state], 31 - pri, dirio + bufio,
                    timestr2, pagef, pagep + pageg);
        }
    }
    while (jpistatus == SS$_NORMAL);
//    while (jpistatus != SS$_NOMOREPROC);
}
