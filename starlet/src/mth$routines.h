/* <mth$routines.h>
 *
 *	Math run-time library routines.
 */
#ifndef _MTH$ROUTINES_H
#define _MTH$ROUTINES_H

float mth$acos();
double mth$dacos(), mth$gacos();

float mth$acosd();
double mth$dacosd(), mth$gacosd();

float mth$asin();
double mth$dasin(), mth$gasin();

float mth$asind();
double mth$dasind(), mth$gasind();

float mth$atan();
double mth$datan(), mth$gatan();

float mth$atan2();
double mth$datan2(), mth$gatan2();

float mth$atand();
double mth$datand(), mth$gatand();

float mth$atanh();
double mth$datanh(), mth$gatanh();

float mth$atand2();
double mth$datand2(), mth$gatand2();



long blas1$visamax(), blas1$vidamax(), blas1$vigamax(),
     blas1$vicamax(), blas1$vizamax(), blas1$viwamax();



float blas1$vsasum();
double blas1$vdasum(), blas1$vgasum();




int blas1$vsaxpy(), blas1$vdaxpy(), blas1$vgaxpy(),
    blas1$vcaxpy(), blas1$vzaxpy(), blas1$vwaxpy();



int blas1$vscopy(), blas1$vdcopy(), blas1$vccopy(), blas1$vzcopy();







float blas1$vsdot();
double blas1$vddot(), blas1$vgdot();





float blas1$vsnrm2();
double blas1$vdnrm2(), blas1$vgnrm2();




int blas1$vsrot(), blas1$vdrot(), blas1$vgrot(),
    blas1$vcsrot(), blas1$vzdrot(), blas1$vwgrot();



int blas1$vsrotg(), blas1$vdrotg(), blas1$vgrotg(),
    blas1$vcrotg(), blas1$vzrotg(), blas1$vwrotg();




int blas1$vsscal(), blas1$vdscal(), blas1$vgscal(),
    blas1$vcscal(), blas1$vcsscal(), blas1$vzscal(),
    blas1$vwscal(), blas1$vzdscal(), blas1$vwgscal();



int blas1$vsswap(), blas1$vdswap(), blas1$vcswap(), blas1$vzswap();


float mth$cabs();
double mth$cdabs(), mth$cgabs();















float mth$cos();
double mth$dcos(), mth$gcos();

float mth$cosd();
double mth$dcosd(), mth$gcosd();

float mth$cosh();
double mth$dcosh(), mth$gcosh();






double mth$cvt_d_g(), mth$cvt_g_d();

double *mth$cvt_da_ga(), *mth$cvt_ga_da();













float mth$exp();

double mth$dexp();

double mth$gexp();

long mth$vjfolrlp_ma_v5();

float mth$vffolrlp_ma_v5();

double mth$vdfolrlp_ma_v5();

double mth$vgfolrlp_ma_v5();

long mth$vjfolrln_ma_v5();

float mth$vffolrln_ma_v5();

double mth$vdfolrln_ma_v5();

double mth$vgfolrln_ma_v5();

long mth$vjfolrlp_m_v2();

float mth$vffolrlp_m_v2();

double mth$vdfolrlp_m_v2();

double mth$vgfolrlp_m_v2();

long mth$vjfolrln_m_v2();

float mth$vffolrln_m_v2();

double mth$vdfolrln_m_v2();

double mth$vgfolrln_m_v2();

long mth$vjfolrlp_a_v2();

float mth$vffolrlp_a_v2();

double mth$vdfolrlp_a_v2();

double mth$vgfolrlp_a_v2();

long mth$vjfolrln_a_v2();

float mth$vffolrln_a_v2();

double mth$vdfolrln_a_v2();

double mth$vgfolrln_a_v2();

int mth$vjfolrp_ma_v15();

int mth$vffolrp_ma_v15();

int mth$vdfolrp_ma_v15();

int mth$vgfolrp_ma_v15();

int mth$vjfolrn_ma_v15();

int mth$vffolrn_ma_v15();

int mth$vdfolrn_ma_v15();

int mth$vgfolrn_ma_v15();

int mth$vjfolrp_m_v8();

int mth$vffolrp_m_v8();

int mth$vdfolrp_m_v8();

int mth$vgfolrp_m_v8();

int mth$vjfolrn_m_v8();

int mth$vffolrn_m_v8() ;

