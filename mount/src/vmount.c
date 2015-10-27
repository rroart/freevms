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
#include <uicdef.h>
#include <hm2def.h>
#include <iodef.h>
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

/* The bits we need when we don't have real VMS routines underneath... */

#if 0
unsigned dodismount(int argc,char *argv[],int qualc,char *qualv[])
{
    struct DEV *dev;
    // note that device lookup interface has changed, fix later
    register int sts = device_lookup(strlen(argv[1]),argv[1],0,&dev);
    if (sts & 1)
    {
        if (dev->vcb != NULL)
        {
            sts = dismount(dev->vcb);
        }
        else
        {
            sts = SS$_DEVNOTMOUNT;
        }
    }
    if ((sts & 1) == 0) printf("%%DISMOUNT-E-STATUS Error: %d\n",sts);
    return sts;
}
#endif

void main()
{
    printf("main is a dummy\n");
    return 1;
}

unsigned domount(int userarg)
{
    setvbuf(stdout, NULL, _IONBF, 0);      // need this to see i/o at all
    int sts;
    $DESCRIPTOR(bind, "bind");
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

    char bindnam[80];
    struct dsc$descriptor binddes;
    binddes.dsc$a_pointer=bindnam;
    binddes.dsc$w_length=80;
    memset (bindnam, 0, 80);
    int bind_sts = cli$present(&bind);
    if (bind_sts&1)
    {
        sts = cli$get_value(&bind, &binddes, &retlen);
        binddes.dsc$w_length = retlen;
    }

    char *dev = c;
    char *lab = 0;
    int devices = 0;
    char *devs[100],*labs[100];
#if 0
    while (*lab != '\0')
    {
        labs[devices++] = lab;
        while (*lab != ',' && *lab != '\0') lab++;
        if (*lab != '\0')
        {
            *lab++ = '\0';
        }
        else
        {
            break;
        }
    }
#endif
#if 0
    // amd64 not happy with this?
    while (cli$get_value(&p, &o, &retlen)&1)
    {
        devs[devices++] = strdup(c);
    }
#else
    cli$get_value(&p, &o, &retlen);
    devs[devices++] = strdup(c);
#endif
    if (bind_sts&1)
    {
        for (dev = 0; dev < devices; dev ++)
        {
            short int chan;
            char buf[512];
            struct _hm2 * hm2 = buf;
            struct dsc$descriptor dsc;
            dsc.dsc$w_length = strlen(devices[dev]);
            dsc.dsc$a_pointer = devices[dev];
            sts=sys$assign(&dsc,&chan,0,0,0);
            char * nam = devices[dev];
            int part = nam[3] - '1';
            struct _iosb iosb;
            sts = sys$qiow(0, chan, IO$_READPBLK, &iosb, 0, 0, buf , 512, 1, part, 0, 0);
            hm2->hm2$w_rvn = dev + 1;
            memcpy(hm2->hm2$t_strucname, binddes.dsc$a_pointer, binddes.dsc$w_length);
            sts = sys$qiow(0, chan, IO$_WRITEPBLK, &iosb, 0, 0, buf , 512, 1, part, 0, 0);
            sys$dassgn(chan);
        }
        return 1;
    }
#if 0
    devices = 0;
    while (*dev != '\0')
    {
        devs[devices++] = dev;
        while (*dev != ',' && *dev != '\0') dev++;
        if (*dev != '\0')
        {
            *dev++ = '\0';
        }
        else
        {
            break;
        }
    }
#endif
    if (devices > 0)
    {
        unsigned i;
        struct VCB *vcb;
        struct item_list_3 it[2];
        it[0].item_code=1; /*not yet */
        it[0].buflen=strlen(devs[0]);
        it[0].bufaddr=devs[0];
        it[1].item_code=0;
        //        sts = mount(options,devices,devs,labs,&vcb);
        sts = sys$mount(it);
#if 0
        if (sts & 1)
        {
            for (i = 0; i < vcb->devices; i++)
                if (vcb->vcbdev[i].dev != NULL)
                    printf("%%MOUNT-I-MOUNTED, Volume %12.12s mounted on %s\n",
                           vcb->vcbdev[i].home.hm2$t_volname,vcb->vcbdev[i].dev->devnam);
            if (setdef_count == 0)
            {
                char *colon,defdir[256];
                strcpy(defdir,vcb->vcbdev[0].dev->devnam);
                colon = strchr(defdir,':');
                if (colon != NULL) *colon = '\0';
                strcpy(defdir + strlen(defdir),":[000000]");
                setdef(defdir);
                test_vcb = vcb;
            }
        }
        else
        {
            printf("Mount failed with %d\n",sts);
        }
#endif
    }
    return sts;
}
