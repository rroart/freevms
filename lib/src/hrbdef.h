#ifndef HRBDEF_H
#define HRBDEF_H

#define HRB$M_STATE_INVALID 0x8000
#define HRB$M_ABORT 0x1
#define HRB$M_ABORTWS 0x2
#define HRB$M_DEQUEUED 0x4
#define HRB$M_ENDMSG 0x8
#define HRB$M_MAP 0x10
#define HRB$M_UNBLOCK 0x20
#define HRB$M_VCFAILED 0x40
#define HRB$M_OLDBUF 0x80
#define HRB$M_WBC_IMMED 0x100
#define HRB$M_FIRST 0x200
#define HRB$M_FLUSH 0x400
#define HRB$M_CMD_TMO 0x800
#define HRB$K_LENGTH 140

#define HRB$K_ST_MSG_WAIT 1
#define HRB$K_ST_SEQ_WAIT 2
#define HRB$K_ST_BUF_WAIT 3
#define HRB$K_ST_SNDAT_WAIT 4
#define HRB$K_ST_DRV_WAIT 5
#define HRB$K_ST_MAP_WAIT 6
#define HRB$K_ST_UNMAP_WAIT 7
#define HRB$K_ST_SNDMS_WAIT 8
#define HRB$K_ST_MEM_WAIT 9
#define HRB$K_ST_FLUSHED 10
#define HRB$K_ST_CACHED 11
#define HRB$S_HRBDEF 140

struct _hrb
{
    void *hrb$l_flink;
    void *hrb$l_blink;
    unsigned short int hrb$w_size;
    unsigned char hrb$b_type;
    unsigned char hrb$b_subtype;
    int (*hrb$l_respc)(void);
    int (*hrb$l_savd_rtn)(void);
    union
    {
        unsigned short int hrb$w_state;
        struct
        {
            unsigned hrb$v_filler : 15;
            unsigned hrb$v_state_invalid : 1;
        };
    } ;

    union
    {
        unsigned short int hrb$w_flags;
        struct
        {
            unsigned hrb$v_abort : 1;
            unsigned hrb$v_abortws : 1;
            unsigned hrb$v_dequeued : 1;
            unsigned hrb$v_endmsg : 1;
            unsigned hrb$v_map : 1;
            unsigned hrb$v_unblock : 1;
            unsigned hrb$v_vcfailed : 1;
            unsigned hrb$v_oldbuf : 1;

            unsigned hrb$v_wbc_immed : 1;
            unsigned hrb$v_first : 1;
            unsigned hrb$v_flush : 1;
            unsigned hrb$v_cmd_tmo : 1;
            unsigned hrb$v_fill_4_ : 4;
        };
    };
    struct _mscp *hrb$l_msgbuf;
    struct _cdrp *hrb$l_irp_cdrp;
    unsigned char hrb$b_lbuff [12];
    void *hrb$l_bd_addr;
    unsigned int hrb$l_buflen;
    void *hrb$l_bufadr;
    unsigned int hrb$l_lbn;
    unsigned int hrb$l_obcnt;
    unsigned int hrb$l_abcnt;
    unsigned int hrb$l_svapte;
    unsigned int hrb$l_bcnt;
    unsigned short int hrb$w_boff;
    unsigned short int hrb$w_reserved;
    void *hrb$l_wait_fl;
    void *hrb$l_wait_bl;
    struct _hqb *hrb$l_hqb;
    struct _uqb *hrb$l_uqb;
    struct _pdt *hrb$l_pdt;
    unsigned int hrb$l_cmd_sts;
    unsigned int hrb$l_object_skip;
    unsigned int hrb$l_current_skip;
    unsigned int hrb$l_io_time;

    void *hrb$l_cache_fl;
    void *hrb$l_cache_bl;
    void *hrb$l_memw_fl;
    void *hrb$l_memw_bl;
    unsigned int hrb$l_record;
    unsigned int hrb$l_cmd_time;
};


#endif

