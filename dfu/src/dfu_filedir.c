// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified DFU source file, 2004.

/*
     DFU V2.2

     DFU_FILEDIR.C

     This file contains the file and directory scanning routines.

     Called by SET , DELETE, DEFRAG and DIRECTORY

*/

#ifndef VAXC
#pragma message disable(ALIGNEXT)
#pragma message disable(GLOBALEXT)
#pragma message disable(INCOMPARRY)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ssdef.h>
#include <atrdef.h>
#include <libdef.h>
#include <descrip.h>
#include <climsgdef.h>
#include <syidef.h>
#include <rms.h>
#include <starlet.h>
#include "fibdef.h"
#include "file_hdr.h"
#include <iodef.h>
#include <sor$routines.h>
#include <cli$routines.h>
#include <lib$routines.h>
#include <smg$routines.h>
#include <smgdef.h>
#include <trmdef.h>
#ifndef IO$M_MOVEFILE
#define IO$M_MOVEFILE 4096
#endif
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
#define CLI$GET_VALUE cli$get_value
#define CLI$PRESENT cli$present

#define FAB _fabdef
#define NAM _namdef
#define RAB _rabdef

typedef struct _d_e {unsigned short size, versionlim;
          unsigned char flags, count, name[]; };
typedef struct _f_e {unsigned short version, fid_num, fid_seq;
          unsigned char fid_rvn, fid_nmx; };

extern unsigned int ctrlc, keyb_id, disp1_id;
extern Boolean smg$enable;
extern char outbuf[255];

/* Share memory space */
#if 0
extern _align(PAGE) struct _hdr {
    char block[512];
    } header[1001];
#else
struct _hdr {
    char block[512];
    } header[1001];
#endif

static unsigned int status, version, alloc, ratio, truncsize, lbn;
static Boolean matalias, matcheck, matversion, matdump, 
  mattruncate, mattest, matempty, matrepair, matlbn;
static FILE *fp;
static struct {unsigned short iosb_1; 
  unsigned int length; short spec; 
  } iostat;

extern globalvalue DFU_ASSIGN, DFU_NOPRIV;

int display_stat();

int set_command(mask)
/* 
   Set any file attribute you like!
*/

{ struct FAB fab;
  struct NAM nam;
  static struct fibdef fib;
  struct {int fiblen; struct fibdef *fibadr;} fibdescr;
  static struct _quad {unsigned int date[2];} bakdate, expdate, credate, 
   revdate, r_bakdate, r_expdate, r_credate, r_revdate;
  static struct _four {unsigned short date[4];} ascdates;
  static char res_str[255], exp_str[255], devnam[15], 
    fname[255], xname[255], dummy[80], ans[4], ident[30];
  unsigned int func, x, bad, dir, lock, log, cont, rvcount, uic,
    bucksize, vfcsize, maxrec, eblock, ebyte, hiblk, modifiers;
  unsigned short chan, rtype,fileorg, recsize;
  static unsigned long uchar , old_uic ;
  static unsigned char attrib[32];
  Boolean matfile, matbadacl, matdir, matupdate, matlock, matcont, matbakdate,
    matexpdate, matcredate, matrevdate, matlog, matrvcount, matrecsize,
    matfortran,matimplied,matprint,matnospan, matnone, matbucksize,
    matvfcsize, matmaxrec, matfixed, matvar, matvfc, matundef, matstream,
    matstreamlf, matstreamcr, matdirect, matindex, matrelat, matseq,
    matebyte, mateblock, matconfirm, matuic;
  static struct { unsigned short atr_size, atr_type;
                  void * atr_address; } 
    acb[9] =  { {32, ATR$C_RECATTR, (long) &attrib }, 
      { 8, ATR$C_ASCDATES, (long) &ascdates },
      { 8, ATR$C_CREDATE, (long) &credate } , 
      { 8, ATR$C_REVDATE, (long) &revdate } , 
      { 8, ATR$C_BAKDATE, (long) &bakdate } , 
      { 8, ATR$C_EXPDATE, (long) &expdate } , 
      { 4, ATR$C_UIC, (long) &old_uic } ,
      { 4, ATR$C_UCHAR, (long) &uchar } , { 0, 0, 0 } };
  FILE *ip;
  $DESCRIPTOR(devnam_descr , devnam);
  $DESCRIPTOR(xname_descr , xname);
  $DESCRIPTOR(dummy_descr , dummy);
  $DESCRIPTOR(answer , ans);
  $DESCRIPTOR(id_descr, ident);
  $DESCRIPTOR(prompt,outbuf);

/* Check the privileges */
  if (mask > -1) 
  { singlemsg(0, DFU_NOPRIV); return(SS$_NOPRIV);
  }

  modifiers = SMG$M_NORECALL | SMG$M_NOKEEP;
/* Check file name */
  status = parse_item("p1", &xname_descr, 0 , &matfile, 0);
  if (matfile == FALSE) 
  { sprintf(outbuf,"%%DFU-E-NOFILSPEC, No file specified on command line");
    put_disp(); return(SS$_NOSUCHFILE);
  }

/* Get all the attributes */
  status = parse_item("confirm", &dummy_descr, 0 , &matconfirm, 2);
  status = parse_item("update", &dummy_descr, 0 , &matupdate, 2);
  status = parse_item("contiguous_best_try", &dummy_descr, &cont, &matcont, 7);
  status = parse_item("locked", &dummy_descr, &lock, &matlock, 7);
  status = parse_item("badacl", &dummy_descr, &bad, &matbadacl, 7);
  status = parse_item("directory", &dummy_descr, &dir, &matdir, 7);
  log = 0;
  status = parse_item("log", &dummy_descr, &log, &matlog, 7);
  status = parse_item("backup_date", &dummy_descr, 
    &r_bakdate, &matbakdate, 4);
  status = parse_item("creation_date", &dummy_descr, 
    &r_credate, &matcredate, 4);
  status = parse_item("expiration_date", &dummy_descr, 
    &r_expdate, &matexpdate, 4);
  status = parse_item("revision_date", &dummy_descr, 
    &r_revdate, &matrevdate, 4);
  ebyte = eblock = 0;
  status = parse_item("ebyte", &dummy_descr, &ebyte, &matebyte, 1);
  status = parse_item("eblock", &dummy_descr, &eblock, &mateblock, 1);
  status = parse_item("rvcount", &dummy_descr, &rvcount, &matrvcount, 1);
  status = parse_item("recsize", &dummy_descr, &recsize, &matrecsize, 1);
  status = parse_item("bucketsize", &dummy_descr, &bucksize, &matbucksize, 1);
  status = parse_item("vfcsize", &dummy_descr, &vfcsize, &matvfcsize, 1);
  status = parse_item("maxrec", &dummy_descr, &maxrec, &matmaxrec, 1);
  status = parse_item("fortran", &dummy_descr, 0 , &matfortran, 2);
  status = parse_item("implied", &dummy_descr, 0 , &matimplied, 2);
  status = parse_item("print", &dummy_descr, 0 , &matprint, 2);
  status = parse_item("none", &dummy_descr, 0 , &matnone, 2);
  status = parse_item("nospan", &dummy_descr, 0 , &matnospan, 2);
  status = parse_item("fixed", &dummy_descr, 0 , &matfixed, 2);
  status = parse_item("variable", &dummy_descr, 0 , &matvar, 2);
  status = parse_item("vfc", &dummy_descr, 0 , &matvfc, 2);
  status = parse_item("undefined", &dummy_descr, 0 , &matundef, 2);
  status = parse_item("stream", &dummy_descr, 0 , &matstream, 2);
  status = parse_item("streamlf", &dummy_descr, 0 , &matstreamlf, 2);
  status = parse_item("streamcr", &dummy_descr, 0 , &matstreamcr, 2);
  status = parse_item("direct", &dummy_descr, 0 , &matdirect, 2);
  status = parse_item("indexed", &dummy_descr, 0 , &matindex, 2);
  status = parse_item("relative", &dummy_descr, 0 , &matrelat, 2);
  status = parse_item("sequential", &dummy_descr, 0 , &matseq, 2);
/* Ident or UIC */
  status = parse_item("ident", &id_descr, 0, &matuic, 0);
#if 0
  if (matuic == TRUE) 
  { status = SYS$ASCTOID(&id_descr, &uic, 0);
    if ((status & 1) != 1) 
    { sprintf(outbuf,"%%DFU-E-INVID, Invalid identifier entered,\n");
      put_disp(); singlemsg(0,status);
      return(status); 
    }
  }
#endif
  if (matuic == FALSE) 
    status = parse_item("uic", &dummy_descr, &uic, &matuic, 6);
  if ((status &1 ) != 1)
    { sprintf(outbuf,"%%DFU-E-INVID, Invalid identifier entered,\n");
      put_disp(); 
      return(status); 
    }
  id_descr.dsc$w_length = 30;

  ip = 0;
  while((matfile) && (ctrlc ==0))/* Loop through the file list */
  { x = strindex(&xname," ",255); xname[x] = '\0';
    if (xname[0] == '@')
    { if (ip == 0) 
      { ip = fopen(&xname[1],"r");
        if (ip == 0)
        { sprintf(outbuf,"%%DFU-E-OPENERR, Error opening file %s",&xname[1]);
          put_disp(); goto next_name;
        }
      }
      status = (int) fgets(fname,255,ip);
      if (status !=0)
      { x = strindex(&fname," ",255); fname[x] = '\0';
      }
       else
      { fclose(ip); ip = 0; goto next_name;
      }
    }
     else strcpy(fname,xname);

    fibdescr.fiblen = sizeof(fib);
    fibdescr.fibadr = &fib;

/* fill in FAB and NAM blocks */

    fab = cc$rms_fab;
    fab.fab$l_fna = (char *) &fname;
    fab.fab$b_fns = strlen(fname);
    fab.fab$l_nam = &nam;

    nam = cc$rms_nam;
    nam.nam$b_rss = NAM$C_MAXRSS;
    nam.nam$l_rsa = (char *) &res_str;
    nam.nam$b_ess = NAM$C_MAXRSS;
    nam.nam$l_esa = (char *) &exp_str;
  
/* get the rest of the fab fields */

    status = SYS$PARSE(&fab , 0 , 0);
    if ((status & 1 ) != 1) 
    { sprintf(outbuf,"%%DFU-E-PARSEERR, Error parsing filename from %s,"
              , fname); put_disp();
      singlemsg(0,status); status = RMS$_NMF; goto next_set;
    }

/* now search for the file */
    
    status = SYS$SEARCH(&fab , 0 , 0);
    while (status != RMS$_NMF)
    { res_str[nam.nam$b_rsl] = '\0';
      if ((status & 1 ) != 1) 
      { sprintf(outbuf,"%%DFU-E-SEARCHERR, Error searching file %s,"
              , res_str); put_disp();
        singlemsg(0,status); status = RMS$_NMF; goto next_set;
      }
      res_str[nam.nam$b_rsl] = '\0';

/* Open a channel to the device */
      devnam_descr.dsc$w_length = nam.nam$t_dvi[0];
      strcpy(devnam,&nam.nam$t_dvi[1]);
      status = SYS$ASSIGN(&devnam_descr, &chan, 0 , 0);
      if ((status & 1 ) != 1) 
      { singlemsg(DFU_ASSIGN,status); return(status);
      }
  
/* Now fill in File id and function code for the ACP call */
      fib.fib$w_fid[0] = nam.nam$w_fid_num;
      fib.fib$w_fid[1] = nam.nam$w_fid_seq;
      fib.fib$w_fid[2] = nam.nam$w_fid_rvn;
      if (matupdate)
        fib.fib$l_acctl = 
          FIB$M_WRITE | FIB$M_WRITETHRU | FIB$M_NOWRITE | FIB$M_NOREAD;
       else
        fib.fib$l_acctl = 
          FIB$M_WRITE | FIB$M_WRITETHRU | FIB$M_NOWRITE | FIB$M_NOREAD |
          FIB$M_NORECORD; 
      if (matuic) fib.fib$l_acctl |= FIB$M_NOLOCK;
      func = IO$_ACCESS | IO$M_ACCESS;
      status = SYS$QIOW(0, chan, func, &iostat, 0,0,
        &fibdescr, 0 ,0 ,0 , acb, 0);
      if ((status & 1) == 1) status = iostat.iosb_1;
      if ((status & 1) != 1) 
      { sprintf(outbuf,"%%DFU-E-OPENERR, Error accesing file %s,", res_str);
        put_disp(); singlemsg(0,status); status = 1; goto next_set;
      }

/* Confirm this file ? */
      ans[0] = 'y';
      if(matconfirm)
      { sprintf(outbuf,"Modify %s ? : ",res_str);
        ans[0] = 'n'; x = 4;
        prompt.dsc$w_length = strlen(outbuf);
        if (smg$enable) 
          status = SMG$READ_COMPOSED_LINE(&keyb_id, 0, &answer,
            &prompt , &x, &disp1_id, &modifiers, 0,0,0,0,0);
         else
          status = SMG$READ_COMPOSED_LINE(&keyb_id, 0, &answer,
            &prompt , &x, 0 , &modifiers, 0,0,0,0,0);
        if ((ans[0] == 'a') || (ans[0] == 'A'))
        { matconfirm = FALSE; 
          ans[0] = 'y'; 
        }
	if ((ans[0] == 'y') || (ans[0] == 'Y') || (ans[0] == '1')
          || (ans[0] == 't') || (ans[0] == 'T'))
              ans[0] = 'y';
         else ans[0] = 'n';
      }

/* Now modify all the required attributes */
      if (ans[0] == 'y')
      { rtype = attrib[0] % 16;
        fileorg = attrib[0] - rtype;
        fileorg = fileorg / 16 ;
        if (matbadacl) 
         if (bad == 0) uchar = uchar | FH2$M_BADACL; /* Set the bit */
           else uchar = uchar & ~FH2$M_BADACL; /* Clear the bit */
        if (matdir) 
         if (dir == 0) uchar = uchar | FH2$M_DIRECTORY; /* Set the bit */
           else uchar = uchar & ~FH2$M_DIRECTORY; /* Clear the bit */
        if (matlock) 
         if (lock == 0) uchar = uchar | FH2$M_LOCKED; /* Set the bit */
           else uchar = uchar & ~FH2$M_LOCKED; /* Clear the bit */
        if (matcont) 
         if (cont == 0) uchar = uchar | FH2$M_CONTIGB; /* Set the bit */
           else 
            uchar = uchar & ~FH2$M_CONTIGB; /* Clear the bit */
        if (matbakdate) memcpy(&bakdate, &r_bakdate, 8);
        if (matrevdate) memcpy(&revdate, &r_revdate, 8);
        if (matexpdate) memcpy(&expdate, &r_expdate, 8);
        if (matcredate) memcpy(&credate, &r_credate, 8);
        if (matebyte) memcpy(&attrib[12], &ebyte, 2);
        if (mateblock)
        { if (!matebyte) 
          { attrib[12] = 0; /* Clear eof byte if it was not specified */
            attrib[13] = 0;
          }
          memcpy(&hiblk, &attrib[4], 4); /* Save highest block */
          if (eblock == 0) 
            eblock = (hiblk / 65536 ) + ((hiblk %65536) << 16);
          if (ebyte == 0) eblock++;
          hiblk = (eblock % 65536) << 16;
          hiblk += (eblock / 65536);
          memcpy(&attrib[8], &hiblk, 4);
        }
        if (matuic) old_uic = uic;
        if (matrvcount) ascdates.date[0] = rvcount;
        if (matrecsize) memcpy(&attrib[2],&recsize,2);
        if (matbucksize) attrib[14] = (unsigned char) bucksize;
        if (matvfcsize) attrib[15] = (unsigned char) vfcsize;
        if (matmaxrec) attrib[16] = (unsigned char) maxrec;
        if (matnone) attrib[1] = 0;
        if (matfortran) attrib[1] = FAT$M_FORTRANCC;
        if (matimplied) attrib[1] = FAT$M_IMPLIEDCC;
        if (matprint) attrib[1] = FAT$M_PRINTCC;
        if (matnospan) attrib[1] = attrib[1] | FAT$M_NOSPAN;
        if (matfixed) rtype = FAT$C_FIXED;
        if (matvar) rtype = FAT$C_VARIABLE;
        if (matvfc) rtype = FAT$C_VFC;
        if (matundef) rtype = FAT$C_UNDEFINED;
        if (matstream) rtype = FAT$C_STREAM;
        if (matstreamlf) rtype = FAT$C_STREAMLF;
        if (matstreamcr) rtype = FAT$C_STREAMCR;
        if (matindex) fileorg = FAT$C_INDEXED;
        if (matdirect) fileorg = FAT$C_DIRECT;
        if (matrelat) fileorg = FAT$C_RELATIVE;
        if (matseq) fileorg = FAT$C_SEQUENTIAL;
        attrib[0] = (unsigned char) (fileorg * 16) + 
         (unsigned char) rtype;
/* Now rewrite the file header */
        status = SYS$QIOW(0, chan, IO$_MODIFY, &iostat, 0,0,
         &fibdescr, 0, 0, 0, acb, 0);
        if ((status & 1) == 1) status = iostat.iosb_1;
        if ((status & 1) != 1) 
        { sprintf(outbuf,"%%DFU-E-SETERR, Error modifying file %s,", res_str);
          put_disp(); singlemsg(0,status); 
        }
         else
          if (log == 0) 
          { sprintf(outbuf,"%%DFU-S-MODIFIED, File %s modified ",res_str);
            put_disp();
          }
      }
      status = SYS$QIOW(0, chan, IO$_DEACCESS, &iostat ,0,0,
        &fibdescr,0,0,0,0,0);
      status = 1;
         
next_set:
      if ((status & 1) == 1) status = SYS$SEARCH(&fab , 0 , 0);
        else status = RMS$_NMF;
      SYS$DASSGN(chan);
      if (ctrlc == 1) 
      { do_abort();
        status = RMS$_NMF;
      }
    }
next_name:
    if (ip == 0)
    { xname_descr.dsc$w_length = 255;
      status = parse_item("p1", &xname_descr, 0 , &matfile, 0);
      if ((status & 1) != 1) matfile = FALSE;
    }
  }
  if (ip != 0) fclose(ip);
  sprintf(outbuf,"\n%%DFU-I-READY, SET command ready");
  put_disp();
  return(1);
}

