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
  } io$r_fcode_structure;

  struct  {
    unsigned io$v_fcode_fill_1		: 6;
    unsigned io$v_filler_1		: 5;
    unsigned io$v_inherlog		: 1;
    unsigned io$v_fill_20_		: 4;
  } io$r_errlog_modifiers;

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
  } io$r_disk_tape_modifiers;

  struct  {
    unsigned io$v_fcode_fill_3		: 6;
    unsigned io$v_rsvd_for_devdepend_3	: 6;
    unsigned io$v_inhseek		: 1;
    unsigned io$v_fill_22_		: 3;
  } io$r_disk_modifiers;

  struct  {
    unsigned io$v_fcode_fill_4		: 6;
    unsigned io$v_reverse		: 1;
    unsigned io$v_nowait		: 1;
    unsigned io$v_rsvd_for_devdepend_4	: 4;
    unsigned io$v_inhextgap		: 1;
    unsigned io$v_retension		: 1;

    unsigned io$v_allowfast		: 1;

    unsigned io$v_mt3_density		: 1;
  } io$r_tape_modifiers;

  struct  {
    unsigned io$v_fcode_fill_5		: 6;
    unsigned io$v_dutux_reserved	: 2;
    unsigned io$v_mscpmodifs		: 1;
    unsigned io$v_fill_23_		: 7;
  } io$r_dutu_xfer_modifiers;
  struct  {
    unsigned io$v_fcode_fill_6		: 6;
    unsigned io$v_shadow		: 1;
    unsigned io$v_locate		: 1;
  } io$r_du_sense_modifiers;
  struct  {
    unsigned io$v_fcode_fill_7		: 6;
    unsigned io$v_mscp_format		: 1;
    unsigned io$v_fill_24_		: 1;
  } io$r_du_packack_modifiers;
  struct  {
    unsigned io$v_fcode_fill_8		: 6;
    unsigned io$v_allhosts		: 1;
    unsigned io$v_dissolve		: 1;
    unsigned io$v_nocleanup		: 1;
    unsigned io$v_fill_25_		: 7;
  } io$r_du_avail_modifiers;
  struct  {
    unsigned io$v_fcode_fill_9		: 6;
    unsigned io$v_spindown		: 1;
    unsigned io$v_fill_26_		: 1;
  } io$r_du_remshad_modifiers;
  struct  {
    unsigned io$v_fcode_fill_11		: 6;
    unsigned io$v_est_com_path		: 1;
    unsigned io$v_lcl_src_unit		: 1;
    unsigned io$v_rtn_com_path		: 1;
    unsigned io$v_fill_27_		: 7;
  } io$r_du_dcd_modifiers;
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
  } io$r_du_whm_modifiers;


  struct  {
    unsigned io$v_fcode_fill_13		: 6;
    unsigned io$v_stepover		: 1;
    unsigned io$v_cs_fill		: 1;
    unsigned io$v_copyop		: 1;
    unsigned io$v_fill_29_		: 7;
  } io$r_hbs_copyshad_modifiers;
  struct  {
    unsigned io$v_fcode_fill_14		: 6;
    unsigned io$v_exists		: 1;
    unsigned io$v_cbs		        : 1;
    unsigned io$v_booting		: 1;
    unsigned io$v_req_minicopy		: 1;
    unsigned io$v_opt_minicopy		: 1;
    unsigned io$v_nominicopy		: 1;
    unsigned io$v_fill_30_		: 4;
  } io$r_hbs_creshad_modifiers;
  struct  {
    unsigned io$v_fcode_fill_15		: 9;
    unsigned io$v_vuex_fc		: 1;
    unsigned io$v_fill_31_		: 6;
  } io$r_hbs_xfer_modifiers;

  struct  {
    unsigned io$v_fcode_fill_10		: 6;

    unsigned io$v_forcepath		: 1;
    unsigned io$v_preferred_cpu		: 1;
    unsigned io$v_sys_assignable	: 1;
    unsigned io$v_fill_32_		: 7;
  } io$r_prfpath_modifiers;

  struct  {
    unsigned io$v_fcode_fill_16		: 6;
    unsigned io$v_commod		: 1;
    unsigned io$v_movetrackd		: 1;
    unsigned io$v_diagnostic		: 1;
    unsigned io$v_skpsecinh		: 1;
    unsigned io$v_fill_33_		: 6;
  } io$r_dr_modifiers;

  struct  {
    unsigned io$v_fcode_fill_17		: 6;
    unsigned io$v_deldata		: 1;
    unsigned io$v_fill_34_		: 1;
  } io$r_dy_modifiers;

  struct  {
    unsigned io$v_fcode_fill_18		: 6;
    unsigned io$v_nomrsp		: 1;
    unsigned io$v_fill_35_		: 1;
  } io$r_dd_modifiers;

  struct  {
    unsigned io$v_fcode_fill_19		: 6;
    unsigned io$v_rsvd_for_devdepend_5	: 2;
    unsigned io$v_swap		        : 1;
    unsigned io$v_opposite		: 1;
    unsigned io$v_fill_36_		: 6;
  } io$r_ts11_modifiers;

  struct  {
    unsigned io$v_fcode_fill_20		: 6;
    unsigned io$v_rsvd_for_devdepend_6	: 3;
    unsigned io$v_clserexcp		: 1;
    unsigned io$v_fill_37_		: 6;
  } io$r_tu_modifiers;

  struct  {
    unsigned io$v_fcode_fill_21		: 6;
    unsigned io$v_chunkdiag		: 1;
    unsigned io$v_rsvd_for_devdepend_7	: 3;
    unsigned io$v_tbc		        : 1;
    unsigned io$v_fill_38_		: 5;
  } io$r_tu_lg_transfer_modifiers;

  struct  {
    unsigned io$v_fcode_fill_22		: 6;
    unsigned io$v_enarep		: 1;
    unsigned io$v_fill_39_		: 1;
  } io$r_tu_setmode_modifiers;


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

  } io$r_acp_modifiers;

  struct  {
    unsigned io$v_fcode_fill_24		: 6;
    unsigned io$v_binary		: 1;
    unsigned io$v_packed		: 1;
  } io$r_cr_modifiers;



  struct  {
    unsigned io$v_fcode_fill_25		: 6;
    unsigned io$v_now		        : 1;
    unsigned io$v_stream		: 1;
    unsigned io$v_readercheck		: 1;
    unsigned io$v_writercheck		: 1;
    unsigned io$v_norswait		: 1;


    unsigned io$v_fill_40_		: 5;
  } io$r_mb_modifiers;


  struct  {
    unsigned io$v_fcode_fill_26		: 6;
    unsigned io$v_mb_room_notify	: 1;
    unsigned io$v_readattn		: 1;
    unsigned io$v_wrtattn		: 1;
    unsigned io$v_setprot		: 1;
    unsigned io$v_readerwait		: 1;
    unsigned io$v_writerwait		: 1;
    unsigned io$v_fill_41_		: 4;
  } io$r_mb_setmode_modifiers;

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
  } io$r_tt_read_modifiers;

  struct  {
    unsigned io$v_fcode_fill_28		: 6;
    unsigned io$v_canctrlo		: 1;
    unsigned io$v_enablmbx		: 1;
    unsigned io$v_noformat		: 1;
    unsigned io$v_breakthru		: 1;
    unsigned io$v_newline		: 1;
    unsigned io$v_fill_42_		: 5;
  } io$r_tt_write_modifiers;

  struct  {
    unsigned io$v_fcode_fill_29		: 6;
    unsigned io$v_typeahdcnt		: 1;
    unsigned io$v_reserve_rd_modem	: 1;
  } io$r_tt_sensemode_modifiers;

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
  } io$r_tt_setmode_modifiers;

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
  } io$r_tt_maint_modifiers;

  struct  {
    unsigned io$v_fcode_fill_32		: 6;
    unsigned io$v_reserve_maint_escape_2 : 5;
    unsigned io$v_include		: 1;
    unsigned io$v_tt_abort		: 1;
    unsigned io$v_fill_44_		: 3;
  } io$r_tt_outofband_modifiers;

  struct  {
    unsigned io$v_fcode_fill_33		: 6;
    unsigned io$v_posixinit		: 1;
    unsigned io$v_posixflow		: 1;
  } io$r_tt_psxsetmode_modifiers;

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
  } io$r_tt_posixinit_modifiers;

  struct  {
    unsigned io$v_fcode_fill_35		: 6;
    unsigned io$v_reserve_flow_escape	: 2;
    unsigned io$v_out_xoff		: 1;
    unsigned io$v_out_xon		: 1;
    unsigned io$v_in_xoff		: 1;
    unsigned io$v_in_xon		: 1;
    unsigned io$v_fill_46_		: 4;
  } io$r_tt_posixflow_modifiers;

  struct  {
    unsigned io$v_fcode_fill_36		: 6;
    unsigned io$v_o_nonblock		: 1;
    unsigned io$v_fill_47_		: 1;
  } io$r_tt_psxread_modifiers;

  struct  {
    unsigned io$v_fcode_fill_37		: 6;
    unsigned io$v_interrupt		: 1;
    unsigned io$v_multiple		: 1;
    unsigned io$v_lockbuf		: 1;
    unsigned io$v_noblock		: 1;


    unsigned io$v_fill_48_		: 6;
  } io$r_net_write_read_modifiers;

  struct  {
    unsigned io$v_fcode_fill_38		: 6;
    unsigned io$v_reserved		: 2;
    unsigned io$v_abort		        : 1;
    unsigned io$v_synch		        : 1;
    unsigned io$v_fill_49_		: 6;
  } io$r_net_accdea_modifiers;

  struct  {
    unsigned io$v_fcode_fill_39		: 6;
    unsigned io$v_response		: 1;
    unsigned io$v_fill_50_		: 1;
  } io$r_dlink_write_modifiers;

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
  } io$r_dlink_setmode_modifiers;

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
  } io$r_dlink_sensemode_modifiers;
  struct  {
    unsigned io$v_fcode_fill_42		: 6;
    unsigned io$v_more		        : 1;
    unsigned io$v_qualified		: 1;
  } io$r_x25_write_modifiers;

  struct  {
    unsigned io$v_fcode_fill_43		: 6;
    unsigned io$v_redirect		: 1;
    unsigned io$v_accept		: 1;
  } io$r_x25_access_modifiers;

  struct  {
    unsigned io$v_fcode_fill_44		: 6;
    unsigned io$v_setevf		: 1;
    unsigned io$v_fill_53_		: 1;
  } io$r_lpa_start_modifiers;

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
  } io$r_xa_modifiers;

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
  } io$r_ibm3271_modifiers;

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
  } io$r_xw_modifiers;

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
  } io$r_lt_modifiers;

  struct  {
    unsigned io$v_lt_map_filler		: 1;
    unsigned io$v_lt_map_nodnam		: 1;
    unsigned io$v_lt_map_pornam		: 1;
    unsigned io$v_lt_map_srvnam		: 1;
    unsigned io$v_lt_map_lnknam		: 1;
    unsigned io$v_lt_map_netadr		: 1;
    unsigned io$v_fill_56_		: 2;
  } io$r_lt_mapitem;

  struct  {
    unsigned io$v_fcode_fill_49		: 6;
    unsigned io$v_flush		        : 1;
    unsigned io$v_write_barrier		: 1;
  } io$r_fs_modifiers;

  struct  {
    unsigned io$v_fcode_fill_50		: 6;
    unsigned io$v_mkfill1		: 1;
    unsigned io$v_allowfast_never		: 1;
    unsigned io$v_allowfast_per_io		: 1;
    unsigned io$v_allowfast_always		: 1;
    unsigned io$v_fill_57_		        : 6;
  } io$r_mk_setmode_modifiers;

  struct  {
    unsigned io$v_fcode_fill_51		: 6;
    unsigned io$v_add		        : 1;
    unsigned io$v_fill_58_		: 1;
  } io$r_sd_modifiers;

  struct  {
    unsigned io$v_fcode_fill_52		: 6;
    unsigned io$v_idstring		: 1;
    unsigned io$v_serialnum		: 1;
  } io$r_usb_usbattr_modifiers;

  struct  {
    unsigned io$v_fcode_fill_53		: 6;
    unsigned io$v_qsrvfill1		: 1;
    unsigned io$v_qsrv_client_update	: 1;
  } io$r_qsrv_packack_modifiers;
};
 
