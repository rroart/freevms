// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/mm.h>
#include <asm/uaccess.h>

#include<starlet.h>

#include<descrip.h>

#include<ihadef.h>
#include<ihddef.h>
#include<ihidef.h>
#include<ihpdef.h>
#include<ihsdef.h>
#include<ihvndef.h>
#include<isddef.h>

#include<imcbdef.h>

#include<va_rangedef.h>

#include<system_data_cells.h>

#include<ssdef.h>

asmlinkage int exe$imgact_wrap(struct struct_args * s) {
  return exe$imgact(s->s1,s->s2,s->s3,s->s4,s->s5,s->s6,s->s7,s->s8);
}

asmlinkage int exe$imgact(void * name, void * dflnam, void ** hdrbuf, unsigned long imgctl, unsigned long long * inadr, unsigned long long * retadr, unsigned long long * ident, unsigned long acmode) {
#ifdef CONFIG_VMS
  struct dsc$descriptor *dscname=name;
  struct dsc$descriptor *dscdflnam=dflnam;
  struct file * f;
  struct _ihd * header=kmalloc(512,GFP_KERNEL);
  struct _ihd * ehdr32=header;
  struct _iha * active;
  struct _isd * section;
  struct _ihi * ihid;
  struct _ihvn * vers;
  struct _ihs * debug;
  struct _va_range img_inadr;
  char * buffer;
  mm_segment_t fs;
  loff_t pos=0;
  struct _imcb * im = kmalloc(sizeof(struct _imcb),GFP_KERNEL);
  bzero(im,sizeof(struct _imcb));
  //  im->imcb$b_type
  f=rms_open_exec(dscdflnam->dsc$a_pointer);
  if (f==0) return 0;
  *hdrbuf=header;
  im->imcb$l_context=f;//w_chan too small, temp place
  fs = get_fs();
  set_fs(KERNEL_DS);
  rms_generic_file_read(f, header, 512, &pos);
  set_fs(fs);

  im->imcb$l_flink=img$gl_imcb_list;
  img$gl_imcb_list=im;

  active=(unsigned long)ehdr32+ehdr32->ihd$w_activoff;
  section=(unsigned long)ehdr32+ehdr32->ihd$w_size;
  ihid=(unsigned long)ehdr32+ehdr32->ihd$w_imgidoff;
  vers=(unsigned long)ehdr32+ehdr32->ihd$w_version_array_off;
  debug=(unsigned long)ehdr32+ehdr32->ihd$w_symdbgoff;

  if (ehdr32->ihd$w_majorid!=IHD$K_MAJORID)
    return 0;

  buffer=section;

  while (section<(buffer+512*ehdr32->ihd$b_hdrblkcnt)) {
    if (section->isd$w_size==0)
      break;
    if (section->isd$w_size==0xffffffff) {
      int no=((unsigned long)section-(unsigned long)buffer)>>9;
      section=buffer+512*(no+1);
      continue;
    }

    img_inadr.va_range$ps_start_va=section->isd$v_vpn<<PAGE_SHIFT;
    img_inadr.va_range$ps_end_va=img_inadr.va_range$ps_start_va+section->isd$w_pagcnt*PAGE_SIZE;
    exe$create_region_32 (section->isd$w_pagcnt*PAGE_SIZE,0x51 ,0x187500   ,0,0,0,img_inadr.va_range$ps_start_va);
    exe$crmpsc(&img_inadr,0,0,0,0,0,0,/*(unsigned short int)*/f,0,section->isd$l_vbn,0,0);

    section=(unsigned long)section+section->isd$w_size;
  }
#endif
  return SS$_NORMAL;

}
