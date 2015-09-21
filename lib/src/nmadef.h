#ifndef NMADEF_H
#define NMADEF_H

#include <vms_types.h>

#define     NMA$C_OBJ_NIC       19

#define     NMA$C_FNC_LOA       15
#define     NMA$C_FNC_DUM       16
#define     NMA$C_FNC_TRI       17
#define     NMA$C_FNC_TES       18
#define     NMA$C_FNC_CHA       19
#define     NMA$C_FNC_REA       20
#define     NMA$C_FNC_ZER       21
#define     NMA$C_FNC_SYS       22

#define     NMA$M_OPT_ENT       0x7
#define     NMA$M_OPT_CLE       0x40
#define     NMA$M_OPT_PER       0x80
#define     NMA$M_OPT_INF       0x70
#define     NMA$C_OPINF_SUM     0
#define     NMA$C_OPINF_STA     1
#define     NMA$C_OPINF_CHA     2
#define     NMA$C_OPINF_COU     3
#define     NMA$C_OPINF_EVE     4

#define     NMA$M_OPT_ACC       0x80
#define     NMA$M_OPT_REA       0x80
#define     NMA$C_SYS_RST       1
#define     NMA$C_SYS_RSX       2
#define     NMA$C_SYS_TOP       3
#define     NMA$C_SYS_VMS       4
#define     NMA$C_SYS_RT        5

#define     NMA$C_ENT_NOD       0
#define     NMA$C_ENT_LIN       1
#define     NMA$C_ENT_LOG       2
#define     NMA$C_ENT_CIR       3
#define     NMA$C_ENT_MOD       4
#define     NMA$C_ENT_ARE       5

#define     NMA$C_SENT_PROXY    2
#define     NMA$C_SENT_ALI      3
#define     NMA$C_SENT_OBJ      4
#define     NMA$C_SENT_PRO      5
#define     NMA$C_SENT_SYS      6
#define     NMA$C_SENT_LNK      7
#define     NMA$C_SENT_WLD      -30
#define     NMA$M_ENT_EXE       0x80
#define     NMA$C_ENT_WAR       -7
#define     NMA$C_ENT_WAD       -6
#define     NMA$C_ENT_ADJ       -4
#define     NMA$C_ENT_ACT       -2
#define     NMA$C_ENT_KNO       -1
#define     NMA$C_ENT_ADD       0
#define     NMA$C_ENT_ALL       -3
#define     NMA$C_ENT_LOO       -3

#define     NMA$C_SNK_CON       1
#define     NMA$C_SNK_FIL       2
#define     NMA$C_SNK_MON       3

#define     NMA$M_CNT_TYP       0xFFF
#define     NMA$M_CNT_MAP       0x1000
#define     NMA$M_CNT_WID       0x6000
#define     NMA$M_CNT_COU       0x8000
#define     NMA$M_CNT_WIL       0x2000
#define     NMA$M_CNT_WIH       0x4000
#define     NMA$S_NMADEF        2

struct _nma
{
    union
    {
        struct
        {
            unsigned nma$v_opt_ent      : 3;
            unsigned nma$$_fill_1       : 3;
            unsigned nma$v_opt_cle      : 1;
            unsigned nma$v_opt_per      : 1;
        };
        struct
        {
            unsigned nma$$_fill_2       : 4;
            unsigned nma$v_opt_inf      : 3;
            unsigned nma$v_fill_0_      : 1;
        };
        struct
        {
            unsigned nma$$_fill_3       : 7;
            unsigned nma$v_opt_acc      : 1;
        };
        struct
        {
            unsigned nma$$_fill_4       : 7;
            unsigned nma$v_opt_rea      : 1;
        };
        struct
        {
            unsigned nma$$_fill_5       : 7;
            unsigned nma$v_ent_exe      : 1;
        };
        struct
        {
            unsigned nma$v_cnt_typ      : 12;
            unsigned nma$v_cnt_map      : 1;
            unsigned nma$v_cnt_wid      : 2;
            unsigned nma$v_cnt_cou      : 1;
        };
        struct
        {
            unsigned nma$$_fill_6       : 13;
            unsigned nma$v_cnt_wil      : 1;
            unsigned nma$v_cnt_wih      : 1;
            unsigned nma$v_fill_1_      : 1;
        };
    };
};

#define     NMA$M_PTY_TYP       0x7FFF
#define     NMA$C_PTY_MAX       15
#define     NMA$M_PTY_CLE       0x3F
#define     NMA$M_PTY_MUL       0x40
#define     NMA$M_PTY_COD       0x80
#define     NMA$M_PTY_CMU       0xC0
#define     NMA$M_PTY_NLE       0xF
#define     NMA$M_PTY_NTY       0x30
#define     NMA$M_PTY_ASC       0x40
#define     NMA$C_NTY_DU        0
#define     NMA$C_NTY_DS        1
#define     NMA$C_NTY_H     2
#define     NMA$C_NTY_O     3

#define     NMA$C_NLE_IMAGE     0
#define     NMA$C_NLE_BYTE      1
#define     NMA$C_NLE_WORD      2
#define     NMA$C_NLE_LONG      4
#define     NMA$C_NLE_QUAD      8

#define     NMA$C_PTY_AI        64
#define     NMA$C_PTY_HI        32
#define     NMA$C_PTY_H1        33
#define     NMA$C_PTY_H2        34
#define     NMA$C_PTY_H4        36
#define     NMA$C_PTY_DU1       1
#define     NMA$C_PTY_DU2       2
#define     NMA$C_PTY_CD1       129
#define     NMA$C_PTY_CM2       194
#define     NMA$C_PTY_CM3       195
#define     NMA$C_PTY_CM4       196
#define     NMA$C_PTY_CM5       197

#define     NMA$C_CTLVL_UI      3
#define     NMA$C_CTLVL_XID     175
#define     NMA$C_CTLVL_XID_P   191
#define     NMA$C_CTLVL_TEST    227
#define     NMA$C_CTLVL_TEST_P  243

#define     NMA$C_PCCI_STA      0
#define     NMA$C_PCCI_SUB      1
#define     NMA$C_PCCI_SER      100
#define     NMA$C_PCCI_LCT      110
#define     NMA$C_PCCI_SPY      120
#define     NMA$C_PCCI_SSB      121
#define     NMA$C_PCCI_CNO      200
#define     NMA$C_PCCI_COB      201
#define     NMA$C_PCCI_LOO      400
#define     NMA$C_PCCI_ADJ      800
#define     NMA$C_PCCI_DRT      801
#define     NMA$C_PCCI_BLO      810
#define     NMA$C_PCCI_COS      900
#define     NMA$C_PCCI_MRT      901
#define     NMA$C_PCCI_RPR      902
#define     NMA$C_PCCI_HET      906
#define     NMA$C_PCCI_LIT      907
#define     NMA$C_PCCI_BLK      910
#define     NMA$C_PCCI_MRC      920
#define     NMA$C_PCCI_RCT      921
#define     NMA$C_PCCI_NUM      930
#define     NMA$C_PCCI_USR      1000
#define     NMA$C_PCCI_POL      1010
#define     NMA$C_PCCI_PLS      1011
#define     NMA$C_PCCI_OWN      1100
#define     NMA$C_PCCI_LIN      1110
#define     NMA$C_PCCI_USE      1111
#define     NMA$C_PCCI_TYP      1112
#define     NMA$C_PCCI_NET      1119
#define     NMA$C_PCCI_DTE      1120
#define     NMA$C_PCCI_CHN      1121
#define     NMA$C_PCCI_MBL      1122
#define     NMA$C_PCCI_MWI      1123
#define     NMA$C_PCCI_TRI      1140
#define     NMA$C_PCCI_BBT      1141
#define     NMA$C_PCCI_TRT      1142
#define     NMA$C_PCCI_RTT      1143
#define     NMA$C_PCCI_MRB      1145

#define     NMA$C_PCCI_MTR      1146
#define     NMA$C_PCCI_ACB      1150
#define     NMA$C_PCCI_ACI      1151
#define     NMA$C_PCCI_IAB      1152
#define     NMA$C_PCCI_IAI      1153
#define     NMA$C_PCCI_IAT      1154
#define     NMA$C_PCCI_DYB      1155
#define     NMA$C_PCCI_DYI      1156
#define     NMA$C_PCCI_DYT      1157
#define     NMA$C_PCCI_DTH      1158

#define     NMA$C_PCCI_RSX_MAC  2320
#define     NMA$C_PCCI_RSX_LOG  2380
#define     NMA$C_PCCI_RSX_DLG  2385
#define     NMA$C_PCCI_RSX_ACT  2390

#define     NMA$C_PCCI_VER      2700
#define     NMA$C_PCCI_XPT      2720

