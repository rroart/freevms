#ifndef mscpdef_h
#define mscpdef_h

#define MSCP$M_EU_NO 0xFF
#define MSCP$M_EU_SUBU 0x7
#define MSCP$M_EU_SUBC 0xF8
#define MSCP$K_EMS_CNSL 0
#define MSCP$K_EMS_RP 1
#define MSCP$K_EMS_RM 2
#define MSCP$K_EMS_RK 3
#define MSCP$K_EMS_RL 4
#define MSCP$K_EMS_RX 5
#define MSCP$K_EMS_FD1 6
#define MSCP$K_EMS_FD2 7
#define MSCP$K_EMS_FD3 8
#define MSCP$K_EMS_FD4 9
#define MSCP$K_EMS_FD5 10
#define MSCP$K_EMS_FD6 11
#define MSCP$K_EMS_FD7 12
#define MSCP$K_EMS_FD8 13
#define MSCP$M_EU_CTYPE 0xF00
#define MSCP$K_EMD_OLD 0
#define MSCP$K_EMD_UDA 1
#define MSCP$K_EMD_HSC 2
#define MSCP$K_EMD_AZT 3
#define MSCP$K_EMD_RDRX 4
#define MSCP$K_EMD_EMUL 5
#define MSCP$M_EU_DESIG 0x7000
#define MSCP$M_SHADOW 0x8000
#define MSCP$M_OP_ATTN 0x40
#define MSCP$M_OP_END 0x80
#define MSCP$M_MD_SEREC 0x100
#define MSCP$M_MD_SECOR 0x200
#define MSCP$M_MD_CLSEX 0x2000
#define MSCP$M_MD_COMP 0x4000
#define MSCP$M_MD_WRSEQ 0x10
#define MSCP$M_MD_WBKVL 0x20
#define MSCP$M_MD_WBKNV 0x40
#define MSCP$M_MD_SSHDW 0x80
#define MSCP$M_MD_SCCHL 0x400
#define MSCP$M_MD_SCCHH 0x800
#define MSCP$M_MD_ERROR 0x1000
#define MSCP$M_MD_EXPRS 0x8000
#define MSCP$M_MD_REWND 0x2
#define MSCP$M_MD_OBJCT 0x4
#define MSCP$M_MD_REVRS 0x8
#define MSCP$M_MD_UNLOD 0x10
#define MSCP$M_MD_IMMED 0x40
#define MSCP$M_MD_DLEOT 0x80
#define MSCP$M_MD_CDATL 0x1000
#define MSCP$M_MD_SPNDW 0x1
#define MSCP$M_MD_ALLCD 0x2
#define MSCP$M_MD_DSOLV 0x10
#define MSCP$M_MD_FLENU 0x1
#define MSCP$M_MD_VOLTL 0x2
#define MSCP$M_MD_NXUNT 0x1
#define MSCP$M_MD_RIP 0x1
#define MSCP$M_MD_IGNMF 0x2
#define MSCP$M_MD_STWRP 0x4
#define MSCP$M_MD_CLWBL 0x8
#define MSCP$M_MD_SHDSP 0x10
#define MSCP$M_MD_EXCAC 0x20
#define MSCP$M_MD_PRIMR 0x1
#define MSCP$M_MD_CRNPR 0x1
#define MSCP$M_MD_ENRWR 0x10
#define MSCP$M_MD_TBC 0x8000
#define MSCP$M_MD_HISLO 0x8
#define MSCP$M_MD_SUPWL 0x10
#define MSCP$M_MD_REUSE 0x80
#define MSCP$M_MD_LOCSU 0x1
#define MSCP$M_MD_ESTCP 0x2
#define MSCP$M_MD_RETCP 0x4
#define MSCP$S_MSCP_MODIFIERS 12

