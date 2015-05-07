// $Id$
// $Locker$

// Author. Roar Thron�s.
// Modified DFU source file, 2004.

/*
    DFU V2.2

    DFU_IFSCAN.C

    This file contains all routines needed
    for scanning and parsing the Index File

    Called by SEARCH, REPORT , VERIFY ,UNDELETE,
    and several DIRECTORY options

*/

#ifndef VAXC
#pragma message disable(ALIGNEXT)
#pragma message disable(GLOBALEXT)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_hdr.h"
#include "home2def.h"
#include <ssdef.h>
#include <descrip.h>
#include "fibdef.h"
#include <starlet.h>
#include <clidef.h>
#include <climsgdef.h>
#include <libdef.h>
#include <sor$routines.h>
#include <str$routines.h>
#include <cli$routines.h>
#include <lib$routines.h>
#include <smg$routines.h>
#include <atrdef.h>
#include <dcdef.h>
#include <devdef.h>
#include <mntdef.h>
#include <dvidef.h>
#include <iodef.h>
#include <strdef.h>
#include <trmdef.h>
#include <smgdef.h>
#include "smgdef2.h"

#include "dfu.h"

#define globalvalue int
#define TRUE 1
#define FALSE 0
#define SYS$QIO sys$qiow
#define SYS$QIOW sys$qiow
#define SYS$ASSIGN sys$assign
#define SYS$DASSGN sys$dassgn
#define SYS$SEARCH sys$search
#define SYS$PARSE sys$parse
#define SYS$FAO sys$fao
#define SYS$ASCTIM sys$asctim
#define SYS$BINTIM sys$bintim
#define SYS$WAITFR sys$waitfr
#define SYS$GETJPIW sys$getjpiw
#define SYS$GETSYIW sys$getsyiw
#define SYS$GETDVIW sys$getdviw
#define CLI$PRESENT cli$present
#define CLI$GET_VALUE cli$get_value

typedef struct _flist
{
    char name[86];
    short flen;
};
typedef struct _quad
{
    int date[2];
};
typedef struct f_id
{
    unsigned short fid_num, fid_seq;
    unsigned char fid_rvn, fid_nmx;
} ;
typedef struct _it3
{
    short buflen, itemcode;
    unsigned int *bufadr;
    unsigned int *retlen;
};

static struct _cln
{
    unsigned int channels : 1;
    unsigned int events : 1;
    unsigned int fopen : 1;
    unsigned int sort : 1;
    unsigned int expreg : 1;
} clean_flags;

/* Structure for work list */
struct work
{
    unsigned short fid_num, fid_seq;
    unsigned char fid_rvn, fid_nmx;
    unsigned short did_num, did_seq;
    unsigned char did_rvn, did_nmx;
    char name[86];
    char function;
    short rvn;
    struct work *next;
} *first, *list;

extern unsigned int ctrlc, SOR$GK_RECORD, keyb_id, tchan,
       rows, colls, disp1_id, paste_id, lbnstart, lbncount;
extern Boolean trigger, smg$enable;
extern int ctrlt_ast();
extern char outbuf[255];
globalvalue DFU_NOPRIV, DFU_EXPREG, DFU_ASSIGN, DFU_INDEXERR,
            DFU_EXHFAIL, DFU_IOERR;

const int iocnt=2;

char block[512];    /* One page/block */

#if 0
extern _align(PAGE) struct _hdr
{
    /* Large enough to hold 1001 headers */
    char block[512];
} header[1001];
#else
extern struct _hdr
{
    /* Large enough to hold 1001 headers */
    char block[512];
} header[1001];
#endif

static struct _ibmap
{
    char block[512];
} bitmap[257]; /* Space for index file bitmap */

static struct HM2_Struct home[33];  /* 32 home blocks */

static struct _vol      /* Structure for relative volume table */
{
    char devnam[64], fulldevnam[64];
    unsigned int devnam_len, fulldevnam_len,
             i_open,vbn_file_1,if_size, maxblocks;
    unsigned short channel, bchan;
    Boolean wlk, slow;
    int addr[2], bmap_addr[2]; /* For EXPREG calls */
} rvt[33];

static unsigned int volcount, maxvol, volnum, orig_rvn, curvol, strategy;
static unsigned int lendev, status, iosize, vbn, headers, highbit;
static unsigned short lock_chan;
static char device[64];

static struct
{
    unsigned short iosb_1;
    unsigned int length;
    short spec;
} iostat[3];

/* context block for I/O processing */
static struct
{
    unsigned int i, starti, endi, thread;
    struct header_area_struct *hdr;
    struct ident_area_struct *id;
    Boolean end;
} ctx;

static int efn[2]; /* Event flags */

static unsigned short map_ptr[4];
static Boolean progress_ind;

static struct _dr
{
    short lendir;
    char dirnam[198];
} dirrec;

static struct
{
    unsigned short atr_size, atr_type;
    int *atr_address;
}
acb[2]= {{200, ATR$C_FILE_SPEC, (int *) &dirrec },
    { 0,0,0}
}; /* Attribute control block */

static struct
{
    int forward_link, handler_addr, arg_count, condition;
} desblk;

static FILE *fp;

void cleanup(), copy_fid(), disass_map_ptr(), add_item(),
     read_indexf_bitmap(), fid_to_name();

int open_device(), get_next_header();


int display_stat(struct dsc$descriptor *out_str, int user_arg)
/* user action routine to catch LIB$SHOW_TIMER output */
{
    if (smg$enable)
    {
        sprintf(outbuf,"%.*s\n",out_str->dsc$w_length, out_str->dsc$a_pointer);
        put_status(2);
    }
    else
        printf("\n%.*s\n",out_str->dsc$w_length, out_str->dsc$a_pointer);
    return(1);
}

void do_abort(void)
{
    sprintf(outbuf,"\n%%DFU-W-ABORTED , command interrupted by CTRL/C");
    put_disp();
    sprintf(outbuf," ");
    put_status(2);
}

int search_command (int mask)

/*
    Search through INDEXF.SYS for files with certain
    attributes specified on the command line

*/

