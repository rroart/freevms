#ifdef USERLAND
#define __LINUX_VMALLOC_H
#define _LINUX_VMALLOC_H
#define LINUX_VMALLOC_H
struct lnmhshs lnmhshs; /* should be one struct, will be solved later */
struct lnmhshp lnmhshp;
#endif 

#include<linux/linkage.h>
#include<stdlib.h>
#include"../../starlet/src/starlet.h"
#include"../../starlet/src/lnmdef.h"
#include"../../starlet/src/ssdef.h"
#include"../../starlet/src/misc.h"
#include"../../librtl/src/descrip.h"
#include"../../lib/src/lnmstrdef.h"
#include"../../lib/src/orbdef.h"
#include"../../lib/src/dyndef.h"
#include"../../lib/src/system_service_setup.h"
#include"lnmsub.h"
#include"sysgen.h" 
#include"system_data_cells.h"

/* Author: Roar Thronæs */

#ifndef USERLAND
#define __KERNEL__         /* We're part of the kernel */
#define MODULE             /* Not a permanent part, though. */
#endif

/* Standard headers for LKMs */
#include <linux/modversions.h> 
#include <linux/module.h>  
#include "system_data_cells.h"

#define _LOOSE_KERNEL_NAMES
/* With some combinations of Linux and gcc, tty.h will not compile if
        you don't define _LOOSE_KERNEL_NAMES.  It's a bug somewhere.
     */
#include <linux/tty.h>      /* console_print() interface */


int isp1(void * a) { return 0; }

int isshar(void * a) { return 1; }

/* Initialize the LKM */
#ifdef MODULE 
int init_module()
{
  /*exe$crelnt();*/
  /*ctl$gl_lnmdirect=LNM$PROCESS_DIRECTORY;
lnm$al_dirtbl[0]=LNM$SYSTEM_DIRECTORY;
lnm$al_dirtbl[1]=ctl$gl_lnmdirect;*/
  console_print("Hello, world - this is the kernel speaking\n");
  /* More normal is printk(), but there's less that can go wrong with 
     console_print(), so let's start simple.
  */

  /* If we return a non zero value, it means that 
   * init_module failed and the LKM can't be loaded 
   */
  return 0;
}


/* Cleanup - undo whatever init_module did */
void cleanup_module()
{
  console_print("Short is the life of an LKM\n");
}
#endif

asmlinkage sys_$CRELNM();
asmlinkage exe$crelnm  (unsigned int *attr, void *tabnam, void *lognam, unsigned char *acmode, void *itmlst)
{
  int status;
  struct lnmb ** amylnmb;
  struct lnmth ** amylnmth;
  struct lnmb * mylnmb;
  struct lnmth * mylnmth;
  struct dsc$descriptor * mytabnam=tabnam;
  struct dsc$descriptor * mylognam=lognam;
  struct item_list_3 * i,j;
  struct struct_rt * RT;

  mylnmb=lnmmalloc(sizeof(struct lnmb));
  bzero(mylnmb,sizeof(struct lnmb));
  mylnmth=lnmmalloc(sizeof(struct lnmth));
  bzero(mylnmth,sizeof(struct lnmth));

  amylnmb = lnmmalloc((sizeof(void *)));
  amylnmth = lnmmalloc((sizeof(void *)));

  RT=(struct struct_rt *) lnmmalloc(sizeof(struct struct_rt));
  bzero(RT,sizeof(struct struct_rt));

  status=lnm$firsttab(mytabnam->dsc$w_length,mytabnam->dsc$a_pointer,amylnmb,amylnmth);

  for(i=itmlst;i->item_code!=0;i+=sizeof(*i)) {
    mylnmb=lnmmalloc(sizeof(struct lnmb));
    bzero(mylnmb,sizeof(struct lnmb));
    mylnmb->lnmb$l_flink=0;
    mylnmb->lnmb$l_blink=0;
    mylnmb->lnmb$w_size=sizeof(struct lnmb);
    mylnmb->lnmb$b_type=DYN$C_LNM;
    mylnmb->lnmb$b_acmode=0;
    mylnmb->lnmb$l_table=0;
    mylnmb->lnmb$b_flags=0;
    mylnmb->lnmb$b_count=mylognam->dsc$w_length;
    strncpy( &(mylnmb->lnmb$t_name[0]),mylognam->dsc$a_pointer,mylognam->dsc$w_length);
    mylnmb->lnmxs[0].lnmx$b_count=i->buflen;
    strncpy( &(mylnmb->lnmxs[0].lnmx$t_xlation[0]),i->bufaddr,i->buflen);
    mylnmb->lnmxs[0].lnmx$b_flags=LNM$M_MYTERMINAL;
    mylnmb->lnmxs[1].lnmx$b_flags=LNM$M_MYXEND;

    status=lnm$inslogtab(mylognam->dsc$w_length,mylognam->dsc$a_pointer,mylnmb,mylnmth);

  }
  
}

