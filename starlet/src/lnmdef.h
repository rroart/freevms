#ifndef LNMDEF_H
#define LNMDEF_H
 
#define LNM$M_NO_ALIAS           0x1
#define LNM$M_CONFINE            0x2
#define LNM$M_CRELOG             0x4
#define LNM$M_TABLE              0x8
#define LNM$M_CONCEALED        0x100
#define LNM$M_TERMINAL         0x200
#define LNM$M_EXISTS           0x400
#define LNM$M_SHAREABLE      0x10000
#define LNM$M_CLUSTERWIDE    0x20000
#define LNM$M_CREATE_IF    0x1000000
#define LNM$M_CASE_BLIND   0x2000000
#define LNM$M_INTERLOCKED  0x4000000
#define LNM$M_LOCAL_ACTION 0x8000000

#define LNM$C_TABNAMLEN   31              
#define LNM$C_NAMLENGTH   255             
#define LNM$C_MAXDEPTH    10               
#define LNM$_INDEX        1                    
#define LNM$_STRING       2                   
#define LNM$_ATTRIBUTES   3               
#define LNM$_TABLE        4                    
#define LNM$_LENGTH       5                   
#define LNM$_ACMODE       6                   
#define LNM$_MAX_INDEX    7                
#define LNM$_PARENT       8                   
#define LNM$_LNMB_ADDR    9                
#define LNM$_AGENT_ACMODE 10            
#define LNM$_CHAIN        -1                   
	
struct lnmdef {
  unsigned lnm$v_no_alias     : 1;    
  unsigned lnm$v_confine      : 1;         
  unsigned lnm$v_crelog       : 1;          
  unsigned lnm$v_table        : 1;           
  unsigned lnm$v_fill_0       : 4;          
  unsigned lnm$v_concealed    : 1;       
  unsigned lnm$v_terminal     : 1;        
  unsigned lnm$v_exists       : 1;          
  unsigned lnm$v_fill_1       : 5;          
  unsigned lnm$v_shareable    : 1;       
  unsigned lnm$v_clusterwide  : 1;     
  unsigned lnm$v_fill_2       : 2;          
  unsigned lnm$v_fill_3       : 4;          
  unsigned lnm$v_create_if    : 1;       
  unsigned lnm$v_case_blind   : 1;      
  unsigned lnm$v_interlocked  : 1;     
  unsigned lnm$v_local_action : 1;    
  unsigned lnm$v_fill_4       : 4;          
} ;

#endif
 
