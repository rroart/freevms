#ifndef acbdef_h
#define acbdef_h
 
#define	ACB$M_FLAGS_VALID	0x4
#define	ACB$M_POSIX_ACB	0x8
#define	ACB$M_PKAST	0x10
#define	ACB$M_NODELETE	0x20
#define	ACB$M_QUOTA	0x40
#define	ACB$M_KAST	0x80
#define	ACB$M_THREAD_SAFE	0x1
#define	ACB$M_THREAD_PID_VALID	0x2
#define	ACB$M_UPCALL	0x4
#define	ACB$M_FASTIO	0x8
#define	ACB$M_64BITS	0x10
#define	ACB$M_NOFLOAT	0x20
#define	ACB$M_KAST_NOFLOAT	0x40
#define	ACB$M_USER_THREAD_ID_VALID	0x80
#define	ACB$M_EXCLUSIVE	0x100
#define	ACB$M_TOLERANT	0x200

#define	ACB$K_LENGTH	36
#define	ACB$C_LENGTH	36
#define	ACB$S_ACBDEF	36
	
#define	ACQUIRE_SCH$QAST	180355072
#define	ACQUIRE_KERNEL_AST	178257920
#define	ACQUIRE_EXEC_AST	178323456
#define	ACQUIRE_SPKAST	178388992
#define	ACQUIRE_PIGGYBACK	178454528
#define	__RELEASE_KERNEL_AST	183500800
#define	__RELEASE_EXEC_AST	183566336
#define	__RELEASE_SPKAST	183631872
#define	__RELEASE_PIGGYBACK	178454528
#define	__RELEASE_AST_ERROR	183369728
#define	__RELEASE_ASTFAULT	183435264
	
struct _acb {
    struct _acb *acb$l_astqfl;          
    struct _acb *acb$l_astqbl;          
    unsigned short int acb$w_size;      
    unsigned char acb$b_type;           
    unsigned char acb$b_rmod;       
    unsigned long acb$l_pid;             
    void (*acb$l_ast)(unsigned long);            
    unsigned long acb$l_astprm;          
    void (*acb$l_kast)(void);               
    };
 
#endif 
 
