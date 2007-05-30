#include <ddtdef.h>
#include <dptdef.h>
#include <fdtdef.h>

#include <linux/string.h>

// prefix these with driver$ later

inline void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn, unsigned long type) {
  f->fdt$ps_func_rtn[mask]=fn;
  f->fdt$q_valid|=mask;
  if (type)
    f->fdt$q_buffered|=mask;
}

inline void ini_fdt_end(struct _fdt * fdt) {
  // but what will it do?
}

inline void ini_dpt_name(struct _dpt * d, char * n) {
  d->dpt$t_name[0]=strlen(n);
  memcpy(&d->dpt$t_name[1],n,d->dpt$t_name[0]);
}

inline void ini_dpt_adapt(struct _dpt * d, unsigned long type) {
  d->dpt$b_adptype=type;
}

inline void ini_dpt_flags(struct _dpt * d, unsigned long type) {
  d->dpt$l_flags=type;
}


inline void ini_dpt_maxunits(struct _dpt * d, unsigned long type) {
  d->dpt$w_maxunits=type;
}


inline void ini_dpt_ucbsize(struct _dpt * d, unsigned long type) {
  d->dpt$w_ucbsize=type;
}


inline void ini_dpt_struc_init(struct _dpt * d, unsigned long type) {
  d->dpt$ps_init_pd=type;
}


inline void ini_dpt_struc_reinit(struct _dpt * d, unsigned long type) {
  d->dpt$ps_reinit_pd=type;
}


inline void ini_dpt_ucb_crams(struct _dpt * d, unsigned long type) {
  //  d->dpt$iw_ucb_crams=type; not now
}


inline void ini_dpt_defunits(struct _dpt * d, unsigned long type) {
  d->dpt$w_defunits=type;
}


inline void ini_dpt_unload(struct _dpt * d, unsigned long type) {
  d->dpt$ps_unload=type;
}

inline void ini_dpt_adptype(struct _dpt * d, long l) {
  d->dpt$b_adptype=l;
}

inline void ini_dpt_vector(struct _dpt * d, long l) {
  d->dpt$ps_vector=l;
}

inline void ini_dpt_end(struct _dpt * d) {
  //  d->dpt$_=type; ??
}

inline void ini_ddt_ctrlinit(struct _ddt * d, unsigned long type) {
  d->ddt$ps_ctrlinit_2=type;
}

inline void ini_ddt_unitinit(struct _ddt * d, unsigned long type) {
  d->ddt$l_unitinit=type;
}


inline void ini_ddt_start(struct _ddt * d, unsigned long type) {
  d->ddt$ps_start_2=type;
}


inline void ini_ddt_kp_startio(struct _ddt * d, unsigned long type) {
  d->ddt$ps_kp_startio=type;
}


inline void ini_ddt_kp_stack_size(struct _ddt * d, unsigned long type) {
  d->ddt$is_stack_bcnt=type;
}


inline void ini_ddt_kp_reg_mask(struct _ddt * d, unsigned long type) {
  d->ddt$is_reg_mask=type;
}


inline void ini_ddt_cancel(struct _ddt * d, unsigned long type) {
  d->ddt$ps_cancel_2=type;
}


inline void ini_ddt_regdmp(struct _ddt * d, unsigned long type) {
  d->ddt$ps_regdump_2=type;
}


inline void ini_ddt_erlgbf(struct _ddt * d, unsigned long type) {
  d->ddt$l_errorbuf=type;
}


inline void ini_ddt_qsrv_helper(struct _ddt * d, unsigned long type) {
  d->ddt$ps_qsrv_helper=type;
}

inline void ini_ddt_end(struct _ddt * d) {
  // d->ddt$=type; ??
}

inline void dpt_store_isr(long a, long b) {
}

