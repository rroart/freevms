#ifndef iodef_h
#define iodef_h

#define		IO$_NOP		        0
#define		IO$_UNLOAD		1
#define		IO$_LOADMCODE		1
#define		IO$_START_BUS		1
#define		IO$_SEEK		2
#define		IO$_SPACEFILE		2
#define		IO$_STARTMPROC		2
#define		IO$_STOP_BUS		2
#define		IO$_RECAL		3
#define		IO$_DUPLEX		3
#define		IO$_STOP		3
#define		IO$_DEF_COMP		3
#define		IO$_DRVCLR		4
#define		IO$_INITIALIZE		4
#define		IO$_MIMIC		4
#define		IO$_DEF_COMP_LIST	4
#define		IO$_RELEASE		5
#define		IO$_SETCLOCKP		5
#define		IO$_START_ANALYSIS	5
#define		IO$_BIND		5
#define		IO$_PATH_VERIFY		5
#define		IO$_OFFSET		6
#define		IO$_ERASETAPE		6
#define		IO$_STARTDATAP		6
#define		IO$_STOP_ANALYSIS	6
#define		IO$_UNBIND		6
#define		IO$_RETCENTER		7
#define		IO$_QSTOP		7
#define		IO$_START_MONITOR	7
#define		IO$_CREDISK		7
#define		IO$_LOCAL_DRAIN		7
#define		IO$_PACKACK		8
#define		IO$_STOP_MONITOR	8
#define		IO$_SEARCH		9
#define		IO$_SPACERECORD		9
#define		IO$_READRCT		9
#define		IO$_REMDISK		9
#define		IO$_MANAGE_PE		9
#define		IO$_WRITECHECK		10
#define		IO$_EXPORT		10
#define		IO$_WRITEPBLK		11
#define		IO$_READPBLK		12
#define		IO$_WRITEHEAD		13
#define		IO$_RDSTATS		13
#define		IO$_CRESHAD		13
#define		IO$_READHEAD		14
#define		IO$_ADDSHAD		14
#define		IO$_WRITETRACKD		15
#define		IO$_COPYSHAD		15
#define		IO$_READTRACKD		16
#define		IO$_REMSHAD		16
#define		IO$_AVAILABLE		17
#define		IO$_SETPRFPATH		18
#define		IO$_DISPLAY		19
#define		IO$_REMSHADMBR		20
#define		IO$_DSE		        21
#define		IO$_REREADN		22
#define		IO$_DISK_COPY_DATA	22
#define		IO$_MOUNTSHAD		23
#define		IO$_REREADP		23
#define		IO$_WHM		        23
#define		IO$_AS_SETCHAR		23
#define		IO$_WRITERET		24
#define		IO$_WRITECHECKH		24
#define		IO$_AS_SENSECHAR	24
#define		IO$_ADDSHADMBR		25
#define		IO$_READPRESET		25
#define		IO$_STARTSPNDL		25
#define		IO$_SETCHAR		26
#define		IO$_SENSECHAR		27
#define		IO$_WRITEMARK		28
#define		IO$_COPYMEM		28
#define		IO$_PSXSETCHAR		28
#define		IO$_WRTTMKR		29
#define		IO$_DIAGNOSE		29
#define		IO$_SHADMV		29
#define		IO$_PSXSENSECHAR	29
#define		IO$_FORMAT		30
#define		IO$_CLEAN		30
#define		IO$_UPSHAD		30
#define		IO$_PHYSICAL		31

#define		IO$_WRITELBLK		32
#define		IO$_READLBLK		33
#define		IO$_REWINDOFF		34
#define		IO$_READRCTL		34
#define		IO$_SETMODE		35
#define		IO$_REWIND		36
#define		IO$_SKIPFILE		37
#define		IO$_PSXSETMODE		37
#define		IO$_SKIPRECORD		38
#define		IO$_PSXSENSEMODE	38
#define		IO$_SENSEMODE		39
#define		IO$_WRITEOF		40
#define		IO$_TTY_PORT_BUFIO	40
#define		IO$_TTY_PORT		41
#define		IO$_FREECAP		41
#define		IO$_FLUSH		42
#define		IO$_AS_SETMODE		42
#define		IO$_READLCHUNK		43
#define		IO$_AS_SENSEMODE	43
#define		IO$_WRITELCHUNK		44
#define		IO$_LOGICAL		47

