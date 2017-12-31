#ifndef chpctldef_h
#define chpctldef_h

#define CHPCTL$M_READ 0x1
#define CHPCTL$M_WRITE 0x2
#define CHPCTL$M_USEREADALL 0x4
#define CHPCTL$M_NOAUDIT 0x8
#define CHPCTL$M_NOFAILAUD 0x10
#define CHPCTL$M_NOSUCCAUD 0x20
#define CHPCTL$M_DELETE 0x40
#define CHPCTL$M_MANDATORY 0x80
#define CHPCTL$M_FLUSH 0x100
#define CHPCTL$M_CREATE 0x200
#define CHPCTL$M_INTERNAL 0x400
#define CHPCTL$M_SERVER 0x800
#define CHPCTL$K_LENGTH 32
#define CHPCTL$C_LENGTH 32

struct _chpctl
{
    unsigned int chpctl$l_access;
    union
    {
        unsigned int chpctl$l_flags;
        struct
        {
            unsigned chpctl$v_read : 1;
            unsigned chpctl$v_write : 1;
            unsigned chpctl$v_usereadall : 1;
            unsigned chpctl$v_noaudit : 1;
            unsigned chpctl$v_nofailaud : 1;
            unsigned chpctl$v_nosuccaud : 1;
            unsigned chpctl$v_delete : 1;
            unsigned chpctl$v_mandatory : 1;
            unsigned chpctl$v_flush : 1;
            unsigned chpctl$v_create : 1;
            unsigned chpctl$v_internal : 1;
            unsigned chpctl$v_server : 1;
            unsigned chpctl$v_fill_2_ : 4;
        };
    };
    unsigned int chpctl$l_mode;
    void *chpctl$l_audit_list;
    int chpctl$l_deaccess_key;
    unsigned int chpctl$l_message;
    union
    {
        struct _arb *chpctl$l_arb;
        struct _psb *chpctl$l_psb;
    };
    struct _orb *chpctl$l_orb;
};

#endif

