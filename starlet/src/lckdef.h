#ifndef lckdef_h
#define lckdef_h

#define LCK$M_VALBLK 0x1
#define LCK$M_CONVERT 0x2
#define LCK$M_NOQUEUE 0x4
#define LCK$M_SYNCSTS 0x8
#define LCK$M_SYSTEM 0x10
#define LCK$M_NOQUOTA 0x20
#define LCK$M_CVTSYS 0x40
#define LCK$M_RECOVER 0x80
#define LCK$M_PROTECT 0x100
#define LCK$M_NODLCKWT 0x200
#define LCK$M_NODLCKBLK 0x400
#define LCK$M_EXPEDITE 0x800
#define LCK$M_QUECVT 0x1000
#define LCK$M_BYPASS 0x2000
#define LCK$M_DEQALL 0x1
#define LCK$M_CANCEL 0x2
#define LCK$M_INVVALBLK 0x4
#define LCK$K_NLMODE 0
#define LCK$K_CRMODE 1
#define LCK$K_CWMODE 2
#define LCK$K_PRMODE 3
#define LCK$K_PWMODE 4
#define LCK$K_EXMODE 5
	
union _lckdef {
  struct  {
    unsigned lck$v_valblk : 1;
    unsigned lck$v_convert : 1;
    unsigned lck$v_noqueue : 1;
    unsigned lck$v_syncsts : 1;
    unsigned lck$v_system : 1;
    unsigned lck$v_noquota : 1;
    unsigned lck$v_cvtsys : 1;
    unsigned lck$v_recover : 1;
    unsigned lck$v_protect : 1;
    unsigned lck$v_nodlckwt : 1;
    unsigned lck$v_nodlckblk : 1;
    unsigned lck$v_expedite : 1;
    unsigned lck$v_quecvt : 1;
    unsigned lck$v_bypass : 1;
    unsigned lck$v_fill_70_ : 2;
  };
  struct  {
    unsigned lck$v_deqall : 1;
    unsigned lck$v_cancel : 1;
    unsigned lck$v_invvalblk : 1;
    unsigned lck$v_fill_71_ : 5;
  };

};
 
#endif
 
