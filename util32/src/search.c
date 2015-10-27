#define MODULE_NAME ODS2
#define MODULE_IDENT    "V1.3"

/*     Ods2.c v1.3   Mainline ODS2 program   */

/*
        This is part of ODS2 written by Paul Nankervis,
        email address:  Paulnank@au1.ibm.com

        ODS2 is distributed freely for all members of the
        VMS community to use. However all derived works
        must maintain comments in their source to acknowledge
        the contibution of the original author.

        The modules in ODS2 are:-

                ACCESS.C        Routines for accessing ODS2 disks
                CACHE.C         Routines for managing memory cache
                DEVICE.C        Routines to maintain device information
                DIRECT.C        Routines for handling directories
                ODS2.C          The mainline program
                PHYVMS.C        Routine to perform physical I/O
                RMS.C           Routines to handle RMS structures
                VMSTIME.C       Routines to handle VMS times

        On non-VMS platforms PHYVMS.C should be replaced as follows:-

                OS/2            PHYOS2.C
                Windows 95/NT   PHYNT.C

        For example under OS/2 the program is compiled using the GCC
        compiler with the single command:-

                gcc -fdollars-in-identifiers ods2.c,rms.c,direct.c,
                      access.c,device.c,cache.c,phyos2.c,vmstime.c
*/

/* Modified by:
 *
 *   31-AUG-2001 01:04  Hunter Goatley <goathunter@goatley.com>
 *
 *  For VMS, added routine getcmd() to read commands with full
 *  command recall capabilities.
 *
 */

/*  This is the top level set of routines. It is fairly
    simple minded asking the user for a command, doing some
    primitive command parsing, and then calling a set of routines
    to perform whatever function is required (for example COPY).
    Some routines are implemented in different ways to test the
    underlying routines - for example TYPE is implemented without
    a NAM block meaning that it cannot support wildcards...
    (sorry! - could be easily fixed though!)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <descrip.h>
#include <fabdef.h>
#include <fiddef.h>
#include <misc.h>
#include <namdef.h>
#include <rabdef.h>
#include <rmsdef.h>
#include <ssdef.h>
#include <uicdef.h>
#include <xabdef.h>
#include <xabdatdef.h>
#include <xabfhcdef.h>
#include <xabprodef.h>
#include <hm2def.h>
#include "../../rms/src/cache.h"
#include "../../f11x/src/access.h"
#include <starlet.h>
#include <cli$routines.h>
//#include "rms.h"

//extern struct _nam cc$rms_nam;
//extern struct _fabdef cc$rms_fab;

#if 0
struct _fabdef cc$rms_fab = {NULL,0,NULL,NULL,0,0,0,0,0,0,0,0,0,0,0,0,0,NULL};
struct _namdef cc$rms_nam = {0,0,0,0,0,0,0,0,0,0,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,0,0};
struct _xabdatdef cc$rms_xabdat = {XAB$C_DAT,0,
           0, 0, 0, 0,
           0, 0,
           0, 0,
           0, 0
};
struct _xabfhcdef cc$rms_xabfhc = {XAB$C_FHC,0,0,0,0,0,0,0,0,0,0,0};
struct _xabprodef1 cc$rms_xabpro = {XAB$C_PRO,0,0,0};
struct _rabdef cc$rms_rab = {NULL,NULL,NULL,NULL,0,0,0,{0,0,0}};
#endif

#define PRINT_ATTR (FAB$M_CR | FAB$M_PRN | FAB$M_FTN)

void main()
{
    printf("main is a dummy\n");
    return 1;
}

#define MAXREC 32767

/* search: a simple file search routine */