{
    static char dummy_item[80], name[86], dir[200],
           fname[80], ident[30], date_s[23];
    struct header_area_struct *hdr;
    struct ident_area_struct *id;
    short keybuf[5] = {1, DSC$K_DTYPE_T, 0, 0, 255 };
    int reclen = 255;
    struct _flist file[50], exclfile[50];
    int fcnt, efcnt, flgs;
    struct _quad cre_bef, cre_sin, mod_bef, mod_sin,
            exp_bef, exp_sin, bak_bef, bak_sin, diff;
    register int i;
    Boolean matmult, dispfull, matsumm, matplaced, matnovolset,
            matnosearch, mathome, matstat, matalloc, dummy, matlbn,
            mat_r_minsize, mat_r_maxsize, mat_a_minsize, mat_a_maxsize,
            matoutput , matname, matexcl, found, matnonebackup, matnoneexpired,
            matbefcreated, matsincreated, matbefmodified, matsinmodified,
            matbefbackup, matsinbackup, matbefexpired, matsinexpired,matsort,
            matbakfid, matfid, matchars, mator, matmark, matuic, matfragment,
            matversion;
    int size_min, size_max, parse_type, free_hdr, ivbn, k, j, x,
        bitblk, bitje, bitval, find, rvn , flags, size,
        retr_min, retr_max, vers_min, vers_max, hdrs, rtvptr, lbn, spacing;
    int parse_item(), read_indexf_multi(), dfu_check();
    unsigned short fid[3] ;
    unsigned int r_size, a_size , tot_r_size, tot_a_size, fcount, val_headers,
             fidnum, infid, bakfid, chars, setchar, uic, tothdr, totfrag,
             offset, map_words;
    void get_map_pointers();
    /* FAO parameters */
    int faolen;
    unsigned short outlen;
    $DESCRIPTOR(out_descr , outbuf);
    $DESCRIPTOR(device_descr , device);
    $DESCRIPTOR(dummy_descr , dummy_item);
    $DESCRIPTOR(dir_descr , dir);
    $DESCRIPTOR(file_descr , fname);
    $DESCRIPTOR(name_descr , name);
    $DESCRIPTOR(id_descr , ident);
    $DESCRIPTOR(date_descr , date_s);
    $DESCRIPTOR(ctrstr0, "!%I (!%U)");
    $DESCRIPTOR(ctrstr4, "!#AS!9UL/!UL");
    $DESCRIPTOR(ctrstr7, "!#AS!9UL/!9<!UL!>!4UL/!UL");

    /* Code starts here */
    /* Parse the various qualifiers */

    progress_ind = TRUE;
    /* Get device name */
    status = parse_item("device", &device_descr, 0, &dummy , 0);
    if (status == 1)
{
        if (strindex(&device,":",64) == -1)
        {
            device[device_descr.dsc$w_length]=':';
            device_descr.dsc$w_length += 1;
        }
    }
    clean_flags.fopen = 0;
    flgs = SMG$M_WRAP_CHAR;
    /* Check file name(s) */
    dummy_descr.dsc$a_pointer = (char *) &fcnt;
    status = parse_item("file", &dummy_descr, &file[0].name, &matname , 3);
    /* Check exclude file name(s) */
    dummy_descr.dsc$a_pointer = (char *) &efcnt;
    status = parse_item("exclude", &dummy_descr, &exclfile[0].name,
                        &matexcl ,3);
    dummy_descr.dsc$a_pointer = (char *) &dummy_item;
    /* Search on allocated filesize */
    status = parse_item("allocated", &dummy_descr, 0, &matalloc, 2);
    if (matalloc == TRUE)
    {
        /* Check minimum size entered */
        status = parse_item("size_min", &dummy_descr, &size_min, &mat_a_minsize , 1);
        if ((status & 1) != 1) return(status);
        /* Check maximum size entered */
        status = parse_item("size_max", &dummy_descr, &size_max, &mat_a_maxsize , 1);
        if ((status & 1) != 1) return(status);
        mat_r_minsize = FALSE;
        mat_r_maxsize = FALSE;
        if (mat_a_minsize == TRUE && mat_a_maxsize == TRUE )
            if (size_min > size_max)
            {
                sprintf(outbuf,"%%DFU-E-SIZEERR, Minimum size > maximum size\n");
                put_disp();
                cleanup();
                return(1);
            }
    }
    else
    {
        /* Check minimum size entered */
        status = parse_item("size_min", &dummy_descr, &size_min, &mat_r_minsize , 1);
        if ((status & 1) != 1) return(status);
        /* Check maximum size entered */
        status = parse_item("size_max", &dummy_descr, &size_max, &mat_r_maxsize , 1);
        if ((status & 1) != 1) return(status);
        mat_a_minsize = FALSE;
        mat_a_maxsize = FALSE;
        if (mat_r_minsize == TRUE && mat_r_maxsize == TRUE )
            if (size_min > size_max)
            {
                sprintf(outbuf,"%%DFU-E-SIZEERR, Minimum size > maximum size\n");
                put_disp();
                cleanup();
                return(1);
            }
    }
    /* Check backlink file id */
    status = parse_item("bakfid", &dummy_descr, &bakfid, &matbakfid , 1);
    if ((status & 1) != 1) return(status);
    /* Check specific LBN */
    status = parse_item("lbn", &dummy_descr, &lbn, &matlbn , 1);
    if ((status & 1) != 1) return(status);
    /* Check file id */
    status = parse_item("fid", &dummy_descr, &infid, &matfid , 1);
    if ((status & 1) != 1) return(status);
    /* Files with multi headers */
    status = parse_item("multiple", &dummy_descr, 0, &matmult, 2);
    /* Full output display */
    status = parse_item("full", &dummy_descr, 0, &dispfull, 2);
    /* Summary output*/
    status = parse_item("summary", &dummy_descr, 0, &matsumm, 2);
    /* Files with placement pointers */
    status = parse_item("placed", &dummy_descr, 0, &matplaced, 2);
    /* Disallow volume set processing*/
    status = parse_item("novolset", &dummy_descr, 0, &matnovolset, 2);
    /* Home block only */
    status = parse_item("nosearch", &dummy_descr, 0, &matnosearch, 2);
    /* Display home block info */
    status = parse_item("home", &dummy_descr, 0, &mathome, 2);
    /* Display statistics */
    status = parse_item("statistics", &dummy_descr, 0, &matstat, 2);
    /* Sort output */
    status = parse_item("sort", &dummy_descr, 0, &matsort, 2);
    /* Fragmentation */
    status = parse_item("fragment", &dummy_descr, 0, &matfragment, 2);
    if (matfragment == TRUE)
    {
        retr_min = 0;
        retr_max = 0;
        status = parse_item("retr_min", &dummy_descr, &retr_min, &matfragment , 1);
        if ((status & 1) != 1) return(status);
        status = parse_item("retr_max", &dummy_descr, &retr_max, &matfragment , 1);
        if ((status & 1) != 1) return(status);
        matfragment = TRUE;
    }
    /* Version check */
    status = parse_item("version_number", &dummy_descr, 0, &matversion, 2);
    if (matversion == TRUE)
    {
        vers_min = 0;
        vers_max = 0;
        status = parse_item("vers_min", &dummy_descr, &vers_min, &matversion , 1);
        if ((status & 1) != 1) return(status);
        status = parse_item("vers_max", &dummy_descr, &vers_max, &matversion , 1);
        if ((status & 1) != 1) return(status);
        matversion = TRUE;
    }
    /* Date and time options */
    status = parse_item("bak_none", &dummy_descr, 0, &matnonebackup, 2);
    status = parse_item("exp_none", &dummy_descr, 0, &matnoneexpired, 2);
    status = parse_item("cre_before", &dummy_descr, &cre_bef, &matbefcreated, 4);
    status = parse_item("cre_since", &dummy_descr, &cre_sin, &matsincreated, 4);
    status = parse_item("mod_before", &dummy_descr, &mod_bef, &matbefmodified,4);
    status = parse_item("mod_since", &dummy_descr, &mod_sin, &matsinmodified,4);
    status = parse_item("bak_before", &dummy_descr, &bak_bef, &matbefbackup, 4);
    status = parse_item("bak_since", &dummy_descr, &bak_sin, &matsinbackup, 4);
    status = parse_item("exp_before", &dummy_descr, &exp_bef, &matbefexpired,4);
    status = parse_item("exp_since", &dummy_descr, &exp_sin, &matsinexpired,4);
    /* File characteristics options */
    chars=0;
    dummy_descr.dsc$a_pointer = (char *) &chars;
    status = parse_item("or", &dummy_descr, 0, &mator, 2);
    setchar = FH2$M_NOBACKUP;
    status = parse_item("nobackup", &dummy_descr, &setchar, &matchars, 5);
    setchar = FH2$M_CONTIGB;
    status = parse_item("besttry", &dummy_descr, &setchar, &matchars, 5);
    setchar = FH2$M_LOCKED;
    status = parse_item("locked", &dummy_descr, &setchar, &matchars, 5);
    setchar = FH2$M_CONTIG;
    status = parse_item("contiguous", &dummy_descr, &setchar, &matchars, 5);
    setchar = FH2$M_BADACL;
    status = parse_item("badacl", &dummy_descr, &setchar, &matchars, 5);
    setchar = FH2$M_SPOOL;
    status = parse_item("spool", &dummy_descr, &setchar, &matchars, 5);
    setchar = FH2$M_DIRECTORY;
    status = parse_item("directory", &dummy_descr, &setchar, &matchars, 5);
    setchar = FH2$M_BADBLOCK;
    status = parse_item("badblock", &dummy_descr, &setchar, &matchars, 5);
    setchar = FH2$M_MARKDEL;
    status = parse_item("marked", &dummy_descr, &setchar, &matmark, 5);
    if (matmark == TRUE) matchars = TRUE;
    setchar = FH2$M_NOCHARGE;
    status = parse_item("nocharge", &dummy_descr, &setchar, &matchars, 5);
    setchar = FH2$M_ERASE;
    status = parse_item("erase", &dummy_descr, &setchar, &matchars, 5);
    setchar = FH2$M_SCRATCH;
    status = parse_item("scratch", &dummy_descr, &setchar, &matchars, 5);
    setchar = FH2$M_NOMOVE;
    status = parse_item("nomove", &dummy_descr, &setchar, &matchars, 5);
    setchar = FH2$M_NOSHELVABLE;
    status = parse_item("noshelvable", &dummy_descr, &setchar, &matchars, 5);
    setchar = FH2$M_SHELVED;
    status = parse_item("isshelved", &dummy_descr, &setchar, &matchars, 5);
    if (chars !=0) matchars = TRUE;
    dummy_descr.dsc$a_pointer = (char *) &dummy_item;
    /* Ident or UIC */
    status = parse_item("ident", &id_descr, 0, &matuic, 0);
#if 0
    if (matuic == TRUE)
    {
        status = SYS$ASCTOID(&id_descr, &uic, 0);
        if ((status & 1) != 1)
        {
            sprintf(outbuf,"%%DFU-E-INVID, Invalid identifier entered,\n");
            put_disp();
            singlemsg(0,status);
            return(status);
        }
    }
#endif
    if (matuic == FALSE)
        status = parse_item("uic", &dummy_descr, &uic, &matuic, 6);
    if ((status &1 ) != 1)
    {
        sprintf(outbuf,"%%DFU-E-INVID, Invalid identifier entered\n");
        put_disp();
        return(1);
    }
    id_descr.dsc$w_length = 30;

    /* Get output file */
    status = parse_item("outfile", &file_descr, 0, &matoutput , 0);
    if (matoutput == FALSE)
    {
        strcpy(fname,"SYS$OUTPUT:");
        file_descr.dsc$w_length = 11;
    }
    else
        fname[file_descr.dsc$w_length] = '\0';
#if 0
    fp = fopen(fname,"w","mrs=255","rfm=var","ctx=rec","rat=cr","rop=WBH");
#else
    fp = stdout;
#endif
    clean_flags.fopen = 1;
#if 1
    if (matsort == FALSE)
        clean_flags.sort = 0;
    else
    {
        status = sor$begin_sort(&keybuf,&reclen,0,0,0,0,SOR$GK_RECORD,0,0);
        if ((status &1 ) != 1)
        {
            singlemsg(0,status);
            return(status);
        }
        clean_flags.sort = 1;
    }
#endif
    /* All qualifiers parsed */

    if (matstat == TRUE) status = lib$init_timer(0);
    clean_flags.channels = 0;
    clean_flags.events = 0;
    clean_flags.expreg = 0;
    tot_r_size = 0;
    tot_a_size = 0;
    tothdr = 0;
    totfrag = 0;
    fcount = 0;
    val_headers = 0;
    spacing = colls;

    /* Open the volume (set) , read in the home block */

    status = open_device(&device_descr,0);
    if ((status & 1) != 1)
    {
        cleanup();
        return(status);
    }

    curvol = 1;
    size = 1;
    ctx.end = FALSE;
    if (smg$enable) SMG$SET_CURSOR_MODE(&paste_id,&SMG$M_CURSOR_OFF);
    while ((curvol <= maxvol) && (!ctx.end))
    {
        ctx.i = -1; /* Clear context */
        if (rvt[curvol].i_open ==1)
        {
            if (maxvol == 1)
            {
                sprintf(outbuf,"%%DFU-I-SEARCH, Start search on %.*s (%s)\n",
                        device_descr.dsc$w_length, device,
                        &rvt[curvol].fulldevnam[1]);
            }
            else
            {
                if (matnovolset == TRUE) curvol = orig_rvn; /*No volume set */
                sprintf(outbuf,"%%DFU-I-SEARCH, Start search on %.*s (%s) , RVN = %d\n",
                        rvt[curvol].devnam_len, rvt[curvol].devnam,
                        &rvt[curvol].fulldevnam[1],curvol);
            }
            put_disp();
            headers=0;
            val_headers = 0;
            /* Read in index file bitmap, and return free hdrs + highest bit set */
            read_indexf_bitmap(&free_hdr);

            /* Display home block info if needed */
            if (mathome == TRUE)
            {
                sprintf(outbuf,"\nVolume name       :  %.12s",home[curvol].volname);
                put_disp();
                sprintf(outbuf,"Volume owner      :  %.12s",home[curvol].ownername);
                put_disp();
                sprintf(outbuf,"Structure name    :  %.12s",home[curvol].strucname);
                put_disp();
                sprintf(outbuf,"Cluster size      :  %d",home[curvol].cluster);
                put_disp();
                sprintf(outbuf,"Maximum # files   :  %d",home[curvol].maxfiles);
                put_disp();
                sprintf(outbuf,"Header count      :  %d",rvt[curvol].if_size);
                put_disp();
                sprintf(outbuf,"First header VBN  :  %d",rvt[curvol].vbn_file_1);
                put_disp();
                sprintf(outbuf,"Available headers :  %d\n",free_hdr);
                put_disp();
            }

            if (matnosearch == TRUE ) ctx.end = TRUE;
            status = get_next_header();
            if ((status & 1) != 1) return(status);
            while (! ctx.end) /* Loop until end of INDEXF or CTRL C entered */
            {
                hdr = ctx.hdr;
                id = ctx.id;
                if (matfid == TRUE) /* Special case if FID requested */
                {
                    ctx.end = TRUE;
                    if (infid > rvt[curvol].if_size)
                    {
                        sprintf(outbuf,"%%DFU-E-FIDTOOBIG, File id specified > max file id\n");
                        put_disp();
                        goto nexti;
                    }
                    vbn = rvt[curvol].vbn_file_1 + infid - 1;
                    status = SYS$QIOW(0,rvt[curvol].channel,IO$_READVBLK,
                                      &iostat[(ctx.thread)-1],0,0,&header[ctx.i],512,vbn,0,0,0);
                }
                /* Check corresponding bitmap bit (must be set) */
                bitblk = (headers + 4095) / 4096 - 1;
                bitje = (headers - 1 - bitblk*4096) / 32;
                bitval = (headers - 1) % 32;
                status = lib$ffs(&bitval,&size,&bitmap[bitblk+1].block[bitje*4],
                                 &find);
                if (status == LIB$_NOTFOU) goto nexti;
                rvn = curvol;
                if (matlbn == FALSE ) if (hdr->seg_num !=0) goto nexti; /* Skip extension header */
                rtvptr = 0;
                status = verify_header(hdr);
                if (status != SS$_NORMAL) goto nexti; /* Not a valid header */

                /* Check marked for delete files, skip unless explicitly requested */
                if (matmark == FALSE)
                    if ((hdr->filechar & FH2$M_MARKDEL) == FH2$M_MARKDEL) goto nexti;

                /*  We have a valid file header.
                    Now check the various attributes for a match
                */
                val_headers++;
                /* Check for specific LBN */
                if (matlbn == TRUE)
                {
                    found = FALSE;
                    offset = hdr->mpoffset; /* Start of map area */
                    map_words = hdr->map_inuse;
                    for (j = offset; j<(offset+map_words);)
                    {
                        disass_map_ptr(hdr,&j);
                        if ((lbn >= lbnstart) && (lbn < (lbnstart + lbncount)))
                        {
                            found = TRUE;
                            j = offset + map_words + 1;
                        }
                    }
                    if (!found) goto nexti;
                    if (hdr->seg_num > 0 ) /* read backlink for primary header */
                    {
                        rvn = hdr->bk_fid_overlay.bk_fid_fields.bk_fid_rvn;
                        if (rvn == 0) rvn = curvol;
                        fidnum = hdr->bk_fid_overlay.bk_fid_fields.bk_fid_nmx << 16;
                        fidnum += hdr->bk_fid_overlay.backlink[0];
                        ivbn = rvt[rvn].vbn_file_1 + fidnum - 1;
                        status = SYS$QIOW(0,rvt[rvn].channel,IO$_READVBLK,
                                          &iostat[iocnt],0,0,&header[ctx.i],512,ivbn,0,0,0);
                        if ((status & 1) == 1) status = iostat[iocnt].iosb_1;
                        if ((status & 1) != 1)
                        {
                            sprintf(outbuf,"%%DFU-E-READERR, Error reading backlink header,\n");
                            put_disp();
                            singlemsg(0,status);
                            goto nexti;
                        }
                        status = verify_header(hdr);
                        if (status != SS$_NORMAL) goto nexti; /* Not a valid header */
                    }
                }

                /* Get size fields (NOTE : they are stored in reverse order ! */
                a_size = hdr->hiblk_overlay.hiblk_fields.hiblkh << 16;
                a_size += hdr->hiblk_overlay.hiblk_fields.hiblkl;
                r_size = hdr->efblk_overlay.efblk_fields.efblkh << 16;
                r_size += hdr->efblk_overlay.efblk_fields.efblkl;
                if (r_size > 0)
                    if (hdr->ffbyte == 0) r_size--; /* Correct size on block boundary*/

                /* Check for 'placed' files */
                if (matplaced)
                {
                    j = hdr->mpoffset; /* Start of map area */
                    if (hdr->map_inuse == 0) goto nexti; /* Skip empty files */
                    disass_map_ptr(hdr,&j);
                    if (lbncount != 0) goto nexti;
                }

                /* Check file characteristics */
                if (matchars == TRUE )
                {
                    setchar = (hdr->filechar & chars);
                    if (mator == FALSE)
                        if (setchar != chars) goto nexti;
                        else if (setchar == 0) goto nexti;
                }

                /* Check files with backlink id */
                if (matbakfid == TRUE)
                {
                    fidnum = hdr->bk_fid_overlay.bk_fid_fields.bk_fid_nmx << 16;
                    fidnum += hdr->bk_fid_overlay.bk_fid_fields.bk_fid_num;
                    if (fidnum != bakfid) goto nexti;
                }

                /* Check multiheader files */
                if (matmult == TRUE)
                    if (hdr->ext_fid_overlay.ext_fid[0] == 0) goto nexti;

                /* Check the UIC */
                if (matuic == TRUE)
                    if (uic != hdr->fileowner_overlay.fileowner) goto nexti;

                /* Check fragmentation */
                hdrs = 1;
                rtvptr = 0;
                if (matfragment ==  TRUE)
                {
                    get_map_pointers(hdr, &rtvptr);
                    if (hdr->ext_fid_overlay.ext_fid[0] != 0)
                        status = follow_extents(hdr, &rtvptr, &hdrs);
                    if (rtvptr < retr_min) goto nexti;
                    if (retr_max !=0) if (rtvptr > retr_max) goto nexti;
                }

                /* Check the file name */
                if (matname || matexcl || matversion)
                {
                    memcpy(&name[0],id->filename,20);
                    if (name[19] != ' ') memcpy(&name[20],id->filenamext,66);
                    name_descr.dsc$w_length = strindex(name," ",86);
                }
                if (matname == TRUE)
                {
                    j=0;
                    found = FALSE;
                    while (!found)
                    {
                        dummy_descr.dsc$a_pointer = (char *) &file[j].name;
                        dummy_descr.dsc$w_length = file[j].flen;
                        status = str$match_wild(&name_descr,&dummy_descr);
                        if (status == STR$_MATCH) found = TRUE;
                        j++;
                        if (j+1 > fcnt ) found = TRUE;
                    }
                    if (status != STR$_MATCH) goto nexti;
                }
                if (matexcl == TRUE)
                {
                    j=0;
                    found = FALSE;
                    while (!found)
                    {
                        dummy_descr.dsc$a_pointer = (char *) &exclfile[j].name;
                        dummy_descr.dsc$w_length = exclfile[j].flen;
                        status = str$match_wild(&name_descr,&dummy_descr);
                        if (status == STR$_MATCH) found = TRUE;
                        j++;
                        if (j+1 > efcnt ) found = TRUE;
                    }
                    if (status == STR$_MATCH) goto nexti;
                }

                /* Version check */
                if (matversion == TRUE)
                {
                    name[name_descr.dsc$w_length]=0;
                    j = atol ( &name[strindex(name,";",86)+1] );
                    if ( j < vers_min) goto nexti;
                    if ( vers_max != 0) if ( j > vers_max ) goto nexti;
                }

                /* Check date/time options */
                if (matbefcreated == TRUE)
                {
                    status = lib$subx(&cre_bef, &id->credate[0], &diff, 0);
                    if (diff.date[1] <= 0) goto nexti;
                }
                if (matsincreated == TRUE)
                {
                    status = lib$subx(&cre_sin, &id->credate[0], &diff, 0);
                    if (diff.date[1] > 0 ) goto nexti;
                }
                if (matbefmodified == TRUE)
                {
                    status = lib$subx(&mod_bef, &id->revdate[0], &diff, 0);
                    if (diff.date[1] <= 0) goto nexti;
                }
                if (matsinmodified == TRUE)
                {
                    status = lib$subx(&mod_sin, &id->revdate[0], &diff, 0);
                    if (diff.date[1] > 0 ) goto nexti;
                }
                if (matbefbackup == TRUE)
                    /* Skip files without backup date */
                {
                    if (id->bakdate[0] == 0 && id->bakdate[1] == 0) goto nexti;
                    status = lib$subx(&bak_bef, &id->bakdate[0], &diff, 0);
                    if (diff.date[1] <= 0) goto nexti;
                }
                if (matsinbackup == TRUE)
                {
                    status = lib$subx(&bak_sin, &id->bakdate[0], &diff, 0);
                    if (diff.date[1] > 0 ) goto nexti;
                }
                if (matbefexpired == TRUE)
                    /* Skip files without expiration date */
                {
                    if (id->expdate[0] == 0 && id->expdate[1] == 0) goto nexti;
                    status = lib$subx(&exp_bef, &id->expdate[0], &diff, 0);
                    if (diff.date[1] <= 0) goto nexti;
                }
                if (matsinexpired == TRUE)
                {
                    status = lib$subx(&exp_sin, &id->expdate[0], &diff, 0);
                    if (diff.date[1] > 0 ) goto nexti;
                }
                if (matnonebackup == TRUE )
                    if (id->bakdate[0] > 0 || id->bakdate[1] > 0) goto nexti;
                if (matnoneexpired == TRUE )
                    if (id->expdate[0] > 0 || id->expdate[1] > 0) goto nexti;

                /* Check file sizes */
                if (mat_r_minsize) if (r_size < size_min) goto nexti;
                if (mat_r_maxsize) if (r_size > size_max) goto nexti;
                if (mat_a_minsize) if (a_size < size_min) goto nexti;
                if (mat_a_maxsize) if (a_size > size_max) goto nexti;

                /* Post the results to user */

                tot_r_size += r_size;
                tot_a_size += a_size;
                tothdr += hdrs;
                totfrag += rtvptr;
                fcount++;
                if (matsumm == TRUE) goto nexti;
                fid_to_name(&dir); /* Get the file name */
                dir_descr.dsc$w_length = strlen(dir);
                if (dispfull == FALSE )
                {
                    if (matfragment == FALSE)
                    {
                        if (spacing > 80) faolen = 86 + spacing*((dir_descr.dsc$w_length + 46 - 1)/spacing);
                        else faolen = 56 + spacing*((dir_descr.dsc$w_length + 24 - 1)/spacing);
                        status = SYS$FAO(&ctrstr4, &outlen, &out_descr,
                                         faolen, &dir_descr, r_size, a_size);
                    }
                    else
                    {
                        if (spacing > 80) faolen = 76 + spacing*((dir_descr.dsc$w_length + 56 - 1)/spacing);
                        else faolen = 46 + spacing*((dir_descr.dsc$w_length + 34 - 1)/spacing);
                        status = SYS$FAO(&ctrstr7, &outlen, &out_descr,
                                         faolen, &dir_descr, r_size, a_size, hdrs, rtvptr);
                    }
                    if (matsort == FALSE)
                    {
                        if (matoutput) fprintf(fp,"%.*s\n",outlen,outbuf);
                        outbuf[outlen] = '\0';
                        put_disp();
                        out_descr.dsc$w_length = 255;
                    }
                    else
                    {
                        out_descr.dsc$w_length = outlen;
#if 1
                        sor$release_rec(&out_descr,0);
#endif
                        out_descr.dsc$w_length = 255;
                    }
                }
                else
                {
                    x = strindex(&dirrec.dirnam,"]",198);
                    x++;
                    sprintf(outbuf,"Filename: %s",&dirrec.dirnam[x]);
                    put_disp();
                    if (matoutput) fprintf(fp,"%s\n",outbuf);
                    fidnum = hdr->bk_fid_overlay.bk_fid_fields.bk_fid_nmx << 16;
                    fidnum += hdr->bk_fid_overlay.bk_fid_fields.bk_fid_num;
                    sprintf(outbuf,"Directory: %.*s , Backlink ID (%d,%d,%d)",
                            x,&dirrec.dirnam[0],fidnum,
                            hdr->bk_fid_overlay.bk_fid_fields.bk_fid_seq,
                            hdr->bk_fid_overlay.bk_fid_fields.bk_fid_rvn);
                    put_disp();
                    if (matoutput) fprintf(fp,"%s\n",outbuf);
                    status = SYS$FAO(&ctrstr0, &id_descr.dsc$w_length , &id_descr,
                                     hdr->fileowner_overlay.fileowner, hdr->fileowner_overlay.fileowner);
                    fidnum = hdr->fid_overlay.fid_fields.fid_nmx << 16;
                    fidnum += hdr->fid_overlay.fid[0];
                    ivbn = rvt[curvol].vbn_file_1 + fidnum - 1;
                    sprintf(outbuf,"VBN : %d , File ID (%d,%d,%d) , UIC : %.*s",
                            ivbn, fidnum, hdr->fid_overlay.fid[1],
                            hdr->fid_overlay.fid_fields.fid_rvn,id_descr.dsc$w_length,ident);
                    put_disp();
                    if (matoutput) fprintf(fp,"%s\n",outbuf);
                    id_descr.dsc$w_length = 30;
                    sprintf(outbuf,"Size : %d/%d,     Revision count : %d",
                            r_size, a_size, id->revision);
                    put_disp();
                    if (matoutput) fprintf(fp,"%s\n",outbuf);
                    status = SYS$ASCTIM(&date_descr.dsc$w_length ,&date_descr,
                                        &id->credate,0);
                    sprintf(outbuf, "Created : %.*s",date_descr.dsc$w_length, date_s);
                    put_disp();
                    if (matoutput) fprintf(fp,"%s\n",outbuf);
                    status = SYS$ASCTIM(&date_descr.dsc$w_length ,&date_descr,
                                        &id->revdate,0);
                    sprintf(outbuf, "Revised : %.*s",date_descr.dsc$w_length, date_s);
                    put_disp();
                    if (matoutput) fprintf(fp,"%s\n",outbuf);
                    if (id->expdate[0] == 0 && id->expdate[1] == 0)
                    {
                        strcpy(date_s,"<None specified>");
                        sprintf(outbuf, "Expired : %s", date_s);
                        put_disp();
                        if (matoutput) fprintf(fp,"%s\n",outbuf);
                    }
                    else
                    {
                        status = SYS$ASCTIM(&date_descr.dsc$w_length ,&date_descr,
                                            &id->expdate,0);
                        sprintf(outbuf, "Expired : %.*s",date_descr.dsc$w_length, date_s);
                        put_disp();
                        if (matoutput) fprintf(fp,"%s\n",outbuf);
                    }
                    if (id->bakdate[0] == 0 && id->bakdate[1] == 0)
                    {
                        strcpy(date_s,"<No backup recorded>");
                        sprintf(outbuf, "Backup  : %s\n", date_s);
                        put_disp();
                        if (matoutput) fprintf(fp,"%s\n",outbuf);
                    }
                    else
                    {
                        status = SYS$ASCTIM(&date_descr.dsc$w_length ,&date_descr,
                                            &id->bakdate,0);
                        sprintf(outbuf, "Backup  : %.*s\n",date_descr.dsc$w_length, date_s);
                        put_disp();
                        if (matoutput) fprintf(fp,"%s\n",outbuf);
                    }
                }

                /* Next header */
nexti:
                status = get_next_header();
                if ((status & 1) != 1) return(status);
            }
        }
        if (ctrlc !=1)
        {
            sprintf(outbuf,"     Progress : 100%%     Status : READY");
            put_status(1);
            sprintf(outbuf,"\n%%DFU-I-EOF, End of file INDEXF.SYS, Primary headers : %d\n"
                    ,val_headers);
            put_disp();
        }
        curvol++;
        ctx.end = FALSE; /* Next volume unless ...*/
        if (matnovolset == TRUE) ctx.end = TRUE;
        if (clean_flags.events == 1)
        {
            for (i=1; i <= iocnt ; i++) status = lib$free_ef(&efn[i-1]);
            clean_flags.events = 0;
        }
    }

    if (ctrlc == 1)
    {
        ctrlc = 0;
        do_abort();
    }
#if 1
    if (matsort == TRUE)
    {
        sprintf(outbuf,"%%DFU-I-SORT, Sorting ... \n");
        put_disp();
        status = sor$sort_merge(0);
        if ((status & 1 ) != 1)
        {
            sprintf(outbuf,"%%DFU-E-SORTERR, Error sorting output,\n");
            put_disp();
            singlemsg(0,status);
        }
        status = sor$return_rec(&out_descr, &outlen);
        while (status != SS$_ENDOFFILE)
        {
            outbuf[outlen] = '\0';
            put_disp();
            if (matoutput) fprintf(fp,"%.*s\n",outlen,outbuf);
            status = sor$return_rec(&out_descr, &outlen);
        }
        sprintf(outbuf," \n");
        put_disp();
    }
#endif
    if (matfragment == FALSE)
    {
        sprintf(outbuf,"%%DFU-S-FND , Files found : %d, Size : %d/%d\n",
                fcount,tot_r_size,tot_a_size);
        put_disp();
    }
    else
    {
        sprintf(outbuf,"%%DFU-S-FND , Files found : %d, Size : %d/%d, Hdr/Frag : %d/%d\n",
                fcount,tot_r_size,tot_a_size, tothdr, totfrag);
        put_disp();
    }
    /* Now cleanup the stuff */
    cleanup();
    if (matstat == TRUE) status = lib$show_timer(0,0,display_stat,0);
    return(1);
}

int report_command(int mask)
/*
    Create disk fragmentation report
    by scanning INDEXF.SYS, BITMAP.SYS and QUOTA.SYS
*/