int delete_file(unsigned short id[3], unsigned short dchan, 
                  Boolean noremove, Boolean deldir, Boolean nolog,
		  Boolean rem_file)
/* 
   Performs the actual file delete
   Assumes channel already opened
*/

{ static char file[86];
  static struct fibdef dfib; 
  static unsigned short back[3];
  static unsigned int uchar;
  unsigned int tmp, status2;
  static struct { unsigned short atr_size, atr_type;
                  void * atr_address; } 
    d_acb[4] =  { {86, ATR$C_ASCNAME, (long) &file[0]}, 
      {6, ATR$C_BACKLINK, (long) &back[0]},
      { 4, ATR$C_UCHAR, (long) &uchar} , {0, 0, 0} };
  struct {int fiblen; struct fibdef *fibadr;} dfib_descr;
  unsigned int func;
                             
/* Setup d_fib */
  dfib_descr.fiblen = sizeof(dfib);
  dfib_descr.fibadr = &dfib;
  func = IO$_ACCESS | IO$M_ACCESS; 
  dfib.fib$w_fid[0] = id[0];
  dfib.fib$w_fid[1] = id[1];
  dfib.fib$w_fid[2] = id[2];
  dfib.fib$w_did[0] = 0; 
  dfib.fib$w_did[1] = 0;
  dfib.fib$w_did[2] = 0;
  dfib.fib$l_acctl = FIB$M_WRITE | FIB$M_WRITETHRU; 
  if (!noremove) dfib.fib$w_nmctl = FIB$M_FINDFID;
   else dfib.fib$w_nmctl = 0;

  file[0]= ' ';
  if (!deldir) /* No need to access this file first when deleting directories*/
  { status = SYS$QIOW(0, dchan, func, &iostat, 0,0,
      &dfib_descr, 0 ,0 ,0 , &d_acb, 0);
    if ((status & 1 ) == 1) status = iostat.iosb_1;
    if ((status & 1) !=1)
    { if (status != SS$_NOSUCHFILE)
/* Now do a IO$_ACCESS without IO$M_ACCESS because we need the file name.
   A little bit clumsy but I know no other way to solve this */
        status2 = SYS$QIOW(0, dchan, IO$_ACCESS, &iostat, 0,0,
          &dfib_descr, 0 ,0 ,0 , &d_acb, 0);
      tmp = strindex(&file," ",86); file[tmp] = '\0';
      sprintf(outbuf,"%%DFU-E-NOTDEL, Error deleting file %s,",file);
      put_disp(); singlemsg(0,status); goto end_del;
    }

    if ((uchar & 32768) == 32768) noremove = TRUE;  /* Marked bit */
/* Now copy the backlink to the directory id */
    if (!noremove)
    { dfib.fib$w_did[0] = back[0]; 
      dfib.fib$w_did[1] = back[1];
      dfib.fib$w_did[2] = back[2];
    }
     else
    { dfib.fib$w_did[0] = 0; 
      dfib.fib$w_did[1] = 0;
      dfib.fib$w_did[2] = 0;
    }
  }
/* And delete the file */  
  tmp = strindex(&file," ",86); file[tmp] = '\0';
  if (rem_file) func = IO$_DELETE;
   else func = IO$_DELETE | IO$M_DELETE;
  status = SYS$QIOW(0, dchan, func, &iostat, 0,0,
    &dfib_descr, 0 ,0 ,0 , 0 , 0);
  if ((status & 1 ) == 1) status = iostat.iosb_1;
  if ((status & 1) !=1)
  { sprintf(outbuf,"%%DFU-E-NOTDEL, Error deleting file %s ,",file);
    put_disp(); singlemsg(0,status); goto end_del;
  }
  if (!deldir)
  { if (!nolog) 
    { sprintf(outbuf,"%%DFU-S-DELETED, File %s deleted ", file);
      put_disp();
    }
    status = SYS$QIOW(0, dchan, IO$_DEACCESS,0,0,0,0,0,0,0,0,0);
  }
  status = 1;
end_del:
  return(status);
}