unsigned search(int userarg)
{
    setvbuf(stdout, NULL, _IONBF, 0);      // need this to see i/o at all
    int sts;
    $DESCRIPTOR(p1, "p1");
    $DESCRIPTOR(p2, "p2");
    char c1[80];
    struct dsc$descriptor o1;
    o1.dsc$a_pointer=c1;
    o1.dsc$w_length=80;
    memset (c1, 0, 80);
    char c2[80];
    struct dsc$descriptor o2;
    o2.dsc$a_pointer=c2;
    o2.dsc$w_length=80;
    memset (c2, 0, 80);
    sts = cli$present(&p1);
    if ((sts&1)==0)
        return sts;
    sts = cli$present(&p2);
    if ((sts&1)==0)
        return sts;
    int retlen;
    sts = cli$get_value(&p1, &o1, &retlen);
    sts = cli$get_value(&p2, &o2, &retlen);

    int filecount = 0;
    int findcount = 0;
    char res[NAM$C_MAXRSS + 1],rsa[NAM$C_MAXRSS + 1];
    struct _namdef nam = cc$rms_nam;
    struct _fabdef fab = cc$rms_fab;
    register char *searstr = c2;
    register char firstch = tolower(*searstr++);
    register char *searend = searstr + strlen(searstr);
    {
        char *str = searstr;
        while (str < searend)
        {
            *str = tolower(*str);
            str++;
        }
    }
    nam = cc$rms_nam;
    fab = cc$rms_fab;
    nam.nam$l_esa = res;
    nam.nam$b_ess = NAM$C_MAXRSS;
    fab.fab$l_nam = &nam;
    fab.fab$l_fna = c1;
    fab.fab$b_fns = strlen(fab.fab$l_fna);
    fab.fab$l_dna = "";
    fab.fab$b_dns = strlen(fab.fab$l_dna);
    sts = sys$parse(&fab, 0, 0);
    if (sts & 1)
    {
        nam.nam$l_rsa = rsa;
        nam.nam$b_rss = NAM$C_MAXRSS;
        fab.fab$l_fop = FAB$M_NAM;
        while ((sts = sys$search(&fab, 0, 0)) & 1)
        {
            sts = sys$open(&fab, 0, 0);
            if ((sts & 1) == 0)
            {
                printf("%%SEARCH-F-OPENFAIL, Open error: %d\n",sts);
            }
            else
            {
                struct _rabdef rab = cc$rms_rab;
                rab.rab$l_fab = &fab;
                if ((sts = sys$connect(&rab, 0, 0)) & 1)
                {
                    int printname = 1;
                    char rec[MAXREC + 2];
                    filecount++;
                    rab.rab$l_ubf = rec;
                    rab.rab$w_usz = MAXREC;
                    while ((sts = sys$get(&rab, 0, 0)) & 1)
                    {
                        register char *strng = rec;
                        register char *strngend = strng + (rab.rab$w_rsz - (searend - searstr));
                        while (strng < strngend)
                        {
                            register char ch = *strng++;
                            if (ch == firstch || (ch >= 'A' && ch <= 'Z' && ch + 32 == firstch))
                            {
                                register char *str = strng;
                                register char *cmp = searstr;
                                while (cmp < searend)
                                {
                                    register char ch2 = *str++;
                                    ch = *cmp;
                                    if (ch2 != ch && (ch2 < 'A' || ch2 > 'Z' || ch2 + 32 != ch)) break;
                                    cmp++;
                                }
                                if (cmp >= searend)
                                {
                                    findcount++;
                                    rec[rab.rab$w_rsz] = '\0';
                                    if (printname)
                                    {
                                        rsa[nam.nam$b_rsl] = '\0';
                                        printf("\n******************************\n%s\n\n",rsa);
                                        printname = 0;
                                    }
                                    fputs(rec,stdout);
                                    if (fab.fab$b_rat & PRINT_ATTR) fputc('\n',stdout);
                                    break;
                                }
                            }
                        }
                    }
                    sys$disconnect(&rab, 0, 0);
                }
                if (sts == SS$_NOTINSTALL)
                {
                    printf("%%SEARCH-W-NOIMPLEM, file operation not implemented\n");
                    sts = 1;
                }
                sys$close(&fab, 0, 0);
            }
        }
        if (sts == RMS$_NMF || sts == RMS$_FNF) sts = 1;
    }
    if (sts & 1)
    {
        if (filecount < 1)
        {
            printf("%%SEARCH-W-NOFILES, no files found\n");
        }
        else
        {
            if (findcount < 1) printf("%%SEARCH-I-NOMATCHES, no strings matched\n");
        }
    }
    else
    {
        printf("%%SEARCH-F-ERROR Status: %d\n",sts);
    }
    return sts;
}