#define MSCP$M_EF_CPRET 0x1
#define MSCP$M_EF_DLS 0x2
#define MSCP$M_EF_PLS 0x4
#define MSCP$M_EF_EOT 0x8
#define MSCP$M_EF_SEREX 0x10
#define MSCP$M_EF_ERLOG 0x20
#define MSCP$M_EF_BBLKU 0x40
#define MSCP$M_EF_BBLKR 0x80
#define MSCP$M_EF_HISLO 0x4
#define MSCP$M_EF_ALLOF 0x8
#define MSCP$M_EF_FASTSKIP_USED 0x80
#define MSCP$M_ST_MASK 0x1F
#define MSCP$K_ST_SUCC 0
#define MSCP$K_ST_ICMD 1
#define MSCP$K_ST_ABRTD 2
#define MSCP$K_ST_OFFLN 3
#define MSCP$K_ST_AVLBL 4
#define MSCP$K_ST_MFMTE 5
#define MSCP$K_ST_WRTPR 6
#define MSCP$K_ST_COMP 7
#define MSCP$K_ST_DATA 8
#define MSCP$K_ST_HSTBF 9
#define MSCP$K_ST_CNTLR 10
#define MSCP$K_ST_DRIVE 11
#define MSCP$K_ST_FMTER 12
#define MSCP$K_ST_BOT 13
#define MSCP$K_ST_TAPEM 14
#define MSCP$K_ST_SHST 12
#define MSCP$K_ST_WHEAE 13
#define MSCP$K_ST_RDTRN 16
#define MSCP$K_ST_PLOST 17
#define MSCP$K_ST_PRESE 18
#define MSCP$K_ST_LED 19
#define MSCP$K_ST_BBR 20
#define MSCP$K_ST_IPARM 21
#define MSCP$K_ST_INFO 22
#define MSCP$K_ST_LOADR 23
#define MSCP$K_ST_HOST 24
#define MSCP$K_ST_UNREC 20
#define MSCP$K_ST_SBCERR 30
#define MSCP$K_ST_DIAG 31
#define MSCP$K_ST_SBCOD 32
#define MSCP$M_ST_SBCOD 0xFFE0
#define MSCP$K_SC_NORML 0
#define MSCP$K_SC_SDIGN 1
#define MSCP$K_SC_STCON 2
#define MSCP$K_SC_DUPUN 4
#define MSCP$K_SC_ALONL 8
#define MSCP$K_SC_STONL 16
#define MSCP$K_SC_EOT 32
#define MSCP$K_SC_INREP 32
#define MSCP$K_SC_IVRCT 64
#define MSCP$K_SC_RDONY 128
#define MSCP$M_SC_SDIGN 0x20
#define MSCP$M_SC_STCON 0x40
#define MSCP$M_SC_DUPUN 0x80
#define MSCP$M_SC_ALONL 0x100
#define MSCP$M_SC_STONL 0x200
#define MSCP$M_SC_EOT 0x400
#define MSCP$M_SC_INREP 0x400
#define MSCP$M_SC_IVRCT 0x800
#define MSCP$M_SC_RDONY 0x1000
#define MSCP$K_SC_INVML 0

#define MSCP$K_SC_UNKNO 0
#define MSCP$K_SC_NOVOL 1
#define MSCP$K_SC_INOPR 2
#define MSCP$K_SC_UDSBL 8
#define MSCP$K_SC_EXUSE 16
#define MSCP$K_SC_LDR 32
#define MSCP$M_SC_NOVOL 0x20
#define MSCP$M_SC_INOPR 0x40
#define MSCP$M_SC_UDSBL 0x100
#define MSCP$M_SC_EXUSE 0x200
#define MSCP$M_SC_LDR 0x400
#define MSCP$K_SC_CPYIP 2
#define MSCP$K_SC_NOMEM 4
#define MSCP$K_SC_ALUSE 32
#define MSCP$M_SC_CPYIP 0x40
#define MSCP$M_SC_NOMEM 0x80
#define MSCP$M_SC_ALUSE 0x400
#define MSCP$K_SC_IVHD 2
#define MSCP$K_SC_NOSYNC 3
#define MSCP$K_SC_SSCM 4
#define MSCP$K_SC_NO512 5
#define MSCP$K_SC_NOFMT 6
#define MSCP$K_SC_ECCER 7
#define MSCP$K_SC_RCTBD 8
#define MSCP$K_SC_NORBL 9

#define MSCP$K_SC_DATAL 8
#define MSCP$K_SC_SOFTW 128
#define MSCP$K_SC_HARDW 256
#define MSCP$M_SC_DATAL 0x100
#define MSCP$M_SC_SOFTW 0x1000
#define MSCP$M_SC_HARDW 0x2000
#define MSCP$K_SC_FRCER 0
#define MSCP$K_SC_LGE 0
#define MSCP$K_SC_MEDIA 3

#define MSCP$K_SC_ODDTA 1
#define MSCP$K_SC_ODDBC 2
#define MSCP$K_SC_NXM 3
#define MSCP$K_SC_MPAR 4
#define MSCP$K_SC_IVPTE 5
#define MSCP$K_SC_IVBFN 6
#define MSCP$K_SC_BLENV 7
#define MSCP$K_SC_ACVIO 8

#define MSCP$K_SC_DLATE 1
#define MSCP$K_SC_EDCER 2
#define MSCP$K_SC_DTSTR 3
#define MSCP$K_SC_IEDC 4
#define MSCP$K_SC_LACIN 5
#define MSCP$K_SC_LACOU 6
#define MSCP$K_SC_LACCB 7
#define MSCP$K_SC_OVRUN 8
#define MSCP$K_SC_MEMER 9
#define MSCP$K_SC_REMRSRC 10
#define MSCP$K_SC_RCONL 20
#define MSCP$K_SC_RCONF 21
#define MSCP$K_SC_BADSA 22
#define MSCP$K_SC_NOSER 23
#define MSCP$K_SC_NORES 24
#define MSCP$K_SC_NOCRE 25
#define MSCP$K_SC_BADPR 26
#define MSCP$K_SC_NEGAK 27
#define MSCP$K_SC_TMOUT 28
#define MSCP$K_SC_LCONF 29
#define MSCP$K_SC_DISCN 30

