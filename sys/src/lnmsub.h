#ifndef lnmsub_h
#define lnmsub_h

#include <lnmstrdef.h>

int lnm$hash(const int length, const unsigned char * log, const unsigned long mask, unsigned long * myhash);
int lnm$searchlog(struct struct_lnm_ret * r,int loglen, char * logical, int  tabnamlen, char * tablename) ;
int lnm$search_one(struct struct_lnm_ret * r,int loglen, char * logical, int  tabnamlen, char * tablename, char * result) ;
int lnm$presearch(struct struct_lnm_ret * r,struct lnmhshs * hashtable,int  loglen,  char * logical) ;
int lnm$contsearch(struct struct_lnm_ret * r,int  loglen, char * logical, int hash, struct lnmhshs * hashtable) ;
int lnm$firsttab(struct struct_lnm_ret * r,int  tabnamlen,  char * tablename) ;
int lnm$setup(struct struct_lnm_ret * r,struct struct_rt * RT, int tabnamlen,  char * tablename) ;
int lnm$table(struct struct_lnm_ret * r,struct struct_rt * RT, int tabnamlen, char * tablename) ;
int lnm$lookup(struct struct_lnm_ret * r,struct struct_rt * RT, int loglen, char * logical) ;
int lnm$table_srch(struct struct_lnm_ret * r,struct struct_rt *RT, int tabnamlen,  char * tablename) ;
int lnm$inslogtab(struct struct_lnm_ret * r,int tabnamlen,  char * tablename, struct lnmb * mylnmb);
int lnm$delete_lnmb();

#endif
