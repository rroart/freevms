#ifndef ttyucbdef_h
#define ttyucbdef_h

#include <ucbdef.h>

struct _ltrm_ucb
{
    struct _ucb ucb$r_ucb;
    struct _acb *ucb$l_tl_ctrly;
    struct _acb *ucb$l_tl_ctrlc;
    unsigned int ucb$l_tl_outband;
    long ucb$l_tl_bandque;
    struct _ucb *ucb$l_tl_phyucb;
    unsigned int ucb$l_tl_ctlpid;
    unsigned long long ucb$q_tl_brkthru;
    void *ucb$l_tl_posix_data;
    void *ucb$l_tl_asian_data;
    union
    {
        unsigned char ucb$b_tl_a_mode;
        unsigned int ucb$l_tl_a_charset;
    };
    void *ucb$l_tl_a_fi_ucb;
};

#define		UCB$C_TL_LENGTH		288
#define		UCB$K_TL_LENGTH		288

#define		TTY$M_ST_POWER		0x1
#define		TTY$M_ST_CTRLS		0x2
#define		TTY$M_ST_LOSTCTS	0x4
#define		TTY$M_ST_MODEM_OFF	0x8
#define		TTY$M_ST_POSIXSTALL	0x10
#define		TTY$M_ST_FILL		0x20
#define		TTY$M_ST_CURSOR		0x40
#define		TTY$M_ST_SENDLF		0x80
#define		TTY$M_ST_BACKSPACE	0x100
#define		TTY$M_ST_MULTI		0x200
#define		TTY$M_ST_WRITE		0x400
#define		TTY$M_ST_POSIXWRITE	0x800
#define		TTY$M_ST_EOL		0x1000
#define		TTY$M_ST_EDITREAD	0x2000
#define		TTY$M_ST_RDVERIFY	0x4000
#define		TTY$M_ST_RECALL		0x8000
#define		TTY$M_ST_READ		0x10000
#define		TTY$M_ST_POSIXREAD	0x20000
#define		TTY$M_ST_CTRLO		0x1
#define		TTY$M_ST_DEL		0x2
#define		TTY$M_ST_PASALL		0x4
#define		TTY$M_ST_NOECHO		0x8
#define		TTY$M_ST_WRTALL		0x10
#define		TTY$M_ST_PROMPT		0x20
#define		TTY$M_ST_NOFLTR		0x40
#define		TTY$M_ST_ESC		0x80
#define		TTY$M_ST_BADESC		0x100
#define		TTY$M_ST_NL		0x200
#define		TTY$M_ST_REFRSH		0x400
#define		TTY$M_ST_ESCAPE		0x800
#define		TTY$M_ST_TYPFUL		0x1000
#define		TTY$M_ST_SKIPLF		0x2000
#define		TTY$M_ST_ESC_O		0x4000
#define		TTY$M_ST_WRAP		0x8000
#define		TTY$M_ST_OVRFLO		0x10000
#define		TTY$M_ST_AUTOP		0x20000
#define		TTY$M_ST_CTRLR		0x40000
#define		TTY$M_ST_SKIPCRLF	0x80000
#define		TTY$M_ST_EDITING	0x100000
#define		TTY$M_ST_TABEXPAND	0x200000
#define		TTY$M_ST_QUOTING	0x400000
#define		TTY$M_ST_OVERSTRIKE	0x800000
#define		TTY$M_ST_TERMNORM	0x1000000
#define		TTY$M_ST_ECHAES		0x2000000
#define		TTY$M_ST_PRE		0x4000000
#define		TTY$M_ST_NINTMULTI	0x8000000
#define		TTY$M_ST_RECONNECT	0x10000000
#define		TTY$M_ST_CTSLOW		0x20000000
#define		TTY$M_ST_TABRIGHT	0x40000000
#define		UCB$M_TT_XXPARITY	0x1
#define		UCB$M_TT_DISPARERR	0x2
#define		UCB$M_TT_USERFRAME	0x4
#define		UCB$M_TT_LEN		0x18
#define		UCB$M_TT_STOP		0x20
#define		UCB$M_TT_PARTY		0x40
#define		UCB$M_TT_ODD		0x80
#define		TTY$M_TANK_PREMPT	0x100
#define		TTY$M_TANK_STOP		0x200
#define		TTY$M_TANK_HOLD		0x400
#define		TTY$M_TANK_BURST	0x800
#define		TTY$M_TANK_DMA		0x1000
#define		TTY$M_PC_NOTIME		0x1
#define		TTY$M_PC_DMAENA		0x2
#define		TTY$M_PC_DMAAVL		0x4
#define		TTY$M_PC_PRMMAP		0x8
#define		TTY$M_PC_MAPAVL		0x10
#define		TTY$M_PC_XOFAVL		0x20
#define		TTY$M_PC_XOFENA		0x40
#define		TTY$M_PC_NOCRLF		0x80
#define		TTY$M_PC_BREAK		0x100
#define		TTY$M_PC_PORTFDT	0x200
#define		TTY$M_PC_NOMODEM	0x400
#define		TTY$M_PC_NODISCONNECT	0x800
#define		TTY$M_PC_SMART_READ	0x1000
#define		TTY$M_PC_ACCPORNAM	0x2000
#define		TTY$M_PC_MULTISESSION	0x8000
#define		UCB$M_TT_DSBL		0x80

