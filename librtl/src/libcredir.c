// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<mytypes.h>

#include<atrdef.h>
#include<descrip.h>
#include<fabdef.h>
#include<fatdef.h>
#include<fibdef.h>
#include<iodef.h>
#include<namdef.h>
#include<ssdef.h>
#include<starlet.h>

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