#define		IO$_WRITEVBLK		48
#define		IO$_READVBLK		49
#define		IO$_ACCESS		50
#define		IO$_PSXWRITEVBLK	50
#define		IO$_CREATE		51
#define		IO$_DEACCESS		52
#define		IO$_PSXREADVBLK		52
#define		IO$_DELETE		53
#define		IO$_MODIFY		54
#define		IO$_NETCONTROL		54
#define		IO$_READPROMPT		55
#define		IO$_SETCLOCK		55
#define		IO$_AUDIO		55
#define		IO$_ACPCONTROL		56
#define		IO$_STARTDATA		56
#define		IO$_IOCTLV		56
#define		IO$_MOUNT		57
#define		IO$_TTYREADALL		58
#define		IO$_DISMOUNT		58
#define		IO$_TTYREADPALL		59
#define		IO$_USBATTR		59
#define		IO$_CONINTREAD		60
#define		IO$_CONINTWRITE		61
#define		IO$_READDIR		62
#define		IO$_VIRTUAL		63

#define		IO$M_FCODE		0x3F
#define		IO$M_FMODIFIERS		0xFFC0
#define		IO$M_INHERLOG		0x800
#define		IO$M_ERASE		0x400
#define		IO$M_EXFUNC		0x2000
#define		IO$M_DATACHECK		0x4000
#define		IO$M_INHRETRY		0x8000
#define		IO$M_SYNCSTS		0x10000
#define		IO$M_NOVCACHE		0x20000
#define		IO$M_BUFOBJ		0x40000
#define		IO$M_TRUSTED		0x80000
#define		IO$M_FILE_FLUSH		0x100000
#define		IO$M_BARRIER		0x200000
#define		IO$M_INHSEEK		0x1000
#define		IO$M_REVERSE		0x40
#define		IO$M_NOWAIT		0x80
#define		IO$M_INHEXTGAP		0x1000
#define		IO$M_RETENSION		0x2000
#define		IO$M_ALLOWFAST		0x4000
#define		IO$M_MT3_DENSITY	0x8000
#define		IO$M_MSCPMODIFS		0x100
#define		IO$M_SHADOW		0x40
#define		IO$M_LOCATE		0x80
#define		IO$M_MSCP_FORMAT	0x40
#define		IO$M_ALLHOSTS		0x40
#define		IO$M_DISSOLVE		0x80
#define		IO$M_NOCLEANUP		0x100
#define		IO$M_SPINDOWN		0x40
#define		IO$M_EST_COM_PATH	0x40
#define		IO$M_LCL_SRC_UNIT	0x80
#define		IO$M_RTN_COM_PATH	0x100
#define		IO$M_DEALC_ALL		0x40
#define		IO$M_DEALC_HRN		0x80
#define		IO$M_DEALC_ENTLOC	0x100
#define		IO$M_DECR_AFC		0x200
#define		IO$M_READ_ALL		0x400
#define		IO$M_READ_HRN		0x800
#define		IO$M_BREAK_CONN		0x1000
#define		IO$M_STEPOVER		0x40
#define		IO$M_COPYOP		0x100
#define		IO$M_EXISTS		0x40
#define		IO$M_CBS		0x80
#define		IO$M_BOOTING		0x100
#define		IO$M_REQ_MINICOPY	0x200
#define		IO$M_OPT_MINICOPY	0x400
#define		IO$M_NOMINICOPY		0x800
#define		IO$M_VUEX_FC		0x200
#define		IO$M_FORCEPATH		0x40
#define		IO$M_PREFERRED_CPU	0x80
#define		IO$M_SYS_ASSIGNABLE	0x100
#define		IO$M_COMMOD		0x40
#define		IO$M_MOVETRACKD		0x80
#define		IO$M_DIAGNOSTIC		0x100
#define		IO$M_SKPSECINH		0x200
#define		IO$M_DELDATA		0x40
#define		IO$M_NOMRSP		0x40
#define		IO$M_SWAP		0x100
#define		IO$M_OPPOSITE		0x200
#define		IO$M_CLSEREXCP		0x200
#define		IO$M_CHUNKDIAG		0x40
#define		IO$M_TBC		0x400
#define		IO$M_ENAREP		0x40
#define		IO$M_ACCESS		0x40
#define		IO$M_CREATE		0x80
#define		IO$M_DELETE		0x100
#define		IO$M_MOUNT		0x200
#define		IO$M_DMOUNT		0x400
#define		IO$M_REMOUNT		0x800
#define		IO$M_MOVEFILE		0x1000
#define		IO$M_RWSHELVED		0x4000
#define		IO$M_SETDIR		0x8000
#define		IO$M_BINARY		0x40
#define		IO$M_PACKED		0x80
#define		IO$M_NOW		0x40
#define		IO$M_STREAM		0x80
#define		IO$M_READERCHECK	0x100
#define		IO$M_WRITERCHECK	0x200
#define		IO$M_NORSWAIT		0x400
#define		IO$M_MB_ROOM_NOTIFY	0x40
#define		IO$M_READATTN		0x80
#define		IO$M_WRTATTN		0x100
#define		IO$M_SETPROT		0x200
#define		IO$M_READERWAIT		0x400
#define		IO$M_WRITERWAIT		0x800
#define		IO$M_NOECHO		0x40
#define		IO$M_TIMED		0x80
#define		IO$M_CVTLOW		0x100
#define		IO$M_NOFILTR		0x200
#define		IO$M_DSABLMBX		0x400
#define		IO$M_PURGE		0x800
#define		IO$M_TRMNOECHO		0x1000
#define		IO$M_REFRESH		0x2000
#define		IO$M_ESCAPE		0x4000
#define		IO$M_EXTEND		0x8000
#define		IO$M_CANCTRLO		0x40
#define		IO$M_ENABLMBX		0x80
#define		IO$M_NOFORMAT		0x100
#define		IO$M_BREAKTHRU		0x200
#define		IO$M_NEWLINE		0x400
#define		IO$M_TYPEAHDCNT		0x40
#define		IO$M_MAINT		0x40
#define		IO$M_CTRLYAST		0x80
#define		IO$M_CTRLCAST		0x100
#define		IO$M_HANGUP		0x200
#define		IO$M_OUTBAND		0x400
#define		IO$M_TT_CONNECT		0x800
#define		IO$M_TT_DISCON		0x1000
#define		IO$M_TT_PROCESS		0x2000
#define		IO$M_BRDCST		0x4000
#define		IO$M_LOOP		0x80
#define		IO$M_UNLOOP		0x100
#define		IO$M_LINE_OFF		0x200
#define		IO$M_SET_MODEM		0x400
#define		IO$M_LINE_ON		0x800
#define		IO$M_LOOP_EXT		0x1000
#define		IO$M_AUTXOF_ENA		0x2000
#define		IO$M_AUTXOF_DIS		0x4000
#define		IO$M_INCLUDE		0x800
#define		IO$M_TT_ABORT		0x1000
#define		IO$M_POSIXINIT		0x40
#define		IO$M_POSIXFLOW		0x80
#define		IO$M_SET_POSIX		0x80
#define		IO$M_CLEAR_POSIX	0x100
#define		IO$M_SET_TERMIOS	0x200
#define		IO$M_SET_PTC		0x400
#define		IO$M_CLEAR_PTC		0x800
#define		IO$M_FLUSH_TAB		0x1000
#define		IO$M_FLUSH_OUTPUT	0x2000
#define		IO$M_UPDATE_PTC		0x4000
#define		IO$M_OUT_XOFF		0x100
#define		IO$M_OUT_XON		0x200
#define		IO$M_IN_XOFF		0x400
#define		IO$M_IN_XON		0x800
#define		IO$M_O_NONBLOCK		0x40
#define		IO$M_INTERRUPT		0x40
#define		IO$M_MULTIPLE		0x80
#define		IO$M_LOCKBUF		0x100
#define		IO$M_NOBLOCK		0x200
#define		IO$M_ABORT		0x100
#define		IO$M_SYNCH		0x200
#define		IO$M_RESPONSE		0x40
#define		IO$M_STARTUP		0x40
#define		IO$M_SHUTDOWN		0x80
#define		IO$M_ATTNAST		0x100
#define		IO$M_CTRL		0x200
#define		IO$M_SET_MAC		0x800
#define		IO$M_UPDATE_MAP		0x1000
#define		IO$M_ROUTE		0x2000
#define		IO$M_RD_MEM		0x40
#define		IO$M_RD_MODEM		0x80
#define		IO$M_RD_COUNT		0x100
#define		IO$M_CLR_COUNT		0x400
#define		IO$M_SENSE_MAC		0x800
#define		IO$M_SHOW_MAP		0x1000
#define		IO$M_SHOW_ROUTE		0x2000
#define		IO$K_SRRUNOUT		0
#define		IO$K_PTPBSC		8192
#define		IO$K_LOOPTEST		57344