#define MSCP$K_SC_BBROK 0
#define MSCP$K_SC_NOTRP 1
#define MSCP$K_SC_RPLFL 2
#define MSCP$K_SC_ICRCT 3
#define MSCP$K_SC_DRVER 4
#define MSCP$K_SC_RCTFULL 5
#define MSCP$K_SC_RECURFAIL 6

#define MSCP$K_SC_NOTAPEFMT 1

#define MSCP$K_SC_IVKLN 1
#define MSCP$K_SC_IVKTY 2
#define MSCP$K_SC_IVKVL 3

#define MSCP$K_SC_ML_TMO 1
#define MSCP$K_SC_ML_TXERR 2
#define MSCP$K_SC_ML_PRTCL 3
#define MSCP$K_SC_ML_ERROR 4

#define MSCP$K_SC_SEGUNDR 1

#define MSCP$K_SC_ALLOF 1
#define MSCP$K_SC_TABFU 2
#define MSCP$K_SC_NOENT 8
#define MSCP$M_SC_ALLOF 0x2
#define MSCP$M_SC_TABFU 0x4
#define MSCP$M_SC_NOENT 0x10
#define MSCP$K_SC_DCDC 3

#define MSCP$K_SC_DST_TIMOUT 1
#define MSCP$K_SC_DST_INCSTA 2
#define MSCP$K_SC_DST_UNRCOV 4
#define MSCP$K_SC_SRC_TIMOUT 1025
#define MSCP$K_SC_SRC_INCSTA 1026
#define MSCP$K_SC_SRC_UNRCOV 1028
#define MSCP$S_MSCP_SUBCODES 12
	
struct _mscp_basic_pkt {
  /* SSP packages has something here, but we do not need it */
  unsigned int mscp$l_cmd_ref;
  unsigned short int mscp$w_unit;
  unsigned short int mscp$w_seq_num;
  unsigned char mscp$b_opcode;
  unsigned char mscp$b_caa;
  unsigned short int mscp$w_modifier;
    struct  {
      union  {
	union  {
	  unsigned char mscp$b_flags;
	  struct  {
	    unsigned mscp$v_ef_cpret : 1;
	    unsigned mscp$v_ef_dls : 1;
	    unsigned mscp$v_ef_pls : 1;
	    unsigned mscp$v_ef_eot : 1;
	    unsigned mscp$v_ef_serex : 1;
	    unsigned mscp$v_ef_erlog : 1;
	    unsigned mscp$v_ef_bblku : 1;
	    unsigned mscp$v_ef_bblkr : 1;
	  } mscp$r_fill_5_;
	} mscp$r_fill_4_;
	union  {
	  unsigned char mscp$b_flags1;
	  struct  {
	    unsigned mscp$v_filler1 : 2;
	    unsigned mscp$v_ef_hislo : 1;
	    unsigned mscp$v_ef_allof : 1;
	    unsigned mscp$v_filler2 : 3;
	    unsigned mscp$v_ef_fastskip_used : 1;
	  } mscp$r_fill_7_;
	} mscp$r_fill_6_;
      } mscp$r_flags_overlay;
      union  {
	union  {
	  unsigned short int mscp$w_status;
	  struct  {
	    unsigned mscp$v_st_mask : 5;
	    unsigned mscp$v_st_sbcod : 11;
	  } mscp$r_fill_9_;
	} mscp$r_fill_8_;

	struct  {
	  union  {
	    struct  {
	      unsigned mscp$v_filler : 5;
	      unsigned mscp$v_sc_sdign : 1;
	      unsigned mscp$v_sc_stcon : 1;
	      unsigned mscp$v_sc_dupun : 1;
	      unsigned mscp$v_sc_alonl : 1;
	      unsigned mscp$v_sc_stonl : 1;
	      unsigned mscp$v_sc_eot : 1;
	      unsigned mscp$v_fill_20_ : 5;
	    } mscp$r_fields_1;
	    struct  {
	      unsigned mscp$v_filler : 10;
	      unsigned mscp$v_sc_inrep : 1;
	      unsigned mscp$v_sc_ivrct : 1;
	      unsigned mscp$v_sc_rdony : 1;
	      unsigned mscp$v_fill_21_ : 3;
	    } mscp$r_field_2;
	  } mscp$r_bit_fields;
	} mscp$r_sc_succ;

	struct  {
	  unsigned mscp$v_filler : 5;
	  unsigned mscp$v_sc_novol : 1;
	  unsigned mscp$v_sc_inopr : 1;
	  unsigned mscp$v_dupun : 1;
	  unsigned mscp$v_sc_udsbl : 1;
	  unsigned mscp$v_sc_exuse : 1;
	  unsigned mscp$v_sc_ldr : 1;
	  unsigned mscp$v_fill_22_ : 5;
	} mscp$r_sc_offln;