struct _tty_ucb
{
    struct _ltrm_ucb ucb$r_ltrmucb;
    unsigned int ucb$l_tt_rdue;
    void (*ucb$l_tt_rtimou)(void);
    union
    {
        unsigned long long ucb$q_tt_state;
        struct
        {
            union
            {
                unsigned int ucb$l_tt_state1;
                struct
                {
                    unsigned tty$v_st_power	: 1;
                    unsigned tty$v_st_ctrls	: 1;
                    unsigned tty$v_st_lostcts	: 1;
                    unsigned tty$v_st_modem_off	: 1;
                    unsigned tty$v_st_posixstall	: 1;
                    unsigned tty$v_st_fill	: 1;
                    unsigned tty$v_st_cursor	: 1;
                    unsigned tty$v_st_sendlf	: 1;
                    unsigned tty$v_st_backspace	: 1;
                    unsigned tty$v_st_multi	: 1;
                    unsigned tty$v_st_write	: 1;
                    unsigned tty$v_st_posixwrite	: 1;
                    unsigned tty$v_st_eol		: 1;
                    unsigned tty$v_st_editread	: 1;
                    unsigned tty$v_st_rdverify	: 1;
                    unsigned tty$v_st_recall	: 1;
                    unsigned tty$v_st_read	: 1;
                    unsigned tty$v_st_posixread	: 1;
                    unsigned tty$v_fill_0_	: 6;
                };
            };
            union
            {
                unsigned int ucb$l_tt_state2;
                struct
                {
                    unsigned tty$v_st_ctrlo	: 1;
                    unsigned tty$v_st_del		: 1;
                    unsigned tty$v_st_pasall	: 1;
                    unsigned tty$v_st_noecho	: 1;
                    unsigned tty$v_st_wrtall	: 1;
                    unsigned tty$v_st_prompt	: 1;
                    unsigned tty$v_st_nofltr	: 1;
                    unsigned tty$v_st_esc		: 1;
                    unsigned tty$v_st_badesc	: 1;
                    unsigned tty$v_st_nl		: 1;
                    unsigned tty$v_st_refrsh	: 1;
                    unsigned tty$v_st_escape	: 1;
                    unsigned tty$v_st_typful	: 1;
                    unsigned tty$v_st_skiplf	: 1;
                    unsigned tty$v_st_esc_o	: 1;
                    unsigned tty$v_st_wrap	: 1;
                    unsigned tty$v_st_ovrflo	: 1;
                    unsigned tty$v_st_autop	: 1;
                    unsigned tty$v_st_ctrlr	: 1;
                    unsigned tty$v_st_skipcrlf	: 1;
                    unsigned tty$v_st_editing	: 1;
                    unsigned tty$v_st_tabexpand	: 1;
                    unsigned tty$v_st_quoting	: 1;
                    unsigned tty$v_st_overstrike	: 1;
                    unsigned tty$v_st_termnorm	: 1;
                    unsigned tty$v_st_echaes	: 1;
                    unsigned tty$v_st_pre		: 1;
                    unsigned tty$v_st_nintmulti	: 1;
                    unsigned tty$v_st_reconnect	: 1;
                    unsigned tty$v_st_ctslow	: 1;
                    unsigned tty$v_st_tabright	: 1;
                    unsigned tty$v_fill_1_	: 1;
                };
            };
        };
    };
    struct _ucb *ucb$l_tt_logucb;
    unsigned int ucb$l_tt_dechar;
    unsigned int ucb$l_tt_decha1;
    unsigned int ucb$l_tt_decha2;
    unsigned int ucb$l_tt_decha3;
    struct _twp *ucb$l_tt_wflink;
    struct _twp *ucb$l_tt_wblink;
    struct _twp *ucb$l_tt_wrtbuf;
    void *ucb$l_tt_multi;
    unsigned short int ucb$w_tt_multilen;
    unsigned short int ucb$w_tt_smltlen;
    void *ucb$l_tt_smlt;
    unsigned short int ucb$w_tt_despee;
    unsigned char ucb$b_tt_decrf;
    unsigned char ucb$b_tt_delff;
    unsigned char ucb$b_tt_depari;
    unsigned char ucb$b_tt_defspe_spare1;
    unsigned short int ucb$w_tt_defspe_spare2;
    unsigned short int ucb$w_tt_desize;
    unsigned char ucb$b_tt_detype;
    unsigned char ucb$b_tt_spare1;
    union
    {
        unsigned short int ucb$w_tt_speed;
        struct
        {
            unsigned char ucb$b_tt_tspeed;
            unsigned char ucb$b_tt_rspeed;
        };
    };
    unsigned char ucb$b_tt_crfill;
    unsigned char ucb$b_tt_lffill;
    union
    {
        unsigned char ucb$b_tt_parity;
        struct
        {
            unsigned ucb$v_tt_xxparity	: 1;
            unsigned ucb$v_tt_disparerr	: 1;
            unsigned ucb$v_tt_userframe	: 1;
            unsigned ucb$v_tt_len		: 2;
            unsigned ucb$v_tt_stop		: 1;
            unsigned ucb$v_tt_party		: 1;
            unsigned ucb$v_tt_odd		: 1;
        };
    };
    unsigned char ucb$b_tt_par_spare1;
    unsigned short int ucb$w_tt_par_spare2;
    void *ucb$l_tt_typahd;
    unsigned short int ucb$w_tt_cursor;
    unsigned char ucb$b_tt_line;
    unsigned char ucb$b_tt_lastc;
    unsigned short int ucb$w_tt_bsplen;
    unsigned char ucb$b_tt_fill;
    unsigned char ucb$b_tt_esc;
    unsigned char ucb$b_tt_esc_o;
    unsigned char ucb$b_tt_intcnt;
    unsigned short int ucb$w_tt_unitbit;
    union
    {
        unsigned short int ucb$w_tt_hold;
        struct
        {
            unsigned char tty$b_tank_char;
            unsigned tty$v_tank_prempt	: 1;
            unsigned tty$v_tank_stop		: 1;
            unsigned tty$v_tank_hold		: 1;
            unsigned tty$v_tank_burst		: 1;
            unsigned tty$v_tank_dma		: 1;
            unsigned tty$v_fill_2_		: 3;
        };
    };
    unsigned char ucb$b_tt_prempt;
    char ucb$b_tt_outype;
    int (*ucb$l_tt_getnxt)(int * chr, int * CC, struct _ucb * u);
    int (*ucb$l_tt_putnxt)(int * chr, int * CC, struct _ucb * u);
    int ucb$l_tt_class;
    int ucb$l_tt_port;
    void *ucb$l_tt_outadr;
    unsigned short int ucb$w_tt_outlen;
    union
    {
        unsigned short int ucb$w_tt_prtctl;
        struct
        {
            unsigned tty$v_pc_notime		: 1;
            unsigned tty$v_pc_dmaena		: 1;
            unsigned tty$v_pc_dmaavl		: 1;
            unsigned tty$v_pc_prmmap		: 1;
            unsigned tty$v_pc_mapavl		: 1;
            unsigned tty$v_pc_xofavl		: 1;
            unsigned tty$v_pc_xofena		: 1;
            unsigned tty$v_pc_nocrlf		: 1;
            unsigned tty$v_pc_break		: 1;
            unsigned tty$v_pc_portfdt		: 1;
            unsigned tty$v_pc_nomodem		: 1;
            unsigned tty$v_pc_nodisconnect	: 1;
            unsigned tty$v_pc_smart_read	: 1;
            unsigned tty$v_pc_accpornam	: 1;
            unsigned tty$v_pc_fill_1		: 1;
            unsigned tty$v_pc_multisession	: 1;
        };
    };
    unsigned int ucb$l_tt_ds_st;
    unsigned char ucb$b_tt_ds_rcv;
    unsigned char ucb$b_tt_ds_tx;
    unsigned short int ucb$w_tt_ds_tim;
    union
    {
        unsigned char ucb$b_tt_maint;
        struct
        {
            unsigned ucb$v_tt_maint_fill	: 7;
            unsigned ucb$v_tt_dsbl		: 1;
        };
    };
    unsigned char ucb$b_tt_oldcpzorg;
    unsigned short int ucb$w_tt_fillrup;
    void *ucb$l_tt_fbk;
    void *ucb$l_tt_rdverify;
    unsigned int ucb$l_tt_class1;
    unsigned int ucb$l_tt_class2;
    void *ucb$l_tt_accpornam;
    void *ucb$l_tt_a_gcbadr;
    unsigned short int ucb$w_tt_a_edsts;
    unsigned char ucb$b_tt_a_state;
    unsigned char ucb$b_tt_a_parse;
    unsigned char ucb$b_tt_a_trans;
    unsigned char ucb$b_tt_a_xedsts;
    unsigned short int ucb$w_tt_a_resrv1;
    union
    {
        unsigned char ucb$b_tt_a_char;
        unsigned int ucb$l_tt_a_dechset;
    };
};