int remove_file(unsigned short id[3], unsigned short did[3],
                unsigned short dchan)
/* 
	Remove a file entry for a non-existent file
	Assumes channel is open
*/

{ static struct fibdef dfib; 
  unsigned int tmp, status2;
  struct {int fiblen; struct fibdef *fibadr;} dfib_descr;
  unsigned int func;
                             
/* Setup d_fib */
  dfib_descr.fiblen = sizeof(dfib);
  dfib_descr.fibadr = &dfib;
  dfib.fib$w_fid[0] = id[0];
  dfib.fib$w_fid[1] = id[1];
  dfib.fib$w_fid[2] = id[2];
  dfib.fib$w_did[0] = did[0]; 
  dfib.fib$w_did[1] = did[1];
  dfib.fib$w_did[2] = did[2];
  dfib.fib$l_acctl = FIB$M_WRITE;
  dfib.fib$w_nmctl = FIB$M_FINDFID;

/* And remove the file */  
  func = IO$_DELETE;
  status = SYS$QIOW(0, dchan, func, &iostat, 0,0,
    &dfib_descr, 0 ,0 ,0 , 0 , 0);
  if ((status & 1 ) == 1) status = iostat.iosb_1;
  return(status);
}

int delete_directory(unsigned int dchan, unsigned short f_id[3], 
     char *r_str, Boolean nolog)
/* 
   Subroutine to open the directory, get all file-id's
   and delete those files.
*/

{ unsigned int func;
  int i, j, k, n, nr, vbn, x, e_blk, h_blk, 
    progress, num_files, lastsize;
  static unsigned int uchar;
  static short attrib[16];
  static struct fibdef fib; 
  static struct { unsigned short atr_size, atr_type;
                  void * atr_address; } 
    acb[3] =  { { 4, ATR$C_UCHAR, (long) &uchar} , 
      { 32, ATR$C_RECATTR, (long) &attrib}, 
      {0, 0, 0} };
  struct {int fiblen; struct fibdef *fibadr;} fib_descr;
  struct _d_e *dir_entry;
  struct _f_e *fid_entry;
  struct _quad {unsigned int address[2];} retadr;
  struct _sh {char block[512];} empty, *i_dir; /* Pointer to shared memory */
  Boolean found;

/* Set up some fib fields */
  empty.block[0] = -1; empty.block[1] = -1;
  fib_descr.fiblen = sizeof(fib);
  fib_descr.fibadr = &fib;
  func = IO$_ACCESS | IO$M_ACCESS;
  num_files = 0;
/* Now fill in File id and function code for the ACP call */
  fib.fib$w_fid[0] = f_id[0];
  fib.fib$w_fid[1] = f_id[1];
  fib.fib$w_fid[2] = f_id[2];
  fib.fib$l_acctl = FIB$M_WRITE | FIB$M_NOWRITE; 
  
/* Open the directory */
  status = SYS$QIOW(0, dchan, func, &iostat,0,0,&fib_descr, 0,0,0, &acb,0);
  if ((status & 1 ) == 1) status = iostat.iosb_1;
  if ((status & 1) !=1)
  { sprintf(outbuf,"%%DFU-E-OPENERR, Error opening directory %s,",r_str);
    put_disp();
    singlemsg(0,status); return(status);
  }
/* Get the directory size */
  e_blk = (attrib[4] << 16) + attrib[5];
  h_blk = (attrib[2] << 16) + attrib[3];
  
/* Check if directory is valid */
  if ( ((attrib[0] & 2048 ) != 2048) /*NOSPAN attribute */
    || ((uchar & 8192) != 8192) /* Directory bit */
    || ((uchar & 128) != 128) ) /* Contiguous bit */
  { sprintf(outbuf,"%%DFU-W-INVDIR, %s is not a valid directory",r_str);
    put_disp();
    status = SYS$QIOW(0, dchan, IO$_DEACCESS,0,0,0,0,0,0,0,0,0);
    return(1);
  }
  if ( strstr(r_str,".DIR;1") == 0) 
  { sprintf(outbuf,"%%DFU-W-INVDIR, %s is not a valid directory",r_str);
    put_disp();
    status = SYS$QIOW(0, dchan, IO$_DEACCESS,0,0,0,0,0,0,0,0,0);
    return(1);
  }
  lastsize = 0;
  if (e_blk > 1000) 
  { /* Allocate dynamic space to read the directory file */
    status = SYS$EXPREG(e_blk, &retadr.address[0], 0, 0);
    if ((status & 1) != 1)
    { sprintf(outbuf,"%%DFU-E-INSFMEM, Error allocating %d pages memory for %s",
        e_blk, r_str);
      put_disp();
      singlemsg(0,status); lastsize = 0;
      status = SYS$QIOW(0, dchan, IO$_DEACCESS,0,0,0,0,0,0,0,0,0);
      return(1);
    }
     else lastsize = e_blk;
  }    
  
/* Now read the directory with 1 I/O */
  vbn = e_blk - 1;
/* Pointer to space for directory blocks (1000 max) */
  if (e_blk <= 1000)
    i_dir = (struct _sh *) &header[0].block[0];
   else
    i_dir = (struct _sh *) retadr.address[0];
  status = SYS$QIOW(0, dchan, IO$_READVBLK, &iostat, 0,0,
    i_dir, (vbn*512), 1, 0,0,0);
  if ((status & 1 ) == 1) status = iostat.iosb_1;
  if ((status & 1) !=1)
  { sprintf(outbuf,"%%DFU-E-READERR, Error reading directory %s,",r_str);
    put_disp(); singlemsg(0,status);  
    status = SYS$QIOW(0, dchan, IO$_DEACCESS,0,0,0,0,0,0,0,0,0);
    goto next_del;
  }
/* Check for corrupted directory */
  if (i_dir->block[0] == 0)
  { sprintf(outbuf,"%%DFU-E-JUNKDIR, Corrupted directory (byte offset 0) %s",
     r_str);
    put_disp();
    status = SYS$QIOW(0, dchan, IO$_DEACCESS,0,0,0,0,0,0,0,0,0);
    goto next_del;
  }

/* Now processs the directory blocks */
  k = 1; n = 0;
  while (k < e_blk)
  { progress = 100 * (k-1) / e_blk;
    sprintf(outbuf,"     Progress : %3d%%     Status : Deleting directory",
      progress);
    put_status(1);
    i = 1; 
    found = FALSE;   
    while (!found)
    { if ((i > 512) || i_dir->block[i-1] == -1) found = TRUE; /* End of block */
     else /* parse this entry */
/* Pointer to directory record */
      { dir_entry = (struct _d_e *) &i_dir->block[i-1]; 
        nr = (dir_entry->size - 4 - dir_entry->count) / 8; /* # of files entries */ 
        n += nr;
        j = i + dir_entry->size + 2; /* End address of this entry */
/* Align count field */
        if ((dir_entry->count &1) ==1) dir_entry->count +=1;
        fid_entry = (struct _f_e *) &i_dir->block[i + 5 + dir_entry->count];
        while (nr >= 1)
        { /* Now delete this entry */
	  status = delete_file(&fid_entry->fid_num, dchan, 
            TRUE, TRUE, TRUE, FALSE);
	  if ((status & 1 ) == 1)
	  { num_files++;
	    if (!nolog) 
            { sprintf(outbuf,"%%DFU-S-DELETED, File %.*s;%d deleted ",
                dir_entry->count, dir_entry->name, fid_entry->version); 
              put_disp();
            }
	  }
	  if (ctrlc == 1) return(1);
          nr--; fid_entry++;
        }
        i = j; /* Pointer to next entry */
      } /* endif*/
    }
    k += 1; i_dir += 1; /* Next block */
  } /* end while */
  sprintf(outbuf,"     Progress : 100%%     Status : READY");
  put_status(1);
  sprintf(outbuf,"%%DFU-S-TOTAL, %d file(s) deleted", num_files);
  put_disp();
/* Now fake the directory as an empty directory and delete it */
  e_blk = 2;
  attrib[4] = 0;
  attrib[5] = 2;
  vbn = 1;
  status = SYS$QIOW(0, dchan, IO$_WRITEVBLK, &iostat,0,0,
    &empty.block[0], 2 , 1, 0,0,0);
  if ((status & 1 ) == 1) status = iostat.iosb_1;
  if ((status & 1) !=1)
    status = SYS$QIOW(0, dchan, IO$_DEACCESS,0,0,0,0,0,0,0,0,0);
   else 
/* Write the new attributes back , truncate and close the file */
  { fib.fib$l_exsz = 0;
    fib.fib$w_exctl = FIB$M_TRUNC;
    fib.fib$l_exvbn = e_blk;
    status = SYS$QIOW(0, dchan, IO$_DEACCESS, &iostat, 0,0,
      &fib_descr, 0,0,0, &acb,0);
  }
  status = delete_file(f_id, dchan, FALSE, FALSE, nolog, FALSE); 
next_del:
  if (lastsize !=0)
    SYS$DELTVA(&retadr.address[0], 0 , 0);
  return(1);
}

int parse_tree(short int dchan, char *r_str, Boolean nolog)
/* 
   Subroutine to parse for all subdirectories.
   All directories found are sorted such that
   delete_directory can be called in proper order
*/

{ struct FAB xfab;
  struct NAM xnam;
  char ex_str[255];
  static struct _srt { char rx_str[255]; unsigned short f_id[3]; } sort_rec;
  int x, y, reclen;
  short keybuf[5] = {1, DSC$K_DTYPE_T, 0, 0, 255};
  struct dsc$descriptor sort_descr = 
   {sizeof(sort_rec), DSC$K_DTYPE_T, DSC$K_CLASS_S, (char *) &sort_rec};

/* Rearrange this directory into a wildcard */
  y = strindex(r_str,"]",255);
  x = y + strindex(&r_str[y],".",255);
  r_str[x] = '\0';
  r_str[y] = '.';
  strcat(r_str,"...]*.DIR;1");

/* Now parse the result */
  xfab = cc$rms_fab;
  xfab.fab$l_fna = r_str;
  xfab.fab$b_fns = strlen(r_str);
  xfab.fab$l_nam = &xnam;
  
  xnam = cc$rms_nam;
  xnam.nam$b_rss = NAM$C_MAXRSS;
  xnam.nam$l_rsa = (char *) &sort_rec.rx_str;
  xnam.nam$b_ess = NAM$C_MAXRSS;
  xnam.nam$l_esa = (char *) &ex_str;

/* get the rest of the fab fields */
  
  status = SYS$PARSE(&xfab , 0 , 0);
  if ((status & 1 ) != 1) 
  { sprintf(outbuf,"%%DFU-E-PARSEERR, Error parsing filename from %s,", r_str);
    put_disp(); singlemsg(0,status); return(status);
  }
  sprintf(outbuf,"%%DFU-I-PARSEDIR, Parsing directory tree...");
  put_disp();
  reclen = sizeof(sort_rec);
#if 1
  status = sor$begin_sort(&keybuf, &reclen, 0,0,0,0,0,0,0,0);
#endif
  status = SYS$SEARCH(&xfab , 0 , 0);
  while (status != RMS$_NMF)
  { if ((status & 1 ) != 1) 
    { sort_rec.rx_str[xnam.nam$b_rsl] = '\0';
      sprintf(outbuf,"%%DFU-W-NOSUBDIR, no subdirectories found in this tree");
      put_disp();
#if 1
      sor$end_sort(); return(1);
#else
      return(1);
#endif
    }
     else 
      for (x = xnam.nam$b_rsl; x <=254; x++) sort_rec.rx_str[x] = ' ';
    sort_rec.f_id[0] = xnam.nam$w_fid_num;
    sort_rec.f_id[1] = xnam.nam$w_fid_seq;
    sort_rec.f_id[2] = xnam.nam$w_fid_rvn;
#if 1
    sor$release_rec(&sort_descr);
#endif
    status = SYS$SEARCH(&xfab , 0 , 0);
  }
/* Now sort the results */
#if 1
  status = sor$sort_merge();
  if ((status & 1) !=1)
  { sor$end_sort(); return(status);
  }
  status = sor$return_rec(&sort_descr);
#endif
  while (status != SS$_ENDOFFILE)
  { x = strindex (sort_rec.rx_str," ",255); sort_rec.rx_str[x] = '\0';
    if (ctrlc != 1)
    { sprintf(outbuf,"\n%%DFU-I-CLEANUP, Deleting %s...", sort_rec.rx_str);
      put_disp();
      status = delete_directory(dchan, &sort_rec.f_id[0], 
                  (char *) &sort_rec.rx_str, nolog);
#if 1
      status = sor$return_rec(&sort_descr);
#endif
    }
     else status = SS$_ENDOFFILE; /* Stop at CTRL/C */
  }
#if 1
  sor$end_sort();
#endif
  return(1);
}