{
    static char dummy_item[80], fname[80], bfile[255];
    Boolean dummy, matoutput, matgraph, matusage, matstat, matnoindex;
    struct header_area_struct *hdr;
    register int i;
    unsigned short badfid[3];
    float a, b, avefrag, scale;
    int ivbn, bitblk, bitje, bitval, find, rvn, x,
        fcount, mcount, acount , ccount, dcount, ucount, badrtv, if_frag,
        bad_r_size, bad_a_size, mark_r_size, mark_a_size, mmp_hdr, mmp_blks,
        r_size, a_size, hdrs, rtvptr, free_hdr, totfrag, tothdr,
        tot_r_size, tot_a_size, map_in_use ,map_prc , k, size;
    int freeext, largest, freeblocks , large_lbn;
    unsigned int page_cnt, table[70];
    char line[70];
    struct
    {
        int own_uic, flag, rsize, asize, hdr;
    } usage_table[750];
    void get_map_pointers(),
         add_usage(), report_usage();
    /* FAO parameters */
    unsigned short outlen;
    $DESCRIPTOR(out_descr , outbuf);
    $DESCRIPTOR(badfile , bfile);
    $DESCRIPTOR(device_descr , device);
    $DESCRIPTOR(file_descr , fname);
    $DESCRIPTOR(dummy_descr , dummy_item);

    /* Code starts here */
    /* Parse the various qualifiers */

    progress_ind = TRUE;
    /* Get device name */
    status = parse_item("device", &device_descr, 0, &dummy , 0);
    if (status == 1)
    {
        if (strindex(&device,":",64) == -1)
        {
            device[device_descr.dsc$w_length]=':';
            device_descr.dsc$w_length += 1;
        }
    }
    status = parse_item("graph", &dummy_descr, 0, &matgraph, 2);
    status = parse_item("nofile", &dummy_descr, 0, &matnoindex, 2);
    status = parse_item("statistics", &dummy_descr, 0, &matstat, 2);
    status = parse_item("usage", &dummy_descr, 0, &matusage, 2);
    if (matusage == TRUE) for (i=0; i < 750; i++) usage_table[i].flag = 0;
    /* Get output file */
    clean_flags.fopen = 0;
    status = parse_item("outfile", &file_descr, 0, &matoutput , 0);
    if (matoutput == FALSE)
    {
        strcpy(fname,"SYS$OUTPUT:");
        file_descr.dsc$w_length = 11;
    }
    else
    {
        fname[file_descr.dsc$w_length] = '\0';
    }
#if 0
    fp = fopen(fname,"w","mrs=255","rfm=var","ctx=rec","rat=cr","rop=WBH");
#else
    fp= stdout;
#endif
    clean_flags.fopen = 1;

    /* All qualifiers parsed */
    if (matstat == TRUE) status = lib$init_timer(0);
    clean_flags.sort = 0;
    clean_flags.channels = 0;
    clean_flags.events = 0;
    clean_flags.expreg = 0;

    /* Open the volume (set) , read in the home block */

    status = open_device(&device_descr,0);
    if ((status & 1) != 1) return(status);

    curvol = 1;
    size = 1;
    ctx.end = FALSE;
    if (smg$enable) SMG$SET_CURSOR_MODE(&paste_id,&SMG$M_CURSOR_OFF);
    while ((curvol <= maxvol) && (!ctx.end))
    {
        ctx.i = -1; /* Clear context */
        if (rvt[curvol].i_open ==1)
        {
            if (maxvol == 1)
            {
                sprintf(outbuf,"%%DFU-I-REPORT, Reporting on %.*s (%s)\n",
                        device_descr.dsc$w_length, device, &rvt[curvol].fulldevnam[1]);
            }
            else
            {
                sprintf(outbuf,"%%DFU-I-REPORT, Reporting on %.*s (%s) , RVN = %d\n",
                        rvt[curvol].devnam_len, rvt[curvol].devnam,
                        &rvt[curvol].fulldevnam[1], curvol);
            }
            put_disp();
            headers=0;
            /* Read in index file bitmap, and return free hdrs + highest bit set */
            read_indexf_bitmap(&free_hdr);
            fcount = 0;
            mcount = 0;
            acount = 0;
            ccount = 0;
            dcount = 0;
            ucount = 0;
            bad_r_size = 0;
            bad_a_size = 0;
            mark_r_size = 0;
            mark_a_size = 0;
            badrtv = 0;
            mmp_hdr = 0;
            mmp_blks = 0;
            headers = 0;
            tot_r_size = 0;
            tot_a_size = 0;
            tothdr = 0;
            totfrag = 0;

            /* Report home block info */
            sprintf(outbuf,"      ***** Volume info for %.*s (from HOME block) *****",
                    rvt[curvol].devnam_len, rvt[curvol].devnam);
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
            sprintf(outbuf," Volume name                      :  %.12s",
                    home[curvol].volname);
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
            sprintf(outbuf," Volume owner                     :  %.12s",
                    home[curvol].ownername);
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
            sprintf(outbuf," Structure name                   :  %.12s",
                    home[curvol].strucname);
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
            sprintf(outbuf," Cluster size                     :  %d",
                    home[curvol].cluster);
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
            sprintf(outbuf," Maximum # files                  :  %d",
                    home[curvol].maxfiles);
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
            sprintf(outbuf," Header count                     :  %d",
                    rvt[curvol].if_size);
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
            sprintf(outbuf," First header VBN                 :  %d",
                    rvt[curvol].vbn_file_1);
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
            sprintf(outbuf," Free headers                     :  %d\n",free_hdr);
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);

            status = get_next_header();
            if ((status & 1) != 1) return(status);
            if (matnoindex) ctx.end = TRUE; /*Skip indexf processing */
            while (! ctx.end) /* Loop until end of INDEXF or CTRL C entered */
            {
                hdr = ctx.hdr;
                /* Check corresponding bitmap bit (must be set) */
                bitblk = (headers + 4095) / 4096 - 1;
                bitje = (headers - 1 - bitblk*4096) / 32;
                bitval = (headers - 1) % 32;
                status = lib$ffs(&bitval,&size,&bitmap[bitblk+1].block[bitje*4],
                                 &find);
                if (status == LIB$_NOTFOU) goto next_rep;
                rvn = curvol;
                if (hdr->seg_num !=0) goto next_rep; /* Skip extension header */
                status = verify_header(hdr);
                if (status != SS$_NORMAL) goto next_rep; /* Not a valid header */
                /* We have a valid header, proceed...*/
                fcount +=1;
                if (headers == 1)
                {
                    map_in_use = hdr->map_inuse;
                    map_prc = (100*map_in_use) / (hdr->acoffset - hdr->mpoffset);
                }

                /* Get size fields (NOTE : they are stored in reverse order ! */
                a_size = hdr->hiblk_overlay.hiblk_fields.hiblkh << 16;
                a_size += hdr->hiblk_overlay.hiblk_fields.hiblkl;
                r_size = hdr->efblk_overlay.efblk_fields.efblkh << 16;
                r_size += hdr->efblk_overlay.efblk_fields.efblkl;
                if (r_size > 0)
                    if (hdr->ffbyte == 0) r_size--; /* Correct size on block boundary*/
                if (a_size > 0) acount++;
                tot_r_size += r_size;
                tot_a_size += a_size;

                /* Increment counters */
                if ((hdr->filechar & FH2$M_MARKDEL) == FH2$M_MARKDEL)
                {
                    mcount++;
                    mark_r_size += r_size;
                    mark_a_size += a_size;
                }
                if ((hdr->filechar & FH2$M_DIRECTORY) == FH2$M_DIRECTORY) dcount++;

                /* Get fragmentation info */
                hdrs = 1;
                rtvptr = 0;
                get_map_pointers(hdr, &rtvptr);
                if (hdr->ext_fid_overlay.ext_fid[0] !=0)
                {
                    ucount++;
                    status = follow_extents(hdr, &rtvptr, &hdrs);
                }
                if (headers == 1) if_frag = rtvptr; /* Indexf.Sys fragments */
                if (rtvptr == 1) ccount++;
                else
                {
                    if (headers == 1) /* Special case for INDEXF.SYS */
                        if (rtvptr == 4) ccount++;
                        else
                        {
                            mmp_hdr = mmp_hdr + rtvptr - 3;
                            mmp_blks = mmp_blks + a_size - rvt[curvol].vbn_file_1;
                        }
                    else if (rtvptr > 3)
                    {
                        mmp_hdr = mmp_hdr + rtvptr - 3;
                        mmp_blks += a_size;
                    }
                }
                tothdr += hdrs;
                if (headers == 1) totfrag++;
                else if (a_size != 0) totfrag += rtvptr;
                if (rtvptr > badrtv)
                {
                    badfid[0] = hdr->fid_overlay.fid[0];
                    badfid[1] = hdr->fid_overlay.fid[1];
                    badfid[2] = curvol + (hdr->fid_overlay.fid_fields.fid_nmx << 16);
                    badrtv = rtvptr;
                    bad_r_size = r_size;
                    bad_a_size = a_size;
                }
                /* Update usage table */
                if ((matusage == TRUE) && (headers > 3))
                    add_usage(&usage_table, hdr->fileowner_overlay.fileowner,
                              r_size, a_size, hdrs);
                /* Next header */
next_rep:
                status = get_next_header();
                if ((status & 1) != 1) return(status);
            }
        }
        if (ctrlc !=1)
        {
            sprintf(outbuf,"     Progress : 100%%     Status : READY");
            put_status(1);
        }
        if ((ctrlc !=1) && (!matnoindex))
        {
            sprintf(outbuf,"      ***** File Statistics (from INDEXF.SYS) *****");
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
            sprintf(outbuf,
                    " INDEXF.SYS fragments/ map_in_use :  %d /%d words ( %d%% used)"
                    ,if_frag, map_in_use, map_prc);
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
            sprintf(outbuf," Total files                      :  %d", fcount);
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
            scale = (fcount > 50) ? 1.0 : fcount/50;
            fcount -= acount;
            sprintf(outbuf," Empty files                      :  %d", fcount);
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
            sprintf(outbuf," Files with allocation            :  %d", acount);
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
            sprintf(outbuf," Files with extension headers     :  %d", ucount);
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
            if (mcount == 0)
                sprintf(outbuf," Files marked for delete          :  %d", mcount);
            else
                sprintf(outbuf," Files marked for delete          :  %d, size %d/%d blocks",
                        mcount, mark_r_size, mark_a_size);
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
            sprintf(outbuf," Directory files                  :  %d", dcount);
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
            sprintf(outbuf," Contiguous files                 :  %d", ccount);
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
            sprintf(outbuf," Total used/ allocated size       :  %d /%d",
                    tot_r_size, tot_a_size);
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
            sprintf(outbuf," Total fragments                  :  %d", totfrag);
            a = totfrag;
            b = acount;
            avefrag = a/b;
            sprintf(outbuf," Average fragments per file       :  %.3f", avefrag);
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
            a = mmp_hdr;
            b = mmp_blks / home[curvol].cluster;
            avefrag = (avefrag*avefrag)-1;
            if (b > 0) avefrag += (a/b*10);
            strcpy(dummy_item," (poor) ");
            if (avefrag < 3.0) strcpy(dummy_item," (fair) ");
            if (avefrag < 2.0) strcpy(dummy_item," (good) ");
            if (avefrag < 1.0) strcpy(dummy_item," (excellent) ");
            sprintf(outbuf," File fragmentation index         :  %.3f %s",
                    avefrag,dummy_item);
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
            tot_a_size = tot_a_size / totfrag;
            sprintf(outbuf," Average size per fragment        :  %d", tot_a_size);
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
            sprintf(outbuf," Most fragmented file             : ");
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
#if 0
            status = lib$fid_to_name(&device_descr, &badfid[0],&badfile,
                                     &outlen, 0, 0);
#endif
            x = strindex(bfile,"[",255);
            outlen -=x;
            sprintf(outbuf,"    %s%.*s ( %d/%d blocks; %d fragments)\n",
                    &rvt[curvol].fulldevnam[1],
                    outlen, &bfile[x], bad_r_size, bad_a_size, badrtv);
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
        }
        if (ctrlc !=1)
            /* Now continue with bitmap processing */
        {
            sprintf(outbuf,"     Progress :   0%%     Status : Processing BITMAP.SYS");
            put_status(1);
            strcpy(device, rvt[curvol].devnam);
            device_descr.dsc$w_length = rvt[curvol].devnam_len;
            rvt[curvol].bchan = 0;
            status = SYS$ASSIGN(&device_descr, &rvt[curvol].bchan, 0, 0);
            if ((status & 1) != 1)
            {
                singlemsg(DFU_ASSIGN,status);
                cleanup();
                return(status);
            }
            page_cnt = (rvt[curvol].maxblocks / home[curvol].cluster) ;
            for (i=1; i<= 70; i++) table[i-1]=100;
            status = scan_bitmap(rvt[curvol].bchan, page_cnt, rvt[curvol].wlk, &freeext,
                                 &largest, &freeblocks, &large_lbn, &table);
            if (status == 1)
            {
                sprintf(outbuf,"     Progress : 100%%     Status : READY");
                put_status(1);
                largest = largest * home[curvol].cluster; /* Adjust for cluster size */
                freeblocks = freeblocks * home[curvol].cluster;
                large_lbn = large_lbn * home[curvol].cluster;
                sprintf(outbuf,"      ***** Free space statistics (from BITMAP.SYS) *****");
                put_disp();
                if (matoutput) fprintf(fp,"%s\n",outbuf);
                sprintf(outbuf," Total blocks on disk             :  %d",
                        rvt[curvol].maxblocks);
                put_disp();
                if (matoutput) fprintf(fp,"%s\n",outbuf);
                sprintf(outbuf," Total free blocks                :  %d",freeblocks);
                put_disp();
                if (matoutput) fprintf(fp,"%s\n",outbuf);
                tot_a_size = (100*freeblocks) / rvt[curvol].maxblocks;
                sprintf(outbuf," Percentage free (rounded)        :  %d",tot_a_size);
                put_disp();
                if (matoutput) fprintf(fp,"%s\n",outbuf);
                sprintf(outbuf," Total free extents               :  %d",freeext);
                put_disp();
                if (matoutput) fprintf(fp,"%s\n",outbuf);
                sprintf(outbuf,
                        " Largest free extent              :  %d  blocks at LBN: %d",
                        largest, large_lbn);
                put_disp();
                if (matoutput) fprintf(fp,"%s\n",outbuf);
                tot_a_size = (freeext == 0) ? 0 : freeblocks / freeext;
                sprintf(outbuf," Average extent size (rounded)    :  %d",tot_a_size);
                put_disp();
                if (matoutput) fprintf(fp,"%s\n",outbuf);
                a = freeext - 1;
                b = (freeblocks/home[curvol].cluster - 1);
                avefrag = (a > 0) ? a/b*100 : 0;
                strcpy(dummy_item," (poor) ");
                if (avefrag < 3.0) strcpy(dummy_item," (fair) ");
                if (avefrag < 2.0) strcpy(dummy_item," (good) ");
                if (avefrag < 1.0) strcpy(dummy_item," (excellent) ");
                sprintf(outbuf," Free space fragmentation index   :  %.3f %s",
                        avefrag,dummy_item);
                put_disp();
                if (matoutput) fprintf(fp,"%s\n",outbuf);
                if (matgraph == TRUE)
                {
                    /* Now print the graph table */
                    tot_a_size = 105;
                    sprintf(outbuf,
                            "\n %%free----------------------------------------------------------------------");
                    put_disp();
                    if (matoutput) fprintf(fp,"%s\n",outbuf);
                    for (k=1; k <=20; k++)
                    {
                        tot_a_size -= 5;
                        for (i=1; i<=70; i++)
                            if (table[i-1] >= tot_a_size)
                                line[i-1] = '*';
                            else
                                line[i-1] = ' ';
                        sprintf(outbuf," %3d |%.*s|",tot_a_size,70,line);
                        put_disp();
                        if (matoutput) fprintf(fp,"%s\n",outbuf);
                    }
                    sprintf(outbuf,
                            "      ----------------------------------------------------------------------");
                    put_disp();
                    if (matoutput) fprintf(fp,"%s\n",outbuf);
                    sprintf(outbuf,
                            " LBN: 0                                                                %d",
                            rvt[curvol].maxblocks);
                    put_disp();
                    if (matoutput) fprintf(fp,"%s\n",outbuf);
                    tot_a_size = rvt[curvol].maxblocks/1400;
                    sprintf(outbuf," Free space distribution; each * = %d free blocks",
                            tot_a_size);
                    put_disp();
                    if (matoutput) fprintf(fp,"%s",outbuf);
                }
            }
            else
            {
                cleanup();
                return(status);
            }
            curvol++;
            if (ctrlc !=1) ctx.end = FALSE; /* Next volume */
            if (curvol <= maxvol)
            {
                sprintf(outbuf," ");
                put_disp();
            }
            if (clean_flags.events == 1)
            {
                for (i=1; i <= iocnt ; i++) status = lib$free_ef(&efn[i-1]);
                clean_flags.events = 0;
            }
        }
    }
    curvol = 1;
    /* Now continue with usage table processing */
    if ((matusage == TRUE) && (ctrlc != 1))
    {
        sprintf(outbuf,"     Progress :   0%%     Status : Processing QUOTA.SYS");
        put_status(1);
        report_usage(&usage_table, rvt[curvol].bchan, fp,matoutput);
        sprintf(outbuf,"     Progress : 100%%     Status : READY");
        put_status(1);
    }
    if (ctrlc == 1) do_abort();
    cleanup();
    sprintf(outbuf,"\n%%DFU-I-READY, REPORT command ready");
    put_disp();
    if (matstat == TRUE) status = lib$show_timer(0,0,display_stat,0);
    return(1);
}

int undel_command(int mask)
/*
    Undelete a file on a disk
    After write locking the disk
    the INDEXF.SYS and BITMAP files are updated
    unless the blocks or file headers have been reused.
    Undeleted files are entered in the original directory
    May also be used to produce a listing of recoverable files
*/

