#ifndef busarraydef_h
#define busarraydef_h

#define BUSARRAY$M_NO_RECONNECT 0x1
	
struct _busarrayentry {
  unsigned long long busarray$q_hw_id;  
  unsigned long long busarray$q_csr;    
  unsigned int busarray$l_node_number; 
  union  {
    unsigned int busarray$l_flags;  
    struct  {
      unsigned busarray$v_no_reconnect : 1;
      unsigned busarray$v_fill_0_ : 7;
    };
  };
  struct _crb *busarray$ps_crb;       
  struct _adp *busarray$ps_adp;       
  union  {
    unsigned int busarray$l_autoconfig; 
    void *busarray$ps_autoconfig;   
  };
  union  {
    unsigned int busarray$l_ctrlltr;
    unsigned char busarray$b_ctrlltr;
  };

  union  {
    unsigned long long busarray$q_bus_specific;
    struct  {
      union  {
	unsigned int busarray$l_bus_specific_l;
	void *busarray$ps_bus_specific_l;
	unsigned int busarray$l_int_vec; 
	unsigned int busarray$l_sys_irq; 
      };
      union  {
	unsigned int busarray$l_bus_specific_h;
	void *busarray$ps_bus_specific_h;
	unsigned int busarray$l_bus_specific_1;
	void *busarray$ps_bus_specific_1;
      };
    };
  };
  unsigned int busarray$l_cpu_affinity; 
  union  {                          
    unsigned int busarray$l_bus_specific_2;
    void *busarray$ps_bus_specific_2;
  };
};
 
#define BUSARRAYENTRY$K_LENGTH 56

#define BUSARRAYHEADER$K_LENGTH 24      

#define BUSARRAY$S_BUSARRAYHEADER 24    
	
struct _busarray_header {
  struct _adp *busarray$ps_parent_adp; 
  unsigned int busarray$l_fill1;      
  unsigned short int busarray$w_size; 
  unsigned char busarray$b_type;      
  unsigned char busarray$b_subtype;   
  unsigned int busarray$l_bus_type;   
  unsigned int busarray$l_bus_node_cnt; 
  unsigned int busarray$l_fill2;      
  union  {
    long long busarray$q_entry_list [1]; 
    struct _busarrayentry busarray$r_bus_array_entry; 
  };
};
 
#endif 
 