	struct  {
	  unsigned mscp$v_filler : 5;
	  unsigned mscp$v_reserved1 : 1;
	  unsigned mscp$v_sc_cpyip : 1;
	  unsigned mscp$v_sc_nomem : 1;
	  unsigned mscp$v_reserved2 : 2;
	  unsigned mscp$v_sc_aluse : 1;
	  unsigned mscp$v_fill_23_ : 5;
	} mscp$r_sc_avlbl;

	struct  {
	  unsigned mscp$v_filler1 : 8;
	  unsigned mscp$v_sc_datal : 1;
	  unsigned mscp$v_filler2 : 3;
	  unsigned mscp$v_sc_softw : 1;
	  unsigned mscp$v_sc_hardw : 1;
	  unsigned mscp$v_fill_24_ : 2;
	} mscp$r_sc_wrtpr;

	struct  {
	  struct  {
	    unsigned mscp$v_filler1 : 1;
	    unsigned mscp$v_sc_allof : 1;
	    unsigned mscp$v_sc_tabfu : 1;
	    unsigned mscp$v_filler2 : 1;
	    unsigned mscp$v_sc_noent : 1;
	    unsigned mscp$v_fill_25_ : 3;
	  } mscp$r_fields;
	} mscp$r_sc_wheae;

      } mscp$r_mscp_subcodes;
    } mscp$r_flags_status;
  } mscp$r_modifiers_status;
};

/* these are numbered like this in NetBSD too */  
#define MSCP$K_OP_ABORT 1
#define MSCP$K_OP_ACCES 16
#define MSCP$K_OP_ACCNM 5
#define MSCP$K_OP_AVAIL 8
#define MSCP$K_OP_CMPCD 17
#define MSCP$K_OP_COMP 32
#define MSCP$K_OP_DSPLY 6
#define MSCP$K_OP_DTACP 11
#define MSCP$K_OP_DCD 13
#define MSCP$K_OP_ERASE 18
#define MSCP$K_OP_ERGAP 22
#define MSCP$K_OP_FLUSH 19
#define MSCP$K_OP_FMT 24
#define MSCP$K_OP_GTCMD 2
#define MSCP$K_OP_GTUNM 7
#define MSCP$K_OP_GTUNT 3
#define MSCP$K_OP_GTLDR 7
#define MSCP$K_OP_MOVE 12
#define MSCP$K_OP_ONLIN 9
#define MSCP$K_OP_RCEDC 35
#define MSCP$K_OP_READ 33
#define MSCP$K_OP_REPLC 20
#define MSCP$K_OP_REPOS 37
#define MSCP$K_OP_STCON 4
#define MSCP$K_OP_STUNT 10
#define MSCP$K_OP_TERCO 48
#define MSCP$K_OP_WRITE 34
#define MSCP$K_OP_WRITM 36
#define MSCP$K_OP_WRHIM 25

#define MSCP$K_OP_END 128

#define MSCP$K_OP_ACPTH 66
#define MSCP$K_OP_AVATN 64
#define MSCP$K_OP_DUPUN 65
#define MSCP$M_SLUN 0x4000
#define MSCP$K_SLUN_RSVP 32767
	
struct _generic_mscp {
  struct _mscp_basic_pkt mscp$r_mscp_basic_pkt;
  struct  {
    unsigned mscp$v_slun_unused : 14;
    unsigned mscp$v_slun : 1;
    unsigned mscp$v_fill_26_ : 1;
  } mscp$r_slun_unit_bit;
};
 
#define MSCP$K_FMT_DFLT 0
#define MSCP$K_FMT_SING 1
#define MSCP$K_FMT_DOUB 2
#define MSCP$K_FMT_RX33 282
	
struct _transfer_commands {
  struct _mscp_basic_pkt mscp$r_mscp_basic_pkt;
  unsigned int mscp$l_byte_cnt;
  unsigned char mscp$b_buffer [12];
  union  {
    struct  {
      union  {
	unsigned int mscp$l_lbn;
	unsigned int mscp$l_frst_bad;
	unsigned int mscp$l_fmt_func;
      } mscp$r_lbn_longword;
      unsigned int mscp$l_key_id;
      unsigned int mscp$l_key_lgh;
      unsigned int mscp$l_key_buf [3];
    } mscp$r_disk;
    struct  {
      unsigned int mscp$l_position;
      unsigned int mscp$l_taperec;
    } mscp$r_tape;
  } mscp$r_disk_tape;
} ;
 
struct _abort_gtcmd {
  struct _mscp_basic_pkt mscp$r_mscp_basic_pkt;
  unsigned int mscp$l_out_ref;
  unsigned int mscp$l_cmd_sts;
} ;

#define MSCP$K_ANM_READ 0
#define MSCP$K_ANM_EXCG 1
#define MSCP$K_ANM_TSST 2
	
