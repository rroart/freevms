#ifndef prdef_h
#define prdef_h

#define PR$_ESP 1
#define PR$_SSP 2
#define PR$_USP 3
#define PR$_ASN 6
#define PR$_ASTEN 48
#define PR$_ASTSR 49
#define PR$_DATFX 23
#define PR$_IPIR 22
#define PR$_IPL 18
#define PR$_MCES 38
#define PR$_PCBB 16
#define PR$_PME 61
#define PR$_PRBR 15
#define PR$_SCBB 17
#define PR$_SIRR 20
#define PR$_SISR 21
#define PR$_TBIA 57
#define PR$_TBIAP 50
#define PR$_TBIS 58
#define PR$_TBIS_64 60
#define PR$_TBISD 59
#define PR$_TBISI 47
#define PR$_VPTB 12
#define PR$_SID_TYP780 1
#define PR$_SID_TYP750 2
#define PR$_SID_TYP730 3
#define PR$_SID_TYP790 4
#define PR$_SID_TYP8SS 5
#define PR$_SID_TYP8NN 6
#define PR$_SID_TYPUV1 7
#define PR$_SID_TYPUV2 8
#define PR$_SID_TYP410 8
#define PR$_SID_TYP009 9
#define PR$_SID_TYP420 10
#define PR$_SID_TYP520 10
#define PR$_SID_TYP650 10
#define PR$_SID_TYP9CC 10
#define PR$_SID_TYP9CI 10
#define PR$_SID_TYP60 10
#define PR$_SID_TYP670 11
#define PR$_SID_TYP9RR 11
#define PR$_SID_TYP43 11
#define PR$_SID_TYP9AQ 14
#define PR$_SID_TYP8PS 17
#define PR$_SID_TYP1202 18
#define PR$_SID_TYP46 18
#define PR$_SID_TYP600 19
#define PR$_SID_TYP690 19
#define PR$_SID_TYP700 19
#define PR$_SID_TYP1302 19
#define PR$_SID_TYP49 19
#define PR$_SID_TYP1303 19
#define PR$_SID_TYP660 20
#define PR$_SID_TYP440 20
#define PR$_SID_TYP4A 20
#define PR$_SID_TYP550 20
#define PR$_SID_TYP1701 23
#define PR$_SID_TYPMAX 23
#define PR$_SID_TYP_NOTAVAX 128
#define PR$_SID_TYPUV 8
#define PR$_XSID_UV_UV 0
#define PR$_XSID_UV_UV2 1
#define PR$_XSID_UV_410 4
#define PR$_SID_TYPCV 10
#define PR$_XSID_CV_CV 0
#define PR$_XSID_CV_650 1
#define PR$_XSID_CV_9CC 2
#define PR$_XSID_CV_60 3
#define PR$_XSID_CV_420 4
#define PR$_XSID_CV_9CI 5
#define PR$_XSID_CV_520 7
#define PR$_SID_TYPRV 11
#define PR$_XSID_RV_RV 0
#define PR$_XSID_RV_670 1
#define PR$_XSID_RV_9RR 2
#define PR$_XSID_RV_43 4
#define PR$_SID_TYPV12 18
#define PR$_XSID_V12_V12 0
#define PR$_XSID_V12_1202 2
#define PR$_XSID_V12_46 4
#define PR$_SID_TYPV13 19
#define PR$_XSID_V13_V13 0
#define PR$_XSID_V13_690 1
#define PR$_XSID_V13_1302 2
#define PR$_XSID_V13_1303 3
#define PR$_XSID_V13_49 4
#define PR$_XSID_V13_700 5
#define PR$_XSID_V13_600 6
#define PR$_SID_TYPV14 20
#define PR$_XSID_V14_V14 0
#define PR$_XSID_V14_660 1
#define PR$_XSID_V14_440 4
#define PR$_XSID_V14_4A 5
#define PR$_XSID_V14_550 7
#define PR$_SID_TYPV17 23
#define PR$_XSID_V17_V17 0
#define PR$_XSID_V17_1701 1
#define PR$_XSID_N8800 0
#define PR$_XSID_N8700 1
#define PR$_XSID_N2 2
#define PR$_XSID_N3 3
#define PR$_XSID_N4 4
#define PR$_XSID_N5 5
#define PR$_XSID_N8550 6
#define PR$_XSID_N8500 7
#define PR$_XSID_N8NNN -1
#define PR$M_ASTEN 0xF
#define PR$M_ASTEN_KEN 0x1
#define PR$M_ASTEN_EEN 0x2
#define PR$M_ASTEN_SEN 0x4
#define PR$M_ASTEN_UEN 0x8
#define PR$M_ASTEN_DSBL_ALL 0
#define PR$M_ASTEN_ENBL_ALL 255
#define PR$M_ASTEN_ENBL_K 17
#define PR$M_ASTEN_ENBL_E 34
#define PR$M_ASTEN_ENBL_S 68
#define PR$M_ASTEN_ENBL_U 136
#define PR$M_ASTEN_PRSRV_ALL 15
#define PR$M_ASTEN_PRSRV_K 1
#define PR$M_ASTEN_PRSRV_E 2
#define PR$M_ASTEN_PRSRV_S 4
#define PR$M_ASTEN_PRSRV_U 8
#define PR$M_ASTSR 0xF
#define PR$M_ASTSR_KPD 0x1
#define PR$M_ASTSR_EPD 0x2
#define PR$M_ASTSR_SPD 0x4
#define PR$M_ASTSR_UPD 0x8
#define PR$M_ASTSR_CLR_ALL 0
#define PR$M_ASTSR_SET_ALL 255
#define PR$M_ASTSR_SET_K 17
#define PR$M_ASTSR_SET_E 34
#define PR$M_ASTSR_SET_S 68
#define PR$M_ASTSR_SET_U 136
#define PR$M_ASTSR_PRSRV_ALL 15
#define PR$M_ASTSR_PRSRV_K 1
#define PR$M_ASTSR_PRSRV_E 2
#define PR$M_ASTSR_PRSRV_S 4
#define PR$M_ASTSR_PRSRV_U 8
#define PR$M_FEN_FEN 0x1
#define PR$M_DATFX_DATFX 0x1
#define PR$M_IPL_IPL 0x1F
#define PR$M_MCES_MCK 0x1
#define PR$M_MCES_SCE 0x2
#define PR$M_MCES_PCE 0x4
#define PR$M_MCES_DPC 0x8
#define PR$M_MCES_DSC 0x10
#define PR$V_PCBB_PA 0
#define PR$S_PCBB_PA 48
#define PR$M_PS_SW 0x3
#define PR$M_PS_PRVMOD 0x3
#define PR$M_PS_SYSSTATE 0x4
#define PR$M_PS_CURMOD 0x18
#define PR$M_PS_VMM 0x80
#define PR$M_PS_IPL 0x1F00
#define PR$M_PS_MBZ_62 0x4000000000000000
#define PR$M_PS_MBZ_63 0x8000000000000000
#define PR$V_PS_MAX_PS_REG_BIT 13
#define PR$C_PS_KERNEL 0
#define PR$C_PS_EXEC 1
#define PR$C_PS_SUPER 2
#define PR$C_PS_USER 3
#define PR$M_PTBR_PFN 0xFFFFFFFF
#define PR$M_SCBB_PFN 0xFFFFFFFF
#define PR$M_SIRR_LVL 0xF
#define PR$M_SISR_SUMMARY 0xFFFF
#define PR$M_SISR_RAZ 0x1
#define PR$M_SISR_IR1 0x2
#define PR$M_SISR_IR2 0x4
#define PR$M_SISR_IR3 0x8
#define PR$M_SISR_IR4 0x10
#define PR$M_SISR_IR5 0x20
#define PR$M_SISR_IR6 0x40
#define PR$M_SISR_IR7 0x80
#define PR$M_SISR_IR8 0x100
#define PR$M_SISR_IR9 0x200
#define PR$M_SISR_IR10 0x400
#define PR$M_SISR_IR11 0x800
#define PR$M_SISR_IR12 0x1000
#define PR$M_SISR_IR13 0x2000
#define PR$M_SISR_IR14 0x4000
#define PR$M_SISR_IR15 0x8000
#define PR$M_TBCHK_VA_PRESENT 0x1
#define PR$M_IEEE_DNOD 0x800000000000
#define PR$M_IEEE_DNZ 0x1000000000000
#define PR$M_IEEE_INVD 0x2000000000000
#define PR$M_IEEE_DZED 0x4000000000000
#define PR$M_IEEE_OVFD 0x8000000000000
#define PR$M_IEEE_INV 0x10000000000000
#define PR$M_IEEE_DZE 0x20000000000000
#define PR$M_IEEE_OVF 0x40000000000000
#define PR$M_IEEE_UNF 0x80000000000000
#define PR$M_IEEE_INE 0x100000000000000
#define PR$M_IEEE_IOV 0x200000000000000
#define PR$M_IEEE_UNDZ 0x1000000000000000
#define PR$M_IEEE_UNFD 0x2000000000000000
#define PR$M_IEEE_INED 0x4000000000000000
#define PR$M_IEEE_SUMMARY 0x8000000000000000
	
