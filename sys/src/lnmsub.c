#ifdef USERLAND
#define __LINUX_VMALLOC_H
#define _LINUX_VMALLOC_H
#define LINUX_VMALLOC_H
inline void panic(char * c) { }
#endif 

#ifdef USERLAND
#include"sysgen.h"
#include"lnmsub.h"
#include"system_data_cells.h"
#include"../../pal/src/queue.h"
#include"../../lib/src/lnmstrdef.h"
#include"../../starlet/src/ssdef.h"
#else
#include<linux/vmalloc.h>
#include"../../freevms/sys/src/sysgen.h"
#include"../../freevms/sys/src/lnmsub.h"
#include"../../freevms/sys/src/system_data_cells.h"
#include"../../freevms/pal/src/queue.h"
#include"../../freevms/lib/src/lnmstrdef.h"
#include"../../freevms/starlet/src/ssdef.h"
#endif

/* Author: Roar Thronæs */

/* << does not rotate ... ? */

int lnm$hash(const int length, const unsigned char * log, const unsigned long mask, unsigned long * myhash) {
  unsigned char *tmp=log;
  unsigned char count=length;
  unsigned long hash=count;
  lnmprintf("count %x %x %s\n",count,tmp,tmp);
  while (count>3) {
    //  lnmprintf("here %x %x\n",tmp[0],count);
    //tmp[0]=tmp[0]&223;
    //tmp[0]&=223;
    //  lnmprintf("here %x %x\n",tmp[0],count);
    //tmp[1]&=223;
    //  lnmprintf("here %x %x\n",tmp[0],count);
    //tmp[2]&=223;
    //  lnmprintf("here %x %x\n",tmp[0],count);
    //tmp[3]&=223;
    //  lnmprintf("here %x %x\n",tmp[0],count);
    hash=hash^(tmp[0]&223)^(tmp[1]&223)^(tmp[2]&223)^(tmp[3]&223);
    //  lnmprintf("here %x %x\n",tmp[0],count);
    count-=4;
    tmp+=4;
    hash=hash<<9;
  };
  //lnmprintf("here4 %x\n",hash);
  while (count) {
    //*tmp&=223;
    hash=hash^((*tmp)&223);
    count--;
    tmp++;
    //hash=hash<<13;
    //lnmprintf("here5 %x\n",hash);
  };
  hash*=0x71279461;
  hash/=3;
  //lnmprintf("here6 %x\n",hash);
  /* 
     hash&=something.LNMHSH$L_MASK;
     Not implemented yet
     */
 hash=hash%LNMSHASHTBL;
 //lnmprintf("herei7 %x\n",hash);
 hash=hash&mask;
 //lnmprintf("here3 %x\n",hash);
 *myhash=hash;
 //lnmprintf("here2 %x %x\n",myhash,hash);
 return SS$_NORMAL;
}

int lnm$searchlog(struct struct_lnm_ret * r,int loglen, char * logical, int tabnamlen, char * tablename) {
  int status;
  struct struct_nt * NT=lnmmalloc(sizeof(struct struct_nt));

  /* local stack */
  /*
    no process table yet
    lnm$presearch();
  */
  lnmprintf("searchlog\n");
  status=lnm$presearch(r,&lnmhshs, loglen, logical);
  if (status==SS$_NOLOGNAM) return status;
  // lnm$setup();
  // lnm$table();
  lnmprintf("searchlogexit\n");
  return status;
}

int lnm$search_one(struct struct_lnm_ret * r,int loglen, char * logical, int tabnamlen, char * tablename, char * result) {
  /* lock mutex */
  // lnm$searchlog();
  lnmprintf("searchoneexit\n");
#ifdef USERLAND
  exit(1);
#endif
  /* unlock */
}

int lnm$presearch(struct struct_lnm_ret * r,struct lnmhshs * hashtable,int loglen,  char * logical) {
  int status;
  unsigned long * myhash;
  // struct lnmb * mylnmb; not needed?
  myhash=lnmmalloc(sizeof(unsigned long));
  lnmprintf("presearch %x %s\n",loglen,logical);
  lnmprintf("presearch %x %s\n",loglen,logical);
  status=lnm$hash(loglen,logical,0xffff,myhash);
  lnmprintf("presearch %s\n",logical);
  status=lnm$contsearch(r,loglen,logical,*myhash,hashtable);
  lnmprintf("presearch %s\n",logical);
  //  r->mylnmb=mylnmb; erroneous?
  lnmfree(myhash);
  return status;
}

int lnm$contsearch(struct struct_lnm_ret * r,int loglen, char * logical, int hash, struct lnmhshs * hashtable) {
  int status;
  int lenstatus;
  int len=strlen(logical);
  struct lnmb *head, *tmp;
  head=lnmhshs.entry[hash*2];
  tmp=head;
  if (tmp) 
    do {
      /*if (tmp->lnmb$b_count>len) return SS$_NOLOGNAM; not yet*/
      if (tmp->lnmb$b_count==len) {
	lenstatus=strcmp(logical,tmp->lnmb$t_name);
	if (!lenstatus) {
	  r->mylnmb=tmp;
	  return SS$_NORMAL;
	}
      }
      /* no case-blind search yet */
      tmp=tmp->lnmb$l_flink;
    } while (tmp!=head) ;
  return SS$_NOLOGNAM;
}

