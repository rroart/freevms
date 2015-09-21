#ifndef SCSDEF_H
#define SCSDEF_H

#include <vms_types.h>

#define SCS$C_OVHD 14
#define SCS$C_CON_REQL 66
#define SCS$C_CON_RSPL 22
#define SCS$C_ACCP_REQL 66
#define SCS$C_ACCP_RSPL 18
#define SCS$C_REJ_REQL 18
#define SCS$C_REJ_RSPL 14
#define SCS$C_DISC_REQL 18
#define SCS$C_DISC_RSPL 14
#define SCS$C_CR_REQL 14
#define SCS$C_CR_RSPL 14
#define SCS$C_CON_REQ 0
#define SCS$C_CON_RSP 1
#define SCS$C_ACCP_REQ 2
#define SCS$C_ACCP_RSP 3
#define SCS$C_REJ_REQ 4
#define SCS$C_REJ_RSP 5
#define SCS$C_DISC_REQ 6
#define SCS$C_DISC_RSP 7
#define SCS$C_CR_REQ 8
#define SCS$C_CR_RSP 9
#define SCS$C_APPL_MSG 10
#define SCS$C_APPL_DG 11
#define SCS$K_APPL_BASE 0
#define SCS$C_APPL_BASE 0
#define SCS$M_UAP 0x1
#define SCS$K_STNORMAL 1
#define SCS$C_STNORMAL 1
#define SCS$K_STNOMAT 10
#define SCS$C_STNOMAT 10
#define SCS$K_STNORS 18
#define SCS$C_STNORS 18
#define SCS$K_STDISC 25
#define SCS$C_STDISC 25
#define SCS$K_STINSFCR 33
#define SCS$C_STINSFCR 33
#define SCS$K_STBALANCE 41
#define SCS$C_STBALANCE 41
#define SCS$K_USE_ALTERNATE_PORT 42
#define SCS$C_USE_ALTERNATE_PORT 42
#define SCS$K_CON_BASE 4
#define SCS$C_CON_BASE 4

#define SCS$S_SCSDEF 84

struct _scs
{
    union
    {
        UINT8 scs$b_ppd[16];
    };
    UINT16 scs$w_length;
    short fill;
    UINT16 scs$w_mtype;
    UINT16 scs$w_credit;
    unsigned long scs$l_dst_conid;
    unsigned long scs$l_src_conid;
    UINT16 scs$w_min_cr;
    UINT16 scs$w_status;
    char scs$t_dst_proc [16];
    UINT8 scs$b_subnode;
    UINT8 scs$b_pgrp;
    UINT16 scs$w_rsv;
    char scs$t_src_proc [16];
    UINT8 scs$b_con_dat [16];
};

#define SCS$S_SCSDEF1 28

struct _scs1
{
    unsigned long scs$l_lconid;
    unsigned long scs$l_rspid;
    unsigned long scs$l_xct_len;
    unsigned long scs$l_snd_name;
    UINT32 scs$l_snd_boff;
    UINT32 scs$l_rec_name;
    UINT32 scs$l_rec_boff;
} ;

#define PPD$C_CNFREC                     0x0023
#define PPD$C_CNFWMREC                   0x003D
#define PPD$C_DATREC                     0x0031
#define PPD$C_DGREC                      0x0021
#define PPD$C_EXTCNTLEN                  0x0040
#define PPD$C_FU_DG                      0x0007
#define PPD$C_IDREC                      0x002B
#define PPD$C_INVFS                      0x0008
#define PPD$C_INVTC                      0x0018
#define PPD$C_LBREC                      0x002D
#define PPD$C_LB_LENGTH                  0x0046
#define PPD$C_LCB_DATA                   0x0013
#define PPD$C_LENGTH                     0x0012
#define PPD$C_MCNFREC                    0x0029
#define PPD$C_MDATREC                    0x0033
#define PPD$C_MIN_DGSIZ                  0x0050
#define PPD$C_MSGREC                     0x0022
#define PPD$C_OPTCNTLEN                  0x0028
#define PPD$C_PSAUTO                     0
#define PPD$C_PSP0                       0x0001
#define PPD$C_PSP1                       0x0002
#define PPD$C_QUEOFFSET                  0x000C