int delete_command(mask)
/*
   Delete file by file-id
   Also used for deleting directories and directory trees
*/

{ static char device[64], dummy[7], dname[160], exp_str[255], 
    res_str[255], t_str[255];
  unsigned long tmp;
  struct FAB fab;
  struct NAM nam;
  static struct {unsigned short num, seq; unsigned char rvn, nmx;} f_id;
  unsigned fid_num, x;
  unsigned short chan;
  Boolean matdevice, matnoremove, matdir, matnolog, mattree, matstat;
  char deflt[] = ".DIR;1";
  $DESCRIPTOR(device_descr , device);
  $DESCRIPTOR(dummy_descr , dummy); 
  $DESCRIPTOR(dname_descr , dname);
  $DESCRIPTOR(item_descr,"FILE_ID");
  $DESCRIPTOR(terminal,"SYS$COMMAND");

/* Check the privileges */
  if (mask > -1) 
  { singlemsg(0, DFU_NOPRIV); return(SS$_NOPRIV);
  }

/* Delete directory ? */
  status = parse_item("directory", &dummy_descr, 0, &matdir, 2);
  status = parse_item("tree", &dummy_descr, 0, &mattree, 2);
  status = parse_item("statistics", &dummy_descr, 0, &matstat, 2);
/* Get device name or directory name */
  status = parse_item("device", &dname_descr, 0, &matdevice , 0);
  x = strindex(&dname," ",160); dname[x]= '\0';
  if (!matdir) 
  {
    strcpy(device,dname); device_descr.dsc$w_length=strlen(device);
  }
/* Nolog entered */
  status = parse_item("nolog", &dummy_descr, 0, &matnolog, 2);

  if (matstat) status = lib$init_timer(0);
/* Now follow different code path depending on the /DIRECTORY qualifier */
  if (!matdir)
  {
/* Remove from directory ? */
    status = parse_item("noremove", &dummy_descr, 0, &matnoremove, 2);

/* get the file id */
    if (CLI$PRESENT(&item_descr) != CLI$_PRESENT)
    { sprintf(outbuf,
       "%%DFU-E-NOFID, Missing /FILE_ID or /DIRECTORY qualifier on command line");
      put_disp();
      return(SS$_NOSUCHFILE);
    }
    dummy_descr.dsc$w_length = 7;
    status = CLI$GET_VALUE(&item_descr, &dummy_descr, &dummy_descr.dsc$w_length);
    tmp = atoi(&dummy);
    f_id.nmx = f_id.rvn = 0;
    f_id.num = (tmp % 65536);
    f_id.nmx = (tmp / 65536);
    dummy_descr.dsc$w_length = 7;
    status = CLI$GET_VALUE(&item_descr, &dummy_descr, &dummy_descr.dsc$w_length);
    if ((status & 1) != 1) 
    { sprintf(outbuf,"%%DFU-E-ERRFID, Incorrect File id on command line");
      put_disp();
      return(SS$_NOSUCHFILE);
    }
    tmp = atoi(&dummy);
    f_id.seq = tmp;
    dummy_descr.dsc$w_length = 7;
    status = CLI$GET_VALUE(&item_descr, &dummy_descr, &dummy_descr.dsc$w_length);
    if ((status & 1) != 1) 
    { sprintf(outbuf,"%%DFU-E-ERRFID, Incorrect File id on command line");
      put_disp();
      return(SS$_NOSUCHFILE);
    }
    tmp = atoi(&dummy);
    f_id.nmx = f_id.nmx + (tmp >> 8);
    f_id.rvn = (tmp % 255);
  
/* Parsing complete , now proceed to delete this file */

/* Open a channel to disk */
    status = SYS$ASSIGN(&device_descr, &chan, 0 , 0);
    if ((status & 1 ) != 1) 
    { singlemsg(DFU_ASSIGN,status); return(status);
    }
/* Now delete this file */
    status = delete_file(&f_id.num, chan, matnoremove, matdir, matnolog, FALSE);
    status = SYS$QIOW(0, chan, IO$_DEACCESS,0,0,0,0,0,0,0,0,0);
    status = SYS$DASSGN(chan);
  } /* File delete done */
   else
/* Delete all files in this directory */
  { ctrlc = 0;
    matnoremove = TRUE;

/* fill in FAB and NAM blocks*/

    fab = cc$rms_fab;
    fab.fab$l_dna = (char *) &deflt;
    fab.fab$b_dns = 6;
    fab.fab$l_fna = (char *) &dname;
    fab.fab$b_fns = strlen(dname);
    fab.fab$l_nam = &nam;
  
    nam = cc$rms_nam;
    nam.nam$b_rss = NAM$C_MAXRSS;
    nam.nam$l_rsa = (char *) &res_str;
    nam.nam$b_ess = NAM$C_MAXRSS;
    nam.nam$l_esa = (char *) &exp_str;
    
/* get the rest of the fab fields */
  
    status = SYS$PARSE(&fab , 0 , 0);
    if ((status & 1 ) != 1) 
    { sprintf(outbuf,"%%DFU-E-PARSEERR, Error parsing filename from %s,", dname);
      put_disp(); singlemsg(0,status); return(status);
    }
  
/* now search for the file */
    
    chan = 0;
    status = SYS$SEARCH(&fab , 0 , 0);
    while (status != RMS$_NMF)
    { res_str[nam.nam$b_rsl] = '\0';
      if ((status & 1 ) != 1) 
      { sprintf(outbuf,"%%DFU-E-SEARCHERR, Error searching file %s,", res_str);
        put_disp(); singlemsg(0,status); return(status);
      }
/* Open a channel to the device */
      if (chan == 0)
      { device_descr.dsc$w_length = nam.nam$t_dvi[0];
        strcpy(device,&nam.nam$t_dvi[1]);
/* Open a second channel to the disk */
        status = SYS$ASSIGN(&device_descr, &chan, 0 , 0);
        if ((status & 1 ) != 1) 
        { singlemsg(DFU_ASSIGN,status); return(status);
        }
      }
/* Now delete the tree first if needed */
      strcpy(t_str,res_str); status = 1;
      if (mattree) status = parse_tree(chan, (char *) &t_str, matnolog);
      if ((status & 1) ==1)
      { if (ctrlc != 1) 
        { sprintf(outbuf,"\n%%DFU-I-CLEANUP, Deleting %s...",res_str);
          put_disp();
          status = delete_directory(chan, &nam.nam$w_fid_num, 
            (char *) &res_str, matnolog);
        }
        if (ctrlc ==1) 
        { do_abort();
          status = SYS$QIOW(0, chan, IO$_DEACCESS,0,0,0,0,0,0,0,0,0);
          status = RMS$_NMF;
        }
         else status = SYS$SEARCH(&fab , 0 , 0);
      }
    }
    status = SYS$DASSGN(chan);
  }
  sprintf(outbuf,"\n%%DFU-I-READY, DELETE command ready");
  put_disp();
  if (matstat) status = lib$show_timer(0,0,display_stat,0);
}  

int defrag_command(mask)
/*
   Defrag files using the XQP MOVEFILE function.
*/

{ int x, y, flag;
  static char device[64], dummy_item[80], fname[86], version[8], 
    dfile[255], file[160];
  struct { short buflen, itemcode;
	    int *bufadr, *retlen;
          } item_list[2];
  Boolean matstat, matfile, matbesttry, matdevice, dummy, matwrite;
  FILE *ip;
  $DESCRIPTOR(fname_descr , fname);
  $DESCRIPTOR(device_descr , device);
  $DESCRIPTOR(dummy_descr , dummy_item); 
  $DESCRIPTOR(terminal,"SYS$COMMAND");

/* Check the privileges */
  if (mask > -1) 
  { singlemsg(0, DFU_NOPRIV); return(SS$_NOPRIV);
  }

  /* Set up itemlist for GETSYI */
  item_list[0].buflen = 8;
  item_list[0].itemcode = SYI$_NODE_SWVERS;
  item_list[0].bufadr = (int *) &version;
  item_list[0].retlen = &x;

  item_list[1].buflen = 0;
  item_list[1].itemcode = 0;

  status = SYS$GETSYIW(0,0,0,&item_list,0,0,0);
  if ((status & 1 ) !=1)
  { singlemsg(0,status); return(status);
  }
  x = strncmp("5.5", &version[1], 3);
  if ( x > 0 )
  { sprintf(outbuf,
      "%%DFU-E-ILLIOFUNC, This function not supported on this version of VMS");
    put_disp(); return(1);
  }

/* Get device name */
  status = parse_item("device", &device_descr, 0, &matdevice , 0);
  if (matdevice) 
  { if (strindex(&device,":",64) == -1) 
    { device[device_descr.dsc$w_length]=':'; 
      device_descr.dsc$w_length += 1;
    }
    device[device_descr.dsc$w_length]='\0';
  }
/* Display statistics */
  status = parse_item("statistics", &dummy_descr, 0, &matstat, 2);
/* Move to a LBN ? */
  status = parse_item("lbn", &dummy_descr, &lbn, &matlbn, 1);
/* Writechecking enable? */
  status = parse_item("writecheck", &dummy_descr, 0, &matwrite, 2);
/* Besttry contiguous ? */
  status = parse_item("besttry", &dummy_descr, 0, &matbesttry, 2);
  flag = matbesttry ? 1 : 0;
  if (matwrite) flag += 10;
/* Check file name */
  status = parse_item("file", &fname_descr, 0 , &matfile, 0);
  if (matfile == FALSE) 
  { sprintf(outbuf,"%%DFU-E-NOFILSPEC, No file specified on command line");
    put_disp(); return(SS$_NOSUCHFILE);
  }
  if (matstat) status = lib$init_timer(0);
  x = strindex(&fname," ",86); fname[x] = '\0';

  { sprintf(outbuf,"%%DFU-I-DEFRAG, Start defragmenting file(s) ...");
    put_disp();
    while((matfile) && (ctrlc ==0))/* Loop through the file list */
    { if (fname[0] == '@')
      { ip = fopen(&fname[1], "r");
        if (ip == 0) 
        { sprintf(outbuf,"%%DFU-E-OPENERR, Error opening file %s",&fname[1]);
          put_disp();
        }
         else
        { status = (int) fgets(file, 160, ip);
          while ((status != 0) && (ctrlc ==0))
          { x = strindex(&file," ",160); file[x] = '\0';
  	  if (matdevice)
            { strcpy(dfile,device); strncat(dfile,file,160); 
            }
             else
  	    strcpy(dfile,file);
            status = movefile(&dfile, flag);  
            status = (int) fgets(file, 160, ip);
          }
          fclose(ip);
        }
      }
       else
      { if (matdevice)
        { strcpy(dfile,device); strncat(dfile,fname,86);
        }
         else
          strcpy(dfile,fname);
        status = movefile(&dfile, flag);
      }
      fname_descr.dsc$w_length = 86;
      status = parse_item("file", &fname_descr, 0 , &matfile, 0);
      if ((status & 1) != 1) matfile = FALSE;
       else
      { x = strindex(&fname," ",86); fname[x] = '\0'; }

    } /*end while */
    if (ctrlc == 1) do_abort();
  }  
  sprintf(outbuf,"\n%%DFU-I-READY, DEFRAG command ready");
  put_disp();
  if (matstat) status = lib$show_timer(0,0,display_stat,0);
  return(1);
}