#define io$v_fcode io$r_fcode_structure.io$v_fcode
#define io$v_fmodifiers io$r_fcode_structure.io$v_fmodifiers
#define io$v_inherlog io$r_errlog_modifiers.io$v_inherlog
#define io$v_erase io$r_disk_tape_modifiers.io$v_erase
#define io$v_exfunc io$r_disk_tape_modifiers.io$v_exfunc
#define io$v_datacheck io$r_disk_tape_modifiers.io$v_datacheck
#define io$v_inhretry io$r_disk_tape_modifiers.io$v_inhretry
#define io$v_syncsts io$r_disk_tape_modifiers.io$v_syncsts
#define io$v_novcache io$r_disk_tape_modifiers.io$v_novcache
#define io$v_bufobj io$r_disk_tape_modifiers.io$v_bufobj
#define io$v_trusted io$r_disk_tape_modifiers.io$v_trusted
#define io$v_file_flush io$r_disk_tape_modifiers.io$v_file_flush
#define io$v_barrier io$r_disk_tape_modifiers.io$v_barrier
#define io$v_inhseek io$r_disk_modifiers.io$v_inhseek
#define io$v_reverse io$r_tape_modifiers.io$v_reverse
#define io$v_nowait io$r_tape_modifiers.io$v_nowait
#define io$v_inhextgap io$r_tape_modifiers.io$v_inhextgap
#define io$v_retension io$r_tape_modifiers.io$v_retension
#define io$v_allowfast io$r_tape_modifiers.io$v_allowfast
#define io$v_mt3_density io$r_tape_modifiers.io$v_mt3_density
#define io$v_mscpmodifs io$r_dutu_xfer_modifiers.io$v_mscpmodifs
#define io$v_shadow io$r_du_sense_modifiers.io$v_shadow
#define io$v_locate io$r_du_sense_modifiers.io$v_locate
#define io$v_mscp_format io$r_du_packack_modifiers.io$v_mscp_format
#define io$v_allhosts io$r_du_avail_modifiers.io$v_allhosts
#define io$v_dissolve io$r_du_avail_modifiers.io$v_dissolve
#define io$v_nocleanup io$r_du_avail_modifiers.io$v_nocleanup
#define io$v_spindown io$r_du_remshad_modifiers.io$v_spindown
#define io$v_est_com_path io$r_du_dcd_modifiers.io$v_est_com_path
#define io$v_lcl_src_unit io$r_du_dcd_modifiers.io$v_lcl_src_unit
#define io$v_rtn_com_path io$r_du_dcd_modifiers.io$v_rtn_com_path
#define io$v_dealc_all io$r_du_whm_modifiers.io$v_dealc_all
#define io$v_dealc_hrn io$r_du_whm_modifiers.io$v_dealc_hrn
#define io$v_dealc_entloc io$r_du_whm_modifiers.io$v_dealc_entloc
#define io$v_decr_afc io$r_du_whm_modifiers.io$v_decr_afc
#define io$v_read_all io$r_du_whm_modifiers.io$v_read_all
#define io$v_read_hrn io$r_du_whm_modifiers.io$v_read_hrn
#define io$v_break_conn io$r_du_whm_modifiers.io$v_break_conn
#define io$v_stepover io$r_hbs_copyshad_modifiers.io$v_stepover
#define io$v_copyop io$r_hbs_copyshad_modifiers.io$v_copyop
#define io$v_exists io$r_hbs_creshad_modifiers.io$v_exists
#define io$v_cbs io$r_hbs_creshad_modifiers.io$v_cbs
#define io$v_booting io$r_hbs_creshad_modifiers.io$v_booting
#define io$v_req_minicopy io$r_hbs_creshad_modifiers.io$v_req_minicopy
#define io$v_opt_minicopy io$r_hbs_creshad_modifiers.io$v_opt_minicopy
#define io$v_nominicopy io$r_hbs_creshad_modifiers.io$v_nominicopy
#define io$v_vuex_fc io$r_hbs_xfer_modifiers.io$v_vuex_fc
#define io$v_forcepath io$r_prfpath_modifiers.io$v_forcepath
#define io$v_preferred_cpu io$r_prfpath_modifiers.io$v_preferred_cpu
#define io$v_sys_assignable io$r_prfpath_modifiers.io$v_sys_assignable
#define io$v_commod io$r_dr_modifiers.io$v_commod
#define io$v_movetrackd io$r_dr_modifiers.io$v_movetrackd
#define io$v_diagnostic io$r_dr_modifiers.io$v_diagnostic
#define io$v_skpsecinh io$r_dr_modifiers.io$v_skpsecinh
#define io$v_deldata io$r_dy_modifiers.io$v_deldata
#define io$v_nomrsp io$r_dd_modifiers.io$v_nomrsp
#define io$v_swap io$r_ts11_modifiers.io$v_swap
#define io$v_opposite io$r_ts11_modifiers.io$v_opposite
#define io$v_clserexcp io$r_tu_modifiers.io$v_clserexcp
#define io$v_chunkdiag io$r_tu_lg_transfer_modifiers.io$v_chunkdiag
#define io$v_tbc io$r_tu_lg_transfer_modifiers.io$v_tbc
#define io$v_enarep io$r_tu_setmode_modifiers.io$v_enarep
#define io$v_access io$r_acp_modifiers.io$v_access
#define io$v_create io$r_acp_modifiers.io$v_create
#define io$v_delete io$r_acp_modifiers.io$v_delete
#define io$v_mount io$r_acp_modifiers.io$v_mount
#define io$v_dmount io$r_acp_modifiers.io$v_dmount
#define io$v_remount io$r_acp_modifiers.io$v_remount
#define io$v_movefile io$r_acp_modifiers.io$v_movefile
#define io$v_rwshelved io$r_acp_modifiers.io$v_rwshelved
#define io$v_setdir io$r_acp_modifiers.io$v_setdir
#define io$v_binary io$r_cr_modifiers.io$v_binary
#define io$v_packed io$r_cr_modifiers.io$v_packed
#define io$v_now io$r_mb_modifiers.io$v_now
#define io$v_stream io$r_mb_modifiers.io$v_stream
#define io$v_readercheck io$r_mb_modifiers.io$v_readercheck
#define io$v_writercheck io$r_mb_modifiers.io$v_writercheck
#define io$v_norswait io$r_mb_modifiers.io$v_norswait
#define io$v_mb_room_notify io$r_mb_setmode_modifiers.io$v_mb_room_notify
#define io$v_readattn io$r_mb_setmode_modifiers.io$v_readattn
#define io$v_wrtattn io$r_mb_setmode_modifiers.io$v_wrtattn
#define io$v_setprot io$r_mb_setmode_modifiers.io$v_setprot
#define io$v_readerwait io$r_mb_setmode_modifiers.io$v_readerwait
#define io$v_writerwait io$r_mb_setmode_modifiers.io$v_writerwait
#define io$v_noecho io$r_tt_read_modifiers.io$v_noecho
#define io$v_timed io$r_tt_read_modifiers.io$v_timed
#define io$v_cvtlow io$r_tt_read_modifiers.io$v_cvtlow
#define io$v_nofiltr io$r_tt_read_modifiers.io$v_nofiltr
#define io$v_dsablmbx io$r_tt_read_modifiers.io$v_dsablmbx
#define io$v_purge io$r_tt_read_modifiers.io$v_purge
#define io$v_trmnoecho io$r_tt_read_modifiers.io$v_trmnoecho
#define io$v_refresh io$r_tt_read_modifiers.io$v_refresh
#define io$v_escape io$r_tt_read_modifiers.io$v_escape
#define io$v_extend io$r_tt_read_modifiers.io$v_extend
#define io$v_canctrlo io$r_tt_write_modifiers.io$v_canctrlo
#define io$v_enablmbx io$r_tt_write_modifiers.io$v_enablmbx
#define io$v_noformat io$r_tt_write_modifiers.io$v_noformat
#define io$v_breakthru io$r_tt_write_modifiers.io$v_breakthru
#define io$v_newline io$r_tt_write_modifiers.io$v_newline
#define io$v_typeahdcnt io$r_tt_sensemode_modifiers.io$v_typeahdcnt
#define io$v_maint io$r_tt_setmode_modifiers.io$v_maint
#define io$v_ctrlyast io$r_tt_setmode_modifiers.io$v_ctrlyast
#define io$v_ctrlcast io$r_tt_setmode_modifiers.io$v_ctrlcast
#define io$v_hangup io$r_tt_setmode_modifiers.io$v_hangup
#define io$v_outband io$r_tt_setmode_modifiers.io$v_outband
#define io$v_tt_connect io$r_tt_setmode_modifiers.io$v_tt_connect
#define io$v_tt_discon io$r_tt_setmode_modifiers.io$v_tt_discon
#define io$v_tt_process io$r_tt_setmode_modifiers.io$v_tt_process
#define io$v_brdcst io$r_tt_setmode_modifiers.io$v_brdcst
#define io$v_loop io$r_tt_maint_modifiers.io$v_loop
#define io$v_unloop io$r_tt_maint_modifiers.io$v_unloop
#define io$v_line_off io$r_tt_maint_modifiers.io$v_line_off
#define io$v_set_modem io$r_tt_maint_modifiers.io$v_set_modem
#define io$v_line_on io$r_tt_maint_modifiers.io$v_line_on
#define io$v_loop_ext io$r_tt_maint_modifiers.io$v_loop_ext
#define io$v_autxof_ena io$r_tt_maint_modifiers.io$v_autxof_ena
#define io$v_autxof_dis io$r_tt_maint_modifiers.io$v_autxof_dis
#define io$v_include io$r_tt_outofband_modifiers.io$v_include
#define io$v_tt_abort io$r_tt_outofband_modifiers.io$v_tt_abort
#define io$v_posixinit io$r_tt_psxsetmode_modifiers.io$v_posixinit
#define io$v_posixflow io$r_tt_psxsetmode_modifiers.io$v_posixflow
#define io$v_set_posix io$r_tt_posixinit_modifiers.io$v_set_posix
#define io$v_clear_posix io$r_tt_posixinit_modifiers.io$v_clear_posix
#define io$v_set_termios io$r_tt_posixinit_modifiers.io$v_set_termios
#define io$v_set_ptc io$r_tt_posixinit_modifiers.io$v_set_ptc
#define io$v_clear_ptc io$r_tt_posixinit_modifiers.io$v_clear_ptc
#define io$v_flush_tab io$r_tt_posixinit_modifiers.io$v_flush_tab
#define io$v_flush_output io$r_tt_posixinit_modifiers.io$v_flush_output
#define io$v_update_ptc io$r_tt_posixinit_modifiers.io$v_update_ptc
#define io$v_out_xoff io$r_tt_posixflow_modifiers.io$v_out_xoff
#define io$v_out_xon io$r_tt_posixflow_modifiers.io$v_out_xon
#define io$v_in_xoff io$r_tt_posixflow_modifiers.io$v_in_xoff
#define io$v_in_xon io$r_tt_posixflow_modifiers.io$v_in_xon
#define io$v_o_nonblock io$r_tt_psxread_modifiers.io$v_o_nonblock
#define io$v_interrupt io$r_net_write_read_modifiers.io$v_interrupt
#define io$v_multiple io$r_net_write_read_modifiers.io$v_multiple
#define io$v_lockbuf io$r_net_write_read_modifiers.io$v_lockbuf
#define io$v_noblock io$r_net_write_read_modifiers.io$v_noblock
#define io$v_abort io$r_net_accdea_modifiers.io$v_abort
#define io$v_synch io$r_net_accdea_modifiers.io$v_synch
#define io$v_response io$r_dlink_write_modifiers.io$v_response
#define io$v_startup io$r_dlink_setmode_modifiers.io$v_startup
#define io$v_shutdown io$r_dlink_setmode_modifiers.io$v_shutdown
#define io$v_attnast io$r_dlink_setmode_modifiers.io$v_attnast
#define io$v_ctrl io$r_dlink_setmode_modifiers.io$v_ctrl
#define io$v_set_mac io$r_dlink_setmode_modifiers.io$v_set_mac
#define io$v_update_map io$r_dlink_setmode_modifiers.io$v_update_map
#define io$v_route io$r_dlink_setmode_modifiers.io$v_route
#define io$v_rd_mem io$r_dlink_sensemode_modifiers.io$v_rd_mem
#define io$v_rd_modem io$r_dlink_sensemode_modifiers.io$v_rd_modem
#define io$v_rd_count io$r_dlink_sensemode_modifiers.io$v_rd_count
#define io$v_clr_count io$r_dlink_sensemode_modifiers.io$v_clr_count
#define io$v_sense_mac io$r_dlink_sensemode_modifiers.io$v_sense_mac
#define io$v_show_map io$r_dlink_sensemode_modifiers.io$v_show_map
#define io$v_show_route io$r_dlink_sensemode_modifiers.io$v_show_route
#define io$v_more io$r_x25_write_modifiers.io$v_more
#define io$v_qualified io$r_x25_write_modifiers.io$v_qualified
#define io$v_redirect io$r_x25_access_modifiers.io$v_redirect
#define io$v_accept io$r_x25_access_modifiers.io$v_accept
#define io$v_setevf io$r_lpa_start_modifiers.io$v_setevf
#define io$v_word io$r_xa_modifiers.io$v_word
#define io$v_setfnct io$r_xa_modifiers.io$v_setfnct
#define io$v_datapath io$r_xa_modifiers.io$v_datapath
#define io$v_cycle io$r_xa_modifiers.io$v_cycle
#define io$v_reset io$r_xa_modifiers.io$v_reset
#define io$v_setcuadr io$r_ibm3271_modifiers.io$v_setcuadr
#define io$v_setbsize io$r_ibm3271_modifiers.io$v_setbsize
#define io$v_setpoolsz io$r_ibm3271_modifiers.io$v_setpoolsz
#define io$v_setenqcnt io$r_ibm3271_modifiers.io$v_setenqcnt
#define io$v_clear io$r_ibm3271_modifiers.io$v_clear
#define io$v_lpbext io$r_ibm3271_modifiers.io$v_lpbext
#define io$v_lpbint io$r_ibm3271_modifiers.io$v_lpbint
#define io$v_readcsr io$r_ibm3271_modifiers.io$v_readcsr
#define io$v_noctswait io$r_xw_modifiers.io$v_noctswait
#define io$v_slavloop io$r_xw_modifiers.io$v_slavloop
#define io$v_nodsrwait io$r_xw_modifiers.io$v_nodsrwait
#define io$v_maintloop io$r_xw_modifiers.io$v_maintloop
#define io$v_lastblock io$r_xw_modifiers.io$v_lastblock
#define io$v_intclock io$r_xw_modifiers.io$v_intclock
#define io$v_lt_connect io$r_lt_modifiers.io$v_lt_connect
#define io$v_lt_discon io$r_lt_modifiers.io$v_lt_discon
#define io$v_lt_readport io$r_lt_modifiers.io$v_lt_readport
#define io$v_lt_map_port io$r_lt_modifiers.io$v_lt_map_port
#define io$v_lt_rating io$r_lt_modifiers.io$v_lt_rating
#define io$v_lt_sol_info io$r_lt_modifiers.io$v_lt_sol_info
#define io$v_lt_rcv_info io$r_lt_modifiers.io$v_lt_rcv_info
#define io$v_lt_setmode io$r_lt_modifiers.io$v_lt_setmode
#define io$v_lt_sensemode io$r_lt_modifiers.io$v_lt_sensemode
#define io$v_lt_que_chg_notif io$r_lt_modifiers.io$v_lt_que_chg_notif
#define io$v_lt_map_filler io$r_lt_mapitem.io$v_lt_map_filler
#define io$v_lt_map_nodnam io$r_lt_mapitem.io$v_lt_map_nodnam
#define io$v_lt_map_pornam io$r_lt_mapitem.io$v_lt_map_pornam
#define io$v_lt_map_srvnam io$r_lt_mapitem.io$v_lt_map_srvnam
#define io$v_lt_map_lnknam io$r_lt_mapitem.io$v_lt_map_lnknam
#define io$v_lt_map_netadr io$r_lt_mapitem.io$v_lt_map_netadr
#define io$v_flush io$r_fs_modifiers.io$v_flush
#define io$v_write_barrier io$r_fs_modifiers.io$v_write_barrier
#define io$v_mkfill1 io$r_mk_setmode_modifiers.io$v_mkfill1
#define io$v_allowfast_never io$r_mk_setmode_modifiers.io$v_allowfast_never
#define io$v_allowfast_per_io io$r_mk_setmode_modifiers.io$v_allowfast_per_io
#define io$v_allowfast_always io$r_mk_setmode_modifiers.io$v_allowfast_always
#define io$v_add io$r_sd_modifiers.io$v_add
#define io$v_idstring io$r_usb_usbattr_modifiers.io$v_idstring
#define io$v_serialnum io$r_usb_usbattr_modifiers.io$v_serialnum
#define io$v_qsrvfill1 io$r_qsrv_packack_modifiers.io$v_qsrvfill1
#define io$v_qsrv_client_update io$r_qsrv_packack_modifiers.io$v_qsrv_client_update
 
#endif
