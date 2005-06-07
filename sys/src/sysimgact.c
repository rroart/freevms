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

asmlinkage int exe$imgact(void * name, void * dflnam, void * hdrbuf, unsigned long imgctl, unsigned long long * inadr, unsigned long long * retadr, unsigned long long * ident, unsigned long acmode) {
#ifdef CONFIG_VMS
  struct dsc$descriptor *dscname=name;
  struct dsc$descriptor *dscdflnam=dflnam;
  struct file * f;
  struct _ihd * header=hdrbuf;
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
  ctl$gl_iaflnkptr=(unsigned long)ehdr32+ehdr32->ihd$l_iafva; // wrong? temp
  ctl$gl_fixuplnk=f; // wrong? temp
#if 0
  img_inadr.va_range$ps_start_va=0x3e000000;
  img_inadr.va_range$ps_end_va=0x3e000000+(((f->f_dentry->d_inode->i_size>>12)+1)<<12);
  exe$create_region_32 ((f->f_dentry->d_inode->i_size>>12)+1,0x45|_PAGE_RW ,0x187500   ,0,0,0,img_inadr.va_range$ps_start_va);
  exe$crmpsc(&img_inadr,0,0,0,0,0,0,/*(unsigned short int)*/f,0,ctl$gl_iaflnkptr,0,0);
#endif

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
    if (section->isd$v_gbl==1) {
      goto skip_it;
    }
    int rw = section->isd$l_flags&ISD$M_WRT;

    int base = 0;
    if (inadr) {
      struct _va_range * addr = inadr;
      base = addr->va_range$ps_start_va;
    } 

    img_inadr.va_range$ps_start_va=base+(section->isd$v_vpn<<PAGE_SHIFT);
    img_inadr.va_range$ps_end_va=img_inadr.va_range$ps_start_va+section->isd$w_pagcnt*PAGE_SIZE;
#ifdef __arch_um__
    exe$create_region_32 (section->isd$w_pagcnt*PAGE_SIZE,0x51 ,0x187500   ,0,0,0,img_inadr.va_range$ps_start_va);
#else
    int rwfl=0;
    if (rw)
      rwfl=_PAGE_RW;
    exe$create_region_32 (section->isd$w_pagcnt*PAGE_SIZE,0x45|rwfl ,0x187500   ,0,0,0,img_inadr.va_range$ps_start_va);
#endif
    exe$crmpsc(&img_inadr,0,0,0,0,0,0,/*(unsigned short int)*/f,0,section->isd$l_vbn,0,0);
  skip_it:
    section=(unsigned long)section+section->isd$w_size;
  }

  section=buffer;

  while (section<(buffer+512*ehdr32->ihd$b_hdrblkcnt)) {
    if (section->isd$w_size==0)
      break;
    if (section->isd$w_size==0xffffffff) {
      int no=((unsigned long)section-(unsigned long)buffer)>>9;
      section=buffer+512*(no+1);
      continue;
    }
    if (section->isd$v_gbl==0) {
      goto skip_it2;
    }
    
    int sts;
    char * path;
    int pathlen;
    path="SYS$SYSTEM:";
    pathlen=strlen(path);
    char image[256];
    memset(image,0,256);
    char * imagebase=&section->isd$t_gblnam;
    char * imgnam = imagebase;
    int len = strlen(imgnam);
    memcpy(image,path,pathlen);
    memcpy(image+pathlen,imagebase,strlen(imagebase));
#if 0
    memcpy(image+pathlen+strlen(imagebase),".exe",4);
    image[pathlen+strlen(imagebase)+4]=0;
#endif

    struct dsc$descriptor aname;
    struct dsc$descriptor dflnam;
    aname.dsc$w_length=len;
    aname.dsc$a_pointer=imgnam;
    dflnam.dsc$w_length=pathlen+strlen(imagebase)+4*0;
    dflnam.dsc$a_pointer=image;

    char * hdrbuf=kmalloc(512,GFP_KERNEL); // leak
    memset(hdrbuf, 0, 512);

    img_inadr.va_range$ps_start_va=img_inadr.va_range$ps_end_va;
    img_inadr.va_range$ps_end_va=0;

    struct _va_range out;
    sts=exe$imgact(&aname,&dflnam,hdrbuf,0,&img_inadr,&out,0,0);
    img_inadr.va_range$ps_start_va=out.va_range$ps_end_va;

    printk("imgact got sts %x\n",sts);
#if 0
    sts=exe$imgfix();
    printf("imgfix got sts %x\n",sts);
#endif

    int rw = section->isd$l_flags&ISD$M_WRT;

    img_inadr.va_range$ps_start_va=section->isd$v_vpn<<PAGE_SHIFT;
    img_inadr.va_range$ps_end_va=img_inadr.va_range$ps_start_va+section->isd$w_pagcnt*PAGE_SIZE;
#ifdef __arch_um__
    exe$create_region_32 (section->isd$w_pagcnt*PAGE_SIZE,0x51 ,0x187500   ,0,0,0,img_inadr.va_range$ps_start_va);
#else
    int rwfl=0;
    if (rw)
      rwfl=_PAGE_RW;
    exe$create_region_32 (section->isd$w_pagcnt*PAGE_SIZE,0x45|rwfl ,0x187500   ,0,0,0,img_inadr.va_range$ps_start_va);
#endif
    exe$crmpsc(&img_inadr,0,0,0,0,0,0,/*(unsigned short int)*/f,0,section->isd$l_vbn,0,0);
  skip_it2:
    section=(unsigned long)section+section->isd$w_size;
  }

  if (retadr) {
    struct _va_range * r=retadr;
    r->va_range$ps_start_va=0;
    r->va_range$ps_end_va=img_inadr.va_range$ps_end_va;
  }

#endif
  return SS$_NORMAL;

}