{
    static char dummy_item[80], fname[86], name[86], ans[4], ident[30], *tmp;
    struct header_area_struct *hdr, *oldhdr;
    struct ident_area_struct *id;
    struct _hd
    {
        unsigned short block[256] ;
    } *head;
    struct
    {
        int pagecnt;
    } bmap[33];
    static struct fibdef fib;   /* File information block */
    struct
    {
        int fiblen;
        struct fibdef *fibadr;
    } fibdescr;
    struct f_id lost_fid;
    Boolean dummy, matstat, matname, matlist, matuic, matnoconfirm,
            matoutput, found;
    register int i;
    unsigned short qchan;
    int ivbn, bitblk, bitje, bitval, find, rvn, x, j,
        uic, s_uic, a_size, hdrs, rtvptr, free_hdr, k, size,
        bytes, fidnum, modifiers, fcount, syslost;
    void dfu_handler(), add_quota();
    $DESCRIPTOR(device_descr , device);
    $DESCRIPTOR(file_descr , fname);
    $DESCRIPTOR(dummy_descr , dummy_item);
    $DESCRIPTOR(id_descr , ident);
    $DESCRIPTOR(name_descr , name);
    $DESCRIPTOR(answer , ans);
    $DESCRIPTOR(prompt,"Recover this file ? (Y/N) [N] : ");

    /* Code starts here */
    /* Check the privileges */
    if (mask > -1)
    {
        singlemsg(0,DFU_NOPRIV);
        return(SS$_NOPRIV);
    }
    /* Fill in FIB */
    fibdescr.fiblen = sizeof(fib);
    fibdescr.fibadr = &fib;
    syslost = 0;
    progress_ind = TRUE;

    /* Parse the various qualifiers */
    /* Get device name */
    status = parse_item("device", &device_descr, 0, &dummy , 0);
    if (status == 1)
    {
        if (strindex(&device,":",64) == -1)
        {
            device[device_descr.dsc$w_length]=':';
            device_descr.dsc$w_length += 1;
        }
    }
    status = parse_item("statistics", &dummy_descr, 0, &matstat, 2);
    status = parse_item("noconfirm", &dummy_descr, 0, &matnoconfirm, 2);
    /* Get output file */
    clean_flags.fopen = 0;
    matoutput = FALSE;
    status = parse_item("list", &file_descr, 0, &matlist , 0);
    if (matlist)
    {
        fname[file_descr.dsc$w_length] = '\0';
        status = strncmp(fname,"SYS$OUTPUT",10);
        if (status != 0) matoutput = TRUE;
    }
    if (matoutput)
#if 0
    {
        fp = fopen(fname,"w","mrs=255","rfm=var","ctx=rec","rat=cr","rop=WBH");
        clean_flags.fopen = 1;
    }
#else
    {
        fp = stdout;
        clean_flags.fopen = 1;
    }
#endif

    /* Ident or UIC */
    status = parse_item("ident", &id_descr, 0, &matuic, 0);
#if 0
    if (matuic == TRUE)
    {
        status = SYS$ASCTOID(&id_descr, &uic, 0);
        if ((status & 1) != 1)
        {
            sprintf(outbuf,"%%DFU-E-INVID, Invalid identifier entered,");
            put_disp();
            singlemsg(0,status);
            return(status);
        }
    }
#endif
    if (matuic == FALSE)
        status = parse_item("uic", &dummy_descr, &uic, &matuic, 6);
    if ((status &1 ) != 1)
    {
        sprintf(outbuf,"%%DFU-E-INVID, Invalid identifier entered,");
        put_disp();
        singlemsg(0,status);
        return(status);
    }
    id_descr.dsc$w_length = 30;

    /* Check file name */
    dummy_descr.dsc$a_pointer = (char *) &x;
    status = parse_item("file", &dummy_descr, &fname, &matname , 3);
    if (matname == FALSE) strcpy(fname,"*.*;*");
    if (strindex(&fname,"\0",80) == -1)
    {
        x = strindex(&fname," ",80);
        fname[x] = '\0';
    }
    dummy_descr.dsc$a_pointer = (char *) &fname;
    dummy_descr.dsc$w_length = strindex(&fname,"\0",86);

    /* All qualifiers parsed */
    first = NULL;
    list = NULL;
    clean_flags.channels = 0;
    clean_flags.events = 0;
    clean_flags.expreg = 0;
    modifiers = SMG$M_NOKEEP | SMG$M_NORECALL;
    if (matstat == TRUE) status = lib$init_timer(0);

    /* Open the volume (set) , read in the home block */

    if (matlist) status = open_device(&device_descr,0);
    else status = open_device(&device_descr,1);
    if ((status & 1) != 1) return(status);
    if (rvt[1].wlk == TRUE) /* Write locked device */
    {
        sprintf(outbuf,"%%DFU-E-WRITELK, Cannot undelete on write-locked device");
        put_disp();
        cleanup();
        return(1);
    }
    if (home[1].volchar_overlay.volchar_bits.erase == 1) /*Erase on delete */
    {
        sprintf(outbuf,
                "%%DFU-E-ERASED, Cannot undelete ; erase-on-delete set on device");
        put_disp();
        cleanup();
        return(1);
    }

    /* Setup exit handler to ensure that we unlock the volume */
    if (!matlist)
    {
        desblk.handler_addr = (int) &dfu_handler;
        desblk.condition = (int) &status;
        desblk.arg_count = 1;
        status = SYS$DCLEXH(&desblk);
        if ((status & 1) !=1)
        {
            singlemsg(DFU_EXHFAIL,status);
            cleanup();
            return(status);
        }
    }

    fcount = 0;
    curvol = 1;
    size = 1;
    ctx.end = FALSE;

    /* Read in the BITMAP.SYS we need it anyway... */
    sprintf(outbuf,"%%DFU-I-READBMAP, Reading BITMAP.SYS...");
    put_disp();
    for (x=1; x <=maxvol; x++)
        /* Assign a channel for BITMAP, create dynamic space, and read in the BITMAP*/
    {
        strcpy(device, rvt[x].devnam);
        device_descr.dsc$w_length = rvt[x].devnam_len;
        status = SYS$ASSIGN(&device_descr, &rvt[x].bchan, 0, 0);
        if ((status & 1) != 1)
        {
            singlemsg(DFU_ASSIGN,status);
            cleanup();
            return(status);
        }
        clean_flags.expreg = 1;
        rvt[x].bmap_addr[0] = 0;
        bmap[x].pagecnt = rvt[x].maxblocks/home[x].cluster;
        bmap[x].pagecnt = (bmap[x].pagecnt + 4095) / 4096;
        status = SYS$EXPREG(bmap[x].pagecnt, &rvt[x].bmap_addr[0],
                            0 , 0);
        if ((status & 1) != 1)
        {
            singlemsg(DFU_EXPREG,status);
            cleanup();
            return(status);
        }
        status = read_bitmap(rvt[x].bchan, bmap[x].pagecnt,
                             rvt[x].wlk, &rvt[x].bmap_addr[0]);
        if ((status & 1) != 1)
        {
            sprintf(outbuf,"%%DFU-E-NOTUNDEL, File undeletes not possible,");
            put_disp();
            cleanup();
            return(status);
        }
    }

    /* Loop for all volumes in the set */
    if (smg$enable) SMG$SET_CURSOR_MODE(&paste_id,&SMG$M_CURSOR_OFF);
    while ((curvol <= maxvol) && (!ctx.end))
    {
        ctx.i = -1;
        if (rvt[curvol].i_open ==1)
        {
            if (maxvol == 1)
            {
                sprintf(outbuf,"%%DFU-I-UNDEL, Start search on %.*s (%s)",
                        device_descr.dsc$w_length, device, &rvt[curvol].fulldevnam[1]);
                put_disp();
            }
            else
            {
                sprintf(outbuf,"%%DFU-I-UNDEL, Start search on %.*s (%s) , RVN = %d",
                        rvt[curvol].devnam_len, rvt[curvol].devnam,
                        &rvt[curvol].fulldevnam[1], curvol);
                put_disp();
            }
            headers=0;
            qchan = 0;

            /* Read in index file bitmap, and return free hdrs + highest bit set */
            read_indexf_bitmap(&free_hdr);

            status = get_next_header();
            if ((status & 1) != 1) return(status);
            while (! ctx.end) /* Loop until end of INDEXF or CTRL C entered */
            {
                hdr = ctx.hdr;
                id = ctx.id;
                rvn = curvol;
                if (hdr->seg_num !=0) goto next_und; /* Skip extension header */

                /* Check for a valid deleted file header */
                if ((header[ctx.i].block[510] != 0) || (header[ctx.i].block[511] != 0))
                    goto next_und; /* Checksum must be zero */
                if (hdr->fid_overlay.fid_fields.fid_num != 0) goto next_und;
                if ((hdr->filechar & FH2$M_MARKDEL) != FH2$M_MARKDEL)
                    goto next_und;
                if ((hdr->filechar & FH2$M_ERASE) == FH2$M_ERASE) /*Skip erased file*/
                    goto next_und;

                if (matuic == TRUE)
                    if (uic != hdr->fileowner_overlay.fileowner) goto next_und;
                /* get the file name and check if we requested this name */
                memcpy(&name[0],id->filename,20);
                if (name[19] != ' ') memcpy(&name[20],id->filenamext,66);
                if (strlen(name) == 0) goto next_und; /* Unused file header */
                name_descr.dsc$w_length = strindex(name," ",86);
                name[name_descr.dsc$w_length] = '\0';
                status = str$match_wild(&name_descr,&dummy_descr);
                if (status != STR$_MATCH) goto next_und;
                /* Check the BITMAP for multiple allocated blocks, if we
                    find some we just skip this file */
                found = TRUE;
                oldhdr = hdr;
                /* Reset fid_num and fid_nmx field */
                hdr->fid_overlay.fid_fields.fid_num = (headers % 65536);
                hdr->fid_overlay.fid_fields.fid_nmx = headers / 65536;
                while (found)
                {
                    status = rebuild_bitmap(hdr, rvt[rvn].bmap_addr[0],
                                            home[rvn].cluster, curvol, TRUE);
                    if ((status & 1) != 1) found = FALSE;
                    if (hdr->ext_fid_overlay.ext_fid[0] ==0) found = FALSE;
                    /* Read in extension header */
                    if (found)
                    {
                        x = 0;
                        rvn = hdr->ext_fid_overlay.ext_fid_fields.ext_fid_rvn;
                        if (rvn == 0) rvn = curvol;
                        fidnum = hdr->ext_fid_overlay.ext_fid_fields.ext_fid_nmx << 16;
                        fidnum += hdr->ext_fid_overlay.ext_fid[0];
                        ivbn = rvt[rvn].vbn_file_1 + fidnum - 1;
                        hdr = (struct header_area_struct *) &header[x];
                        status = SYS$QIOW(0,rvt[rvn].channel,IO$_READVBLK,
                                          &iostat[iocnt],0,0,&header[0],512,ivbn,0,0,0);
                        if ((status & 1) == 1) status = iostat[iocnt].iosb_1;
                        if ((status & 1) != 1) found = FALSE;
                        /* Check if this header is valid */
                        if (found)
                        {
                            if ((hdr->bk_fid_overlay.bk_fid_fields.bk_fid_num !=
                                    oldhdr->fid_overlay.fid_fields.fid_num) ||
                                    (hdr->bk_fid_overlay.bk_fid_fields.bk_fid_seq !=
                                     oldhdr->fid_overlay.fid_fields.fid_seq) ||
                                    (hdr->bk_fid_overlay.bk_fid_fields.bk_fid_nmx !=
                                     oldhdr->fid_overlay.fid_fields.fid_nmx))
                            {
                                found = FALSE;
                                status = 0;
                            }
                        }
                    }
                }
                if ((status & 1) != 1) goto next_und;
                hdr = oldhdr;
                rvn = curvol;

                /* get the full directory name */
                if (hdr->bk_fid_overlay.bk_fid_fields.bk_fid_num == 0 )
                    strcpy(dirrec.dirnam,"[]");
                else
                {
                    copy_fid(&fib.fib$w_fid[0],
                             &hdr->bk_fid_overlay.bk_fid_fields.bk_fid_num, TRUE);
                    fib.fib$w_did[0] = 0;
                    fib.fib$w_did[1] = 0;
                    fib.fib$w_did[2] = 0;
                    fib.fib$w_nmctl = 0;
                    fib.fib$l_acctl = 0;
                    status = SYS$QIOW(0,rvt[rvn].channel,IO$_ACCESS,
                                      &iostat[iocnt],0,0,&fibdescr,0,0,0,&acb,0);
                    status = iostat[iocnt].iosb_1;
                    if ((status & 1) != 1) strcpy(dirrec.dirnam,"[]");
                    else
                        /* Now rearrange the directory name found */
                    {
                        x = strindex(&dirrec.dirnam,"[",198);
                        strncpy(dirrec.dirnam,&dirrec.dirnam[x],(198-x));
                        tmp = (char *) strstr(dirrec.dirnam,".DIR;");
                        if (tmp != NULL)
                        {
                            *tmp = ']';
                            tmp = (char *) strstr(dirrec.dirnam,"]");
                            *tmp = '.';
                        }
                        else strcpy(dirrec.dirnam,"[]");
                        if (strstr(dirrec.dirnam,"[000000") != 0)
                        {
                            dirrec.dirnam[7] = '[';
                            strcpy(dirrec.dirnam,&dirrec.dirnam[7]);
                        }
                    }
                    x = strindex(&dirrec.dirnam,"]",198);
                    dirrec.dirnam[x+1] = '\0';
                }
                fcount++;
                sprintf(outbuf,"Recoverable file %s%s found ", dirrec.dirnam, name);
                put_disp();
                if (matoutput) fprintf(fp,"%s\n",outbuf);
                x = 4;
                ans[0] = 'n';
                if (!matlist)
                    if(!matnoconfirm)
                    {
                        if (smg$enable)
                        {
                            SMG$SET_CURSOR_MODE(&paste_id,&SMG$M_CURSOR_ON);
                            status = SMG$READ_COMPOSED_LINE(&keyb_id, 0, &answer,
                                                            &prompt, &k, &disp1_id, &modifiers, 0,0,0,0,0);
                            SMG$SET_CURSOR_MODE(&paste_id,&SMG$M_CURSOR_OFF);
                        }
                        else
                            status = SMG$READ_COMPOSED_LINE(&keyb_id, 0, &answer,
                                                            &prompt, &k, 0, &modifiers, 0,0,0,0,0);
                    }
                    else
                        strcpy(ans,"Y");
                if ((ans[0] == 'y') || (ans[0] == 'Y'))
                {
                    /* Actual recovery starts here as follows (loop for all extension headers) :
                            - Reenter retrieval pointers in BITMAP
                        - If oke then
                            - Rewrite all BITMAP.SYS
                            - Rewrite all Headers
                            - Rewrite index file bitmap bit. At this point recovery is oke
                            - Update QUOTA.SYS
                            - Setup entry for enter file routine
                        - After all undeletes are done ...
                            - Unlock volume
                            - Reenter files in directory

                    Save size and uic for possible quota processing */

                    a_size = hdr->hiblk_overlay.hiblk_fields.hiblkh << 16;
                    a_size += hdr->hiblk_overlay.hiblk_fields.hiblkl;
                    s_uic = hdr->fileowner_overlay.fileowner;
                    found = TRUE;
                    hdrs = 0;
                    x = i;
                    oldhdr = hdr;
                    /* Loop for all headers... */
                    while (found)
                    {
                        hdrs +=1;
                        /* Now rebuild the mapping pointers into the bitmap.
                           If any multiple allocated blocks are discovered we break
                           off the whole undelete process */
                        status = rebuild_bitmap(hdr, rvt[rvn].bmap_addr[0],
                                                home[rvn].cluster, curvol, FALSE);
                        if ((status & 1) != 1)
                        {
                            sprintf(outbuf,"%%DFU-E-NOTUNDEL, File cannot be undeleted");
                            put_disp();
                            ctx.end = TRUE;
                            curvol = maxvol;
                            goto next_und;
                        }
                        if (hdr->ext_fid_overlay.ext_fid[0] == 0) found = FALSE;
                        else
                            /* Read in extension header */
                        {
                            x = 0;
                            rvn = hdr->ext_fid_overlay.ext_fid_fields.ext_fid_rvn;
                            if (rvn == 0) rvn = curvol;
                            fidnum = hdr->ext_fid_overlay.ext_fid_fields.ext_fid_nmx << 16;
                            fidnum += hdr->ext_fid_overlay.ext_fid[0];
                            ivbn = rvt[rvn].vbn_file_1 + fidnum - 1;
                            hdr = (struct header_area_struct *) &header[x];
                            status = SYS$QIOW(0,rvt[rvn].channel,IO$_READVBLK,
                                              &iostat[iocnt],0,0,&header[0],512,ivbn,0,0,0);
                            if ((status & 1) == 1) status = iostat[iocnt].iosb_1;
                            if ((status & 1) != 1)
                            {
                                sprintf(outbuf,
                                        "%%DFU-E-READERR, Error reading extension header,");
                                put_disp();
                                singlemsg(0,status);
                                sprintf(outbuf,"%%DFU-E-NOTUNDEL, File cannot be undeleted");
                                put_disp();
                                ctx.end = TRUE;
                                curvol = maxvol;
                                goto next_und;
                            }
                            /* Check if this header is valid */
                            if ((hdr->bk_fid_overlay.bk_fid_fields.bk_fid_num !=
                                    oldhdr->fid_overlay.fid_fields.fid_num) ||
                                    (hdr->bk_fid_overlay.bk_fid_fields.bk_fid_seq !=
                                     oldhdr->fid_overlay.fid_fields.fid_seq) ||
                                    (hdr->bk_fid_overlay.bk_fid_fields.bk_fid_nmx !=
                                     oldhdr->fid_overlay.fid_fields.fid_nmx))
                            {
                                sprintf(outbuf,
                                        "%%DFU-E-BADEXTHDR, Extension header linkage broken");
                                put_disp();
                                sprintf(outbuf,"%%DFU-E-NOTUNDEL, File cannot be undeleted");
                                put_disp();
                                ctx.end = TRUE;
                                curvol = maxvol;
                                goto next_und;
                            }
                        }
                    } /* end while */
                    /* So we have rebuild the bitmap(s), now rewrite them */
                    for (j = 1; j <= maxvol; j++)
                    {
                        if (rvt[j].bchan != 0)
                        {
                            bytes = 512 * bmap[j].pagecnt;
                            status = SYS$QIOW(0,rvt[j].bchan, IO$_WRITEVBLK, &iostat[iocnt],
                                              0,0, rvt[j].bmap_addr[0], bytes, 2,0,0,0);
                            if ((status & 1) == 1) status = iostat[iocnt].iosb_1;
                            if ((status & 1) != 1)
                            {
                                sprintf(outbuf,"%%DFU-E-BITMAPERR, Error writing BITMAP.SYS,");
                                put_disp();
                                singlemsg(0,status);
                                sprintf(outbuf,
                                        "- Use ANALYZE/DISK/REPAIR to repair the volume");
                                put_disp();
                                ctx.end = TRUE;
                                curvol = maxvol;
                                goto next_und;
                            }
                        }
                    }
                    /* Now rewrite all the file headers */
                    /* Setup the RVN, FileChar and Checksum fields */
                    x = ctx.i;
                    rvn = curvol;
                    found = TRUE;
                    bytes = 512;
                    hdr = (struct header_area_struct *) &header[x];
                    fidnum = hdr->fid_overlay.fid_fields.fid_nmx << 16;
                    fidnum += hdr->fid_overlay.fid[0];
                    while(found)
                    {
                        hdr->fid_overlay.fid_fields.fid_rvn = 0;
                        hdr->filechar = hdr->filechar & ~(FH2$M_MARKDEL);
                        /* Check if we are dealing with a directory file */
                        if ( ((hdr->rattrib & FAT$M_NOSPAN) == FAT$M_NOSPAN) ||
                                (strstr(name , ".DIR;1") != 0) )
                        {
                            hdr->filechar = hdr->filechar | FH2$M_DIRECTORY;
                            hdr->filechar = hdr->filechar | FH2$M_CONTIG;
                        }
                        head = (struct _hd *) hdr;
                        head->block[255] = 0;
                        for (j=0; j<=254; j++) head->block[255] += head->block[j];
                        vbn = rvt[rvn].vbn_file_1 + fidnum - 1;
                        status = SYS$QIOW(0, rvt[rvn].channel, IO$_WRITEVBLK,
                                          &iostat[iocnt], 0, 0, &header[x], bytes, vbn, 0, 0, 0);
                        if ((status & 1) == 1) status = iostat[iocnt].iosb_1;
                        if ((status & 1) != 1)
                        {
                            sprintf(outbuf,"%%DFU-E-INDEXFERR, Error writing INDEXF.SYS,");
                            put_disp();
                            singlemsg(0,status);
                            sprintf(outbuf,"- Use ANALYZE/DISK/REPAIR to repair the volume");
                            put_disp();
                            ctx.end = TRUE;
                            curvol = maxvol;
                            goto next_und;
                        }
                        if (hdr->ext_fid_overlay.ext_fid[0] == 0) found = FALSE;
                        else
                            /* Read in extension header */
                        {
                            x = 0;
                            rvn = hdr->ext_fid_overlay.ext_fid_fields.ext_fid_rvn;
                            if (rvn == 0) rvn = curvol;
                            fidnum = hdr->ext_fid_overlay.ext_fid_fields.ext_fid_nmx << 16;
                            fidnum += hdr->ext_fid_overlay.ext_fid[0];
                            ivbn = rvt[rvn].vbn_file_1 + fidnum - 1;
                            hdr = (struct header_area_struct *) &header[x];
                            status = SYS$QIOW(0,rvt[rvn].channel,IO$_READVBLK,
                                              &iostat[iocnt],0,0,&header[0],512,ivbn,0,0,0);
                            hdr->fid_overlay.fid_fields.fid_num = fidnum % 65536;
                            hdr->fid_overlay.fid_fields.fid_nmx = fidnum / 65536;
                        }
                    } /* End while */
                    hdr = (struct header_area_struct *) &header[ctx.i];
                    sprintf(outbuf,"%%DFU-S-RECOVER, File succesfully recovered");
                    put_disp();
                    /* Rewrite the indexf bitmap bit */
                    bitblk = (headers + 4095) / 4096 - 1;
                    bitje = (headers - 1 - bitblk*4096) / 32;
                    bitval = (headers - 1) % 32;
                    j = 1;
                    lib$insv(&j, &bitval, &j, &bitmap[bitblk+1].block[bitje*4]);
                    bytes = 512;
                    vbn = home[curvol].ibmapvbn + bitblk;
                    status = SYS$QIOW(0, rvt[curvol].channel, IO$_WRITEVBLK,
                                      &iostat[iocnt],0,0,&bitmap[bitblk+1],bytes,vbn,0,0,0);
                    /* Add quota if needed */
                    strcpy(device, rvt[1].devnam);
                    device_descr.dsc$w_length = rvt[1].devnam_len;
                    if (qchan == 0)
                    {
                        status = SYS$ASSIGN(&device_descr, &qchan, 0, 0);
                        if ((status & 1) != 1)
                            singlemsg(DFU_ASSIGN,status);
                    }
                    if (qchan != 0) add_quota(qchan, s_uic, hdrs, a_size);
                    sprintf(outbuf," ");
                    put_disp();
                    /* Set up work entry for entering file in directory */
                    if (hdr->bk_fid_overlay.bk_fid_fields.bk_fid_num == 0)
                    {
                        sprintf(outbuf,
                                "%%DFU-E-INVBAKFID, Invalid backlink, file cannot be entered in directory");
                        put_disp();
                    }
                    else
                    {
                        rvn = hdr->bk_fid_overlay.bk_fid_fields.bk_fid_rvn;
                        if (rvn ==0) rvn = curvol;
                        if (first == NULL )
                        {
                            list = (struct work *) malloc(sizeof(struct work));
                            first = list; /* pointer to first element */
                        }
                        else
                        {
                            list->next = (struct work *) malloc(sizeof(struct work));
                            list = list->next;
                        }
                        /* File in Directory and File id */
                        list->rvn = rvn;
                        copy_fid(&list->did_num,
                                 &hdr->bk_fid_overlay.bk_fid_fields.bk_fid_num, FALSE);
                        /* Special case for MFD */
                        if (headers == 4)
                            list->did_rvn = 0;
                        else
                            list->did_rvn = hdr->bk_fid_overlay.bk_fid_fields.bk_fid_rvn;
                        copy_fid(&list->fid_num, &hdr->fid_overlay.fid_fields.fid_num,
                                 FALSE);
                        list->fid_rvn = curvol;
                        list->next = NULL;
                        strcpy(list->name,name);
                    }
                }
                /* Next header */
next_und:
                status = get_next_header();
                if ((status & 1) != 1) return(status);
            }
        }
        if (ctrlc == 1)
        {
            ctx.end = TRUE;
            do_abort();
        }
        else
        {
            curvol++;
            sprintf(outbuf,"     Progress : 100%%     Status : READY");
            put_status(1);
            ctx.end = FALSE; /* Next volume */
            if (clean_flags.events == 1)
            {
                for (i=1; i <= iocnt ; i++) status = lib$free_ef(&efn[i-1]);
                clean_flags.events = 0;
            }
            if (curvol <= maxvol)
            {
                sprintf(outbuf," ");
                put_disp();
            }
        }
    }

    /* Now unlock the volume */
    if (!matlist)
    {
        dfu_handler();
        SYS$CANEXH(&desblk);
    }
    if (qchan !=0) SYS$DASSGN(qchan);

    /* Now enter files in directories */
    fib.fib$w_nmctl = FIB$M_NEWVER;
    if (first != NULL)
    {
        sprintf(outbuf,"%%DFU-I-ENTER, Entering file(s) in directory...");
        put_disp();
    }
    for (i=1; i <=maxvol ; i++)
        status = SYS$QIOW(0,rvt[i].channel,IO$_DEACCESS,&iostat[iocnt],
                          0,0,0,0,0,0,0,0);
    while (first != NULL)
    {
        list = first;
        rvn = list->rvn;
        copy_fid(&fib.fib$w_did[0], &list->did_num, FALSE);
        copy_fid(&fib.fib$w_fid[0], &list->fid_num, FALSE);
        strcpy(name,list->name);
        name_descr.dsc$w_length = strlen(name);
        first = list->next;
        free(list); /*Return dynamic space to pool */
        status = SYS$QIOW(0, rvt[rvn].channel, IO$_CREATE, &iostat[ctx.thread],
                          0,0, &fibdescr, &name_descr , 0, 0, 0, 0);
        if ((status & 1) == 1) status = iostat[ctx.thread].iosb_1;
        if ((status & 1) != 1)
        {
            if (syslost == 0)
                syslost = make_syslost(&lost_fid);
            if (syslost == 1) /* Try entering the file in [SYSLOST] */
            {
                copy_fid(&fib.fib$w_did[0], &lost_fid.fid_num, FALSE);
                status = SYS$QIOW(0, rvt[1].channel, IO$_CREATE, &iostat[ctx.thread],
                                  0,0, &fibdescr, &name_descr , 0, 0, 0, 0);
                if ((status & 1) == 1) status = iostat[ctx.thread].iosb_1;
                if ((status & 1) == 1)
                {
                    sprintf(outbuf,"%%DFU-S-ENTERED, File %s entered in [SYSLOST]",name);
                    put_disp();
                }
            }
            if (syslost == 2)
            {
                sprintf(outbuf,"%%DFU-E-NOTENTER, File %s not entered in directory,",name);
                put_disp();
                singlemsg(0,status);
                sprintf(outbuf,
                        "- Use ANALYZE/DISK/REPAIR to move the file to [SYSLOST]");
                put_disp();
            }
        }
        else
        {
            sprintf(outbuf,"%%DFU-S-ENTERED, File %s entered in original directory",
                    name);
            put_disp();
        }
    }

    curvol = 1;
    cleanup();
    if (matlist)
    {
        sprintf(outbuf,"\n%%DFU-S-FND , %d recoverable files found",fcount);
        put_disp();
    }
    sprintf(outbuf,"\n%%DFU-I-READY, UNDELETE command ready");
    put_disp();
    if (matstat == TRUE) status = lib$show_timer(0,0,display_stat,0);
    return(1);
}

