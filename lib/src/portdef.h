#ifndef portdef_h
#define portdef_h

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

struct _port {

  void * port$a_portqb;
  void * port$a_intr_srv;
  /* missing word? */
  unsigned char port$b_type;
  unsigned char port$b_sub_type;
  unsigned char port$b_vc_num;
  unsigned char port$b_vc_cnt;
  unsigned char port$b_vc_last;
  unsigned char port$b_max_cache;
  unsigned long long port$q_fork;
  unsigned short port$w_sts;
  unsigned char port$b_max_rexmt;
  unsigned char port$b_flck;
  void * port$a_fpc;
  /* missing quad?  */
  union {
    unsigned long long port$q_vc_work;
    unsigned long long port$q_que_first;
  }
  unsigned long long port$q_sbuf_free;
  unsigned long long port$q_lbuf_free;
  unsigned long long port$q_dfq;
  unsigned long long port$q_mfq;
  unsigned long long port$q_xmt_ldl;
  unsigned long long port$q_rwaitq;
  unsigned long long port$q_rspq;
  unsigned long long port$q_bus_list;
  unsigned long port$l_secs_zeroed;
  unsigned short port$w_clock;
  unsigned short port$w_wrk;
  unsigned long long port$q_authorize;
  unsigned long port$l_services;
  unsigned short port$w_max_lngmsg;
  unsigned short port$w_max_lngdg;
  void * port$a_vcvec0;
  unsigned short port$w_delay_ack;
  unsigned short port$w_delay_seq;
  unsigned char port$t_nodename[8] /* 6 + word? */;
  unsigned short port$w_sbuf_size;
  unsigned short port$w_lbuf_size;
  unsigned short port$w_sbuf_cnt;
  unsigned short port$w_sbuf_quo;
  unsigned short port$w_sbuf_max;
  unsigned short port$w_lbuf_cnt;
  unsigned short port$w_lbuf_quo;
  unsigned short port$w_lbuf_max;
  unsigned long port$l_tot_lbuf_allocs;
  unsigned long port$l_tot_sbuf_allocs;
  unsigned short port$w_lbuf_lookaside_miss;
  unsigned short port$w_sbuf_lookaside_miss;
  unsigned short port$w_sbuf_inuse_cnt;
  unsigned short port$w_sbuf_inuse_peak;
  unsigned short port$w_sysid_hi;
  unsigned short port$w_lbuf_inuse_cnt;
  unsigned short port$w_lbuf_inuse_peak;
  unsigned short port$w_pci_sbuf_empty;
  unsigned short port$w_pci_lbuf_empty;
  unsigned short port$w_tr_sbuf_empty;
  unsigned short port$w_bus_count;
  void * port$a_link;
  unsigned char port$b_scan_xack;
  unsigned char port$b_scan_xseq;
  unsigned char port$b_scan_sbuf;
  unsigned char port$b_need_sack;
  unsigned long port$l_sysid_lo;
  /* missing long? */
  unsigned short port$w_group;
  unsigned long port$l_mcast_lo;
  unsigned short port$w_mcast_hi;
  unsigned char port$b_tim_err_log;
  unsigned long port$l_maint;
  unsigned char port$b_minor;
  unsigned char port$b_major;
  unsigned short port$w_eco;
  unsigned long port$l_port_fcn;
  unsigned long port$l_sta_info;
  unsigned char port$t_rstport[8]  /*?*/;
  unsigned char port$t_mask_sbuf[32] /*?*/;
  unsigned char port$t_mask_xack[32] /*?*/;
  unsigned char port$t_mask_xseq[32] /*?*/;

}


#endif
