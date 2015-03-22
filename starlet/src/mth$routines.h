/* <mth$routines.h>
 *
 *  Math run-time library routines.
 */
#ifndef _MTH$ROUTINES_H
#define _MTH$ROUTINES_H

float mth$acos(void);
double mth$dacos(void);
double mth$gacos(void);

float mth$acosd(void);
double mth$dacosd(void);
double mth$gacosd(void);

float mth$asin(void);
double mth$dasin(void);
double mth$gasin(void);

float mth$asind(void);
double mth$dasind(void);
double mth$gasind(void);

float mth$atan(void);
double mth$datan(void);
double mth$gatan(void);

float mth$atan2(void);
double mth$datan2(void);
double mth$gatan2(void);

float mth$atand(void);
double mth$datand(void);
double mth$gatand(void);

float mth$atanh(void);
double mth$datanh(void);
double mth$gatanh(void);

float mth$atand2(void);
double mth$datand2(void);
double mth$gatand2(void);

long blas1$visamax(void);
double blas1$vidamax(void);
double blas1$vigamax(void);
double blas1$vicamax(void);
double blas1$vizamax(void);
double blas1$viwamax(void);

float blas1$vsasum(void);
double blas1$vdasum(void);
double blas1$vgasum(void);

int blas1$vsaxpy(void);
double blas1$vdaxpy(void);
double blas1$vgaxpy(void);
double blas1$vcaxpy(void);
double blas1$vzaxpy(void);
double blas1$vwaxpy(void);

int blas1$vscopy(void);
double blas1$vdcopy(void);
double blas1$vccopy(void);
double blas1$vzcopy(void);

float blas1$vsdot(void);
double blas1$vddot(void);
double blas1$vgdot(void);

float blas1$vsnrm2(void);
double blas1$vdnrm2(void);
double blas1$vgnrm2(void);

int blas1$vsrot(void);
double blas1$vdrot(void);
double blas1$vgrot(void);
double blas1$vcsrot(void);
double blas1$vzdrot(void);
double blas1$vwgrot(void);

int blas1$vsrotg(void);
double blas1$vdrotg(void);
double blas1$vgrotg(void);
double blas1$vcrotg(void);
double blas1$vzrotg(void);
double blas1$vwrotg(void);

int blas1$vsscal(void);
double blas1$vdscal(void);
double blas1$vgscal(void);
double blas1$vcscal(void);
double blas1$vcsscal(void);
double blas1$vzscal(void);
double blas1$vwscal(void);
double blas1$vzdscal(void);
double blas1$vwgscal(void);

int blas1$vsswap(void);
double blas1$vdswap(void);
double blas1$vcswap(void);
double blas1$vzswap(void);

float mth$cabs(void);
double mth$cdabs(void);
double mth$cgabs(void);

float mth$cos(void);
double mth$dcos(void);
double mth$gcos(void);

float mth$cosd(void);
double mth$dcosd(void);
double mth$gcosd(void);

float mth$cosh(void);
double mth$dcosh(void);
double mth$gcosh(void);

double mth$cvt_d_g(void);
double mth$cvt_g_d(void);

double *mth$cvt_da_ga(void);
double *mth$cvt_ga_da(void);

float mth$exp(void);

double mth$dexp(void);

double mth$gexp(void);

long mth$vjfolrlp_ma_v5(void);

float mth$vffolrlp_ma_v5(void);

double mth$vdfolrlp_ma_v5(void);

double mth$vgfolrlp_ma_v5(void);

long mth$vjfolrln_ma_v5(void);

float mth$vffolrln_ma_v5(void);

double mth$vdfolrln_ma_v5(void);

double mth$vgfolrln_ma_v5(void);

long mth$vjfolrlp_m_v2(void);

float mth$vffolrlp_m_v2(void);

double mth$vdfolrlp_m_v2(void);

double mth$vgfolrlp_m_v2(void);

long mth$vjfolrln_m_v2(void);

float mth$vffolrln_m_v2(void);

double mth$vdfolrln_m_v2(void);

double mth$vgfolrln_m_v2(void);

long mth$vjfolrlp_a_v2(void);

float mth$vffolrlp_a_v2(void);

double mth$vdfolrlp_a_v2(void);

double mth$vgfolrlp_a_v2(void);

long mth$vjfolrln_a_v2(void);

float mth$vffolrln_a_v2(void);

double mth$vdfolrln_a_v2(void);

double mth$vgfolrln_a_v2(void);

int mth$vjfolrp_ma_v15(void);

int mth$vffolrp_ma_v15(void);

int mth$vdfolrp_ma_v15(void);

int mth$vgfolrp_ma_v15(void);

int mth$vjfolrn_ma_v15(void);

int mth$vffolrn_ma_v15(void);

int mth$vdfolrn_ma_v15(void);

int mth$vgfolrn_ma_v15(void);

int mth$vjfolrp_m_v8(void);

int mth$vffolrp_m_v8(void);

int mth$vdfolrp_m_v8(void);

int mth$vgfolrp_m_v8(void);

int mth$vjfolrn_m_v8(void);

int mth$vffolrn_m_v8(void);

int mth$vdfolrn_m_v8(void);

int mth$vgfolrn_m_v8(void);