int make_syslost(struct f_id *l_fid)
/*
    Routine to create the syslost directory on RVN 1
    Returns 1 on succes, 2 on failure
*/

{
    char fname[14] = "SYSLOST.DIR;1";
    struct dsc$descriptor device_descr;
    struct dsc$descriptor fname_descr;
    int func;
    short empty = -1;
    static unsigned int uchar ,uic;
    static short attrib[16], fpro, l_chan;
    static struct fibdef lost_fib;
    static struct
    {
        unsigned short iosb_1;
        unsigned int length;
        short spec;
    } io_stat;
    static struct
    {
        unsigned short atr_size, atr_type;
        int *atr_address;
    }
    acb[5] =  { { 4, ATR$C_UCHAR, (int *) &uchar} ,
        { 32, ATR$C_RECATTR, (int *) &attrib},
        { 2, ATR$C_FPRO, (int *) &fpro},
        {4, ATR$C_UIC, (int *) &uic},
        {0, 0, 0}
    };
    static struct
    {
        int fiblen;
        struct fibdef *fibadr;
    } fibdescr;

    fname_descr.dsc$w_length = strlen(fname);
    fname_descr.dsc$a_pointer = (char *) &fname;
    device_descr.dsc$a_pointer = (char *) &rvt[1].devnam;
    device_descr.dsc$w_length = rvt[1].devnam_len;
    status = SYS$ASSIGN(&device_descr, &l_chan, 0, 0);
    /* Setup fib. First we access the MFD to get the correct attributes,
       next we create SYSLOST */
    if ((status & 1) != 1) return(2);
    fibdescr.fiblen = sizeof(lost_fib);
    fibdescr.fibadr = &lost_fib;
    lost_fib.fib$w_fid[0] = 4;
    lost_fib.fib$w_fid[1] = 4;
    lost_fib.fib$w_fid[2] = 1;
    lost_fib.fib$w_did[0] = 0;
    lost_fib.fib$w_did[1] = 0;
    lost_fib.fib$w_did[2] = 0;
    status = SYS$QIOW(0, l_chan, IO$_ACCESS, &io_stat,
                      0,0, &fibdescr, 0 , 0, 0, &acb, 0);
    if ((status & 1) == 1 ) status = io_stat.iosb_1;
    if ((status & 1) != 1 )
    {
        status = SYS$DASSGN(l_chan);
        return(2);
    }
    else
    {
        attrib[2] = attrib[3] = attrib[4] = 0;
        attrib[5] = 2 ; /* Efblk and Hiblk */
        lost_fib.fib$w_fid[0] = 0;
        lost_fib.fib$w_fid[1] = 0;
        lost_fib.fib$w_fid[2] = 0;
        lost_fib.fib$w_did[0] = 4;
        lost_fib.fib$w_did[1] = 4;
        lost_fib.fib$w_did[2] = 1;
        lost_fib.fib$l_acctl = FIB$M_WRITE | FIB$M_NOWRITE;
        lost_fib.fib$w_exctl =
            FIB$M_EXTEND | FIB$M_ALCON | FIB$M_FILCON;
        lost_fib.fib$l_exsz = 1;
        lost_fib.fib$w_nmctl = 0;
        /* Go on and create this directory */
        func = IO$_ACCESS | IO$M_CREATE | IO$M_ACCESS;
        status = SYS$QIOW(0, l_chan, func, &io_stat,
                          0,0, &fibdescr, &fname_descr , 0, 0, &acb, 0);
        if ((status & 1) == 1 ) status = io_stat.iosb_1;
        if ((status & 1) != 1 )
        {
            sprintf(outbuf,"%%DFU-E-MKLOST, Error creating/accessing SYSLOST.DIR,");
            put_disp();
            singlemsg(0,status);
            status = SYS$DASSGN(l_chan);
            return(2);
        }
        else if (status == SS$_CREATED)
        {
            /* empty the first directory block to fake a new directory */
            status = SYS$QIOW(0, l_chan, IO$_WRITEVBLK, &io_stat,
                              0,0, &empty, 2, 1 , 0, 0, 0);
        }
    }
    SYS$DASSGN(l_chan);
    copy_fid(&l_fid->fid_num, &lost_fib.fib$w_fid[0], FALSE);
    return(1);
}

int verify_command(int mask)
/*
    Verify disk (like ANALYZE/DISK) and report errors.
    /FIX fixes some basic errors (like ANA/DISK/REPAI)
*/

