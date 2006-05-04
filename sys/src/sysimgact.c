// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/mm.h>
#include <asm/uaccess.h>

#include<starlet.h>

#include<descrip.h>

#include<iafdef.h>
#include<ihadef.h>
#include<ihddef.h>
#include<ihidef.h>
#include<ihpdef.h>
#include<ihsdef.h>
#include<ihvndef.h>
#include<isddef.h>
#include<shldef.h>

#include<imcbdef.h>

#include<va_rangedef.h>

#include<system_data_cells.h>

#include<ssdef.h>

#include<kfedef.h>
#include <exe_routines.h>
#include <misc_routines.h>
#include <linux/slab.h>

#include <fcbdef.h>
#include <fabdef.h>
#include <rabdef.h>

#include <linux/file.h>

int img$known_image(struct dsc$descriptor * name, void ** kfe_p) {
  struct _kfe * kfe = exe$gl_known_files;
  for (;kfe;kfe = kfe->kfe$l_kfelink) {
    char * tmpstr;
    char * kfestr = &kfe->kfe$l_obsolete_1;
    tmpstr = strrchr(kfestr,']');
    if (tmpstr==0)
      tmpstr=strrchr(kfestr,':');
    if (tmpstr==0)
      tmpstr=strrchr(kfestr,'/');
    if (tmpstr)
      kfestr=tmpstr+1;
    char * namestr=name->dsc$a_pointer;
    int namelen=name->dsc$w_length;
    tmpstr = strrchr(namestr,']');
    if (tmpstr==0)
      tmpstr=strrchr(namestr,':');
    if (tmpstr==0)
      tmpstr=strrchr(namestr,'/');
    if (tmpstr)
      namelen-=1+tmpstr-namestr;
    if (tmpstr)
      namestr=tmpstr+1;
    if (0==strncmp(kfestr,namestr,namelen)) {
      if (kfe_p)
	*kfe_p=kfe;
      return 1;
    }
  }
  return 0;
}

asmlinkage int exe$imgact_wrap(struct struct_args * s) {
  return exe$imgact(s->s1,s->s2,s->s3,s->s4,s->s5,s->s6,s->s7,s->s8);
}