#define		IO$M_MORE		0x40
#define		IO$M_QUALIFIED		0x80
#define		IO$M_REDIRECT		0x40
#define		IO$M_ACCEPT		0x80
#define		IO$M_SETEVF		0x40
#define		IO$M_WORD		0x40
#define		IO$M_SETFNCT		0x200
#define		IO$M_DATAPATH		0x400
#define		IO$M_CYCLE		0x1000
#define		IO$M_RESET		0x2000
#define		IO$M_SETCUADR		0x100
#define		IO$M_SETBSIZE		0x200
#define		IO$M_SETPOOLSZ		0x400
#define		IO$M_SETENQCNT		0x800
#define		IO$M_CLEAR		0x1000
#define		IO$M_LPBEXT		0x2000
#define		IO$M_LPBINT		0x4000
#define		IO$M_READCSR		0x8000
#define		IO$M_NOCTSWAIT		0x40
#define		IO$M_SLAVLOOP		0x80
#define		IO$M_NODSRWAIT		0x100
#define		IO$M_MAINTLOOP		0x200
#define		IO$M_LASTBLOCK		0x400
#define		IO$M_INTCLOCK		0x1000
#define		IO$M_LT_CONNECT		0x40
#define		IO$M_LT_DISCON		0x80
#define		IO$M_LT_READPORT	0x100
#define		IO$M_LT_MAP_PORT	0x200
#define		IO$M_LT_RATING		0x400
#define		IO$M_LT_SOL_INFO	0x800
#define		IO$M_LT_RCV_INFO	0x1000
#define		IO$M_LT_SETMODE		0x2000
#define		IO$M_LT_SENSEMODE	0x4000
#define		IO$M_LT_QUE_CHG_NOTIF	0x8000
#define		IO$M_LT_MAP_FILLER	0x1
#define		IO$M_LT_MAP_NODNAM	0x2
#define		IO$M_LT_MAP_PORNAM	0x4
#define		IO$M_LT_MAP_SRVNAM	0x8
#define		IO$M_LT_MAP_LNKNAM	0x10
#define		IO$M_LT_MAP_NETADR	0x20
#define		IO$M_FLUSH		0x40
#define		IO$M_WRITE_BARRIER	0x80
#define		IO$M_MKFILL1		0x40
#define		IO$M_ALLOWFAST_NEVER	0x80
#define		IO$M_ALLOWFAST_PER_IO	0x100
#define		IO$M_ALLOWFAST_ALWAYS	0x200
#define		IO$M_ADD		0x40
#define		IO$M_IDSTRING		0x40
#define		IO$M_SERIALNUM		0x80
#define		IO$M_QSRVFILL1		0x40
#define		IO$M_QSRV_CLIENT_UPDATE	0x80
	