#define     NMA$C_PCCI_IRC      2750
#define     NMA$C_PCCI_ORC      2751
#define     NMA$C_PCCI_GRP      2752
#define     NMA$C_PCCI_NOP      2753
#define     NMA$C_PCCI_CAL      2754
#define     NMA$C_PCCI_INA      2755
#define     NMA$C_PCCI_RED      2756
#define     NMA$C_PCCI_MOD      2757
#define     NMA$C_PCCI_REQ      2758
#define     NMA$C_PCCI_DTW      2759
#define     NMA$C_PCCI_PRO      2760
#define     NMA$C_PCCI_INF      2761
#define     NMA$C_PCCI_ACC      2762
#define     NMA$C_PCCI_CLR      2763
#define     NMA$C_PCCI_DTC      2764
#define     NMA$C_PCCI_CCG      2765
#define     NMA$C_PCCI_ESA      2766
#define     NMA$C_PCCI_DTI      2767
#define     NMA$C_PCCI_SWC      2768
#define     NMA$C_PCCI_TIC      2769
#define     NMA$C_PCCI_CSG      2770
#define     NMA$C_PCCI_AAS      2771
#define     NMA$C_PCCI_DTS      2772
#define     NMA$C_PCCI_CAS      2773
#define     NMA$C_PCCI_CPS      2774
#define     NMA$C_PCCI_CNT      2775
#define     NMA$C_PCCI_RAT      2776
#define     NMA$C_PCCI_PRD      2777
#define     NMA$C_PCCI_DAY      2778
#define     NMA$C_PCCI_BFN      2779
#define     NMA$C_PCCI_BSZ      2780
#define     NMA$C_PCCI_MDM      2781
#define     NMA$C_PCCI_DTL      2782
#define     NMA$C_PCCI_IDL      2783
#define     NMA$C_PCCI_IMT      2784
#define     NMA$C_PCCI_CAC      2785
#define     NMA$C_PCCI_ORD      2786
#define     NMA$C_PCCI_CID      2787

#define     NMA$C_PCCI_MST      2810

#define     NMA$C_PCCI_SRV_LOG  3380
#define     NMA$C_PCCI_SRV_DLG  3385
#define     NMA$C_PCCI_SRV_ACT  3390

#define     NMA$C_PCLI_STA      0
#define     NMA$C_PCLI_SUB      1
#define     NMA$C_PCLI_SER      100
#define     NMA$C_PCLI_LCT      110
#define     NMA$C_PCLI_LOO      400
#define     NMA$C_PCLI_ADJ      800
#define     NMA$C_PCLI_BLO      810
#define     NMA$C_PCLI_COS      900
#define     NMA$C_PCLI_DEV      1100
#define     NMA$C_PCLI_BFN      1105
#define     NMA$C_PCLI_CON      1110
#define     NMA$C_PCLI_DUP      1111
#define     NMA$C_PCLI_PRO      1112
#define     NMA$C_PCLI_LTY      1112
#define     NMA$C_PCLI_CLO      1113
#define     NMA$C_PCLI_STI      1120
#define     NMA$C_PCLI_NTI      1121
#define     NMA$C_PCLI_RTT      1121
#define     NMA$C_PCLI_HTI      1122
#define     NMA$C_PCLI_MBL      1130
#define     NMA$C_PCLI_MRT      1131
#define     NMA$C_PCLI_MWI      1132
#define     NMA$C_PCLI_TRI      1140
#define     NMA$C_PCLI_SLT      1150
#define     NMA$C_PCLI_DDT      1151
#define     NMA$C_PCLI_DLT      1152
#define     NMA$C_PCLI_SRT      1153
#define     NMA$C_PCLI_HWA      1160

#define     NMA$C_PCLI_TREQ         1161
#define     NMA$C_PCLI_TVX          1162
#define     NMA$C_PCLI_REST_TTO     1163
#define     NMA$C_PCLI_RPE          1164
#define     NMA$C_PCLI_NIF_TARG     1165
#define     NMA$C_PCLI_SIF_CONF_TARG    1166
#define     NMA$C_PCLI_SIF_OP_TARG      1167
#define     NMA$C_PCLI_ECHO_TARG        1168
#define     NMA$C_PCLI_ECHO_DAT     1169

#define     NMA$C_PCLI_STN_ADR      1170
#define     NMA$C_PCLI_FNC_ADR      1171
#define     NMA$C_PCLI_GRP_ADR      1172
#define     NMA$C_PCLI_UNA          1173
#define     NMA$C_PCLI_RNG_NUM      1174
#define     NMA$C_PCLI_AUTH_PR      1175
#define     NMA$C_PCLI_RNG_SPD      1176
#define     NMA$C_PCLI_ETR          1177
#define     NMA$C_PCLI_SRC_ROU      1178
#define     NMA$C_PCLI_ADR_TYP      1179
#define     NMA$C_PCLI_A_TIM        1400

#define     NMA$C_PCLI_ECHO_LEN     1180
#define     NMA$C_PCLI_LAST_NIF     1181
#define     NMA$C_PCLI_LAST_SIF     1182
#define     NMA$C_PCLI_LAST_ECHO        1183
#define     NMA$C_PCLI_T_NEG        1184
#define     NMA$C_PCLI_DAT          1185
#define     NMA$C_PCLI_OLD_UNA      1187
#define     NMA$C_PCLI_UN_DAT       1188
#define     NMA$C_PCLI_DNA          1189
#define     NMA$C_PCLI_OLD_DNA      1192
#define     NMA$C_PCLI_RPS          1193
#define     NMA$C_PCLI_RER          1194
#define     NMA$C_PCLI_FDE          1198
#define     NMA$C_PCLI_NBR_PHY      1300
#define     NMA$C_PCLI_LEE          1301
#define     NMA$C_PCLI_RJR          1302
#define     NMA$C_PCLI_NET          1190
#define     NMA$C_PCLI_XMD          1191

#define     NMA$C_PCLI_RSX_OWN      2300
#define     NMA$C_PCLI_RSX_CCS      2310
#define     NMA$C_PCLI_RSX_UCS      2311
#define     NMA$C_PCLI_RSX_VEC      2312
#define     NMA$C_PCLI_RSX_PRI      2313
#define     NMA$C_PCLI_RSX_MDE      2321
#define     NMA$C_PCLI_RSX_LLO      2330

#define     NMA$C_PCLI_RSX_LOG      2380
#define     NMA$C_PCLI_RSX_DLG      2385
#define     NMA$C_PCLI_RSX_ACT      2390

#define     NMA$C_PCLI_MCD      2701
#define     NMA$C_PCLI_EPT      2720
#define     NMA$C_PCLI_LNS      2730
#define     NMA$C_PCLI_SWI      2740
#define     NMA$C_PCLI_HNG      2750
#define     NMA$C_PCLI_TPI      2760
#define     NMA$C_PCLI_NRZI     2761
#define     NMA$C_PCLI_CODE     2762

#define     NMA$C_PCLI_FMT      2770
#define     NMA$C_PCLI_SRV      2771
#define     NMA$C_PCLI_SAP      2772
#define     NMA$C_PCLI_GSP      2773
#define     NMA$C_PCLI_PID      2774
#define     NMA$C_PCLI_CNM      2775
#define     NMA$C_PCLI_CCA      2776
#define     NMA$C_PCLI_APC      2777
#define     NMA$C_PCLI_MED      2778
#define     NMA$C_PCLI_PNM      2779
#define     NMA$C_PCLI_SNM      2780

#define     NMA$C_PCLI_MONCONTEND       2781
#define     NMA$C_PCLI_CACHE_ENT        2782
#define     NMA$C_PCLI_ROUTEDIS     2783

#define     NMA$C_PCLI_LINEMEDIA        2784
#define     NMA$C_PCLI_LINESPEED        2785

#define     NMA$C_PCLI_LES_HWA      2786
#define     NMA$C_PCLI_PVC_REQ      2787
#define     NMA$C_PCLI_PVC          2788
#define     NMA$C_PCLI_ELAN_STATE_REQ   2789
#define     NMA$C_PCLI_ELAN_STATE_RSP   2790
#define     NMA$C_PCLI_EVENT_REQ        2791
#define     NMA$C_PCLI_EVENT        2792
#define     NMA$C_PCLI_EXT_SENSE_REQ    2793
#define     NMA$C_PCLI_EXT_SENSE        2794
#define     NMA$C_PCLI_ELAN_BUS     2795
#define     NMA$C_PCLI_ELAN_PAR     2796
#define     NMA$C_PCLI_ELAN_DESC        2797
#define     NMA$C_PCLI_NUM_PVC      2798
#define     NMA$C_PCLI_MAXPVC       64

#define     NMA$C_PCLI_CLIP_LIS_HWA     2702
#define     NMA$C_PCLI_CLIP_STATE_REQ   2703
#define     NMA$C_PCLI_CLIP_STATE_RSP   2704
#define     NMA$C_PCLI_CLIP_IP_ADDR     2705
#define     NMA$C_PCLI_CLIP_IP_SUBNET   2706
#define     NMA$C_PCLI_CLIP_NAME        2707
#define     NMA$C_PCLI_CLIP_USER_TYPE   2708
#define     NMA$C_PCLI_CLIP_ATM_ADDR    2709
#define     NMA$C_PCLI_CLIP_PAR     2711
#define     NMA$C_PCLI_CLIP_PVC_IP_ADDR 2712
#define     NMA$C_PCLI_CLIP_PVC_STATUS  2713
#define     NMA$C_PCLI_CLIP_GET_PVC     2714

#define     NMA$C_PCLI_BUS      2801
#define     NMA$C_PCLI_NMS      2810
#define     NMA$C_PCLI_PHA      2820
#define     NMA$C_PCLI_DPA      2821
#define     NMA$C_PCLI_PTY      2830
#define     NMA$C_PCLI_MCA      2831

#define     NMA$C_PCLI_ILP      2839

