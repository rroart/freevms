#ifndef lnmsub_h
#define lnmsub_h

#include <lnmstrdef.h>

int lnm$hash(const int length, const unsigned char * log, const unsigned long mask, unsigned long * myhash);
int lnm$searchlog(struct struct_lnm_ret * r,int loglen, char * logical, int  tabnamlen, char * tablename) ;
int lnm$search_one(struct struct_lnm_ret * r,int loglen, char * logical, int  tabnamlen, char * tablename, char * result) ;
int lnm$presearch(struct struct_lnm_ret * r,struct lnmhshs * hashtable, struct struct_nt * nt) ;
int lnm$contsearch(struct struct_lnm_ret * r, int hash, struct lnmhshs * hashtable, struct struct_nt * nt) ;
int lnm$firsttab(struct struct_lnm_ret * r,int  tabnamlen,  char * tablename) ;
int lnm$setup(struct struct_lnm_ret * r,struct struct_rt * RT, int tabnamlen,  char * tablename, struct struct_nt * nt) ;
int lnm$table(struct struct_lnm_ret * r,struct struct_rt * rt, struct struct_nt * nt) ;
int lnm$lookup(struct struct_lnm_ret * r,struct struct_rt * rt, int loglen, char * lognam, struct struct_nt * nt) ;
int lnm$table_srch(struct struct_lnm_ret * r,struct struct_rt *rt, struct struct_nt * nt) ;
int lnm$inslogtab(struct struct_lnm_ret * r, struct _lnmb * mylnmb);
int lnm$delete_lnmb();

#endif
