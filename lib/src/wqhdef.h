#ifndef wqhdef_h
#define wqhdef_h
 
#define WQH$K_LENGTH 16
#define WQH$C_LENGTH 16
#define WQH$S_WQHDEF 16
	
struct _wqh {
 struct _wqh *wqh$l_wqfl;
 struct _wqh *wqh$l_wqbl;
 unsigned long wqh$l_wqcnt;
 unsigned long wqh$l_wqstate;
};
 
#endif
 