union _iodef {

  struct  {
    unsigned io$v_fcode		        : 6;
    unsigned io$v_fmodifiers		: 10;
  };

  struct  {
    unsigned io$v_fcode_fill_1		: 6;
    unsigned io$v_filler_1		: 5;
    unsigned io$v_inherlog		: 1;
    unsigned io$v_fill_20_		: 4;
  };

  struct  {
    unsigned io$v_fcode_fill_2		: 6;
    unsigned io$v_rsvd_for_devdepend_1	: 4;
    unsigned io$v_erase		        : 1;
    unsigned io$v_filler_2		: 1;
    unsigned io$v_rsvd_for_devdepend_2	: 1;
    unsigned io$v_exfunc		: 1;
    unsigned io$v_datacheck		: 1;
    unsigned io$v_inhretry		: 1;

    unsigned io$v_syncsts		: 1;
    unsigned io$v_novcache		: 1;
    unsigned io$v_bufobj		: 1;
    unsigned io$v_trusted		: 1;
    unsigned io$v_file_flush		: 1;
    unsigned io$v_barrier		: 1;
    unsigned io$v_fill_21_		: 2;
  };

  struct  {
    unsigned io$v_fcode_fill_3		: 6;
    unsigned io$v_rsvd_for_devdepend_3	: 6;
    unsigned io$v_inhseek		: 1;
    unsigned io$v_fill_22_		: 3;
  };

