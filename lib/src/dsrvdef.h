#ifndef DSRVDEF_H
#define DSRVDEF_H

#include <vms_types.h>

#define     DSRV$M_LOG_ENABLD   0x1
#define     DSRV$M_LOG_PRESENT  0x2
#define     DSRV$M_PKT_LOGGED   0x4
#define     DSRV$M_PKT_LOST     0x8
#define     DSRV$M_LBSTEP1      0x10
#define     DSRV$M_LBSTEP2      0x20
#define     DSRV$M_LBEVENT      0x40
#define     DSRV$M_HULB_DEL     0x80
#define     DSRV$M_MON_ACTIVE   0x100
#define     DSRV$M_LB_REQ       0x200
#define     DSRV$M_CONFIG_WAIT  0x400
#define     DSRV$C_LENGTH       2968
#define     DSRV$K_LENGTH       2968
#define     DSRV$K_AR_ADD       2
#define     DSRV$K_MAX_UNITS    512
#define     DSRV$S_DSRVDEF      2968

struct _dsrv
{
    void *dsrv$l_flink;
    void *dsrv$l_blink;
    UINT16 dsrv$w_size;
    UINT8 dsrv$b_type;
    UINT8 dsrv$b_subtype;
    union
    {
        UINT16 dsrv$w_state;
        struct
        {
            unsigned dsrv$v_log_enabld : 1;
            unsigned dsrv$v_log_present : 1;
            unsigned dsrv$v_pkt_logged : 1;
            unsigned dsrv$v_pkt_lost : 1;

            unsigned dsrv$v_lbstep1 : 1;
            unsigned dsrv$v_lbstep2 : 1;
            unsigned dsrv$v_lbevent : 1;

            unsigned dsrv$v_hulb_del : 1;
            unsigned dsrv$v_mon_active : 1;
            unsigned dsrv$v_lb_req : 1;
            unsigned dsrv$v_config_wait : 1;
            unsigned dsrv$v_fill_2_ : 5;
        };
    };
    UINT16 dsrv$w_bufwait;
    void *dsrv$l_log_buf_start;
    void *dsrv$l_log_buf_end;
    void *dsrv$l_next_read;
    void *dsrv$l_next_write;
    UINT16 dsrv$w_inc_lolim;
    UINT16 dsrv$w_inc_hilim;
    UINT16 dsrv$w_exc_lolim;
    UINT16 dsrv$w_exc_hilim;
    void *dsrv$l_srvbuf;
    void *dsrv$l_free_list;
    UINT32 dsrv$l_avail;
    UINT32 dsrv$l_buffer_min;
    UINT32 dsrv$l_splitxfer;
    struct
    {
        UINT16 dsrv$w_version;
        UINT16 dsrv$w_cflags;
        UINT16 dsrv$w_ctimo;
        UINT16 dsrv$w_reserved;
    };
    UINT64 dsrv$q_ctrl_id;
    UINT32 dsrv$l_memw_tot;
    UINT16 dsrv$w_memw_cnt;
    UINT16 dsrv$w_memw_max;
    void *dsrv$l_memw_fl;
    void *dsrv$l_memw_bl;
    UINT16 dsrv$w_num_host;
    UINT16 dsrv$w_num_unit;
    void *dsrv$l_hqb_fl;
    void *dsrv$l_hqb_bl;
    void *dsrv$l_uqb_fl;
    void *dsrv$l_uqb_bl;
    UINT16 dsrv$w_load_avail;
    UINT16 dsrv$w_load_capacity;
    UINT16 dsrv$w_lbload;
    UINT16 dsrv$w_lbresp;
    UINT16 dsrv$w_lm_load1;
    UINT16 dsrv$w_lm_load2;
    UINT16 dsrv$w_lm_load3;
    UINT16 dsrv$w_lm_load4;
    UINT16 dsrv$w_lbinit_cnt;
    UINT16 dsrv$w_lbfail_cnt;
    UINT16 dsrv$w_lbreq_cnt;
    UINT16 dsrv$w_lbresp_cnt;
    UINT32 dsrv$l_lbreq_time;
    UINT32 dsrv$l_lbmon_time;
    struct _fkb *dsrv$l_lm_fkb;
    struct _fkb *dsrv$l_lb_fkb;
    UINT16 dsrv$w_lm_interval;
    UINT8 dsrv$b_lb_count1;
    UINT8 dsrv$b_lb_count2;
    void *dsrv$l_hulb_fl;
    void *dsrv$l_hulb_bl;
    UINT8 dsrv$b_hosts [32];
    void *dsrv$l_units [512];
    struct
    {
        UINT32 dsrv$l_opcount;
        UINT32 dsrv$l_abort_cnt;
        UINT32 dsrv$l_get_cmd_cnt;
        UINT32 dsrv$l_get_unt_cnt;
        UINT32 dsrv$l_set_con_cnt;
        UINT32 dsrv$l_acc_nvm_cnt;
        UINT32 dsrv$l_display_cnt;
        UINT32 dsrv$l_get_unn_cnt;
        UINT32 dsrv$l_avail_cnt;
        UINT32 dsrv$l_onlin_cnt;
        UINT32 dsrv$l_set_unt_cnt;
        UINT32 dsrv$l_det_acc_cnt;
        UINT32 dsrv$l_move_cnt;
        UINT32 dsrv$l_dcd_cnt;
        UINT32 dsrv$l_reserved14;
        UINT32 dsrv$l_reserved15;
        UINT32 dsrv$l_acces_cnt;
        UINT32 dsrv$l_cmp_con_cnt;
        UINT32 dsrv$l_erase_cnt;
        UINT32 dsrv$l_flush_cnt;
        UINT32 dsrv$l_replc_cnt;
        UINT32 dsrv$l_reserved21;
        UINT32 dsrv$l_eraseg_cnt;
        UINT32 dsrv$l_reserved22;
        UINT32 dsrv$l_format_cnt;
        UINT32 dsrv$l_wri_his_cnt;
        UINT32 dsrv$l_reserved26;
        UINT32 dsrv$l_reserved27;
        UINT32 dsrv$l_reserved28;
        UINT32 dsrv$l_reserved29;
        UINT32 dsrv$l_reserved30;
        UINT32 dsrv$l_reserved31;
        UINT32 dsrv$l_cmp_hst_cnt;
        UINT32 dsrv$l_read_cnt;
        UINT32 dsrv$l_write_cnt;
        UINT32 dsrv$l_rea_ced_cnt;
        UINT32 dsrv$l_wri_tm_cnt;
        UINT32 dsrv$l_repos_cnt;
        UINT32 dsrv$l_reserved38;
        UINT32 dsrv$l_reserved39;
        UINT32 dsrv$l_reserved40;
        UINT32 dsrv$l_reserved41;
        UINT32 dsrv$l_reserved42;
        UINT32 dsrv$l_reserved43;
        UINT32 dsrv$l_reserved44;
        UINT32 dsrv$l_reserved45;
        UINT32 dsrv$l_reserved46;
        UINT32 dsrv$l_reserved47;
        UINT32 dsrv$l_terco_cnt;
    };
    UINT32 dsrv$l_vcfail_cnt;
    UINT32 dsrv$l_blkcount [129];
    UINT32 dsrv$l_pcb;
    UINT32 dsrv$l_hrb_tmo_cntr;
};

#endif

