// $Id$
// $Locker$

// Author. Paul Nankervis.
// Author. Roar Thronæs.

/* check for cr - return terminator - update file length */
/* RMS.c v1.3  RMS components */

/*
        This is part of ODS2 written by Paul Nankervis,
        email address:  Paulnank@au1.ibm.com

        ODS2 is distributed freely for all members of the
        VMS community to use. However all derived works
        must maintain comments in their source to acknowledge
        the contibution of the original author.
*/

/*  Boy some cleanups are needed here - especially for
    error handling and deallocation of memory after errors..
    For now we have to settle for loosing memory left/right/center...

    This module implements file name parsing, file searches,
    file opens, gets, etc...       */


#define DEBUGx x

#include<linux/config.h>
#include<linux/linkage.h>
#include<linux/mm.h>

//#include <stdio.h>
#include <linux/ctype.h>

#define NO_DOLLAR
#define RMS$INITIALIZE          /* used by rms.h to create templates */
#include <mytypes.h>
#include <aqbdef.h>
#include <atrdef.h>
#include <fatdef.h>
#include <iodef.h>
#include <iosbdef.h>
#include <vcbdef.h>
#include <wcbdef.h>
#include <descrip.h>
#include <devdef.h>
#include <ssdef.h>
#include <uicdef.h>
#include <namdef.h>
#include <fabdef.h>
#include <rabdef.h>
#include <fibdef.h>
#include <fiddef.h>
#include <rmsdef.h>
#include <sbkdef.h>
#include <ucbdef.h>
#include <xabdef.h>
#include <xabdatdef.h>
#include <xabfhcdef.h>
#include <xabprodef.h>
#include <fh2def.h>
#include <fcbdef.h>
#include <vmstime.h>
#include <xabkeydef.h>
#include <xaballdef.h>
#include <keydef.h>
#include <bktdef.h>
#include <areadef.h>
#include <ifbdef.h>
#include <irbdef.h>

//#include <rms.h>
#include "cache.h"
#include "access.h"
#include "direct.h"

#include "rmsmisc.h"
#include <starlet.h>
#include <exe_routines.h>
#include <misc_routines.h>

#include <linux/slab.h>

#if 0
struct _namdef cc$rms_nam = {0,0,0,0,0,0,0,0,0,0,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,0,0};

struct _fabdef cc$rms_fab = {NULL,0,NULL,NULL,0,0,0,0,0,0,0,0,0,0,0,0,0,NULL};
#endif

// borrowed from starlet/src/rms.c