struct _accnm {
  struct _mscp_basic_pkt mscp$r_mscp_basic_pkt;
  unsigned int mscp$l_anm_size;
  unsigned int mscp$l_anm_offs;
  unsigned short int mscp$w_anm_oper;
  unsigned short int mscp$w_anm_dlgh;
  char mscp$t_anm_memd [32];
} ;

	
 struct _display_cmd {
  struct _mscp_basic_pkt mscp$r_mscp_basic_pkt;
  unsigned short int mscp$w_ditem;
  unsigned short int mscp$w_dmode;
  char mscp$t_dtext [16];
} ;

#define MSCP$M_UF_CMPRD 0x1
#define MSCP$M_UF_CMPWR 0x2
#define MSCP$M_UF_WBKNV 0x40
#define MSCP$M_UF_WRTPD 0x100
#define MSCP$M_UF_EXACC 0x400
#define MSCP$M_UF_SCCHH 0x800
#define MSCP$M_UF_WRTPS 0x1000
#define MSCP$M_UF_WRTPH 0x2000
#define MSCP$M_UF_576 0x4
#define MSCP$M_UF_WHL 0x8
#define MSCP$M_UF_RMVBL 0x80
#define MSCP$M_UF_SSMST 0x200
#define MSCP$M_UF_SSMEM 0x4000
#define MSCP$M_UF_REPLC 0x8000
#define MSCP$M_UF_CACFL 0x4
#define MSCP$M_UF_EWRER 0x8
#define MSCP$M_UF_VARSP 0x10
#define MSCP$M_UF_VSMSU 0x20
#define MSCP$M_UF_LOADR 0x200
#define MSCP$M_UF_CACH 0x8000
#define MSCP$M_MTD_ENH 0x1
#define MSCP$M_COMP_SUP 0x2
#define MSCP$M_COMP_ENA 0x4
#define MSCP$M_SLUN_C 0x1F
#define MSCP$M_SLUN_D1 0x3E0
#define MSCP$M_SLUN_D0 0x7C00
#define MSCP$M_MTYP_N 0x7F
#define MSCP$M_MTYP_A2 0xF80
#define MSCP$M_MTYP_A1 0x1F000
#define MSCP$M_MTYP_A0 0x3E0000
#define MSCP$M_MTYP_D1 0x7C00000
#define MSCP$M_MTYP_D0 0xF8000000
#define MSCP$K_CM_NOCPY 0
#define MSCP$K_CM_COPY 1
#define MSCP$K_CM_MGCPY 2
#define MSCP$K_CM_MMRG 3
#define MSCP$M_TF_800 0x1
#define MSCP$M_TF_PE 0x2
#define MSCP$M_TF_GCR 0x4
#define MSCP$M_TF_BLK 0x8
#define MSCP$M_TF_NOR 0x1
#define MSCP$M_TF_BHD 0x2
#define MSCP$M_TF_DN2 0x4
#define MSCP$M_TF_DN3 0x8
#define MSCP$M_TF_T87 0x10
#define MSCP$M_TF_T87C 0x20
#define MSCP$M_TF_ENH 0x2
#define MSCP$M_TF_NDC 0x4
#define MSCP$M_TF_EDC 0x8
#define MSCP$M_TF_DCP 0x2
#define MSCP$M_TF_X82 0x1
#define MSCP$M_TF_X85 0x2
#define MSCP$M_TF_X5C 0x4
#define MSCP$K_TC_OLD 0
#define MSCP$K_TC_9TR 256
#define MSCP$K_TC_CTP 512
#define MSCP$K_TC_HPC 768
#define MSCP$K_TC_WOD 1024
#define MSCP$K_TC_DAT 1280
#define MSCP$K_TC_8MM 1536
#define MSCP$K_TC_QIC 1792
#define MSCP$M_TF_MASK 255
#define MSCP$K_TF_CODE 256
#define MSCP$S_ONLIN_STUNT 44

	
 struct _gtunt {
  struct _mscp_basic_pkt mscp$r_mscp_basic_pkt;
  unsigned short int mscp$w_mult_unt;
unsigned short int mscp$w_unt_flgs;
unsigned short int mscp$w_tape_char;
  short int mscp$w_reserved;
  union  {
    union  {
      unsigned int64 mscp$q_unit_id;
      struct  {
	void *mscp$l_excl_lba;
	unsigned int mscp$l_excl_lbc;
      } mscp$r_fill_30_;
    } mscp$r_fill_29_;
    union  {
      int64 mscp$q_slun_overlay;
      struct  {
	unsigned int mscp$l_slun_allocls;
	unsigned short int mscp$w_slun_unit;
	union  {
	  unsigned short int mscp$w_slun_devname;
	  struct  {
	    unsigned mscp$v_slun_c : 5;
	    unsigned mscp$v_slun_d1 : 5;
	    unsigned mscp$v_slun_d0 : 5;
	    unsigned mscp$v_reserved : 1;
	  } mscp$r_fill_34_;
	} mscp$r_fill_33_;
      } mscp$r_fill_32_;
    } mscp$r_fill_31_;
  } mscp$r_unit_id_overlay;
  union  {
    unsigned int mscp$l_dev_parm;
    union  {
      unsigned int mscp$l_media_id;
      struct  {
	unsigned mscp$v_mtyp_n : 7;
	unsigned mscp$v_mtyp_a2 : 5;
	unsigned mscp$v_mtyp_a1 : 5;
	unsigned mscp$v_mtyp_a0 : 5;
	unsigned mscp$v_mtyp_d1 : 5;
	unsigned mscp$v_mtyp_d0 : 5;
      } mscp$r_fill_36_;
    } mscp$r_fill_35_;
  } mscp$r_dev_parm_overlay;
  union  {
    struct  {
      unsigned short int mscp$w_shdw_unt;
      union  {
	unsigned short int mscp$w_copy_mod;
	unsigned short int mscp$w_shdw_sts;
      } mscp$r_spd_sts;
    } mscp$r_disk_cmd;
    struct  {
      union  {
	union  {
	  unsigned short int mscp$w_format;
	  struct  {
	    unsigned mscp$v_tf_800 : 1;
	    unsigned mscp$v_tf_pe : 1;
	    unsigned mscp$v_tf_gcr : 1;
	    unsigned mscp$v_tf_blk : 1;
	    unsigned mscp$v_fill_49_ : 4;
	  } mscp$r_fill_38_;
	} mscp$r_fill_37_;
	union  {
	  unsigned short int mscp$w_format_ctp;
	  struct  {
	    unsigned mscp$v_tf_nor : 1;
	    unsigned mscp$v_tf_bhd : 1;
	    unsigned mscp$v_tf_dn2 : 1;
	    unsigned mscp$v_tf_dn3 : 1;
	    unsigned mscp$v_tf_t87 : 1;
	    unsigned mscp$v_tf_t87c : 1;
	    unsigned mscp$v_fill_50_ : 2;
	  } mscp$r_fill_40_;
	} mscp$r_fill_39_;
	union  {
	  unsigned short int mscp$w_format_hpc;
	  struct  {
	    unsigned mscp$v_fill_10 : 1;
	    unsigned mscp$v_tf_enh : 1;
	    unsigned mscp$v_tf_ndc : 1;
	    unsigned mscp$v_tf_edc : 1;
	    unsigned mscp$v_fill_51_ : 4;
	  } mscp$r_fill_42_;
	} mscp$r_fill_41_;
	union  {
	  unsigned short int mscp$w_format_dat;
	  struct  {
	    unsigned mscp$v_fill_11 : 1;
	    unsigned mscp$v_tf_dcp : 1;
	    unsigned mscp$v_fill_52_ : 6;
	  } mscp$r_fill_44_;
	} mscp$r_fill_43_;
	union  {
	  unsigned short int mscp$w_format_8mm;
	  struct  {
	    unsigned mscp$v_tf_x82 : 1;
	    unsigned mscp$v_tf_x85 : 1;
	    unsigned mscp$v_tf_x5c : 1;
	    unsigned mscp$v_fill_53_ : 5;
	  } mscp$r_fill_46_;
	} mscp$r_fill_45_;
      } mscp$r_format_overlay;
      unsigned short int mscp$w_speed;
    } mscp$r_tape_cmd;
  } mscp$r_disk_tape_cmd;
  union  {
    union  {
      struct  {
	unsigned short int mscp$w_track;
	unsigned short int mscp$w_group;
	unsigned short int mscp$w_cylinder;
	unsigned char mscp$b_unit_svr;
	unsigned char mscp$b_unit_hvr;
	unsigned short int mscp$w_rct_size;
	unsigned char mscp$b_rbns;
	unsigned char mscp$b_rct_cpys;
	unsigned short int mscp$w_load_avail;
      } mscp$r_disk_end;
      struct  {
	unsigned short int mscp$w_formenu;
	unsigned char mscp$b_freecap;
      } mscp$r_tape_end;
    } mscp$r_disk_tape_end;

    union  {
      struct  {
	unsigned int mscp$l_unt_size;
	unsigned int mscp$l_vol_ser;
      } mscp$r_disk;
      struct  {
	unsigned int mscp$l_maxwtrec;
	unsigned short int mscp$w_noiserec;
      } mscp$r_tape;
    } mscp$r_disk_tape;

  } mscp$r_online_stunt;
};
 
 struct _rcedc {
  struct _mscp_basic_pkt mscp$r_mscp_basic_pkt;
  int mscp$l_filler [5];
  unsigned int mscp$l_code;
} ;

	
 struct _replc {
  struct _mscp_basic_pkt mscp$r_mscp_basic_pkt;
  unsigned int mscp$l_rbn;
};

	
 struct _repos {
  struct _mscp_basic_pkt mscp$r_mscp_basic_pkt;
  union  {
    struct  {
      unsigned int mscp$l_rec_cnt;
      unsigned int mscp$l_tmgp_cnt;
    } mscp$r_cmd;
    struct  {
      unsigned int mscp$l_rcskiped;
      unsigned int mscp$l_tmskiped;
    } mscp$r_endmsg;
  } mscp$r_cmdend;
} REPOS;

