#ifndef psldef_h
#define psldef_h

#define PSL$M_PRVMOD 0x3
#define PSL$M_CURMOD 0x18
#define PSL$M_IPL 0x1F00
#define PSL$V_MAX_PS_REG_BIT 13

#define PSL$C_KERNEL 0
#define PSL$C_EXEC 1
#define PSL$C_SUPER 2
#define PSL$C_USER 3
	
union _psldef {
  struct  {
    unsigned psl$v_prvmod    : 2;
    unsigned psldef$$_fill_1 : 1;
    unsigned psl$v_curmod    : 2;
    unsigned psldef$$_fill_2 : 3;
    unsigned psl$v_ipl       : 5;
    unsigned psl$v_fill_86_  : 3;
  };
};
 
#endif
 
