#include"sysgen.h"
#include"lnmsub.h"
#include"../../lbr/src/queue.h"
#include"../../lib/src/lnmstrdef.h"
#include"../../starlet/src/ssdef.h"

/* Author: Roar Thronæs */

/* << does not rotate ... ? */

int lnm$hash(const int length, const unsigned char * log, const unsigned long mask, unsigned long * myhash) {
  unsigned char *tmp=log;
  unsigned char count=length;
  unsigned long hash=count;
  printf("count %x %x %s\n",count,tmp,tmp);
  while (count>3) {
    //  printf("here %x %x\n",tmp[0],count);
    //tmp[0]=tmp[0]&223;
    //tmp[0]&=223;
    //  printf("here %x %x\n",tmp[0],count);
    //tmp[1]&=223;
    //  printf("here %x %x\n",tmp[0],count);
    //tmp[2]&=223;
    //  printf("here %x %x\n",tmp[0],count);
    //tmp[3]&=223;
    //  printf("here %x %x\n",tmp[0],count);
    hash=hash^(tmp[0]&223)^(tmp[1]&223)^(tmp[2]&223)^(tmp[3]&223);
    //  printf("here %x %x\n",tmp[0],count);
    count-=4;
    tmp+=4;
    hash=hash<<9;
  };
  //printf("here4 %x\n",hash);
  while (count) {
    //*tmp&=223;
    hash=hash^((*tmp)&223);
    count--;
    tmp++;
    //hash=hash<<13;
    //printf("here5 %x\n",hash);
  };
  hash*=0x71279461;
  hash/=3;
  //printf("here6 %x\n",hash);
  /* 
     hash&=something.LNMHSH$L_MASK;
     Not implemented yet
     */
 hash=hash%LNMSHASHTBL;
 //printf("herei7 %x\n",hash);
 hash=hash&mask;
 //printf("here3 %x\n",hash);
 *myhash=hash;
 //printf("here2 %x %x\n",myhash,hash);
 return SS$_NORMAL;
}

int lnm$searchlog(int loglen, char * logical, int tabnamlen, char * tablename,struct lnmb ** mylnmb) {
  int status;
  /* local stack */
  /*
    no process table yet
    lnm$presearch();
  */
  printf("searchlog\n");
  status=lnm$presearch(&lnmhshs, loglen, logical,mylnmb);
  if (status==SS$_NOLOGNAM) return status;
  // lnm$setup();
  // lnm$table();
  printf("searchlogexit\n");
  exit(1);
}

int lnm$search_one(int loglen, char * logical, int tabnamlen, char * tablename, char * result) {
  /* lock mutex */
  // lnm$searchlog();
  printf("searchoneexit\n");
  exit(1);
  /* unlock */
}

int lnm$presearch(struct lnmhshs * hashtable,int loglen,  char * logical, struct lnmb ** alnmb) {
  int status;
  unsigned long * myhash;
  struct lnmb * mylnmb;
  myhash=lnmmalloc(sizeof(unsigned long));
  printf("presearch %x %s\n",loglen,logical);
  printf("presearch %x %s\n",loglen,logical);
  status=lnm$hash(loglen,logical,0xffff,myhash);
  printf("presearch %s\n",logical);
  status=lnm$contsearch(loglen,logical,*myhash,hashtable,&mylnmb);
  printf("presearch %s\n",logical);
  *alnmb=mylnmb;
  lnmfree(myhash);
  return status;
}

int lnm$contsearch(int loglen, char * logical, int hash, struct lnmhshs * hashtable, struct lnmb ** mylnmb) {
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
	  *mylnmb=tmp;
	  return SS$_NORMAL;
	}
      }
      /* no case-blind search yet */
      tmp=tmp->lnmb$l_flink;
    } while (tmp!=head) ;
  return SS$_NOLOGNAM;
}

