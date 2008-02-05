#ifndef acldef_h
#define acldef_h

#define		ACL$K_LENGTH			12
#define		ACL$C_LENGTH			12
#define		ACL$C_FILE			1
#define		ACL$C_DEVICE			2
#define		ACL$C_JOBCTL_QUEUE		3
#define		ACL$C_COMMON_EF_CLUSTER		4
#define		ACL$C_LOGICAL_NAME_TABLE	5
#define		ACL$C_PROCESS			6
#define		ACL$C_GROUP_GLOBAL_SECTION	7
#define		ACL$C_SYSTEM_GLOBAL_SECTION	8
#define		ACL$C_CAPABILITY		9
#define		ACL$C_EVENT_FACILITY		10
#define		ACL$C_LOCK			11
#define		ACL$C_VOLUME			12
#define		ACL$C_MAX_OBJECT		13
#define		ACL$C_NUM_OBJECTS		12
#define		ACL$K_NUM_OBJECTS		12

#define		ACL$C_ADDACLENT		1
#define		ACL$C_DELACLENT		2
#define		ACL$C_MODACLENT		3
#define		ACL$C_FNDACLENT		4
#define		ACL$C_FNDACETYP		5
#define		ACL$C_DELETEACL		6
#define		ACL$C_READACL		7
#define		ACL$C_ACLLENGTH		8
#define		ACL$C_READACE		9
#define		ACL$C_RLOCK_ACL		10
#define		ACL$C_WLOCK_ACL		11
#define		ACL$C_UNLOCK_ACL	12
#define		ACL$C_GRANT_ACE		13
#define		ACL$C_NEXT_ACE		14
#define		ACL$C_DELETE_ALL	15
#define		ACL$C_RESERVED_ITEM_2	16
#define		ACL$C_RESERVED_ITEM_3	17
#define		ACL$S_ADDACLENT		255
#define		ACL$S_DELACLENT		255
#define		ACL$S_MODACLENT		255
#define		ACL$S_FNDACLENT		255
#define		ACL$S_FNDACETYP		255
#define		ACL$S_DELETEACL		255
#define		ACL$S_READACL		512
#define		ACL$S_ACLLENGTH		4
#define		ACL$S_READACE		255
#define		ACL$S_RLOCK_ACL		4
#define		ACL$S_WLOCK_ACL		4
#define		ACL$S_UNLOCK_ACL	4
#define		ACL$S_GRANT_ACE		255
#define		ACL$S_NEXT_ACE		4
#define		ACL$S_DELETE_ALL	255
#define		ACL$S_RESERVED_ITEM_2	255
#define		ACL$S_RESERVED_ITEM_3	255
#define		ACL$C_MAX_SEGMENT_SIZE	512
#define		ACL$K_MAX_SEGMENT_SIZE	512
	
struct _acldef {
  unsigned int acl$l_flink;
  unsigned int acl$l_blink;
  unsigned short int acl$w_size;
  unsigned char acl$b_type;
  char acldef$$_fill_1;
  unsigned int acl$l_list;
};
 
#endif
 