struct _fabdef cc$rms_fab={FAB$C_BID,FAB$C_BLN,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
struct _rabdef cc$rms_rab={RAB$C_BID,RAB$C_BLN,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
struct _namdef cc$rms_nam={NAM$C_BID,NAM$C_BLN,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/* Table of file name component delimeters... */

char char_delim[] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0, 0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0};

struct WCCFILE *ifi_table[] = {
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
};

struct _fabdet * ifi_alloc[] = {
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
};

struct _ifbdef *ifb_table[] = {
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
};

struct _irbdef *irb_table[] = {
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
};

/* Routine to find size of file name components */

unsigned name_delim(char *str,int len,int size[5])
{
    unsigned ch = 0;
    char *curptr = str;
    char *endptr = curptr + len;
    char *begptr = curptr;
    while (curptr < endptr) {
        ch = (*curptr++ & 127);
        if (char_delim[ch]) break;
    }
    if (curptr > begptr && ch == ':') {
        size[0] = (curptr - begptr);
        begptr = curptr;
        while (curptr < endptr) {
            ch = (*curptr++ & 127);
            if (char_delim[ch]) break;
        }
    } else {
        size[0] = 0;
    }
    if (ch == '[' || ch == '<') {
        if (curptr != begptr + 1) return RMS$_FNM;
        while (curptr < endptr) {
            ch = (*curptr++ & 127);
                if (char_delim[ch]) if (ch != '.') break;
        }
        if (curptr < begptr + 2 || (ch != ']' && ch != '>')) return RMS$_FNM;
        size[1] = (curptr - begptr);
        begptr = curptr;
        while (curptr < endptr) {
            ch = (*curptr++ & 127);
            if (char_delim[ch]) break;
        }
    } else {
        size[1] = 0;
    }
    if (curptr > begptr && char_delim[ch]) {
        size[2] = (curptr - begptr) - 1;
        begptr = curptr - 1;
    } else {
        size[2] = (curptr - begptr);
        begptr = curptr;
    }
    if (curptr > begptr && ch == '.') {
        while (curptr < endptr) {
            ch = (*curptr++ & 127);
            if (char_delim[ch]) break;
        }
        if (curptr > begptr && char_delim[ch]) {
            size[3] = (curptr - begptr) - 1;
            begptr = curptr - 1;
        } else {
            size[3] = (curptr - begptr);
            begptr = curptr;
        }
    } else {
        size[3] = 0;
    }
    if (curptr > begptr && (ch == ';' || ch == '.')) {
        while (curptr < endptr) {
            ch = (*curptr++ & 127);
            if (char_delim[ch]) break;
        }
        size[4] = (curptr - begptr);
    } else {
        size[4] = 0;
    }
#ifdef DEBUG
    printk("Name delim %d %d %d %d %d\n",size[0],size[1],size[2],size[3],size[4]);
#endif
    if (curptr >= endptr) {
        return 1;
    } else {
        return RMS$_FNM;
    }
}

/* Routine to find directory name in cache... NOT CURRENTLY IN USE!!! */

unsigned dircache(struct _vcb *vcb,char *dirnam,int dirlen,struct _fiddef *dirid)
{
    struct DIRCACHE *dir;
    if (dirlen < 1) {
        dirid->fid$w_num = 4;
        dirid->fid$w_seq = 4;
        dirid->fid$b_rvn = 0;
        dirid->fid$b_nmx = 0;
        return 1;
    } else {
        unsigned sts;
	dir = NULL;
	//        dir = cache_find((void *) &vcb->dircache,dirlen,dirnam,&sts,dircmp,NULL);
#if 0
        if (dir != NULL) {
            memcpy(dirid,&dir->dirid,sizeof(struct _fiddef));
            return 1;
        }
#endif
        return 0;
    }
}


/* Function to remove WCCFILE and WCCDIR structures when not required */

void cleanup_wcf(struct WCCFILE *wccfile)
{
    if (wccfile != NULL) {
        struct WCCDIR *wcc = wccfile->wcf_wcd.wcd_next;
        wccfile->wcf_wcd.wcd_next = NULL;
        wccfile->wcf_wcd.wcd_prev = NULL;
        /* should deaccess volume */
        int sts = exe_std$deanonpgdsiz(wccfile, sizeof(struct WCCFILE) + 256);
        while (wcc != NULL) {
            struct WCCDIR *next = wcc->wcd_next;
            wcc->wcd_next = NULL;
            wcc->wcd_prev = NULL;
            int sts = exe_std$deanonpgdsiz(wcc, wcc->wcd_size);
            wcc = next;
        }
    }
}


/* Function to perform an RMS search... */

unsigned do_search(struct _fabdef *fab,struct WCCFILE *wccfile)
{
    struct _iosb iosb={0};
    int sts;
    unsigned short dummy;
    struct _fibdef fibblk;
    memset(&fibblk, 0, sizeof(struct _fibdef));
    struct WCCDIR *wcc;
    struct dsc$descriptor fibdsc,resdsc;
    struct _namdef *nam = fab->fab$l_nam;
    wcc = &wccfile->wcf_wcd;
    int ifi_no = fab->fab$w_ifi;
    if (ifb_table[ifi_no]->ifb$l_devchar&DEV$M_TRM)
      return SS$_NORMAL;

    /* if first time through position at top directory... WCCDIR */

    while ((wcc->wcd_status & STATUS_INIT) == 0 && wcc->wcd_next != NULL) {
        wcc = wcc->wcd_next;
    }
    fibdsc.dsc$w_length = sizeof(struct _fibdef);
    fibdsc.dsc$a_pointer = (char *) &fibblk;
    while (1) {
        if ((wcc->wcd_status & STATUS_INIT) == 0 || wcc->wcd_wcc != 0) {
            wcc->wcd_status |= STATUS_INIT;
            resdsc.dsc$w_length = 256 - wcc->wcd_prelen;
            resdsc.dsc$a_pointer = wccfile->wcf_result + wcc->wcd_prelen;
            memcpy(&fibblk.fib$w_did_num,&wcc->wcd_dirid,sizeof(struct _fiddef));
            fibblk.fib$w_nmctl = 0;     /* FIB_M_WILD; */
            fibblk.fib$l_acctl = 0;
            fibblk.fib$w_fid_num = 0;
            fibblk.fib$w_fid_seq = 0;
            fibblk.fib$b_fid_rvn = 0;
            fibblk.fib$b_fid_nmx = 0;
            fibblk.fib$l_wcc = wcc->wcd_wcc;
#ifdef DEBUG
            wcc->wcd_sernam[wcc->wcd_serdsc.dsc$w_length] = '\0';
            wccfile->wcf_result[wcc->wcd_prelen + wcc->wcd_reslen] = '\0';
            printk("Ser: '%s' (%d,%d,%d) WCC: %d Prelen: %d '%s'\n",wcc->wcd_sernam,
                   fibblk.fib$w_did_num | (fibblk.fib$b_did_nmx << 16),
                   fibblk.fib$w_did_seq,fibblk.fib$b_did_rvn,
                   wcc->wcd_wcc,wcc->wcd_prelen,wccfile->wcf_result + wcc->wcd_prelen);
#endif
            fibblk.fib$w_fid_num = fibblk.fib$w_did_num;
            fibblk.fib$w_fid_seq = fibblk.fib$w_did_seq;
            fibblk.fib$w_did_num = 0;
            fibblk.fib$w_did_seq = 0;
	    sts = sys$qiow(RMS_EF,ifb_table[ifi_no /*fab->fab$w_ifi*/]->ifb$w_chnl,IO$_ACCESS|IO$M_ACCESS,&iosb,0,0,
			   &fibdsc,&wcc->wcd_serdsc,&wcc->wcd_reslen,&resdsc,0,0);
	    sts = iosb.iosb$w_status;

            fibblk.fib$w_did_num = fibblk.fib$w_fid_num;
            fibblk.fib$w_did_seq = fibblk.fib$w_fid_seq;
            fibblk.fib$w_fid_num = 0;
            fibblk.fib$w_fid_seq = 0;
	    sts = sys$qiow(RMS_EF,ifb_table[ifi_no /*fab->fab$w_ifi*/]->ifb$w_chnl,IO$_ACCESS,&iosb,0,0,
			   &fibdsc,&wcc->wcd_serdsc,&wcc->wcd_reslen,&resdsc,0,0);
	    sts = iosb.iosb$w_status;
        } else {
            sts = SS$_NOMOREFILES;
        }
        if (sts & 1) {
#ifdef DEBUG
            wccfile->wcf_result[wcc->wcd_prelen + wcc->wcd_reslen] = '\0';
            printk("Fnd: '%s'  (%d,%d,%d) WCC: %d\n",wccfile->wcf_result + wcc->wcd_prelen,
                   fibblk.fib$w_fid_num | (fibblk.fib$b_fid_nmx << 16),
                   fibblk.fib$w_fid_seq,fibblk.fib$b_fid_rvn,
                   wcc->wcd_wcc);
#endif
            wcc->wcd_wcc = fibblk.fib$l_wcc;
            if (wcc->wcd_prev) {/* go down directory */
                if (wcc->wcd_prev->wcd_next != wcc) printk("wcd_PREV corruption\n");
                if (fibblk.fib$w_fid_num != 4 || fibblk.fib$b_fid_nmx != 0 ||
                    wcc == &wccfile->wcf_wcd ||
                    memcmp(wcc->wcd_sernam,"000000.",7) == 0) {
                    memcpy(&wcc->wcd_prev->wcd_dirid,&fibblk.fib$w_fid_num,sizeof(struct _fiddef));
                    if (wcc->wcd_next) wccfile->wcf_result[wcc->wcd_prelen - 1] = '.';
                    wcc->wcd_prev->wcd_prelen = wcc->wcd_prelen + wcc->wcd_reslen - 5;
                    wcc = wcc->wcd_prev;        /* go down one level */
                    if (wcc->wcd_prev == NULL) wccfile->wcf_result[wcc->wcd_prelen - 1] = ']';
                }
            } else {
                if (nam != NULL) {
                    int fna_size[5];
                    memcpy(&nam->nam$w_fid_num,&fibblk.fib$w_fid_num,sizeof(struct _fiddef));
                    nam->nam$b_rsl = wcc->wcd_prelen + wcc->wcd_reslen;
                    name_delim(wccfile->wcf_result,nam->nam$b_rsl,fna_size);
                    nam->nam$l_dev = nam->nam$l_rsa;
                    nam->nam$b_dev = fna_size[0];
                    nam->nam$l_dir = nam->nam$l_dev + fna_size[0];
                    nam->nam$b_dir = fna_size[1];
                    nam->nam$l_name = nam->nam$l_dir + fna_size[1];
                    nam->nam$b_name = fna_size[2];
                    nam->nam$l_type = nam->nam$l_name + fna_size[2];
                    nam->nam$b_type = fna_size[3];
                    nam->nam$l_ver = nam->nam$l_type + fna_size[3];
                    nam->nam$b_ver = fna_size[4];
                    if (nam->nam$b_rsl <= nam->nam$b_rss) {
                        memcpy(nam->nam$l_rsa,wccfile->wcf_result,nam->nam$b_rsl);
                    } else {
                        return RMS$_RSS;
                    }
                }
                memcpy(&wccfile->wcf_fib.fib$w_fid_num,&fibblk.fib$w_fid_num,sizeof(struct _fiddef));

                return 1;
            }
        } else {
#ifdef DEBUG
            printk("Err: %d\n",sts);
#endif
            if (sts == SS$_BADIRECTORY) {
                if (wcc->wcd_next != NULL) {
                    if (wcc->wcd_next->wcd_status & STATUS_INIT) sts = SS$_NOMOREFILES;
                }
            }
            if (sts == SS$_NOMOREFILES) {
                wcc->wcd_status &= ~STATUS_INIT;
                wcc->wcd_wcc = 0;
                wcc->wcd_reslen = 0;
                if (wcc->wcd_status & STATUS_TMPDIR) {
                    struct WCCDIR *savwcc = wcc;
                    if (wcc->wcd_next != NULL) wcc->wcd_next->wcd_prev = wcc->wcd_prev;
                    if (wcc->wcd_prev != NULL) wcc->wcd_prev->wcd_next = wcc->wcd_next;
                    wcc = wcc->wcd_next;
                    memcpy(wccfile->wcf_result + wcc->wcd_prelen + wcc->wcd_reslen - 6,".DIR;1",6);
                    int sts = exe_std$deanonpgdsiz(savwcc, sizeof(struct WCCFILE) + 256);
                } else {
                    if ((wccfile->wcf_status & STATUS_RECURSE) && wcc->wcd_prev == NULL) {
                        struct WCCDIR *newwcc;
			int alosize;
			int sts = exe_std$alononpaged(sizeof(struct WCCDIR) + 8, &alosize, &newwcc);
			memset(newwcc,0,sizeof(struct WCCDIR) + 8);
                        newwcc->wcd_next = wcc->wcd_next;
                        newwcc->wcd_prev = wcc;
                        newwcc->wcd_wcc = 0;
                        newwcc->wcd_status = STATUS_TMPDIR;
                        newwcc->wcd_reslen = 0;
                        if (wcc->wcd_next != NULL) {
                            wcc->wcd_next->wcd_prev = newwcc;
                        }
                        wcc->wcd_next = newwcc;
                        memcpy(&newwcc->wcd_dirid,&wcc->wcd_dirid,sizeof(struct _fiddef));
                        newwcc->wcd_serdsc.dsc$w_length = 7;
                        newwcc->wcd_serdsc.dsc$a_pointer = newwcc->wcd_sernam;
                        memcpy(newwcc->wcd_sernam,"*.DIR;1",7);
                        newwcc->wcd_prelen = wcc->wcd_prelen;
                        wcc = newwcc;

                    } else {
                        if (wcc->wcd_next != NULL) {
#ifdef DEBUG
                            if (wcc->wcd_next->wcd_prev != wcc) printk("wcd_NEXT corruption\n");
#endif
                            wcc = wcc->wcd_next;        /* backup one level */
                            memcpy(wccfile->wcf_result + wcc->wcd_prelen + wcc->wcd_reslen - 6,".DIR;1",6);
                        } else {
                            sts = RMS$_NMF;
                            break;      /* giveup */
                        }
                    }
                }
            } else {
                if (sts == SS$_NOSUCHFILE) {
                    if (wcc->wcd_prev) {
                        sts = RMS$_DNF;
                    } else {
                        sts = RMS$_FNF;
                    }
                }
                break;          /* error - abort! */
            }
        }
    }
    // cleanup_wcf(wccfile); no, I know this is still in use
    if (sts == RMS$_NMF)
      if (nam != NULL) nam->nam$l_wcc = 0;
#if 0
    // check whether this should be done
    fab->fab$w_ifi = 0;         /* must dealloc memory blocks! */
#endif
    return sts;
}


/* External entry for search function... */

unsigned exe$search(struct _fabdef *fab)
{
    int sts;
    struct _namdef *nam = fab->fab$l_nam;
    struct WCCFILE *wccfile;
    if (nam == NULL) return RMS$_NAM;
    wccfile = (struct WCCFILE *) nam->nam$l_wcc;
    if (wccfile == NULL) return RMS$_WCC;
    if (fab->fab$w_ifi != 0) return RMS$_IFI;
    fab->fab$w_ifi = nam->namdef$$_fill_7[0]; // borrow this instead of nam$l_wcc;
    sts = do_search(fab,wccfile);
    fab->fab$w_ifi = 0;
    return sts;
}



#define DEFAULT_SIZE 120
char default_buffer[DEFAULT_SIZE];
char *default_name = "DKA200:[000000].;";
int default_size[] = {7,8,0,1,1};

int alloc_ifab(struct _fabdef * fab) {
  int sts = 1;
  int ifi_no = 1;
  while (ifi_alloc[ifi_no] != NULL && ifi_no < IFI_MAX) ifi_no++;
  if (ifi_no >= IFI_MAX) return RMS$_IFI;

  ifi_alloc[ifi_no]=fab;
  if (ifb_table[ifi_no]==0) {
    int alosize;
    char * buffer;
    struct _ifbdef * ifb;
    sts=exe_std$alononpaged(sizeof(struct _ifbdef),&alosize,&ifb);
    memset(ifb, 0, sizeof(struct _ifbdef));
    ifb_table[ifi_no]=ifb;
  }

  fab->fab$w_ifi = ifi_no;

  return SS$_NORMAL;
}

int set_ifab(struct _fabdef * fab, int size, char * name) {
  int sts;
  int ifi_no = fab->fab$w_ifi;
  struct _ucb *dev;
  short int chan;

  sts = device_lookup(size,name,0,&chan);
  if ((sts & 1) == 0) return sts;
  dev = ctl$ga_ccb_table[chan].ccb$l_ucb;
  struct _ifbdef * ifb;
  ifb = ifb_table[ifi_no];
  ifb->ifb$l_devchar = dev->ucb$l_devchar;
  ifb->ifb$l_devchar2 = dev->ucb$l_devchar2;
  ifb->ifb$w_chnl = chan;

  return SS$_NORMAL;
}

/* Function to perform RMS parse.... */

unsigned do_parse(struct _fabdef *fab,struct WCCFILE **wccret)
{
    int sts;
    struct WCCFILE *wccfile = 0;
    char *fna;
    int fns;
    sts = search_log_repl(fab->fab$l_fna,&fna,&fns);
    if ((sts&1)==0) {
      fna=fab->fab$l_fna;
      fns=fab->fab$b_fns;
    }
    char *dna = fab->fab$l_dna;
    struct _namdef *nam = fab->fab$l_nam;
    int fna_size[5] = {0, 0, 0, 0, 0},dna_size[5] = {0, 0, 0, 0, 0};
    if (nam != NULL) if (nam->nam$l_wcc == 0) {
      cleanup_wcf((struct WCCFILE *) nam->nam$l_wcc);
      nam->nam$l_wcc = 0;
    }
    /* Break up file specifications... */

    sts = name_delim(fna,fns/*fab->fab$b_fns*/,fna_size);
    if ((sts & 1) == 0) return sts;
    if (dna) {
        sts = name_delim(dna,fab->fab$b_dns,dna_size);
        if ((sts & 1) == 0) return sts;
    }
    /* Make WCCFILE entry for rest of processing */

    if (nam != NULL) wccfile = nam->nam$l_wcc;

    if (wccfile == 0) {
        int alosize;
	int sts = exe_std$alononpaged(sizeof(struct WCCFILE) + 256, &alosize, &wccfile);
	memset(wccfile,0,sizeof(struct WCCFILE) + 256);
        if (wccfile == NULL) return SS$_INSFMEM;
memset(wccfile,0,sizeof(struct WCCFILE)+256);
        wccfile->wcf_fab = fab;
        wccfile->wcf_status = 0;
        wccfile->wcf_wcd.wcd_status = 0;
    }

    /* Combine file specifications */

    {
        int field,ess = MAX_FILELEN;
        char *esa,*def = default_name;
        esa = wccfile->wcf_result;
        for (field = 0; field < 5; field++) {
            char *src;
            int len = fna_size[field];
            if (len > 0) {
                src = fna;
            } else {
                len = dna_size[field];
                if (len > 0) {
                    src = dna;
                } else {
                    len = default_size[field];
                    src = def;
                }
            }
            fna += fna_size[field];
            if (field == 1) {
                int dirlen = len;
                if (len < 3) {
                    dirlen = len = default_size[field];
                    src = def;
                } else {
                    char ch1 = *(src + 1);
                    char ch2 = *(src + 2);
                    if (ch1 == '.' || (ch1 == '-' &&
                                       (ch2 == '-' || ch2 == '.' || ch2 == ']'))) {
                        char *dir = def;
                        int count = default_size[1] - 1;
                        len--;
                        src++;
                        while (len >= 2 && *src == '-') {
                            len--;
                            src++;
                            if (count < 2 || (count == 7 &&
                                              memcmp(dir,"[000000",7) == 0)) return RMS$_DIR;
                            while (count > 1) {
                                if (dir[--count] == '.') break;
                            }
                        }
                        if (count < 2 && len < 2) {
                            src = "[000000]";
                            dirlen = len = 8;
                        } else {
                            if (*src != '.' && *src != ']') return RMS$_DIR;
                            if (*src == '.' && count < 2) {
                                src++;
                                len--;
                            }
                            dirlen = len + count;
                            if ((ess -= count) < 0) return RMS$_ESS;
                            memcpy(esa,dir,count);
                            esa += count;
                        }
                    }
                }
                fna_size[field] = dirlen;
            } else {
                fna_size[field] = len;
            }
            dna += dna_size[field];
            def += default_size[field];
            if ((ess -= len) < 0) return RMS$_ESS;
            while (len-- > 0) {
                char ch;
                *esa++ = ch = *src++;
                if (ch == '*' || ch == '%')
                    wccfile->wcf_status |= STATUS_WILDCARD;
            }
        }
        /* Pass back results... */
        if (nam) {
            nam->nam$l_dev = nam->nam$l_esa;
            nam->nam$b_dev = fna_size[0];
            nam->nam$l_dir = nam->nam$l_dev + fna_size[0];
            nam->nam$b_dir = fna_size[1];
            nam->nam$l_name = nam->nam$l_dir + fna_size[1];
            nam->nam$b_name = fna_size[2];
            nam->nam$l_type = nam->nam$l_name + fna_size[2];
            nam->nam$b_type = fna_size[3];
            nam->nam$l_ver = nam->nam$l_type + fna_size[3];
            nam->nam$b_ver = fna_size[4];
            nam->nam$b_esl = esa - wccfile->wcf_result;
            nam->nam$l_fnb = 0;
            if (wccfile->wcf_status & STATUS_WILDCARD) nam->nam$l_fnb = NAM$M_WILDCARD;
            if (nam->nam$b_esl <= nam->nam$b_ess) {
                memcpy(nam->nam$l_esa,wccfile->wcf_result,nam->nam$b_esl);
            } else {
                return RMS$_ESS;
            }
        }
    }
    sts = 1;
        if (nam != NULL) if (nam->nam$b_nop & NAM$M_SYNCHK) sts = 0;

    /* Now build up WCC structures as required */

    if (sts) {
	struct WCCDIR *wcc;
	int dirlen,dirsiz;
	char *dirnam;
	
	sts = set_ifab(fab, fna_size[0], wccfile->wcf_result);
	if ((sts & 1) == 0) return sts;

	int ifi_no = fab->fab$w_ifi;
	struct _ifbdef * ifb;
	ifb = ifb_table[ifi_no];
	short int chan;
	chan = ifb->ifb$w_chnl;
	struct _ucb *dev;
	dev = ctl$ga_ccb_table[chan].ccb$l_ucb;
	int disk_or_tape = (dev->ucb$l_devchar & DEV$M_TRM)==0;
	if (!disk_or_tape) {
	  ifi_table[ifi_no] = wccfile;
	  return SS$_DEVNOTMOUNT;
	}
        if (dev->ucb$l_vcb == NULL && disk_or_tape) return SS$_DEVNOTMOUNT;
        wcc = &wccfile->wcf_wcd;
        wcc->wcd_prev = NULL;
        wcc->wcd_next = NULL;



        /* find directory name - chop off ... if found */

        dirnam = wccfile->wcf_result + fna_size[0] + 1;
        dirlen = fna_size[1] - 2;       /* Don't include [] */
        if (dirlen >= 3) {
            if (memcmp(dirnam + dirlen - 3,"...",3) == 0) {
                wccfile->wcf_status |= STATUS_RECURSE;
                dirlen -= 3;
                wccfile->wcf_status |= STATUS_WILDCARD;
            }
        }
        /* see if we can find directory in cache... */

        dirsiz = dirlen;
        do {
            char *dirend = dirnam + dirsiz;
            if (dircache(0,dirnam,dirsiz,&wcc->wcd_dirid)) break;
            while (dirsiz > 0) {
                dirsiz--;
                if (char_delim[*--dirend & 127]) break;
            }
        } while (1);


        /* Create directory wcc blocks for what's left ... */

        while (dirsiz < dirlen) {
            int seglen = 0;
            char *dirptr = dirnam + dirsiz;
            struct WCCDIR *wcd;
            do {
                if (char_delim[*dirptr++ & 127]) break;
                seglen++;
            } while (dirsiz + seglen < dirlen);
	    int alosize;
            int sts = exe_std$alononpaged(sizeof(struct WCCDIR) + seglen + 8, &alosize, &wcd);
	    memset(wcd,0,sizeof(struct WCCDIR) + seglen + 8);
	    wcd->wcd_size = sizeof(struct WCCDIR) + seglen + 8;
            wcd->wcd_wcc = 0;
            wcd->wcd_status = 0;
            wcd->wcd_prelen = 0;
            wcd->wcd_reslen = 0;
            memcpy(wcd->wcd_sernam,dirnam + dirsiz,seglen);
            memcpy(wcd->wcd_sernam + seglen,".DIR;1",7);
            wcd->wcd_serdsc.dsc$w_length = seglen + 6;
            wcd->wcd_serdsc.dsc$a_pointer = wcd->wcd_sernam;
            wcd->wcd_prev = wcc;
            wcd->wcd_next = wcc->wcd_next;
            if (wcc->wcd_next != NULL) wcc->wcd_next->wcd_prev = wcd;
            wcc->wcd_next = wcd;
            wcd->wcd_prelen = fna_size[0] + dirsiz + 1; /* Include [. */
            memcpy(&wcd->wcd_dirid,&wccfile->wcf_wcd.wcd_dirid,sizeof(struct _fiddef));
            dirsiz += seglen + 1;
        }
        wcc->wcd_wcc = 0;
        wcc->wcd_status = 0;
        wcc->wcd_reslen = 0;
        wcc->wcd_serdsc.dsc$w_length = fna_size[2] + fna_size[3] + fna_size[4];
        wcc->wcd_serdsc.dsc$a_pointer = wcc->wcd_sernam;
        memcpy(wcc->wcd_sernam,wccfile->wcf_result + fna_size[0] + fna_size[1],
               wcc->wcd_serdsc.dsc$w_length);
#ifdef DEBUG
        wcc->wcd_sernam[wcc->wcd_serdsc.dsc$w_length] = '\0';
        printk("Parse spec is %s\n",wccfile->wcf_wcd.wcd_sernam);
        for (dirsiz = 0; dirsiz < 5; dirsiz++) printk("  %d",fna_size[dirsiz]);
        printk("\n");
#endif
    }
    if (wccret != NULL) *wccret = wccfile;
    if (nam != NULL) nam->nam$l_wcc =  wccfile;
    return SS$_NORMAL;
}


/* External entry for parse function... */

unsigned exe$parse(struct _fabdef *fab)
{
    int sts;
    struct _namdef *nam = fab->fab$l_nam;
    if (nam == NULL) return RMS$_NAM;
    if (fab->fab$w_ifi != 0) return RMS$_IFI;
    sts = alloc_ifab(fab);
    if ((sts&1)==0)
      return sts;
    sts = do_parse(fab,NULL);
    nam->namdef$$_fill_7[0] = fab->fab$w_ifi; // borrow this instead of nam$l_wcc;
    fab->fab$w_ifi = 0;
    return sts;
}


/* Function to set default directory (heck we can sneak in the device...)  */

unsigned exe$setddir(struct dsc$descriptor *newdir,unsigned short *oldlen,
                     struct dsc$descriptor *olddir)
{
  struct _iosb iosb={0};
    unsigned sts = 1;
    if (oldlen != NULL) {
        int retlen = default_size[0] + default_size[1];
        if (retlen > olddir->dsc$w_length) retlen = olddir->dsc$w_length;
        *oldlen = retlen;
        memcpy(olddir->dsc$a_pointer,default_name,retlen);
    }
    if (newdir != NULL) {
        struct _fabdef fab = cc$rms_fab;
        struct _namdef nam = cc$rms_nam;
        fab.fab$l_nam = &nam;
        nam.nam$b_nop |= NAM$M_SYNCHK;
        nam.nam$b_ess = DEFAULT_SIZE;
        nam.nam$l_esa = default_buffer;
        fab.fab$b_fns = newdir->dsc$w_length;
        fab.fab$l_fna = newdir->dsc$a_pointer;
        sts = exe$parse(&fab);
        if (sts & 1) {
            if (nam.nam$b_name + nam.nam$b_type + nam.nam$b_ver > 2) return RMS$_DIR;
            if (nam.nam$l_fnb & NAM$M_WILDCARD) return RMS$_WLD;
            default_name = default_buffer;
            default_size[0] = nam.nam$b_dev;
            default_size[1] = nam.nam$b_dir;
            memcpy(default_name + nam.nam$b_dev + nam.nam$b_dir,".;",3);
        }
    }
    return sts;
}


/* This version of connect only resets record pointer */

unsigned exe$connect(struct _rabdef *rab)
{
  struct _iosb iosb={0};
    rab->rab$w_rfa[0] = 0;
    rab->rab$w_rfa[1] = 0;
    rab->rab$w_rfa[2] = 0;
    rab->rab$w_rsz = 0;
    if (rab->rab$l_fab->fab$b_org == FAB$C_SEQ) {
        return 1;
    } else {
      //return SS$_NOTINSTALL;
      printk("warning: in very experimental rms index area %x %s\n",rab->rab$l_fab->fab$l_fna,rab->rab$l_fab->fab$l_fna);
	return 1;
    }
}


/* Disconnect is even more boring */

unsigned exe$disconnect(struct _rabdef *rab)
{
  struct _iosb iosb={0};
    return 1;
}



/* get for sequential files */

unsigned exe$get(struct _rabdef *rab)
{
  struct _iosb iosb={0};
  struct dsc$descriptor fibdsc;
  struct _atrdef atr[2];
  struct _fatdef recattr;
  char *buffer,*recbuff;
  unsigned block,blocks,offset;
  unsigned cpylen,reclen;
  unsigned delim,rfm,sts = 1;
  struct _fabdef * fab = rab->rab$l_fab;
  int ifi_no = fab->fab$w_ifi;
  if (ifi_no < 1 || ifi_no >= IFI_MAX) return RMS$_IFI;

  if (rab->rab$l_fab->fab$b_org == FAB$C_IDX)
    return rms$get_idx(rab);

  reclen = rab->rab$w_usz;
  recbuff = rab->rab$l_ubf;
  delim = 0;
  switch (rfm = rab->rab$l_fab->fab$b_rfm) {
  case FAB$C_STMLF:
    delim = 1;
    break;
  case FAB$C_STMCR:
    delim = 2;
    break;
  case FAB$C_STM:
    delim = 3;
    break;
  case FAB$C_VFC:
    reclen += rab->rab$l_fab->fab$b_fsz;
    break;
  case FAB$C_FIX:
    if (reclen < rab->rab$l_fab->fab$w_mrs) return RMS$_RTB;
    reclen = rab->rab$l_fab->fab$w_mrs;
    break;
  }

  offset = rab->rab$w_rfa[2] % 512;
  block = (rab->rab$w_rfa[1] << 16) + rab->rab$w_rfa[0];
  if (block == 0) block = 1;

  atr[0].atr$w_type=ATR$C_RECATTR;
  atr[0].atr$w_size=ATR$S_RECATTR;
  atr[0].atr$l_addr=&recattr;
  atr[1].atr$w_type=0;
  fibdsc.dsc$w_length=sizeof(struct _fibdef);
  fibdsc.dsc$a_pointer=&ifi_table[rab->rab$l_fab->fab$w_ifi]->wcf_fib;
#if 0
  if ((ifb_table[ifi_no]->ifb$l_devchar&DEV$M_TRM))
    printk("get %x %x %x\n",ifb_table[fab->fab$w_ifi]->ifb$w_chnl,fab->fab$l_stv,reclen);
#endif
  if ((ifb_table[ifi_no]->ifb$l_devchar&DEV$M_TRM)==0) {
    sts = sys$qiow(RMS_EF,ifb_table[fab->fab$w_ifi]->ifb$w_chnl,IO$_ACCESS|IO$M_ACCESS,&iosb,0,0,
		   &fibdsc,0,0,0,atr,0);
    sts = iosb.iosb$w_status;
  }

  if ((ifb_table[ifi_no]->ifb$l_devchar&DEV$M_TRM)==0)
  {
    unsigned eofblk;

    eofblk = VMSSWAP(recattr.fat$l_efblk);
    if (block > eofblk || (block == eofblk &&
			   offset >= VMSWORD(recattr.fat$w_ffbyte))) return RMS$_EOF;
  }
  int alosize;
  sts=exe_std$alononpaged(512,&alosize,&buffer);
  sts = sys$qiow(RMS_EF,ifb_table[fab->fab$w_ifi]->ifb$w_chnl,IO$_READVBLK,&iosb,0,0,
		 buffer,512,block,0,0,0);
  sts = iosb.iosb$w_status;
  blocks=1;
  if ((sts & 1) == 0) {
    if (sts == SS$_ENDOFFILE) sts = RMS$_EOF;
    exe_std$deanonpgdsiz(buffer,512);
    return sts;
  }

  if ((ifb_table[ifi_no]->ifb$l_devchar&DEV$M_TRM)) {
    cpylen = iosb.iosb$w_bcnt;
    memcpy(recbuff, buffer, cpylen);
    goto out;
  }

  if (rfm == FAB$C_VAR || rfm == FAB$C_VFC) {
    vmsword *lenptr = (vmsword *) (buffer + offset);
    reclen = VMSWORD(*lenptr);
    offset += 2;
    if (reclen > rab->rab$w_usz) {
      sts = deaccesschunk(0,0,0);
      exe_std$deanonpgdsiz(buffer,512);
      return RMS$_RTB;
    } 
  }

  cpylen = 0;
  while (1) {
    int dellen = 0;
    int seglen = blocks * 512 - offset;
    if (delim) {
      if (delim >= 3) {
	char *ptr = buffer + offset;
	if (dellen == 1 && *ptr != '\n') {
	  if (cpylen >= reclen) {
	    seglen = 0;
	    sts = RMS$_RTB;
	  } else {
	    *recbuff++ = '\r';
	    cpylen++;
	  }
	}
	while (seglen-- > 0) {
	  char ch = *ptr++;
	  if (ch == '\n' || ch == '\f' || ch == '\v') {
	    if (ch == '\n') {
	      dellen++;
	    } else {
	      dellen = 0;
	    }
	    delim = 99;
	    break; 
	  }
	  dellen = 0;
	  if (ch == '\r') dellen = 1;
	}
	seglen = ptr - (buffer + offset) - dellen;;
      } else {
	char *ptr = buffer + offset;
	char term = '\r';
	if (delim == 1) term = '\n';
	while (seglen-- > 0) {
	  if (*ptr++ == term) {
	    dellen = 1;
	    delim = 99;
	    break;
	  }
	}
	seglen = ptr - (buffer + offset) - dellen;;
      }
    } else {
      if (seglen > reclen - cpylen) seglen = reclen - cpylen;
      if (rfm == FAB$C_VFC && cpylen < rab->rab$l_fab->fab$b_fsz) {
	unsigned fsz = rab->rab$l_fab->fab$b_fsz - cpylen;
	if (fsz > seglen) fsz = seglen;
	if (rab->rab$l_rhb) memcpy(rab->rab$l_rhb + cpylen,buffer + offset,fsz);
	cpylen += fsz;
	offset += fsz;
	seglen -= fsz;
      }
    }
    if (seglen) {
      if (cpylen + seglen > reclen) {
	seglen = reclen - cpylen;
	sts = RMS$_RTB;
      }
      memcpy(recbuff,buffer + offset,seglen);
      recbuff += seglen;
      cpylen += seglen;
    }
    offset += seglen + dellen;
    if ((offset & 1) && (rfm == FAB$C_VAR || rfm == FAB$C_VFC)) offset++;
    deaccesschunk(0,0,1);
    if ((sts & 1) == 0) {
      exe_std$deanonpgdsiz(buffer,512);
      return sts;
    }
    block += offset / 512;
    offset %= 512;
    if ((delim == 0 && cpylen >= reclen) || delim == 99) {
      break;
    } else {
      sts = sys$qiow(RMS_EF,ifb_table[fab->fab$w_ifi]->ifb$w_chnl,IO$_READVBLK,&iosb,0,0,
		     buffer,512,block,0,0,0);
      sts = iosb.iosb$w_status;
      blocks=1;
      if ((sts & 1) == 0) {
	if (sts == SS$_ENDOFFILE) sts = RMS$_EOF;
	exe_std$deanonpgdsiz(buffer,512);
	return sts;
      }
      offset = 0;
    }
  }
  if (rfm == FAB$C_VFC) cpylen -= rab->rab$l_fab->fab$b_fsz;
 out:
  rab->rab$w_rsz = cpylen;
  rab->rab$l_rbf = rab->rab$l_ubf;

  rab->rab$w_rfa[0] = block & 0xffff;
  rab->rab$w_rfa[1] = block >> 16;
  rab->rab$w_rfa[2] = offset;
  exe_std$deanonpgdsiz(buffer,512);
  return sts;
}


/* put for sequential files */

unsigned exe$put(struct _rabdef *rab)
{
  struct _iosb iosb={0};
  char buffer[512],*recbuff;
  struct dsc$descriptor fibdsc;
  struct _atrdef atr[2];
  struct _fatdef recattr;
  unsigned block=0,blocks,offset;
  unsigned cpylen,reclen;
  unsigned delim,rfm,sts = 1;
  struct _fabdef * fab = rab->rab$l_fab;
  int ifi_no = fab->fab$w_ifi;
  if (ifi_no < 1 || ifi_no >= IFI_MAX) return RMS$_IFI;
#if 0
  struct _fcb *fcb = ifi_table[rab->rab$l_fab->fab$w_ifi]->wcf_fcb;
  struct _fh2 * head;
  gethead(fcb,&head);
#endif
  reclen = rab->rab$w_rsz;
  recbuff = rab->rab$l_rbf;
  delim = 0;
  switch (rfm = rab->rab$l_fab->fab$b_rfm) {
  case FAB$C_STMLF:
    if (reclen < 1) {
      delim = 1;
    } else {
      if (recbuff[reclen] != '\n') delim = 1;
    }
    break;
  case FAB$C_STMCR:
    if (reclen < 1) {
      delim = 2;
    } else {
      if (recbuff[reclen] != '\r') delim = 2;
    }
    break;
  case FAB$C_STM:
    if (reclen < 2) {
      delim = 3;
    } else {
      if (recbuff[reclen-1] != '\r' || recbuff[reclen] != '\n') delim = 3;
    }
    break;
  case FAB$C_VFC:
    reclen += rab->rab$l_fab->fab$b_fsz;
    break;
  case FAB$C_FIX:
    if (reclen != rab->rab$l_fab->fab$w_mrs) return RMS$_RSZ;
    break;
  }

  atr[0].atr$w_type=ATR$C_RECATTR;
  atr[0].atr$w_size=ATR$S_RECATTR;
  atr[0].atr$l_addr=&recattr;
  atr[1].atr$w_type=0;

  fibdsc.dsc$w_length=sizeof(struct _fibdef);
  fibdsc.dsc$a_pointer=&ifi_table[rab->rab$l_fab->fab$w_ifi]->wcf_fib;
#if 0
  if ((ifb_table[ifi_no]->ifb$l_devchar&DEV$M_TRM))
    printk("put %x %x\n",ifb_table[fab->fab$w_ifi]->ifb$w_chnl,fab->fab$l_stv);
#endif
  if ((ifb_table[ifi_no]->ifb$l_devchar&DEV$M_TRM)==0) {
    sts = sys$qiow(RMS_EF,ifb_table[fab->fab$w_ifi]->ifb$w_chnl,IO$_ACCESS|IO$M_ACCESS,&iosb,0,0,
		   &fibdsc,0,0,0,atr,0);
    sts = iosb.iosb$w_status;
  }

#if 0
  block = VMSSWAP(recattr.fat$l_efblk);
  offset = VMSWORD(recattr.fat$w_ffbyte);
#endif

#if 1
  offset = rab->rab$w_rfa[2] % 512;
  block = (rab->rab$w_rfa[1] << 16) + rab->rab$w_rfa[0];
  if (block == 0) block = 1;
#endif

  //buffer=recbuff;
  if ((ifb_table[ifi_no]->ifb$l_devchar&DEV$M_TRM)==0) {
    sts = sys$qiow(RMS_EF,ifb_table[fab->fab$w_ifi]->ifb$w_chnl,IO$_READVBLK,&iosb,0,0,
		   buffer,512,block,0,0,0);
    sts = iosb.iosb$w_status;
  }
  blocks=1;
  if ((sts & 1) == 0) return sts;

  if (rfm == FAB$C_VAR || rfm == FAB$C_VFC) {
    vmsword *lenptr = (vmsword *) (buffer + offset);
    *lenptr = VMSWORD(reclen);
    offset += 2;
  }

  cpylen = 0;
  while (1) {
    int seglen = blocks * 512 - offset;
    if (seglen > reclen - cpylen) seglen = reclen - cpylen;
    if (rfm == FAB$C_VFC && cpylen < rab->rab$l_fab->fab$b_fsz) {
      unsigned fsz = rab->rab$l_fab->fab$b_fsz - cpylen;
      if (fsz > seglen) fsz = seglen;
      if (rab->rab$l_rhb) {
	memcpy(buffer + offset,rab->rab$l_rhb + cpylen,fsz);
      } else {
	memset(buffer + offset,0,fsz);
      }
      cpylen += fsz;
      offset += fsz;
      seglen -= fsz;
    }
    if (seglen) {
      memcpy(buffer + offset,recbuff,seglen);
      recbuff += seglen;
      cpylen += seglen;
      offset += seglen;
    }
    if (delim && offset < blocks * 512) {
      offset++;
      switch (delim) {
      case 1:
	*buffer = '\n';
	delim = 0;
	break;	
      case 2:
	*buffer = '\r';
	delim = 0;
	break;	
      case 3:
	*buffer = '\r';
	if (offset < blocks * 512) {
	  offset++;
	  *buffer = '\n';
	  delim = 0;
	} else {
	  delim = 2;
	}
	break;	
      }
    }
    sts = deaccesschunk(block,blocks,1);
    if ((sts & 1) == 0) return sts;
    int wbuf_len=512;
    if (ifb_table[ifi_no]->ifb$l_devchar&DEV$M_TRM)
      wbuf_len=reclen;
    if (wbuf_len==0)
      return SS$_NORMAL;
    if ((ifb_table[ifi_no]->ifb$l_devchar&DEV$M_TRM)==0/*rab->rab$v_asy == 0*/)
      sts = sys$qiow(RMS_EF,ifb_table[fab->fab$w_ifi]->ifb$w_chnl,IO$_WRITEVBLK,&iosb,0,0,
		   buffer,wbuf_len,block,0,0,0);
    else {
      sts = sys$qio(RMS_EF,ifb_table[fab->fab$w_ifi]->ifb$w_chnl,IO$_WRITEVBLK,&iosb,0,0,
		    buffer,wbuf_len,block,0,0,0); // workaround for tza
    }
    if ((ifb_table[ifi_no]->ifb$l_devchar&DEV$M_TRM)/*rab->rab$v_asy == 0*/)
      return 1; // check. do better TRM check later
    sts = iosb.iosb$w_status;
    //block += blocks;
    if (cpylen >= reclen && delim == 0) {
      break;
    } else {
      block++;
      if ((ifb_table[ifi_no]->ifb$l_devchar&DEV$M_TRM)==0) {
	sts = sys$qiow(RMS_EF,ifb_table[fab->fab$w_ifi]->ifb$w_chnl,IO$_READVBLK,&iosb,0,0,
		       buffer,512,block,0,0,0);
	sts = iosb.iosb$w_status;
      }
      blocks=1;
      if ((sts & 1) == 0) return sts;
      offset = 0;
    }
  }
  if ((offset & 1) && (rfm == FAB$C_VAR || rfm == FAB$C_VFC)) offset++;
  block += offset / 512;
  offset %= 512;

  if( (block > VMSSWAP(recattr.fat$l_efblk)) || ( (block == VMSSWAP(recattr.fat$l_efblk)) && (offset > VMSWORD(recattr.fat$w_ffbyte)))) {
    recattr.fat$l_efblk = VMSSWAP(block);
    recattr.fat$w_ffbyte = VMSWORD(offset);

    if ((ifb_table[ifi_no]->ifb$l_devchar&DEV$M_TRM)==0) {
      sts = sys$qiow(RMS_EF,ifb_table[fab->fab$w_ifi]->ifb$w_chnl,IO$_MODIFY,&iosb,0,0,
		     &fibdsc,0,0,0,atr,0);

      sts = iosb.iosb$w_status;
    }
  }

  rab->rab$w_rfa[0] = block & 0xffff;
  rab->rab$w_rfa[1] = block >> 16;
  rab->rab$w_rfa[2] = offset;
  return sts;
}

/* display to fill fab & xabs with info from the file header... */

unsigned exe$display(struct _fabdef *fab)
{
    struct _iosb iosb={0};
    struct _xabdatdef *xab = fab->fab$l_xab;

    struct _atrdef atr[8];

    void * myfi$t_filename;
    unsigned short myfi$w_revision = 0;
    unsigned long long myfi$q_credate;
    unsigned long long myfi$q_revdate;
    unsigned long long myfi$q_expdate;
    unsigned long long myfi$q_bakdate;
    void * myfi$t_filenamext;

    struct _fatdef recattr;
    memset(&recattr, 0, sizeof(struct _fatdef));

    int ifi_no = fab->fab$w_ifi;
    unsigned short *pp;
    unsigned short fileprot;
    unsigned long fileowner;
    struct dsc$descriptor fibdsc;
    int chan=ifb_table[fab->fab$w_ifi]->ifb$w_chnl;
    int sts;

    atr[0].atr$w_type=ATR$C_CREDATE;
    atr[0].atr$w_size=ATR$S_CREDATE;
    atr[0].atr$l_addr=&myfi$q_credate;
    atr[1].atr$w_type=ATR$C_REVDATE;
    atr[1].atr$w_size=ATR$S_REVDATE;
    atr[1].atr$l_addr=&myfi$q_revdate;
    atr[2].atr$w_type=ATR$C_EXPDATE;
    atr[2].atr$w_size=ATR$S_EXPDATE;
    atr[2].atr$l_addr=&myfi$q_expdate;
    atr[3].atr$w_type=ATR$C_BAKDATE;
    atr[3].atr$w_size=ATR$S_BAKDATE;
    atr[3].atr$l_addr=&myfi$q_bakdate;
    atr[4].atr$w_type=ATR$C_RECATTR;
    atr[4].atr$w_size=ATR$S_RECATTR;
    atr[4].atr$l_addr=&recattr;
    atr[5].atr$w_type=ATR$C_FPRO;
    atr[5].atr$w_size=ATR$S_FPRO;
    atr[5].atr$l_addr=&fileprot;
    atr[6].atr$w_type=ATR$C_UIC;
    atr[6].atr$w_size=ATR$S_UIC;
    atr[6].atr$l_addr=&fileowner;
    atr[7].atr$w_type=0;
    fibdsc.dsc$w_length=sizeof(struct _fibdef);
    fibdsc.dsc$a_pointer=&ifi_table[fab->fab$w_ifi]->wcf_fib;
    if ((ifb_table[ifi_no]->ifb$l_devchar&DEV$M_TRM)==0) {
      sts = sys$qiow(RMS_EF,ifb_table[fab->fab$w_ifi]->ifb$w_chnl,IO$_ACCESS|IO$M_ACCESS,&iosb,0,0,
		     &fibdsc,0,0,0,atr,0);
      sts = iosb.iosb$w_status;
    }
    if (ifi_no == 0 || ifi_no >= IFI_MAX) return RMS$_IFI;
    fab->fab$l_alq = VMSSWAP(recattr.fat$l_hiblk);
    fab->fab$b_bks = recattr.fat$b_bktsize;
    fab->fab$w_deq = VMSWORD(recattr.fat$w_defext);
    fab->fab$b_fsz = recattr.fat$b_vfcsize;
    fab->fab$w_gbc = VMSWORD(recattr.fat$w_gbc);
    fab->fab$w_mrs = VMSWORD(recattr.fat$w_maxrec);
    fab->fab$b_org = recattr.fat$b_rtype & 0xf0;
    fab->fab$b_rfm = recattr.fat$b_rtype & 0x0f;
    fab->fab$b_rat = recattr.fat$b_rattrib;
    while (xab != NULL) {
        switch (xab->xab$b_cod) {
            case XAB$C_DAT:
	      memcpy(&xab->xab$q_bdt,&myfi$q_bakdate,8);
	      memcpy(&xab->xab$q_cdt,&myfi$q_credate,8);
	      memcpy(&xab->xab$q_edt,&myfi$q_expdate,8);
	      memcpy(&xab->xab$q_rdt,&myfi$q_revdate,8);
	      xab->xab$w_rvn = myfi$w_revision;
	      break;
            case XAB$C_FHC:{
                    struct _xabfhcdef *fhc = (struct _xabfhcdef *) xab;
                    fhc->xab$b_atr = recattr.fat$b_rattrib;
                    fhc->xab$b_bkz = recattr.fat$b_bktsize;
                    fhc->xab$w_dxq = VMSWORD(recattr.fat$w_defext);
                    fhc->xab$l_ebk = VMSSWAP(recattr.fat$l_efblk);
                    fhc->xab$w_ffb = VMSWORD(recattr.fat$w_ffbyte);
                    if (fhc->xab$l_ebk == 0) {
                        fhc->xab$l_ebk = fab->fab$l_alq;
                        if (fhc->xab$w_ffb == 0) fhc->xab$l_ebk++;
                    }
                    fhc->xab$w_gbc = VMSWORD(recattr.fat$w_gbc);
                    fhc->xab$l_hbk = VMSSWAP(recattr.fat$l_hiblk);
                    fhc->xab$b_hsz = recattr.fat$b_vfcsize;
                    fhc->xab$w_lrl = VMSWORD(recattr.fat$w_maxrec);
                    fhc->xab$w_verlimit = VMSWORD(recattr.fat$w_versions);
                }
                break;
            case XAB$C_PRO:{
                    struct _xabprodef1 *pro = (struct _xabprodef1 *) xab;
                    pro->xab$w_pro = VMSWORD(fileprot);
                    memcpy(&pro->xab$l_uic,&fileowner,4);
                }
        }
        xab = xab->xab$l_nxt;
    }

    return 1;
}


/* close a file */

unsigned exe$close(struct _fabdef *fab)
{
    int sts=SS$_NORMAL;
    int ifi_no = fab->fab$w_ifi;
    if (ifi_no < 1 || ifi_no >= IFI_MAX) return RMS$_IFI;
    //sts = deaccessfile(0);
    if (sts & 1) {
        if (ifi_table[ifi_no]->wcf_status & STATUS_TMPWCC) {
            cleanup_wcf(ifi_table[ifi_no]);
            if (fab->fab$l_nam != NULL) fab->fab$l_nam->nam$l_wcc = 0;
        }
        fab->fab$w_ifi = 0;
        ifi_table[ifi_no] = NULL;
	exe_std$deanonpgdsiz(ifb_table[ifi_no],sizeof(struct _ifbdef));
	ifb_table[ifi_no] = NULL;
	ifi_alloc[ifi_no] = NULL;
    }
    return sts;
}


/* open a file */

unsigned exe$open(struct _fabdef *fab)
{
    struct _iosb iosb={0};
    unsigned sts = 1;
    int ifi_no = 1;
    int wcc_flag = 0;
    struct WCCFILE *wccfile = NULL;
    struct _namdef *nam = fab->fab$l_nam;
    struct dsc$descriptor fibdsc;
    struct _atrdef atr[2];
    if (fab->fab$w_ifi != 0) return RMS$_IFI;
    sts = alloc_ifab(fab);
    if ((sts&1)==0)
      return sts;
    ifi_no = fab->fab$w_ifi;
    if (nam != NULL) {
        wccfile = (struct WCCFILE *) nam->nam$l_wcc;
    }
    if (wccfile == NULL) {
        sts = do_parse(fab,&wccfile);
        if (sts & 1) {
            wcc_flag = 1;
            if (wccfile->wcf_status & STATUS_WILDCARD) {
                sts = RMS$_WLD;
            } else {
                sts = do_search(fab,wccfile);
            }
            wccfile->wcf_status |= STATUS_TMPWCC;
        }
    } else {
        sts = 1;
	sts = set_ifab(fab, fab->fab$l_nam->nam$b_dev, wccfile->wcf_result);
    }
    atr[0].atr$w_type=ATR$C_HEADER;
    atr[0].atr$w_size=ATR$S_HEADER;
    atr[0].atr$w_type=0;
    fibdsc.dsc$w_length=sizeof(struct _fibdef);
    fibdsc.dsc$a_pointer=&wccfile->wcf_fib;
    if ((ifb_table[ifi_no]->ifb$l_devchar&DEV$M_TRM)==0) {
      sts = sys$qiow(RMS_EF,ifb_table[ifi_no /*fab->fab$w_ifi*/]->ifb$w_chnl,IO$_ACCESS|IO$M_ACCESS,&iosb,0,0,
		     &fibdsc,0,0,0,atr,0);
      sts = iosb.iosb$w_status;
    }
    if (sts & 1) {
        ifi_table[ifi_no] = wccfile;
	if (fab->fab$w_ifi && fab->fab$w_ifi!=ifi_no)
	  printk("help %x %x %x\n",fab,fab->fab$w_ifi,ifi_no);
        fab->fab$w_ifi = ifi_no;
        // if (head.fh2$w_recattr.fat$b_rtype == 0) head.fh2$w_recattr.fat$b_rtype = FAB$C_STMLF; // of no use now?
        exe$display(fab);
    }
    if (wcc_flag && ((sts & 1) == 0)) {
        cleanup_wcf(wccfile);
        if (nam != NULL) nam->nam$l_wcc = 0;
    }
    if (fab->fab$b_org == FAB$C_IDX) {
      char buffer[512];
      struct _prologue_key * key;
      struct _prologue_area * area;
      struct _xabkeydef * xabkey;
      struct _xaballdef * xaball;
      int block=1,prevblock=0;
      int offset=0;
      int i,sts;
      int rootvbn,datavbn;
      do {
	if (prevblock!=block) {
	  sts = sys$qiow(RMS_EF,ifb_table[fab->fab$w_ifi]->ifb$w_chnl,IO$_READVBLK,&iosb,0,0,
			 buffer,512,block,0,0,0);
	  sts = iosb.iosb$w_status;
	}
	int alosize;
	int sts = exe_std$alononpaged(sizeof(struct _prologue_key), &alosize, key);
	memcpy(key,buffer_offset(buffer,offset),sizeof(struct _prologue_key));
	sts = exe_std$alononpaged(sizeof(struct _xabkeydef), &alosize, &xabkey);
	xabkey->xab$b_cod=XAB$C_KEY;
	memcpy(key,((unsigned long)xabkey)+2,sizeof(struct _prologue_key));
	xabkey->xab$l_dvb=key->key$l_ldvbn;
	xabkey->xab$l_nxt=wccfile->xab;
	wccfile->xab=xabkey;
	prevblock=block;
	block=key->key$l_idxfl;
	offset=key->key$w_noff;
      } while (block);
      rootvbn=key->key$l_rootvbn;
      datavbn=key->key$l_ldvbn;
      block=prevblock+1;
      offset=0;
      for(i=key->key$b_ianum;i;i--) {
	sts = sys$qiow(RMS_EF,ifb_table[fab->fab$w_ifi]->ifb$w_chnl,IO$_READVBLK,&iosb,0,0,
		       buffer,512,block,0,0,0);
	sts = iosb.iosb$w_status;
	area=buffer_offset(buffer,offset);
	int alosize;
	int sts = exe_std$alononpaged(sizeof(struct _xaballdef), &alosize, &xaball);
	xaball->xab$b_cod=XAB$C_ALL;
	xaball->xab$b_aop=area->area$b_aop;
	xaball->xab$b_aln=area->area$b_aln;
	xaball->xab$b_aid=area->area$b_areaid;
	xaball->xab$l_nxt=wccfile->xab;
	wccfile->xab=xabkey;
	offset+=64;;
	if (offset==512) {
	  offset=0;
	  block++;
	}
      }
      for(i=key->key$b_lanum;i;i--) {
	sts = sys$qiow(RMS_EF,ifb_table[fab->fab$w_ifi]->ifb$w_chnl,IO$_READVBLK,&iosb,0,0,
		       buffer,512,block,0,0,0);
	sts = iosb.iosb$w_status;
	area=buffer_offset(buffer,offset);
	int alosize;
	int sts = exe_std$alononpaged(sizeof(struct _xaballdef), &alosize, &xaball);
	xaball->xab$b_cod=XAB$C_ALL;
	xaball->xab$b_aop=area->area$b_aop;
	xaball->xab$b_aln=area->area$b_aln;
	xaball->xab$b_aid=area->area$b_areaid;
	xaball->xab$l_nxt=wccfile->xab;
	wccfile->xab=xabkey;
	offset+=64;;
	if (offset==512) {
	  offset=0;
	  block++;
	}
      }
      if (offset)
	block++;
    block=rootvbn;
    sts = sys$qiow(RMS_EF,ifb_table[fab->fab$w_ifi]->ifb$w_chnl,IO$_READVBLK,&iosb,0,0,
		   buffer,512,block,0,0,0);
    sts = iosb.iosb$w_status;
    {
      struct _bkt * buck = buffer;
      printk("buck\n");
    }
 
    block=datavbn;
    sts = sys$qiow(RMS_EF,ifb_table[fab->fab$w_ifi]->ifb$w_chnl,IO$_READVBLK,&iosb,0,0,
		   buffer,512,block,0,0,0);
    sts = iosb.iosb$w_status;
  }
  if (sts==SS$_NORMAL)
    fab->fab$l_stv=ifb_table[fab->fab$w_ifi]->ifb$w_chnl;
  return sts;
}


/* blow away a file */

unsigned exe$erase(struct _fabdef *fab)
{
  struct _iosb iosb={0};
    unsigned sts;
    int ifi_no = 1;
    int wcc_flag = 0;
    struct WCCFILE *wccfile = NULL;
    struct _namdef *nam = fab->fab$l_nam;
    if (fab->fab$w_ifi != 0) return RMS$_IFI;
    sts = alloc_ifab(fab);
    if ((sts&1)==0)
      return sts;
    ifi_no = fab->fab$w_ifi;
    if (nam != NULL) {
        wccfile = (struct WCCFILE *) fab->fab$l_nam->nam$l_wcc;
    }
    if (wccfile == NULL) {
        sts = do_parse(fab,&wccfile);
        if (sts & 1) {
            wcc_flag = 1;
            if (wccfile->wcf_status & STATUS_WILDCARD) {
                sts = RMS$_WLD;
            } else {
                sts = do_search(fab,wccfile);
            }
        }
    } else {
        sts = 1;
	sts = set_ifab(fab, fab->fab$l_nam->nam$b_dev, wccfile->wcf_result);
    }
    if (sts & 1) {
        struct _fibdef fibblk;
	memset(&fibblk, 0, sizeof(struct _fibdef));
        struct dsc$descriptor fibdsc,serdsc;
        fibdsc.dsc$w_length = sizeof(struct _fibdef);
        fibdsc.dsc$a_pointer = (char *) &fibblk;
        serdsc.dsc$w_length = wccfile->wcf_wcd.wcd_reslen;
        serdsc.dsc$a_pointer = wccfile->wcf_result + wccfile->wcf_wcd.wcd_prelen;
        memcpy(&fibblk.fib$w_did_num,&wccfile->wcf_wcd.wcd_dirid,sizeof(struct _fiddef));
        fibblk.fib$w_nmctl = 0;
        fibblk.fib$l_acctl = 0;
        fibblk.fib$w_fid_num = 0;
        fibblk.fib$w_fid_seq = 0;
        fibblk.fib$b_fid_rvn = 0;
        fibblk.fib$b_fid_nmx = 0;
        fibblk.fib$l_wcc = 0;
	// res not really needed? and serdsc?
	int reslen;
	struct dsc$descriptor resdsc;
	char c[256];
	resdsc.dsc$w_length = 256; // - wcc->wcd_prelen;
	resdsc.dsc$a_pointer = c; //wccfile->wcf_result + wcc->wcd_prelen;
	sts = sys$qiow(RMS_EF,ifb_table[fab->fab$w_ifi]->ifb$w_chnl,IO$_ACCESS/*|IO$M_ACCESS*/,&iosb,0,0,
		       &fibdsc,&serdsc,&reslen,&resdsc,0,0);
	sts = iosb.iosb$w_status;
        if (sts & 1) {
	  sts = sys$qiow(RMS_EF,ifb_table[fab->fab$w_ifi]->ifb$w_chnl,IO$_DELETE|IO$M_DELETE,&iosb,0,0,
			 &fibdsc,&serdsc,&reslen,&resdsc,0,0);
	  sts = iosb.iosb$w_status;
	  //sts = accesserase(wccfile->wcf_vcb,&wccfile->wcf_fib);
	} else {
	    printk("Direct status is %d\n",sts);
	}
    }
    if (wcc_flag) {
        cleanup_wcf(wccfile);
        if (nam != NULL) nam->nam$l_wcc = 0;
    }
    return sts;
}

unsigned exe$create(struct _fabdef *fab)
{
  struct _iosb iosb={0};
  unsigned sts;
  int ifi_no = 1;
  int wcc_flag = 0;
  struct WCCFILE *wccfile = NULL;
  struct _namdef *nam = fab->fab$l_nam;
  struct _atrdef atr[2];
  struct _fatdef recattr;
  if (fab->fab$w_ifi != 0) return RMS$_IFI;
  sts = alloc_ifab(fab);
  if ((sts&1)==0)
    return sts;
  ifi_no = fab->fab$w_ifi;
  if (nam != NULL) {
    wccfile = (struct WCCFILE *) fab->fab$l_nam->nam$l_wcc;
  }
  if (wccfile == NULL) {
    sts = do_parse(fab,&wccfile);
    if (sts & 1) {
      wcc_flag = 1;
      if (wccfile->wcf_status & STATUS_WILDCARD) {
	sts = RMS$_WLD;
      } else {
	sts = do_search(fab,wccfile);
	if (sts == RMS$_FNF) sts = 1;
      }
    }
  } else {
    sts = 1;
    sts = set_ifab(fab, fab->fab$l_nam->nam$b_dev, wccfile->wcf_result);
  }
  if (sts & 1) {
    struct _fibdef fibblk;
    memset(&fibblk, 0, sizeof(struct _fibdef));
    struct dsc$descriptor fibdsc,serdsc;
    fibdsc.dsc$w_length = sizeof(struct _fibdef);
    fibdsc.dsc$a_pointer = (char *) &fibblk;
    serdsc.dsc$w_length = wccfile->wcf_wcd.wcd_reslen;
    serdsc.dsc$a_pointer = wccfile->wcf_result + wccfile->wcf_wcd.wcd_prelen;
    memcpy(&fibblk.fib$w_did_num,&wccfile->wcf_wcd.wcd_dirid,sizeof(struct _fiddef));
    fibblk.fib$w_nmctl = 0;
    fibblk.fib$l_acctl = 0;
    //fibblk.fib$w_did_num = 0;
    //fibblk.fib$w_did_seq = 0;
    //fibblk.fib$b_did_rvn = 0;
    //fibblk.fib$b_did_nmx = 0;
    fibblk.fib$l_wcc = 0;
    fibblk.fib$w_exctl|=FIB$M_EXTEND;
    fibblk.fib$l_exsz=fab->fab$l_alq;

    atr[0].atr$w_type=ATR$C_RECATTR;
    atr[0].atr$w_size=ATR$S_RECATTR;
    atr[0].atr$l_addr=&recattr;
    atr[1].atr$w_type=0;

    memset(&recattr, 0, sizeof(recattr));
    // alq set by extend?
    recattr.fat$b_bktsize = fab->fab$b_bks;
    recattr.fat$w_defext = VMSWORD(fab->fab$w_deq);
    recattr.fat$b_vfcsize = fab->fab$b_fsz;
    recattr.fat$w_gbc = VMSWORD(fab->fab$w_gbc);
    recattr.fat$w_maxrec = VMSWORD(fab->fab$w_mrs);
    recattr.fat$b_rtype = fab->fab$b_rfm | fab->fab$b_org;
    recattr.fat$b_rattrib = fab->fab$b_rat;

    sts = sys$qiow(RMS_EF,ifb_table[fab->fab$w_ifi]->ifb$w_chnl,IO$_CREATE|IO$M_ACCESS,&iosb,0,0,
		   &fibdsc,&wccfile->wcf_wcd.wcd_serdsc,0,0,&atr[0],0);
    if (sts == SS$_ILLIOFUNC) {
      sts = 1;
      goto go;
    }
    sts = iosb.iosb$w_status;
    memcpy(&wccfile->wcf_fib.fib$w_fid_num,&fibblk.fib$w_fid_num,sizeof(struct _fiddef));
  }
 go:
  if (sts & 1) {
    ifi_table[ifi_no] = wccfile;
    fab->fab$w_ifi = ifi_no;
  }
  if (nam != NULL) nam->nam$l_wcc = 0;
  if (sts==SS$_NORMAL)
    fab->fab$l_stv=ifb_table[fab->fab$w_ifi]->ifb$w_chnl;
  return sts;
}

unsigned exe$extend(struct _fabdef *fab)
{
  int ifi_no = fab->fab$w_ifi;
  struct _iosb iosb={0};
  unsigned sts;
  int wcc_flag = 0;
  struct _fibdef fibblk;
  memset(&fibblk, 0, sizeof(struct _fibdef));
  struct dsc$descriptor fibdsc,serdsc;
  struct _atrdef atr[2];
  struct _fatdef recattr;
  struct WCCFILE *wccfile = NULL;
  struct _namdef *nam = fab->fab$l_nam;
  if (fab->fab$w_ifi != 0) return RMS$_IFI;
  sts = alloc_ifab(fab);
  if ((sts&1)==0)
    return sts;
  ifi_no = fab->fab$w_ifi;
  if (nam != NULL) {
    wccfile = (struct WCCFILE *) fab->fab$l_nam->nam$l_wcc;
  }
  if (wccfile == NULL) {
    sts = do_parse(fab,&wccfile);
    if (sts & 1) {
      wcc_flag = 1;
      if (wccfile->wcf_status & STATUS_WILDCARD) {
	sts = RMS$_WLD;
      } else {
	sts = do_search(fab,wccfile);
      }
    }
  } else {
    sts = 1;
    sts = set_ifab(fab, fab->fab$l_nam->nam$b_dev, wccfile->wcf_result);
  }
  fibdsc.dsc$w_length = sizeof(struct _fibdef);
  fibdsc.dsc$a_pointer = (char *) &fibblk;
  serdsc.dsc$w_length = wccfile->wcf_wcd.wcd_reslen;
  serdsc.dsc$a_pointer = wccfile->wcf_result + wccfile->wcf_wcd.wcd_prelen;
  if (ifi_no < 1 || ifi_no >= IFI_MAX) return RMS$_IFI;
  atr[0].atr$w_type=ATR$C_RECATTR;
  atr[0].atr$w_size=ATR$S_RECATTR;
  atr[0].atr$l_addr=&recattr;
  atr[1].atr$w_type=0;
  fibdsc.dsc$w_length=sizeof(struct _fibdef);
  fibdsc.dsc$a_pointer=&ifi_table[fab->fab$w_ifi]->wcf_fib;
  sts = sys$qiow(RMS_EF,ifb_table[fab->fab$w_ifi]->ifb$w_chnl,IO$_ACCESS|IO$M_ACCESS,&iosb,0,0,
		 &fibdsc,0,0,0,atr,0);
  sts = iosb.iosb$w_status;
  fibblk.fib$l_wcc = 0;
  fibblk.fib$w_exctl|=FIB$M_EXTEND;
  fibblk.fib$l_exsz=fab->fab$l_alq - recattr.fat$l_efblk;
  sts = sys$qiow(RMS_EF,ifb_table[fab->fab$w_ifi]->ifb$w_chnl,IO$_MODIFY|IO$M_ACCESS,&iosb,0,0,
		   &fibdsc,&wccfile->wcf_wcd.wcd_serdsc,0,0,0,0);
  sts = iosb.iosb$w_status;
  //sts = update_extend(0 ,fab->fab$l_alq - ifi_table[ifi_no]->wcf_fcb->fcb$l_efblk,0);
  memcpy(&fibblk.fib$w_did_num,&wccfile->wcf_wcd.wcd_dirid,sizeof(struct _fiddef));
  fibblk.fib$w_nmctl = 0;
  fibblk.fib$l_acctl = 0;
  //fibblk.fib$w_did_num = 0;
  //fibblk.fib$w_did_seq = 0;
  //fibblk.fib$b_did_rvn = 0;
  //fibblk.fib$b_did_nmx = 0;
  fibblk.fib$l_wcc = 0;
  fibblk.fib$w_exctl|=FIB$M_EXTEND;
  fibblk.fib$l_exsz=10;
  sts = sys$qiow(RMS_EF,ifb_table[fab->fab$w_ifi]->ifb$w_chnl,IO$_MODIFY,&iosb,0,0,
		 &fibdsc,&wccfile->wcf_wcd.wcd_serdsc,0,0,0,0);
  sts = iosb.iosb$w_status;
  return sts;
}