#define MSCP$M_CF_576 0x1
#define MSCP$M_CF_SHADW 0x2
#define MSCP$M_CF_MLTHS 0x4
#define MSCP$M_CF_LDCD 0x8
#define MSCP$M_CF_THIS 0x10
#define MSCP$M_CF_OTHER 0x20
#define MSCP$M_CF_MISC 0x40
#define MSCP$M_CF_ATTN 0x80
#define MSCP$M_CF_RDCD 0x100
#define MSCP$M_CF_WHL 0x200
#define MSCP$M_CF_RDO 0x400
#define MSCP$M_CF_NFESC 0x800
#define MSCP$M_CF_LOAD 0x2000
#define MSCP$M_CF_EDCRP 0x4000
#define MSCP$M_CF_REPLC 0x8000
#define MSCP$M_CF_MTDEN 0x4000
#define MSCP$M_CF_SRT 0x8000
#define MSCP$K_CL_CNTRL 1
#define MSCP$K_CL_DISK 2
#define MSCP$K_CL_TAPE 3
#define MSCP$K_CL_D144 4
#define MSCP$K_CL_LDR 5

#define MSCP$K_CM_HSC50 1
#define MSCP$K_CM_UDA50 2
#define MSCP$K_CM_RC25 3
#define MSCP$K_CM_EMULA 4
#define MSCP$K_CM_TU81 5
#define MSCP$K_CM_UDA52 6
#define MSCP$K_CM_UDA50A 6
#define MSCP$K_CM_RDRX 7
#define MSCP$K_CM_TOPS 8
#define MSCP$K_CM_TK50 9
#define MSCP$K_CM_TQK50 9
#define MSCP$K_CM_RUX50 10
#define MSCP$K_CM_AIO 12
#define MSCP$K_CM_KFBTA 12
#define MSCP$K_CM_KDA50 13
#define MSCP$K_CM_TK70 14
#define MSCP$K_CM_TQK70 14
#define MSCP$K_CM_RV20 15
#define MSCP$K_CM_RRD50 16
#define MSCP$K_CM_RRD50Q 16
#define MSCP$K_CM_KDB50 18
#define MSCP$K_CM_RQDX3 19
#define MSCP$K_CM_RQDX4 20
#define MSCP$K_CM_DSSI_DISK 21
#define MSCP$K_CM_DSSI_TAPE 22
#define MSCP$K_CM_DSSI_DSKTAP 23
#define MSCP$K_CM_DSSI_OTHER 24
#define MSCP$K_CM_TUK50 25
#define MSCP$K_CM_RRD50U 26
#define MSCP$K_CM_KDM70 27
#define MSCP$K_CM_TQL70 28
#define MSCP$K_CM_TM32 29
#define MSCP$K_CM_HSC70 32
#define MSCP$K_CM_HSC40 33
#define MSCP$K_CM_HSC60 34
#define MSCP$K_CM_HSC90 35
#define MSCP$K_CM_RN20 36
#define MSCP$K_CM_ENE10 37
#define MSCP$K_CM_TN10 38
#define MSCP$K_CM_HSJ40 40
#define MSCP$K_CM_HSC65 41
#define MSCP$K_CM_HSC95 42
#define MSCP$K_CM_HSJ80 46
#define MSCP$K_CM_KSB50 64
#define MSCP$K_CM_TK50_DEBNT 65
#define MSCP$K_CM_TBK70 66
#define MSCP$K_CM_TBK7L 68
#define MSCP$K_CM_RF30 96
#define MSCP$K_CM_RF71 97
#define MSCP$K_CM_TF85 98
#define MSCP$K_CM_TF70 99
#define MSCP$K_CM_RF31 100
#define MSCP$K_CM_RF72 101
#define MSCP$K_CM_RF73 102
#define MSCP$K_CM_RF32 103
#define MSCP$K_CM_RF35 104
#define MSCP$K_CM_RF36 108
#define MSCP$K_CM_RF74 109
#define MSCP$K_CM_TF86 110
#define MSCP$K_CM_HSD20 111
#define MSCP$K_CM_RF75 112
#define MSCP$K_CM_RF37 114
#define MSCP$K_CM_HSX50 128
#define MSCP$K_CM_ULTRIX 248
#define MSCP$K_CM_SVS 249
	
 struct _stcon {
  struct _mscp_basic_pkt mscp$r_mscp_basic_pkt;
  unsigned short int mscp$w_version;
  union  {
    union  {
      unsigned short int mscp$w_cnt_flgs;
      struct  {
	unsigned mscp$v_cf_576 : 1;
	unsigned mscp$v_cf_shadw : 1;
	unsigned mscp$v_cf_mlths : 1;
	unsigned mscp$v_cf_ldcd : 1;
	unsigned mscp$v_cf_this : 1;
	unsigned mscp$v_cf_other : 1;
	unsigned mscp$v_cf_misc : 1;
	unsigned mscp$v_cf_attn : 1;
	unsigned mscp$v_cf_rdcd : 1;
	unsigned mscp$v_cf_whl : 1;
	unsigned mscp$v_cf_rdo : 1;
	unsigned mscp$v_cf_nfesc : 1;
	unsigned mscp$v_filler : 1;
	unsigned mscp$v_cf_load : 1;
	unsigned mscp$v_cf_edcrp : 1;
	unsigned mscp$v_cf_replc : 1;
      } mscp$r_fill_55_;
    } mscp$r_fill_54_;
    union  {
      unsigned short int mscp$w_tu_cnt_flgs;
      struct  {
	unsigned mscp$v_filler : 14;
	unsigned mscp$v_cf_mtden : 1;
	unsigned mscp$v_cf_srt : 1;
      } mscp$r_fill_57_;
    } mscp$r_fill_56_;
  } mscp$r_cnt_flgs_overlay;
  union  {
    unsigned short int mscp$w_hst_tmo;
    struct  {
      unsigned short int mscp$w_cnt_tmo;
    } mscp$r_fill_59_;
  } mscp$r_fill_58_;
  unsigned char mscp$b_cnt_svr;
  unsigned char mscp$b_cnt_hvr;
  union  {
    unsigned int64 mscp$q_time;
    struct  {
      unsigned int64 mscp$q_cnt_id;
    } mscp$r_fill_61_;
  } mscp$r_fill_60_;
  unsigned int mscp$l_maxbcnt;
  unsigned short int mscp$w_conn_ref;

} ;
 

