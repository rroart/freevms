#ifndef DDTDEF_H
#define DDTDEF_H

#include <vms_types.h>

#define DDT$M_DIAGBUF64 32768
#define DDT$K_ITCLVL_DRVR 0
#define DDT$K_ITCLVL_MPDEV 4096
#define DDT$K_ITCLVL_HSM 24576
#define DDT$K_ITCLVL_TOP 32767
#define DDT$K_LENGTH_MIN 120

#define DDT$K_LENGTH 152
#define DDT$C_LENGTH 152
#define DDT$S_DDTDEF 152

struct _ddt
{
    UINT16 ddt$w_size;
    UINT8 ddt$b_type;
    UINT8 ddt$b_subtype;
    INT16 ddt$w_intercept_level;
    unsigned long ddt$l_start;
    unsigned long ddt$l_unsolint;
    struct _fdt * ddt$l_fdt; /* functb */
    unsigned long ddt$l_cancel;
    unsigned long ddt$l_regdump;
    unsigned long ddt$l_diagbuf;
    unsigned long ddt$l_errorbuf;
    unsigned long ddt$l_unitinit;
    unsigned long ddt$l_altstart;
    unsigned long ddt$l_mntver;
    unsigned long ddt$l_cloneducb;
    UINT16 ddt$w_fdtsize;
    UINT16 ddt$w_reserved;
    void (*ddt$ps_start_2)(void);
    void (*ddt$ps_start_jsb)(void);
    int (*ddt$ps_ctrlinit_2)(void);
    int (*ddt$ps_unitinit_2)(void);
    int (*ddt$ps_cloneducb_2)(void);
    struct _fdt *ddt$ps_fdt_2;
    void (*ddt$ps_cancel_2)(void);
    void (*ddt$ps_regdump_2)(void);
    void (*ddt$ps_altstart_2)(void);
    void (*ddt$ps_altstart_jsb)(void);
    void (*ddt$ps_mntver_2)(void);
    int (*ddt$ps_mntv_sssc)(void);
    int (*ddt$ps_mntv_for)(void);
    int (*ddt$ps_mntv_sqd)(void);
    int (*ddt$ps_aux_storage)(void);
    int (*ddt$ps_aux_routine)(void);
    void (*ddt$ps_channel_assign_2)(void);
    int (*ddt$ps_cancel_selective_2)(void);
    unsigned int ddt$is_stack_bcnt;
    unsigned int ddt$is_reg_mask;
    void (*ddt$ps_kp_startio)(void);
    int (*ddt$ps_csr_mapping)(void);
    int (*ddt$ps_fast_fdt)(void);
    int (*ddt$ps_pending_io)(void);
    void *ddt$ps_customer;
    int (*ddt$ps_make_devpath)(void);
    int (*ddt$ps_setprfpath)(void);
    int (*ddt$ps_change_preferred)(void);
    int (*ddt$ps_qsrv_helper)(void);
    int (*ddt$ps_qsrv_evnt_notify)(void);
    union
    {
        int (*ddt$ps_mgt_register)(void);
        int (*ddt$ps_configure)(void);
    };
    union
    {
        int (*ddt$ps_mgt_deregister)(void);
        int (*ddt$ps_deconfigure)(void);
    };
};

#endif

