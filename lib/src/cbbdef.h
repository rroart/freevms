#ifndef cbbdef_h
#define cbbdef_h

#define CBB$M_LOCK_BIT 0x1
#define CBB$M_AUTO_LOCK 0x2
#define CBB$M_TIMEOUT_CRASH 0x4
#define CBB$M_SUMMARY_BITS 0x8
#define CBB$M_SET_COUNT 0x10
	
#define CBB$K_LENGTH 48                 
#define CBB$C_LENGTH 48                 

#define CBB$C_OR 0                      
#define CBB$C_AND 1                     
#define CBB$C_XOR 2                     
#define CBB$C_BIC 3                     
#define CBB$C_ORNOT 4                   
#define CBB$C_EQV 5                     
#define CBB$C_MAX_FUNCTION 5
	
#define CBB$M_OVERRIDE_LOCK 1		
 
struct _cbb {
  unsigned int cbb$l_data_offset;     
  unsigned short int cbb$w_unit_count; 
  unsigned char cbb$b_unit_size;      
  unsigned char cbb$b_lock_ipl;       
  unsigned short int cbb$w_size;      
  unsigned char cbb$b_type;           
  unsigned char cbb$b_subtype;        
  unsigned int cbb$l_bit_count;       
  union  {
    unsigned long long cbb$q_interlock; 
    union  {
      unsigned long long cbb$q_state; 
      struct  {
	unsigned cbb$v_lock_bit : 1; 
	unsigned cbb$v_auto_lock : 1; 
	unsigned cbb$v_timeout_crash : 1; 
	unsigned cbb$v_summary_bits : 1; 
	unsigned cbb$v_set_count : 1; 
	unsigned cbb$v_fill_2_ : 3;
      };
    };
  };
  unsigned int cbb$l_timeout_count;   
  unsigned int cbb$l_saved_ipl;       
  unsigned int cbb$l_valid_bits;      
  char cbb$b_cbb_padding_0 [4];       
  unsigned long long cbb$q_summary_bitmask; 
};
 
typedef struct _cbb *  CBB_PQ;	      
typedef struct _cbb ** CBB_PPQ;	      

#endif
 