  struct  {
    unsigned io$v_fcode_fill_4		: 6;
    unsigned io$v_reverse		: 1;
    unsigned io$v_nowait		: 1;
    unsigned io$v_rsvd_for_devdepend_4	: 4;
    unsigned io$v_inhextgap		: 1;
    unsigned io$v_retension		: 1;

    unsigned io$v_allowfast		: 1;

    unsigned io$v_mt3_density		: 1;
  };

  struct  {
    unsigned io$v_fcode_fill_5		: 6;
    unsigned io$v_dutux_reserved	: 2;
    unsigned io$v_mscpmodifs		: 1;
    unsigned io$v_fill_23_		: 7;
  };
  struct  {
    unsigned io$v_fcode_fill_6		: 6;
    unsigned io$v_shadow		: 1;
    unsigned io$v_locate		: 1;
  };
  struct  {
    unsigned io$v_fcode_fill_7		: 6;
    unsigned io$v_mscp_format		: 1;
    unsigned io$v_fill_24_		: 1;
  };
  struct  {
    unsigned io$v_fcode_fill_8		: 6;
    unsigned io$v_allhosts		: 1;
    unsigned io$v_dissolve		: 1;
    unsigned io$v_nocleanup		: 1;
    unsigned io$v_fill_25_		: 7;
  };
  struct  {
    unsigned io$v_fcode_fill_9		: 6;
    unsigned io$v_spindown		: 1;
    unsigned io$v_fill_26_		: 1;
  };
  struct  {
    unsigned io$v_fcode_fill_11		: 6;
    unsigned io$v_est_com_path		: 1;
    unsigned io$v_lcl_src_unit		: 1;
    unsigned io$v_rtn_com_path		: 1;
    unsigned io$v_fill_27_		: 7;
  };
  struct  {
    unsigned io$v_fcode_fill_12		: 6;
    unsigned io$v_dealc_all		: 1;
    unsigned io$v_dealc_hrn		: 1;
    unsigned io$v_dealc_entloc		: 1;
    unsigned io$v_decr_afc		: 1;
    unsigned io$v_read_all		: 1;
    unsigned io$v_read_hrn		: 1;
    unsigned io$v_break_conn		: 1;
    unsigned io$v_fill_28_		: 3;
  };


  struct  {
    unsigned io$v_fcode_fill_13		: 6;
    unsigned io$v_stepover		: 1;
    unsigned io$v_cs_fill		: 1;
    unsigned io$v_copyop		: 1;
    unsigned io$v_fill_29_		: 7;
  };
  struct  {
    unsigned io$v_fcode_fill_14		: 6;
    unsigned io$v_exists		: 1;
    unsigned io$v_cbs		        : 1;
    unsigned io$v_booting		: 1;
    unsigned io$v_req_minicopy		: 1;
    unsigned io$v_opt_minicopy		: 1;
    unsigned io$v_nominicopy		: 1;
    unsigned io$v_fill_30_		: 4;
  };
  struct  {
    unsigned io$v_fcode_fill_15		: 9;
    unsigned io$v_vuex_fc		: 1;
    unsigned io$v_fill_31_		: 6;
  };

  struct  {
    unsigned io$v_fcode_fill_10		: 6;

    unsigned io$v_forcepath		: 1;
    unsigned io$v_preferred_cpu		: 1;
    unsigned io$v_sys_assignable	: 1;
    unsigned io$v_fill_32_		: 7;
  };

  struct  {
    unsigned io$v_fcode_fill_16		: 6;
    unsigned io$v_commod		: 1;
    unsigned io$v_movetrackd		: 1;
    unsigned io$v_diagnostic		: 1;
    unsigned io$v_skpsecinh		: 1;
    unsigned io$v_fill_33_		: 6;
  };