asmlinkage sys_$CRELNT();
/*asmlinkage sys$crelnt  (unsigned int *attr, void *resnam, unsigned
  short int *reslen, unsigned int *quota,
  unsigned short int *promsk, void *tabnam, void
  *partab, unsigned char *acmode);*/

asmlinkage int exe$crelnt  (struct struct_crelnt *s) {
  int status;
  /*  char * mytabnam;
      int tabnamlen;*/
  struct lnmb ** amylnmb;
  struct lnmth ** amylnmth;
  struct lnmb * mylnmb;
  struct lnmx * mylnmx;
  struct lnmth * mylnmth;
  struct orb * myorb;
  long * trailer;
  struct struct_rt * RT;
  struct dsc$descriptor_s * mytabnam, * mypartab;
  amylnmb = lnmmalloc((sizeof(void *)));
  amylnmth = lnmmalloc((sizeof(void *)));
  if (!(s->partab)) return -1;
  mypartab=s->partab;
  lnmprintf("partab %s\n",mypartab->dsc$a_pointer);
  if (s->tabnam) mytabnam=s->tabnam;
  else {
    /*    mytabnam="LNM$something";*/
  }
  /* set ipl 2*/
  mylnmb=lnmmalloc(sizeof(struct lnmb));
  bzero(mylnmb,sizeof(struct lnmb));
  mylnmx=lnmmalloc(sizeof(struct lnmx));
  bzero(mylnmx,sizeof(struct lnmx));
  mylnmth=lnmmalloc(sizeof(struct lnmth));
  bzero(mylnmth,sizeof(struct lnmth));
  myorb=lnmmalloc(sizeof(struct orb));
  bzero(myorb,sizeof(struct orb));
  trailer=lnmmalloc(sizeof(long));

  /* mutex lock */
  RT=(struct struct_rt *) lnmmalloc(sizeof(struct struct_rt));
  bzero(RT,sizeof(struct struct_rt));
  //      lnmprintf("");/* this makes a difference somehow */
  //  lnmprintf("this %x\n",mytabnam->dsc$w_length);
  //  lnmprintf("this %x %s\n",mytabnam->dsc$w_length,mytabnam->dsc$a_pointer);
  lnmprintf("fir %x\n",amylnmth);
  status=lnm$firsttab(mypartab->dsc$w_length,mypartab->dsc$a_pointer,amylnmb,amylnmth);
  lnmprintf("fir %x\n",amylnmth);
  if (status==SS$_NOLOGTAB) {
    lnmfree(mylnmb);
    lnmfree(mylnmx);
    lnmfree(mylnmth);
    lnmfree(myorb);
    lnmfree(trailer);
    /* unlock mutex */
    return status;
  }

  if (isp1(mylnmth)){
    /*
      lnmfree(mylnmb);
      lnmfree(mylnmx);
      lnmfree(mylnmth);
      lnmfree(myorb);
      lnmfree(trailer);
      deallocate these
      and allocate from process-private region insted
    */
  }
  if (isshar(mylnmth)){
    status=lnm$check_prot();
    if (status!=SS$_NORMAL) {
      lnmfree(mylnmb);
      lnmfree(mylnmx);
      lnmfree(mylnmth);
      lnmfree(myorb);
      lnmfree(trailer);
      /* unlock mutex */
      return SS$_NOPRIV;
    }
  }
  /* check quota 

     lnmfree(mylnmb);
     lnmfree(mylnmx);
     lnmfree(mylnmth);
     lnmfree(myorb);
     lnmfree(trailer);
     unlock mutex 
     return SS$_EXLNMQUOTA;

  */

  mylnmb->lnmb$l_flink=0;
  mylnmb->lnmb$l_blink=0;
  mylnmb->lnmb$w_size=sizeof(struct lnmb);
  mylnmb->lnmb$b_type=DYN$C_LNM;
  mylnmb->lnmb$b_acmode=0;
  mylnmth=&mylnmb->lnmxs[0].lnmx$b_count;
  mylnmb->lnmb$l_table=mylnmth; /* should be parent */
  mylnmb->lnmb$b_flags=LNM$M_TABLE;
  mylnmb->lnmb$b_count=mytabnam->dsc$w_length;
  strncpy( &(mylnmb->lnmb$t_name[0]),mytabnam->dsc$a_pointer,mytabnam->dsc$w_length);

  mylnmb->lnmxs[0].lnmx$b_flags=LNM$M_MYTERMINAL;
  mylnmb->lnmxs[0].lnmx$b_index=LNM$C_TABLE;
  mylnmb->lnmxs[1].lnmx$b_flags=LNM$M_MYXEND;


  mylnmth->lnmth$b_flags=LNM$M_MYSHAREABLE|LNM$M_MYDIRECTORY;
  mylnmth->lnmth$l_name=mylnmb;
  mylnmth->lnmth$l_parent=*amylnmb;
  mylnmth->lnmth$l_sibling=0;
  mylnmth->lnmth$l_child=0;
  mylnmth->lnmth$l_qtable=0;
  mylnmth->lnmth$l_hash=0;
  mylnmth->lnmth$l_orb=0;
  mylnmth->lnmth$l_byteslm=0;
  mylnmth->lnmth$l_bytes=0;

  lnmprintf("bef inslogtab %x %s\n",mylnmb->lnmb$b_count,mytabnam->dsc$a_pointer);
  status=lnm$inslogtab(mylnmb->lnmb$b_count,mytabnam->dsc$a_pointer,mylnmb,mylnmth);
  //  lnmprintf("exit here\n");
  //  exit(1);
  lnmprintf("so far %x\n",status);

  /* unlock mutex */

  return status;
}

