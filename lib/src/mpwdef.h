#ifndef mpwdef_h
#define mpwdef_h

#define		MPW$C_MAINTAIN		1
#define		MPW$C_SVAPTE		2
#define		MPW$C_OPCCRASH		3
#define		MPW$C_PAGE_TABLE	4
#define		MPW$C_IDLE		0
#define		MPW$C_MAINT_STATE	1
#define		MPW$C_SELECTIVE		2
#define		MPW$C_CRASH_STATE	3
#define		MPW$C_MAXSTATE		4
#define		MPW$C_DPTSCAN		0
#define		MPW$C_DGBLSC		1
#define		MPW$C_RELPHD		2
#define		MPW$C_FREELIM		3
#define		MPW$C_MPWCHECK		4
#define		MPW$C_CRASH		5
#define		MPW$C_MAXID		6
#define		MPW$M_RCODE		0xFF
#define		MPW$M_RMODIFIERS	0xFF00
#define		MPW$M_RESERVED		0xFF0000
#define		MPW$M_IDCODE		0xFF000000
#define		MPW$M_LOLIMIT		0x100
#define		MPW$S_MPWDEF		4

struct _mpw
{
    union
    {
        struct
        {
            unsigned mpw$v_rcode	: 8;
            unsigned mpw$v_rmodifiers	: 8;
            unsigned mpw$v_reserved	: 8;
            unsigned mpw$v_idcode	: 8;
        };
        struct
        {
            unsigned mpw$v_rcode_fill	: 8;
            unsigned mpw$v_lolimit	: 1;
            unsigned mpw$v_fill_0_	: 7;
        };
    };
};

#endif