int move_to_lbn(unsigned short * from, unsigned int lbn_to, 
                unsigned short chan)
/*
   Called by movefile when defrag /lbn is used.
   Move a file to a specific LBN
   No placement pointer created in new header !
*/

{ static struct fibdef m_fib;
  struct {int fiblen; struct fibdef *fibadr;} fibdescr;
  unsigned int func;

  /* fill in FAB and NAM blocks */

  fibdescr.fiblen = sizeof(m_fib);
  fibdescr.fibadr = &m_fib;

  m_fib.fib$l_mov_svbn = 1;
  m_fib.fib$l_mov_vbncnt = 0;
  m_fib.fib$l_acctl = FIB$M_NOVERIFY | FIB$M_CHANGE_VOL; 
  func = (IO$_MODIFY | IO$M_MOVEFILE);
    
/* Now fill in File id and function code for the ACP call */

  copy_fid(&m_fib.fib$w_fid[0], from, TRUE);
  m_fib.fib$w_exctl = (FIB$M_ALCON + FIB$M_NOPLACE);
  m_fib.fib$b_alopts = FIB$M_EXACT;
  m_fib.fib$b_alalign = FIB$C_LBN;
  m_fib.fib$b_loc_rvn = 0;
  m_fib.fib$l_loc_addr = lbn_to;

/* Now defrag the file */
  status = SYS$QIOW(0, chan, func, &iostat, 0, 0,
    &fibdescr, 0,0,0,0,0);
  if ((status & 1) == 1) status = iostat.iosb_1;
  return(status);
}

int movefile(char *defr_file, int flag)
/*
   Performs the actual MOVEFILE function
   First $PARSE and $SEARCH the input file to get the full
   file spec and the File id for the move file function.
   We need to parse and search because wildcards are allowed.
	V2.2 : if LBN specified then call move_to_lbn
*/

{ struct FAB fab;
  struct NAM nam;
  static struct fibdef m_fib;
  struct {int fiblen; struct fibdef *fibadr;} fibdescr;
  static char res_str[255], exp_str[255], devnam[15];
  unsigned int func;
  unsigned short chan;
  $DESCRIPTOR(devnam_descr , devnam);

  /* fill in FAB and NAM blocks */

  fibdescr.fiblen = sizeof(m_fib);
  fibdescr.fibadr = &m_fib;

  fab = cc$rms_fab;
  fab.fab$l_fna = defr_file;
  fab.fab$b_fns = strlen(defr_file);
  fab.fab$l_nam = &nam;

  nam = cc$rms_nam;
  nam.nam$b_rss = NAM$C_MAXRSS;
  nam.nam$l_rsa = (char *) &res_str;
  nam.nam$b_ess = NAM$C_MAXRSS;
  nam.nam$l_esa = (char *) &exp_str;
  
/* get the rest of the fab fields */

  status = SYS$PARSE(&fab , 0 , 0);
  if ((status & 1 ) != 1) 
  { sprintf(outbuf,"%%DFU-E-PARSEERR, Error parsing filename from %s,", defr_file);
    put_disp(); singlemsg(0,status); return(status);
  }

  m_fib.fib$l_mov_svbn = 1;
  m_fib.fib$l_mov_vbncnt = 0;
  if (flag >= 10) 
  { m_fib.fib$l_acctl = 0; 
    flag -= 10;
  }
   else  /* disable writechecking */
    m_fib.fib$l_acctl = FIB$M_NOVERIFY; 
  m_fib.fib$l_acctl |= FIB$M_CHANGE_VOL; /* Allow vol.set moves */ 
  func = (IO$_MODIFY | IO$M_MOVEFILE);
/* now search for the file */
    
  status = SYS$SEARCH(&fab , 0 , 0);
  while (status != RMS$_NMF)
  { res_str[nam.nam$b_rsl] = '\0';
    if ((status & 1 ) != 1) 
    { sprintf(outbuf,"%%DFU-E-SEARCHERR, Error searching file %s,", res_str);
      put_disp(); singlemsg(0,status); return(status);
    }
    res_str[nam.nam$b_rsl] = '\0';
/* Open a channel to the device */
    devnam_descr.dsc$w_length = nam.nam$t_dvi[0];
    strcpy(devnam,&nam.nam$t_dvi[1]);
    status = SYS$ASSIGN(&devnam_descr, &chan, 0 , 0);
    if ((status & 1 ) != 1) 
    { singlemsg(DFU_ASSIGN,status); return(status);
    }
    if (matlbn)
    { status = move_to_lbn(&nam.nam$w_fid_num, lbn, chan);
    }
     else
    {
/* Now fill in File id and function code for the ACP call */
      m_fib.fib$w_fid[0] = nam.nam$w_fid_num;
      m_fib.fib$w_fid[1] = nam.nam$w_fid_seq;
      m_fib.fib$w_fid[2] = nam.nam$w_fid_rvn;
/* Contiguous or best_try ? */
      if (flag == 0)
        m_fib.fib$w_exctl = (FIB$M_ALCON);
       else
        m_fib.fib$w_exctl = (FIB$M_ALCONB);

/* Now defrag the file */
      status = SYS$QIOW(0, chan, func, &iostat, 0, 0,
         &fibdescr, 0,0,0,0,0);
      if ((status & 1) == 1) status = iostat.iosb_1;
    }
    if ((status & 1) != 1)
/* Report the result */
    { sprintf(outbuf,"%%DFU-E-NOTMOVED, Error moving %s, ",res_str);
      put_disp(); singlemsg(0,status);
    }
     else
    { sprintf(outbuf,"%%DFU-S-MOVED, File %s defragmented",res_str);
      put_disp();
    }
    status = SYS$DASSGN(chan);
/* Next file if any...*/
    status = SYS$SEARCH(&fab , 0 , 0);
  }  
  return(status);
}

int directory_command(mask)
/*
   Performs the directory command
   There are 3 major functions :
	- compress /dump a directory
        - create a directory with a specific preallocation
	- search all directories of the disk for either files with many
	    versions , files with aliases or empty directories,
	    or even corrupted directories (/check).
   Although called by the same command (DIRECTORY) the actual command
   processing is quite different. 
*/

{ static char fname[80], dname[160], dummy_item[80], device[64],
   dfile[255], file[160];
  int x, build_dir_table();
  FILE *ip;
  Boolean matstat, matcompress, dummy, matoutput, matdevice, 
          matcreate, matalloc, matratio;
  void create_dir();
  $DESCRIPTOR(dname_descr , dname);
  $DESCRIPTOR(dummy_descr , dummy_item); 
  $DESCRIPTOR(file_descr , fname);
  $DESCRIPTOR(device_descr , device); 
  $DESCRIPTOR(terminal,"SYS$COMMAND");

  dname[0] = '\0';
/* Get device name */
  status = parse_item("device", &device_descr, 0, &matdevice,0);
  if (matdevice)
  { if (strindex(&device,":",64) == -1) 
    { device[device_descr.dsc$w_length]=':'; 
      device_descr.dsc$w_length += 1;
    }
    device[device_descr.dsc$w_length]='\0';
  }
/* Check directory or device name */
  status = parse_item("directory", &dname_descr, 0 , &dummy, 0);
/* Dumping ? */
  status = parse_item("dump", &dummy_descr, 0, &matdump, 2);
/* Compressing ? */
  status = parse_item("compress", &dummy_descr, 0, &matcompress, 2);
/* Fill factor */
  status = parse_item("ratio", &dummy_descr, &ratio, &matratio, 1);
  if (!matratio) ratio = 100;
  if ( (ratio < 50) || (ratio > 100) )
  { sprintf(outbuf,"%%DFU-E-RATIOERR, fill factor out of range");
    put_disp(); return(1);
  } 
/* Creating ? */
  status = parse_item("create", &dummy_descr, 0, &matcreate, 2);
/* Just testing ? */
  mattest = 0;
  if (matcompress) 
  { status = parse_item("test", &dummy_descr, 0, &mattest, 2);
/* Truncate ? */
    status = parse_item("truncate", &dummy_descr, &truncsize, &mattruncate, 1);
    if ((status & 1) != 1) return(status);
  }
/* Display statistics */
  status = parse_item("statistics", &dummy_descr, 0, &matstat, 2);
/* Version search ? */
  status = parse_item("version", &dummy_descr, &version, &matversion, 1);
/* Allocate size  */
  if (matcreate) 
  { status = parse_item("allocate", &dummy_descr, &alloc, &matalloc, 1);
    if ((status & 1) != 1) return(status);
    if (!matalloc) alloc = 1; 
  }
/* Alias search */
  status = parse_item("alias", &dummy_descr, 0, &matalias, 2);
/* Check directories ?*/
  status = parse_item("check", &dummy_descr, 0, &matcheck, 2);
  status = parse_item("repair", &dummy_descr, 0, &matrepair, 2);
/* Empty directory search */
  status = parse_item("empty", &dummy_descr, 0, &matempty, 2);
/* Get output file */
  status = parse_item("outfile", &file_descr, 0, &matoutput , 0);
  if (matoutput == FALSE) 
    {strcpy(fname,"SYS$OUTPUT:"); file_descr.dsc$w_length = 11;}
   else
    {fname[file_descr.dsc$w_length] = '\0'; }
  fp = fopen(fname,"w");	/* Open output file */  

  if (matcompress)
/* Check the privileges */
    if (mask > -1) 
    { singlemsg(0, DFU_NOPRIV); return(SS$_NOPRIV);
    }

/* All qualifiers parsed */
  if (!matcreate && !matdump && !matcompress && 
      !matversion && !matalias && !matempty && !matcheck)
  { sprintf(outbuf,"%%DFU-E-SYNTAXERR, missing required qualifier");
    put_disp(); return(1);
  }

  x = strindex(&dname," ",160); dname[x] = '\0';
  if (matversion || matalias || matempty || matcheck) 
/* Follow different code path for this function */
  { /* Call build_dir_table. This function scans indexf.sys, builds 
	a directory table, and calls scan_directories */
    if (matstat) status = lib$init_timer(0);
    status = build_dir_table(&dname, matoutput);
    if (matstat) lib$show_timer(0,0,display_stat,0);
    fclose(fp);
    return(status); /*Thats all*/
  }
   else
  { if (matstat) status = lib$init_timer(0);
    if (matcreate) create_dir(&dname,alloc);
     else
    { if (dname[0] == '@')
      { ip = fopen(&dname[1],"r");
        if (ip==0) 
        { sprintf(outbuf,"%%DFU-E-OPENERR, Error opening file %s",&dname[1]);
          put_disp();
        }
         else
        { status = (int) fgets(file, 160, ip);
          while ((status != 0) && (ctrlc ==0))
          { x = strindex(&file," ",160); file[x] = '\0';
            if (matdevice)
            { strcpy(dfile,device); strncat(dfile,file,160); 
            }
             else
              strcpy(dfile,file);
            status = compdir(&dfile, matoutput);  
            status = (int) fgets(file, 160, ip);
          }
          fclose(ip);
        }
      }
       else
      { if (matdevice)
        { strcpy(dfile,device); strncat(dfile,fname,86);
        }
         else
          strcpy(dfile,dname);
        status = compdir(&dfile,matoutput);
      }
    }
  }
  sprintf(outbuf,"\n%%DFU-I-READY, DIRECTORY command ready");
  put_disp();
  if (matstat) status = lib$show_timer(0,0,display_stat,0);
  fclose(fp);
  return(1);
}

