#ifndef VCDEF_H
#define VCDEF_H

#include <vms_types.h>

#define vc$c_que_number 4 ;

struct _vcdef
{
    UINT64 vc$q_qlnk;
    UINT16 vc$w_size;
    UINT8 vc$b_type;
    UINT8 vc$b_sub_type;
    union
    {
        UINT16 vc$w_sts;
        struct
        {
            unsigned vc$v_open      :1;
            unsigned vc$v_dqi       :1;
            unsigned vc$v_path      :1;
            unsigned vc$v_queued    :1;
            unsigned vc$v_tim_rxmt  :1;
            unsigned vc$v_pipe_quota :1;
            unsigned vc$v_rwait     :1;
            unsigned vc$v_restart   :1;
        };
    };
    UINT8 vc$b_node_hw_type;
    union
    {
        UINT8 vc$b_wrk;
        struct
        {
            unsigned vc$v_wrk_sseq  :1;
            unsigned vc$v_wrk_xmt   :1;
            unsigned vc$v_wrk_sack  :1;
            unsigned vc$v_wrk_abuf  :1;
        };
    };
    struct _chdef * vc$l_preferred_channel;
    unsigned long vc$l_delay_time;
    unsigned long vc$l_buffer_size;
    unsigned long vc$l_fill_longword;
    unsigned long vc$l_channel_count;
    unsigned long vc$l_channel_selection_count;
    UINT64 vc$q_open_time;
    UINT64 vc$q_close_time;
    UINT16 vc$w_tim_xack;
    UINT16 vc$w_tim_rack;
    unsigned long vc$l_ret_id_attempts;
    unsigned long vc$l_ret_ids_sent;
    unsigned long vc$l_topology_change;
    unsigned long vc$l_npagedyn_low;
    unsigned long vc$l_xmt_msg;
    unsigned long vc$l_xmt_unseq;
    unsigned long vc$l_xmt_seq;
    unsigned long vc$l_xmt_ack;
    unsigned long vc$l_xmt_rexmt;
    unsigned long vc$l_xmt_bytes;
    unsigned long vc$l_xmt_noxch;
    unsigned long vc$l_rcv_msg;
    unsigned long vc$l_rcv_unseq;
    unsigned long vc$l_rcv_seq;
    unsigned long vc$l_rcv_ack;
    unsigned long vc$l_rcv_rercv;
    unsigned long vc$l_rcv_bytes;
    unsigned long vc$l_rcv_cache;
    unsigned long vc$l_tr_pipe_quota;
    UINT16 vc$w_rcv_tr_short;
    UINT16 vc$w_rcv_ill_ack;
    UINT16 vc$w_rcv_ill_seq;
    UINT16 vc$w_rcv_bad_cksum;
    UINT16 vc$w_xmt_seq_tmo;
    UINT16 vc$w_tr_dfq_empty;
    UINT16 vc$w_tr_mfq_empty;
    UINT16 vc$w_cc_dfq_empty;
    UINT16 vc$w_cc_mfq_empty;
    UINT16 vc$w_fill_word;
    void * vc$l_hash_link;
    char nodename[8];
    unsigned long vc$l_remsys_lo;
    UINT16 vc$w_remsys_hi;
    unsigned char vc$w_port_inx;
    unsigned char vc$w_retries;
    void * vc$l_buf_action;
    void * vc$l_buf_rcv;
    void * vc$l_buf_first;
    void * vc$l_buf_last;
    void * vc$l_buf_rexmt;
    UINT16 vc$w_cmdq_len ;
    UINT16 vc$w_haa;
    UINT16 vc$w_lar;
    UINT16 vc$w_hsr;
    UINT16 vc$w_nsu;
    UINT16 vc$w_rsvp_thresh;
    unsigned char vc$w_pipe_quota;
    unsigned char vc$w_mask_que;
    UINT16 vc$w_max_cmd_len;
    unsigned long vc$l_cache_mask;
    UINT64 vc$q_rwaitq ;
    UINT64 vc$q_cmdq3 ;
    UINT64 vc$q_cmdq2 ;
    UINT64 vc$q_cmdqhi ;
    UINT64 vc$q_cmdqlo ;
    UINT64 vc$q_active_channels;
    UINT64 vc$q_dead_channels;
    union
    {
        unsigned long vc$l_maint;
        struct
        {
            unsigned ch$v_maint_id  : 31;
            unsigned ch$v_d  :1;
        };
    };
    unsigned char vc$w_minor;
    unsigned char vc$w_major;
    UINT16 vc$w_eco;
    unsigned long vc$l_port_fcn;
    union
    {
        unsigned long vc$l_sta_info;
        struct
        {
            unsigned ch$v_spare : 8;
            unsigned ch$v_m  :1;
            unsigned ch$v_ps  : 2;
            unsigned ch$v_sys_state  : 21;
        };
    };
    unsigned long vc$l_srv;
    unsigned long vc$l_cache_miss;
    unsigned long vc$l_rexmt_requests;
};



#endif
