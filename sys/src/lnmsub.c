// $Id$
// $Locker$

// Author. Roar Thronæs.

#ifdef USERLAND
#define __LINUX_VMALLOC_H
#define _LINUX_VMALLOC_H
#define LINUX_VMALLOC_H
inline void panic(char * c) { }
#endif 

#ifdef USERLAND
#include<sysgen.h>
#include<lnmsub.h>
#include<system_data_cells.h>
#include<queue.h>
#include<lnmstrdef.h>
#include<ssdef.h>
#else
#include<linux/mm.h>
#include<sysgen.h>
#include<lnmsub.h>
#include<system_data_cells.h>
#include<queue.h>
#include<lnmdef.h>
#include<lnmstrdef.h>
#include<ssdef.h>
#include<descrip.h>
#include<misc.h>
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

int lnm$searchlog(struct struct_lnm_ret * r,int loglen, char * lognam, int tabnamlen, char * tablename) {
  int status;
  void * hash; 
  long ahash;
  struct _pcb * pcb = smp$gl_cpu_data[0]->cpu$l_curpcb; 
  struct struct_rt * rt=lnmmalloc(sizeof(struct struct_rt));
  struct struct_nt * nt=lnmmalloc(sizeof(struct struct_nt));
  bzero(rt,sizeof(struct struct_rt));
  bzero(nt,sizeof(struct struct_nt));

  lnmprintf("searchlog\n");

  status=lnm$hash(loglen,lognam,0xffff,&ahash);

  nt->loglen=loglen;
  nt->lognam=lognam;
  nt->hash=ahash;

  hash=pcb->pcb$l_affinity_callback;
  status=lnm$presearch(r, hash, nt);

  if ((status&1)==0) {
    hash=&lnmhshs;
    status=lnm$presearch(r, hash, nt);
  }

  if ((status&1)==0) 
    return status;

  lnm$setup(r, rt, loglen, lognam,nt);
  lnm$table(r, rt, nt);
  lnm$contsearch(r, ahash, hash, nt);
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

int lnm$presearch(struct struct_lnm_ret * r,struct lnmhshs * hashtable, struct struct_nt * nt) {
  int status;
  unsigned long * myhash;
  // struct _lnmb * mylnmb; not needed?
  myhash=lnmmalloc(sizeof(unsigned long));
  lnmprintf("presearch %x %s\n",nt->loglen,nt->lognam);
  status=lnm$hash(nt->loglen,nt->lognam,0xffff,myhash);
  status=lnm$contsearch(r,*myhash,hashtable,nt);
  //  r->mylnmb=mylnmb; erroneous?
  lnmfree(myhash);
  return status;
}

int lnm$contsearch(struct struct_lnm_ret * r, int hash, struct lnmhshs * hashtable, struct struct_nt * nt) {
  int status;
  int lenstatus;
  struct _lnmb *head, *tmp;
  lnmprintf("contsearch\n");
  head=hashtable->entry[hash*2];
  if (head) {
    tmp=nt->lnmb_cur;
    if (tmp==0) {
      tmp=head;
      nt->lnmb_cur=tmp;
    }
    do {
      /*if (tmp->lnmb$b_count>nt->loglen) return SS$_NOLOGNAM; not yet*/
      if (tmp->lnmb$b_count==nt->loglen) {
	lenstatus=strcmp(nt->lognam,tmp->lnmb$t_name);
	if (lenstatus==0) {
	  if (nt->lnmth && nt->lnmth!=tmp->lnmb$l_table) { 
	  } else { 
	    r->mylnmb=tmp;
	    return SS$_NORMAL;
	  }
	}
      }
      /* no case-blind search yet */
      nt->lnmb_cur=tmp;
      tmp=tmp->lnmb$l_flink;
    } while (tmp!=head) ;
  }
  return SS$_NOLOGNAM;
}

int lnm$firsttab(struct struct_lnm_ret * r,int  tabnamlen,  char * tablename) {
  struct struct_rt * MYRT;
  struct struct_nt * MYNT;
  MYRT=(struct struct_rt *)lnmmalloc(sizeof(struct struct_rt));
  bzero(MYRT,sizeof(struct struct_rt));
  MYNT=(struct struct_rt *)lnmmalloc(sizeof(struct struct_rt));
  bzero(MYNT,sizeof(struct struct_rt));
#if 0
  MYNT->loglen=tabnamlen;
  MYNT->lognam=tablename;
#endif
  lnmprintf("firstab %s\n",tablename);
  lnm$setup(r,MYRT,tabnamlen,tablename,MYNT);
  lnmfree(MYRT);
}

int lnm$setup(struct struct_lnm_ret * r,struct struct_rt * RT,  int tabnamlen, char * tablename, struct struct_nt * nt) {
  int status;
  RT->depth=0;
  RT->tries=255;
  lnmprintf("lnm$setup %x %s\n",tablename, tablename);
  status=lnm$lookup(r, RT, tabnamlen, tablename, nt);
  if (status==SS$_NORMAL) RT->context[RT->depth]=r->mylnmb;
  else return status;
  /* cache not implemented */
  status=lnm$table_srch(r, RT, nt);
  status=lnm$table(r, RT, nt);
  return status;
}

int lnm$table(struct struct_lnm_ret * r,struct struct_rt * rt, struct struct_nt * nt ) {
  /* cache not implemented */
  int status;
  status=lnm$table_srch(r,rt,nt);
  return status;
}

int lnm$lookup(struct struct_lnm_ret * r,struct struct_rt * rt, int loglen, char * lognam, struct struct_nt * nt) {
  int status;
  struct _pcb * pcb = smp$gl_cpu_data[0]->cpu$l_curpcb;
  void * hash;
  nt->loglen=loglen;
  nt->lognam=lognam;
  lnmprintf("lookup %s %x\n",nt->lognam,nt->loglen);
  nt->lnmb=pcb->pcb$l_ns_reserved_q1;
  hash=pcb->pcb$l_affinity_callback;
  status=lnm$presearch(r,hash,nt);
  if ((status&1)==0) {
    nt->lnmb=lnm$al_dirtbl[0];
    hash=&lnmhshs;
    status=lnm$presearch(r,hash,nt);
  }
  if (status!=SS$_NOLOGNAM) return status;
  return status;
}

int lnm$table_srch(struct struct_lnm_ret * r,struct struct_rt *RT, struct struct_nt * nt) {
  struct _lnmx * lnmx = (r->mylnmb)->lnmb$l_lnmx;
  int len, status;
  do {
    RT->tries--;
    if (!RT->tries) return SS$_TOOMANYLNAM;
    if ((lnmx->lnmx$l_flags)&LNMX$M_TERMINAL)
      RT->flags|=LNMX$M_TERMINAL;
    else
      RT->flags&=~LNMX$M_TERMINAL;

    if (RT->depth>10) return SS$_TOOMANYLNAM;
    RT->context[RT->depth]=(r->mylnmb);
    if (lnmx->lnmx$l_index==LNMX$C_TABLE) {
      return SS$_NORMAL;
    }
    RT->depth++;
    len=lnmx->lnmx$l_xlen;
    lnmprintf("tsr %x %s \n",lnmx->lnmx$t_xlation,lnmx->lnmx$t_xlation);
    status=lnm$lookup(r,RT,len,lnmx->lnmx$t_xlation,nt);
    lnmx=lnmx->lnmx$l_next;
  } while (lnmx);
  if (lnmx && lnmx->lnmx$l_index==LNMX$C_TABLE) {
    RT->depth--;
  }
}

int lnm$inslogtab(struct struct_lnm_ret * r, struct _lnmb * mylnmb) {
  int status;
  unsigned long * myhash;
  myhash=lnmmalloc(sizeof(unsigned long));
  lnmprintf("inslog\n");
  lnmprintf("%x %s\n",mylnmb->lnmb$b_count,&(mylnmb->lnmb$t_name[0]));
  status=lnm$hash(mylnmb->lnmb$b_count,&(mylnmb->lnmb$t_name[0]),0xffff,myhash);
  if (lnmhshs.entry[2*(*myhash)])
    insque(mylnmb,lnmhshs.entry[2*(*myhash)]);
  else {
    lnmhshs.entry[2*(*myhash)]=mylnmb;
    lnmhshs.entry[2*(*myhash)+1]=mylnmb;
    mylnmb->lnmb$l_flink=mylnmb;
    mylnmb->lnmb$l_blink=mylnmb;
  }
}

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

char * search_log_prc(char * name) {
  $DESCRIPTOR(prc,"LNM$PROCESS_TABLE");
  int sts;
  struct item_list_3 itm[2];
  struct dsc$descriptor mytabnam, mynam;
  char resstring[LNM$C_NAMLENGTH]="";
  mynam.dsc$w_length=strlen(name);
  mynam.dsc$a_pointer=name;
  itm[0].item_code=1;
  itm[0].buflen=LNM$C_NAMLENGTH;
  itm[0].bufaddr=resstring;
  bzero(&itm[1],sizeof(struct item_list_3));
  sts = exe$trnlnm(0, &prc, &mynam, 0, itm);
  if (resstring[0]) {
    char * c = kmalloc(strlen(resstring),GFP_KERNEL);
    memcpy(c,resstring,strlen(resstring));
    return c;
  } else
    return name;
}
