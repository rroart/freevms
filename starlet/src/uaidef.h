#ifndef uaidef_h
#define uaidef_h

#define		UAI$_RTYPE		1
#define		UAI$_VERSION		2
#define		UAI$_USRDATOFF		3
#define		UAI$_USERNAME		4
#define		UAI$_USERNAME_TAG	5
#define		UAI$_UIC		6
#define		UAI$_MEM		7
#define		UAI$_GRP		8
#define		UAI$_SUB_ID		9
#define		UAI$_PARENT_ID		10
#define		UAI$_ACCOUNT		11
#define		UAI$_OWNER		12
#define		UAI$_DEFDEV		13
#define		UAI$_DEFDIR		14
#define		UAI$_LGICMD		15
#define		UAI$_DEFCLI		16
#define		UAI$_CLITABLES		17
#define		UAI$_PWD		18
#define		UAI$_PWD2		19
#define		UAI$_LOGFAILS		20
#define		UAI$_SALT		21
#define		UAI$_ENCRYPT		22
#define		UAI$_ENCRYPT2		23
#define		UAI$_PWD_LENGTH		24
#define		UAI$_EXPIRATION		25
#define		UAI$_PWD_LIFETIME	26
#define		UAI$_PWD_DATE		27
#define		UAI$_PWD2_DATE		28
#define		UAI$_LASTLOGIN_I	29
#define		UAI$_LASTLOGIN_N	30
#define		UAI$_PRIV		31
#define		UAI$_DEF_PRIV		32
#define		UAI$_MIN_CLASS		33
#define		UAI$_MAX_CLASS		34
#define		UAI$_FLAGS		35
#define		UAI$_NETWORK_ACCESS_P	36
#define		UAI$_NETWORK_ACCESS_S	37
#define		UAI$_BATCH_ACCESS_P	38
#define		UAI$_BATCH_ACCESS_S	39
#define		UAI$_LOCAL_ACCESS_P	40
#define		UAI$_LOCAL_ACCESS_S	41
#define		UAI$_DIALUP_ACCESS_P	42
#define		UAI$_DIALUP_ACCESS_S	43
#define		UAI$_REMOTE_ACCESS_P	44
#define		UAI$_REMOTE_ACCESS_S	45
#define		UAI$_PRIMEDAYS		46
#define		UAI$_PRI		47
#define		UAI$_QUEPRI		48
#define		UAI$_MAXJOBS		49
#define		UAI$_MAXACCTJOBS	50
#define		UAI$_MAXDETACH		51
#define		UAI$_PRCCNT		52
#define		UAI$_BIOLM		53
#define		UAI$_DIOLM		54
#define		UAI$_TQCNT		55
#define		UAI$_ASTLM		56
#define		UAI$_ENQLM		57
#define		UAI$_FILLM		58
#define		UAI$_SHRFILLM		59
#define		UAI$_WSQUOTA		60
#define		UAI$_DFWSCNT		61
#define		UAI$_WSEXTENT		62
#define		UAI$_PGFLQUOTA		63
#define		UAI$_CPUTIM		64
#define		UAI$_BYTLM		65
#define		UAI$_PBYTLM		66
#define		UAI$_JTQUOTA		67
#define		UAI$_PROXY_LIM		68
#define		UAI$_PROXIES		69
#define		UAI$_ACCOUNT_LIM	70
#define		UAI$_ACCOUNTS		71
#define		UAI$_USER_DATA		72
#define		UAI$_PASSWORD		73
#define		UAI$_PASSWORD2		74
#define		UAI$_DEF_CLASS		75
#define		UAI$_AUDIT_FLAGS	76

#define		UAI$_MAX_ITEM_CODE	77

#define		UAI$_DEFCHARGE_CODE	11

#define		UAI$C_AD_II			0
#define		UAI$C_PURDY			1
#define		UAI$C_PURDY_V			2
#define		UAI$C_PURDY_S			3
#define		UAI$C_PREFERED_ALGORITHM	127
#define		UAI$C_PREFERRED_ALGORITHM	127
#define		UAI$C_CUST_ALGORITHM		128
#define		UAI$C_MAX_PWD_LENGTH		32

