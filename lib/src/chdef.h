#ifndef chdef_h
#define chdef_h

#define ch$c_rexmt_penalty 500
#define ch$c_xmtfail_penalty 10000

struct _chdef
{
    unsigned long long ch$q_timer_entry;
    unsigned short ch$w_size;
    unsigned char ch$b_type;
    unsigned char ch$b_sub_type;
    unsigned short ch$w_state;
    union
    {
        unsigned char ch$b_sts;
        struct
        {
            unsigned ch$v_path :1;
            unsigned ch$v_open :1;
            unsigned ch$v_xmt_chaining_disabled :1;
            unsigned ch$v_rmt_hwa_valid :1;
        };
    };
    unsigned char ch$b_ring_index;
    void * ch$l_bus;
    void * ch$l_vc;
    unsigned long long ch$q_channel_list;
    void * ch$l_ch_hash_link;
    unsigned long ch$l_average_xmt_time;
    unsigned short ch$w_rsvp_thresh;
    unsigned char ch$b_remote_ring_size;
    unsigned char ch$b_remote_device_type;
    unsigned short ch$w_lcl_chan_seq_num;
    unsigned short ch$w_rmt_chan_seq_num;
    unsigned short ch$w_remote_buffer_size;
    unsigned short ch$w_max_buffer_size;
    unsigned long ch$l_supported_services;
    unsigned long long ch$q_remote_net_address;
    unsigned long long ch$q_open_time;
    unsigned long long ch$q_close_time;
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
    unsigned char ch$b_minor;
    unsigned char ch$b_major;
    unsigned short ch$w_eco;
    unsigned char ch$ab_rmt_dev_name[16];
    unsigned char ch$ab_rmt_lan_hwa[6];
    unsigned char ch$b_rmt_node_hw_type;
    unsigned char ch$b_fill_byte;
    unsigned short ch$w_cc_hs_tmo;
    unsigned short ch$w_cc_listen_tmo;
    unsigned short ch$w_cc_rcv_bad_authorize;
    unsigned short ch$w_cc_rcv_bad_eco;
    unsigned short ch$w_cc_rcv_bad_mc;
    unsigned short ch$w_cc_rcv_short_msg;
    unsigned short ch$w_cc_rcv_incompat_channel;
    unsigned short ch$w_tr_rcv_old_msg;
    unsigned short ch$w_cc_no_mscp_server;
    unsigned short ch$w_cc_disk_not_served;
    unsigned short ch$w_topology_change;
};

#endif
