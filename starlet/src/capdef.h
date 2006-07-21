#ifndef capdef_h
#define capdef_h

#define		CAP$K_ALL_USER		-1
#define		CAP$K_ALL_USER_ADD	-1
#define		CAP$K_ALL_CPU_ADD	-1
#define		CAP$K_ALL_ACTIVE_CPUS	-1
#define		CAP$K_ALL_USER_REMOVE	0
#define		CAP$K_ALL_CPU_REMOVE	0
#define		CAP$K_GET_FREE_CAP	0

#define		CAP$M_USER1		0x1
#define		CAP$M_USER2		0x2
#define		CAP$M_USER3		0x4
#define		CAP$M_USER4		0x8
#define		CAP$M_USER5		0x10
#define		CAP$M_USER6		0x20
#define		CAP$M_USER7		0x40
#define		CAP$M_USER8		0x80
#define		CAP$M_USER9		0x100
#define		CAP$M_USER10		0x200
#define		CAP$M_USER11		0x400
#define		CAP$M_USER12		0x800
#define		CAP$M_USER13		0x1000
#define		CAP$M_USER14		0x2000
#define		CAP$M_USER15		0x4000
#define		CAP$M_USER16		0x8000

struct _cap {
  union  {
    unsigned int cap$l_cap;
    struct  {
      unsigned cap$v_user1		: 1;
      unsigned cap$v_user2		: 1;
      unsigned cap$v_user3		: 1;
      unsigned cap$v_user4		: 1;
      unsigned cap$v_user5		: 1;
      unsigned cap$v_user6		: 1;
      unsigned cap$v_user7		: 1;
      unsigned cap$v_user8		: 1;
      unsigned cap$v_user9		: 1;
      unsigned cap$v_user10		: 1;
      unsigned cap$v_user11		: 1;
      unsigned cap$v_user12		: 1;
      unsigned cap$v_user13		: 1;
      unsigned cap$v_user14		: 1;
      unsigned cap$v_user15		: 1;
      unsigned cap$v_user16		: 1;
      unsigned cap$v_fill_1		: 16;
    };
  };
};
 
#define		CAP$M_CPU0		0x1
#define		CAP$M_CPU1		0x2
#define		CAP$M_CPU2		0x4
#define		CAP$M_CPU3		0x8
#define		CAP$M_CPU4		0x10
#define		CAP$M_CPU5		0x20
#define		CAP$M_CPU6		0x40
#define		CAP$M_CPU7		0x80
#define		CAP$M_CPU8		0x100
#define		CAP$M_CPU9		0x200
#define		CAP$M_CPU10		0x400
#define		CAP$M_CPU11		0x800
#define		CAP$M_CPU12		0x1000
#define		CAP$M_CPU13		0x2000
#define		CAP$M_CPU14		0x4000
#define		CAP$M_CPU15		0x8000
#define		CAP$M_CPU16		0x10000
#define		CAP$M_CPU17		0x20000
#define		CAP$M_CPU18		0x40000
#define		CAP$M_CPU19		0x80000
#define		CAP$M_CPU20		0x100000
#define		CAP$M_CPU21		0x200000
#define		CAP$M_CPU22		0x400000
#define		CAP$M_CPU23		0x800000
#define		CAP$M_CPU24		0x1000000
#define		CAP$M_CPU25		0x2000000
#define		CAP$M_CPU26		0x4000000
#define		CAP$M_CPU27		0x8000000
#define		CAP$M_CPU28		0x10000000
#define		CAP$M_CPU29		0x20000000
#define		CAP$M_CPU30		0x40000000
#define		CAP$M_CPU31		0x80000000
	
struct _cap_cpus {
  union  {
    unsigned int cap$l_cap_cpus;
    struct  {
      unsigned cap$v_cpu0		: 1;
      unsigned cap$v_cpu1		: 1;
      unsigned cap$v_cpu2		: 1;
      unsigned cap$v_cpu3		: 1;
      unsigned cap$v_cpu4		: 1;
      unsigned cap$v_cpu5		: 1;
      unsigned cap$v_cpu6		: 1;
      unsigned cap$v_cpu7		: 1;
      unsigned cap$v_cpu8		: 1;
      unsigned cap$v_cpu9		: 1;
      unsigned cap$v_cpu10		: 1;
      unsigned cap$v_cpu11		: 1;
      unsigned cap$v_cpu12		: 1;
      unsigned cap$v_cpu13		: 1;
      unsigned cap$v_cpu14		: 1;
      unsigned cap$v_cpu15		: 1;
      unsigned cap$v_cpu16		: 1;
      unsigned cap$v_cpu17		: 1;
      unsigned cap$v_cpu18		: 1;
      unsigned cap$v_cpu19		: 1;
      unsigned cap$v_cpu20		: 1;
      unsigned cap$v_cpu21		: 1;
      unsigned cap$v_cpu22		: 1;
      unsigned cap$v_cpu23		: 1;
      unsigned cap$v_cpu24		: 1;
      unsigned cap$v_cpu25		: 1;
      unsigned cap$v_cpu26		: 1;
      unsigned cap$v_cpu27		: 1;
      unsigned cap$v_cpu28		: 1;
      unsigned cap$v_cpu29		: 1;
      unsigned cap$v_cpu30		: 1;
      unsigned cap$v_cpu31		: 1;
    };
  };
};
 
#define		CAP$M_IMPLICIT_AFFINITY_CLEAR	0x1
#define		CAP$M_IMPLICIT_AFFINITY_SET	0x2
#define		CAP$M_IMPLICIT_DEFAULT_ONLY	0x4
#define		CAP$M_IMPLICIT_ALL_THREADS	0x8
	
struct _imp_flags {
  struct  {
    unsigned cap$v_implicit_affinity_clear	: 1;
    unsigned cap$v_implicit_affinity_set	: 1;
    unsigned cap$v_implicit_default_only	: 1;
    unsigned cap$v_implicit_all_threads		: 1;
    unsigned cap$v_fill_62_			: 4;
  };
};

#define		CAP$M_FLAG_CHECK_CPU		0x1
#define		CAP$M_FLAG_PERMANENT		0x2
#define		CAP$M_FLAG_CHECK_CPU_ACTIVE	0x8
#define		CAP$M_FLAG_DEFAULT_ONLY		0x10
#define		CAP$M_ALL_THREADS		0x20
#define		CAP$M_PURGE_WS_IF_NEW_RAD	0x40
	
struct _cap_flags {
  struct  {
    unsigned cap$v_flag_check_cpu	: 1;
    unsigned cap$v_flag_permanent	: 1;
    unsigned cap$v_reserved1		: 1;
    unsigned cap$v_flag_check_cpu_active: 1;
    unsigned cap$v_flag_default_only	: 1;
    unsigned cap$v_all_threads		: 1;
    unsigned cap$v_purge_ws_if_new_rad	: 1;
    unsigned cap$v_fill_63_		: 1;
  };
};
 
#endif

