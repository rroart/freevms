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
	
#define	ACQUIRE_SCH$QAST	0x0ac00000
#define	ACQUIRE_KERNEL_AST	0x0aa00000
#define	ACQUIRE_EXEC_AST	0x0aa10000
#define	ACQUIRE_SPKAST	        0x0aa20000
#define	ACQUIRE_PIGGYBACK	0x0aa30000
#define	__RELEASE_KERNEL_AST	0x0af00000
#define	__RELEASE_EXEC_AST	0x0af10000
#define	__RELEASE_SPKAST	0x0af20000
#define	__RELEASE_PIGGYBACK	0x0aa30000
#define	__RELEASE_AST_ERROR	0x0aee0000
#define	__RELEASE_ASTFAULT	0x0aef0000
	
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
 