{
    static char dummy_item[80], fname[80], name[86];
    struct header_area_struct *hdr;
    struct ident_area_struct *id;
    Boolean dummy, matoutput, matstat, matlock, bitset, matfix,
            matreb, trigger;
    register int i;
    int ivbn, bitblk, bitje, bitval, find, rvn, x, y,
        k, size, free_hdr, a_size, r_size, hdrs, namelen, rtvptr;
    unsigned int page_cnt, bakfid, syslost, bytes;
    struct _da
    {
        int bakfid;
        char rvn, bitje;
    } *dyn_array;
    struct
    {
        int own_uic, flag, rsize, asize, hdr;
    } usage_table[750];
    struct f_id lost_fid;
    static struct fibdef fib;   /* File information block */
    struct
    {
        int fiblen;
        struct fibdef *fibadr;
    } fibdescr;
    /* Set up list for mutiple allocated blocks */
    struct mult
    {
        unsigned int lbnstart,lbnend;
        struct mult *next;
    } *m_first, *m_list;
    Boolean multalloc;
    void get_map_pointers(),
         add_usage(), dfu_handler(), new_bitmap(),
         check_usage(), report_lost_files();
    /* FAO parameters */
    unsigned short outlen;
    $DESCRIPTOR(out_descr , outbuf);
    $DESCRIPTOR(device_descr , device);
    $DESCRIPTOR(file_descr , fname);
    $DESCRIPTOR(dummy_descr , dummy_item);
    $DESCRIPTOR(name_descr , name);

    /* Code starts here */
    /* Parse the various qualifiers */

    progress_ind = TRUE;
    y = 0;
#if 0
    status = parse_item("fix", &dummy_descr, 0, &matfix, 2);
    status = parse_item("rebuild", &dummy_descr, 0, &matreb, 2);
#else
    matfix=0;
    matreb=0;
#endif
    /* Check the privileges */
    if ( ((matfix) || (matreb)) && (mask > -1))
    {
        singlemsg(0, DFU_NOPRIV);
        return(SS$_NOPRIV);
    }
    /* Get device name */
    status = parse_item("device", &device_descr, 0, &dummy , 0);
    if (status == 1)
    {
        if (strindex(&device,":",64) == -1)
        {
            device[device_descr.dsc$w_length]=':';
            device_descr.dsc$w_length += 1;
        }
    }
#if 0
    status = parse_item("statistics", &dummy_descr, 0, &matstat, 2);
    status = parse_item("lock", &dummy_descr, 0, &matlock, 2);
#else
    matstat=0;
    matlock=0;
#endif
    if (matreb) matlock = TRUE; /* Lock anyway if rebuild requested */
    for (i=0; i < 750; i++) usage_table[i].flag = 0;
    /* Get output file */
    clean_flags.fopen = 0;
#if 0
    status = parse_item("outfile", &file_descr, 0, &matoutput , 0);
#else
    matoutput=0;
#endif
    if (matoutput == FALSE)
    {
        strcpy(fname,"SYS$OUTPUT:");
        file_descr.dsc$w_length = 11;
    }
    else
    {
        fname[file_descr.dsc$w_length] = '\0';
    }
#if 0
    fp = fopen(fname,"w","mrs=255","rfm=var","ctx=rec","rat=cr","rop=WBH");
#else
    fp = stdout;
#endif
    clean_flags.fopen = 1;

    /* Fill in FIB */
    fibdescr.fiblen = sizeof(fib);
    fibdescr.fibadr = &fib;
    syslost = 0;

    /* All qualifiers parsed */
    first = NULL;
    list = NULL;
    if (matstat == TRUE) status = lib$init_timer(0);
    clean_flags.channels = 0;
    clean_flags.sort = 0;
    clean_flags.events = 0;
    clean_flags.expreg = 0;

    /* Open the volume (set) , read in the home block */

    i = (matlock == TRUE) ? 1 : 0;
    status = open_device(&device_descr,i);
    if ((status & 1) != 1) return(status);
    if ((matlock) && (rvt[1].wlk == TRUE))
    {
        sprintf(outbuf,
                "%%DFU-W-WRITELK, Cannot rebuild or lock on write-locked device");
        put_disp();
        matlock = FALSE;
        matreb = FALSE;
    }
    if ((matlock == TRUE) && (lock_chan != 0))
    {
        /* Setup exit handler to ensure that we unlock the volume */
        desblk.handler_addr = (int) &dfu_handler;
        desblk.condition = (int) &status;
        desblk.arg_count = 1;
        status = SYS$DCLEXH(&desblk);
        if ((status & 1) !=1)
        {
            singlemsg(DFU_EXHFAIL,status);
            cleanup();
            return(status);
        }
    }

    curvol = 1;
    size = 1;
    ctx.end = FALSE;
    if (smg$enable) SMG$SET_CURSOR_MODE(&paste_id,&SMG$M_CURSOR_OFF);
    multalloc = FALSE;
    while ((curvol <= maxvol) && (!ctx.end))
    {
        ctx.i = -1;
        trigger = FALSE;
        if (rvt[curvol].i_open ==1)
        {
            if (maxvol == 1)
            {
                sprintf(outbuf,"%%DFU-I-VERIFY, Verifying %.*s (%s) ",
                        device_descr.dsc$w_length, device, &rvt[curvol].fulldevnam[1]);
            }
            else
            {
                sprintf(outbuf,"%%DFU-I-VERIFY, Verifying %.*s (%s) , RVN = %d",
                        rvt[curvol].devnam_len, rvt[curvol].devnam,
                        &rvt[curvol].fulldevnam[1], curvol);
            }
            put_disp();
            headers=0;
            m_list = (struct mult *) malloc (sizeof (struct mult));
            m_first = m_list;
            m_list->next = NULL;
            /* Phase 1 : Read and parse INDEXF.SYS */
            /* Create dynamic array. each entry = 6 bytes so Index file size /6
                + 1 must be enough. Also create space for new bitmap */

            page_cnt = 1 + (rvt[curvol].if_size*6)/512;
            rvt[curvol].addr[0] = 0;
            rvt[curvol].bmap_addr[0] = 0;
            status = SYS$EXPREG(page_cnt, &rvt[curvol].addr[0],0,0);
            if ((status &1) != 1)
            {
                singlemsg(DFU_EXPREG,status);
                cleanup();
                return(status);
            }
            clean_flags.expreg = 1;
            dyn_array = (struct _da *) rvt[curvol].addr[0];
            page_cnt = (rvt[curvol].maxblocks/home[curvol].cluster);
            page_cnt = (page_cnt + 4095 ) / 4096;
            status = SYS$EXPREG(page_cnt, &rvt[curvol].bmap_addr[0],0,0);
            if ((status &1) != 1)
            {
                singlemsg(DFU_EXPREG,status);
                cleanup();
                return(status);
            }

            /* Report home block info */
            if ((home[curvol].altidxlbn !=0) &&
                    (home[curvol].altidxvbn != 0) &&
                    (home[curvol].cluster != 0) &&
                    (home[curvol].homevbn != 0) &&
                    (home[curvol].ibmapvbn != 0) &&
                    (home[curvol].ibmaplbn != 0) &&
                    (home[curvol].maxfiles != 0) &&
                    (home[curvol].ibmapsize != 0) &&
                    (home[curvol].resfiles != 0) )
                sprintf(outbuf,"%%DFU-S-CHKHOME, Home block info verified OK");
            else
                sprintf(outbuf,"%%DFU-E-ERRHOME, Home block info not OK");
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);

            /* Read in index file bitmap, and return free hdrs + highest bit set */
            read_indexf_bitmap(&free_hdr);

            status = get_next_header();
            if ((status & 1) != 1) return(status);
            while (! ctx.end) /* Loop until end of INDEXF or CTRL C entered */
            {
                hdr = ctx.hdr;
                id = ctx.id;
                /* Setup fields in dynamic array */
                bakfid = hdr->bk_fid_overlay.bk_fid_fields.bk_fid_nmx << 16;
                bakfid += hdr->bk_fid_overlay.bk_fid_fields.bk_fid_num;
                (dyn_array+headers)->bitje = 0;
                (dyn_array+headers)->bakfid = bakfid;

                /* Check corresponding bitmap bit (must be set) */
                bitblk = (headers + 4095) / 4096 - 1;
                bitje = (headers - 1 - bitblk*4096) / 32;
                bitval = (headers - 1) % 32;
                status = lib$ffs(&bitval,&size,&bitmap[bitblk+1].block[bitje*4],
                                 &find);
                if (status == LIB$_NOTFOU)
                    bitset = FALSE;
                else bitset = TRUE;
                rvn = curvol;
                status = verify_header(hdr);
                if (status != SS$_NORMAL)
                {
                    if (bitset == TRUE)
                    {
                        sprintf(outbuf,"%%DFU-W-NOBITCLR, file (%d,%d,%d) deleted file header marked BUSY in Index File bitmap"
                                ,headers,hdr->fid_overlay.fid_fields.fid_seq,curvol);
                        put_disp();
                        if (matoutput) fprintf(fp,"%s\n",outbuf);
                        /* Clear the indexf bitmap bit */
                        if (matreb)
                        {
                            trigger = TRUE;
                            lib$insv(&y, &bitval, &size, &bitmap[bitblk+1].block[bitje*4]);
                        }
                    }
                    goto next_ver ; /* Not a valid header */
                }

                /* We have a valid header, proceed...*/
                /* Set up new bitmap and report multiple allocated blocks */
                new_bitmap(rvt[curvol].bmap_addr[0], hdr, home[curvol].cluster,
                           curvol, fp, 1, &m_list, &multalloc, matoutput);
                m_list->next = NULL;
                if (hdr->seg_num !=0) goto next_ver ; /* Skip extension header */
                /*Set lost file bit */
                (dyn_array+headers)->bitje = (dyn_array+headers)->bitje | 1;
                /* Get size fields (NOTE : they are stored in reverse order ! */
                a_size = hdr->hiblk_overlay.hiblk_fields.hiblkh << 16;
                a_size += hdr->hiblk_overlay.hiblk_fields.hiblkl;
                r_size = hdr->efblk_overlay.efblk_fields.efblkh << 16;
                r_size += hdr->efblk_overlay.efblk_fields.efblkl;
                if (r_size > 0)
                    if (hdr->ffbyte == 0) r_size--; /* Correct size on block boundary*/
                /* Get name */
                memcpy(&name[0],id->filename,20);
                if (name[19] != ' ') memcpy(&name[20],id->filenamext,66);
                namelen = strindex(&name[0]," ",86);

                /* Check marked for delete bit */
                if ((hdr->filechar & FH2$M_MARKDEL) == FH2$M_MARKDEL)
                {
                    (dyn_array+headers)->bitje = 0;
                    sprintf(outbuf,
                            "%%DFU-W-DELETED, file (%d,%d,%d) %.*s marked for delete"
                            ,headers,hdr->fid_overlay.fid_fields.fid_seq,curvol
                            ,namelen,name);
                    put_disp();
                    if (matoutput) fprintf(fp,"%s\n",outbuf);
                    /* If /FIX entered we enter this file in the work list */
                    if (matfix)
                    {
                        if (first == NULL )
                        {
                            list = (struct work *) malloc(sizeof(struct work));
                            first = list; /* pointer to first element */
                        }
                        else
                        {
                            list->next = (struct work *) malloc(sizeof(struct work));
                            list = list->next;
                        }
                        /* Fill in File id */
                        copy_fid(&list->fid_num, &hdr->fid_overlay.fid_fields.fid_num,
                                 FALSE);
                        list->fid_rvn = 1;
                        list->function = 1;
                        list->next = NULL;
                    }
                    goto add_usage; /* Skip other tests */
                }

                /* Check locked bit */
                if ((hdr->filechar & FH2$M_LOCKED) == FH2$M_LOCKED)
                {
                    sprintf(outbuf,
                            "%%DFU-W-LOCKED, file (%d,%d,%d) %.*s is deaccess locked "
                            ,headers,hdr->fid_overlay.fid_fields.fid_seq,curvol,namelen,name);
                    put_disp();
                    if (matoutput) fprintf(fp,"%s\n",outbuf);
                }

                /* Check badblock bit */
                if ((hdr->filechar & FH2$M_BADBLOCK) == FH2$M_BADBLOCK)
                {
                    sprintf(outbuf,
                            "%%DFU-W-BADBLOCK, file (%d,%d,%d) %.*s has suspected bad blocks"
                            ,headers,hdr->fid_overlay.fid_fields.fid_seq,curvol,namelen,name);
                    put_disp();
                    if (matoutput) fprintf(fp,"%s\n",outbuf);
                }

                /* Check directory bit */
                if ((hdr->filechar & FH2$M_DIRECTORY) == FH2$M_DIRECTORY)
                {
                    (dyn_array+headers)->bitje = (dyn_array+headers)->bitje | 4;
                    if (maxvol > 1)
                    {
                        y = hdr->bk_fid_overlay.bk_fid_fields.bk_fid_rvn;
                        if ((bakfid == 4) && (y > 1))
                        {
                            sprintf(outbuf,"%%DFU-W-BADMFDLNK, directory %.*s has backlink to 000000.DIR on RVN %d"
                                    , namelen,name, y);
                            put_disp();
                            if (matoutput) fprintf(fp,"%s\n",outbuf);
                        }
                        y = 0;
                    }
                }

                /* Check corresponding bit in bitmap */
                if (bitset == FALSE)
                {
                    sprintf(outbuf,"%%DFU-W-NOBITSET, file (%d,%d,%d) %.*s Index File bitmap bit not set"
                            ,headers,hdr->fid_overlay.fid_fields.fid_seq,curvol,namelen,name);
                    put_disp();
                    if (matoutput) fprintf(fp,"%s\n",outbuf);
                    if (matreb)
                    {
                        y = 1; /*NOw set this bit in the indexf bitmap */
                        lib$insv(&y, &bitval, &size, &bitmap[bitblk+1].block[bitje*4]);
                        y = 0;
                        trigger = TRUE;
                    }
                }

                /* Check file owner */
                if (hdr->fileowner_overlay.fileowner == 0)
                {
                    sprintf(outbuf,"%%DFU-W-NOOWNER, file (%d,%d,%d) %.*s has no owner"
                            ,headers,hdr->fid_overlay.fid_fields.fid_seq,curvol,namelen,name);
                    put_disp();
                    if (matoutput) fprintf(fp,"%s\n",outbuf);
                }

                /* Check backlink ; special case for the MFD */
                if ( (bakfid == 0) || ((headers == 4) && (bakfid != 4)) )
                {
                    sprintf(outbuf,
                            "%%DFU-E-INVBAKFID, file (%d,%d,%d) %.*s has invalid backlink"
                            ,headers,hdr->fid_overlay.fid_fields.fid_seq,curvol,namelen,name);
                    put_disp();
                    if (matoutput) fprintf(fp,"%s\n",outbuf);
                    /* Set invalid backlink bit */
                    if (bakfid ==0)
                        (dyn_array+headers)->bitje = (dyn_array+headers)->bitje | 2;
                    if (matfix)
                    {
                        if (first == NULL )
                        {
                            list = (struct work *) malloc(sizeof(struct work));
                            first = list; /* pointer to first element */
                        }
                        else
                        {
                            list->next = (struct work *) malloc(sizeof(struct work));
                            list = list->next;
                        }
                        /* Fill in File id */
                        copy_fid(&list->fid_num,&hdr->fid_overlay.fid_fields.fid_num
                                 , FALSE);
                        list->fid_rvn = curvol;
                        list->function = 2;
                        list->next = NULL;
                        strncpy(list->name,name,namelen);
                    }
                }
                if ((bakfid == headers) && (headers !=4)) /* File backlinks to itself */
                    /* need to check volume set*/
                    if (hdr->fid_overlay.fid_fields.fid_rvn ==
                            hdr->bk_fid_overlay.bk_fid_fields.bk_fid_rvn)
                    {
                        sprintf(outbuf,
                                "%%DFU-E-SLFBAKFID, file (%d,%d,%d) %.*s backlink points to itself"
                                ,headers,hdr->fid_overlay.fid_fields.fid_seq,curvol,namelen,name);
                        put_disp();
                        if (matoutput) fprintf(fp,"%s\n",outbuf);
                        if (matfix)
                        {
                            if (first == NULL )
                            {
                                list = (struct work *) malloc(sizeof(struct work));
                                first = list; /* pointer to first element */
                            }
                            else
                            {
                                list->next = (struct work *) malloc(sizeof(struct work));
                                list = list->next;
                            }
                            /* Fill in File id */
                            copy_fid(&list->fid_num,
                                     &hdr->fid_overlay.fid_fields.fid_num, FALSE);
                            list->fid_rvn = curvol;
                            list->function = 4;
                            list->next = NULL;
                            strncpy(list->name,name,namelen);
                        }
                    }
                /* If backlink points to other disk we must save this RVN */
                if (hdr->bk_fid_overlay.bk_fid_fields.bk_fid_rvn == 0)
                    hdr->bk_fid_overlay.bk_fid_fields.bk_fid_rvn = curvol;
                if (hdr->bk_fid_overlay.bk_fid_fields.bk_fid_rvn != curvol)
                {
                    (dyn_array+headers)->rvn =
                        hdr->bk_fid_overlay.bk_fid_fields.bk_fid_rvn;
                    (dyn_array+headers)->bitje = (dyn_array+headers)->bitje | 8;
                }
add_usage:
                hdrs = 1;
                rtvptr = 0;
                if (hdr->ext_fid_overlay.ext_fid[0] !=0)
                    status = follow_extents(hdr, &rtvptr, &hdrs);
                /* Update usage table */
                if (headers > 3) /* Skip first 3 reserved files */
                    add_usage(&usage_table, hdr->fileowner_overlay.fileowner,
                              r_size, a_size, hdrs);
                /* Next header */
next_ver :
                status = get_next_header();
                if ((status & 1) != 1) return(status);
            }
        }
        /* Save array bounds */
        dyn_array->bakfid = headers;

        /* Phase 1A If we have multiple allocated blocks we will make
           a new (empty) bitmap with the bits set for the blocks reported
           in the dynamic array m_list. Thus, a second pass thru INDEXF.SYS
           and the routine new_bitmap will report ALL files involved. */
        if (multalloc)
        {
            /* Recreate the new bitmap */
            sprintf(outbuf,"%%DFU-W-MULTFND, reporting multiple allocated blocks...");
            put_disp();
            if (matoutput) fprintf(fp,"%s\n",outbuf);
            status = SYS$DELTVA(&rvt[curvol].bmap_addr[0],
                                &rvt[curvol].bmap_addr[0], 0);
            rvt[curvol].bmap_addr[0] = 0;
            page_cnt = (rvt[curvol].maxblocks/home[curvol].cluster);
            page_cnt = (page_cnt + 4095 ) / 4096;
            status = SYS$EXPREG(page_cnt, &rvt[curvol].bmap_addr[0],0,0);
            if ((status &1) != 1)
            {
                singlemsg(DFU_EXPREG,status);
                cleanup();
                return(status);
            }
            /* Now fill in the multiple allocated blocks */
            while (m_first->next != NULL)
            {
                m_list = m_first;
                set_bitmap(rvt[curvol].bmap_addr[0], home[curvol].cluster,
                           m_list->lbnstart, m_list->lbnend, TRUE);
                m_first = m_list->next;
                free(m_list);
            }
            /* Now rescan INDEXF.SYS */

            ctx.end = FALSE;
            ctx.i = -1;
            status = get_next_header();
            if ((status & 1) != 1) return(status);
            while (! ctx.end) /* Loop until end of INDEXF or CTRL C entered */
            {
                hdr = ctx.hdr;
                rvn = curvol;
                status = verify_header(hdr);
                if (status != SS$_NORMAL) goto next_ver2 ;

                /* We have a valid header, proceed...*/
                /* Set up new bitmap and report multiple allocated blocks */
                new_bitmap(rvt[curvol].bmap_addr[0], hdr, home[curvol].cluster,
                           curvol, fp, 2, NULL, &multalloc, matoutput);
next_ver2 :
                status = get_next_header();
                if ((status & 1) != 1) return(status);
            }
        } /*end multalloc */
        free(m_first);
        multalloc = FALSE;

        if (ctrlc != 1)
            if ((matreb) && (trigger))
                /* Rewrite the indexf bitmap at this point */
            {
                bytes = 512 * home[curvol].ibmapsize;
                status = SYS$QIOW(0,rvt[curvol].channel,IO$_WRITEVBLK,&iostat[0],
                                  0,0,&bitmap[1],bytes,home[curvol].ibmapvbn,0,0,0);
                if ((status & 1) == 1) status = iostat[0].iosb_1;
                if ((status & 1) == 1)
                {
                    sprintf(outbuf,"%%DFU-S-REBIFMAP, Indexfile bitmap rebuild");
                    put_disp();
                    if (matoutput) fprintf(fp,"%s\n",outbuf);
                }
                else
                {
                    sprintf(outbuf,"%%DFU-E-REBIFMAP, Indexf Bitmap rebuild failed,");
                    put_disp();
                    singlemsg(0,status);
                }
            }

        if (ctrlc !=1)
        {
            /* Phase 2: Now continue with bitmap processing */
            sprintf(outbuf,"     Progress : 100%%     Status : READY");
            put_status(1);
            strcpy(device, rvt[curvol].devnam);
            device_descr.dsc$w_length = rvt[curvol].devnam_len;
            rvt[curvol].bchan = 0;
            status = SYS$ASSIGN(&device_descr, &rvt[curvol].bchan, 0, 0);
            if ((status & 1) != 1)
            {
                singlemsg(DFU_ASSIGN,status);
                cleanup();
                return(status);
            }
            sprintf(outbuf,"     Progress :   0%%     Status : Processing BITMAP.SYS");
            put_status(1);
            page_cnt = (rvt[curvol].maxblocks / home[curvol].cluster) ;
            /* Check for incorrect blocks in BITMAP.SYS and rebuild on request */
            status = compare_bitmap(rvt[curvol].bchan, rvt[curvol].bmap_addr[0],
                                    page_cnt, rvt[curvol].wlk, home[curvol].cluster,
                                    fp, matreb, matoutput);
            sprintf(outbuf,"     Progress : 100%%     Status : READY");
            put_status(1);
            if ((status &1) !=1)
            {
                cleanup();
                return(status);
            }
            /* Free the new bitmap space, we don't need it any more */
            status = SYS$DELTVA(&rvt[curvol].bmap_addr[0],
                                &rvt[curvol].bmap_addr[0], 0);
            rvt[curvol].bmap_addr[0] = 0;
        }
        if (ctrlc == 1)
        {
            ctx.end = TRUE;
        }
        else
        {
            curvol++;
            ctx.end = FALSE; /* Next volume */
            if (clean_flags.events == 1)
            {
                for (i=1; i <= iocnt ; i++) status = lib$free_ef(&efn[i-1]);
                clean_flags.events = 0;
            }
            if (curvol <= maxvol)
            {
                sprintf(outbuf," ");
                put_disp();
            }
        }
    }
    curvol = 1;
    /* Phase 3 : check lost files */
    if (ctrlc != 1) report_lost_files(matfix, matoutput);
    /* Phase 4 : check diskquota */
    if (ctrlc != 1)
    {
        sprintf(outbuf,"     Progress :   0%%     Status : Processing QUOTA.SYS");
        put_status(1);
        check_usage(&usage_table, rvt[curvol].bchan, fp, matreb, matoutput);
        sprintf(outbuf,"     Progress : 100%%     Status : READY");
        put_status(1);
    }

    /* Now unlock the volume */
    if (matlock)
    {
        dfu_handler();
        SYS$CANEXH(&desblk);
    }

    /* Phase 5 : fix errors */
    if (matfix && (first != NULL))
    {
        for (i=1; i <=maxvol ; i++)
            status = SYS$QIOW(0,rvt[i].channel,IO$_DEACCESS,&iostat[iocnt],
                              0,0,0,0,0,0,0,0);
        if (!ctrlc)
        {
            sprintf(outbuf,"%%DFU-I-REPAIR, Fixing errors...");
            put_disp();
        }
        fib.fib$w_nmctl = FIB$M_NEWVER;
        while (first != NULL)
        {
            list = first;
            if (!ctrlc)
            {
                if (list->function == 1) /* Delete file */
                    status = delete_file(&list->fid_num, rvt[1].channel,
                                         TRUE, FALSE, FALSE, FALSE);
                if (list->function == 4) /* Remove file entry, then enter file*/
                {
                    status = delete_file(&list->fid_num, rvt[1].channel,
                                         TRUE, FALSE, TRUE, TRUE);
                    list->function = 2;
                }
                if (list->function == 3) /* Remove file entry*/
                    status = delete_file(&list->fid_num, rvt[1].channel,
                                         TRUE, FALSE, TRUE, TRUE);
                if (list->function == 2) /* Enter file in SYSLOST */
                {
                    if (syslost == 0) /* Create the [SYSLOST] if needed */
                    {
                        syslost = make_syslost(&lost_fid);
                        copy_fid(&fib.fib$w_did[0], &lost_fid.fid_num, FALSE);
                    }
                    if (syslost == 1)
                    {
                        copy_fid(&fib.fib$w_fid[0], &list->fid_num, FALSE);
                        strcpy(name,list->name);
                        name_descr.dsc$w_length = strlen(name);
                        status = SYS$QIOW(0, rvt[1].channel, IO$_CREATE,
                                          &iostat[ctx.thread],0,0, &fibdescr, &name_descr , 0, 0, 0, 0);
                        if ((status & 1) == 1) status = iostat[ctx.thread].iosb_1;
                        if ((status & 1) == 1)
                        {
                            sprintf(outbuf,
                                    "%%DFU-S-ENTERED, File %s entered in [SYSLOST]",name);
                            put_disp();
                        }
                    }
                }
            }
            first = list->next;
            free(list); /*Return dynamic space to pool */
        }
    }
    if (ctrlc) do_abort();

    cleanup();
    sprintf(outbuf,"\n%%DFU-I-READY, VERIFY command ready");
    put_disp();
    if (matstat == TRUE) status = lib$show_timer(0,0,display_stat,0);
    return(1);
}

void report_lost_files(Boolean matfix, Boolean matoutput)
/* The dynamic tables build up in Phase 1 of verify will be used
   to check for lost files. If the backlink = 0 or points to an
   invalid or vanished directory we have a lost file. In case of a
   volume set we must use the RVN field in the dynamic array to
   check the backlink on another disk */

{
    struct _da
    {
        int bakfid;
        char rvn, bitje;
    } *dyn_array, *temp;
    struct header_area_struct *hdr, *hdr1;
    struct ident_area_struct *id;
    register int i;
    int j, k, maxi, rvn, xrvn, status, namelen, dirlen;
    char name[86], dirname[86];

    sprintf(outbuf,"%%DFU-I-CHKLOST, Checking for lost files...");
    put_disp();
    if (matoutput) fprintf(fp,"%s\n",outbuf);
    hdr = (struct header_area_struct *) &header[0];
    hdr1 = (struct header_area_struct *) &header[1];
    for (j = 1; j <= maxvol; j++)
    {
        dyn_array = (struct _da *) rvt[j].addr[0]; /* Pointer to correct table */
        maxi = dyn_array->bakfid; /* We saved to highest found header here */
        for (i = 1; i<=maxi; i++)
        {
            /* Lost file bit must be set*/
            if (( (dyn_array+i)->bitje & 1) != 1) goto skip;
            /* Invalid backlink already reported */
            if (( (dyn_array+i)->bitje & 2) == 2) goto skip;
            /* Check if backlink on other volume */
            k = (dyn_array+i)->bakfid;
            if (( (dyn_array+i)->bitje & 8) == 8)
            {
                xrvn = (dyn_array+i)->rvn; /* get rvn */
                temp = (struct _da *) rvt[xrvn].addr[0];  /* point to correct table */
                temp = (temp+k); /* Get backlink record */
            }
            else
            {
                xrvn = j;
                temp = (dyn_array+k); /*Get backlink record */
            }
            /* Temp now points to the file backlink file header (parent directory).
               If this file is not valid we have a lost header */
            /* case 1 : Lost file bit clear. Probably this directory is deleted*/
            namelen = -1;
            if ( (temp->bitje & 1) != 1)
            {
                vbn = i - 1 + rvt[j].vbn_file_1; /*Read this header */
                status = SYS$QIOW(0,rvt[j].channel,IO$_READVBLK,
                                  &iostat[0],0,0,&header[0],512,vbn,0,0,0);
                id = (struct ident_area_struct *)
                     &header[0].block[2*(hdr->idoffset)];  /* Ident area */
                memcpy(&name[0], id->filename,20);
                if (name[19] != ' ') memcpy(&name[20], id->filenamext,66);
                namelen = strindex(&name[0]," ", 86);
                sprintf(outbuf,
                        "%%DFU-W-LOSTHDR1, file (%d,%d,%d) %.*s found in nonexistent directory"
                        ,i ,hdr->fid_overlay.fid_fields.fid_seq,j,namelen,name);
                put_disp();
                if (matoutput) fprintf(fp,"%s\n",outbuf);
            }
            else
                /* case 2 and 3 : directory has invalid backlink, or directory bit not set */

            {
                if ( ((temp->bitje & 2) == 2) || ((temp->bitje & 4) != 4) )
                {
                    vbn = i - 1 + rvt[j].vbn_file_1; /*Read this header */
                    status = SYS$QIOW(0,rvt[j].channel,IO$_READVBLK,
                                      &iostat[0],0,0,&header[0],512,vbn,0,0,0);
                    id = (struct ident_area_struct *)
                         &header[0].block[2*(hdr->idoffset)];  /* Ident area */
                    memcpy(&name[0], id->filename,20);
                    if (name[19] != ' ') memcpy(&name[20], id->filenamext,66);
                    namelen = strindex(&name[0]," ", 86);
                    vbn = k - 1 + rvt[xrvn].vbn_file_1; /*Read backlink header */
                    status = SYS$QIOW(0,rvt[xrvn].channel,IO$_READVBLK,
                                      &iostat[0],0,0,&header[1],512,vbn,0,0,0);
                    id = (struct ident_area_struct *)
                         &header[1].block[2*(hdr1->idoffset)];  /* Ident area */
                    memcpy(&dirname[0], id->filename,20);
                    if (dirname[19] != ' ') memcpy(&dirname[20], id->filenamext,66);
                    dirlen = strindex(&dirname[0]," ", 86);
                    if ((temp->bitje & 2) == 2)
                        sprintf(outbuf,"%%DFU-W-LOSTHDR2, file (%d,%d,%d) %.*s in directory with bad backlink (%d,%d,%d) %.*s"
                                ,i ,hdr->fid_overlay.fid_fields.fid_seq,j,namelen,name,
                                k ,hdr1->fid_overlay.fid_fields.fid_seq,xrvn,dirlen,dirname);
                    else
                        sprintf(outbuf,"%%DFU-W-LOSTHDR3, file (%d,%d,%d) %.*s found in invalid directory (%d,%d,%d) %.*s"
                                ,i ,hdr->fid_overlay.fid_fields.fid_seq,j,namelen,name,
                                k ,hdr1->fid_overlay.fid_fields.fid_seq,xrvn,dirlen,dirname);
                    put_disp();
                    if (matoutput) fprintf(fp,"%s\n",outbuf);
                }
            }
            if (matfix && ( namelen != -1))
            {
                if (first == NULL )
                {
                    list = (struct work *) malloc(sizeof(struct work));
                    first = list; /* pointer to first element */
                }
                else
                {
                    list->next = (struct work *) malloc(sizeof(struct work));
                    list = list->next;
                }
                /* Fill in File id ;  First we remove the old entry
                    next we enter the file in syslost */
                list->fid_rvn = 1;
                list->fid_num = i % 65536;
                list->fid_seq = hdr->fid_overlay.fid_fields.fid_seq;
                list->fid_nmx = i / 65536;
                list->function = 3;
                list->next = NULL;
                list->next = (struct work *) malloc(sizeof(struct work));
                list = list->next;

                list->fid_rvn = j;
                list->fid_num = i % 65536;
                list->fid_seq = hdr->fid_overlay.fid_fields.fid_seq;
                list->fid_nmx = i / 65536;
                list->function = 2;
                list->next = NULL;
                strncpy(list->name,name,namelen);
            }
skip: ; /* Next header */
            if (ctrlc ==1) i = maxi; /* Dumb method to break off the report */
        } /* End i loop */
        if (ctrlc == 1) j = maxvol;
    } /* End j loop */
}

int build_dir_table(char *dev_str, Boolean matoutput)
/*
    Build directory table needed for DIR/VERSION. DIR/CHECK, DIR/EMPTY
    and DIR/ALIAS
    V2.2 : Also save the file size for DIR/VERSION
*/

