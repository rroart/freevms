#ifndef HRBDEF_H
#define HRBDEF_H

#include <vms_types.h>

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
    UINT16 hrb$w_size;
    UINT8 hrb$b_type;
    UINT8 hrb$b_subtype;
    INT32 (*hrb$l_respc)(void);
    INT32 (*hrb$l_savd_rtn)(void);
    union
    {
        UINT16 hrb$w_state;
        struct
        {
            unsigned hrb$v_filler : 15;
            unsigned hrb$v_state_invalid : 1;
        };
    } ;

    union
    {
        UINT16 hrb$w_flags;
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
    UINT8 hrb$b_lbuff [12];
    void *hrb$l_bd_addr;
    UINT32 hrb$l_buflen;
    void *hrb$l_bufadr;
    UINT32 hrb$l_lbn;
    UINT32 hrb$l_obcnt;
    UINT32 hrb$l_abcnt;
    UINT32 hrb$l_svapte;
    UINT32 hrb$l_bcnt;
    UINT16 hrb$w_boff;
    UINT16 hrb$w_reserved;
    void *hrb$l_wait_fl;
    void *hrb$l_wait_bl;
    struct _hqb *hrb$l_hqb;
    struct _uqb *hrb$l_uqb;
    struct _pdt *hrb$l_pdt;
    UINT32 hrb$l_cmd_sts;
    UINT32 hrb$l_object_skip;
    UINT32 hrb$l_current_skip;
    UINT32 hrb$l_io_time;

    void *hrb$l_cache_fl;
    void *hrb$l_cache_bl;
    void *hrb$l_memw_fl;
    void *hrb$l_memw_bl;
    UINT32 hrb$l_record;
    UINT32 hrb$l_cmd_time;
};


#endif