  struct  {
    unsigned io$v_fcode_fill_17		: 6;
    unsigned io$v_deldata		: 1;
    unsigned io$v_fill_34_		: 1;
  };

  struct  {
    unsigned io$v_fcode_fill_18		: 6;
    unsigned io$v_nomrsp		: 1;
    unsigned io$v_fill_35_		: 1;
  };

  struct  {
    unsigned io$v_fcode_fill_19		: 6;
    unsigned io$v_rsvd_for_devdepend_5	: 2;
    unsigned io$v_swap		        : 1;
    unsigned io$v_opposite		: 1;
    unsigned io$v_fill_36_		: 6;
  };

  struct  {
    unsigned io$v_fcode_fill_20		: 6;
    unsigned io$v_rsvd_for_devdepend_6	: 3;
    unsigned io$v_clserexcp		: 1;
    unsigned io$v_fill_37_		: 6;
  };

  struct  {
    unsigned io$v_fcode_fill_21		: 6;
    unsigned io$v_chunkdiag		: 1;
    unsigned io$v_rsvd_for_devdepend_7	: 3;
    unsigned io$v_tbc		        : 1;
    unsigned io$v_fill_38_		: 5;
  };

  struct  {
    unsigned io$v_fcode_fill_22		: 6;
    unsigned io$v_enarep		: 1;
    unsigned io$v_fill_39_		: 1;
  };


  struct  {
    unsigned io$v_fcode_fill_23		: 6;
    unsigned io$v_access		: 1;
    unsigned io$v_create		: 1;
    unsigned io$v_delete		: 1;
    unsigned io$v_mount		        : 1;
    unsigned io$v_dmount		: 1;
    unsigned io$v_remount		: 1;
    unsigned io$v_movefile		: 1;
    unsigned io$v_filler_8		: 1;
    unsigned io$v_rwshelved		: 1;
    unsigned io$v_setdir		: 1;

  };

  struct  {
    unsigned io$v_fcode_fill_24		: 6;
    unsigned io$v_binary		: 1;
    unsigned io$v_packed		: 1;
  };



  struct  {
    unsigned io$v_fcode_fill_25		: 6;
    unsigned io$v_now		        : 1;
    unsigned io$v_stream		: 1;
    unsigned io$v_readercheck		: 1;
    unsigned io$v_writercheck		: 1;
    unsigned io$v_norswait		: 1;


    unsigned io$v_fill_40_		: 5;
  };


  struct  {
    unsigned io$v_fcode_fill_26		: 6;
    unsigned io$v_mb_room_notify	: 1;
    unsigned io$v_readattn		: 1;
    unsigned io$v_wrtattn		: 1;
    unsigned io$v_setprot		: 1;
    unsigned io$v_readerwait		: 1;
    unsigned io$v_writerwait		: 1;
    unsigned io$v_fill_41_		: 4;
  };

  struct  {
    unsigned io$v_fcode_fill_27		: 6;
    unsigned io$v_noecho		: 1;
    unsigned io$v_timed		        : 1;
    unsigned io$v_cvtlow		: 1;
    unsigned io$v_nofiltr		: 1;
    unsigned io$v_dsablmbx		: 1;
    unsigned io$v_purge		        : 1;
    unsigned io$v_trmnoecho		: 1;
    unsigned io$v_refresh		: 1;
    unsigned io$v_escape		: 1;
    unsigned io$v_extend		: 1;
  };

  struct  {
    unsigned io$v_fcode_fill_28		: 6;
    unsigned io$v_canctrlo		: 1;
    unsigned io$v_enablmbx		: 1;
    unsigned io$v_noformat		: 1;
    unsigned io$v_breakthru		: 1;
    unsigned io$v_newline		: 1;
    unsigned io$v_fill_42_		: 5;
  };

  struct  {
    unsigned io$v_fcode_fill_29		: 6;
    unsigned io$v_typeahdcnt		: 1;
    unsigned io$v_reserve_rd_modem	: 1;
  };

