#ifndef stsdef_h
#define stsdef_h

#define STS$M_SEVERITY 0x7
#define STS$M_COND_ID 0xFFFFFF8
#define STS$M_CONTROL 0xF0000000
#define STS$M_SUCCESS 0x1
#define STS$M_MSG_NO 0xFFF8
#define STS$M_CODE 0x7FF8
#define STS$M_FAC_SP 0x8000
#define STS$M_CUST_DEF 0x8000000
#define STS$M_INHIB_MSG 0x10000000
#define STS$M_FAC_NO 0xFFF0000
#define STS$K_WARNING 0                 
#define STS$K_SUCCESS 1                 
#define STS$K_ERROR 2                   
#define STS$K_INFO 3                    
#define STS$K_SEVERE 4                  
	
union _stsdef {
  struct  {
    unsigned sts$v_severity : 3;    
    unsigned sts$v_cond_id : 25;    
    unsigned sts$v_control : 4;     
  };
  struct  {
    unsigned sts$v_success : 1;     
    unsigned stsdef$$_fill_1 : 2;   
    unsigned sts$v_msg_no : 13;     
  };
  struct  {
    unsigned stsdef$$_fill_2 : 3;   
    unsigned sts$v_code : 12;       
    unsigned sts$v_fac_sp : 1;      
    unsigned stsdef$$_fill_3 : 11;
    unsigned sts$v_cust_def : 1;    
    unsigned sts$v_inhib_msg : 1;   
    unsigned stsdef$$_fill_4 : 3;   
  };
  struct  {
    unsigned stsdef$$_fill_5 : 16;  
    unsigned sts$v_fac_no : 12;     
    unsigned stsdef$$_fill_6 : 4;   
  };
};
 
#define STS$S_CODE       0x0C
#define STS$S_COND_ID    0x19
#define STS$S_CONTROL    0x04
#define STS$S_FAC_NO     0x0C
#define STS$S_MSG_NO     0x0D
#define STS$S_SEVERITY   0x03
#define STS$V_CODE       0x03
#define STS$V_COND_ID    0x03
#define STS$V_CONTROL    0x1C
#define STS$V_CUST_DEF   0x1B
#define STS$V_FAC_NO     0x10
#define STS$V_FAC_SP     0x0F
#define STS$V_INHIB_MSG  0x1C
#define STS$V_MSG_NO     0x03
#define STS$V_SEVERITY   0x00
#define STS$V_SUCCESS    0x00

#define $VMS_STATUS_CODE(code) 		( ( (code) & STS$M_CODE ) 	>> STS$V_CODE )
#define $VMS_STATUS_COND_ID(code) 	( ( (code) & STS$M_COND_ID ) 	>> STS$V_COND_ID )
#define $VMS_STATUS_CONTROL(code) 	( ( (code) & STS$M_CONTROL ) 	>> STS$V_CONTROL )
#define $VMS_STATUS_CUST_DEF(code) 	( ( (code) & STS$M_CUST_DEF ) 	>> STS$V_CUST_DEF )
#define $VMS_STATUS_FAC_NO(code) 	( ( (code) & STS$M_FAC_NO ) 	>> STS$V_FAC_NO )
#define $VMS_STATUS_FAC_SP(code) 	( ( (code) & STS$M_FAC_SP ) 	>> STS$V_FAC_SP )
#define $VMS_STATUS_INHIB_MSG(code)	( ( (code) & STS$M_INHIB_MSG ) 	>> STS$V_INHIB_MSG )
#define $VMS_STATUS_MSG_NO(code) 	( ( (code) & STS$M_MSG_NO ) 	>> STS$V_MSG_NO )
#define $VMS_STATUS_SEVERITY(code) 	( ( (code) & STS$M_SEVERITY ) 	>> STS$V_SEVERITY )
#define $VMS_STATUS_SUCCESS(code) 	( ( (code) & STS$M_SUCCESS ) 	>> STS$V_SUCCESS )
 
#endif 
 
