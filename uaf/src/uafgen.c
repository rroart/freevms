// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <descrip.h>
#include <ssdef.h>
#include <uafdef.h>

#include <stdio.h> 
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef __x86_64__
#define MYSIZE 8
#else
#define MYSIZE 4
#endif

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
      int i = strtoll(b,0,10);
      memcpy(dest,&i,MYSIZE);
    } else {
      memcpy(dest,&b,MYSIZE);
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

main() {
  int fdin = open("sysuaf.dat", O_RDONLY);
  int fdout = creat("i386/sysuaf.dat", 0644);

  char buf[1024]; // watch this size
  int size=read(fdin, buf, 1024);
  struct myuaf myuaf;
  char * newbuf=buf;
  while (newbuf<(buf+size)) {
    newbuf=mylinesearch(newbuf,':',0,&myuaf.username,&myuaf.usernamelen);
    newbuf=mylinesearch(newbuf,':',0,&myuaf.password,&myuaf.passwordlen);
    newbuf=mylinesearch(newbuf,':',1,&myuaf.uid,0);
    newbuf=mylinesearch(newbuf,':',1,&myuaf.gid,0);
    newbuf=mylinesearch(newbuf,':',0,&myuaf.fullname,&myuaf.fullnamelen);
    newbuf=mylinesearch(newbuf,':',1,&myuaf.priv,0);
    newbuf++;

    struct _uaf uaf;
    memset(&uaf, 0, sizeof(uaf));
    memcpy(&uaf.uaf$t_username, myuaf.username, myuaf.usernamelen);
    memcpy(&uaf.uaf$l_pwd, myuaf.password, myuaf.passwordlen); // wrong
    memcpy(&uaf.uaf$w_mem, &myuaf.uid, 4);
    memcpy(&uaf.uaf$w_grp, &myuaf.gid, 4);
    memcpy(&uaf.uaf$t_owner, myuaf.fullname, myuaf.fullnamelen);
    memcpy(&uaf.uaf$q_priv, &myuaf.priv, 4);
    memcpy(&uaf.uaf$q_def_priv, &myuaf.priv, 4);

    printf("Writing user %s\n",uaf.uaf$t_username);
    write(fdout, &uaf, sizeof(uaf));
  }
  close (fdin);
  close (fdout);
}
