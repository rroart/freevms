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
  } lck$r_lckdef0;
  struct  {
    unsigned lck$v_deqall : 1;
    unsigned lck$v_cancel : 1;
    unsigned lck$v_invvalblk : 1;
    unsigned lck$v_fill_71_ : 5;
  } lck$r_lckdef1;

} LCKDEF;
 
#define lck$v_valblk lck$r_lckdef0.lck$v_valblk
#define lck$v_convert lck$r_lckdef0.lck$v_convert
#define lck$v_noqueue lck$r_lckdef0.lck$v_noqueue
#define lck$v_syncsts lck$r_lckdef0.lck$v_syncsts
#define lck$v_system lck$r_lckdef0.lck$v_system
#define lck$v_noquota lck$r_lckdef0.lck$v_noquota
#define lck$v_cvtsys lck$r_lckdef0.lck$v_cvtsys
#define lck$v_recover lck$r_lckdef0.lck$v_recover
#define lck$v_protect lck$r_lckdef0.lck$v_protect
#define lck$v_nodlckwt lck$r_lckdef0.lck$v_nodlckwt
#define lck$v_nodlckblk lck$r_lckdef0.lck$v_nodlckblk
#define lck$v_expedite lck$r_lckdef0.lck$v_expedite
#define lck$v_quecvt lck$r_lckdef0.lck$v_quecvt
#define lck$v_bypass lck$r_lckdef0.lck$v_bypass
#define lck$v_deqall lck$r_lckdef1.lck$v_deqall
#define lck$v_cancel lck$r_lckdef1.lck$v_cancel
#define lck$v_invvalblk lck$r_lckdef1.lck$v_invvalblk
 
#endif
 