#define     NMA$C_PCLI_PRM      2840
#define     NMA$C_PCLI_MLT      2841
#define     NMA$C_PCLI_PAD      2842
#define     NMA$C_PCLI_DCH      2843
#define     NMA$C_PCLI_CRC      2844
#define     NMA$C_PCLI_HBQ      2845
#define     NMA$C_PCLI_ACC      2846
#define     NMA$C_PCLI_EKO      2847
#define     NMA$C_PCLI_BSZ      2848
#define     NMA$C_PCLI_DES      2849
#define     NMA$C_PCLI_RET      2850
#define     NMA$C_PCLI_MOD      2851
#define     NMA$C_PCLI_RIB      2852
#define     NMA$C_PCLI_MNTL     2860

#define     NMA$C_PCLI_INTL0    2861
#define     NMA$C_PCLI_INTL1    2862
#define     NMA$C_PCLI_INTL2    2863
#define     NMA$C_PCLI_INTL3    2864
#define     NMA$C_PCLI_FRA      2865
#define     NMA$C_PCLI_STI1     2866
#define     NMA$C_PCLI_STI2     2867
#define     NMA$C_PCLI_TMO      2868
#define     NMA$C_PCLI_MCL      2869
#define     NMA$C_PCLI_SYC      2870
#define     NMA$C_PCLI_BPC      2871
#define     NMA$C_PCLI_MBS      2872
#define     NMA$C_PCLI_RES      2873
#define     NMA$C_PCLI_XFC      2874
#define     NMA$C_PCLI_RFC      2875

#define     NMA$C_PCLI_MAP      2876
#define     NMA$C_MAP_ADD       0
#define     NMA$C_MAP_CHANGE    1
#define     NMA$C_MAP_DELETE    2
#define     NMA$C_PCLI_MRB      2877
#define     NMA$C_PCLI_MINRCV   2878
#define     NMA$C_PCLI_MAXRCV   2879

#define     NMA$C_PCLI_ROUTE    2880
#define     NMA$C_SR_ADD        0
#define     NMA$C_SR_DEL        1
#define     NMA$C_PCLI_SRC      2881

#define     NMA$C_PCLI_FCA      2883
#define     NMA$C_PCLI_XAC      2884
#define     NMA$C_PCLI_RAC      2885
#define     NMA$C_PCLI_FAMODE   2886
#define     NMA$C_PCLI_SRMODE   2887
#define     NMA$C_SR_TRANSPARENT    0
#define     NMA$C_SR_SELF       1

#define     NMA$C_PCLI_SRV_OWN  3300
#define     NMA$C_PCLI_SRV_UCS  3311
#define     NMA$C_PCLI_SRV_VEC  3312
#define     NMA$C_PCLI_SRV_PRI  3313
#define     NMA$C_PCLI_SRV_LOG  3380
#define     NMA$C_PCLI_SRV_DLG  3385
#define     NMA$C_PCLI_SRV_ACT  3390

#define     NMA$C_LINMD_CSMACD  10
#define     NMA$C_LINMD_FDDI    11
#define     NMA$C_LINMD_CI      12
#define     NMA$C_LINMD_TR      13
#define     NMA$C_LINMD_ATM     14

#define     NMA$C_PCCO_RTR      110

#define     NMA$C_PCLD_ASS      10

#define     NMA$C_PCLP_ASS      10

#define     NMA$C_PCCN_CIR      100
#define     NMA$C_PCCN_SUR      110
#define     NMA$C_PCCN_ELT      111
#define     NMA$C_PCCN_PHA      120
#define     NMA$C_PCCN_LRP      130
#define     NMA$C_PCCN_MVR      20001
#define     NMA$C_PCCN_FCT      20002
#define     NMA$C_PCCN_CUS      20003
#define     NMA$C_PCCN_RTR      20004
#define     NMA$C_PCCN_CSZ      20005
#define     NMA$C_PCCN_RSZ      20006
#define     NMA$C_PCCN_HWA      20007
#define     NMA$C_PCCN_DTY      20100
#define     NMA$C_PCCN_SFI      20200
#define     NMA$C_PCCN_SPR      20300
#define     NMA$C_PCCN_DLK      20400

#define     NMA$C_PCLO_STA      0
#define     NMA$C_PCLO_LNA      100
#define     NMA$C_PCLO_SIN      200
#define     NMA$C_PCLO_EVE      201

#define     NMA$C_PCXA_NOD      320
#define     NMA$C_PCXA_USR      330
#define     NMA$C_PCXA_SPW      331
#define     NMA$C_PCXA_RPW      331
#define     NMA$C_PCXA_ACC      332
#define     NMA$C_PCXA_NET      1110

#define     NMA$C_PCXA_RSX_ADS  2310
#define     NMA$C_PCXA_RSX_ANB  2320
#define     NMA$C_PCXA_RSX_ASC  2330

#define     NMA$C_PCXA_SRV_ADS  3310
#define     NMA$C_PCXA_SRV_ANB  3320
#define     NMA$C_PCXA_SRV_ASC  3330

#define     NMA$C_PCXP_STA      0
#define     NMA$C_PCXP_SBS      1
#define     NMA$C_PCXP_CTM      100
#define     NMA$C_PCXP_ACH      1000
#define     NMA$C_PCXP_ASW      1010
#define     NMA$C_PCXP_DTE      1100
#define     NMA$C_PCXP_GRP      1101
#define     NMA$C_PCXP_NETENT   1110
#define     NMA$C_PCXP_DNT      1111
#define     NMA$C_PCXP_LIN      1120
#define     NMA$C_PCXP_CHN      1130
#define     NMA$C_PCXP_MCH      1131
#define     NMA$C_PCXP_DBL      1140
#define     NMA$C_PCXP_DWI      1141
#define     NMA$C_PCXP_MBL      1150
#define     NMA$C_PCXP_MWI      1151
#define     NMA$C_PCXP_MCL      1152
#define     NMA$C_PCXP_MRS      1153
#define     NMA$C_PCXP_MST      1154
#define     NMA$C_PCXP_CAT      1160
#define     NMA$C_PCXP_CLT      1161
#define     NMA$C_PCXP_RST      1162
#define     NMA$C_PCXP_STT      1163
#define     NMA$C_PCXP_ITT      1164
#define     NMA$C_PCXP_GDT      1170
#define     NMA$C_PCXP_GNM      1171
#define     NMA$C_PCXP_GTY      1172
#define     NMA$C_PCXP_GNT      1173
#define     NMA$C_PCXP_MODE     1180
#define     NMA$C_PCXP_PROF     1190

#define     NMA$C_PCXP_RSX_PMC  2300

#define     NMA$C_PCXP_MCI      2710

#define     NMA$C_PCXP_SRV_PMC  3300

#define     NMA$C_PCXS_STA      1
#define     NMA$C_PCXS_CTM      100
#define     NMA$C_PCXS_ACI      200
#define     NMA$C_PCXS_DST      300
#define     NMA$C_PCXS_MCI      310
#define     NMA$C_PCXS_NOD      320
#define     NMA$C_PCXS_USR      330
#define     NMA$C_PCXS_SPW      331
#define     NMA$C_PCXS_RPW      331
#define     NMA$C_PCXS_ACC      332
#define     NMA$C_PCXS_OBJ      340
#define     NMA$C_PCXS_PRI      350
#define     NMA$C_PCXS_CMK      351
#define     NMA$C_PCXS_CVL      352
#define     NMA$C_PCXS_GRP      353
#define     NMA$C_PCXS_SDTE     354
#define     NMA$C_PCXS_SAD      355
#define     NMA$C_PCXS_RED      390
#define     NMA$C_PCXS_CDTE     391
#define     NMA$C_PCXS_RDTE     392
#define     NMA$C_PCXS_NET      393
#define     NMA$C_PCXS_EMK      394
#define     NMA$C_PCXS_EVL      395
#define     NMA$C_PCXS_IDTE     396

#define     NMA$C_PCXS_RSX_5ST  2310

#define     NMA$C_PCXS_FIL      2710

#define     NMA$C_PCXS_SRV_5ST  3310

#define     NMA$C_PCXT_STA      0
#define     NMA$C_PCXT_BSZ      100
#define     NMA$C_PCXT_MBK      101
#define     NMA$C_PCXT_FNM      102
#define     NMA$C_PCXT_MBF      103
#define     NMA$C_PCXT_CPL      104
#define     NMA$C_PCXT_MVR      105
#define     NMA$C_PCXT_TPT      106
#define     NMA$C_PCXT_CPS      110
#define     NMA$C_PCXT_TST      111