int lnm$firsttab(int  tabnamlen,  char * tablename, struct lnmb ** mylnmb, struct lnmth ** mylnmth) {
  struct struct_rt * MYRT;
  MYRT=(struct struct_rt *)lnmmalloc(sizeof(struct struct_rt));
  printf("firstab %s\n",tablename);
  lnm$setup(MYRT,tabnamlen,tablename,mylnmb,mylnmth);
  lnmfree(MYRT);
}

int lnm$setup(struct struct_rt * RT, int tabnamlen,  char * tablename, struct lnmb ** mylnmb, struct lnmth ** mylnmth) {
  int status;
  RT->depth=0;
  RT->tries=255;
  printf("lnm$setup %x %s\n",tabnamlen, tablename);
  status=lnm$lookup(RT, tabnamlen, tablename, mylnmb,mylnmth);
  if (status==SS$_NORMAL) RT->context[RT->depth]=(*mylnmb)->lnmb$t_name;
  else return status;
  /* cache not implemented */
  status=lnm$table(RT,tabnamlen,tablename,mylnmb,mylnmth);
  return status;
}

int lnm$table(struct struct_rt * RT, int tabnamlen, char * tablename, struct lnmb ** mylnmb, struct lnmth ** mylnmth) {
  /* cache not implemented */
  int status;
  status=lnm$table_srch(RT,tabnamlen,tablename,mylnmb,mylnmth);
  return status;
}

int lnm$lookup(struct struct_rt * RT,int loglen, char * logical, struct lnmb ** mylnmb, struct lnmth ** mylnmth) {
  int status;
  printf("lookup %s %x\n",logical,loglen);
  status=lnm$presearch(&lnmhshs,loglen,logical,mylnmb);
  if (status!=SS$_NOLOGNAM) return status;
  return status;
}

int lnm$table_srch(struct struct_rt *RT, int tabnamlen,  char * tablename, struct lnmb ** mylnmb, struct lnmth ** mylnmth) {
  int xname=0;
  int len;
  do {
    RT->tries--;
    if (!RT->tries) return SS$_TOOMANYLNAM;
    if (((*mylnmb)->lnmxs[xname].lnmx$b_flags)&LNM$M_MYTERMINAL) {
      RT->context[RT->depth]=&((*mylnmb)->lnmxs[xname].lnmx$b_count);
    }
    if (RT->depth>10) return SS$_TOOMANYLNAM;
    RT->depth++;
    len=(*mylnmb)->lnmxs[xname].lnmx$b_count;
    printf("tsr %x %s \n",(*mylnmb)->lnmxs[xname].lnmx$t_xlation,(*mylnmb)->lnmxs[xname].lnmx$t_xlation);
    lnm$lookup(RT,len,(*mylnmb)->lnmxs[xname].lnmx$t_xlation,mylnmb,mylnmth);
    xname++;
  } while (xname<20 && !((*mylnmb)->lnmxs[xname].lnmx$b_flags)&LNM$M_MYXEND);
  if ((*mylnmb)->lnmxs[xname].lnmx$b_index==LNM$C_TABLE) {
    RT->depth--;
  }



}

int lnm$inslogtab(int tabnamlen,  char * tablename,struct lnmb ** mylnmb,struct lnmth ** mylnmth) {
  int status;
  unsigned long * myhash;
  myhash=lnmmalloc(sizeof(unsigned long));
  printf("inslog\n");
  printf("%x %s\n",tabnamlen,tablename);
  status=lnm$hash(tabnamlen,tablename,0xffff,myhash);
  printf("inslog\n");
  printf("inslog myhash %x\n",*myhash);
  if (lnmhshs.entry[2*(*myhash)])
    insque(mylnmb,lnmhshs.entry[2*(*myhash)]);
  else {
    lnmhshs.entry[2*(*myhash)]=mylnmb;
    lnmhshs.entry[2*(*myhash)+1]=mylnmb;
  }
  printf("inslog\n");
  
  ; }

int lnm$check_prot() { ; }