int mth$vjfolrp_a_v8(void);

int mth$vffolrp_a_v8(void);

int mth$vdfolrp_a_v8(void);

int mth$vgfolrp_a_v8(void);

int mth$vjfolrn_a_v8(void);

int mth$vffolrn_a_v8(void);

int mth$vdfolrn_a_v8(void);

int mth$vgfolrn_a_v8(void);

int mth$hacos(void);

int mth$hacosd(void);

int mth$hasin(void);

int mth$hasind(void);

int mth$hatan(void);

int mth$hatan2(void);

int mth$hatand(void);

int mth$hatanh(void);

int mth$hcos(void);

int mth$hcosd(void);

int mth$hcosh(void);

int mth$hexp(void);

int mth$hlog10(void);

int mth$hlog(void);

int mth$hlog2(void);

int mth$hsin(void);

int mth$hsind(void);

int mth$hsinh(void);

int mth$hsqrt(void);

int mth$htan(void);

int mth$htand(void);

int mth$htanh(void);

int mth$hatand2(void);

float mth$aimag(void);
double mth$dimag(void);
double mth$gimag(void);

float mth$alog(void);
double mth$dlog(void);
double mth$glog(void);

float mth$alog10(void);
double mth$dlog10(void);
double mth$glog10(void);

float mth$alog2(void);
double mth$dlog2(void);
double mth$glog2(void);

float mth$random(void);

float mth$real(void);
double mth$dreal(void);
double mth$greal(void);

float mth$sin(void);
double mth$dsin(void);
double mth$gsin(void);

float mth$sind(void);
double mth$dsind(void);
double mth$gsind(void);

float mth$sinh(void);
double mth$dsinh(void);
double mth$gsinh(void);

int mth$sincos(void);
double mth$dsincos(void);
double mth$gsincos(void);
double mth$hsincos(void);

int mth$sincosd(void);
double mth$dsincosd(void);
double mth$gsincosd(void);
double mth$hsincosd(void);

float mth$sqrt(void);
double mth$dsqrt(void);
double mth$gsqrt(void);

float mth$tan(void);
double mth$dtan(void);
double mth$gtan(void);

float mth$tand(void);
double mth$dtand(void);
double mth$gtand(void);

float mth$tanh(void);
double mth$dtanh(void);
double mth$gtanh(void);

unsigned long mth$umax(void);

unsigned long mth$umin(void);

float mth$abs(void);
double mth$dabs(void);
double mth$gabs(void);
int mth$habs(void);

short mth$iiabs(void);
long mth$jiabs(void);

unsigned short mth$iiand(void);

unsigned long mth$jiand(void);

double mth$dble(void);
double mth$gdble(void);

float mth$dim(void);
double mth$ddim(void);
double mth$gdim(void);
int mth$hdim(void);

short mth$iidim(void);
long mth$jidim(void);

unsigned short mth$iieor(void);

unsigned long mth$jieor(void);

short mth$iifix(void);

long mth$jifix(void);

float mth$floati(void);
double mth$dfloti(void);
double mth$gfloti(void);

float mth$floatj(void);
double mth$dflotj(void);
double mth$gflotj(void);

float mth$floor(void);
double mth$dfloor(void);
double mth$gfloor(void);
int mth$hfloor(void);

float mth$aint(void);

double mth$dint(void);

short mth$iidint(void);

long mth$jidint(void);

double mth$gint(void);

short mth$iigint(void);

long mth$jigint(void);

int mth$hint(void);

short mth$iihint(void);

long mth$jihint(void);

short mth$iint(void);

long mth$jint(void);

unsigned short mth$iior(void);

unsigned long mth$jior(void);

float mth$aimax0(void);

float mth$ajmax0(void);

short mth$imax0(void);

long mth$jmax0(void);

float mth$amax1(void);

double mth$dmax1(void);

double mth$gmax1(void);

int mth$hmax1(void);

short mth$imax1(void);

long mth$jmax1(void);

float mth$aimin0(void);

float mth$ajmin0(void);

short mth$imin0(void);

long mth$jmin0(void);

float mth$amin1(void);

double mth$dmin1(void);

double mth$gmin1(void);

int mth$hmin1(void);

short mth$imin1(void);

long mth$jmin1(void);

float mth$amod(void);
double mth$dmod(void);
double mth$gmod(void);
int mth$hmod(void);

short mth$imod(void);
long mth$jmod(void);

float mth$anint(void);
double mth$dnint(void);
double mth$gnint(void);
int mth$hnint(void);

short mth$inint(void);
long mth$jnint(void);

short mth$iidnnt(void);
long mth$jidnnt(void);

short mth$iignnt(void);
long mth$jignnt(void);

short mth$iihnnt(void);
long mth$jihnnt(void);

unsigned short mth$inot(void);

unsigned long mth$jnot(void);

double mth$dprod(void);
double mth$gprod(void);

long mth$sgn(void);

unsigned short mth$iishft(void);

unsigned long mth$jishft(void);

float mth$sign(void);

double mth$dsign(void);

double mth$gsign(void);

int mth$hsign(void);

short mth$iisign(void);

long mth$jisign(void);

float mth$sngl(void);
double mth$snglg(void);

#endif