#define     NMA$C_PCNO_STA      0
#define     NMA$C_PCNO_PHA      10
#define     NMA$C_PCNO_IDE      100
#define     NMA$C_PCNO_MVE      101
#define     NMA$C_PCNO_SLI      110
#define     NMA$C_PCNO_SPA      111
#define     NMA$C_PCNO_SDV      112
#define     NMA$C_PCNO_CPU      113
#define     NMA$C_PCNO_HWA      114
#define     NMA$C_PCNO_SNV      115
#define     NMA$C_PCNO_LOA      120
#define     NMA$C_PCNO_SLO      121
#define     NMA$C_PCNO_TLO      122
#define     NMA$C_PCNO_DFL      123
#define     NMA$C_PCNO_STY      125
#define     NMA$C_PCNO_SID      126
#define     NMA$C_PCNO_MFL      127
#define     NMA$C_PCNO_DUM      130
#define     NMA$C_PCNO_SDU      131
#define     NMA$C_PCNO_DAD      135
#define     NMA$C_PCNO_DCT      136
#define     NMA$C_PCNO_OHO      140
#define     NMA$C_PCNO_IHO      141
#define     NMA$C_PCNO_LPC      150
#define     NMA$C_PCNO_LPL      151
#define     NMA$C_PCNO_LPD      152
#define     NMA$C_PCNO_LPA      153
#define     NMA$C_PCNO_LPH      154
#define     NMA$C_PCNO_LPN      155
#define     NMA$C_PCNO_LAN      156
#define     NMA$C_PCNO_CTI      160
#define     NMA$C_PCNO_NNA      500
#define     NMA$C_PCNO_NLI      501
#define     NMA$C_PCNO_ADD      502
#define     NMA$C_PCNO_ITI      510
#define     NMA$C_PCNO_OTI      511
#define     NMA$C_PCNO_IPR      522
#define     NMA$C_PCNO_OPR      523
#define     NMA$C_PCNO_ACL      600
#define     NMA$C_PCNO_DEL      601
#define     NMA$C_PCNO_NVE      700
#define     NMA$C_PCNO_MLK      710
#define     NMA$C_PCNO_DFA      720
#define     NMA$C_PCNO_DWE      721
#define     NMA$C_PCNO_IAT      722
#define     NMA$C_PCNO_RFA      723
#define     NMA$C_PCNO_DTY      810
#define     NMA$C_PCNO_DCO      820
#define     NMA$C_PCNO_DHO      821
#define     NMA$C_PCNO_DLI      822
#define     NMA$C_PCNO_NND      830
#define     NMA$C_PCNO_RVE      900
#define     NMA$C_PCNO_ETY      901
#define     NMA$C_PCNO_RTI      910
#define     NMA$C_PCNO_SAD      911
#define     NMA$C_PCNO_BRT      912
#define     NMA$C_PCNO_MAD      920
#define     NMA$C_PCNO_MLN      921
#define     NMA$C_PCNO_MCO      922
#define     NMA$C_PCNO_MHO      923
#define     NMA$C_PCNO_MVI      924
#define     NMA$C_PCNO_MAR      925
#define     NMA$C_PCNO_MBE      926
#define     NMA$C_PCNO_MBR      927
#define     NMA$C_PCNO_AMC      928
#define     NMA$C_PCNO_AMH      929
#define     NMA$C_PCNO_MBU      930
#define     NMA$C_PCNO_BUS      931
#define     NMA$C_PCNO_SBS      932
#define     NMA$C_PCNO_MPS      933
#define     NMA$C_PCNO_FBS      933

#define     NMA$C_PCNO_RSX_RPA  2300

#define     NMA$C_PCNO_RSX_TPA  2301

#define     NMA$C_PCNO_RSX_VER  2310

#define     NMA$C_PCNO_PUS      2704
#define     NMA$C_PCNO_PAC      2705
#define     NMA$C_PCNO_PPW      2706
#define     NMA$C_PCNO_NUS      2712
#define     NMA$C_PCNO_NAC      2713
#define     NMA$C_PCNO_NPW      2714
#define     NMA$C_PCNO_RPA      2720
#define     NMA$C_PCNO_TPA      2721
#define     NMA$C_PCNO_ACC      2730
#define     NMA$C_PCNO_DAC      2731
#define     NMA$C_PCNO_PIQ      2740
#define     NMA$C_PCNO_ALI      2742
#define     NMA$C_PCNO_ALM      2743
#define     NMA$C_PCNO_ALN      2744
#define     NMA$C_PCNO_PRX      2750
#define     NMA$C_PCNO_DPX      2751
#define     NMA$C_PCNO_COP      2760
#define     NMA$C_PCNO_INB      2765
#define     NMA$C_PCNO_LAA      2770
#define     NMA$C_PCNO_LAP      2771
#define     NMA$C_PCNO_PSP      2780

#define     NMA$C_PCNO_MDO      2785
#define     NMA$C_PCNO_DNS      2790
#define     NMA$C_PCNO_IDP      2791
#define     NMA$C_PCNO_DNM      2792

#define     NMA$C_PCNO_SRV_RPA  3300

#define     NMA$C_PCNO_SRV_TPA  3301

#define     NMA$C_PCNO_SRV_VER  3310

#define     NMA$C_PCNO_SRV_ACB  3402
#define     NMA$C_PCNO_SRV_ASB  3404
#define     NMA$C_PCNO_SRV_ALB  3406
#define     NMA$C_PCNO_SRV_MCB  3410
#define     NMA$C_PCNO_SRV_MSB  3420
#define     NMA$C_PCNO_SRV_MLB  3430
#define     NMA$C_PCNO_SRV_LBS  3431
#define     NMA$C_PCNO_SRV_NRB  3440
#define     NMA$C_PCNO_SRV_CPT  3450
#define     NMA$C_PCNO_SRV_CPF  3452
#define     NMA$C_PCNO_SRV_CPL  3454
#define     NMA$C_PCNO_SRV_XPT  3460
#define     NMA$C_PCNO_SRV_XPF  3462
#define     NMA$C_PCNO_SRV_XPL  3464

#define     NMA$C_PCAR_STA      0
#define     NMA$C_PCAR_COS      820
#define     NMA$C_PCAR_HOP      821
#define     NMA$C_PCAR_CIR      822
#define     NMA$C_PCAR_NND      830

#define     NMA$C_PCOB_OAN      400
#define     NMA$C_PCOB_OAC      410
#define     NMA$C_PCOB_ONA      500
#define     NMA$C_PCOB_OCO      510
#define     NMA$C_PCOB_OUS      511
#define     NMA$C_PCOB_OVE      520
#define     NMA$C_PCOB_NAM      500
#define     NMA$C_PCOB_NUM      513
#define     NMA$C_PCOB_FID      530
#define     NMA$C_PCOB_PID      535
#define     NMA$C_PCOB_PRV      540
#define     NMA$C_PCOB_OCPRV    542
#define     NMA$C_PCOB_USR      550
#define     NMA$C_PCOB_ACC      551
#define     NMA$C_PCOB_PSW      552
#define     NMA$C_PCOB_PRX      560
#define     NMA$C_PCOB_ALO      565
#define     NMA$C_PCOB_ALI      566

#define     NMA$C_PCLK_STA      0
#define     NMA$C_PCLK_PID      101
#define     NMA$C_PCLK_NID      102
#define     NMA$C_PCLK_LAD      105

#define     NMA$C_PCLK_DLY      110
#define     NMA$C_PCLK_RLN      120
#define     NMA$C_PCLK_RID      121
#define     NMA$C_PCLK_USR      130
#define     NMA$C_PCLK_PRC      131

#define     NMA$C_CTCIR_ZER     0
#define     NMA$C_CTCIR_APR     800
#define     NMA$C_CTCIR_DPS     801
#define     NMA$C_CTCIR_ACL     802
#define     NMA$C_CTCIR_CRL     805
#define     NMA$C_CTCIR_TPR     810
#define     NMA$C_CTCIR_TPS     811
#define     NMA$C_CTCIR_TCL     812
#define     NMA$C_CTCIR_LDN     820
#define     NMA$C_CTCIR_IFL     821
#define     NMA$C_CTCIR_AJD     822
#define     NMA$C_CTCIR_PAJ     900
#define     NMA$C_CTCIR_BRC     1000
#define     NMA$C_CTCIR_BSN     1001
#define     NMA$C_CTCIR_MBY     1002
#define     NMA$C_CTCIR_DBR     1010
#define     NMA$C_CTCIR_DBS     1011
#define     NMA$C_CTCIR_DEI     1020
#define     NMA$C_CTCIR_DEO     1021
#define     NMA$C_CTCIR_RRT     1030
#define     NMA$C_CTCIR_LRT     1031
#define     NMA$C_CTCIR_RBE     1040
#define     NMA$C_CTCIR_LBE     1041
#define     NMA$C_CTCIR_SIE     1050
#define     NMA$C_CTCIR_SLT     1051
#define     NMA$C_CTCIR_UBU     1065
#define     NMA$C_CTCIR_RPE     1100
#define     NMA$C_CTCIR_LPE     1101
#define     NMA$C_CTCIR_LIR     1240
#define     NMA$C_CTCIR_RIR     1241
#define     NMA$C_CTCIR_NIR     1242

#define     NMA$C_CTCIR_MNE     2701

#define     NMA$C_CTCIR_ERI     2750

#define     NMA$C_CTCIR_ERO     2751

#define     NMA$C_CTCIR_RTO     2752

#define     NMA$C_CTCIR_LTO     2753
#define     NMA$C_CTCIR_BER     2754
#define     NMA$C_CTCIR_BEL     2755

