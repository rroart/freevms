#ifndef cmbdef_h
#define cmbdef_h

#define CMB$M_READONLY  0x1
#define CMB$M_WRITEONLY 0x2
	
union _cmbdef {
  struct {
    unsigned cmb$v_readonly  : 1;
    unsigned cmb$v_writeonly : 1;
    unsigned cmb$v_nothing   : 30;
  };
};

#endif
 