#define PPD$C_RDCNT                      0x001A
#define PPD$C_REQDAT                     0x0008
#define PPD$C_REQDAT0                    0x0008
#define PPD$C_REQDAT1                    0x0009
#define PPD$C_REQDAT2                    0x000A
#define PPD$C_REQID                      0x0005
#define PPD$C_REQID_LEN                  0x0018
#define PPD$C_REQMDAT                    0x000E
#define PPD$C_RETCNF                     0x0003
#define PPD$C_RETCNFWM                   0x003E
#define PPD$C_RETDAT                     0x0011
#define PPD$C_SCSOFFSET_SNODE            0x0008
#define PPD$C_SETCKT                     0x0019
#define PPD$C_SETCKT_LEN                 0x001C
#define PPD$C_SNDDAT                     0x0010
#define PPD$C_SNDDATWM                   0x003C
#define PPD$C_SNDDG                      0x0001
#define PPD$C_SNDLB                      0x000D
#define PPD$C_SNDMDAT                    0x0012
#define PPD$C_SNDMSG                     0x0002
#define PPD$C_SNDRST                     0x0006
#define PPD$C_SNDSTRT                    0x0007

#define PPD$C_STABO                      0x0004
#define PPD$C_STINVDP                    0x0002
#define PPD$C_STINVSN                    0x0006
#define PPD$C_STPSV                      0
#define PPD$C_STURC                      0x0003
#define PPD$C_STURP                      0x0001
#define PPD$C_TYPACCV                    0x0004
#define PPD$C_TYPBLV                     0x0003
#define PPD$C_TYPBMSE                    0x0006
#define PPD$C_TYPINVBN                   0x0002
#define PPD$C_TYPNP                      0x0005
#define PPD$C_TYPOK                      0
#define PPD$C_TYPOTHER                   0x0007
#define PPD$C_TYPVCC                     0x0001
#define PPD$C_UNIMCMD                    0x0007
#define PPD$K_LB_LENGTH                  0x0046
#define PPD$K_LENGTH                     0x0012
#define PPD$K_QUEOFFSET                  0x000C
#define PPD$K_REQID_LEN                  0x0018
#define PPD$K_SETCKT_LEN                 0x001C
#define PPD$M_DISPOSE                    0x0001
#define PPD$M_DSTART                     0x0080
#define PPD$M_EXTCNT                     0x0080
#define PPD$M_FORCE                      0x0080
#define PPD$M_RSP                        0x0001
#define PPD$V_DISPOSE                    0
#define PPD$V_ERR                        0
#define PPD$V_EXTCNT                     0x0007
#define PPD$V_PS                         0x0001
#define PPD$V_RCD                        0x0005
#define PPD$V_RSP                        0
#define PPD$V_STSST                      0x0001
#define PPD$V_STSTYP                     0x0005
#define ppd$s_ststyp 3
#define ppd$s_stsst 4

