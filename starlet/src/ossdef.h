#ifndef ossdef_h
#define ossdef_h
 
#define OSS$M_WLOCK 0x1
#define OSS$M_RELCTX 0x2
#define OSS$M_LOCAL 0x4
	
struct _ossdef {
  struct  {
    unsigned oss$v_wlock		: 1;
    unsigned oss$v_relctx		: 1;
    unsigned oss$v_local		: 1;
    unsigned oss$v_fill_47_		: 5;
  };
};
 
#define		OSS$_ACCESS_NAMES		1
#define		OSS$_ACCESS_NAMES_LENGTH	2
#define		OSS$_ACL_ADD_ENTRY		3
#define		OSS$_ACL_DELETE_ENTRY		4
#define		OSS$_ACL_DELETE			5
#define		OSS$_ACL_DELETE_ALL		6
#define		OSS$_ACL_FIND_ENTRY		7
#define		OSS$_ACL_FIND_NEXT		8
#define		OSS$_ACL_FIND_TYPE		9
#define		OSS$_ACL_GRANT_ACE		10
#define		OSS$_ACL_LENGTH			11
#define		OSS$_ACL_MODIFY_ENTRY		12
#define		OSS$_ACL_POSITION		13
#define		OSS$_ACL_POSITION_TOP		14
#define		OSS$_ACL_POSITION_BOTTOM	15
#define		OSS$_ACL_READ_ENTRY		16
#define		OSS$_ACL_READ			17
#define		OSS$_MAX_CLASS			18
#define		OSS$_MIN_CLASS			19
#define		OSS$_NEXT_OBJECT		20
#define		OSS$_OWNER			21
#define		OSS$_PROTECTION			22
#define		OSS$_SYS_PROT			23
#define		OSS$_OWN_PROT			24
#define		OSS$_GRP_PROT			25
#define		OSS$_WOR_PROT			26
#define		OSS$_CLASS_NAME			27
#define		OSS$_FIRST_TEMPLATE		28
#define		OSS$_NEXT_TEMPLATE		29
#define		OSS$_OBJECT_NAME		30
#define		OSS$_ACCESS_CLASS_NAME		31
#define		OSS$_DAMAGED_ACL		32
#define		OSS$_IN_TRANSITION		33
#define		OSS$_TRANQUILITY_COUNT		34
#define		OSS$_ACCESS_BITNAMES		1
#define		OSS$_ACCESS_BITNAMES_LENGTH	2
#define		OSS$_ACL_ADDACE			3
#define		OSS$_ACL_DELACE			4
#define		OSS$_ACL_FNDACE			7
#define		OSS$_ACL_FNDNXT			8
#define		OSS$_ACL_FNDTYP			9
#define		OSS$_ACL_MODACE			12
#define		OSS$_ACL_READACE		16
	
#endif