union _prdef {
  unsigned long long pr$q_quad_access;
  unsigned int pr$l_long_access [2];
  struct  {
    unsigned pr$v_sid_sn : 12;
    unsigned pr$v_sid_pl : 3;
    unsigned pr$v_sid_eco : 9;
    unsigned pr$v_sid_type : 8;
  };
  struct  {
    unsigned pr$v_fill_xsid_bits : 24;
    unsigned pr$v_xsid_type : 8;
  };
  union  {
    unsigned pr$v_asten : 4;
    struct  {
      unsigned pr$v_asten_ken : 1;
      unsigned pr$v_asten_een : 1;
      unsigned pr$v_asten_sen : 1;
      unsigned pr$v_asten_uen : 1;
      unsigned pr$v_fill_65_ : 4;
    };
  };
  union  {
    unsigned pr$v_astsr : 4;
    struct  {
      unsigned pr$v_astsr_kpd : 1;
      unsigned pr$v_astsr_epd : 1;
      unsigned pr$v_astsr_spd : 1;
      unsigned pr$v_astsr_upd : 1;
      unsigned pr$v_fill_66_ : 4;
    };
  };
  unsigned pr$v_fen_fen : 1;
  unsigned pr$v_datfx_datfx : 1;
  unsigned pr$v_ipl_ipl : 5;
  struct  {
    unsigned pr$v_mces_mck : 1;
    unsigned pr$v_mces_sce : 1;
    unsigned pr$v_mces_pce : 1;
    unsigned pr$v_mces_dpc : 1;
    unsigned pr$v_mces_dsc : 1;
    unsigned pr$v_fill_67_ : 3;
  };