int lnm$firsttab(struct struct_lnm_ret * r,int  tabnamlen,  char * tablename) {
  struct struct_rt * MYRT;
  MYRT=(struct struct_rt *)lnmmalloc(sizeof(struct struct_rt));
  bzero(MYRT,sizeof(struct struct_rt));
  lnmprintf("firstab %s\n",tablename);
  lnm$setup(r,MYRT,tabnamlen,tablename);
  lnmfree(MYRT);
}

int lnm$setup(struct struct_lnm_ret * r,struct struct_rt * RT, int tabnamlen,  char * tablename) {
  int status;
  RT->depth=0;
  RT->tries=255;
  lnmprintf("lnm$setup %x %s\n",tabnamlen, tablename);
  status=lnm$lookup(r, RT, tabnamlen, tablename);
  if (status==SS$_NORMAL) RT->context[RT->depth]=r->mylnmb;
  else return status;
  /* cache not implemented */
  status=lnm$table(r, RT,tabnamlen,tablename);
  return status;
}

int lnm$table(struct struct_lnm_ret * r,struct struct_rt * RT, int tabnamlen, char * tablename) {
  /* cache not implemented */
  int status;
  status=lnm$table_srch(r,RT,tabnamlen,tablename);
  return status;
}

int lnm$lookup(struct struct_lnm_ret * r,struct struct_rt * RT,int loglen, char * logical) {
  int status;
  lnmprintf("lookup %s %x\n",logical,loglen);
  status=lnm$presearch(r,&lnmhshs,loglen,logical);
  if (status!=SS$_NOLOGNAM) return status;
  return status;
}

int lnm$table_srch(struct struct_lnm_ret * r,struct struct_rt *RT, int tabnamlen,  char * tablename) {
  int xname=0;
  int len, status;
  do {
    RT->tries--;
    if (!RT->tries) return SS$_TOOMANYLNAM;
    if (((r->mylnmb)->lnmxs[xname].lnmx$b_flags)&LNM$M_MYTERMINAL)
      RT->flags|=LNM$M_MYTERMINAL;
    else
      RT->flags&=~LNM$M_MYTERMINAL;

    if (RT->depth>10) return SS$_TOOMANYLNAM;
    RT->context[RT->depth]=(r->mylnmb);
    if ((r->mylnmb)->lnmxs[xname].lnmx$b_index==LNM$C_TABLE) {
      return SS$_NORMAL;
    }
    RT->depth++;
    len=(r->mylnmb)->lnmxs[xname].lnmx$b_count;
    lnmprintf("tsr %x %s \n",(r->mylnmb)->lnmxs[xname].lnmx$t_xlation,(r->mylnmb)->lnmxs[xname].lnmx$t_xlation);
    status=lnm$lookup(r,RT,len,(r->mylnmb)->lnmxs[xname].lnmx$t_xlation);
    xname++;
  } while (xname<20 && !((r->mylnmb)->lnmxs[xname].lnmx$b_flags)&LNM$M_MYXEND);
  if ((r->mylnmb)->lnmxs[xname].lnmx$b_index==LNM$C_TABLE) {
    RT->depth--;
  }



}

int lnm$inslogtab(struct struct_lnm_ret * r,int tabnamlen,  char * tablename, struct lnmb * mylnmb) {
  int status;
  unsigned long * myhash;
  myhash=lnmmalloc(sizeof(unsigned long));
  lnmprintf("inslog\n");
  lnmprintf("%x %s\n",tabnamlen,tablename);
  status=lnm$hash(tabnamlen,tablename,0xffff,myhash);
  lnmprintf("inslog %x\n",r->mylnmb);
  lnmprintf("inslog myhash %x\n",*myhash);
  if (lnmhshs.entry[2*(*myhash)])
    insque(mylnmb,lnmhshs.entry[2*(*myhash)]);
  else {
    lnmhshs.entry[2*(*myhash)]=mylnmb;
    lnmhshs.entry[2*(*myhash)+1]=mylnmb;
    mylnmb->lnmb$l_flink=mylnmb;
    mylnmb->lnmb$l_blink=mylnmb;
  }
  lnmprintf("inslog\n");
  
  ; }

int lnm$check_prot() { ; }

#ifdef USERLAND

void lnm$lock(void) {
}

void lnm$unlock(void) {
}

void lnm$lockr(void) {
}

void lnm$unlockr(void) {
}

void lnm$lockw(void) {
}

void lnm$unlockw(void) {
}

void getipl(void) {
}

void setipl(void) {
}

void mycli(void) {
}

void mysti(void) {
}

#else

void lnm$lock(void) {
  sch$lockw(&lnm$aq_mutex);
}

void lnm$unlock(void) {
  sch$unlockw(&lnm$aq_mutex);
}

void lnm$lockr(void) {
  sch$lockw(&lnm$aq_mutex);
}

void lnm$unlockr(void) {
  sch$unlockw(&lnm$aq_mutex);
}

void lnm$lockw(void) {
  sch$lockw(&lnm$aq_mutex);
}

void lnm$unlockw(void) {
  sch$unlockw(&lnm$aq_mutex);
}

#endif