void create_dir (char *crea_file, int all_size)
/*
   Create a new directory with a preallocated size
*/
{ static struct fibdef fib;
  char res_str[255], exp_str[255];
  struct {int fiblen; struct fibdef *fibadr;} fibdescr;
  static int uchar;
  struct FAB fab;
  struct RAB rab;
  struct NAM nam;
  short i, channel, buf[256];
  static struct { unsigned short atr_size, atr_type;
                  void * atr_address; } 
    acb[2] =  { { ATR$S_UCHAR, ATR$C_UCHAR, (long) &uchar}, {0, 0, 0} };


  buf[0] = -1;
  fibdescr.fiblen = sizeof(fib);
  fibdescr.fibadr = &fib;
  fab = cc$rms_fab;
  fab.fab$l_fna = crea_file;
  fab.fab$b_fns = strlen(fab.fab$l_fna);
  fab.fab$l_dna = ".DIR;1";
  fab.fab$b_dns = strlen(fab.fab$l_dna);
  fab.fab$l_fop = FAB$M_CTG;
  fab.fab$b_rat = FAB$M_BLK;
  fab.fab$b_fac = FAB$M_BIO | FAB$M_PUT | FAB$M_GET ;
  fab.fab$w_mrs = 512;
  fab.fab$l_alq = all_size; 
  fab.fab$l_nam = 0;

  rab = cc$rms_rab;
  rab.rab$l_fab = &fab;
  rab.rab$l_rbf = (char *) buf;
  rab.rab$w_rsz = sizeof buf;

  nam = cc$rms_nam;
  nam.nam$b_rss = NAM$C_MAXRSS;
  nam.nam$l_rsa = (char *) &res_str;
  nam.nam$b_ess = NAM$C_MAXRSS;
  nam.nam$l_esa = (char *) &exp_str;
    
  for (i = 1; i <= 255; i++) buf[i] = 0;
  status=sys$create(&fab, 0, 0);
  if (status & 1) status = sys$connect(&rab, 0, 0);
  if (status & 1) status = sys$write(&rab, 0, 0);
  if (status & 1) status = sys$close(&fab, 0, 0);
  
  fab.fab$l_fop = FAB$M_UFO;
  fab.fab$l_nam = &nam;
  if (status & 1)  status = sys$open(&fab, 0, 0);
  channel = fab.fab$l_stv;
  uchar = FH2$M_DIRECTORY;
  fib.fib$l_exsz = 0;
  fib.fib$w_exctl = 0;
  fib.fib$w_fid[0] = fib.fib$w_fid[1] = fib.fib$w_fid[2] = 0;
  if (status & 1) 
   status = SYS$QIOW(0, channel, IO$_DEACCESS, &iostat, 0,0,
      &fibdescr, 0,0,0, &acb,0);
  if ((status & 1) == 1) status = iostat.iosb_1;
  if ((status & 1) != 1)
  { sprintf(outbuf, "%%DFU-E-NOTCREATE, Error creating directory %s,", crea_file);
    put_disp(); singlemsg(0,status);
  }
   else 
  { sprintf(outbuf,"%%DFU-S-CREATED, Created directory %.*s", 
     nam.nam$b_rsl, res_str);
    put_disp();
  }
}

int compdir (char *comp_file, Boolean matoutput)
/*
   Performs the actual directory compress or dump
   V2.2 : added the /FILL_FACTOR and /TRUNCATE options
*/

