#ifndef tt2def_h
#define tt2def_h
 
#define		TT2$M_LOCALECHO		0x1
#define		TT2$M_AUTOBAUD		0x2
#define		TT2$M_HANGUP		0x4
#define		TT2$M_MODHANGUP		0x8
#define		TT2$M_BRDCSTMBX		0x10
#define		TT2$M_XON		0x20
#define		TT2$M_DMA		0x40
#define		TT2$M_ALTYPEAHD		0x80
#define		TT2$M_SETSPEED		0x100
#define		TT2$M_DCL_MAILBX	0x200
#define		TT2$M_DECCRT4		0x400
#define		TT2$M_COMMSYNC		0x800
#define		TT2$M_EDITING		0x1000
#define		TT2$M_INSERT		0x2000
#define		TT2$M_FALLBACK		0x4000
#define		TT2$M_DIALUP		0x8000
#define		TT2$M_SECURE		0x10000
#define		TT2$M_DISCONNECT	0x20000
#define		TT2$M_PASTHRU		0x40000
#define		TT2$M_SYSPWD		0x80000
#define		TT2$M_SIXEL		0x100000
#define		TT2$M_DRCS		0x200000
#define		TT2$M_PRINTER		0x400000
#define		TT2$M_APP_KEYPAD	0x800000
#define		TT2$M_ANSICRT		0x1000000
#define		TT2$M_REGIS		0x2000000
#define		TT2$M_BLOCK		0x4000000
#define		TT2$M_AVO		0x8000000
#define		TT2$M_EDIT		0x10000000
#define		TT2$M_DECCRT		0x20000000
#define		TT2$M_DECCRT2		0x40000000
#define		TT2$M_DECCRT3		0x80000000
	
#define		TT2$V_LOCALECHO		0
#define		TT2$V_AUTOBAUD		1
#define		TT2$V_HANGUP		2
#define		TT2$V_MODHANGUP		3
#define		TT2$V_BRDCSTMBX		4
#define		TT2$V_XON		5
#define		TT2$V_DMA		6
#define		TT2$V_ALTYPEAHD		7
#define		TT2$V_SETSPEED		8
#define		TT2$V_DCL_MAILBX	9
#define		TT2$V_DECCRT4		10
#define		TT2$V_COMMSYNC		11
#define		TT2$V_EDITING		12
#define		TT2$V_INSERT		13
#define		TT2$V_FALLBACK		14
#define		TT2$V_DIALUP		15
#define		TT2$V_SECURE		16
#define		TT2$V_DISCONNECT	17
#define		TT2$V_PASTHRU		18
#define		TT2$V_SYSPWD		19
#define		TT2$V_SIXEL		20
#define		TT2$V_DRCS		21
#define		TT2$V_PRINTER		22
#define		TT2$V_APP_KEYPAD	23
#define		TT2$V_ANSICRT		24
#define		TT2$V_REGIS		25
#define		TT2$V_BLOCK		26
#define		TT2$V_AVO		27
#define		TT2$V_EDIT		28
#define		TT2$V_DECCRT		29
#define		TT2$V_DECCRT2		30
#define		TT2$V_DECCRT3		31
 
union _tt2def {
  struct  {
    unsigned tt2$v_localecho		: 1;
    unsigned tt2$v_autobaud		: 1;
    unsigned tt2$v_hangup		: 1;
    unsigned tt2$v_modhangup		: 1;
    unsigned tt2$v_brdcstmbx		: 1;
    unsigned tt2$v_xon			: 1;
    unsigned tt2$v_dma			: 1;
    unsigned tt2$v_altypeahd		: 1;
    unsigned tt2$v_setspeed		: 1;
    unsigned tt2$v_dcl_mailbx		: 1;
    unsigned tt2$v_deccrt4		: 1;
    unsigned tt2$v_commsync		: 1;
    unsigned tt2$v_editing		: 1;
    unsigned tt2$v_insert		: 1;
    unsigned tt2$v_fallback		: 1;
    unsigned tt2$v_dialup		: 1;
    unsigned tt2$v_secure		: 1;
    unsigned tt2$v_disconnect		: 1;
    unsigned tt2$v_pasthru		: 1;
    unsigned tt2$v_syspwd		: 1;
    unsigned tt2$v_sixel		: 1;
    unsigned tt2$v_drcs			: 1;
    unsigned tt2$v_printer		: 1;
    unsigned tt2$v_app_keypad		: 1;
    unsigned tt2$v_ansicrt		: 1;
    unsigned tt2$v_regis		: 1;
    unsigned tt2$v_block		: 1;
    unsigned tt2$v_avo			: 1;
    unsigned tt2$v_edit			: 1;
    unsigned tt2$v_deccrt		: 1;
    unsigned tt2$v_deccrt2		: 1;
    unsigned tt2$v_deccrt3		: 1;
  }; 
};
 
#endif
 