int mth$vdfolrn_m_v8();

int mth$vgfolrn_m_v8();

int mth$vjfolrp_a_v8();

int mth$vffolrp_a_v8();

int mth$vdfolrp_a_v8();

int mth$vgfolrp_a_v8();

int mth$vjfolrn_a_v8();

int mth$vffolrn_a_v8();

int mth$vdfolrn_a_v8();

int mth$vgfolrn_a_v8();

int mth$hacos();

int mth$hacosd();

int mth$hasin();

int mth$hasind();

int mth$hatan();

int mth$hatan2();

int mth$hatand();

int mth$hatanh();

int mth$hcos();

int mth$hcosd();

int mth$hcosh();

int mth$hexp();

int mth$hlog10();

int mth$hlog();

int mth$hlog2();

int mth$hsin();

int mth$hsind();

int mth$hsinh();

int mth$hsqrt();

int mth$htan();

int mth$htand();

int mth$htanh();

int mth$hatand2();


float mth$aimag();
double mth$dimag(), mth$gimag();

float mth$alog();
double mth$dlog(), mth$glog();

float mth$alog10();
double mth$dlog10(), mth$glog10();

float mth$alog2();
double mth$dlog2(), mth$glog2();

float mth$random();

float mth$real();
double mth$dreal(), mth$greal();

float mth$sin();
double mth$dsin(), mth$gsin();

float mth$sind();
double mth$dsind(), mth$gsind();

float mth$sinh();
double mth$dsinh(), mth$gsinh();



int mth$sincos(), mth$dsincos(), mth$gsincos(), mth$hsincos();


int mth$sincosd(), mth$dsincosd(), mth$gsincosd(), mth$hsincosd();


float mth$sqrt();
double mth$dsqrt(), mth$gsqrt();

float mth$tan();
double mth$dtan(), mth$gtan();

float mth$tand();
double mth$dtand(), mth$gtand();

float mth$tanh();
double mth$dtanh(), mth$gtanh();


unsigned long mth$umax();

unsigned long mth$umin();

float mth$abs();
double mth$dabs(), mth$gabs();
int mth$habs();			

short mth$iiabs();
long mth$jiabs();

unsigned short mth$iiand();

unsigned long mth$jiand();

double mth$dble(), mth$gdble();

float mth$dim();
double mth$ddim(), mth$gdim();
int mth$hdim();			

short mth$iidim();
long mth$jidim();

unsigned short mth$iieor();

unsigned long mth$jieor();

short mth$iifix();

long mth$jifix();


float mth$floati();
double mth$dfloti(), mth$gfloti();

float mth$floatj();
double mth$dflotj(), mth$gflotj();


float mth$floor();
double mth$dfloor(), mth$gfloor();
int mth$hfloor();		

float mth$aint();

double mth$dint();

short mth$iidint();

long mth$jidint();

double mth$gint();

short mth$iigint();

long mth$jigint();

int mth$hint();

short mth$iihint();

long mth$jihint();

short mth$iint();

long mth$jint();

unsigned short mth$iior();

unsigned long mth$jior();

float mth$aimax0();

float mth$ajmax0();

short mth$imax0();

long mth$jmax0();

float mth$amax1();

double mth$dmax1();

double mth$gmax1();

int mth$hmax1();

short mth$imax1();

long mth$jmax1();

float mth$aimin0();

float mth$ajmin0();

short mth$imin0();

long mth$jmin0();

float mth$amin1();

double mth$dmin1();

double mth$gmin1();

int mth$hmin1();

short mth$imin1();

long mth$jmin1();


float mth$amod();
double mth$dmod(), mth$gmod();
int mth$hmod();			

short mth$imod();
long mth$jmod();


float mth$anint();
double mth$dnint(), mth$gnint();
int mth$hnint();		

short mth$inint();
long mth$jnint();

short mth$iidnnt();
long mth$jidnnt();

short mth$iignnt();
long mth$jignnt();

short mth$iihnnt();
long mth$jihnnt();

unsigned short mth$inot();

unsigned long mth$jnot();

double mth$dprod(), mth$gprod();

long mth$sgn();

unsigned short mth$iishft();

unsigned long mth$jishft();

float mth$sign();

double mth$dsign();

double mth$gsign();

int mth$hsign();

short mth$iisign();

long mth$jisign();

float mth$sngl(), mth$snglg();

#endif	
