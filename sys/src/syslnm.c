#include<linux/linkage.h>
#include<stdlib.h>
#include"../../starlet/src/starlet.h"
#include"../../starlet/src/lnmdef.h"
#include"../../starlet/src/ssdef.h"
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
asmlinkage sys$crelnm  (unsigned int *attr, void *tabnam, void *lognam, unsigned char *acmode, void *itmlst)
{
}

asmlinkage sys_$CRELNT();
/*asmlinkage sys$crelnt  (unsigned int *attr, void *resnam, unsigned
  short int *reslen, unsigned int *quota,
  unsigned short int *promsk, void *tabnam, void
  *partab, unsigned char *acmode);*/

asmlinkage sys$_crelnt  (struct struct_crelnt *s) {
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
  printf("partab %s\n",mypartab->dsc$a_pointer);
  if (s->tabnam) mytabnam=s->tabnam;
  else {
    /*    mytabnam="LNM$something";*/
  }
  /* set ipl 2*/
  mylnmb=lnmmalloc(sizeof(struct lnmb));
  mylnmx=lnmmalloc(sizeof(struct lnmx));
  mylnmth=lnmmalloc(sizeof(struct lnmth));
  myorb=lnmmalloc(sizeof(struct orb));
  trailer=lnmmalloc(sizeof(long));

  /* mutex lock */
  RT=(struct struct_rt *) lnmmalloc(sizeof(struct struct_rt));
  //      printf("");/* this makes a difference somehow */
  //  printf("this %x\n",mytabnam->dsc$w_length);
  //  printf("this %x %s\n",mytabnam->dsc$w_length,mytabnam->dsc$a_pointer);
  printf("fir %x\n",amylnmth);
  status=lnm$firsttab(mypartab->dsc$w_length,mypartab->dsc$a_pointer,amylnmb,amylnmth);
  printf("fir %x\n",amylnmth);
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
    mylnmth->lnmth$l_name=mylnmth;
    mylnmth->lnmth$l_parent=0;
    mylnmth->lnmth$l_sibling=0;
    mylnmth->lnmth$l_child=0;
    mylnmth->lnmth$l_qtable=0;
    mylnmth->lnmth$l_hash=0;
    mylnmth->lnmth$l_orb=0;
    mylnmth->lnmth$l_byteslm=0;
    mylnmth->lnmth$l_bytes=0;

  printf("bef inslogtab %x %s\n",mylnmb->lnmb$b_count,mytabnam->dsc$a_pointer);
        status=lnm$inslogtab(&mylnmb->lnmb$b_count,mytabnam->dsc$a_pointer,mylnmb,mylnmth);
  //  printf("exit here\n");
    //  exit(1);
  printf("so far %x\n",status);

  /* unlock mutex */

  return status;
}

asmlinkage sys_$DELLNM() {;}
asmlinkage sys$dellnm  (void *tabnam, void *lognam, unsigned char *acmode) {
}

asmlinkage sys_$TRNLNM() {;}
asmlinkage sys$trnlnm  (unsigned int *attr, void *tabnam, void
			*lognam, unsigned char *acmode, void *itmlst) {
int status;
  if (!(tabnam && itmlst)) return -1;
      /* lock mutex */
//      status=lnm$searchlog();
      if (status==SS$_NOLOGNAM) {
	/* unlock mutex */
	return status;
      }

      }

#ifdef USERLAND
  main(){
    int c;
    unsigned long * myhash;
    int status;
    struct lnmb * lnm$system_directory;
    struct lnmth * lnm$system_directory_b;
    struct struct_crelnt * s;
    $DESCRIPTOR(mytabnam,"MYTEST");
    $DESCRIPTOR(mypartab,"LNM$SYSTEM_DIRECTORY");
    /*    lnm$system_directory_b=lnmmalloc(sizeof(struct lnmth));*/
    lnm$system_directory=lnmmalloc(sizeof(struct lnmb));
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
    printf("here %x %x\n",myhash,*myhash);
  lnmhshs.entry[2*(*myhash)]=lnm$system_directory;
  lnmhshs.entry[2*(*myhash)+1]=lnm$system_directory;
    s=lnmmalloc(sizeof(struct struct_crelnt));
    s->partab=&mypartab;
    s->tabnam=&mytabnam;
    status=sys$_crelnt(s);
    printf("end status %x\n",status);
    for (c=0;c<LNMSHASHTBL;c++) {
      if (lnmhshs.entry[2*c]) printf("lnmhshs entry %x %x %s\n",c,lnmhshs.entry[2*c],((struct lnmb *)(lnmhshs.entry[2*c]))->lnmb$t_name);
    }
  }
#endif

