#ifndef wwiddef_h
#define wwiddef_h

#define WWIDHDR$M_WWID_LENGTH 0xFFFFF
#define WWIDHDR$M_ASCII_DATA 0x100000
#define WWIDHDR$M_WWID_TYPE 0xFF000000
#define WWIDHDR$K_IEEE_REGEXT 1         
#define WWIDHDR$K_IEEE_REG 2            
#define WWIDHDR$K_PG83TY1_VID 3         
#define WWIDHDR$K_PG80_SERIALNO 4       
#define WWIDHDR$K_INQ_SERIALNO 5        
#define WWIDHDR$K_TGT_IDENT 6           
#define WWIDHDR$K_BTL 7                 
#define WWIDHDR$K_DEC_UNIQUE 8          
#define WWIDHDR$K_HSZ_THISOTHER 9       
#define WWIDHDR$K_IEEE48_VEXT 10        
#define WWIDHDR$K_IEEE48_0EXT 11        
#define WWIDHDR$K_IEEE_EUI64 12         
	
#define WWID$K_LENGTH 280
	
#define SN_WWID$K_LENGTH 280
#define WWID$K_MAX_LENGTH 280
	
#define WWIDENT$K_LENGTH 128
#define WWIDTBL$K_MAX_ENTRIES 1000      
	
struct _wwidhdr {
  struct  {
    unsigned wwidhdr$v_wwid_length : 20; 
    unsigned wwidhdr$v_ascii_data : 1; 
    unsigned wwidhdr$v_reserved1 : 3; 
    unsigned wwidhdr$v_wwid_type : 8; 
  };
};

struct _wwid {
  struct _wwidhdr wwid$r_header;
  char wwid$b_data [276];
};

struct _sn_wwid {
  struct _wwidhdr sn_wwid$r_header;
  unsigned char sn_wwid$b_vendor_id [8];
  unsigned char sn_wwid$b_product_id [16];
  unsigned char sn_wwid$b_serial_no [252];
};

struct _wwident {
  int wwident$l_unit_no;
  int wwident$l_dnp;
  unsigned int wwident$l_wwidlen;
  struct  {
    struct _wwidhdr wwident$r_header;
    char wwident$b_fcwwid [112];
  };
};
 
struct _fc_wwid_64b {
  unsigned char fc_wwidb_name [8];
};
 
#endif 
 