#define UCB$C_TT_CLSLEN 460
#define UCB$K_TT_CLSLEN 460

#define TTY$M_TP_ABORT 0x1
#define TTY$M_TP_ALLOC 0x2
#define TTY$M_TP_DLLOC 0x4

struct _tpd_ucb
{
    struct _tty_ucb ucb$r_ttyucb;
    int ucb$l_tp_map;
    union
    {
        unsigned char ucb$b_tp_stat;
        struct
        {
            unsigned tty$v_tp_abort	: 1;
            unsigned tty$v_tp_alloc	: 1;
            unsigned tty$v_tp_dlloc	: 1;
            unsigned tty$v_fill_3_	: 5;
        };
    };
    unsigned char ucb$b_tp_spare1;
    unsigned short int ucb$w_tp_spare2;
};

#define UCB$C_TP_LENGTH 468
#define UCB$K_TP_LENGTH 468
#define UCB$C_TT_LENGTH 468
#define UCB$K_TT_LENGTH 468

#define TTY$M_SX_LOSTCTS 0x4
#define TTY$M_SX_POSIXSTALL 0x10

struct _sx_state
{
    struct _tpd_ucb tty$r_tpducb;
    struct
    {
        unsigned tty$v_sx_power		: 1;
        unsigned tty$v_sx_ctrls		: 1;
        unsigned tty$v_sx_lostcts		: 1;
        unsigned tty$v_sx_modem_off		: 1;
        unsigned tty$v_sx_posixstall	: 1;
        unsigned tty$v_sx_fill		: 1;
        unsigned tty$v_sx_cursor		: 1;
        unsigned tty$v_sx_sendlf		: 1;
        unsigned tty$v_sx_backspace		: 1;
        unsigned tty$v_sx_multi		: 1;
        unsigned tty$v_sx_write		: 1;
        unsigned tty$v_sx_posixwrite	: 1;
        unsigned tty$v_sx_eol		: 1;
        unsigned tty$v_sx_editread		: 1;
        unsigned tty$v_sx_rdverify		: 1;
        unsigned tty$v_sx_recall		: 1;
        unsigned tty$v_sx_read		: 1;
        unsigned tty$v_sx_posixread		: 1;
        unsigned tty$v_sx_fillbits		: 14;
        unsigned tty$v_sx_ctrlo		: 1;
        unsigned tty$v_sx_del		: 1;
        unsigned tty$v_sx_pasall		: 1;
        unsigned tty$v_sx_noecho		: 1;
        unsigned tty$v_sx_wrtall		: 1;
        unsigned tty$v_sx_prompt		: 1;
        unsigned tty$v_sx_nofltr		: 1;
        unsigned tty$v_sx_esc		: 1;
        unsigned tty$v_sx_badesc		: 1;
        unsigned tty$v_sx_nl		: 1;
        unsigned tty$v_sx_refrsh		: 1;
        unsigned tty$v_sx_escape		: 1;
        unsigned tty$v_sx_typful		: 1;
        unsigned tty$v_sx_skiplf		: 1;
        unsigned tty$v_sx_esc_o		: 1;
        unsigned tty$v_sx_wrap		: 1;
        unsigned tty$v_sx_ovrflo		: 1;
        unsigned tty$v_sx_autop		: 1;
        unsigned tty$v_sx_ctrlr		: 1;
        unsigned tty$v_sx_skipcrlf		: 1;
        unsigned tty$v_sx_editing		: 1;
        unsigned tty$v_sx_tabexpand		: 1;
        unsigned tty$v_sx_quoting		: 1;
        unsigned tty$v_sx_overstrike	: 1;
        unsigned tty$v_sx_termnorm		: 1;
        unsigned tty$v_sx_echaes		: 1;
        unsigned tty$v_sx_pre		: 1;
        unsigned tty$v_sx_nintmulti		: 1;
        unsigned tty$v_sx_reconnect		: 1;
        unsigned tty$v_sx_ctslow		: 1;
        unsigned tty$v_sx_tabright		: 1;
        unsigned tty$v_fill_4_		: 1;
    };
};

