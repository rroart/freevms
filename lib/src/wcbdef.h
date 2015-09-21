#ifndef WCBDEF_H
#define WCBDEF_H

#include <vms_types.h>

#define     WCB$M_READ      0x1
#define     WCB$M_WRITE     0x2
#define     WCB$M_NOTFCP        0x4
#define     WCB$M_SHRWCB        0x8
#define     WCB$M_OVERDRAWN     0x10
#define     WCB$M_COMPLETE      0x20
#define     WCB$M_CATHEDRAL     0x40
#define     WCB$M_EXPIRE        0x80
#define     WCB$M_CONTROL       0x1000
#define     WCB$M_NO_READ_DATA  0x2000
#define     WCB$K_MAP       56
#define     WCB$C_MAP       56
#define     WCB$K_LENGTH        56
#define     WCB$C_LENGTH        56
#define     WCB$S_WCBDEF        80

struct _wcb
{
    struct _wcb *wcb$l_wlfl;
    struct _wcb *wcb$l_wlbl;
    UINT16 wcb$w_size;
    UINT8 wcb$b_type;
    union
    {
        UINT8 wcb$b_access;
        struct
        {
            unsigned wcb$v_read : 1;
            unsigned wcb$v_write : 1;
            unsigned wcb$v_notfcp : 1;
            unsigned wcb$v_shrwcb : 1;
            unsigned wcb$v_overdrawn : 1;
            unsigned wcb$v_complete : 1;
            unsigned wcb$v_cathedral : 1;
            unsigned wcb$v_expire : 1;
        };
    };
    UINT32 wcb$l_pid;
    UINT32 wcb$l_refcnt;
    struct _ucb *wcb$l_orgucb;
    union
    {
        UINT16 wcb$w_acon;
        struct
        {
            unsigned wcb$v_nowrite : 1;
            unsigned wcb$v_dlock : 1;
            unsigned wcb$$_fill_1 : 2;
            unsigned wcb$v_spool : 1;
            unsigned wcb$v_writeck : 1;
            unsigned wcb$v_seqonly : 1;
            unsigned wcb$v_snapshot : 1;
            unsigned wcb$v_writeac : 1;
            unsigned wcb$v_readck : 1;
            unsigned wcb$v_noread : 1;
            unsigned wcb$v_notrunc : 1;
            unsigned wcb$v_control : 1;
            unsigned wcb$v_no_read_data : 1;
            unsigned wcb$v_fill_2 : 2;
        };
        struct
        {
            unsigned wcb$$_fill_3 : 2;
            unsigned wcb$v_noacclock : 1;
            unsigned wcb$$_fill_4 : 11;
            unsigned wcb$v_readinit : 1;
            unsigned wcb$v_write_turn : 1;
        };
    };
    UINT16 wcb$w_nmap;
    struct _fcb *wcb$l_fcb;
    struct _rvt *wcb$l_rvt;
    void *wcb$l_link;
    UINT32 wcb$l_reads;
    UINT32 wcb$l_writes;
    UINT32 wcb$l_stvbn;
    union
    {
        struct
        {
            UINT16 wcb$w_p1_count;
            INT8 wcb$b_fill_8 [10];
        };
        struct
        {
            UINT32 wcb$l_p1_count;
            UINT32 wcb$l_p1_lbn;
            UINT32 wcb$l_p1_rvn;
        };
        struct
        {
            UINT64 wcb$q_deliq;
            INT8 wcb$b_fill_7 [4];
        };
    };
    UINT32 wcb$l_p2_count;
    UINT32 wcb$l_p2_lbn;
    UINT32 wcb$l_p2_rvn;

    // borrowing until I understand wcb better

#if 0
    unsigned hd_seg_num;        /* Header segment number */
    unsigned short extcount;    /* Extents in use */
    unsigned phylen[20];
    unsigned phyblk[20];
    unsigned char rvn[20];
    unsigned long loblk,hiblk;
    unsigned hd_basevbn;        /* File blocks prior to header */
#endif
};

#define WCB$K_MAP_PTR_LENGTH 12
#define WCB$C_MAP_PTR_LENGTH 12
#define WCB$S_WCBDEF1 12

struct _wcb1
{
    UINT32 wcb$l_count;
    UINT32 wcb$l_lbn;
    UINT32 wcb$l_rvn;
};

#define WCB$S_WCBDEF2 13

struct _wcb2
{
    INT8 wcb$$_fill_6;
};

#endif