#define     NMA$C_CTLIN_ZER     0
#define     NMA$C_CTLIN_APR     800
#define     NMA$C_CTLIN_DPS     801
#define     NMA$C_CTLIN_ACL     802
#define     NMA$C_CTLIN_TPR     810
#define     NMA$C_CTLIN_TPS     811
#define     NMA$C_CTLIN_TCL     812
#define     NMA$C_CTLIN_LDN     820
#define     NMA$C_CTLIN_IFL     821
#define     NMA$C_CTLIN_BRC     1000
#define     NMA$C_CTLIN_BSN     1001
#define     NMA$C_CTLIN_MBY     1002
#define     NMA$C_CTLIN_DBR     1010
#define     NMA$C_CTLIN_DBS     1011
#define     NMA$C_CTLIN_MBL     1012
#define     NMA$C_CTLIN_BID     1013
#define     NMA$C_CTLIN_BS1     1014
#define     NMA$C_CTLIN_BSM     1015
#define     NMA$C_CTLIN_MFC     1016
#define     NMA$C_CTLIN_MEC     1017
#define     NMA$C_CTLIN_MLC     1018
#define     NMA$C_CTLIN_DEI     1020
#define     NMA$C_CTLIN_DEO     1021
#define     NMA$C_CTLIN_RRT     1030
#define     NMA$C_CTLIN_LRT     1031
#define     NMA$C_CTLIN_RII     1032
#define     NMA$C_CTLIN_RIR     1033
#define     NMA$C_CTLIN_RBI     1034
#define     NMA$C_CTLIN_DAT     1035
#define     NMA$C_CTLIN_DTD     1036
#define     NMA$C_CTLIN_RPR     1037
#define     NMA$C_CTLIN_FSE     1038
#define     NMA$C_CTLIN_TRI     1039
#define     NMA$C_CTLIN_RBE     1040
#define     NMA$C_CTLIN_LBE     1041
#define     NMA$C_CTLIN_TRR     1042
#define     NMA$C_CTLIN_DBC     1043
#define     NMA$C_CTLIN_SIE     1050
#define     NMA$C_CTLIN_SLT     1051
#define     NMA$C_CTLIN_SFL     1060
#define     NMA$C_CTLIN_CDC     1061
#define     NMA$C_CTLIN_RFL     1062
#define     NMA$C_CTLIN_UFD     1063
#define     NMA$C_CTLIN_OVR     1064
#define     NMA$C_CTLIN_SBU     1065
#define     NMA$C_CTLIN_UBU     1066
#define     NMA$C_CTLIN_SFR     1070
#define     NMA$C_CTLIN_RFR     1071
#define     NMA$C_CTLIN_IFR     1072
#define     NMA$C_CTLIN_RGF     1073
#define     NMA$C_CTLIN_RPG     1074
#define     NMA$C_CTLIN_MNC     1075
#define     NMA$C_CTLIN_BCN     1076
#define     NMA$C_CTLIN_LER     1080
#define     NMA$C_CTLIN_IER     1081
#define     NMA$C_CTLIN_BER     1082
#define     NMA$C_CTLIN_RAE     1083
#define     NMA$C_CTLIN_ADS     1084
#define     NMA$C_CTLIN_PIE     1085
#define     NMA$C_CTLIN_TLF     1086
#define     NMA$C_CTLIN_RCE     1087
#define     NMA$C_CTLIN_FCE     1088
#define     NMA$C_CTLIN_FER     1089
#define     NMA$C_CTLIN_TER     1090
#define     NMA$C_CTLIN_PNE     1091
#define     NMA$C_CTLIN_RPE     1100
#define     NMA$C_CTLIN_LPE     1101
#define     NMA$C_CTLIN_EBE     1200
#define     NMA$C_CTLIN_LCT     1201
#define     NMA$C_CTLIN_LEM     1202
#define     NMA$C_CTLIN_LNK     1203
#define     NMA$C_CTLIN_CNC     1204

#define     NMA$S_NMADEF1       2

struct _nma1
{
    union
    {
        UINT16 nma$w_node;
        struct
        {
            unsigned nma$v_addr     : 10;
            unsigned nma$v_area     : 6;
        };
        struct
        {
            unsigned nma$v_pty_typ      : 15;
            unsigned nma$v_fill_2_      : 1;
        };
        struct
        {
            unsigned nma$v_pty_cle      : 6;
            unsigned nma$v_pty_mul      : 1;
            unsigned nma$v_pty_cod      : 1;
        };
        struct
        {
            unsigned nma$$_fill_7       : 6;
            unsigned nma$v_pty_cmu      : 2;
        };
        struct
        {
            unsigned nma$v_pty_nle      : 4;
            unsigned nma$v_pty_nty      : 2;
            unsigned nma$v_pty_asc      : 1;
            unsigned nma$v_fill_3_      : 1;
        };
    };
};

#define     NMA$M_CTLIN_BTL     0x8
#define     NMA$M_CTLIN_FCS     0x10
#define     NMA$M_CTLIN_TRJ     0x20
#define     NMA$S_NMADEF2       1

struct _nma2
{
    union
    {
        INT8 nma$$_fill_8;
        struct
        {
            unsigned nma$$_fill_9       : 3;
            unsigned nma$v_ctlin_btl        : 1;
            unsigned nma$v_ctlin_fcs        : 1;
            unsigned nma$v_ctlin_trj        : 1;
            unsigned nma$v_fill_4_      : 2;
        };
    };
};

#define     NMA$M_CTLIN_RRJ     0x8
#define     NMA$S_NMADEF3       1

struct _nma3
{
    union
    {
        INT8 nma$$_fill_10;
        struct
        {
            unsigned nma$$_fill_11      : 3;
            unsigned nma$v_ctlin_rrj        : 1;
            unsigned nma$v_fill_5_      : 4;
        };
    };
};

#define     NMA$M_CTLIN_RRN     0x4
#define     NMA$S_NMADEF4       1

struct _nma4
{
    union
    {
        INT8 nma$$_fill_12;
        struct
        {
            unsigned nma$$_fill_13      : 2;
            unsigned nma$v_ctlin_rrn        : 1;
            unsigned nma$v_fill_6_      : 5;
        };
    };
};

#define     NMA$M_CTLIN_TRN     0x4
#define     NMA$S_NMADEF5       1

struct _nma5
{
    union
    {
        INT8 nma$$_fill_14;
        struct
        {
            unsigned nma$$_fill_15      : 2;
            unsigned nma$v_ctlin_trn        : 1;
            unsigned nma$v_fill_7_      : 5;
        };
    };
};

#define     NMA$M_CTLIN_INR     0x10
#define     NMA$M_CTLIN_FMS     0x20
#define     NMA$S_NMADEF6       1

struct _nma6
{
    union
    {
        INT8 nma$$_fill_16;
        struct
        {
            unsigned nma$$_fill_17      : 4;
            unsigned nma$v_ctlin_inr        : 1;
            unsigned nma$v_ctlin_fms        : 1;
            unsigned nma$v_fill_8_      : 2;
        };
    };
};

#define     NMA$M_CTLIN_TUN     0x4
#define     NMA$M_CTLIN_RUN     0x10
#define     NMA$M_CTLIN_FMR     0x20
#define     NMA$C_CTLIN_MBS     2701
#define     NMA$C_CTLIN_MSN     2702
#define     NMA$C_CTLIN_RME     2750

#define     NMA$C_CTLIN_LCE     2751

#define     NMA$C_CTLIN_MSE     2752

#define     NMA$C_CTNOD_ZER     0
#define     NMA$C_CTNOD_BRC     600
#define     NMA$C_CTNOD_BSN     601
#define     NMA$C_CTNOD_MRC     610
#define     NMA$C_CTNOD_MSN     611
#define     NMA$C_CTNOD_CRC     620
#define     NMA$C_CTNOD_CSN     621
#define     NMA$C_CTNOD_RTO     630
#define     NMA$C_CTNOD_RSE     640
#define     NMA$C_CTNOD_BUN     650
#define     NMA$C_CTNOD_MLL     700
#define     NMA$C_CTNOD_APL     900
#define     NMA$C_CTNOD_NUL     901
#define     NMA$C_CTNOD_NOL     902
#define     NMA$C_CTNOD_OPL     903
#define     NMA$C_CTNOD_PFE     910
#define     NMA$C_CTNOD_RUL     920
#define     NMA$C_CTNOD_VER     930

#define     NMA$C_CTNOD_SRV_SYC 3310
#define     NMA$C_CTNOD_SRV_SYS 3320
#define     NMA$C_CTNOD_SRV_SYL 3330
#define     NMA$C_CTNOD_SRV_SYR 3340

#define     NMA$C_CTXP_ZER      0
#define     NMA$C_CTXP_BRC      1000
#define     NMA$C_CTXP_BSN      1001
#define     NMA$C_CTXP_BLR      1010
#define     NMA$C_CTXP_BLS      1011
#define     NMA$C_CTXP_CRC      1200
#define     NMA$C_CTXP_CSN      1201
#define     NMA$C_CTXP_FSR      1210
#define     NMA$C_CTXP_FSS      1211
#define     NMA$C_CTXP_MSA      1220
#define     NMA$C_CTXP_MCA      1221
#define     NMA$C_CTXP_RSE      1230
#define     NMA$C_CTXP_LIR      1240
#define     NMA$C_CTXP_RIR      1241
#define     NMA$C_CTXP_NIR      1242
#define     NMA$C_CTXP_RST      1250

#define     NMA$C_CTXS_ZER      0
#define     NMA$C_CTXS_MCA      200
#define     NMA$C_CTXS_ICR      210
#define     NMA$C_CTXS_LLR      211

#define     NMA$C_LOOP_MIX      2
#define     NMA$C_LOOP_ONE      1
#define     NMA$C_LOOP_ZER      0

#define     NMA$C_LOOP_DCNT     1
#define     NMA$C_LOOP_DSIZ     40

#define     NMA$C_LOOP_XMIT     0
#define     NMA$C_LOOP_RECV     1
#define     NMA$C_LOOP_FULL     2

#define     NMA$C_STATE_ON      0
#define     NMA$C_STATE_OFF     1

#define     NMA$C_DNS_ENA       0
#define     NMA$C_DNS_DIS       1

#define     NMA$C_STATE_SER     2
#define     NMA$C_STATE_CLE     3

#define     NMA$C_STATE_HOL     2

