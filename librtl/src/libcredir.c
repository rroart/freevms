// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<mytypes.h>

#include<fiddef.h>
#include<atrdef.h>
#include<descrip.h>
#include<fabdef.h>
#include<fatdef.h>
#include<fibdef.h>
#include<iodef.h>
#include<namdef.h>
#include<ssdef.h>
#include<starlet.h>
#include<rms.h>
#include<uicdef.h>
#include<fh2def.h>
#include <misc.h> 

#include<stdio.h>
#include<string.h>

// change to w_[df]id_rvn from b_ later

unsigned long lib$create_dir(const void * v) {
  struct dsc$descriptor * d = v;
  int sts;
  int chan;
  struct dsc$descriptor devnam;
  struct _fabdef fab = cc$rms_fab;
  struct _namdef nam = cc$rms_nam;
  char res[NAM$C_MAXRSS + 1],rsa[NAM$C_MAXRSS + 1];
  struct _iosb iosb;
  struct dsc$descriptor fibdsc;
  struct _fibdef fib;
  struct _atrdef atr[3];
  struct _fatdef recattr;
  unsigned long filechar;
  char emptybuffer[512];

  atr[0].atr$w_type=ATR$C_RECATTR;
  atr[0].atr$w_size=ATR$S_RECATTR;
  atr[0].atr$l_addr=&recattr;
  atr[1].atr$w_type=ATR$C_UCHAR;
  atr[1].atr$w_size=ATR$S_UCHAR;
  atr[1].atr$l_addr=&filechar;
  atr[2].atr$w_type=0;

  nam.nam$l_esa = res;
  nam.nam$b_ess = NAM$C_MAXRSS;

  fab.fab$l_nam = &nam;
  fab.fab$l_fna = d->dsc$a_pointer;
  fab.fab$b_fns = d->dsc$w_length;

  sts = sys$parse(&fab,0,0);

  devnam.dsc$a_pointer=nam.nam$l_dev;
  devnam.dsc$w_length=nam.nam$b_dev;

  sts = sys$assign(&devnam, &chan, 0, 0, 0);

  fibdsc.dsc$w_length=sizeof(struct _fibdef);
  fibdsc.dsc$a_pointer=&fib;

  fib.fib$w_fid_num = 4;
  fib.fib$w_fid_seq = 4;
  fib.fib$l_acctl = 0; // ?

  fib.fib$w_did_num = fib.fib$w_fid_num;
  fib.fib$w_did_seq = fib.fib$w_fid_seq;
  fib.fib$b_did_rvn = fib.fib$b_fid_rvn;

  sts = sys$qiow(0,chan,IO$_ACCESS,&iosb,0,0,
		 &fibdsc,v,0,0,0,0);
  sts = iosb.iosb$w_status;

  fib.fib$w_fid_num = fib.fib$w_did_num;
  fib.fib$w_fid_seq = fib.fib$w_did_seq;
  fib.fib$b_fid_rvn = fib.fib$b_did_rvn;
  fib.fib$w_did_num = 0;
  fib.fib$w_did_seq = 0;
  fib.fib$b_did_rvn = 0;

  sts = sys$qiow(0,chan,IO$_ACCESS,&iosb,0,0,
		 &fibdsc,0,0,0,&atr,0);
  sts = iosb.iosb$w_status;

  fib.fib$w_did_num = fib.fib$w_fid_num;
  fib.fib$w_did_seq = fib.fib$w_fid_seq;
  fib.fib$b_did_rvn = fib.fib$b_fid_rvn;

  sts = sys$qiow(0,chan,IO$M_ACCESS|IO$M_CREATE|IO$_CREATE,&iosb,0,0,
		 &fibdsc,v,0,0,&atr,0);
  sts = iosb.iosb$w_status;

  memset(emptybuffer,255,512);

  sts = sys$qiow(0,chan,IO$_WRITEVBLK,&iosb,0,0,
		 emptybuffer,512,1,0,0,0);
  sts = iosb.iosb$w_status;

  // and a deaccess later

  return SS$_NORMAL;
}

#define SYS$QIOW sys$qiow
#define FAB _fabdef
#define NAM _namdef
#define RAB _rabdef
#define fibdef _fibdef

unsigned long lib$create_dir2 (char *crea_file, int all_size)
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
static struct {unsigned short iosb_1; 
  unsigned int length; short spec; 
  } iostat;
  static struct { unsigned short atr_size, atr_type;
                  void * atr_address; } 
    acb[2] =  { { ATR$S_UCHAR, ATR$C_UCHAR, &uchar}, {0, 0, 0} };


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
  int status=sys$create(&fab,0,0);
  if (status & 1) status = sys$connect(&rab,0,0);
  rab.rab$w_rsz = sizeof buf; // workaround, $connect 0s this, should it?
  if (status & 1) status = sys$put /* not yet write*/(&rab,0,0);
  if (status & 1) status = sys$close(&fab,0,0);
  
  fab.fab$l_fop = FAB$M_UFO;
  fab.fab$l_nam = &nam;
  if (status & 1)  status = sys$open(&fab,0,0);
  channel = fab.fab$l_stv;
  uchar = FH2$M_DIRECTORY;
  fib.fib$l_exsz = 0;
  fib.fib$w_exctl = 0;
  //fib.fib$w_fid[0] = fib.fib$w_fid[1] = fib.fib$w_fid[2] = 0;
  memset(&fib.fib$w_fid_num,0,6);

  if (status & 1) 
    status = SYS$QIOW(0, channel, IO$_MODIFY /* can not yet handle IO$_DEACCESS */, &iostat, 0,0,
      &fibdescr, 0,0,0, &acb,0);
  if ((status & 1) == 1) status = iostat.iosb_1;
  if ((status & 1) != 1)
  { printf("%%CREATE-E-NOTCREATE, Error creating directory %s,", crea_file);
  }
   else 
  { printf("%%CREATE-S-CREATED, Created directory %.*s", 
     nam.nam$b_rsl, res_str);
  }
  return SS$_NORMAL;
}
