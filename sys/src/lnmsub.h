#ifndef lnmsub_h
#define lnmsub_h

#include"../../lib/src/lnmstrdef.h"

int lnm$hash(const int length, const unsigned char * log, const unsigned long mask, unsigned long * myhash);
int lnm$searchlog(int loglen, char * logical, int  tabnamlen, char * tablename,struct lnmb ** mylnmb) ;
int lnm$search_one(int loglen, char * logical, int  tabnamlen, char * tablename, char * result) ;
int lnm$presearch(struct lnmhshs * hashtable,int  loglen,  char * logical, struct lnmb ** alnmb) ;
int lnm$contsearch(int  loglen, char * logical, int hash, struct lnmhshs * hashtable, struct lnmb ** mylnmb) ;
int lnm$firsttab(int  tabnamlen,  char * tablename, struct lnmb ** mylnmb, struct lnmth ** mylnmth) ;
int lnm$setup(struct struct_rt * RT, int tabnamlen,  char * tablename, struct lnmb ** mylnmb, struct lnmth ** mylnmth) ;
int lnm$table(struct struct_rt * RT, int tabnamlen, char * tablename, struct lnmb ** mylnmb, struct lnmth ** mylnmth) ;
int lnm$lookup(struct struct_rt * RT, int loglen, char * logical, struct lnmb ** mylnmb, struct lnmth ** mylnmth) ;
int lnm$table_srch(struct struct_rt *RT, int tabnamlen,  char * tablename, struct lnmb ** mylnmb, struct lnmth ** mylnmth) ;
int lnm$inslogtab(int tabnamlen,  char * tablename,struct lnmb ** mylnmb,struct lnmth ** mylnmth);
int lnm$delete_lnmb();

#endif
