#ifndef ttdef_h
#define ttdef_h

#define		TT$C_BAUD_50		1
#define		TT$C_BAUD_75		2
#define		TT$C_BAUD_110		3
#define		TT$C_BAUD_134		4
#define		TT$C_BAUD_150		5
#define		TT$C_BAUD_300		6
#define		TT$C_BAUD_600		7
#define		TT$C_BAUD_1200		8
#define		TT$C_BAUD_1800		9
#define		TT$C_BAUD_2000		10
#define		TT$C_BAUD_2400		11
#define		TT$C_BAUD_3600		12
#define		TT$C_BAUD_4800		13
#define		TT$C_BAUD_7200		14
#define		TT$C_BAUD_9600		15
#define		TT$C_BAUD_19200		16
#define		TT$C_BAUD_38400		17
#define		TT$C_BAUD_57600		18
#define		TT$C_BAUD_76800		19
#define		TT$C_BAUD_115200	20

#define		TT$_UNKNOWN		0
#define		TT$_VT05		1
#define		TT$_VK100		2
#define		TT$_VT173		3
#define		TT$_TQ_BTS		4
#define		TT$_TEK401X		10
#define		TT$_FT1			16
#define		TT$_FT2			17
#define		TT$_FT3			18
#define		TT$_FT4			19
#define		TT$_FT5			20
#define		TT$_FT6			21
#define		TT$_FT7			22
#define		TT$_FT8			23

#define		TT$_LAX			32
#define		TT$_LA36		32
#define		TT$_LA120		33
#define		TT$_LA34		34
#define		TT$_LA38		35
#define		TT$_LA12		36
#define		TT$_LA100		37
#define		TT$_LA24		37
#define		TT$_LQP02		38
#define		TT$_LA84		39
#define		TT$_LA210		40
#define		TT$_LN03		41
#define		TT$_LN01K		42
#define		TT$_LA80		43
#define		TT$_VT5X		64
#define		TT$_VT52		64
#define		TT$_VT55		65
#define		TT$_VT100		96
#define		TT$_VT101		97
#define		TT$_VT102		98
#define		TT$_VT105		99
#define		TT$_VT125		100
#define		TT$_VT131		101
#define		TT$_VT132		102
#define		TT$_VT80		103
#define		TT$_VT200_SERIES	110
#define		TT$_PRO_SERIES		111
#define		TT$_VT300_SERIES	112
#define		TT$_VT400_SERIES	113
#define		TT$_VT500_SERIES	114

#define		TT$M_PASSALL		0x1
#define		TT$M_NOECHO		0x2
#define		TT$M_NOTYPEAHD		0x4
#define		TT$M_ESCAPE		0x8
#define		TT$M_HOSTSYNC		0x10
#define		TT$M_TTSYNC		0x20
#define		TT$M_SCRIPT		0x40
#define		TT$M_LOWER		0x80
#define		TT$M_MECHTAB		0x100
#define		TT$M_WRAP		0x200
#define		TT$M_CRFILL		0x400
#define		TT$M_LFFILL		0x800
#define		TT$M_SCOPE		0x1000
#define		TT$M_REMOTE		0x2000
#define		TT$M_RESERVED		0x4000
#define		TT$M_EIGHTBIT		0x8000
#define		TT$M_MBXDSABL		0x10000
#define		TT$M_NOBRDCST		0x20000
#define		TT$M_READSYNC		0x40000
#define		TT$M_MECHFORM		0x80000
#define		TT$M_HALFDUP		0x100000
#define		TT$M_MODEM		0x200000
#define		TT$M_OPER		0x400000
#define		TT$M_PAGE		0xFF000000
#define		TT$M_ALTFRAME		0x10
#define		TT$M_ALTRPAR		0x20
#define		TT$M_PARITY		0x40
#define		TT$M_ODD		0x80
#define		TT$M_TWOSTOP		0x100
#define		TT$M_DISPARERR		0x200
#define		TT$M_ALTDISPAR		0x400
#define		TT$M_BREAK		0x800
#define		TT$M_DS_DTR		0x2
#define		TT$M_DS_SECTX		0x8
#define		TT$M_DS_RTS		0x10
#define		TT$M_DS_SECREC		0x8
#define		TT$M_DS_CTS		0x10
#define		TT$M_DS_CARRIER		0x20
#define		TT$M_DS_RING		0x40
#define		TT$M_DS_DSR		0x80
	