#define		UAI$M_DISCTLY			0x1
#define		UAI$M_DEFCLI			0x2
#define		UAI$M_LOCKPWD			0x4
#define		UAI$M_RESTRICTED		0x8
#define		UAI$M_DISACNT			0x10
#define		UAI$M_DISWELCOM			0x20
#define		UAI$M_DISMAIL			0x40
#define		UAI$M_NOMAIL			0x80
#define		UAI$M_GENPWD			0x100
#define		UAI$M_PWD_EXPIRED		0x200
#define		UAI$M_PWD2_EXPIRED		0x400
#define		UAI$M_AUDIT			0x800
#define		UAI$M_DISREPORT			0x1000
#define		UAI$M_DISRECONNECT		0x2000
#define		UAI$M_AUTOLOGIN			0x4000
#define		UAI$M_DISFORCE_PWD_CHANGE	0x8000
#define		UAI$M_CAPTIVE			0x10000
#define		UAI$M_DISIMAGE			0x20000
#define		UAI$M_DISPWDDIC			0x40000
#define		UAI$M_DISPWDHIS			0x80000
#define		UAI$M_DEFCLSVAL			0x100000
#define		UAI$M_EXTAUTH			0x200000
#define		UAI$M_MIGRATEPWD		0x400000
#define		UAI$M_VMSAUTH			0x800000
#define		UAI$M_DISPWDSYNCH		0x1000000
	
struct _flags {
  unsigned uai$v_disctly		: 1;
  unsigned uai$v_defcli			: 1;
  unsigned uai$v_lockpwd		: 1;
  unsigned uai$v_restricted		: 1;
  unsigned uai$v_disacnt		: 1;
  unsigned uai$v_diswelcom		: 1;
  unsigned uai$v_dismail		: 1;
  unsigned uai$v_nomail			: 1;
  unsigned uai$v_genpwd			: 1;
  unsigned uai$v_pwd_expired		: 1;
  unsigned uai$v_pwd2_expired		: 1;
  unsigned uai$v_audit			: 1;
  unsigned uai$v_disreport		: 1;
  unsigned uai$v_disreconnect		: 1;
  unsigned uai$v_autologin		: 1;
  unsigned uai$v_disforce_pwd_change	: 1;
  unsigned uai$v_captive		: 1;
  unsigned uai$v_disimage		: 1;
  unsigned uai$v_dispwddic		: 1;
  unsigned uai$v_dispwdhis		: 1;
  unsigned uai$v_defclsval		: 1;
  unsigned uai$v_extauth		: 1;
  unsigned uai$v_migratepwd		: 1;
  unsigned uai$v_vmsauth		: 1;
  unsigned uai$v_dispwdsynch		: 1;
  unsigned uai$v_fill_59_		: 7;
};

struct _primedays {
  unsigned uai$v_monday		: 1;
  unsigned uai$v_tuesday	: 1;
  unsigned uai$v_wednesday	: 1;
  unsigned uai$v_thursday	: 1;
  unsigned uai$v_friday		: 1;
  unsigned uai$v_saturday	: 1;
  unsigned uai$v_sunday		: 1;
  unsigned uai$v_fill_60_	: 1;
};

#define		UAI$V_DISCTLY			0	
#define		UAI$V_DEFCLI			1
#define		UAI$V_LOCKPWD			2
#define		UAI$V_RESTRICTED		3
#define		UAI$V_DISACNT			4
#define		UAI$V_DISWELCOM			5
#define		UAI$V_DISMAIL			6
#define		UAI$V_NOMAIL			7
#define		UAI$V_GENPWD			8
#define		UAI$V_PWD_EXPIRED		9
#define		UAI$V_PWD2_EXPIRED		10
#define		UAI$V_AUDIT			11
#define		UAI$V_DISREPORT			12
#define		UAI$V_DISRECONNECT		13
#define		UAI$V_AUTOLOGIN			14
#define		UAI$V_DISFORCE_PWD_CHANGE	15
#define		UAI$V_CAPTIVE			16
#define		UAI$V_DISIMAGE			17
#define		UAI$V_DISPWDDIC			18
#define		UAI$V_DISPWDHIS			19
#define		UAI$V_DEFCLSVAL			20
#define		UAI$V_EXTAUTH			21
#define		UAI$V_MIGRATEPWD		22
#define		UAI$V_VMSAUTH			23
#define		UAI$V_DISPWDSYNCH		24
#define		UAI$V_MONDAY			0
#define		UAI$V_TUESDAY			1
#define		UAI$V_WEDNESDAY			2
#define		UAI$V_THURSDAY			3
#define		UAI$V_FRIDAY			4
#define		UAI$V_SATURDAY			5
#define		UAI$V_SUNDAY			6

#endif