asmlinkage sys_$DELLNM() {;}
asmlinkage sys$dellnm  (void *tabnam, void *lognam, unsigned char *acmode) {
}

asmlinkage sys_$TRNLNM() {;}
asmlinkage exe$trnlnm  (unsigned int *attr, void *tabnam, void
			*lognam, unsigned char *acmode, void *itmlst) {
  int status;
  struct dsc$descriptor *mytabnam, *mylognam;
  struct lnmb ** amylnmb;
  struct item_list_3 * i=itmlst;

  amylnmb = lnmmalloc((sizeof(void *)));
  mylognam=lognam;
  mytabnam=tabnam;
  if (!(tabnam && itmlst)) return -1;
  /* lock mutex */
  status=lnm$searchlog(mylognam->dsc$w_length,mylognam->dsc$a_pointer,mytabnam->dsc$w_length,mytabnam->dsc$a_pointer,amylnmb);
  if (status==SS$_NOLOGNAM || status!=SS$_NORMAL) {
    /* unlock mutex */
    return status;
  }
  i->buflen=(*amylnmb)->lnmxs[0].lnmx$b_count;
  i->bufaddr=(*amylnmb)->lnmxs[0].lnmx$t_xlation;
  lnmprintf("found lnm %x %s\n",i->bufaddr,i->bufaddr);
  return status;
}