#define MSCP$K_MIN_SIZ 12
#define MSCP$C_MIN_SIZ 12
#define MSCP$K_MXCMDLEN 36
#define MSCP$C_MXCMDLEN 36
#define MSCP$K_DCDCMDLEN 60
#define MSCP$C_DCDCMDLEN 60
#define MSCP$K_LEN 50
#define MSCP$C_LEN 50
	
 struct _dcd {
  struct _mscp_basic_pkt mscp$r_mscp_basic_pkt;
  union  {
    struct  {
      unsigned int mscp$l_lbcount;
      unsigned short int mscp$w_src_unum;
      short int mscp$w_reserved;
      unsigned int64 mscp$q_src_uid;
      unsigned int mscp$l_dest_lbn;
      unsigned short int mscp$w_hrn;
      unsigned short int mscp$w_ent_id;
      int mscp$l_reserved;
      unsigned int mscp$l_src_lbn;
      unsigned int64 mscp$q_port_adr;
      unsigned int64 mscp$q_sys_adr;
    } mscp$r_lbcount_structure;
    struct  {
      int mscp$l_filler;
      unsigned char mscp$z_sbc_sts [16];
      unsigned short int mscp$w_ent_loc;
    } mscp$r_lbcount_end_structure;
  } mscp$r_lbcount_overlay;
};
 

