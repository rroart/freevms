// $Id$
// $Locker$

// Author. Roar Thronæs.

// temporary module. should really be at least one privileged shareable image

#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/linkage.h>
#include<linux/string.h>
#include<descrip.h>
#include<iledef.h>
#include<ssdef.h>
#include<uafdef.h>
#include<uaidef.h>
#include <exe_routines.h>
#include <misc_routines.h>
#include <rabdef.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/file.h>

asmlinkage void exe$check_access    (void) { }
asmlinkage void exe$getuai_not          (void) { }
asmlinkage void exe$setuai          (void) { }
asmlinkage void exe$idtoasc         (void) { }
asmlinkage void exe$asctoid         (void) { }
asmlinkage void exe$add_ident       (void) { }
asmlinkage void exe$rem_ident       (void) { }
asmlinkage void exe$find_held       (void) { }
asmlinkage void exe$find_holder     (void) { }
asmlinkage void exe$mod_ident       (void) { }
asmlinkage void exe$mod_holder      (void) { }
asmlinkage void exe$grantid      (void) { }
asmlinkage void exe$revokid      (void) { }

myopenfile(char * filename, char * rmsfilename) {
  struct file * file=0;
  extern int mount_root_vfs;
  if (!IS_ERR(file)) {
    file = rms_open_exec(rmsfilename);
  }
  return file;
}

myread(void * file, void * buf, int size) {
  unsigned long long pos=0;
#if 0
    int retsize=rms_generic_file_read(file,buf,size,&pos);
#else
    int retsize = kernel_read(file,0,buf,size);
#endif
    return retsize;
}

int exe$getuai(unsigned int efn, unsigned int *contxt, void *usrnam, void *itmlst, struct _iosb *iosb, void (*astadr)(__unknown_params), int astprm) {
#if 0
  void * file = myopenfile("/vms$common/sysexe/sysuaf.dat", "[vms$common.sysexe]sysuaf.dat");
#else
  int fd = sys_open("[vms$common.sysexe]sysuaf.dat",0,0);
#endif
  int gotsize;
  char * charbuf;
  int sts = exe_std$alononpaged(4096, &gotsize, &charbuf);
  struct _uaf * buf = charbuf;
#if 0
  int read=myread(file,buf,4096);
#else
  asmlinkage ssize_t sys_read(unsigned int fd, char * buf, size_t count);
  int read = sys_read(fd, buf, 4096);
#endif
  struct dsc$descriptor * user_dsc = usrnam;
  int i;
  for(i=0;i<2;i++)
    if (0==strncmp(buf[i].uaf$t_username,user_dsc->dsc$a_pointer,user_dsc->dsc$w_length)) {
      break;
    }
  if (i==2) {
    exe_std$deanonpgdsiz(buf,4096);
    return 0;
  }
  struct _uaf * uaf = &buf[i];

  struct _ile3 * it=itmlst;
  while (it->ile3$w_code) {
    int * bufaddr_int = it->ile3$ps_bufaddr;
    switch (it->ile3$w_code) {
    case UAI$_USERNAME:
      memcpy(bufaddr_int, uaf->uaf$t_username, 32);
      break;
    case UAI$_PASSWORD:
      memcpy(bufaddr_int, uaf->uaf$l_pwd, 12); // wrong
      break;
    case UAI$_MEM:
      memcpy(bufaddr_int, &uaf->uaf$w_mem, 2);
      break;
    case UAI$_GRP:
      memcpy(bufaddr_int, &uaf->uaf$w_grp, 2);
      break;
    case UAI$_OWNER:
      memcpy(bufaddr_int, uaf->uaf$t_owner, 20);
      break;
    case UAI$_PRIV:
    case UAI$_DEF_PRIV:
      memcpy(bufaddr_int, &uaf->uaf$q_priv, 4);
      break;
    default:
      printk("getdvi item_code %x not implemented\n",it->ile3$w_code);
      break;
    }
    it++;
  }
  exe_std$deanonpgdsiz(buf,4096);
  return SS$_NORMAL;
}

asmlinkage int exe$getuai_wrap(long * s) {
  return exe$getuai(s[0],s[1],s[2],s[3],s[4],s[5],s[6]);
}
