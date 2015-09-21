#ifndef ORBDEF_H
#define ORBDEF_H

#include <vms_types.h>

#define ORB$M_PROT_16 0x1
#define ORB$M_ACL_QUEUE 0x2
#define ORB$M_MODE_VECTOR 0x4
#define ORB$M_NOACL 0x8
#define ORB$M_CLASS_PROT 0x10
#define ORB$M_NOAUDIT 0x20
#define ORB$M_MODE_VALID 0x80
#define ORB$M_PROFILE_LOCKED 0x100
#define ORB$M_INDIRECT_ACL 0x200
#define ORB$M_BOOTTIME 0x400
#define ORB$M_UNMODIFIED 0x800
#define ORB$M_DAMAGED 0x1000
#define ORB$M_TEMPLATE 0x2000
#define ORB$M_TRANSITION 0x4000
#define ORB$M_EXT_NAMEBLOCK 0x8000
#define ORB$K_LENGTH 124
#define ORB$C_LENGTH 124
#define ORB$K_DEVNAM_LENGTH 64
#define ORB$C_DEVNAM_LENGTH 64
#define ORB$S_ORBDEF 125

struct _orb
{
    union
    {
        UINT32 orb$l_owner;
        struct
        {
            UINT16 orb$w_uicmember;
            UINT16 orb$w_uicgroup;
        };
    };
    UINT32 orb$l_acl_mutex;
    UINT16 orb$w_size;
    UINT8 orb$b_type;
    UINT8 orb$b_subtype;
    union
    {
        union
        {
            UINT16 orb$w_flags;
            struct
            {
                UINT8 orb$b_flags_1;
                UINT8 orb$b_flags_2;
            };
        };
        UINT8 orb$b_flags;
        union
        {
            UINT16 orb$w_flag_bits;
            struct
            {
                unsigned orb$v_prot_16      : 1;
                unsigned orb$v_acl_queue    : 1;
                unsigned orb$v_mode_vector  : 1;
                unsigned orb$v_noacl        : 1;
                unsigned orb$v_class_prot   : 1;
                unsigned orb$v_noaudit      : 1;
                unsigned orb$v_reserved_1   : 1;
                unsigned orb$v_mode_valid   : 1;
                unsigned orb$v_profile_locked   : 1;
                unsigned orb$v_indirect_acl : 1;
                unsigned orb$v_boottime     : 1;
                unsigned orb$v_unmodified   : 1;
                unsigned orb$v_damaged      : 1;
                unsigned orb$v_template     : 1;
                unsigned orb$v_transition   : 1;
                unsigned orb$v_ext_nameblock    : 1;
            };
        };
    };
    UINT16 orb$w_refcount;
    union
    {
        union
        {
            UINT64 orb$q_mode_prot;
            struct
            {
                UINT32 orb$l_mode_protl;
                UINT32 orb$l_mode_proth;
            };
        };
        UINT32 orb$l_mode;
    };
    union
    {
        UINT32 orb$l_sys_prot;
        UINT16 orb$w_prot;
    };
    UINT32 orb$l_own_prot;
    UINT32 orb$l_grp_prot;
    UINT32 orb$l_wor_prot;
    union
    {
        struct _ace *orb$l_aclfl;
        UINT32 orb$l_acl_count;
    };
    union
    {
        struct _ace *orb$l_aclbl;
        void *orb$l_acl_desc;
    };
    struct
    {
        INT8 orb$b_fill_2 [20];
    };
    struct
    {
        INT8 orb$b_fill_3 [20];
    };
    UINT16 orb$w_name_length;
    INT16 orb$w_fill_3;
    void *orb$l_name_pointer;
    struct _ocb *orb$l_ocb;
    struct _orb *orb$l_template_orb;
    INT32 orb$l_object_specific;
    struct _orb *orb$l_original_orb;
    UINT32 orb$l_updseq;
    void *orb$l_mutex_address;
    INT32 orb$l_reserve2;
    char orb$t_object_name;
};

#endif

