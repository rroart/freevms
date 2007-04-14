#define MODULE_NAME	ODS2
#define MODULE_IDENT	"V1.3"

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
 *   31-AUG-2001 01:04	Hunter Goatley <goathunter@goatley.com>
 *
 *	For VMS, added routine getcmd() to read commands with full
 *	command recall capabilities.
 *
 */

/*  This version will compile and run using normal VMS I/O by
    defining VMSIO
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

#ifdef VMS
#ifdef __DECC
#pragma module MODULE_NAME MODULE_IDENT
#else
#ifdef vaxc
#module MODULE_NAME MODULE_IDENT
#endif /* vaxc */
#endif /* __DECC */
#endif /* VMS */

#define DEBUGx on
#define VMSIOx on

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#ifdef VMSIO
#include <ssdef.h>
#include <descrip.h>
#include <starlet.h>
#include <rms.h>
#include <fiddef.h>
#define sys_parse       sys$parse
#define sys_search      sys$search
#define sys_open        sys$open
#define sys_close       sys$close
#define sys_connect     sys$connect
#define sys_disconnect  sys$disconnect
#define sys_get         sys$get
#define sys_put         sys$put
#define sys_create      sys$create
#define sys_erase       sys$erase
#define sys_extend      sys$extend
#define sys_asctim      sys$asctim
#define sys_setddir     sys$setddir
#define dsc_descriptor  dsc$descriptor
#define dsc_w_length    dsc$w_length
#define dsc_a_pointer   dsc$a_pointer

#else
#include <mytypes.h>
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
//#include "rms.h"
#endif

//extern struct _nam cc$rms_nam;
//extern struct _fab cc$rms_fab;

#if 0
struct _fabdef cc$rms_fab = {NULL,0,NULL,NULL,0,0,0,0,0,0,0,0,0,0,0,0,0,NULL};
struct _namdef cc$rms_nam = {0,0,0,0,0,0,0,0,0,0,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,0,0};
struct _xabdatdef cc$rms_xabdat = {XAB$C_DAT,0,
				   0, 0, 0, 0,
			       VMSTIME_ZERO, VMSTIME_ZERO,
			       VMSTIME_ZERO, VMSTIME_ZERO,
			       VMSTIME_ZERO, VMSTIME_ZERO};
struct _xabfhcdef cc$rms_xabfhc = {XAB$C_FHC,0,0,0,0,0,0,0,0,0,0,0};
struct _xabprodef1 cc$rms_xabpro = {XAB$C_PRO,0,0,0};
struct _rabdef cc$rms_rab = {NULL,NULL,NULL,NULL,0,0,0,{0,0,0}};
#endif

#define PRINT_ATTR (FAB$M_CR | FAB$M_PRN | FAB$M_FTN)

void main() {
  printf("main is a dummy\n");
  return 1;
}

#define MAXREC 32767

/* typ: a file TYPE routine */

unsigned typ(int userarg)
{
    setvbuf(stdout, NULL, _IONBF, 0);      // need this to see i/o at all
    int sts;
    $DESCRIPTOR(p, "p1");
    char c[80];
    struct dsc$descriptor o;
    o.dsc$a_pointer=c;
    o.dsc$w_length=80;
    memset (c, 0, 80);
    sts = cli$present(&p);
    if ((sts&1)==0)
      return sts;
    int retlen;
    sts = cli$get_value(&p, &o, &retlen);
    int records = 0;
    struct _fabdef fab = cc$rms_fab;
    fab.fab$l_fna = c;
    fab.fab$b_fns = strlen(fab.fab$l_fna);
    if ((sts = sys$open(&fab)) & 1) {
        struct _rabdef rab = cc$rms_rab;
        rab.rab$l_fab = &fab;
        if ((sts = sys$connect(&rab)) & 1) {
            char rec[MAXREC + 2];
            rab.rab$l_ubf = rec;
            rab.rab$w_usz = MAXREC;
            while ((sts = sys$get(&rab)) & 1) {
                unsigned rsz = rab.rab$w_rsz;
                if (fab.fab$b_rat & PRINT_ATTR) rec[rsz++] = '\n';
                rec[rsz++] = '\0';
                fputs(rec,stdout);
                records++;
            }
            sys$disconnect(&rab);
        }
        sys$close(&fab);
        if (sts == RMS$_EOF) sts = 1;
    }
    if ((sts & 1) == 0) {
        printf("%%TYPE-F-ERROR Status: %d\n",sts);
    }
    return sts;
}
