#ifndef fscndef_h
#define fscndef_h

#define		FSCN$M_NODE		0x1
#define		FSCN$M_DEVICE		0x2
#define		FSCN$M_ROOT		0x4
#define		FSCN$M_DIRECTORY	0x8
#define		FSCN$M_NAME		0x10
#define		FSCN$M_TYPE		0x20
#define		FSCN$M_VERSION		0x40
#define		FSCN$M_NODE_PRIMARY	0x80
#define		FSCN$M_NODE_ACS		0x100
#define		FSCN$M_NODE_SECONDARY	0x200
	
struct _fldflags {
  unsigned fscn$v_node			: 1;
  unsigned fscn$v_device		: 1;
  unsigned fscn$v_root			: 1;
  unsigned fscn$v_directory		: 1;
  unsigned fscn$v_name			: 1;
  unsigned fscn$v_type			: 1;
  unsigned fscn$v_version		: 1;
  unsigned fscn$v_node_primary		: 1;
  unsigned fscn$v_node_acs		: 1;
  unsigned fscn$v_node_secondary	: 1;
  unsigned fscndef$$_fill_1		: 22;
};

#define		FSCN$_FILESPEC		1
#define		FSCN$_NODE		2
#define		FSCN$_DEVICE		3
#define		FSCN$_ROOT		4
#define		FSCN$_DIRECTORY		5
#define		FSCN$_NAME		6
#define		FSCN$_TYPE		7
#define		FSCN$_VERSION		8
#define		FSCN$_NODE_PRIMARY	9
#define		FSCN$_NODE_ACS		10
#define		FSCN$_NODE_SECONDARY	11
#define		FSCN$S_ITEM_LEN		8
	
struct _fscndef {
  unsigned short int fscn$w_length;
  unsigned short int fscn$w_item_code;
  void *fscn$l_addr;
};
 
#endif