#define     NMA$C_STATE_SHU     2
#define     NMA$C_STATE_RES     3
#define     NMA$C_STATE_REA     4
#define     NMA$C_STATE_UNR     5

#define     NMA$C_PCNO_DMAD     1023

#define     NMA$C_ASS_ENA       0
#define     NMA$C_ASS_DIS       1

#define     NMA$C_SUR_ENA       0
#define     NMA$C_SUR_DIS       1

#define     NMA$C_LINSS_STA     0
#define     NMA$C_LINSS_REF     1
#define     NMA$C_LINSS_LOO     2
#define     NMA$C_LINSS_LOA     3
#define     NMA$C_LINSS_DUM     4
#define     NMA$C_LINSS_TRI     5
#define     NMA$C_LINSS_ASE     6
#define     NMA$C_LINSS_ALO     7
#define     NMA$C_LINSS_ADU     8
#define     NMA$C_LINSS_ATR     9
#define     NMA$C_LINSS_SYN     10
#define     NMA$C_LINSS_FAI     11
#define     NMA$C_LINSS_RUN     12
#define     NMA$C_LINSS_UNS     13
#define     NMA$C_LINSS_IDL     14

#define     NMA$C_CIRTY_POI     0
#define     NMA$C_CIRTY_CON     1
#define     NMA$C_CIRTY_TRI     2
#define     NMA$C_CIRTY_X25     3
#define     NMA$C_CIRTY_DMC     4

#define     NMA$C_CIRTY_NI      6
#define     NMA$C_CIRTY_TRNG    11
#define     NMA$C_CIRTY_FDDI    12

#define     NMA$C_LINSV_ENA     0
#define     NMA$C_LINSV_DIS     1

#define     NMA$C_CIRPST_AUT    1
#define     NMA$C_CIRPST_ACT    2
#define     NMA$C_CIRPST_INA    3
#define     NMA$C_CIRPST_DIE    4
#define     NMA$C_CIRPST_DED    5

#define     NMA$C_CIRBLK_ENA    0
#define     NMA$C_CIRBLK_DIS    1

#define     NMA$C_CIRUS_PER     0
#define     NMA$C_CIRUS_INC     1
#define     NMA$C_CIRUS_OUT     2

#define     NMA$C_CIRHS_ENA     0
#define     NMA$C_CIRHS_DIS     1

#define     NMA$C_CIRBF_UNL     255

#define     NMA$C_CIRVE_ENA     0
#define     NMA$C_CIRVE_DIS     1
#define     NMA$C_CIRVE_INB     2

#define     NMA$C_CIRXPT_ZND    1
#define     NMA$C_CIRXPT_PH2    2
#define     NMA$C_CIRXPT_PH3    3
#define     NMA$C_CIRXPT_RO3    3
#define     NMA$C_CIRXPT_NR4    4

#define     NMA$C_DPX_FUL       0
#define     NMA$C_DPX_HAL       1
#define     NMA$C_DPX_MPT       4

#define     NMA$C_LINCN_NOR     0
#define     NMA$C_LINCN_LOO     1

#define     NMA$C_LINPR_POI     0
#define     NMA$C_LINPR_CON     1
#define     NMA$C_LINPR_TRI     2
#define     NMA$C_LINPR_DMC     4
#define     NMA$C_LINPR_LAPB    5
#define     NMA$C_LINPR_NI      6
#define     NMA$C_LINPR_BSY     9
#define     NMA$C_LINPR_GENBYTE 9
#define     NMA$C_LINPR_LAPBE   10
#define     NMA$C_LINPR_TRNG    11
#define     NMA$C_LINPR_FDDI    12
#define     NMA$C_LINPR_EA_HDLC 20
#define     NMA$C_LINPR_SDLC    21
#define     NMA$C_LINPR_BISYNC  22
#define     NMA$C_LINPR_SWIFT   23
#define     NMA$C_LINPR_CHIPS   24
#define     NMA$M_LINPR_MOP     128

#define     NMA$C_CODE_ASCII    1
#define     NMA$C_CODE_EBCDIC   2

#define     NMA$C_LINPR_MAS     1
#define     NMA$C_LINPR_NEU     2
#define     NMA$C_LINPR_SEC     0

#define     NMA$C_LINCL_EXT     0
#define     NMA$C_LINCL_INT     1

#define     NMA$C_LINFM_802E    0
#define     NMA$C_LINFM_ETH     1
#define     NMA$C_LINFM_802     2
#define     NMA$C_LINFM_SMT     4
#define     NMA$C_LINFM_ATM     6

#define     NMA$C_LINCN_LEN     0
#define     NMA$C_LINCN_NAM     1

#define     NMA$C_LINSR_USR     1
#define     NMA$C_LINSR_CLI     2

#define     NMA$C_LINSWI_DIS        1
#define     NMA$C_LINSWI_ENA        0

#define     NMA$C_LINRPE_ON         1
#define     NMA$C_LINRPE_OFF        0

#define     NMA$C_LINATY_HIORD      0
#define     NMA$C_LINATY_HW         1
#define     NMA$C_LINATY_USER       2

#define     NMA$C_LINRNG_FOUR       0
#define     NMA$C_LINRNG_SIXTN      1

#define     NMA$C_LINETR_ENA        0
#define     NMA$C_LINETR_DIS        1

#define     NMA$C_LINSRC_ENA        0
#define     NMA$C_LINSRC_DIS        1

#define     NMA$C_MEDIA_STP         0
#define     NMA$C_MEDIA_UTP         1
#define     NMA$C_MEDIA_AUI         2
#define     NMA$C_MEDIA_TP          3
#define     NMA$C_MEDIA_AUTO        4
#define     NMA$C_MEDIA_UNSPECIFIED     5
#define     NMA$C_MEDIA_BNC         6
#define     NMA$C_MEDIA_ANY         65535

#define     NMA$C_LINHNG_DIS        1
#define     NMA$C_LINHNG_ENA        0

#define     NMA$C_LINRES_DIS        1
#define     NMA$C_LINRES_ENA        0

#define     NMA$C_LINTY_POI     0
#define     NMA$C_LINTY_CON     1
#define     NMA$C_LINTY_TRI     2
#define     NMA$C_LINTY_DMC     3

#define     NMA$C_LINMC_SET     1
#define     NMA$C_LINMC_CLR     2
#define     NMA$C_LINMC_CAL     3
#define     NMA$C_LINMC_SDF     4

#define     NMA$C_LINDAT_UNK        0
#define     NMA$C_LINDAT_SUC        1
#define     NMA$C_LINDAT_DUP        2

#define     NMA$C_LINUN_DAT_UNK     0
#define     NMA$C_LINUN_DAT_SUC     1
#define     NMA$C_LINUN_DAT_DUP     2

#define     NMA$C_LINRPS_OFF        0
#define     NMA$C_LINRPS_CAN        1
#define     NMA$C_LINRPS_NON        2
#define     NMA$C_LINRPS_PUR        3

#define     NMA$C_LINRER_NOE        0
#define     NMA$C_LINRER_RII        5
#define     NMA$C_LINRER_RIR        6
#define     NMA$C_LINRER_RBI        7
#define     NMA$C_LINRER_DAD        8
#define     NMA$C_LINRER_DTD        9
#define     NMA$C_LINRER_RPE        10
#define     NMA$C_LINRER_FSE        11
#define     NMA$C_LINRER_ROC        12
#define     NMA$C_LINRER_DBR        13
#define     NMA$C_LINRER_PCTI       14
#define     NMA$C_LINRER_PCTR       15

#define     NMA$C_LINNBR_PHY_A      0
#define     NMA$C_LINNBR_PHY_B      1
#define     NMA$C_LINNBR_PHY_S      2
#define     NMA$C_LINNBR_PHY_M      3
#define     NMA$C_LINNBR_PHY_U      4

#define     NMA$C_LINRJR_NON        0
#define     NMA$C_LINRJR_LLCT       1
#define     NMA$C_LINRJR_RLCT       2
#define     NMA$C_LINRJR_LCTB       3
#define     NMA$C_LINRJR_LEM        4
#define     NMA$C_LINRJR_TOP        5
#define     NMA$C_LINRJR_NRJ        6
#define     NMA$C_LINRJR_RRJ        7
#define     NMA$C_LINRJR_TIP        8
#define     NMA$C_LINRJR_TRD        9
#define     NMA$C_LINRJR_STA        10
#define     NMA$C_LINRJR_LCTE       11

#define     NMA$C_ACC_SHR       1
#define     NMA$C_ACC_LIM       2
#define     NMA$C_ACC_EXC       3
#define     NMA$C_ACC_SEL       4

#define     NMA$C_LINMO_AUT     1
#define     NMA$C_LINMO_SIL     2

#define     NMA$C_X25MD_DTE     1
#define     NMA$C_X25MD_DCE     2
#define     NMA$C_X25MD_DTL     3
#define     NMA$C_X25MD_DCL     4
#define     NMA$C_X25MD_NEG     5

#define     NMA$C_X25RED_BUSY       0
#define     NMA$C_X25RED_OUT_OF_ORDER   1
#define     NMA$C_X25RED_SYSTEMATIC     2

#define     NMA$C_NODTY_ROU     0
#define     NMA$C_NODTY_NON     1
#define     NMA$C_NODTY_PHA     2
#define     NMA$C_NODTY_AREA    3
#define     NMA$C_NODTY_RT4     4
#define     NMA$C_NODTY_NR4     5
#define     NMA$C_NODTY_AREAP   6
#define     NMA$C_NODTY_RT4P    7
#define     NMA$C_NODTY_NR4P    8

