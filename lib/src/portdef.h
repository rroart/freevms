#ifndef PORTDEF_H
#define PORTDEF_H

#include <vms_types.h>

#define PORT$C_ERRLOG_THRES            0x0A
#define PORT$C_ERR_RATE_INT            0x03
#define PORT$C_INIT_LBUF_MAX           0x0180
#define PORT$C_INIT_LBUF_QUO           0x01
#define PORT$C_INIT_SBUF_MAX           0x0300
#define PORT$C_INIT_SBUF_QUO           0x08
#define PORT$C_LENGTH                  0x0264
#define PORT$C_QUE_NUMBER              0x0A
#define PORT$C_SBUF_QUO_INCR           0x01
#define PORT$C_VC_MAX                  0x0100

#define PORT$K_FIRST_CYCL_WRK          0x01
#define PORT$K_FIRST_WRK               0x00
#define PORT$K_LAST_WRK                0x04

#define PORT$M_AUTHORIZE               0x01
#define PORT$M_DISABLED                0x40
#define PORT$M_FORK                    0x02
#define PORT$M_NEED_DFQ                0x20
#define PORT$M_NEED_LBUF               0x04
#define PORT$M_NEED_MFQ                0x10
#define PORT$M_NEED_SBUF               0x08
#define PORT$M_WRK_LDL                 0x08
#define PORT$M_WRK_RWAITQ              0x01
#define PORT$M_WRK_VCQ                 0x02

#define PORT$Q_AUTHORIZE               0x80
#define PORT$Q_BUS_LIST                0x70

#define PORT$S_HASH                    0x06
#define PORT$S_NODENAME                0x08

#define PORT$V_AUTHORIZE               0x00
#define PORT$V_DISABLED                0x06
#define PORT$V_FORK                    0x01
#define PORT$V_HASH                    0x20
#define PORT$V_NEED_DFQ                0x05
#define PORT$V_NEED_LBUF               0x02
#define PORT$V_NEED_MFQ                0x04
#define PORT$V_NEED_SBUF               0x03
#define PORT$V_WRK_INTR                0x02
#define PORT$V_WRK_LDL                 0x03
#define PORT$V_WRK_RWAITQ              0x00
#define PORT$V_WRK_TIMER               0x04
#define PORT$V_WRK_VCQ                 0x01

struct _port
{

    void * port$a_portqb;
    void * port$a_intr_srv;
    UINT16 port$w_short;
    UINT8 port$b_type;
    UINT8 port$b_sub_type;
    UINT8 port$b_vc_num;
    UINT8 port$b_vc_cnt;
    UINT8 port$b_vc_last;
    UINT8 port$b_max_cache;
    UINT64 port$q_fork;
    UINT16 port$w_sts;
    UINT8 port$b_max_rexmt;
    UINT8 port$b_flck;
    void * port$a_fpc;
    unsigned long port$l_fr3;
    unsigned long port$l_fr4;
    union
    {
        UINT64 port$q_vc_work;
        UINT64 port$q_que_first;
    }
    UINT64 port$q_sbuf_free;
    UINT64 port$q_lbuf_free;
    UINT64 port$q_dfq;
    UINT64 port$q_mfq;
    UINT64 port$q_xmt_ldl;
    UINT64 port$q_rwaitq;
    UINT64 port$q_rspq;
    UINT64 port$q_bus_list;
    unsigned long port$l_secs_zeroed;
    UINT16 port$w_clock;
    UINT16 port$w_wrk;
    UINT64 port$q_authorize;
    unsigned long port$l_services;
    UINT16 port$w_max_lngmsg;
    UINT16 port$w_max_lngdg;
    void * port$a_vcvec0;
    UINT16 port$w_delay_ack;
    UINT16 port$w_delay_seq;
    UINT8 port$t_nodename[8];
    UINT16 port$w_sbuf_size;
    UINT16 port$w_lbuf_size;
    UINT16 port$w_sbuf_cnt;
    UINT16 port$w_sbuf_quo;
    UINT16 port$w_sbuf_max;
    UINT16 port$w_lbuf_cnt;
    UINT16 port$w_lbuf_quo;
    UINT16 port$w_lbuf_max;
    unsigned long port$l_tot_lbuf_allocs;
    unsigned long port$l_tot_sbuf_allocs;
    UINT16 port$w_lbuf_lookaside_miss;
    UINT16 port$w_sbuf_lookaside_miss;
    UINT16 port$w_sbuf_inuse_cnt;
    UINT16 port$w_sbuf_inuse_peak;
    UINT16 port$w_sysid_hi;
    UINT16 port$w_lbuf_inuse_cnt;
    UINT16 port$w_lbuf_inuse_peak;
    UINT16 port$w_pci_sbuf_empty;
    UINT16 port$w_pci_lbuf_empty;
    UINT16 port$w_tr_sbuf_empty;
    UINT16 port$w_bus_count;
    void * port$a_link;
    UINT8 port$b_scan_xack;
    UINT8 port$b_scan_xseq;
    UINT8 port$b_scan_sbuf;
    UINT8 port$b_need_sack;
    unsigned long port$l_sysid_lo;
    UINT16 port$w_sysid_hi;
    UINT16 port$w_group;
    unsigned long port$l_mcast_lo;
    UINT16 port$w_mcast_hi;
    UINT8 port$b_tim_err_log;
    unsigned long port$l_maint;
    UINT8 port$b_minor;
    UINT8 port$b_major;
    UINT16 port$w_eco;
    unsigned long port$l_port_fcn;
    unsigned long port$l_sta_info;
    UINT8 port$t_rstport[6];
    UINT16 port$w_filler;
    UINT8 port$t_mask_sbuf[32];
    UINT8 port$t_mask_xack[32];
    UINT8 port$t_mask_xseq[32];
    UINT8 port$al_vec0_hash[256];
    void * port$a_pdt;
    void * port$a_ucb;
}


#endif