struct _ppd
{
    union
    {
        unsigned long ppd$l_flink;
        unsigned long ppd$l_quefl;
    };
    union
    {
        unsigned long ppd$l_blink;
        unsigned long ppd$l_quebl;
    };
    union
    {
        UINT16 ppd$w_qsize;
        UINT16 ppd$w_size;
    };
    union
    {
        UINT8 ppd$b_qtype;
        UINT8 ppd$b_type;
    };
    union
    {
        UINT8 ppd$b_spare;
        UINT8 ppd$b_swflag;
    };
    union
    {
        struct
        {
            UINT8 ppd$b_port;
            UINT8 ppd$b_status;
        };
        UINT16 ppd$w_lcb_len7;
    };
    union
    {
        UINT8 ppd$b_lcb_port;
        UINT8 ppd$b_opc;
    };
    union
    {
        UINT8 ppd$b_flags;
        UINT8 ppd$b_lcb_nport;
    };
    // two next are tmp
    UINT16 ppd$w_length;
    UINT16 ppd$w_mtype;
#if 0
    10 UINT64 ppd$q_xct_id
    10 UINT8 ppd$b_d_snode
    10 UINT8 ppd$b_lcb_lport
    10 unsigned long ppd$l_p0_ack
    10 UINT16 ppd$w_d_xport
    10 UINT16 ppd$w_length
    10 UINT16 ppd$w_mask
    11 UINT8 ppd$b_d_pgrp
    11 UINT8 ppd$b_lcb_opc
    12 UINT8 ppd$b_lbdata
    12 UINT8 ppd$b_lcb_0
    12 UINT8 ppd$b_s_snode
    12 UINT16 ppd$w_mtype
    12 UINT16 ppd$w_s_xport
    13 UINT8 ppd$b_s_pgrp
    14 UINT8 ppd$b_systemid
    14 unsigned long ppd$l_p0_nak
    14 unsigned long ppd$l_rsvd_test
    14 UINT16 ppd$w_m_val
    18 unsigned long ppd$l_in_vcd
    18 unsigned long ppd$l_p0_nrsp
    18 unsigned long ppd$l_rport_typ
    18 unsigned long ppd$l_st_addr
    18 unsigned long ppd$l_xct_len
    1a UINT8 ppd$b_protocol
    1c UINT8 ppd$b_def_st
    1c unsigned long ppd$l_p1_ack
    1c unsigned long ppd$l_rport_rev
    1c unsigned long ppd$l_snd_name
    1c UINT16 ppd$w_maxdg
    1e UINT16 ppd$w_maxmsg
    20 unsigned long ppd$l_p1_nak
    20 unsigned long ppd$l_rport_fcn
    20 unsigned long ppd$l_snd_boff
    20 unsigned char ppd$t_swtype[4]
    24 UINT8 ppd$b_rst_port
    24 unsigned long ppd$l_p1_nrsp
    24 unsigned long ppd$l_rec_name
    24 unsigned char ppd$t_swvers
    25 UINT8 ppd$b_rstate
    28 unsigned long ppd$l_dg_disc
    28 unsigned long ppd$l_port_fcn_ext
    28 unsigned long ppd$l_rec_boff
    28 UINT64 ppd$q_swincarn
    2c UINT8 ppd$b_prtctr_len
    2c unsigned long ppd$l_port_fcn_ext2
    2d UINT8 ppd$b_impctr_len
    30 unsigned long ppd$l_spare1_cnt
    30 UINT64 ppd$q_sub_map
    30 unsigned char ppd$t_hwtype[4]
    34 UINT8 ppd$b_hwvers
    34 unsigned long ppd$l_spare2_cnt
    38 unsigned long ppd$l_spare3_cnt
    3c unsigned long ppd$l_spare4_cnt
    40 unsigned long ppd$l_snddat_oper_snt
    40 UINT64 ppd$q_nodename
    42 unsigned long ppd$l_lbcrc
    44 unsigned long ppd$l_snddat_data_snt
    48 unsigned long ppd$l_snddat_bodies_snt
    48 UINT64 ppd$q_curtime
    4c unsigned long ppd$l_reqdat_oper_snt
    50 unsigned long ppd$l_retdat_data_rcv
    54 unsigned long ppd$l_retdat_bodies_rcv
    58 unsigned long ppd$l_sntdat_bodies_rec
    58 unsigned long ppd$l_sntdat_bodies_rec
    5c unsigned long ppd$l_sntdat_data_rec
    5c unsigned long ppd$l_sntdat_data_rec
    60 unsigned long ppd$l_cnf_snt
    64 unsigned long ppd$l_datreq_bodies_rcv
    68 unsigned long ppd$l_retdat_bodies_snt
    6c unsigned long ppd$l_retdat_data_snt
    70 unsigned long ppd$l_dgsnt
    70 unsigned long ppd$l_pckt_xmit
    74 unsigned long ppd$l_dg_txt_snt
    74 unsigned long ppd$l_pckt_rcv
    78 unsigned long ppd$l_msg_snt
    78 unsigned long ppd$l_pckt_rcrc
    7c unsigned long ppd$l_msg_txt_snt
    7c UINT16 ppd$w_rspr_wpe
    7e UINT16 ppd$w_mbpb_wpe
    7e UINT16 ppd$w_mbpb_wpe
    80 unsigned long ppd$l_misc_snt
    80 UINT16 ppd$w_cmdr_wpe
    82 UINT16 ppd$w_intr_wpe
    84 unsigned long ppd$l_dg_rec
    84 UINT16 ppd$w_mar_wpe
    86 UINT16 ppd$w_mbr_wpe
    88 unsigned long ppd$l_dg_txtrec
    88 UINT16 ppd$w_mpb_rpe
    8a UINT16 ppd$w_tbuf_pe
    8c unsigned long ppd$l_msg_rec
    8c UINT16 ppd$w_mib_pe
    8e UINT16 ppd$w_mcwi_pe
    90 unsigned long ppd$l_msg_txt_rec
    90 UINT16 ppd$w_yreg_pe
    92 UINT16 ppd$w_xreg_pe
    94 unsigned long ppd$l_misc_rec
#endif
};

#endif