#define     NMA$C_NODINB_ROUT   1
#define     NMA$C_NODINB_ENDN   2

#define     NMA$C_NODPW_SET     0

#define     NMA$C_CPU_8     0
#define     NMA$C_CPU_11        1
#define     NMA$C_CPU_1020      2
#define     NMA$C_CPU_VAX       3

#define     NMA$C_NODSNV_PH3    0
#define     NMA$C_NODSNV_PH4    1

#define     NMA$C_SOFT_SECL     0
#define     NMA$C_SOFT_TERL     1
#define     NMA$C_SOFT_OSYS     2
#define     NMA$C_SOFT_DIAG     3

#define     NMA$C_ACES_NONE     0
#define     NMA$C_ACES_INCO     1
#define     NMA$C_ACES_OUTG     2
#define     NMA$C_ACES_BOTH     3
#define     NMA$C_ACES_REQU     4

#define     NMA$C_ALIINC_ENA    0
#define     NMA$C_ALIINC_DIS    1

#define     NMA$C_ALOUT_ENA     0
#define     NMA$C_ALOUT_DIS     1

#define     NMA$C_ALINC_ENA     0
#define     NMA$C_ALINC_DIS     1

#define     NMA$C_PRXY_ENA      0
#define     NMA$C_PRXY_DIS      1

#define     NMA$C_PSPCY_NOR     0
#define     NMA$C_PSPCY_INT     1

#define     NMA$C_XPRTY_BIL     1

#define     NMA$C_XPRST_ON      0
#define     NMA$C_XPRST_OFF     1
#define     NMA$C_XPRST_SHU     2

#define     NMA$C_XPRMN_ENA     0
#define     NMA$C_XPRMN_DIS     1

#define     NMA$C_XPRSB_RUN     12
#define     NMA$C_XPRSB_UNS     13
#define     NMA$C_XPRSB_SYN     10

#define     NMA$C_CLEAR_STRING  0
#define     NMA$C_CLEAR_LONGWORD    -1
#define     NMA$C_CAL_CLR       0
#define     NMA$C_CAL_NOW       1
#define     NMA$C_DAY_ALL       0
#define     NMA$C_DAY_MON       1
#define     NMA$C_DAY_TUE       2
#define     NMA$C_DAY_WED       3
#define     NMA$C_DAY_THU       4
#define     NMA$C_DAY_FRI       5
#define     NMA$C_DAY_SAT       6
#define     NMA$C_DAY_SUN       7
#define     NMA$C_TIC_NO_CUT    0
#define     NMA$C_TIC_CUT       1
#define     NMA$C_CSG_NO_SIGNAL 0
#define     NMA$C_CSG_SIGNAL    1
#define     NMA$C_IRC_DIS       0
#define     NMA$C_IRC_ENA       1
#define     NMA$C_ORC_DIS       0
#define     NMA$C_ORC_ENA       1
#define     NMA$C_RED_DIS       0
#define     NMA$C_RED_ENA       1
#define     NMA$C_MOD_NOAUTO    0
#define     NMA$C_MOD_AUTO      1
#define     NMA$C_SWC_DIS       0
#define     NMA$C_SWC_ENA       1
#define     NMA$C_MDM_OFF       0
#define     NMA$C_MDM_ON        1
#define     NMA$C_DTS_NO_CABLE  1
#define     NMA$C_DTS_NO_X21_CABLE  2
#define     NMA$C_DTS_READY     3
#define     NMA$C_DTS_NOT_READY 4
#define     NMA$C_DTS_ACTIVE    5
#define     NMA$C_DTS_NO_OUTGOING   6
#define     NMA$C_CAS_NONE      1
#define     NMA$C_CAS_OUT       2
#define     NMA$C_CAS_IN        3
#define     NMA$C_CAS_OUT_R     4
#define     NMA$C_CAS_IN_R      5
#define     NMA$C_DTL_ACCEPT    1
#define     NMA$C_DTL_REJECT    2
#define     NMA$C_CAC_MAN       1
#define     NMA$C_CAC_AUTO_CONNECT  2
#define     NMA$C_CAC_AUTO_ACCEPT   3

#define     NMA$C_JAN       1
#define     NMA$C_FEB       2
#define     NMA$C_MAR       3
#define     NMA$C_APR       4
#define     NMA$C_MAY       5
#define     NMA$C_JUN       6
#define     NMA$C_JUL       7
#define     NMA$C_AUG       8
#define     NMA$C_SEP       9
#define     NMA$C_OCT       10
#define     NMA$C_NOV       11
#define     NMA$C_DEC       12

#define     NMA$C_SOFD_DP       0
#define     NMA$C_SOFD_UNA      1
#define     NMA$C_SOFD_DU       2
#define     NMA$C_SOFD_CNA      3
#define     NMA$C_SOFD_DL       4
#define     NMA$C_SOFD_QNA      5
#define     NMA$C_SOFD_DQ       6
#define     NMA$C_SOFD_CI       7
#define     NMA$C_SOFD_DA       8
#define     NMA$C_SOFD_PCL      9
#define     NMA$C_SOFD_DUP      10
#define     NMA$C_SOFD_LUA      11
#define     NMA$C_SOFD_DMC      12
#define     NMA$C_SOFD_LNA      13
#define     NMA$C_SOFD_DN       14
#define     NMA$C_SOFD_DLV      16
#define     NMA$C_SOFD_LCS      17
#define     NMA$C_SOFD_DMP      18
#define     NMA$C_SOFD_AMB      19
#define     NMA$C_SOFD_DTE      20
#define     NMA$C_SOFD_DBT      21
#define     NMA$C_SOFD_DV       22
#define     NMA$C_SOFD_BNA      23
#define     NMA$C_SOFD_BNT      23
#define     NMA$C_SOFD_DZ       24
#define     NMA$C_SOFD_LPC      25
#define     NMA$C_SOFD_DSV      26
#define     NMA$C_SOFD_CEC      27
#define     NMA$C_SOFD_KDP      28
#define     NMA$C_SOFD_IEC      29
#define     NMA$C_SOFD_KDZ      30
#define     NMA$C_SOFD_UEC      31
#define     NMA$C_SOFD_KL8      32
#define     NMA$C_SOFD_DS2      33
#define     NMA$C_SOFD_DMV      34
#define     NMA$C_SOFD_DS5      35
#define     NMA$C_SOFD_DPV      36
#define     NMA$C_SOFD_LQA      37
#define     NMA$C_SOFD_DMF      38
#define     NMA$C_SOFD_SVA      39
#define     NMA$C_SOFD_DMR      40
#define     NMA$C_SOFD_MUX      41
#define     NMA$C_SOFD_KMY      42
#define     NMA$C_SOFD_DEP      43
#define     NMA$C_SOFD_KMX      44
#define     NMA$C_SOFD_LTM      45
#define     NMA$C_SOFD_DMB      46
#define     NMA$C_SOFD_DES      47
#define     NMA$C_SOFD_KCP      48
#define     NMA$C_SOFD_MX3      49
#define     NMA$C_SOFD_SYN      50
#define     NMA$C_SOFD_MEB      51
#define     NMA$C_SOFD_DSB      52
#define     NMA$C_SOFD_BAM      53
#define     NMA$C_SOFD_DST      54
#define     NMA$C_SOFD_FAT      55
#define     NMA$C_SOFD_RSM      56
#define     NMA$C_SOFD_RES      57
#define     NMA$C_SOFD_3C2      58
#define     NMA$C_SOFD_3CM      59
#define     NMA$C_SOFD_DS3      60
#define     NMA$C_SOFD_MF2      61
#define     NMA$C_SOFD_MMR      62
#define     NMA$C_SOFD_VIT      63
#define     NMA$C_SOFD_VT5      64
#define     NMA$C_SOFD_BNI      65
#define     NMA$C_SOFD_MNA      66
#define     NMA$C_SOFD_PMX      67
#define     NMA$C_SOFD_NI5      68
#define     NMA$C_SOFD_NI9      69
#define     NMA$C_SOFD_KMK      70
#define     NMA$C_SOFD_3CP      71
#define     NMA$C_SOFD_DP2      72
#define     NMA$C_SOFD_ISA      73
#define     NMA$C_SOFD_DIV      74
#define     NMA$C_SOFD_QTA      75
#define     NMA$C_SOFD_B15      76
#define     NMA$C_SOFD_WD8      77
#define     NMA$C_SOFD_ILA      78
#define     NMA$C_SOFD_ILM      79
#define     NMA$C_SOFD_APR      80
#define     NMA$C_SOFD_ASN      81
#define     NMA$C_SOFD_ASE      82
#define     NMA$C_SOFD_TRW      83
#define     NMA$C_SOFD_EDX      84
#define     NMA$C_SOFD_EDA      85
#define     NMA$C_SOFD_DR2      86
#define     NMA$C_SOFD_SCC      87
#define     NMA$C_SOFD_DCA      88
#define     NMA$C_SOFD_TIA      89
#define     NMA$C_SOFD_FBN      90
#define     NMA$C_SOFD_FEB      91
#define     NMA$C_SOFD_FCN      92
#define     NMA$C_SOFD_MFA      93
#define     NMA$C_SOFD_MXE      94
#define     NMA$C_SOFD_CED      95
#define     NMA$C_SOFD_C20      96
#define     NMA$C_SOFD_CS1      97
#define     NMA$C_SOFD_C2M      98
#define     NMA$C_SOFD_ACA      99
#define     NMA$C_SOFD_GSM      100
#define     NMA$C_SOFD_DSF      101
#define     NMA$C_SOFD_CS5      102
#define     NMA$C_SOFD_XIR      103
#define     NMA$C_SOFD_KFE      104
#define     NMA$C_SOFD_RT3      105
#define     NMA$C_SOFD_SPI      106
#define     NMA$C_SOFD_FOR      107
#define     NMA$C_SOFD_MER      108
#define     NMA$C_SOFD_PER      109
#define     NMA$C_SOFD_STR      110
#define     NMA$C_SOFD_MPS      111
#define     NMA$C_SOFD_L20      112
#define     NMA$C_SOFD_VT2      113
#define     NMA$C_SOFD_DWT      114
#define     NMA$C_SOFD_WGB      115
#define     NMA$C_SOFD_ZEN      116
#define     NMA$C_SOFD_TSS      117
#define     NMA$C_SOFD_MNE      118
#define     NMA$C_SOFD_FZA      119
#define     NMA$C_SOFD_90L      120
#define     NMA$C_SOFD_CIS      121
#define     NMA$C_SOFD_STC      122
#define     NMA$C_SOFD_UBE      123
#define     NMA$C_SOFD_DW2      124
#define     NMA$C_SOFD_FUE      125
#define     NMA$C_SOFD_M38      126
#define     NMA$C_SOFD_NTI      127
#define     NMA$C_SOFD_RAD      130
#define     NMA$C_SOFD_INF      131
#define     NMA$C_SOFD_XMX      132
#define     NMA$C_SOFD_NDI      133
#define     NMA$C_SOFD_ND2      134
#define     NMA$C_SOFD_TRN      135
#define     NMA$C_SOFD_DEV      136
#define     NMA$C_SOFD_ACE      137
#define     NMA$C_SOFD_PNT      138
#define     NMA$C_SOFD_ISE      139
#define     NMA$C_SOFD_IST      140
#define     NMA$C_SOFD_ISH      141
#define     NMA$C_SOFD_ISF      142
#define     NMA$C_SOFD_DSW      149
#define     NMA$C_SOFD_DW4      150
#define     NMA$C_SOFD_ITC      154
#define     NMA$C_SOFD_FTA      160
#define     NMA$C_SOFD_FAA      161
#define     NMA$C_SOFD_FEA      162
#define     NMA$C_SOFD_FQA      169
#define     NMA$C_SOFD_A35      170
#define     NMA$C_SOFD_V49      172
#define     NMA$C_SOFD_TRA      175
#define     NMA$C_SOFD_TRB      176
#define     NMA$C_SOFD_ERA      182
#define     NMA$C_SOFD_A33      188
#define     NMA$C_SOFD_TRE      189
#define     NMA$C_SOFD_ETA      202
#define     NMA$C_SOFD_EWA      203
#define     NMA$C_SOFD_FWA      204
#define     NMA$C_SOFD_EIA      204
#define     NMA$C_SOFD_AZA      213
#define     NMA$C_SOFD_FPA      216
#define     NMA$C_SOFD_GPA      217
#define     NMA$C_SOFD_TRP      253
#define     NMA$C_SOFD_ELA      254
#define     NMA$C_SOFD_EBA      255