#ifdef USERLAND
main(){
  struct item_list_3 i[2];
  int c;
  unsigned long * myhash;
  int status;
  struct lnmb * lnm$system_directory;
  struct lnmth * lnm$system_directory_b;
  struct struct_crelnt * s;
  $DESCRIPTOR(mynam,"BIBI");
  $DESCRIPTOR(mytabnam3,"MYTEST3");
  $DESCRIPTOR(mytabnam2,"MYTEST2");
  $DESCRIPTOR(mytabnam,"MYTEST");
  $DESCRIPTOR(mypartab,"LNM$SYSTEM_DIRECTORY");
  /*    lnm$system_directory_b=lnmmalloc(sizeof(struct lnmth));*/
  lnm$system_directory=lnmmalloc(sizeof(struct lnmb));
  bzero(lnm$system_directory,sizeof(struct lnmb));
  lnm$system_directory->lnmb$l_flink=0;
  lnm$system_directory->lnmb$l_blink=0;
  lnm$system_directory->lnmb$b_type=DYN$C_LNM;
  lnm$system_directory->lnmb$b_acmode=MODE_K_KERNEL;
  lnm$system_directory_b=(struct lnmth *)&lnm$system_directory->lnmxs[0].lnmx$b_count;
  lnm$system_directory->lnmb$l_table=lnm$system_directory_b;
  lnm$system_directory->lnmb$b_flags=LNM$M_NO_ALIAS|LNM$M_TABLE|LNM$M_NO_DELETE;
  lnm$system_directory->lnmb$b_count=mypartab.dsc$w_length;
  strncpy(lnm$system_directory->lnmb$t_name,mypartab.dsc$a_pointer,lnm$system_directory->lnmb$b_count);
  lnm$system_directory->lnmxs[0].lnmx$b_flags=LNM$M_MYTERMINAL;
  lnm$system_directory->lnmxs[0].lnmx$b_index=LNM$C_TABLE;
  lnm$system_directory->lnmxs[1].lnmx$b_flags=LNM$M_MYXEND;

  lnm$system_directory_b->lnmth$b_flags=LNM$M_MYSHAREABLE|LNM$M_MYDIRECTORY;
  lnm$system_directory_b->lnmth$l_name=lnm$system_directory;
  lnm$system_directory_b->lnmth$l_parent=0;
  lnm$system_directory_b->lnmth$l_sibling=0;
  lnm$system_directory_b->lnmth$l_child=0;
  lnm$system_directory_b->lnmth$l_qtable=0;
  lnm$system_directory_b->lnmth$l_hash=0;
  lnm$system_directory_b->lnmth$l_orb=0;
  lnm$system_directory_b->lnmth$l_byteslm=0;
  lnm$system_directory_b->lnmth$l_bytes=0;

  /*ctl$gl_lnmdirect=LNM$PROCESS_DIRECTORY;
    lnm$al_dirtbl[0]=LNM$SYSTEM_DIRECTORY;
    lnm$al_dirtbl[1]=ctl$gl_lnmdirect;*/
  myhash=lnmmalloc(sizeof(unsigned long));
  status=lnm$hash(mypartab.dsc$w_length,mypartab.dsc$a_pointer,0xffff,myhash);
  lnmprintf("here %x %x\n",myhash,*myhash);
  lnmhshs.entry[2*(*myhash)]=lnm$system_directory;
  lnmhshs.entry[2*(*myhash)+1]=lnm$system_directory;
  s=lnmmalloc(sizeof(struct struct_crelnt));
  s->partab=&mypartab;
  s->tabnam=&mytabnam;
  status=exe$crelnt(s);
  s=lnmmalloc(sizeof(struct struct_crelnt));
  s->partab=&mypartab;
  s->tabnam=&mytabnam2;
  status=exe$crelnt(s);
  s=lnmmalloc(sizeof(struct struct_crelnt));
  s->partab=&mytabnam2;
  s->tabnam=&mytabnam3;
  status=exe$crelnt(s);
  i[0].item_code=1;
  i[0].buflen=5;
  i[0].bufaddr="mylog";
  bzero(&i[1],sizeof(struct item_list_3));
  status=exe$crelnm(0,&mytabnam2,&mynam,0,i);
  status=exe$trnlnm(0,&mytabnam2,&mynam,0,i);
  lnmprintf("end status %x\n",status);
  for (c=0;c<LNMSHASHTBL;c++) {
    if (lnmhshs.entry[2*c]) { 
      struct lnmth * l;
      lnmprintf("lnmhshs entry %x %x %s\n",c,lnmhshs.entry[2*c],((struct lnmb *)(lnmhshs.entry[2*c]))->lnmb$t_name);
      l=&(((struct lnmb *)(lnmhshs.entry[2*c]))->lnmxs[0].lnmx$b_count);
      lnmprintf("     parent %x\n",l->lnmth$l_parent);
    }
  }
}
#endif