{
    static char dummy_item[80], name[86];
    struct header_area_struct *hdr;
    struct ident_area_struct *id;
    Boolean dummy, bitset;
    register int i;
    int ivbn, bitblk, bitje, bitval, find, rvn, x, maxdir,
        k, size, free_hdr, a_size, r_size, hdrs, namelen, rtvptr;
    unsigned int page_cnt, dir_cnt, bakfid;
    struct _da
    {
        short fid[3];
        unsigned int bakfid, a_size;
        char rvn, bitje;
    } *dyn_array;
    void get_map_pointers(), dfu_handler();
    /* FAO parameters */
    unsigned short outlen;
    $DESCRIPTOR(out_descr , outbuf);
    $DESCRIPTOR(device_descr , device);
    $DESCRIPTOR(dummy_descr , dummy_item);

    /* Code starts here */
    /* Parse the various qualifiers */

    /* Get device name */
    strcpy(device, dev_str);
    device_descr.dsc$w_length = strlen(device);
    if (strindex(&device,":",64) == -1)
    {
        device[device_descr.dsc$w_length]=':';
        device_descr.dsc$w_length += 1;
    }

    progress_ind = TRUE;
    /* All qualifiers parsed */
    clean_flags.fopen = 0;
    clean_flags.sort = 0;
    clean_flags.channels = 0;
    clean_flags.events = 0;
    clean_flags.expreg = 0;
    dir_cnt = 0;

    /* Open the volume (set) , read in the home block */

    status = open_device(&device_descr,0);
    if ((status & 1) != 1) return(status);

    curvol = 1;
    size = 1;
    ctx.end = FALSE;
    if (smg$enable) SMG$SET_CURSOR_MODE(&paste_id,&SMG$M_CURSOR_OFF);
    while ((curvol <= maxvol) && (!ctx.end))
    {
        ctx.i = -1;
        if (rvt[curvol].i_open ==1)
        {
            if (maxvol == 1)
            {
                sprintf(outbuf,"%%DFU-I-INDSCAN, Making directory table for %.*s (%s) ",
                        device_descr.dsc$w_length, device, &rvt[curvol].fulldevnam[1]);
            }
            else
            {
                sprintf(outbuf,"%%DFU-I-INDSCAN, Making directory table for %.*s (%s) , RVN = %d",
                        rvt[curvol].devnam_len, rvt[curvol].devnam,
                        &rvt[curvol].fulldevnam[1], curvol);
            }
            put_disp();
            headers=0;
            /* Phase 1 : Read and parse INDEXF.SYS */
            /* Create dynamic array. each entry = 16 bytes. */

            page_cnt = 1 + (rvt[curvol].if_size*20)/512;
            rvt[curvol].addr[0] = 0;
            rvt[curvol].bmap_addr[0] = 0;
            status = SYS$EXPREG(page_cnt, &rvt[curvol].addr[0],0,0);
            if ((status &1) != 1)
            {
                singlemsg(DFU_EXPREG,status);
                cleanup();
                return(status);
            }
            clean_flags.expreg = 1;
            dyn_array = (struct _da *) rvt[curvol].addr[0]; /* Setup pointer to array */

            /* Read in index file bitmap, and return free hdrs + highest bit set */
            read_indexf_bitmap(&free_hdr);

            status = get_next_header();
            if ((status & 1) != 1) return(status);
            while (! ctx.end) /* Loop until end of INDEXF or CTRL C entered */
            {
                hdr = ctx.hdr;
                id = ctx.id;
                status = verify_header(hdr);
                if (status != SS$_NORMAL) goto next_file;
                /* Setup fields in dynamic array */
                (dyn_array+headers)->fid[0] = hdr->fid_overlay.fid[0];
                (dyn_array+headers)->fid[1] = hdr->fid_overlay.fid[1];
                (dyn_array+headers)->fid[2] = hdr->fid_overlay.fid[2];
                bakfid = hdr->bk_fid_overlay.bk_fid_fields.bk_fid_nmx << 16;
                bakfid += hdr->bk_fid_overlay.bk_fid_fields.bk_fid_num;
                (dyn_array+headers)->bakfid = bakfid;
                (dyn_array+headers)->a_size = 0;
                a_size = hdr->hiblk_overlay.hiblk_fields.hiblkh << 16;
                a_size += hdr->hiblk_overlay.hiblk_fields.hiblkl;
                (dyn_array+headers)->a_size = a_size;

                /* We have a valid header, proceed...*/
                if (hdr->seg_num !=0) goto next_file; /* Skip extension header */

                /* Check marked for delete bit */
                if ((hdr->filechar & FH2$M_MARKDEL) == FH2$M_MARKDEL) goto next_file;

                /* Use bit 1 as a valid file indicator */
                (dyn_array+headers)->bitje = (dyn_array+headers)->bitje | 1;

                /* Check if this is a directory and if yes set bit 4 */
                if ((hdr->filechar & FH2$M_DIRECTORY) == FH2$M_DIRECTORY)
                {
                    if ((hdr->filechar & FH2$M_CONTIG) != FH2$M_CONTIG) goto next_file;
                    if ((hdr->rattrib & FAT$M_NOSPAN) != FAT$M_NOSPAN) goto next_file;
                    memcpy(&name[0],id->filename,20);
                    if (name[19] != ' ') memcpy(&name[20],id->filenamext,66);
                    if (strstr(name,".DIR;1") == 0) goto next_file;
                    (dyn_array+headers)->bitje = (dyn_array+headers)->bitje | 4;
                    dir_cnt++;
                }
                /* Next header */
next_file:
                status = get_next_header();
                if ((status & 1) != 1) return(status);
            }
        }
        /* Save array bounds */
        dyn_array->fid[0] = headers % 65536;
        dyn_array->fid[1] = headers / 65536;
        if (ctrlc == 1)
        {
            ctx.end = TRUE;
        }
        else
        {
            curvol++;
            ctx.end = FALSE; /* Next volume */
            if (clean_flags.events == 1)
            {
                for (i=1; i <= iocnt ; i++) status = lib$free_ef(&efn[i-1]);
                clean_flags.events = 0;
            }
            if (curvol <= maxvol)
            {
                sprintf(outbuf," ");
                put_disp();
            }
        }
    }
    curvol = 1;
    /* Now scan all the directories */
    sprintf(outbuf,"     Progress : 100%%     Status : READY");
    put_status(1);
    if (ctrlc !=1)
    {
        sprintf(outbuf,"%%DFU-I-DIRSCAN, Scanning %d directories...",
                dir_cnt);
        put_disp();
    }
    /* First close the channels as we need them in SCAN_DIRECTORIES */
    for (i = 1; i <= maxvol; i++)
        status = SYS$QIOW(0,rvt[i].channel,IO$_DEACCESS,&iostat[iocnt],
                          0,0,0,0,0,0,0,0);
    /* Now scan the directories */
    maxdir = dir_cnt;
    dir_cnt = 0;
    for (i = 1; i <= maxvol; i++)
        if (ctrlc !=1)
            dir_cnt += scan_directories(&rvt[0] , i, dir_cnt, maxdir, matoutput);
    sprintf(outbuf,"     Progress : 100%%     Status : READY");
    put_status(1);

    sprintf(outbuf," ");
    put_disp();
    if (ctrlc == 1) do_abort();
    sprintf(outbuf,"%%DFU-S-DONE, Directories scanned : %d",dir_cnt);
    put_disp();
    cleanup();
    return(1);
}


int parse_item(char *inp, struct dsc$descriptor *return_descr,
               int *convert, Boolean *flag, int typ)
/* Check presence of parameter  in command line
Outputs :
  result string and length
  flag set to true or false
  value converted to integer in *convert if typ indicates integer
*/

{
    int stat, i, j, x, *y;
    static char item[80], uic_s[30], uic_x[30];
    struct _flist *tmp;
    struct _quad *tmpdat;
    char date[23];
    $DESCRIPTOR(item_descr , item);
    $DESCRIPTOR(uic_descr , uic_s);

    strcpy(item,inp);
    item_descr.dsc$w_length = strlen(item);
    *flag = FALSE;  /* Assume item not present */

    stat = CLI$PRESENT(&item_descr);
    if ((stat == CLI$_PRESENT ) || (stat == CLI$_NEGATED))
    {
        *flag = TRUE;
        switch(typ)
        {
        case 0 : /* Get the string */
            stat = CLI$GET_VALUE(&item_descr,return_descr,
                                 &return_descr->dsc$w_length);
            if ((stat & 1) != 1) return(stat);
            break;
        case 1 : /* Convert to integer */
            *convert = 0;
            return_descr->dsc$w_length = 80;
            stat = CLI$GET_VALUE(&item_descr,return_descr,
                                 &return_descr->dsc$w_length);
            if ((stat & 1) != 1) return(stat);
            *convert = atoi(return_descr->dsc$a_pointer);
            if (*convert < 0)
            {
                sprintf(outbuf,"%%DFU-E-PARSEERR, Value must be a positive integer");
                put_disp();
                *flag = FALSE;
                return(0);
            }
            break;
        case 2 : /* Only check the presence */
            break;
        case 3 : /* List processing for /file and /exclude */
            tmp = (struct _flist *)convert;
            /* convert used as address to list */
            y = (int *) return_descr->dsc$a_pointer; /* list counter */
            *y = 0;
            return_descr->dsc$a_pointer = (char *) &tmp->name;
            stat = CLI$GET_VALUE(&item_descr, return_descr, &i);
            tmp->flen = i;
            while (stat != CLI$_ABSENT)
            {
                x = strindex(tmp->name,";",86);
                if (x == -1)
                {
                    x = strindex(tmp->name," ",86);
                    memset(&tmp->name[x],'\0',1);
                    strcat(tmp->name,";*");
                    tmp->flen += 2;
                }
                else if (tmp->name[x+1] == ' ')
                {
                    memset(&tmp->name[x+1],'*',1) ;
                    tmp->flen += 1;
                }
                *y = *y + 1;
                tmp++; /* next item*/
                return_descr->dsc$a_pointer = (char *) &tmp->name;
                stat = CLI$GET_VALUE(&item_descr, return_descr, &i);
                tmp->flen = i;
            }
            return_descr->dsc$a_pointer = (char *) y;
            break;
        case 4: /* Date and time options */
            return_descr->dsc$a_pointer = (char *) &date;
            return_descr->dsc$w_length = 23;
            tmpdat = (struct _quad *) convert;
            if (stat == CLI$_NEGATED)
            {
                tmpdat->date[0] = 0;
                tmpdat->date[1] = 0;
            }
            else
            {
                stat = CLI$GET_VALUE(&item_descr,return_descr,
                                     &return_descr->dsc$w_length);
                if ((stat & 1) != 1) return(stat);
                stat = SYS$BINTIM(return_descr, &tmpdat->date[0]);
            }
            break;
        case 5 : /* Bit set options */
            y = (int *) return_descr->dsc$a_pointer; /* Characteristics */
            *y = *y | *convert;
            break;
        case 6 : /* UIC parsing */
            j = 0;
            stat = CLI$GET_VALUE(&item_descr, &uic_descr,
                                 &uic_descr.dsc$w_length);
            uic_x[0] = '\0';
            x = strindex(&uic_s,",",30);
            if (x == -1)
            {
                sprintf(outbuf,"%%DFU-E-UICERR, Invalid UIC entered");
                put_disp();
                return(0);
            }
            strncat(uic_x,&uic_s[1],--x);
            sscanf(uic_x,"%o",&j);
            *convert = j << 16;
            uic_x[0] = '\0';
            i = strindex(&uic_s,"]",30);
            if (i == -1)
            {
                sprintf(outbuf,"%%DFU-E-UICERR, Invalid UIC entered");
                put_disp();
                return(0);
            }
            x += 2;
            i = i - x;
            strncat(uic_x,&uic_s[x],i);
            sscanf(uic_x,"%o",&j);
            *convert += j;
            if (*convert == 0)
            {
                sprintf(outbuf,"%%DFU-E-UICERR, Invalid UIC entered");
                put_disp();
                return(0);
            }
            break;
        case 7 : /* Check if negated */
            *convert = 0;
            if (stat == CLI$_NEGATED) *convert = -1;
            break;
        }
        return(1);
    }
    else return(1);
}

void fid_to_name(char * ret_dir)
/*
   Get the filename from the file-id or the file header
   Implicit input : ctx block
   V2.2 : fixed volume set problem
*/
{
    Boolean error;
    struct header_area_struct *hdr;
    struct ident_area_struct *id;
    static struct fibdef fib;   /* File information block */
    struct
    {
        int fiblen;
        struct fibdef *fibadr;
    } fibdescr;
    int x, bakfid, rrvn;
    char name[86], *tmp;

    /* Fill in FIB */
    fibdescr.fiblen = sizeof(fib);
    fibdescr.fibadr = &fib;

    error = FALSE;
    hdr = ctx.hdr;
    bakfid = hdr->bk_fid_overlay.bk_fid_fields.bk_fid_nmx << 16;
    bakfid += hdr->bk_fid_overlay.bk_fid_fields.bk_fid_num;
    rrvn = hdr->bk_fid_overlay.bk_fid_fields.bk_fid_rvn;
    if (rrvn == 0) rrvn = curvol;
    if (bakfid == 0 )
        /* invalid backlink */
        error = TRUE;
    else /* Get the full file name via an ACP call */
    {
        copy_fid(&fib.fib$w_fid[0], &hdr->fid_overlay.fid_fields.fid_num,
                 TRUE);
        fib.fib$w_did[0] = 0;
        fib.fib$w_did[1] = 0;
        fib.fib$w_did[2] = 0;
        fib.fib$w_nmctl = 0;
        fib.fib$l_acctl = 0;
        status = SYS$QIOW(0,rvt[curvol].channel,IO$_ACCESS,
                          &iostat[iocnt],0,0,&fibdescr,0,0,0,&acb,0);
        if ((status & 1) == 1) status = iostat[iocnt].iosb_1;
        if ((status & 1) !=1)
        {
            error = TRUE;
            /*     singlemsg(0,status); */
        }
        else
        {
            x = strindex(&dirrec.dirnam,"[",198);
            /* Make sure we use the correct device name on voluem sets */
            if (bakfid == 4)
                strcpy(ret_dir,&rvt[rrvn].fulldevnam[1]);
            else strcpy(ret_dir,&rvt[1].fulldevnam[1]);
            if (x != -1) strcat(ret_dir,&dirrec.dirnam[x]);
        }
    }
    if (error)
    {
        id = ctx.id;
        memcpy(&name[0],id->filename,20);
        if (name[19] != ' ') memcpy(&name[20],id->filenamext,66);
        tmp = (char *) strstr(&name," ");
        if (tmp != NULL) *tmp = 0;
        strcpy(ret_dir,"[]");
        strcat(ret_dir,name);
        strcpy(dirrec.dirnam,ret_dir);
        dirrec.lendir = strindex(ret_dir," ",200);
    }
}

int open_device(struct dsc$descriptor *device_descr, int flag)
/* Open the device or volume set.
   Outputs :
    All fields in RVT (relative volume table)
    Flags : 0 -> process volume set
        1 -> process volume set and lock the volumes on LOCK_CHAN
*/

{
    register int i;
    static struct fibdef if_fib;    /* File information block */
    struct
    {
        int fiblen;
        struct fibdef *fibadr;
    } fibdescr;
    struct _it3 item_list[9];
    int bytes, stat, devclass, devchar, func_code, efblk, volnum, devtype;
    struct header_area_struct *hdr_area;

    /* Fill in FIB */
    fibdescr.fiblen = 10; /* Short FIB */
    fibdescr.fibadr = &if_fib;
    if_fib.fib$l_acctl = FIB$M_WRITE | FIB$M_NOLOCK;
    if_fib.fib$w_fid[0] = 1;
    if_fib.fib$w_fid[1] = 1;
    if_fib.fib$w_fid[2] = 0;
    maxvol = 0;
    curvol = 1;
    strcpy(rvt[1].devnam," ");

    /* Set up itemlist for GETDVI */
    add_item(&item_list[0].buflen, 64, DVI$_ROOTDEVNAM,
             &rvt[1].devnam, &rvt[1].devnam_len);
    add_item(&item_list[1].buflen,4, DVI$_VOLCOUNT, &volcount, 0);
    add_item(&item_list[2].buflen,4, DVI$_VOLNUMBER, &orig_rvn, 0);
    add_item(&item_list[3].buflen,4, DVI$_DEVCLASS, &devclass, 0);
    add_item(&item_list[4].buflen,4, DVI$_DEVCHAR, &devchar, 0);
    add_item(&item_list[5].buflen,4, DVI$_MAXBLOCK, &rvt[1].maxblocks, 0);
    add_item(&item_list[6].buflen,4, DVI$_DEVTYPE, &devtype, 0);
    add_item(&item_list[7].buflen,64,DVI$_FULLDEVNAM,
             &rvt[1].fulldevnam, &rvt[1].fulldevnam_len);
    add_item(&item_list[8].buflen,0,0,0,0);

    stat = SYS$GETDVIW(0,0,device_descr,&item_list,&iostat[0],0,0,0);
    if ((stat & 1) == 1) stat = iostat[0].iosb_1;
    if ((stat & 1) != 1)
    {
        singlemsg(0,stat);
        return(stat);
    }
    if (devclass != DC$_DISK)
    {
        sprintf(outbuf,"%%DFU-E-NODISK, Device is not a disk device");
        put_disp();
        return(SS$_NOSUCHDEV);
    }
    if (volcount > 32)
    {
        sprintf(outbuf,"%%DFU-F-TOOMANYVOL, DFU does not allow > 32 volumes");
        put_disp();
        return(SS$_NOSUCHDEV);
    }

    /* So we have a valid device, proceed... */

    if ((devtype == 34) || (devtype == 53) || (devtype ==72))
        rvt[1].slow = TRUE;
    else rvt[1].slow = FALSE; /* Decide if we have a CD ROM */
    if ((devchar & DEV$M_SWL) == DEV$M_SWL) /* We have a write locked device */
    {
        if_fib.fib$l_acctl = FIB$M_NOLOCK;
        rvt[1].wlk = TRUE;
    }
    else rvt[1].wlk = FALSE;
    maxvol=volcount;
    if (volcount > 1)  /* Use root device name in case of volume set */
    {
        strcpy(device_descr->dsc$a_pointer , rvt[1].devnam);
        device_descr->dsc$w_length = rvt[1].devnam_len;
    }
    else /* Reset original name entered on command line */
    {
        strncpy(rvt[1].devnam , device_descr->dsc$a_pointer,
                device_descr->dsc$w_length);
        rvt[1].devnam_len = device_descr->dsc$w_length;
    }

    /* Make sure we have the correct maxblocks for the root device name */
    if ((volcount > 1) && (orig_rvn != 1))
    {
        add_item(&item_list[0].buflen, 4, DVI$_MAXBLOCK, &rvt[1].maxblocks, 0);
        add_item(&item_list[1].buflen, 64,DVI$_FULLDEVNAM,
                 &rvt[1].fulldevnam, &rvt[1].fulldevnam_len);
        add_item(&item_list[2].buflen,0,0,0,0);

        stat = SYS$GETDVIW(0,0,device_descr,&item_list,&iostat[0],0,0,0);
        if ((stat & 1) == 1) stat = iostat[0].iosb_1;
        if ((stat & 1) != 1)
        {
            singlemsg(0,stat);
            return(stat);
        }
    }

    /* Lock volume if requested (flag=1) */
    lock_chan = 0;
    if ((flag ==1) && (rvt[1].wlk == FALSE))
    {
        stat = SYS$ASSIGN(device_descr, &lock_chan,0,0);
        if ((stat & 1) != 1)
        {
            singlemsg(DFU_ASSIGN,stat);
            cleanup();
            return(stat);
        }
        if_fib.fib$w_exctl = FIB$C_LOCK_VOL;
        fibdescr.fiblen = sizeof(if_fib);
        stat = SYS$QIOW(0,lock_chan,IO$_ACPCONTROL,&iostat[0],
                        0,0,&fibdescr,0,0,0,0,0);
        if_fib.fib$w_exctl = 0;
        fibdescr.fiblen = 10; /* Short FIB */
        if ((stat & 1) == 1) stat = iostat[0].iosb_1;
        if ((stat & 1) != 1)
        {
            sprintf(outbuf,"%%DFU-E-LOCKERR, Error locking volume,");
            put_disp();
            singlemsg(0,stat);
            cleanup();
            return(stat);
        }
        sprintf(outbuf,"%%DFU-W-LOCKED, Volume now write locked");
        put_disp();
    }

    /* Now loop for all members in volume set */
    func_code = IO$_ACCESS | IO$M_ACCESS;
    i=1;
    if (maxvol ==0) maxvol = 1;
    while (i <= maxvol)
    {
        rvt[i].i_open = 0; /* First assign a channel for this volume */
        stat = SYS$ASSIGN(device_descr,&rvt[i].channel,0,0);
        if ((stat & 1) != 1)
        {
            rvt[i].maxblocks=0;
            singlemsg(DFU_ASSIGN,stat);
            cleanup();
            return(stat);
        }
        clean_flags.channels = 1;
        rvt[i].i_open = 1;
        rvt[i].vbn_file_1 = 0;
        if (flag !=2) if_fib.fib$w_fid[2] = i; /* Fill in RVN */
        else if_fib.fib$w_fid[2] = orig_rvn;

        /* Open the file on ACP level */
        stat = SYS$QIOW(0,rvt[i].channel,func_code,
                        &iostat[0],0,0,&fibdescr,0,0,0,0,0);
        if ((stat & 1) == 1) stat = iostat[0].iosb_1;
        if ((stat & 1) != 1)
        {
            singlemsg(DFU_INDEXERR,stat);
            cleanup();
            return(stat);
        }

        /* Read in the home block (VBN 2) */
        stat = SYS$QIOW(0,rvt[i].channel,IO$_READVBLK,
                        &iostat[0],0,0,&home[i],512,2,0,0,0);
        if ((stat & 1) == 1) stat = iostat[0].iosb_1;
        if ((stat & 1) != 1)
        {
            singlemsg(0,stat);
            cleanup();
            return(stat);
        }
        if (home[i].struclev_overlay.strucver_fields.struclev != 2)
        {
            /* Not an ODS2 disk */
            sprintf(outbuf,"%%DFU-NOTODS2, Device is not an ODS2 disk");
            put_disp();
            cleanup();
            return(1);
        }
        /* read in Indexf.Sys header, we need the file size */
        rvt[i].vbn_file_1 = home[i].ibmapvbn + home[i].ibmapsize;
        stat = SYS$QIOW(0,rvt[i].channel, IO$_READVBLK,
                        &iostat[0],0,0,block,512,rvt[i].vbn_file_1,0,0,0);
        if ((stat & 1) == 1) stat = iostat[0].iosb_1;
        if ((stat & 1) != 1)
        {
            singlemsg(0,stat);
            cleanup();
            return(stat);
        }
        hdr_area = (struct header_area_struct *) &block[0];
        efblk = hdr_area->efblk_overlay.efblk_fields.efblkh << 16;
        efblk += hdr_area->efblk_overlay.efblk_fields.efblkl;
        rvt[i].if_size = efblk - rvt[i].vbn_file_1;
        i++;
        if (i <= maxvol)    /*Next volume in set */
        {
            add_item(&item_list[0].buflen, 64, DVI$_NEXTDEVNAM,
                     &rvt[0].devnam, &rvt[0].devnam_len);
            add_item(&item_list[1].buflen, 0,0,0,0);
            stat = SYS$GETDVIW(0,0,device_descr,&item_list,&iostat[0],0,0,0);
            device_descr->dsc$w_length = rvt[0].devnam_len;
            strcpy(device_descr->dsc$a_pointer , rvt[0].devnam);

            /* Get actual volume number and maximum blocks */
            add_item(&item_list[0].buflen, 4, DVI$_VOLNUMBER, &volnum, 0);
            add_item(&item_list[1].buflen, 4, DVI$_MAXBLOCK, &rvt[0].maxblocks, 0);
            add_item(&item_list[2].buflen, 4, DVI$_DEVCHAR, &devchar, 0);
            add_item(&item_list[3].buflen,64, DVI$_FULLDEVNAM,
                     &rvt[i].fulldevnam, &rvt[i].fulldevnam_len);
            add_item(&item_list[4].buflen,0,0,0,0);

            stat = SYS$GETDVIW(0,0,device_descr,&item_list,&iostat[0],0,0,0);
            if ((stat & 1) == 1) stat = iostat[0].iosb_1;
            if ((stat & 1) != 1)
            {
                sprintf(outbuf,"%%DFU-E-NOVOLSET, One ore more volume set members missing");
                put_disp();
                cleanup();
                return(stat);
            }
            i=volnum;
            rvt[i].maxblocks = rvt[0].maxblocks;
            if ((devchar & DEV$M_SWL) == DEV$M_SWL)
            {
                rvt[i].wlk = TRUE;
            }
            else
            {
                rvt[i].wlk = FALSE;
            }
            rvt[i].devnam_len = rvt[0].devnam_len;
            strncpy(rvt[i].devnam , rvt[0].devnam, rvt[0].devnam_len);
        }
    }

    return(1);
}