#define     NMA$_SUCCESS        1
#define     NMA$_SUCCFLDRPL     9
#define     NMA$_BADFID     0
#define     NMA$_BADDAT     8
#define     NMA$_BADOPR     16
#define     NMA$_BUFTOOSMALL    24
#define     NMA$_FLDNOTFND      32

#define     NMA$C_OPN_MIN       0
#define     NMA$C_OPN_NODE      0
#define     NMA$C_OPN_LINE      1
#define     NMA$C_OPN_LOG       2
#define     NMA$C_OPN_OBJ       3
#define     NMA$C_OPN_CIR       4
#define     NMA$C_OPN_X25       5
#define     NMA$C_OPN_X29       6
#define     NMA$C_OPN_CNF       7
#define     NMA$C_OPN_MAX       7
#define     NMA$C_OPN_ALL       127

#define     NMA$C_OPN_AC_RO     0
#define     NMA$C_OPN_AC_RW     1

#define     NMA$C_FN2_DLL       2
#define     NMA$C_FN2_ULD       3
#define     NMA$C_FN2_TRI       4
#define     NMA$C_FN2_LOO       5
#define     NMA$C_FN2_TES       6
#define     NMA$C_FN2_SET       7
#define     NMA$C_FN2_REA       8
#define     NMA$C_FN2_ZER       9
#define     NMA$C_FN2_LNS       14

#define     NMA$C_OP2_CHNST     5
#define     NMA$C_OP2_CHLST     8

#define     NMA$C_OP2_RENCT     0
#define     NMA$C_OP2_RENST     1
#define     NMA$C_OP2_RELCT     4
#define     NMA$C_OP2_RELST     5

#define     NMA$C_OP2_ZENCT     0
#define     NMA$C_OP2_ZELCT     2

#define     NMA$C_EN2_KNO       0
#define     NMA$C_EN2_LID       1
#define     NMA$C_EN2_LCN       2

#define     NMA$C_STS_SUC       1
#define     NMA$C_STS_MOR       2
#define     NMA$C_STS_PAR       3

#define     NMA$C_STS_DON       -128

#define     NMA$C_STS_FUN       -1
#define     NMA$C_STS_INV       -2
#define     NMA$C_STS_PRI       -3
#define     NMA$C_STS_SIZ       -4
#define     NMA$C_STS_MPR       -5
#define     NMA$C_STS_PTY       -6
#define     NMA$C_STS_MVE       -7
#define     NMA$C_STS_CMP       -8
#define     NMA$C_STS_IDE       -9
#define     NMA$C_STS_LCO       -10
#define     NMA$C_STS_STA       -11
#define     NMA$C_STS_FOP       -13
#define     NMA$C_STS_FCO       -14
#define     NMA$C_STS_RES       -15
#define     NMA$C_STS_PVA       -16
#define     NMA$C_STS_LPR       -17
#define     NMA$C_STS_FIO       -18
#define     NMA$C_STS_MLD       -19
#define     NMA$C_STS_ROO       -20
#define     NMA$C_STS_MCF       -21
#define     NMA$C_STS_PNA       -22
#define     NMA$C_STS_PLO       -23
#define     NMA$C_STS_HAR       -24
#define     NMA$C_STS_OPE       -25
#define     NMA$C_STS_SYS       -26

#define     NMA$C_STS_PGP       -27
#define     NMA$C_STS_BLR       -28
#define     NMA$C_STS_PMS       -29

#define     NMA$C_STS_ALI       -127
#define     NMA$C_STS_OBJ       -126
#define     NMA$C_STS_PRO       -125
#define     NMA$C_STS_LNK       -124

#define     NMA$C_FOPDTL_PDB        0
#define     NMA$C_FOPDTL_LFL        1
#define     NMA$C_FOPDTL_DFL        2
#define     NMA$C_FOPDTL_SLF        3
#define     NMA$C_FOPDTL_TLF        4
#define     NMA$C_FOPDTL_SDF        5
#define     NMA$C_FOPDTL_PDR        6
#define     NMA$C_FOPDTL_MFL        7

#define     NMA$C_NCEDTL_NNA        0
#define     NMA$C_NCEDTL_INN        1
#define     NMA$C_NCEDTL_UNA        2
#define     NMA$C_NCEDTL_UNR        3
#define     NMA$C_NCEDTL_RSC        4
#define     NMA$C_NCEDTL_RJC        5
#define     NMA$C_NCEDTL_ONA        6
#define     NMA$C_NCEDTL_OBJ        7
#define     NMA$C_NCEDTL_ACC        8
#define     NMA$C_NCEDTL_BSY        9
#define     NMA$C_NCEDTL_NRS        10
#define     NMA$C_NCEDTL_NSD        11
#define     NMA$C_NCEDTL_DIE        12
#define     NMA$C_NCEDTL_DIS        13
#define     NMA$C_NCEDTL_ABO        14
#define     NMA$C_NCEDTL_ABM        15

#define     NMA$C_OPEDTL_DCH        0
#define     NMA$C_OPEDTL_TIM        1
#define     NMA$C_OPEDTL_ORN        2
#define     NMA$C_OPEDTL_ACT        3
#define     NMA$C_OPEDTL_BAF        4
#define     NMA$C_OPEDTL_RUN        5
#define     NMA$C_OPEDTL_DSC        6
#define     NMA$C_OPEDTL_FTL        8
#define     NMA$C_OPEDTL_MNT        11
#define     NMA$C_OPEDTL_LST        12
#define     NMA$C_OPEDTL_THR        13
#define     NMA$C_OPEDTL_TRB        14
#define     NMA$C_OPEDTL_STA        15
#define     NMA$S_NMADEF7           1

struct _nma7
{
    union
    {
        INT8 nma$$_fill_18;
        struct
        {
            unsigned nma$$_fill_19      : 2;
            unsigned nma$v_ctlin_tun        : 1;
            unsigned nma$$_fill_20      : 1;
            unsigned nma$v_ctlin_run        : 1;
            unsigned nma$v_ctlin_fmr        : 1;
            unsigned nma$v_fill_9_      : 2;
        };
    };
};

#endif /* NMADEF_H */