#define MSCP$K_WHM_DALL 1
#define MSCP$K_WHM_DHRN 2
#define MSCP$K_WHM_DELO 3
#define MSCP$K_WHM_RALL 4
#define MSCP$K_WHM_RHRN 5
#define MSCP$K_WHM_DAFC 6
	
 struct _wrhim {
  MSCP_BASIC_PKT mscp$r_mscp_basic_pkt;
  int mscp$l_reserved;
  union  {
    unsigned char mscp$b_wrhis_bd [12];
    struct  {
      unsigned short int mscp$w_unit_al;
      unsigned short int mscp$w_serv_al;
      unsigned short int mscp$w_serv_unal;
    } mscp$r_unit_al_structure;
  } mscp$r_wrhis_bd_overlay;
  union  {
    unsigned short int mscp$w_oper;
    unsigned short int mscp$w_count;
  } mscp$r_oper_overlay;
  unsigned short int mscp$w_offset;
};
 

#define WHIS$M_ET_TLIB 0x4000
#define WHIS$M_ET_ERR 0x8000
#define WHIS$K_WRITELOGLEN 16
#define WHIS$C_WRITELOGLEN 16
	
struct _write_log_entry {
  unsigned short int whis$w_elo;
  unsigned short int whis$w_unit;
  unsigned int whis$l_length;
  unsigned int whis$l_lbn;
  unsigned short int whis$w_hrn;
  union  {
    unsigned short int whis$w_entflgs;
    struct  {
      unsigned whis$v_filler : 14;
      unsigned whis$v_et_tlib : 1;
      unsigned whis$v_et_err : 1;
    } whis$r_fill_63_;
  } whis$r_fill_62_;
} ;
 

#define whis$w_entflgs whis$r_fill_62_.whis$w_entflgs
#define whis$v_et_tlib whis$r_fill_62_.whis$r_fill_63_.whis$v_et_tlib
#define whis$v_et_err whis$r_fill_62_.whis$r_fill_63_.whis$v_et_err

	
struct _get_unit_name {
  struct _mscp_basic_pkt mscp$r_mscp_basic_pkt;
  unsigned int mscp$l_ddr_namelen;
  char mscp$t_ddr_name [28];
};
 
#endif
 
