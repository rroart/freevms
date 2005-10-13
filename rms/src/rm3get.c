// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/config.h>
#include<linux/linkage.h>
#include<linux/string.h>

//#include <stdio.h>
#include <linux/ctype.h>

#include <mytypes.h>
#include <aqbdef.h>
#include <atrdef.h>
#include <fatdef.h>
#include <iodef.h>
#include <iosbdef.h>
#include <vcbdef.h>
#include <wcbdef.h>
#include <descrip.h>
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
#include <keydef.h>
#include <bktdef.h>
#include <ircdef.h>
#include <ifbdef.h>

//#include <rms.h>
#include "cache.h"
#include "access.h"
#include "direct.h"

#include "rmsmisc.h"

unsigned long inline buffer_offset(unsigned long buffer, unsigned long offset) {
  return buffer+offset;
}

unsigned short get_buffer_long(unsigned long buffer, unsigned long offset) {
  return *(unsigned long*)(buffer+offset);
}

unsigned short get_buffer_word(unsigned long buffer, unsigned long offset) {
  return *(unsigned short*)(buffer+offset);
}

unsigned char get_buffer_byte(unsigned long buffer, unsigned long offset) {
  return *(unsigned char*)(buffer+offset);
}

int mymax(int x, int y) {
  if (x>y) return x;
  else return y;
}

int mymin(int x, int y) {
  if (x<y) return x;
  else return y;
}

int primkeystrlen(unsigned char *c,int max) {
  int ret=0;
  while (max && *c) {
    ret++;
    c++;
    max--;
  }
  return ret;
}

int rms$get_idx(struct _rabdef * rab) {
  int rootvbn, datavbn, sts, block, offset = 0;
  char buffer[512];
  struct _iosb iosb={0};
  struct WCCFILE * wccfile = ifi_table[rab->rab$l_fab->fab$w_ifi];
  struct _xabkeydef * xabkey = wccfile->xab;
  struct _bkt * bkt;
  unsigned long index, nextindex;
  unsigned long vbn;
  unsigned long recordid;
  unsigned long keylen,primkeylen;
  unsigned long rest, key, restlen;
  int count;
  struct _atrdef atr[2];
  struct _fatdef recattr;
  struct dsc$descriptor fibdsc;

  atr[0].atr$w_type=ATR$C_RECATTR;
  atr[0].atr$w_size=ATR$S_RECATTR;
  atr[0].atr$l_addr=&recattr;
  atr[1].atr$w_type=0;
  fibdsc.dsc$w_length=sizeof(struct _fibdef);
  fibdsc.dsc$a_pointer=&ifi_table[rab->rab$l_fab->fab$w_ifi]->wcf_fib;
  sts = sys$qiow(RMS_EF,ifb_table[rab->rab$l_fab->fab$w_ifi]->ifb$w_chnl,IO$_ACCESS|IO$M_ACCESS,&iosb,0,0,
		 &fibdsc,0,0,0,atr,0);
  sts = iosb.iosb$w_status;

  while(xabkey->xab$l_nxt) {
    if (xabkey->xab$b_cod==XAB$C_KEY)
      break;
    xabkey=xabkey->xab$l_nxt;
  }
  //  rootvbn=xabkey->xab$l_rvb;
  rootvbn=xabkey->xab$l_rvb_l + (xabkey->xab$l_rvb_h<<16);
  datavbn=xabkey->xab$l_dvb;

  block=datavbn;

  sts = sys$qiow(RMS_EF,ifb_table[rab->rab$l_fab->fab$w_ifi]->ifb$w_chnl,IO$_READVBLK,&iosb,0,0,
		 buffer,512,block,0,0,0);
  sts = iosb.iosb$w_status;

  bkt = buffer_offset(buffer,offset);
  offset+=14; // sizeof(struct _bkt) should be 14, but is 16

  index = buffer_offset(buffer,offset);

  while (get_buffer_long(index,5)) {

    recordid = get_buffer_word(index,1);
    vbn = get_buffer_long(index,5);
    nextindex = index + get_buffer_word(index, 9) + 9 /* offset? */ + 2 /* word */;
    keylen = get_buffer_word(index,11);
    key = buffer_offset(index, 13);
    rest = buffer_offset(key,keylen);
    restlen = get_buffer_word(rest, 0);
    primkeylen=primkeystrlen(key,keylen);

    count=mymin(primkeylen,rab->rab$b_ksz);

    if (0==strncmp(key,rab->rab$l_kbf,count)) {
      bcopy(key,rab->rab$l_ubf,primkeylen);
      bcopy(rest+2,primkeylen+rab->rab$l_ubf,restlen);
      rab->rab$w_rsz=primkeylen+restlen;
      return SS$_NORMAL;
    }

    index=nextindex;
  }
  return SS$_NORMAL;
}
