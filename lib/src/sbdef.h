#ifndef sbdef_h
#define sbdef_h
	
#define SB$M_LOCAL 0x1
#define SB$M_LOCAL_DIRECTORY 0x2
	
#define SB$K_LENGTH 120
#define SB$C_LENGTH 120
#define SB$S_SBDEF 120
 
struct _sb {
  void *sb$l_flink;
  void *sb$l_blink;
  unsigned short int sb$w_size;
  unsigned char sb$b_type;
  unsigned char sb$b_subtyp;
  struct _pb *sb$l_pbfl;
  struct _pb *sb$l_pbbl;
  struct _pb *sb$l_pbconnx;

  int sb$$_fill_2;
  unsigned char sb$b_systemid [6];
  short int sb$$_fill_1;
  unsigned short int sb$w_maxdg;
  unsigned short int sb$w_maxmsg;
  char sb$t_swtype [4];
  char sb$t_swvers [4];
  unsigned long long sb$q_swincarn;
  char sb$t_hwtype [4];
  unsigned char sb$b_hwvers [12];
  char sb$t_nodename [16];
  struct _ddb *sb$l_ddb;
  short int sb$w_timeout;
  unsigned char sb$b_enbmsk [2];
  struct _csb *sb$l_csb;
  unsigned int sb$l_port_map;
  unsigned int sb$l_status;
  void *sb$ps_proc_names;
  unsigned int sb$l_mount_lkid;
  void *sb$ps_allocls_list;
};
 
#endif
 
