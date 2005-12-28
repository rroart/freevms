// $Id$
// $Locker$

// Author. Roar Thronæs.

// temporary module. should really be at least one privileged shareable image

#include<linux/fs.h>
#include<linux/linkage.h>
#include<linux/string.h>
#include<descrip.h>
#include<iledef.h>
#include<ssdef.h>
#include<uaidef.h>

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
  if (mount_root_vfs)
    file = filp_open(filename,O_RDONLY,0);
  if (!IS_ERR(file)) {
#ifdef CONFIG_VMS
    file = rms_open_exec(rmsfilename);
#endif
  }
  return file;
}

myread(void * file, void * buf, int size) {
  unsigned long long pos=0;
#if 0
#ifdef CONFIG_VMS
    int retsize=rms_generic_file_read(file,buf,size,&pos);
#else
    int retsize=generic_file_read(file,buf,size,&pos);
#endif
#else
    int retsize = kernel_read(file,0,buf,size);
#endif
    return retsize;
}

int mysearch(void * buf, int size, char * name) {
  char * c, *b, *n;
  int i=0;
  b=buf;
  while (b<(buf+size)) {
    c=strchr(b,':');
    n=strchr(b,'\n');
    //printk("Q %s %x %x %x %x\n",b,b,n,c,c-b);
    if (0==strncmp(b, name, c-b)) {
      return b;
    }
    b=n+1;
  }
  return 0;
}

void * mylinesearch(void * buf, char sep, int convert, char * dest, int * retlen) {
  char * c, *b, *n;
  int i=0;
  b=buf;
  n=strchr(b,'\n');
  while (b<n) {
    c=strchr(b,sep);
    if (convert) {
      int i = simple_strtoll(b,0,10);
      memcpy(dest,&i,4);
    } else {
      memcpy(dest,&b,4);
      if (retlen)
	*retlen=c-b;
    }
    b=c+1;
    return b;
  }
  return 0;
}

struct myuaf {
  int usernamelen;
  char * username;
  int passwordlen;
  char * password;
  int uid;
  int gid;
  int fullnamelen;
  char * fullname;
  int priv;
};

int exe$getuai(unsigned int efn, unsigned int *contxt, void *usrnam, void *itmlst, struct _iosb *iosb, void (*astadr)(__unknown_params), int astprm) {
  void * file = myopenfile("/vms$common/sysexe/sysuaf.dat", "[vms$common.sysexe]sysuaf.dat");
  char buf[1024]; // watch this size
  int read=myread(file,buf,1024);
  struct dsc$descriptor * user_dsc = usrnam;
  char * line = mysearch(buf, 1024, user_dsc->dsc$a_pointer);
  if (line==0)
    return SS$_BADPARAM; // maybe this?
  struct myuaf myuaf;
  char * newbuf=line;
  newbuf=mylinesearch(newbuf,':',0,&myuaf.username,&myuaf.usernamelen);
  newbuf=mylinesearch(newbuf,':',0,&myuaf.password,&myuaf.passwordlen);
  newbuf=mylinesearch(newbuf,':',1,&myuaf.uid,0);
  newbuf=mylinesearch(newbuf,':',1,&myuaf.gid,0);
  newbuf=mylinesearch(newbuf,':',0,&myuaf.fullname,&myuaf.fullnamelen);
  newbuf=mylinesearch(newbuf,':',1,&myuaf.priv,0);

  struct _ile3 * it=itmlst;
  while (it->ile3$w_code) {
    int * bufaddr_int = it->ile3$ps_bufaddr;
    switch (it->ile3$w_code) {
    case UAI$_USERNAME:
      memcpy(bufaddr_int, myuaf.username, myuaf.usernamelen);
      break;
    case UAI$_PASSWORD:
      memcpy(bufaddr_int, myuaf.password, myuaf.passwordlen);
      break;
    case UAI$_MEM:
      memcpy(bufaddr_int, &myuaf.uid, 4);
      break;
    case UAI$_GRP:
      memcpy(bufaddr_int, &myuaf.gid, 4);
      break;
    case UAI$_OWNER:
      memcpy(bufaddr_int, myuaf.fullname, myuaf.fullnamelen);
      break;
    case UAI$_PRIV:
    case UAI$_DEF_PRIV:
      memcpy(bufaddr_int, &myuaf.priv, 4);
      break;
    default:
      printk("getdvi item_code %x not implemented\n",it->ile3$w_code);
      break;
    }
    it++;
  }
  return SS$_NORMAL;
}

asmlinkage int exe$getuai_wrap(long * s) {
  return exe$getuai(s[0],s[1],s[2],s[3],s[4],s[5],s[6]);
}
