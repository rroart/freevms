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

/* copy: a file copy routine */

#define MAXREC 32767

unsigned copy(int userarg)
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

    struct _namdef inam = cc$rms_nam;
    struct _fabdef ifab = cc$rms_fab;
    struct _fabdef ofab = cc$rms_fab;
    char res[NAM$C_MAXRSS + 1],rsa[NAM$C_MAXRSS + 1];
    int filecount = 0;
    inam.nam$l_esa = res;
    inam.nam$b_ess = NAM$C_MAXRSS;
    ifab.fab$l_nam = &inam;
    ifab.fab$l_fna = c1;
    ifab.fab$b_fns = strlen(ifab.fab$l_fna);
    ofab.fab$l_fna = c2;
    ofab.fab$b_fns = strlen(ofab.fab$l_fna);

    sts = sys$parse(&ifab);
    if (sts & 1) {
        inam.nam$l_rsa = rsa;
        inam.nam$b_rss = NAM$C_MAXRSS;
        ifab.fab$l_fop = FAB$M_NAM;
        while ((sts = sys$search(&ifab)) & 1) {
            sts = sys$open(&ifab);
            if ((sts & 1) == 0) {
                printf("%%COPY-F-OPENFAIL, Open error: %d\n",sts);
                perror("-COPY-F-ERR ");
            } else {
                struct _rabdef irab = cc$rms_rab;
                irab.rab$l_fab = &ifab;
                if ((sts = sys$connect(&irab)) & 1) {
		  char name[NAM$C_MAXRSS + 1];
		  unsigned records = 0;

		  ofab.fab$b_org=ifab.fab$b_org;
		  ofab.fab$b_rat=ifab.fab$b_rat;
		  ofab.fab$b_rfm=ifab.fab$b_rfm;
		  ofab.fab$w_mrs=ifab.fab$w_mrs;
		  ofab.fab$l_alq=ifab.fab$l_alq;

		  memcpy(name,c2,strlen(c2));

		  if ((sts = sys$create(&ofab)) & 1) {
		    struct _rabdef orab = cc$rms_rab;
		    orab.rab$l_fab = &ofab;
		    if ((sts = sys$connect(&orab)) & 1) {
		      char rec[MAXREC + 2];
		      orab.rab$l_rbf = rec;
		      orab.rab$w_usz = MAXREC;

		      if ((sts & 1) == 0) {
			printf("%%COPY-F-OPENOUT, Could not open %s\n",name);
			perror("-COPY-F-ERR ");
		      } else {
                        filecount++;
                        irab.rab$l_ubf = rec;
                        irab.rab$w_usz = MAXREC;
                        while ((sts = sys$get(&irab)) & 1) {
			  orab.rab$l_rbf = irab.rab$l_rbf;
			  orab.rab$w_rsz = irab.rab$w_rsz;
			  sts = sys$put(&orab);
			  if ((sts & 1) == 0) {
			    printf("%%COPY-F- fwrite error!!\n");
			    perror("-COPY-F-ERR ");
			    break;
			  }
			  records++;
                        }
		      }
		      rsa[inam.nam$b_rsl] = '\0';
		      if (sts == RMS$_EOF) {
                        printf("%%COPY-S-COPIED, %s copied to %s (%d record%s)\n",
                               rsa,name,records,(records == 1 ? "" : "s"));
		      } else {
                        printf("%%COPY-F-ERROR Status: %d for %s\n",sts,rsa);
                        sts = 1;
		      }
		      sys$disconnect(&orab);
		    }
		    sys$close(&ofab);
		  }
		  sys$disconnect(&irab);
		}
		sys$close(&ifab);
	    }
	}
	if (sts == RMS$_NMF) sts = 1;
    }
    if (sts & 1) {
      if (filecount > 0) printf("%%COPY-S-NEWFILES, %d file%s created\n",
				filecount,(filecount == 1 ? "" : "s"));
    } else {
      printf("%%COPY-F-ERROR Status: %d\n",sts);
    }
    return sts;
}

/* export: a file copy routine */