#define TTY$S_TTYUCB 188
#define TTY$S_TTYRTTUCB 188
#define TTY$S_TTYUCBDEF 476

#define		FLG$M_CTRLO		0x1
#define		FLG$M_CANCTRLO		0x2
#define		FLG$M_VAXTOVAX		0x4
#define		FLG$M_CTRLC		0x8
#define		FLG$M_INIT		0x10
#define		FLG$M_RESET_TIMER	0x20
#define		FLG$M_DECNET_BUSY	0x40
#define		FLG$M_OUTPUT_BUSY	0x80
#define		FLG$M_READ_BUSY		0x100
#define		FLG$M_SENSE_BUSY	0x200
#define		FLG$M_OOB_CHAR		0x400
#define		FLG$M_FLUSH_OUTPUT	0x800
#define		FLG$M_CLR_NOBRDCST	0x1000
#define		FLG$M_READ_ABORT	0x2000
#define		FLG$M_READ_ABORTED	0x4000
#define		FLG$M_ISUPPORT		0x8000
#define		FLG$M_PARTIAL_READ	0x10000

struct _rtt_ucb
{
    struct _ltrm_ucb ucb$r_ltrmucb;
    struct
    {
        struct _ucb *ucb$l_rtt_netucb;
        struct _wcb *ucb$l_rtt_netwind;
        struct _irp *ucb$l_rtt_irpfl;
        struct _irp *ucb$l_rtt_irpbl;
        struct _irp *ucb$l_rtt_netirp;
        int ucb$l_rtt_bandincl;
        unsigned int ucb$l_rtt_bandinmsk;
        unsigned int ucb$l_rtt_bandexcl;
        unsigned int ucb$l_rtt_bandexmsk;
        unsigned char ucb$b_rtt_provrs;
        unsigned char ucb$b_rtt_proeco;
        unsigned short int ucb$w_rtt_link;
        unsigned char ucb$b_rtt_obj;
        unsigned short int ucb$w_rtt_systype;
        unsigned char ucb$b_rtt_fillbyte;
        union
        {
            unsigned int ucb$l_ct_flags;
            struct
            {
                unsigned flg$v_ctrlo		: 1;
                unsigned flg$v_canctrlo		: 1;
                unsigned flg$v_vaxtovax		: 1;
                unsigned flg$v_ctrlc		: 1;
                unsigned flg$v_init		: 1;
                unsigned flg$v_reset_timer	: 1;
                unsigned flg$v_decnet_busy	: 1;
                unsigned flg$v_output_busy	: 1;
                unsigned flg$v_read_busy	: 1;
                unsigned flg$v_sense_busy	: 1;
                unsigned flg$v_oob_char		: 1;
                unsigned flg$v_flush_output	: 1;
                unsigned flg$v_clr_nobrdcst	: 1;
                unsigned flg$v_read_abort	: 1;
                unsigned flg$v_read_aborted	: 1;
                unsigned flg$v_isupport		: 1;
                unsigned flg$v_partial_read	: 1;
                unsigned flg$v_fill_5_		: 7;
            };
        };
        struct _irp *ucb$l_ct_wiirp;
        struct _tqe *ucb$l_ct_tqe;
        struct _dcb *ucb$l_ct_netqfl;
        struct _dcb *ucb$l_ct_netqbl;
        struct _irp *ucb$l_ct_senseqfl;
        struct _irp *ucb$l_ct_senseqbl;
        struct _irp *ucb$l_ct_readqfl;
        struct _irp *ucb$l_ct_readqbl;
        struct _dcb *ucb$l_ct_wrtdcb;
        struct _dcb *ucb$l_ct_curdcb;
        unsigned short int ucb$w_ct_remsiz;
        short int ucb$w_ct_qdcbcnt;
        unsigned short int ucb$w_ct_maxmsg;
        unsigned short int ucb$w_ct_maxread;
        unsigned int ucb$l_ct_legalmsg;
        unsigned char ucb$b_ct_version;
        unsigned char ucb$b_ct_eco;
        unsigned short int ucb$w_ct_speed;
        unsigned char ucb$b_ct_crfill;
        unsigned char ucb$b_ct_lffill;
        unsigned short int ucb$w_ct_parity;
        unsigned int ucb$l_ct_include;
        unsigned int ucb$l_ct_exclude;
        unsigned int ucb$l_ct_abort;
        unsigned char ucb$b_ct_oob_char;
        unsigned char ucb$b_ct_fill_byte;
        unsigned short int ucb$w_ct_prtctl;
        unsigned long long ucb$q_ct_isupport;
        unsigned int ucb$l_ct_fill_longword;
    };
};

#define UCB$C_RTT_LENGTH 424
#define UCB$K_RTT_LENGTH 424

#endif

