#ifndef CHDEF_H
#define CHDEF_H

#include <vms_types.h>

#define ch$c_rexmt_penalty 500
#define ch$c_xmtfail_penalty 10000

struct _chdef
{
    UINT64 ch$q_timer_entry;
    UINT16 ch$w_size;
    UINT8 ch$b_type;
    UINT8 ch$b_sub_type;
    UINT16 ch$w_state;
    union
    {
        UINT8 ch$b_sts;
        struct
        {
            unsigned ch$v_path :1;
            unsigned ch$v_open :1;
            unsigned ch$v_xmt_chaining_disabled :1;
            unsigned ch$v_rmt_hwa_valid :1;
        };
    };
    UINT8 ch$b_ring_index;
    void * ch$l_bus;
    void * ch$l_vc;
    UINT64 ch$q_channel_list;
    void * ch$l_ch_hash_link;
    unsigned long ch$l_average_xmt_time;
    UINT16 ch$w_rsvp_thresh;
    UINT8 ch$b_remote_ring_size;
    UINT8 ch$b_remote_device_type;
    UINT16 ch$w_lcl_chan_seq_num;
    UINT16 ch$w_rmt_chan_seq_num;
    UINT16 ch$w_remote_buffer_size;
    UINT16 ch$w_max_buffer_size;
    unsigned long ch$l_supported_services;
    UINT64 ch$q_remote_net_address;
    UINT64 ch$q_open_time;
    UINT64 ch$q_close_time;
    unsigned long ch$l_best_channel_count;
    unsigned long ch$l_preferred_channel_cnt;
    unsigned long ch$l_rexmt_penalty;
    unsigned long ch$l_xmtfail_penalty;
    unsigned long ch$l_xmt_msgs;
    unsigned long ch$l_xmt_bytes;
    unsigned long ch$l_xmt_ctrl_msgs;
    unsigned long ch$l_xmt_ctrl_bytes;
    unsigned long ch$l_rcv_mc_msgs;
    unsigned long ch$l_rcv_mc_bytes;
    unsigned long ch$l_rcv_ctrl_msgs;
    unsigned long ch$l_rcv_ctrl_bytes;
    unsigned long ch$l_rcv_msgs;
    unsigned long ch$l_rcv_bytes;
    void * ch$l_clst;
    UINT8 ch$b_minor;
    UINT8 ch$b_major;
    UINT16 ch$w_eco;
    unsigned char ch$ab_rmt_dev_name[16];
    unsigned char ch$ab_rmt_lan_hwa[6];
    UINT8 ch$b_rmt_node_hw_type;
    UINT8 ch$b_fill_byte;
    UINT16 ch$w_cc_hs_tmo;
    UINT16 ch$w_cc_listen_tmo;
    UINT16 ch$w_cc_rcv_bad_authorize;
    UINT16 ch$w_cc_rcv_bad_eco;
    UINT16 ch$w_cc_rcv_bad_mc;
    UINT16 ch$w_cc_rcv_short_msg;
    UINT16 ch$w_cc_rcv_incompat_channel;
    UINT16 ch$w_tr_rcv_old_msg;
    UINT16 ch$w_cc_no_mscp_server;
    UINT16 ch$w_cc_disk_not_served;
    UINT16 ch$w_topology_change;
};

#endif