{ struct FAB fab;
  struct NAM nam;
  struct _d_e *dir_entry;
  struct _f_e *fid_entry;
  unsigned short chan, func, fid_num;
  Boolean found;
  static struct fibdef fib; 
  static short attrib[16];
  static unsigned int uchar;
  static struct { unsigned short atr_size, atr_type;
                  void * atr_address; } 
    acb[3] =  { {32, ATR$C_RECATTR, (long) &attrib}, 
                { 4, ATR$C_UCHAR, (long) &uchar} ,
                {0, 0, 0} };
  struct {int fiblen; struct fibdef *fibadr;} fibdescr;
  static char res_str[255], exp_str[255], devnam[15];
  int i, j, k, n, nr, newi, newj, vbn, e_blk, h_blk, oldeblk, lastsize, limit;
  char deflt[] = ".DIR;1";
  struct _quad {unsigned int address[2];} retadr , outadr;
  struct _sh {char block[512];} *i_dir, *o_dir; /* Pointer to shared memory */
  $DESCRIPTOR(devnam_descr , devnam);
 
/* fill in FAB , NAM and FIB blocks*/
  fibdescr.fiblen = sizeof(fib);
  fibdescr.fibadr = &fib;
  func = IO$_ACCESS | IO$M_ACCESS;
  
  fab = cc$rms_fab;
  fab.fab$l_dna = (char *) &deflt;
  fab.fab$b_dns = 6;
  fab.fab$l_fna = comp_file;
  fab.fab$b_fns = strlen(comp_file);
  fab.fab$l_nam = &nam;
  
  nam = cc$rms_nam;
  nam.nam$b_rss = NAM$C_MAXRSS;
  nam.nam$l_rsa = (char *) &res_str;
  nam.nam$b_ess = NAM$C_MAXRSS;
  nam.nam$l_esa = (char *) &exp_str;
   
/* get the rest of the fab fields */
  
  status = SYS$PARSE(&fab , 0 , 0);
  if ((status & 1 ) != 1) 
  { sprintf(outbuf,"%%DFU-E-PARSEERR, Error parsing filename from %s,", comp_file);
    put_disp(); singlemsg(0,status); return(status);
  }
  
/* now search for the file */
    
  chan = 0;
  lastsize = 0;
  status = SYS$SEARCH(&fab , 0 , 0);
  while (status != RMS$_NMF)
  { res_str[nam.nam$b_rsl] = '\0';
    if ((status & 1 ) != 1) 
    { sprintf(outbuf, "%%DFU-E-SEARCHERR, Error searching file %s,", res_str);
      put_disp(); singlemsg(0,status); return(status);
    }
/* Open a channel to the device */
    if (chan == 0)
    { devnam_descr.dsc$w_length = nam.nam$t_dvi[0];
      strcpy(devnam,&nam.nam$t_dvi[1]);
      status = SYS$ASSIGN(&devnam_descr, &chan, 0 , 0);
      if ((status & 1 ) != 1) 
      { singlemsg(DFU_ASSIGN,status); return(status);
      }
    }
/* Now fill in File id and function code for the ACP call */
    fib.fib$w_fid[0] = nam.nam$w_fid_num;
    fib.fib$w_fid[1] = nam.nam$w_fid_seq;
    fib.fib$w_fid[2] = nam.nam$w_fid_rvn;
    if (!mattest && !matdump) fib.fib$l_acctl = 
       FIB$M_WRITE | FIB$M_NOWRITE;
      else fib.fib$l_acctl = FIB$M_NOLOCK; 

/* Open the directory */
    status = SYS$QIOW(0, chan, func, &iostat,0,0,&fibdescr, 0,0,0, &acb,0);
    if ((status & 1 ) == 1) status = iostat.iosb_1;
    if ((status & 1) !=1)
    { sprintf(outbuf,"%%DFU-E-OPENERR, Error opening directory %s,",res_str);
      put_disp(); singlemsg(0,status); 
      goto next_dir;
    }
/* Get the directory size */
    e_blk = (attrib[4] << 16) + attrib[5];
    h_blk = (attrib[2] << 16) + attrib[3];
    oldeblk = e_blk - 1;

/* Check if directory is valid */
    if ( ((attrib[0] & 2048 ) != 2048) /*NOSPAN attribute */
      || ((uchar & 8192) != 8192) /* Directory bit */
      || ((uchar & 128) != 128) ) /* Contiguous bit */
    { sprintf(outbuf,"%%DFU-W-INVDIR, %s is not a valid directory",res_str);
      put_disp();
      goto next_dir;
    }
    if ( strstr(res_str,".DIR;1") == 0) 
    { sprintf(outbuf,"%%DFU-W-INVDIR, %s is not a valid directory",res_str);
      put_disp();
      goto next_dir;
    }
    lastsize = 0;
    if (e_blk > 1000) 
    { /* Allocate dynamic space to read the directory file */
      status = SYS$EXPREG(e_blk, &retadr.address[0], 0, 0);
      if ((status & 1) != 1)
      { sprintf(outbuf,"%%DFU-E-INSFMEM, Error allocating %d pages memory for %s",
         e_blk, res_str);
        put_disp();
        singlemsg(0,status); lastsize = 0;
        goto next_dir;
      }
       else lastsize = e_blk;
    }    
/* Allocate space for new directory (if needed) */
    outadr.address[0] = 0;
    if (ratio != 100)
    { status = SYS$EXPREG( h_blk, &outadr.address[0],0,0);
      if ((status & 1) != 1)              
      { sprintf(outbuf,"%%DFU-E-INSFMEM, Error allocating %d pages memory ",
         h_blk);
        put_disp();
        singlemsg(0,status);
        goto next_dir;
      }
   }

   limit = (512*ratio / 100); /* Our famous fill factor */

/* Now read the directory with 1 I/O */
    vbn = e_blk - 1;
/* Pointer to space for directory blocks */
    if (lastsize <= 1000)
      i_dir = (struct _sh *) &header[0].block[0];
     else
      i_dir = (struct _sh *) retadr.address[0];
    if (ratio !=100)
      o_dir = (struct _sh *) outadr.address[0];
     else
      o_dir = i_dir;
    status = SYS$QIOW(0, chan, IO$_READVBLK, &iostat, 0,0,
            i_dir, (vbn*512), 1, 0,0,0);
    if ((status & 1 ) == 1) status = iostat.iosb_1;
    if ((status & 1) !=1)
    { sprintf(outbuf,"%%DFU-E-READERR, Error reading directory %s,",res_str);
      put_disp(); singlemsg(0,status); 
      goto next_dir;
    }
/* Check for corrupted directory */
    if (i_dir->block[0] == 0)
    { sprintf(outbuf,"%%DFU-E-JUNKDIR, Corrupted directory (byte offset 0) %s",
        res_str);
      put_disp();
      goto next_dir;
    }
   
/* If just testing we can release the directory now */
    if (mattest || matdump) 
      status = SYS$QIOW(0, chan, IO$_DEACCESS,0,0,0,0,0,0,0,0,0);
 
/* Now processs the directory blocks */
    newi = 1; newj = 1; k = 1; n = 0;
    while (k < e_blk)
    { i = 1; 
      found = FALSE;   
      if (matdump) 
      { sprintf(outbuf,"DUMP of directory block %d",k);
        put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
      }
      while (!found)
      { if ((i > 512) || i_dir->block[i-1] == -1) found = TRUE; /* End of block */
         else /* parse this entry */
        { dir_entry = (struct _d_e *) &i_dir->block[i-1]; 
/* Pointer to directory record */
          nr = (dir_entry->size - 4 - dir_entry->count) / 8; /* # of files entries */ 
          n += nr;
          j = i + dir_entry->size + 2; /* End address of this entry */

/* Make sure we can put this entry in a block */
          if ((dir_entry->size + 2) > limit)
	  { sprintf(outbuf,
     "%%DFU-E-LOWFILL, Directory entry too large, choose a higher fill factor");
	    put_disp(); goto next_dir;
          }
/* If /dump entered display this entry */
          if (matdump)
          { sprintf(outbuf,
              "Size: %d, Version limit: %d, Type: %d, Name(%d): %.*s",
              dir_entry->size, dir_entry->versionlim,
              dir_entry->flags, dir_entry->count, 
              dir_entry->count, dir_entry->name);
            put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
/* Align count field */
            if ((dir_entry->count &1) ==1) dir_entry->count +=1;
            fid_entry = (struct _f_e *) &i_dir->block[i + 5 + dir_entry->count];
            while (nr >= 1)
            { fid_num = (fid_entry->fid_nmx << 16) + fid_entry->fid_num;
              sprintf(outbuf,"   Version: %d, FID : (%d,%d,%d)",
              fid_entry->version, fid_num, fid_entry->fid_seq, 
              fid_entry->fid_rvn);
              put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
              nr--; fid_entry++;
            }
          }

/* Fill in new directory */
          if ((newj + j - i) >= limit) /* Check if we reached  block bounds */
          { o_dir->block[newj-1] = -1; /* Mark end of block */
            o_dir->block[newj] = -1;
            newj = 1; newi += 1; o_dir += 1; /* Next block */
          }
/* Test if we didn't have an impossible fill_factor */
	  if (newi > h_blk )
          { sprintf(outbuf,"%%DFU-E-EXTERR, %s cannot be extended,",res_str);
            put_disp(); 
	    sprintf(outbuf," Choose a higher fill factor"); put_disp();
            goto next_dir;
          }
          memmove(&o_dir->block[newj-1], dir_entry, (j-i));
          newj += (j-i);
          i = j; /* Pointer to next entry */
        } /* endif*/
      } /* end while */
      k += 1; i_dir += 1; /* Next block */
      if (matdump) 
      { sprintf(outbuf," ");
        put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
      }
      if (matdump && (ctrlc == 1)) goto next_dir;
    } /* end 'k' loop */
/* Mark end of block */
    o_dir->block[newj-1] = -1; 
    o_dir->block[newj] = -1;
 /* Clear the rest of the bits */
    for (j = (newj+2); j <= 512; j++) o_dir->block[j-1] = 0;
    if ((!mattest) && (!matdump))
/* Rewrite the new , compressed directory */
    { e_blk = newi + 1;
      attrib[4] = e_blk >> 16;
      attrib[5] = e_blk - (attrib[4] << 16);
      vbn = e_blk - 1;
      if (ratio != 100)
        o_dir = (struct _sh *) outadr.address[0];
       else 
        { if (lastsize <= 1000)
	  o_dir = (struct _sh *) &header[0].block[0];
	   else
          o_dir = (struct _sh *) retadr.address[0];
        }
      status = SYS$QIOW(0, chan, IO$_WRITEVBLK, &iostat,0,0,
        o_dir, (vbn*512), 1, 0,0,0);
      if ((status & 1 ) == 1) status = iostat.iosb_1;
      if ((status & 1) !=1)
      { sprintf(outbuf,"%%DFU-E-WRITEERR, Error writing directory %s,",res_str);
        put_disp(); singlemsg(0,status); 
        goto next_dir;
      }
/* Write the new attributes back , truncate and close the file */
      fib.fib$l_exsz = 0;
      if (mattruncate) 
      { fib.fib$w_exctl = FIB$M_TRUNC;
	if ( (truncsize == 0) || (truncsize < e_blk) )
          fib.fib$l_exvbn = e_blk;
	 else fib.fib$l_exvbn = truncsize;
      }
       else
      { fib.fib$w_exctl = 0;
        fib.fib$l_exvbn = 0;
      }
      status = SYS$QIOW(0, chan, IO$_DEACCESS, &iostat, 0,0,
        &fibdescr, 0,0,0, &acb,0);
      if (mattruncate) e_blk = fib.fib$l_exvbn - 1;
        else e_blk = h_blk;
    }
    if (mattest)
      sprintf(outbuf,
        "%%DFU-I-TESTONLY, %s : %d files; old size : %d/%d, new : %d blocks",
        res_str, n, oldeblk, h_blk, newi);
     else 
      if (matdump)
        sprintf(outbuf,"%%DFU-I-TOTAL, %s : %d files", res_str, n);
       else
        sprintf(outbuf,
          "%%DFU-S-DONE, %s : %d files; was : %d/%d, now : %d/%d blocks",
          res_str, n, oldeblk, h_blk, newi, e_blk);
    put_disp();

/* Next file if any...*/
next_dir: 
    SYS$QIOW(0, chan, IO$_DEACCESS,0,0,0,0,0,0,0,0,0);
    if (lastsize !=0)
      SYS$DELTVA(&retadr.address[0], 0 , 0);
    if (outadr.address[0] !=0)
      SYS$DELTVA(&outadr.address[0], 0, 0);
    if (ctrlc == 1) status = RMS$_NMF;
      else status = SYS$SEARCH(&fab , 0 , 0);
  }
  status = SYS$DASSGN(chan);

  if (ctrlc == 1) do_abort();
  return(1);
}

int scan_directories(int *rvt, int rvn, int cnt, int max, Boolean matoutput)
/* Scan all directories found on this disk. Implicitly called
     by the DIR/VERSION , /ALIAS , /CHECK or DIR/EMPTY command. 
     RVT is the relative volume table which contains the adresses 
     for the dynamic array and the device channel. 
*/