  struct  {
    unsigned io$v_fcode_fill_30		: 6;
    unsigned io$v_maint		        : 1;
    unsigned io$v_ctrlyast		: 1;
    unsigned io$v_ctrlcast		: 1;
    unsigned io$v_hangup		: 1;
    unsigned io$v_outband		: 1;
    unsigned io$v_tt_connect		: 1;
    unsigned io$v_tt_discon		: 1;
    unsigned io$v_tt_process		: 1;
    unsigned io$v_brdcst		: 1;
    unsigned io$v_fill_43_		: 1;
  };

  struct  {
    unsigned io$v_fcode_fill_31		: 6;
    unsigned io$v_reserve_maint_escape_1 : 1;
    unsigned io$v_loop		        : 1;
    unsigned io$v_unloop		: 1;
    unsigned io$v_line_off		: 1;
    unsigned io$v_set_modem		: 1;
    unsigned io$v_line_on		: 1;
    unsigned io$v_loop_ext		: 1;
    unsigned io$v_autxof_ena		: 1;
    unsigned io$v_autxof_dis		: 1;
    unsigned io$v_reserve_int_disable		: 1;
  };

  struct  {
    unsigned io$v_fcode_fill_32		: 6;
    unsigned io$v_reserve_maint_escape_2 : 5;
    unsigned io$v_include		: 1;
    unsigned io$v_tt_abort		: 1;
    unsigned io$v_fill_44_		: 3;
  };

  struct  {
    unsigned io$v_fcode_fill_33		: 6;
    unsigned io$v_posixinit		: 1;
    unsigned io$v_posixflow		: 1;
  };

  struct  {
    unsigned io$v_fcode_fill_34		: 6;
    unsigned io$v_reserve_init_escape	: 1;
    unsigned io$v_set_posix		: 1;
    unsigned io$v_clear_posix		: 1;
    unsigned io$v_set_termios		: 1;
    unsigned io$v_set_ptc		: 1;
    unsigned io$v_clear_ptc		: 1;
    unsigned io$v_flush_tab		: 1;
    unsigned io$v_flush_output		: 1;
    unsigned io$v_update_ptc		: 1;
    unsigned io$v_fill_45_		: 1;
  };

  struct  {
    unsigned io$v_fcode_fill_35		: 6;
    unsigned io$v_reserve_flow_escape	: 2;
    unsigned io$v_out_xoff		: 1;
    unsigned io$v_out_xon		: 1;
    unsigned io$v_in_xoff		: 1;
    unsigned io$v_in_xon		: 1;
    unsigned io$v_fill_46_		: 4;
  };

  struct  {
    unsigned io$v_fcode_fill_36		: 6;
    unsigned io$v_o_nonblock		: 1;
    unsigned io$v_fill_47_		: 1;
  };

  struct  {
    unsigned io$v_fcode_fill_37		: 6;
    unsigned io$v_interrupt		: 1;
    unsigned io$v_multiple		: 1;
    unsigned io$v_lockbuf		: 1;
    unsigned io$v_noblock		: 1;


    unsigned io$v_fill_48_		: 6;
  };

  struct  {
    unsigned io$v_fcode_fill_38		: 6;
    unsigned io$v_reserved		: 2;
    unsigned io$v_abort		        : 1;
    unsigned io$v_synch		        : 1;
    unsigned io$v_fill_49_		: 6;
  };

  struct  {
    unsigned io$v_fcode_fill_39		: 6;
    unsigned io$v_response		: 1;
    unsigned io$v_fill_50_		: 1;
  };

  struct  {
    unsigned io$v_fcode_fill_40		: 6;
    unsigned io$v_startup		: 1;
    unsigned io$v_shutdown		: 1;
    unsigned io$v_attnast		: 1;
    unsigned io$v_ctrl		        : 1;
    unsigned io$v_reserve_set_modem	: 1;
    unsigned io$v_set_mac		: 1;
    unsigned io$v_update_map		: 1;
    unsigned io$v_route		        : 1;
    unsigned io$v_fill_51_		: 2;
  };

  struct  {
    unsigned io$v_fcode_fill_41		: 6;
    unsigned io$v_rd_mem		: 1;
    unsigned io$v_rd_modem		: 1;
    unsigned io$v_rd_count		: 1;
    unsigned io$v_reserve_ctrl		: 1;
    unsigned io$v_clr_count		: 1;
    unsigned io$v_sense_mac		: 1;
    unsigned io$v_show_map		: 1;
    unsigned io$v_show_route		: 1;
    unsigned io$v_fill_52_		: 2;
  };
  struct  {
    unsigned io$v_fcode_fill_42		: 6;
    unsigned io$v_more		        : 1;
    unsigned io$v_qualified		: 1;
  };

