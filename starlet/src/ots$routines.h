/* <ots$routines.h>
 *
 *	Compiler support run-time library routines.
 */
#ifndef _OTS$ROUTINES_H
#define _OTS$ROUTINES_H
# ifdef __cplusplus
extern "C" {
# endif


unsigned long ots$cvt_t_d(const void *,double *,...),
	      ots$cvt_t_f(const void *,float *,...),
	      ots$cvt_t_g(const void *,double *,...),
	      ots$cvt_t_h(const void *,void *,...);


 
 



unsigned long ots$div_pk_long(const void *,const void *,short,void *,short,short,short),
	      ots$div_pk_short(const void *,const void *,short,void *,short,short);



void ots$move3(int,const void *,void *),
     ots$move5(int,const void *,int,int,void *);


 


unsigned long ots$cnvout(const double *,void *,unsigned),
	      ots$cnvout_g(const double *,void *,unsigned),
	      ots$cnvout_h(const void *,void *,unsigned);


 
 

 
 






double ots$powdd(double,double),
       ots$powdj(double,long),
       ots$powdr(double,float),
       ots$powgg(double,double),
       ots$powgj(double,long);



 




short ots$powii(short,short);
long ots$powjj(long,long);
unsigned long ots$powlulu(unsigned long,unsigned long);




double ots$powrd(float,double);
float ots$powrj(float,long), ots$powrr(float,float);



float ots$powrlu(float,unsigned long);
double ots$powdlu(double,unsigned long), ots$powglu(double,unsigned long);
 



unsigned short ots$scopy_dxdx(const void *,void *),
	       ots$scopy_r_dx(unsigned short,const void *,void *);



void ots$sfree1_dd(void *),
     ots$sfreen_dd(unsigned,void *),
     ots$sget1_dd(unsigned short,void *);







unsigned long ots$cvt_l_tb(const void *,void *,...),
	      ots$cvt_l_ti(const void *,void *,...),
	      ots$cvt_l_tl(const long *,void *),
	      ots$cvt_l_to(const void *,void *,...),
	      ots$cvt_l_tu(const void *,void *,...),
	      ots$cvt_l_tz(const void *,void *,...);







unsigned long ots$cvt_tb_l(const void *,void *,...),
	      ots$cvt_ti_l(const void *,void *,...),
	      ots$cvt_tl_l(const void *,void *,...),
	      ots$cvt_to_l(const void *,void *,...),
	      ots$cvt_tu_l(const void *,void *,...),
	      ots$cvt_tz_l(const void *,void *,...);

# ifdef __cplusplus
}
# endif
#endif	