{ 
  struct _rv 
  { /* Structure for relative volume table */
    char devnam[64], fulldevnam[64]; 
    unsigned int devnam_len,fulldevnam_len, i_open,vbn_file_1,if_size,maxblocks;
    unsigned short channel, bchan;
    Boolean wlk, slow;
    int addr[2], bmap_addr[2]; /* For EXPREG calls */
  } *rvt_t;
  struct _t { unsigned short fid[3]; unsigned int bakfid, a_size; 
             char rvn, bitje; } *dyn_array, *temp;
  struct _d_e *dir_entry;
  static struct _f_e *fid_entry, did;
  unsigned int lastsize, progress, totsize;
  struct _quad {unsigned int address[2];} retadr;
  struct _sh {char block[512];} *i_dir; /* Pointer to shared memory */
  char *tmp;
  static struct fibdef fib; 
  struct {int fiblen; struct fibdef *fibadr;} fibdescr;
  static short attrib[16];
  static struct { short lendir;
	 char dirnam[198]; } dirrec, filrec;
  static struct { unsigned short atr_size, atr_type;
                  void * atr_address; } 
    acb[3] =  { {32, ATR$C_RECATTR, (long) &attrib}, 
                {200, ATR$C_FILE_SPEC, (long) &dirrec}, {0, 0, 0} },
    acb_fil[3] =  { {6, ATR$C_BACKLINK, (long) &did.fid_num}, 
                {200, ATR$C_FILE_SPEC, (long) &filrec}, {0, 0, 0} };
  int i, j, k, n, nr, newi, x, vbn, e_blk, maxi, func, dir_cnt;
  static char devnam[64], savnam[86], oldnam[86], fulldevnam[64];
  unsigned int fid, bakfid, acp_stat, xrvn;
  register int l;
  Boolean found, flag, getdir;
  $DESCRIPTOR(devnam_descr , devnam);

  lastsize = 0; 
  rvt_t = (struct _rv *) rvt; /* Pointer to real relative volume table */
  strcpy(devnam,(rvt_t+rvn)->devnam); 
  strcpy(fulldevnam, &((rvt_t+rvn)->fulldevnam[1]) );
  devnam_descr.dsc$w_length = (rvt_t+rvn)->devnam_len;
  dyn_array = (struct _t *) (rvt_t+rvn)->addr[0]; /* Pointer to dynamic array */      
  maxi = (dyn_array->fid[1] << 16) + dyn_array->fid[0]; /* Highest found file header */  
  fibdescr.fiblen = sizeof(fib);
  fibdescr.fibadr = &fib;
  func = IO$_ACCESS | IO$M_ACCESS;
  fib.fib$l_acctl = FIB$M_NOLOCK;
/* Scan the array */
  l = 1; dir_cnt = 0;
  while ((l <= maxi) && (ctrlc !=1))
  { if (( (dyn_array+l)->bitje & 4) != 4) goto next;/* Not a directory */
/* Now fill in File id and function code for the ACP call */
    fib.fib$w_fid[0] = (dyn_array+l)->fid[0];
    fib.fib$w_fid[1] = (dyn_array+l)->fid[1];
    fib.fib$w_fid[2] = (dyn_array+l)->fid[2];
    getdir = FALSE;

/* Open the directory */
    status = SYS$QIOW(0, (rvt_t+rvn)->channel, func, &iostat,0,0,&fibdescr, 
      0,0,0,&acb,0);
    if ((status & 1 ) == 1) status = iostat.iosb_1;
    if ((status & 1) !=1) goto next;
  
/* Get the directory size */
    e_blk = (attrib[4] << 16) + attrib[5]; 
    lastsize = 0;
    if (e_blk > 1000) 
    { /* Allocate dynamic space to read the directory file */
      status = SYS$EXPREG(e_blk, &retadr.address[0], 0, 0);
      if ((status & 1) != 1)
      { lastsize = 0;
        SYS$QIOW(0, (rvt_t+rvn)->channel, IO$_DEACCESS,0,0,0,0,0,0,0,0,0);
        goto next;
      }
       else lastsize = e_blk;
    }    

/* Now read the directory with 1 I/O */
    vbn = e_blk -1 ;
/* Pointer to space for directory blocks (1000 max) */
    if (e_blk <= 1000)
      i_dir = (struct _sh *) &header[0].block[0];
     else
      i_dir = (struct _sh *) retadr.address[0];
    status = SYS$QIOW(0, (rvt_t+rvn)->channel, IO$_READVBLK, &iostat, 0,0,
      i_dir, (vbn*512), 1, 0,0,0);
    SYS$QIOW(0, (rvt_t+rvn)->channel, IO$_DEACCESS,0,0,0,0,0,0,0,0,0);
    if ((status & 1 ) == 1) status = iostat.iosb_1;
    if ((status & 1) !=1) goto next;

/* Rearrange directory name */
    x = strindex(&dirrec.dirnam,"[",198); 
    strncpy(dirrec.dirnam,&dirrec.dirnam[x],(198-x));
    tmp = (char *) strstr(dirrec.dirnam,".DIR;"); *tmp = ']';
    tmp = (char *) strstr(dirrec.dirnam,"]"); *tmp = '.';
    if (strstr(dirrec.dirnam,"[000000") != 0)
    { dirrec.dirnam[7] = '[';
      strcpy(dirrec.dirnam,&dirrec.dirnam[7]);
    }
    x = strindex(&dirrec.dirnam,"]",198); dirrec.dirnam[x+1] = '\0';
  
    if ( (dir_cnt + cnt) % 10 == 0 )
    { progress = 100 * (dir_cnt + cnt) / max;
      sprintf(outbuf,"     Progress : %3d%%     Status : Scanning directories",
        progress);
      put_status(1);
    }
    strcpy(oldnam,"\0");
    dir_cnt++;
/* Now processs the directory blocks */
    if (i_dir->block[0] == -1 ) /* Empty dir */
    { if (matempty) 
      { sprintf(outbuf,"%s%s is empty ", fulldevnam, &dirrec.dirnam);
        put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
      }
      goto next;
    } 
/* If we are searching for empty directories ONLY skip the next code */
    if (matempty && (!matalias && !matversion && !matcheck)) goto next;
    newi = 1; k = 1; n = 0; flag = FALSE;
    while (k < e_blk)
    { /* Check first for corrupted directory otherwise we might ACCVIO */
      if (i_dir->block[0] == 0) 
      { sprintf(outbuf,"Directory corrupt %s%s first byte , block %d ",
         fulldevnam, &dirrec.dirnam,k);
        put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
        goto next;
      }
      i = 1; 
      found = FALSE;   
      while (!found)
      { if ((i > 512) || i_dir->block[i-1] == -1) found = TRUE; /* End of block */
         else /* parse this entry */
        { /* Pointer to directory record */
          dir_entry = (struct _d_e *) &i_dir->block[i-1]; 
          j = i + dir_entry->size + 2; /* End address of this entry */
/* Align count field */
          if ((dir_entry->count & 1) ==1) dir_entry->count +=1;

/* CHECK code starts here...*/
	  if (matcheck)
          { x = strncmp(&oldnam, dir_entry->name, dir_entry->count);
            strncpy(oldnam, dir_entry->name, dir_entry->count);
            oldnam[dir_entry->count] = '\0';
            if (x > 0) 
            { sprintf(outbuf,"%s%s%s out of alphabetical order",
                         fulldevnam, &dirrec.dirnam, &oldnam);
              put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
            }
            n = (dir_entry->size - 4 - dir_entry->count) / 8; 
/* Now check if we have a valid file header */
            fid_entry = (struct _f_e *) &i_dir->block[i + 5 + dir_entry->count];
	    while (n > 0)
            { fid = (fid_entry->fid_nmx << 16) + fid_entry->fid_num;
/* Check if RVN on other disk (possible only in volume sets) */
	      if ( (fid_entry->fid_rvn != 0 ) && (fid_entry->fid_rvn != rvn) )
              { xrvn = fid_entry->fid_rvn;
                temp = (struct _t *) (rvt_t+xrvn)->addr[0]; /* Pointer to other table */
                temp = (temp+fid);
              }
               else
              { xrvn = rvn;
	        temp = (struct _t *) (dyn_array+fid);
              }
              if ( (temp->bitje & 1) != 1) /* No valid file header */
              { sprintf(outbuf,"%s%s%.*s;%d has no valid file header",
                fulldevnam, &dirrec.dirnam, dir_entry->count, 
                dir_entry->name,fid_entry->version);
	        put_disp();
	        if (matrepair) /* Remove this entry */
	        { status = remove_file(&fid_entry->fid_num, &fib.fib$w_fid[0],
                  (rvt_t+xrvn)->channel);
		  if ((status & 1) != 1) 
	          { sprintf(outbuf,"%%DFU-E-NOREMOVE, error removing %.*s",
                     dir_entry->count,dir_entry->name); 
                     put_disp(); singlemsg(0,status);
                  }
	           else
	          { sprintf(outbuf,"%DFU-S-REMOVED, file %.*s removed",
                     dir_entry->count,dir_entry->name); put_disp();
                  }
                }
              } 
               else
              if ( temp->fid[1] != fid_entry->fid_seq) /*Invalid seq*/
              { sprintf(outbuf,"%s%s%.*s;%d invalid file sequence number",
                fulldevnam, &dirrec.dirnam, dir_entry->count, 
                dir_entry->name,fid_entry->version);
	        put_disp();
              } 
	      n--; fid_entry++;
            }
	  } /* End of /CHECK */

/* VERSION code starts here...*/
	  if (matversion)
          { if (!flag) totsize = nr = 0;
             else 
              { x = strncmp(&savnam, dir_entry->name, dir_entry->count);
                if (x != 0) /* report previous entry */
                { if (nr >= version)  
   	          { sprintf(outbuf,"%s%s%s , %d versions , %d blocks",
                     fulldevnam, &dirrec.dirnam, &savnam, nr, totsize);
                    put_disp(); 
                    if (matoutput) fprintf(fp,"%s\n",outbuf);
                  }
                  totsize = nr = 0;
                }
              }
/* # of files entries */ 
            n = (dir_entry->size - 4 - dir_entry->count) / 8; 
            nr += n;
/* Count the size of the individual files (was saved in the
   dynamic array) */
            fid_entry = (struct _f_e *) &i_dir->block[i + 5 + dir_entry->count];
	    while (n > 0)
            { fid = (fid_entry->fid_nmx << 16) + fid_entry->fid_num;
/* Check if RVN on other disk (possible only in volume sets) */
	      if ( (fid_entry->fid_rvn != 0 ) && (fid_entry->fid_rvn != rvn) )
              { xrvn = fid_entry->fid_rvn;
                temp = (struct _t *) (rvt_t+xrvn)->addr[0]; /* Pointer to other table */
                temp = (temp+fid);
              }
               else
              { xrvn = rvn;
	        temp = (struct _t *) (dyn_array+fid);
              }
              totsize += temp->a_size;
	      n--; fid_entry++;
            }
	    if ((j > 512) || i_dir->block[j-1] == -1) flag = TRUE;
              else flag = FALSE;
/* Save name if last entry in this block . We must do this
   because directory entries can span blocks */
	    if (flag) 
            { strncpy(savnam, dir_entry->name, dir_entry->count);
              savnam[dir_entry->count] = '\0';
            }
	    if ((nr >= version) && (!flag))
/* Report this file */
   	    { sprintf(outbuf,"%s%s%.*s , %d versions , %d blocks",
                fulldevnam, &dirrec.dirnam, dir_entry->count, 
                dir_entry->name, nr, totsize);
              put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
	      totsize = nr = 0;
            }
          } /* End version check */

/* ALIAS code starts here */
	  if (matalias)
/* Setup fid pointer for alias check */
          { fid_entry = (struct _f_e *) &i_dir->block[i + 5 + dir_entry->count];
            n = (dir_entry->size - 4 - dir_entry->count) / 8; 
            while (n > 0)
            { fid = (fid_entry->fid_nmx << 16) + fid_entry->fid_num;
/* Check if RVN on other disk (possible only in volume sets) */
	      if ( (fid_entry->fid_rvn != 0 ) && (fid_entry->fid_rvn != rvn) )
              { xrvn = fid_entry->fid_rvn;
                temp = (struct _t *) (rvt_t+xrvn)->addr[0]; /* Pointer to other table */
                temp = (temp+fid);
              }
               else
              { xrvn = rvn;
	        temp = (struct _t *) (dyn_array+fid);
              }
              bakfid = temp->bakfid;
              if ((bakfid !=l) && (bakfid !=0))
/* Set up FIB to do a pseudo FID_TO_NAME call */
	      { fib.fib$w_fid[0] = fid_entry->fid_num;
                fib.fib$w_fid[1] = fid_entry->fid_seq;
                fib.fib$w_fid[2] = 
                  (fid_entry->fid_nmx << 8) + fid_entry->fid_rvn;
                status = SYS$QIOW(0, (rvt_t+xrvn)->channel, IO$_ACCESS, 
                  &iostat,0,0,&fibdescr, 0,0,0, &acb_fil,0);
                if ((status & 1 ) == 1) status = iostat.iosb_1;
		if ((status & 1) != 1)
	          singlemsg(0,status);
	         else
/* Check the directory backlink found fot this file. We must do this
   because the file header may have been changed since we set up
   the dynamic array (because of concurrent disk activity). */
	          { if ( ((did.fid_nmx << 16) + did.fid_num) != l)
                    { x = strindex(&filrec.dirnam,"[",198); 
                      strncpy(filrec.dirnam,&filrec.dirnam[x],(198-x));
                      sprintf(outbuf,"%s%s%.*s;%d is alias for %s%s",
                       fulldevnam, &dirrec.dirnam, dir_entry->count, 
                       dir_entry->name,fid_entry->version, 
                       fulldevnam, &filrec.dirnam);
                      put_disp(); 
                      if (matoutput) fprintf(fp,"%s\n",outbuf);
                    }
                  }
              }  
              n--; fid_entry++;
            }
          } /* End Alias check */

          i = j; /* Pointer to next entry */
        } /* endif*/
      } /* end while */
      k += 1; 
      i_dir += 1; /* Next block */
      if (ctrlc == 1) goto next;
    } /* end 'k' loop */

/* Check if we have one unreported file left */
    if ((flag) && (nr >= version))
    { flag = FALSE;
/* Report this file */
      sprintf(outbuf,"%s%s%.*s , %d versions , %d blocks",
        fulldevnam, &dirrec.dirnam, dir_entry->count, 
        dir_entry->name, nr, totsize);
      put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
    }

next: 
    l++; /* Next file */
    if (lastsize !=0)
      SYS$DELTVA(&retadr.address[0], 0 , 0);
  }
  return(dir_cnt);
}