asmlinkage int exe$imgact(void * name, void * dflnam, void * hdrbuf, unsigned long imgctl, unsigned long long * inadr, unsigned long long * retadr, unsigned long long * ident, unsigned long acmode) {
  struct dsc$descriptor *dscname=name;
  struct dsc$descriptor *dscdflnam=dflnam;
  struct file * f;
  struct _ihd * header=hdrbuf;
  struct _ihd * ehdr32=header;
#ifdef CONFIG_VMS
  struct _iha * active;
  struct _isd * section;
  struct _ihi * ihid;
  struct _ihvn * vers;
  struct _ihs * debug;
  struct _va_range img_inadr;
  char * buffer;
  mm_segment_t fs;
  loff_t pos=0;
  int no;
  struct _imcb * im = kmalloc(sizeof(struct _imcb),GFP_KERNEL);
  memset(im,0,sizeof(struct _imcb));
  //  im->imcb$b_type
  f=rms_open_exec(dscdflnam->dsc$a_pointer);
  if (f==0) return 0;

#if 0
  struct _rabdef * rab = fget(f);
  struct _fabdef * fab = rab->rab$l_fab;
  int chan = fab->fab$l_stv;
  printk("imgact chan %x\n",chan);
  struct _ccb * ccb = &ctl$ga_ccb_table[chan];
  ccb->ccb$l_wind = 0;
#endif

  struct _kfe * kfe;
  if (img$known_image(dscdflnam,&kfe))
    ctl$gl_pcb->pcb$l_priv|=kfe->kfe$q_procpriv;
  // do the same with phd copy

  im->imcb$l_context=f;//w_chan too small, temp place
#if 0
  fs = get_fs();
  set_fs(KERNEL_DS);
  rms_generic_file_read(f, header, 512, &pos);
  set_fs(fs);
#else
  kernel_read(f, 0, header, 512);
#endif

  if (ehdr32->ihd$w_majorid!=IHD$K_MAJORID || ehdr32->ihd$w_minorid!=IHD$K_MINORID) {
    return exe$imgact_elf(dscdflnam,ehdr32);
  }

  im->imcb$l_flink=img$gl_imcb_list;
  img$gl_imcb_list=im;

  int base = 0;
  if (inadr) {
    struct _va_range * addr = inadr;
    base = addr->va_range$ps_start_va;
  } 

  active=(unsigned long)ehdr32+ehdr32->ihd$w_activoff;
  section=(unsigned long)ehdr32+ehdr32->ihd$w_size;
  ihid=(unsigned long)ehdr32+ehdr32->ihd$w_imgidoff;
  vers=(unsigned long)ehdr32+ehdr32->ihd$w_version_array_off;
  debug=(unsigned long)ehdr32+ehdr32->ihd$w_symdbgoff;
  //ctl$gl_iaflnkptr=(unsigned long)ehdr32+ehdr32->ihd$l_iafva; // wrong? temp

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
    int rw = section->isd$l_flags&(ISD$M_WRT|ISD$M_FIXUPVEC);

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
    // printk("range %x %x\n",img_inadr.va_range$ps_start_va,img_inadr.va_range$ps_end_va);
    exe$crmpsc(&img_inadr,0,0,0,0,0,0,/*(unsigned short int)*/f,0,section->isd$l_vbn,0,0);
  skip_it:
    section=(unsigned long)section+section->isd$w_size;
  }

  struct _iaf * iaf=(unsigned long)base+(((int)ehdr32->ihd$l_iafva)<<9);
  iaf->iaf$l_fixuplnk=ctl$gl_fixuplnk;
  ctl$gl_fixuplnk=iaf;
  iaf->iaf$l_iaflink=hdrbuf;
  iaf->iaf$l_permctx=base;
  section=buffer;

  no=0;

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
    path="SYS$LIBRARY:";
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

    struct _shl * shlst = (long)iaf+iaf->iaf$l_shlstoff;
    int shli=no;
    shlst[shli].shl$l_baseva=img_inadr.va_range$ps_start_va;

    struct _iaf * tmpiaf=ctl$gl_fixuplnk;
    for(;tmpiaf;tmpiaf=tmpiaf->iaf$l_fixuplnk) {
      struct _ihd * ihd=tmpiaf->iaf$l_iaflink;
      struct _ihi * ihi=(unsigned long)ihd+ihd->ihd$w_imgidoff;
      //printk("cmp %s %s %x\n",&ihi->ihi$t_imgnam[1],imgnam,ihi->ihi$t_imgnam[0]);
      if (0==strncmp(&ihi->ihi$t_imgnam[1],imgnam,ihi->ihi$t_imgnam[0])) {
	//printk("%s already loaded\n",imgnam);
	int shli=no;
	struct _shl * shlst2=(long)tmpiaf+tmpiaf->iaf$l_shlstoff;
	shlst[shli].shl$l_baseva=shlst2[0].shl$l_baseva;
	goto skip_it2;
      }
    }

    struct _va_range out;
    printk("Loading image %s from %s\n",imgnam,image);
    sts=exe$imgact(&aname,&dflnam,hdrbuf,0,&img_inadr,&out,0,0);
    img_inadr.va_range$ps_start_va=out.va_range$ps_end_va;
    img_inadr.va_range$ps_end_va=out.va_range$ps_end_va;

    printk("imgact got sts %x\n",sts);
#if 0
    sts=exe$imgfix();
    printf("imgfix got sts %x\n",sts);
#endif

#if 0
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
    //printk("range %x %x\n",img_inadr.va_range$ps_start_va,img_inadr.va_range$ps_end_va);
    exe$crmpsc(&img_inadr,0,0,0,0,0,0,/*(unsigned short int)*/f,0,section->isd$l_vbn,0,0);
#endif
  skip_it2:
    section=(unsigned long)section+section->isd$w_size;
    no++;
  }

  if (retadr) {
    struct _va_range * r=retadr;
    r->va_range$ps_start_va=0;
    r->va_range$ps_end_va=img_inadr.va_range$ps_end_va;
  }

#else
  loff_t pos=0;
  mm_segment_t fs;
#ifndef CONFIG_VMS
  f=open_exec(dscdflnam->dsc$a_pointer);
  if (f==0) return 0;
#endif
#if 0
  fs = get_fs();
  set_fs(KERNEL_DS);
  generic_file_read(f, header, 512, &pos);
  set_fs(fs);
#else
  kernel_read(f, 0, header, 512);
#endif
  return exe$imgact_elf(dscdflnam,ehdr32);
#endif
}
