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
#include <string.h>
#include <ctype.h>

#include <acedef.h>
#include <descrip.h>
#include <fabdef.h>
#include <fiddef.h>
#include <iledef.h>
#include <misc.h>
#include <namdef.h>
#include <ossdef.h>
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

unsigned dir(int userarg)
{
    setvbuf(stdout, NULL, _IONBF, 0);      // need this to see i/o at all
    int sts;
    $DESCRIPTOR(acl, "acl");
    $DESCRIPTOR(date, "date");
    $DESCRIPTOR(size, "size");
    $DESCRIPTOR(file_id, "file_id");
    int acl_sts, date_sts, size_sts, file_id_sts;
    acl_sts=cli$present(&acl);
    date_sts=cli$present(&date);
    size_sts=cli$present(&size);
    file_id_sts=cli$present(&file_id);
    int options=(acl_sts|date_sts|size_sts|file_id_sts)&1;
    $DESCRIPTOR(p, "p1");
    char c[80];
    struct dsc$descriptor o;
    o.dsc$a_pointer=c;
    o.dsc$w_length=80;
    memset (c, 0, 80);
    sts = cli$present(&p);
    int retlen;
    sts = cli$get_value(&p, &o, &retlen);
    char res[NAM$C_MAXRSS + 1],rsa[NAM$C_MAXRSS + 1];
    int filecount = 0;
    struct _namdef nam = cc$rms_nam;
    struct _fabdef fab = cc$rms_fab;
    struct _xabdatdef dat = cc$rms_xabdat;
    struct _xabfhcdef fhc = cc$rms_xabfhc;
    nam.nam$l_esa = res;
    nam.nam$b_ess = NAM$C_MAXRSS;
    fab.fab$l_nam = &nam;
    fab.fab$l_xab = &dat;
    dat.xab$l_nxt = &fhc;
    fab.fab$l_fna = c;
    fab.fab$b_fns = strlen(fab.fab$l_fna);
    fab.fab$l_dna = "*.*;*";
    fab.fab$b_dns = strlen(fab.fab$l_dna);
    sts = sys$parse(&fab, 0, 0);
    if (sts & 1)
    {
        char dir[NAM$C_MAXRSS + 1];
        int namelen;
        int dirlen = 0;
        int dirfiles = 0,dircount = 0;
        int dirblocks = 0,totblocks = 0;
        int printcol = 0;
#ifdef DEBUG
        res[nam.nam$b_esl] = '\0';
        printf("Parse: %s\n",res);
#endif
        nam.nam$l_rsa = rsa;
        nam.nam$b_rss = NAM$C_MAXRSS;
        fab.fab$l_fop = FAB$M_NAM;
        while ((sts = sys$search(&fab, 0, 0)) & 1)
        {
            if (dirlen != nam.nam$b_dev + nam.nam$b_dir ||
                    memcmp(rsa,dir,nam.nam$b_dev + nam.nam$b_dir) != 0)
            {
                if (dirfiles > 0)
                {
                    if (printcol > 0) printf("\n");
                    printf("\nTotal of %d file%s",dirfiles,(dirfiles == 1 ? "" : "s"));
                    if (size_sts & 1)
                    {
                        printf(", %d block%s.\n",dirblocks,(dirblocks == 1 ? "" : "s"));
                    }
                    else
                    {
                        fputs(".\n",stdout);
                    }
                }
                dirlen = nam.nam$b_dev + nam.nam$b_dir;
                memcpy(dir,rsa,dirlen);
                dir[dirlen] = '\0';
                printf("\nDirectory %s\n\n",dir);
                filecount += dirfiles;
                totblocks += dirblocks;
                dircount++;
                dirfiles = 0;
                dirblocks = 0;
                printcol = 0;
            }
            rsa[nam.nam$b_rsl] = '\0';
            namelen = nam.nam$b_name + nam.nam$b_type + nam.nam$b_ver;
            if (options == 0)
            {
                if (printcol > 0)
                {
                    int newcol = (printcol + 20) / 20 * 20;
                    if (newcol + namelen >= 80)
                    {
                        fputs("\n",stdout);
                        printcol = 0;
                    }
                    else
                    {
                        printf("%*s",newcol - printcol," ");
                        printcol = newcol;
                    }
                }
                fputs(rsa + dirlen,stdout);
                printcol += namelen;
            }
            else
            {
                if (namelen > 18)
                {
                    printf("%s\n                   ",rsa + dirlen);
                }
                else
                {
                    printf("%-19s",rsa + dirlen);
                }
                sts = sys$open(&fab, 0, 0);
                if ((sts & 1) == 0)
                {
                    printf("Open error: %d\n",sts);
                }
                else
                {
                    sts = sys$close(&fab, 0, 0);
                    if (file_id_sts & 1)
                    {
                        char fileid[100];
                        sprintf(fileid,"(%d,%d,%d)",
                                (nam.nam$b_fid_nmx << 16) | nam.nam$w_fid_num,
                                nam.nam$w_fid_seq,nam.nam$b_fid_rvn);
                        printf("  %-22s",fileid);
                    }
                    if (size_sts & 1)
                    {
                        unsigned filesize = fhc.xab$l_ebk;
                        if (fhc.xab$w_ffb == 0) filesize--;
                        printf("%9d",filesize);
                        dirblocks += filesize;
                    }
                    if (date_sts & 1)
                    {
                        char tim[24];
                        struct dsc$descriptor timdsc;
                        timdsc.dsc$w_length = 23;
                        timdsc.dsc$a_pointer = tim;
                        sts = sys$asctim(0,&timdsc,&dat.xab$q_cdt,0);
                        if ((sts & 1) == 0) printf("Asctim error: %d\n",sts);
                        tim[23] = '\0';
                        printf("  %s",tim);
                    }
                    if (acl_sts & 1)
                    {
                        int sts;
                        $DESCRIPTOR(file, "FILE");
                        struct dsc$descriptor filename;
                        filename.dsc$a_pointer = rsa;
                        filename.dsc$w_length = strlen(rsa);
                        struct _ile3 itmlst[2];
                        memset (itmlst, 0, 2 * sizeof(struct _ile3));
                        int retlen = 0;
                        char buf[512];
                        itmlst[0].ile3$w_length = 512;
                        itmlst[0].ile3$w_code = OSS$_ACL_READ;
                        itmlst[0].ile3$ps_bufaddr = buf;
                        itmlst[0].ile3$ps_retlen_addr = &retlen;
                        sts = sys$get_security(&file, &filename, 0, 0, &itmlst, 0, 0);
                        // check. todo: ods2 check
                        unsigned short *mp;
                        long acep;
                        struct _acedef * ace;
                        mp = buf + 2*buf[2];
                        if (buf[2]==-1 || buf[2]==255)
                            goto myout;
                        acep = mp;
                        ace = acep;

                        while (ace->ace$b_size)
                        {
                            switch (ace->ace$b_type)
                            {
                            case ACE$C_AUDIT:
                                break;
                            case ACE$C_ALARM:
                                break;
                            case ACE$C_DIRDEF:
                                break;
                            case ACE$C_INFO:
                                break;
                            case ACE$C_KEYID:
                            {
                                char * access_types[] =
                                {
                                    "",
                                    "READ",
                                    "WRITE",
                                    "EXECUTE",
                                    "DELETE",
                                    "CONTROL",
                                    ""
                                };

                                int access_types_values[] = { 0, ACE$M_READ, ACE$M_WRITE, ACE$M_EXECUTE, ACE$M_DELETE, ACE$M_CONTROL };

                                printf("          IDENTIFIER=%x,ACCESS=", ace->ace$l_key);
                                int i;
                                int onewritten = 0;
                                for (i = 0; i <= 5; i++)
                                {
                                    if (access_types_values[i] & ace->ace$l_access)
                                    {
                                        if (onewritten)
                                            printf("+");
                                        printf(access_types[i]);
                                        onewritten = 1;
                                    }
                                }
                                printf(")\n");
                            }
                            break;

                            case ACE$C_RMSJNL_AI:
                            case ACE$C_RMSJNL_AT:
                            case ACE$C_RMSJNL_BI:
                            case ACE$C_RMSJNL_RU:
                            case ACE$C_RMSJNL_RU_DEFAULT:
                                break;
                            default:
                                printf("ace does not exist\n");
                                break;
                            }
                            acep += ace->ace$b_size;
                            ace = acep;
                        }
myout:
                        {}
                    }
                    printf("\n");
                }
            }
            dirfiles++;
        }
        if (sts == RMS$_NMF) sts = 1;
        if (printcol > 0) printf("\n");
        if (dirfiles > 0)
        {
            printf("\nTotal of %d file%s",dirfiles,(dirfiles == 1 ? "" : "s"));
            if (size_sts & 1)
            {
                printf(", %d block%s.\n",dirblocks,(dirblocks == 1 ? "" : "s"));
            }
            else
            {
                fputs(".\n",stdout);
            }
            filecount += dirfiles;
            totblocks += dirblocks;
            if (dircount > 1)
            {
                printf("\nGrand total of %d director%s, %d file%s",
                       dircount,(dircount == 1 ? "y" : "ies"),
                       filecount,(filecount == 1 ? "" : "s"));
                if (size_sts & 1)
                {
                    printf(", %d block%s.\n",totblocks,(totblocks == 1 ? "" : "s"));
                }
                else
                {
                    fputs(".\n",stdout);
                }
            }
        }
    }
    if (sts & 1)
    {
        if (filecount < 1) printf("%%DIRECT-W-NOFILES, no files found\n");
    }
    else
    {
        printf("%%DIR-E-ERROR Status: %d\n",sts);
    }
    return sts;
}