  struct  {
    unsigned pr$v_ps_sw : 2;
    unsigned pr$v_fill_68_ : 6;
  };
  struct  {
    unsigned pr$v_ps_prvmod : 2;
    unsigned pr$v_ps_sysstate : 1;
    unsigned pr$v_ps_curmod : 2;
    unsigned psdef$$_ps_fill_1 : 2;
    unsigned pr$v_ps_vmm : 1;
    unsigned pr$v_ps_ipl : 5;
    unsigned psdef$$_ps_fill_2 : 20;
    unsigned psdef$$_ps_fill_3 : 23;
    unsigned pr$v_ps_sp_align : 6;
    unsigned pr$v_ps_mbz_62 : 1;
    unsigned pr$v_ps_mbz_63 : 1;
  };
  unsigned pr$v_ptbr_pfn : 32;
  unsigned pr$v_scbb_pfn : 32;
  unsigned pr$v_sirr_lvl : 4;
  union  {
    unsigned pr$v_sisr_summary : 16;
    struct  {
      unsigned pr$v_sisr_raz : 1;
      unsigned pr$v_sisr_ir1 : 1;
      unsigned pr$v_sisr_ir2 : 1;
      unsigned pr$v_sisr_ir3 : 1;
      unsigned pr$v_sisr_ir4 : 1;
      unsigned pr$v_sisr_ir5 : 1;
      unsigned pr$v_sisr_ir6 : 1;
      unsigned pr$v_sisr_ir7 : 1;
      unsigned pr$v_sisr_ir8 : 1;
      unsigned pr$v_sisr_ir9 : 1;
      unsigned pr$v_sisr_ir10 : 1;
      unsigned pr$v_sisr_ir11 : 1;
      unsigned pr$v_sisr_ir12 : 1;
      unsigned pr$v_sisr_ir13 : 1;
      unsigned pr$v_sisr_ir14 : 1;
      unsigned pr$v_sisr_ir15 : 1;
    };
  };
  struct  {
    unsigned pr$v_tbchk_va_present : 1;
    unsigned pr$v_fill_1 : 31;
    unsigned pr$v_fill_2 : 31;
    unsigned pr$v_tbchk_no_tbchk : 1;
  };
  struct  {
    unsigned pr$v_fpcr_fill_1 : 32;
    unsigned pr$v_fpcr_fill_2 : 15;
    unsigned pr$v_ieee_dnod : 1;
    unsigned pr$v_ieee_dnz : 1;
    unsigned pr$v_ieee_invd : 1;
    unsigned pr$v_ieee_dzed : 1;
    unsigned pr$v_ieee_ovfd : 1;
    unsigned pr$v_ieee_inv : 1;
    unsigned pr$v_ieee_dze : 1;
    unsigned pr$v_ieee_ovf : 1;
    unsigned pr$v_ieee_unf : 1;
    unsigned pr$v_ieee_ine : 1;
    unsigned pr$v_ieee_iov : 1;
    unsigned pr$v_ieee_dyn_rnd : 2;
    unsigned pr$v_ieee_undz : 1;
    unsigned pr$v_ieee_unfd : 1;
    unsigned pr$v_ieee_ined : 1;
    unsigned pr$v_ieee_summary : 1;
  };
};
 
#endif
 
