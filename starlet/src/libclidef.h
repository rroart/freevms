#ifndef libclidef_h
#define libclidef_h

#define LIB$K_CLI_LOCAL_SYM 1
#define LIB$K_CLI_GLOBAL_SYM 2
#define LIB$M_CLI_CTRLT 0x100000
#define LIB$M_CLI_CTRLY 0x2000000
	
struct _cli$ctrl_fields {
  unsigned lib$v_fill_a_s : 20;       
  unsigned lib$v_cli_ctrlt : 1;
  unsigned lib$v_fill_u_x : 4;        
  unsigned lib$v_cli_ctrly : 1;
  unsigned lib$v_fill_0_ : 6;
};
 
#endif 
 