  struct  {
    unsigned io$v_fcode_fill_43		: 6;
    unsigned io$v_redirect		: 1;
    unsigned io$v_accept		: 1;
  };

  struct  {
    unsigned io$v_fcode_fill_44		: 6;
    unsigned io$v_setevf		: 1;
    unsigned io$v_fill_53_		: 1;
  };

  struct  {
    unsigned io$v_fcode_fill_45		: 6;
    unsigned io$v_word		        : 1;
    unsigned io$v_filler_3		: 1;
    unsigned io$v_filler_4		: 1;
    unsigned io$v_setfnct		: 1;
    unsigned io$v_datapath		: 1;
    unsigned io$v_filler_5		: 1;
    unsigned io$v_cycle		        : 1;
    unsigned io$v_reset		        : 1;
    unsigned io$v_fill_54_		: 2;
  };

  struct  {
    unsigned io$v_fcode_fill_46		: 6;
    unsigned io$v_filler_6		: 2;
    unsigned io$v_setcuadr		: 1;
    unsigned io$v_setbsize		: 1;
    unsigned io$v_setpoolsz		: 1;
    unsigned io$v_setenqcnt		: 1;
    unsigned io$v_clear		        : 1;
    unsigned io$v_lpbext		: 1;
    unsigned io$v_lpbint		: 1;
    unsigned io$v_readcsr		: 1;
  };

  struct  {
    unsigned io$v_fcode_fill_47		: 6;
    unsigned io$v_noctswait		: 1;
    unsigned io$v_slavloop		: 1;
    unsigned io$v_nodsrwait		: 1;
    unsigned io$v_maintloop		: 1;
    unsigned io$v_lastblock		: 1;
    unsigned io$v_filler_7		: 1;
    unsigned io$v_intclock		: 1;
    unsigned io$v_fill_55_		: 3;
  };

  struct  {
    unsigned io$v_fcode_fill_48		: 6;
    unsigned io$v_lt_connect		: 1;
    unsigned io$v_lt_discon		: 1;
    unsigned io$v_lt_readport		: 1;
    unsigned io$v_lt_map_port		: 1;
    unsigned io$v_lt_rating		: 1;
    unsigned io$v_lt_sol_info		: 1;
    unsigned io$v_lt_rcv_info		: 1;
    unsigned io$v_lt_setmode		: 1;
    unsigned io$v_lt_sensemode		: 1;
    unsigned io$v_lt_que_chg_notif	: 1;
  };

  struct  {
    unsigned io$v_lt_map_filler		: 1;
    unsigned io$v_lt_map_nodnam		: 1;
    unsigned io$v_lt_map_pornam		: 1;
    unsigned io$v_lt_map_srvnam		: 1;
    unsigned io$v_lt_map_lnknam		: 1;
    unsigned io$v_lt_map_netadr		: 1;
    unsigned io$v_fill_56_		: 2;
  };

  struct  {
    unsigned io$v_fcode_fill_49		: 6;
    unsigned io$v_flush		        : 1;
    unsigned io$v_write_barrier		: 1;
  };

  struct  {
    unsigned io$v_fcode_fill_50		: 6;
    unsigned io$v_mkfill1		: 1;
    unsigned io$v_allowfast_never		: 1;
    unsigned io$v_allowfast_per_io		: 1;
    unsigned io$v_allowfast_always		: 1;
    unsigned io$v_fill_57_		        : 6;
  };

  struct  {
    unsigned io$v_fcode_fill_51		: 6;
    unsigned io$v_add		        : 1;
    unsigned io$v_fill_58_		: 1;
  };

  struct  {
    unsigned io$v_fcode_fill_52		: 6;
    unsigned io$v_idstring		: 1;
    unsigned io$v_serialnum		: 1;
  };

  struct  {
    unsigned io$v_fcode_fill_53		: 6;
    unsigned io$v_qsrvfill1		: 1;
    unsigned io$v_qsrv_client_update	: 1;
  };
};
 
#endif