#define		TT$V_PASSALL		0x00
#define		TT$V_NOECHO		0x01
#define		TT$V_NOTYPEAHD		0x02
#define		TT$V_ESCAPE		0x03
#define		TT$V_HOSTSYNC		0x04
#define		TT$V_TTSYNC		0x05
#define		TT$V_SCRIPT		0x06
#define		TT$V_LOWER		0x07
#define		TT$V_MECHTAB		0x08
#define		TT$V_WRAP		0x09
#define		TT$V_CRFILL		0x0A
#define		TT$V_LFFILL		0x0B
#define		TT$V_SCOPE		0x0C
#define		TT$V_REMOTE		0x0D
#define		TT$V_RESERVED		0x0E
#define		TT$V_EIGHTBIT		0x0F
#define		TT$V_MBXDSABL		0x10
#define		TT$V_NOBRDCST		0x11
#define		TT$V_READSYNC		0x12
#define		TT$V_MECHFORM		0x13
#define		TT$V_HALFDUP		0x14
#define		TT$V_MODEM		0x15
#define		TT$V_OPER		0x16
#define		TT$V_PAGE		0x18
#define		TT$V_ALTFRAME		0x04
#define		TT$V_ALTRPAR		0x05
#define		TT$V_PARITY		0x06
#define		TT$V_ODD		0x07
#define		TT$V_TWOSTOP		0x08
#define		TT$V_DISPARERR		0x09
#define		TT$V_ALTDISPAR		0x0A
#define		TT$V_BREAK		0x0B
#define		TT$V_DS_DTR		0x01
#define		TT$V_DS_SECTX		0x03
#define		TT$V_DS_RTS		0x04
#define		TT$V_DS_SECREC		0x03
#define		TT$V_DS_CTS		0x04
#define		TT$V_DS_CARRIER		0x05
#define		TT$V_DS_RING		0x06
#define		TT$V_DS_DSR		0x07
 
union _ttdef {
  struct  {
    unsigned tt$v_passall		: 1;
    unsigned tt$v_noecho		: 1;
    unsigned tt$v_notypeahd		: 1;
    unsigned tt$v_escape		: 1;
    unsigned tt$v_hostsync		: 1;
    unsigned tt$v_ttsync		: 1;
    unsigned tt$v_script		: 1;
    unsigned tt$v_lower			: 1;
    unsigned tt$v_mechtab		: 1;
    unsigned tt$v_wrap			: 1;
    unsigned tt$v_crfill		: 1;
    unsigned tt$v_lffill		: 1;
    unsigned tt$v_scope			: 1;
    unsigned tt$v_remote		: 1;
    unsigned tt$v_reserved		: 1;
    unsigned tt$v_eightbit		: 1;
    unsigned tt$v_mbxdsabl		: 1;
    unsigned tt$v_nobrdcst		: 1;
    unsigned tt$v_readsync		: 1;
    unsigned tt$v_mechform		: 1;
    unsigned tt$v_halfdup		: 1;
    unsigned tt$v_modem			: 1;
    unsigned tt$v_oper			: 1;
    unsigned ttdef$$_fill_1		: 1;
    unsigned tt$v_page			: 8;
  };
  struct  {
    unsigned ttdef$$_fill_2		: 4;
    unsigned tt$v_altframe		: 1;
    unsigned tt$v_altrpar		: 1;
    unsigned tt$v_parity		: 1;
    unsigned tt$v_odd			: 1;
    unsigned tt$v_twostop		: 1;
    unsigned tt$v_disparerr		: 1;
    unsigned tt$v_altdispar		: 1;
    unsigned tt$v_break			: 1;
    unsigned tt$v_fill_43_		: 4;
  };
  struct  {
    unsigned ttdef$$_fill_3		: 1;
    unsigned tt$v_ds_dtr		: 1;
    unsigned ttdef$$_fill_4		: 1;
    unsigned tt$v_ds_sectx		: 1;
    unsigned tt$v_ds_rts		: 1;
    unsigned tt$v_fill_44_		: 3;
  };
  struct  {
    unsigned ttdef$$_fill_5		: 3;
    unsigned tt$v_ds_secrec		: 1;
    unsigned tt$v_ds_cts		: 1;
    unsigned tt$v_ds_carrier		: 1;
    unsigned tt$v_ds_ring		: 1;
    unsigned tt$v_ds_dsr		: 1;
  };
};
 
#endif
 