void read_indexf_bitmap(int *free_hdr)
/*
   Read in Indexf.Sys Bitmap.
   Return: Free Headers in this bitmap
           Highest bit set in this bitmap. This functions as
       a kind of logical EOF for the INDEXF.SYS.
*/
{
    register int i, j;
    char size;
    int k, endi, bitval, find, bytes;
    unsigned long int stat;
    Boolean found;

    bytes = 512 * home[curvol].ibmapsize;
    stat = SYS$QIOW(0,rvt[curvol].channel,IO$_READVBLK,&iostat[0],
                    0,0,&bitmap[1],bytes,home[curvol].ibmapvbn,0,0,0);
    endi = 1+rvt[curvol].if_size/4096; /* Maximum # of blocks used in bitmap */
    *free_hdr = 0;
    for (i=1; i <= endi; i++) /* All blocks */
        for (j=1; j <= 128; j++) /* All longwords in block i */
        {
            bitval=0;
            size=32;
            while (bitval < 32)    /* Search for clear bits in this longword */
            {
                stat = lib$ffc(&bitval,&size,&bitmap[i].block[(j-1)*4],&find);
                if (stat != LIB$_NOTFOU)
                {
                    *free_hdr = *free_hdr + 1;
                    bitval = find + 1;
                    size = 32 - bitval;
                }
                else
                    bitval = 32; /* No free bits found */
            }
        }

    /* Now subtract all bits in the bitmap which cannot be used
       because INDEXF.SYS is not large enough */

    *free_hdr = *free_hdr - (endi*4096 - rvt[curvol].if_size);

    /* We calculate the highest bit set in the bitmap. This is an
       artificial way to detect the highest used file header in
       INDEXF.SYS so we can limit the search time */
    found = FALSE;
    i = endi; /* Search backwards ! */
    j = 512;
    highbit = 0;
    size = 1;
    while (!found)
        if (bitmap[i].block[j-1] == 0)
        {
            j--;
            if (j==0)
            {
                i--;    /* Next Block */
                j = 512;
            }
            if (i==0) found = TRUE; /* End of bitmap */
        }
        else
        {
            k = 31; /* Scan longword backwards */
            while (!found)
                if (lib$extzv(&k,&size,&bitmap[i].block[j-1]) == 1)
                {
                    found = TRUE;
                }
                else
                {
                    k--;
                    if (k==0) found = TRUE;
                }
        }
    highbit = (i-1)*4096 + (j-1)*8 + k + 1;
    iosize = (highbit < 1500) ? (highbit/3) : 500;
    if (rvt[1].slow) iosize = 125;
}

int read_indexf_multi()
/* Read IOSIZE file headers with IOCNT threads.
   Return VBN where next READ IO should resume
*/
{
    int i,stat;

    vbn = rvt[curvol].vbn_file_1; /* First file header is here */
    for (i=1; i<=iocnt; i++)        /* Fire off 2 QIO's */
    {
        stat = lib$get_ef(&efn[i-1]);
        if ((stat & 1) != 1)
        {
            sprintf(outbuf,"%%DFU-E-GETEF, Error getting event flag,");
            put_disp();
            singlemsg(0,stat);
            return(stat);
        }
        /* Read 500 file headers */
        stat = SYS$QIO(efn[i-1],rvt[curvol].channel,IO$_READVBLK,
                       &iostat[i-1],0,0,&header[(i-1)*iosize+1],
                       (iosize*512),vbn,0,0,0);
        if ((stat & 1) != 1)
        {
            singlemsg(DFU_IOERR,stat);
            return(stat);
        }
        if (i != iocnt) vbn += iosize ;
    }
    return(1);
}

int get_next_header()
/*
   This routine is responsible for returning the next
   file header to the calling procedure . The context is kept
   in the ctx context block, so we do not need to pass
   any arguments.
*/
{
    unsigned int progress;

    if (ctx.i == -1) /*start of I/O processing */
    {
        ctx.i = 0;
        /* Fire off the first 2 asynchrouous IO's */
        progress = 0;
        if (progress_ind)
        {
            sprintf(outbuf,"     Progress : %3d%%     Status : Scanning INDEXF.SYS",
                    progress);
            put_status(1);
        }
        headers=0;
        status = read_indexf_multi(&vbn);
        if ((status & 1) != 1)
        {
            cleanup();
            return(status);
        }
        ctx.thread = 1;
        clean_flags.events = 1;
        status = SYS$WAITFR(efn[ctx.thread-1]); /* Wait for IO completion */
        status = iostat[ctx.thread-1].iosb_1;
        if ((status & 1) != 1)
            if (status != SS$_ENDOFFILE)
            {
                cleanup();
                return(status);
            }
        ctx.starti = (ctx.thread-1)*iosize + 1;
        ctx.endi = iostat[ctx.thread-1].length/512 + ctx.starti - 1;
        ctx.i = ctx.starti - 1;
    }
    /* now get the next header */
    {
        headers++;
        if ((headers % 65536) ==0)
        {
            headers++;
            (ctx.i)++;
        }
        (ctx.i)++;
        if (ctx.i > ctx.endi) /* Next IOSIZE headers */
        {
            if (ctrlc ==1) ctx.end = TRUE;
            if (iostat[ctx.thread-1].length < (iosize*512)) ctx.end = TRUE;
            if (vbn > (rvt[curvol].if_size+rvt[curvol].vbn_file_1 -1))
                ctx.end = TRUE;
            if (ctx.end == FALSE) /* Read next IOSIZE headers */
            {
                vbn += iosize;
                status = SYS$QIO(efn[ctx.thread-1],rvt[curvol].channel,IO$_READVBLK,
                                 &iostat[ctx.thread-1],0,0,&header[(ctx.thread-1)*iosize+1],
                                 (iosize*512),vbn,0,0,0);
                if ((status & 1) != 1)
                {
                    singlemsg(DFU_IOERR,status);
                    cleanup();
                    return(status);
                }
                (ctx.thread)++;
                if (ctx.thread > iocnt) ctx.thread = 1;
            }
            if (!(ctx.end)) /* Wait for IO completion for this thread */
            {
                status = SYS$WAITFR(efn[ctx.thread-1]); /* Wait for IO completion */
                status = iostat[ctx.thread-1].iosb_1;
                if ((status & 1) != 1)
                    if (status != SS$_ENDOFFILE)
                    {
                        cleanup();
                        ctx.end = TRUE;
                        return(status);
                    }
                ctx.starti = (ctx.thread-1)*iosize + 1;
                ctx.endi = iostat[ctx.thread-1].length/512 + ctx.starti - 1;
                ctx.i = ctx.starti;
            }
            if (progress_ind)
            {
                progress = (100*headers / highbit);
                sprintf(outbuf,"     Progress : %3d%%     Status : Scanning INDEXF.SYS",
                        progress);
                put_status(1);
            }
        }
        if (ctrlc == 1)
        {
            ctx.end = TRUE;
            return(1);
        }
        if (headers > highbit)         /* Do not proceed further than this */
        {
            ctx.end = TRUE;
            return(1);
        }
        ctx.hdr = (struct header_area_struct *) &header[ctx.i];
        ctx.id = (struct ident_area_struct *)
                 &header[ctx.i].block[2*((ctx.hdr)->idoffset)];
    }
    return(1);
}

void cleanup()
/*
   Various cleanup actions depending on flags
*/
{
    int i, stat;
    void dfu_handler();

    if (clean_flags.fopen == 1)
    {
        stat = fclose(fp);
    }

#if 1
    if (clean_flags.sort == 1)
    {
        stat = sor$end_sort(0);
    }
#endif

    if (clean_flags.events == 1)
        for (i=1; i <= iocnt ; i++)
        {
            stat = lib$free_ef(&efn[i-1]);
        }

    if (clean_flags.channels == 1)
        for (i=1; i <=maxvol ; i++)
        {
            stat = SYS$QIOW(0,rvt[i].channel,IO$_DEACCESS,&iostat[iocnt],
                            0,0,0,0,0,0,0,0);
            stat = SYS$DASSGN(rvt[i].channel);
            rvt[i].channel = 0;
            if (rvt[i].bchan != 0)
            {
                stat = SYS$QIOW(0,rvt[i].bchan,IO$_DEACCESS,&iostat[iocnt],
                                0,0,0,0,0,0,0,0);
                stat = SYS$DASSGN(rvt[i].bchan);
                rvt[i].bchan = 0;
            }
            if (lock_chan != 0)
            {
                dfu_handler();
                stat = SYS$CANEXH(&desblk);
            }
        }

    if (clean_flags.expreg == 1)
        for (i=1; i <=maxvol ; i++)
        {
            if (rvt[i].addr[0] != 0)
                stat = SYS$DELTVA(&rvt[i].addr[0], &rvt[i].addr[0], 0);
            if (rvt[i].bmap_addr[0] != 0)
                stat = SYS$DELTVA(&rvt[i].bmap_addr[0], &rvt[i].bmap_addr[0], 0);
        }
}

void copy_fid(struct f_id *target,struct f_id *from, Boolean check_rvn)
/*
    Procedure to copy FID's from one structure to
    another. If check_rvn is true the rvn field will
    be filled with the value of CURVOL if it is 0
*/
{
    struct f_id *p1,*p2; /* Two dummy pointers */

    p1 = target;
    p2 = from;
    p1->fid_num = p2->fid_num;
    p1->fid_seq = p2->fid_seq;
    p1->fid_nmx = p2->fid_nmx;
    p1->fid_rvn = p2->fid_rvn;
    if (check_rvn)
        if (p1->fid_rvn ==0) p1->fid_rvn = curvol;
}

int verify_header (struct header_area_struct *fh2)
/*
   Validate if a file header is valid. See File System
   Internals for the definition of a valid file header.
   Warning : does not check the file headers checksum
*/
{
    int highwater = 76; /* from $FH2DEF */
    if (fh2->struclev_overlay.struclev_fields.struclev != 2)
        return (SS$_BADFILEHDR);
    if (fh2->struclev_overlay.struclev_fields.strucver < 1)
        return (SS$_BADFILEHDR);
    if (fh2->idoffset < (highwater/2)) return (SS$_BADFILEHDR);
    if (fh2->idoffset > fh2->mpoffset ) return (SS$_BADFILEHDR);
    if (fh2->mpoffset > fh2->acoffset ) return (SS$_BADFILEHDR);
    if (fh2->acoffset > fh2->rsoffset ) return (SS$_BADFILEHDR);
    if (fh2->map_inuse > (fh2->acoffset - fh2->mpoffset)) return (SS$_BADFILEHDR);
    if (fh2->fid_overlay.fid_fields.fid_num == 0) return (SS$_BADFILEHDR);
    if (fh2->fid_overlay.fid_fields.fid_seq == 0) return (SS$_BADFILEHDR);
    return (SS$_NORMAL);
}

void get_map_pointers(struct header_area_struct *hdr, unsigned int *ptrs)
/*
   Count retrieval pointers in file header
   Do not count a placement pointer a s a real pointer
*/
{
    unsigned int i, format, offset, map_bytes, size;
    struct _hd
    {
        unsigned short block[256] ;
    } *head;

    head = (struct _hd *) hdr; /* We can now view the header as a 256 word strcuture */
    offset    = hdr->mpoffset ;
    map_bytes = hdr->map_inuse;
    size = 0;
    *ptrs = 0;
    for (i = offset; i < offset + map_bytes;)
    {
        format = head->block[i] >> 14;
        (*ptrs)++;
        switch (format)
        {
        case 0:
            (*ptrs)--;
            break;

        case 1:
            size += (head->block[i] & 255) + 1;
            break;

        case 2:
            size += (head->block[i] & 0x3fff) + 1;
            break;

        case 3:
            size += ((head->block[i] & 0x3fff) << 16);
            size += head->block[i + 1] + 1;
            break;
        }
        i += format + 1;
    }
}

void disass_map_ptr(struct header_area_struct *hdr, int *j)
/*
    Disassemble mapping pointer. Return start lbn, count and offset to
    next ptr.
*/
{
    unsigned short format;
    unsigned int i;
    struct _hd
    {
        unsigned short block[256] ;
    } *head;

    head = (struct _hd *) hdr; /* We can now view the header as a 256 word strcuture */
    i = *j;
    format = head->block[i] >> 14;
    switch (format)
    {
    case 0:
        lbncount = 0;
        lbnstart = 0;
        break;
    case 1:
        lbncount = (head->block[i] & 255) + 1;
        lbnstart = ((head->block[i] & 0x3f00) << 8)
                   + head->block[i+1];
        break;
    case 2:
        lbncount = (head->block[i] & 0x3fff) + 1;
        lbnstart = (head->block[i+2] << 16) + head->block[i+1];
        break;
    case 3:
        lbncount = ((head->block[i] & 0x3fff) << 16)
                   + head->block[i+1] + 1;
        lbnstart = (head->block[i+3] << 16) + head->block[i+2];
        break;
    }   /* End switch */
    *j = *j + format + 1;
}

int follow_extents(struct header_area_struct *hdr, int *rtv, int *num_hdrs)
/*
   Follow extension links and get the retrieval pointers
*/

{
    unsigned int rtvptr, ivbn, rvn, fidnum;
    static struct header_area_struct head;

    head.ext_fid_overlay.ext_fid[0] = hdr->ext_fid_overlay.ext_fid[0];
    head.ext_fid_overlay.ext_fid[1] = hdr->ext_fid_overlay.ext_fid[1];
    head.ext_fid_overlay.ext_fid[2] = hdr->ext_fid_overlay.ext_fid[2];
    while (head.ext_fid_overlay.ext_fid[0] !=0)
    {
        rvn = head.ext_fid_overlay.ext_fid_fields.ext_fid_rvn;
        if (rvn == 0) rvn = curvol;
        fidnum = head.ext_fid_overlay.ext_fid_fields.ext_fid_nmx << 16;
        fidnum += head.ext_fid_overlay.ext_fid[0];
        ivbn = rvt[rvn].vbn_file_1 + fidnum - 1;
        status = SYS$QIOW(0,rvt[rvn].channel,IO$_READVBLK,
                          &iostat[iocnt],0,0,&head,512,ivbn,0,0,0);
        if ((status & 1) == 1) status = iostat[iocnt].iosb_1;
        if ((status & 1) != 1)
        {
            sprintf(outbuf,"%%DFU-E-READERR, Error reading extension header,");
            put_disp();
            singlemsg(0,status);
            head.ext_fid_overlay.ext_fid[0] = 0;
            return(status);
        }
        else
        {
            get_map_pointers(&head, &rtvptr);
            *rtv += rtvptr;
            *num_hdrs += 1;
        }
    }
    return(1);
}

void dfu_handler()
/* Exit handler to unlock volume */

{
    static struct fibdef i_fib; /* File information block */
    struct
    {
        int fiblen;
        struct fibdef *fibadr;
    } fibdescr;
    int stat;

    fibdescr.fiblen = sizeof(i_fib);
    fibdescr.fibadr = &i_fib;
    i_fib.fib$w_exctl = FIB$C_UNLK_VOL;
    if (lock_chan !=0)
    {
        stat = SYS$QIOW(0,lock_chan,IO$_ACPCONTROL,&iostat[0],
                        0,0,&fibdescr,0,0,0,0,0);
        sprintf(outbuf,"%%DFU-I-UNLOCK, Volume unlocked");
        put_disp();
        stat = SYS$DASSGN(lock_chan);
        lock_chan = 0;
    }
}

void add_item(struct _it3 *i_list, int len, int itm, int bufadr, int ret)
/*
   Routine to fill an item in an VMS item_list
*/

{
    i_list->buflen = len;
    i_list->itemcode = itm;
    i_list->bufadr = (unsigned int *) bufadr;
    i_list->retlen = (unsigned int *) ret;
}