unsigned export(int userarg)
{
    setvbuf(stdout, NULL, _IONBF, 0);      // need this to see i/o at all
    int sts;
    $DESCRIPTOR(p1, "p1");
    $DESCRIPTOR(p2, "p2");
    $DESCRIPTOR(q1, "binary");
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
    int binary_sts = cli$present(&q1);
    int retlen;
    sts = cli$get_value(&p1, &o1, &retlen);
    sts = cli$get_value(&p2, &o2, &retlen);

    struct _namdef nam = cc$rms_nam;
    struct _fabdef fab = cc$rms_fab;
    char res[NAM$C_MAXRSS + 1],rsa[NAM$C_MAXRSS + 1];
    int filecount = 0;
    nam.nam$l_esa = res;
    nam.nam$b_ess = NAM$C_MAXRSS;
    fab.fab$l_nam = &nam;
    fab.fab$l_fna = c1;
    fab.fab$b_fns = strlen(fab.fab$l_fna);
    sts = sys$parse(&fab);
    if (sts & 1) {
        nam.nam$l_rsa = rsa;
        nam.nam$b_rss = NAM$C_MAXRSS;
        fab.fab$l_fop = FAB$M_NAM;
        while ((sts = sys$search(&fab)) & 1) {
            sts = sys$open(&fab);
            if ((sts & 1) == 0) {
                printf("%%EXPORT-F-OPENFAIL, Open error: %d\n",sts);
                perror("-EXPORT-F-ERR ");
            } else {
                struct _rabdef rab = cc$rms_rab;
                rab.rab$l_fab = &fab;
                if ((sts = sys$connect(&rab)) & 1) {
                    FILE *tof;
                    char name[NAM$C_MAXRSS + 1];
                    unsigned records = 0;
                    {
                        char *out = name,*inp = c2;
                        int dot = 0;
                        while (*inp != '\0') {
                            if (*inp == '*') {
                                inp++;
                                if (dot) {
                                    memcpy(out,nam.nam$l_type + 1,nam.nam$b_type - 1);
                                    out += nam.nam$b_type - 1;
                                } else {
                                    unsigned length = nam.nam$b_name;
                                    if (*inp == '\0') length += nam.nam$b_type;
                                    memcpy(out,nam.nam$l_name,length);
                                    out += length;
                                }
                            } else {
                                if (*inp == '.') {
                                    dot = 1;
                                } else {
                                    if (strchr(":]\\/",*inp)) dot = 0;
                                }
                                *out++ = *inp++;
                            }
                        }
                        *out++ = '\0';
                    }
#ifndef _WIN32
                    tof = fopen(name,"w");
#else
		    if ((binary_sts & 1) == 0 && fab.fab$b_rat & PRINT_ATTR) {
                        tof = fopen(name,"w");
		    } else {
		        tof = fopen(name,"wb");
		    }
#endif
                    if (tof == NULL) {
                        printf("%%EXPORT-F-OPENOUT, Could not open %s\n",name);
                        perror("-EXPORT-F-ERR ");
                    } else {
                        char rec[MAXREC + 2];
                        filecount++;
                        rab.rab$l_ubf = rec;
                        rab.rab$w_usz = MAXREC;
                        while ((sts = sys$get(&rab)) & 1) {
                            unsigned rsz = rab.rab$w_rsz;
                            if ((binary_sts & 1) == 0 &&
                                 fab.fab$b_rat & PRINT_ATTR) rec[rsz++] = '\n';
                            if (fwrite(rec,rsz,1,tof) == 1) {
                                records++;
                            } else {
                                printf("%%EXPORT-F- fwrite error!!\n");
                                perror("-EXPORT-F-ERR ");
                                break;
                            }
                        }
                        if (fclose(tof)) {
                            printf("%%EXPORT-F- fclose error!!\n");
                            perror("-EXPORT-F-ERR ");
                        }
                    }
                    sys$disconnect(&rab);
                    rsa[nam.nam$b_rsl] = '\0';
                    if (sts == RMS$_EOF) {
                        printf("%%EXPORT-S-COPIED, %s copied to %s (%d record%s)\n",
                               rsa,name,records,(records == 1 ? "" : "s"));
                    } else {
                        printf("%%EXPORT-F-ERROR Status: %d for %s\n",sts,rsa);
                        sts = 1;
                    }
                }
                sys$close(&fab);
            }
        }
        if (sts == RMS$_NMF) sts = 1;
    }
    if (sts & 1) {
        if (filecount > 0) printf("%%EXPORT-S-NEWFILES, %d file%s created\n",
                                  filecount,(filecount == 1 ? "" : "s"));
    } else {
        printf("%%EXPORT-F-ERROR Status: %d\n",sts);
    }
    return sts;
}

/* import: a file copy routine */

unsigned import(int userarg)
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

    FILE *fromf;
    fromf = fopen(c1,"r");
    if (fromf != NULL) {
        struct _fabdef fab = cc$rms_fab;
        fab.fab$l_fna = c2;
        fab.fab$b_fns = strlen(fab.fab$l_fna);
        if ((sts = sys$create(&fab)) & 1) {
            struct _rabdef rab = cc$rms_rab;
            rab.rab$l_fab = &fab;
            if ((sts = sys$connect(&rab)) & 1) {
                char rec[MAXREC + 2];
                rab.rab$l_rbf = rec;
                rab.rab$w_usz = MAXREC;
                while (fgets(rec,sizeof(rec),fromf) != NULL) {
                    rab.rab$w_rsz = strlen(rec);
                    sts = sys$put(&rab);
                    if ((sts & 1) == 0) break;
                }
                sys$disconnect(&rab);
            }
            sys$close(&fab);
        }
        fclose(fromf);
        if (!(sts & 1)) {
            printf("%%IMPORT-F-ERROR Status: %d\n",sts);
        }
    } else {
        printf("Can't open %s\n",c1);
    }
    return sts;
}
