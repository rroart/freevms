#ifndef jpidef_h
#define jpidef_h

#define		JPI$K_DEFAULT_POLICY		0
#define		JPI$K_PSX_FIFO_POLICY		1
#define		JPI$K_PSX_RR_POLICY		2
#define		JPI$K_ALL_THREADS		-2147483648

#define		JPI$C_ADRTYPE		1
#define		JPI$C_CTLTYPE		2
#define		JPI$C_PCBTYPE		3
#define		JPI$C_PHDTYPE		4
#define		JPI$C_PCBFLDTYPE	5
#define		JPI$C_PHDFLDTYPE	6
#define		JPI$C_JIBTYPE		7

#define		JPI$C_PSBTYPE		8
#define		JPI$C_KTBTYPE		9
#define		JPI$C_MAXSTRUC		6
#define		JPI$C_LISTEND		0
#define		JPI$_CHAIN		-1
#define		JPI$_GETJPI_CONTROL_FLAGS	-2
#define		JPI$M_NO_TARGET_INSWAP		0x1
#define		JPI$M_NO_TARGET_AST		0x2
#define		JPI$M_IGNORE_TARGET_STATUS	0x4
#define		JPI$M_THREAD		0x8
#define		JPI$M_NATURAL_PERSONA	0x10
#define		JPI$M_FILL1		0x7FE0
#define		JPI$M_NEED_THREAD_CAP	0x8000
	
struct _jpictldef {
  union  {
    unsigned int jpi$l_jpictlflgs;
    struct  {
      unsigned jpi$v_no_target_inswap		: 1;
      unsigned jpi$v_no_target_ast		: 1;
      unsigned jpi$v_ignore_target_status	: 1;
      unsigned jpi$v_thread		        : 1;
      unsigned jpi$v_natural_persona		: 1;
      unsigned jpi$v_fill1		        : 10;
      unsigned jpi$v_need_thread_cap		: 1;
    };
  };
};
 
#define		JPI$K_OTHER		0
#define		JPI$K_NETWORK		1
#define		JPI$K_BATCH		2
#define		JPI$K_INTERACTIVE	3
#define		JPI$K_DETACHED		0
#define		JPI$K_LOCAL		3
#define		JPI$K_DIALUP		4
#define		JPI$K_REMOTE		5

#define		JPI$M_NEW_MAIL_AT_LOGIN		0x1
#define		JPI$M_PASSWORD_CHANGED		0x2
#define		JPI$M_PASSWORD_EXPIRED		0x4
#define		JPI$M_PASSWORD_WARNING		0x8
#define		JPI$M_PASSWORD2_CHANGED		0x10
#define		JPI$M_PASSWORD2_EXPIRED		0x20
#define		JPI$M_PASSWORD2_WARNING		0x40
	
struct _jpilgidef {
  union  {
    unsigned int jpi$l_jpilgiflgs;
    struct  {
      unsigned jpi$v_new_mail_at_login		: 1;
      unsigned jpi$v_password_changed		: 1;
      unsigned jpi$v_password_expired		: 1;
      unsigned jpi$v_password_warning		: 1;
      unsigned jpi$v_password2_changed		: 1;
      unsigned jpi$v_password2_expired		: 1;
      unsigned jpi$v_password2_warning		: 1;
      unsigned jpi$v_fill_64_		        : 1;
    };
  };
};
 
#define		JPI$_ASTACT		768
#define		JPI$_ASTEN		769
#define		JPI$_PRI		770
#define		JPI$_OWNER		771
#define		JPI$_UIC		772
#define		JPI$_STS		773
#define		JPI$_STATE		774
#define		JPI$_MEM		775
#define		JPI$_GRP		776
#define		JPI$_PRIB		777
#define		JPI$_APTCNT		778
#define		JPI$_TMBU		779
#define		JPI$_GPGCNT		780
#define		JPI$_PPGCNT		781
#define		JPI$_ASTCNT		782
#define		JPI$_BIOCNT		783
#define		JPI$_BIOLM		784
#define		JPI$_BYTCNT		785
#define		JPI$_DIOCNT		786
#define		JPI$_DIOLM		787
#define		JPI$_FILCNT		788
#define		JPI$_TQCNT		789
#define		JPI$_EFWM		790
#define		JPI$_EFCS		791
#define		JPI$_EFCU		792
#define		JPI$_PID		793
#define		JPI$_BYTLM		794
#define		JPI$_PRCCNT		795
#define		JPI$_PRCNAM		796
#define		JPI$_TERMINAL		797
#define		JPI$_JOBPRCCNT		798
#define		JPI$_ENQCNT		799
#define		JPI$_ENQLM		800
#define		JPI$_SWPFILLOC		801
#define		JPI$_MODE		802
#define		JPI$_JOBTYPE		803
#define		JPI$_PROC_INDEX		804
#define		JPI$_MASTER_PID		805
#define		JPI$_RIGHTSLIST		806
#define		JPI$_CPU_ID		807
#define		JPI$_STS2		808
#define		JPI$_NODENAME		809
#define		JPI$_NODE_CSID		810
#define		JPI$_NODE_VERSION		811
#define		JPI$_TT_PHYDEVNAM		812
#define		JPI$_TT_ACCPORNAM		813
#define		JPI$_PROCESS_RIGHTS		814
#define		JPI$_SYSTEM_RIGHTS		815
#define		JPI$_IMAGE_RIGHTS		816
#define		JPI$_RIGHTS_SIZE		817
#define		JPI$_CLASSIFICATION		818
#define		JPI$_SCHED_POLICY		819
#define		JPI$_RMS_FILEPROT		820
#define		JPI$_MULTITHREAD		821
#define		JPI$_KT_COUNT	        	822
#define		JPI$_INITIAL_THREAD_PID		823
#define		JPI$_THREAD_INDEX		824
#define		JPI$_CURRENT_USERCAP_MASK		825
#define		JPI$_PERMANENT_USERCAP_MASK		826
#define		JPI$_CURRENT_AFFINITY_MASK		827
#define		JPI$_PERMANENT_AFFINITY_MASK		828
#define		JPI$_PERSONA_ID		        829
#define		JPI$_PERSONA_AUTHPRIV		830
#define		JPI$_PERSONA_PERMPRIV		831
#define		JPI$_PERSONA_WORKPRIV		832
#define		JPI$_IMAGE_WORKPRIV		833
#define		JPI$_PERSONA_RIGHTS_SIZE	834
#define		JPI$_PERSONA_RIGHTS		835
#define		JPI$_SUBSYSTEM_RIGHTS_SIZE	836
#define		JPI$_SUBSYSTEM_RIGHTS		837
#define		JPI$_INSTALL_RIGHTS_SIZE	838
#define		JPI$_INSTALL_RIGHTS		839
#define		JPI$_SYSTEM_RIGHTS_SIZE		840
#define		JPI$_CURRENT_CAP_MASK		841
#define		JPI$_PERMANENT_CAP_MASK		842
#define		JPI$_SCHED_CLASS_NAME		843
#define		JPI$_HOME_RAD		844
#define		JPI$_CREATOR		847
#define		JPI$_KRNLTIM		848
#define		JPI$_EXECTIM		849
#define		JPI$_SUPRTIM		850
#define		JPI$_USERTIM		851

#define		JPI$_LASTPCB		852

#define		JPI$_CURPRIV		1024
#define		JPI$_WSAUTH		1025
#define		JPI$_WSQUOTA		1026
#define		JPI$_DFWSCNT		1027
#define		JPI$_FREP0VA		1028
#define		JPI$_FREP1VA		1029
#define		JPI$_DFPFC		1030
#define		JPI$_CPUTIM		1031
#define		JPI$_PRCLM		1032
#define		JPI$_ASTLM		1033
#define		JPI$_PAGEFLTS		1034
#define		JPI$_DIRIO		1035
#define		JPI$_BUFIO		1036
#define		JPI$_CPULIM		1037
#define		JPI$_PGFLQUOTA		1038
#define		JPI$_FILLM		1039
#define		JPI$_TQLM		1040
#define		JPI$_WSSIZE		1041
#define		JPI$_AUTHPRIV		1042
#define		JPI$_IMAGPRIV		1043
#define		JPI$_PAGFILCNT		1044
#define		JPI$_FREPTECNT		1045
#define		JPI$_WSEXTENT		1046
#define		JPI$_WSAUTHEXT		1047
#define		JPI$_AUTHPRI		1048
#define		JPI$_PAGFILLOC		1049
#define		JPI$_IMAGECOUNT		1050
#define		JPI$_PHDFLAGS		1051
#define		JPI$_VP_CPUTIM		1052
#define		JPI$_VP_CONSUMER		1053
#define		JPI$_P0_FIRST_FREE_VA_64	1054
#define		JPI$_P1_FIRST_FREE_VA_64	1055
#define		JPI$_P2_FIRST_FREE_VA_64	1056
#define		JPI$_IMAGE_AUTHPRIV		1057
#define		JPI$_IMAGE_PERMPRIV		1058

#define		JPI$_LASTPHD		1059

#define		JPI$_VIRTPEAK		512
#define		JPI$_WSPEAK		513
#define		JPI$_USERNAME		514
#define		JPI$_ACCOUNT		515
#define		JPI$_PROCPRIV		516
#define		JPI$_VOLUMES		517
#define		JPI$_LOGINTIM		518
#define		JPI$_IMAGNAME		519
#define		JPI$_SITESPEC		520
#define		JPI$_MSGMASK		521
#define		JPI$_CLINAME		522
#define		JPI$_TABLENAME		523
#define		JPI$_CREPRC_FLAGS	524
#define		JPI$_UAF_FLAGS		525
#define		JPI$_MAXDETACH		526
#define		JPI$_MAXJOBS		527
#define		JPI$_SHRFILLM		528
#define		JPI$_FAST_VP_SWITCH	529
#define		JPI$_SLOW_VP_SWITCH	530
#define		JPI$_LAST_LOGIN_I	531
#define		JPI$_LAST_LOGIN_N	532
#define		JPI$_LOGIN_FAILURES	533
#define		JPI$_LOGIN_FLAGS	534
#define		JPI$_RMS_DFMBC	        535
#define		JPI$_RMS_DFMBFSDK	536
#define		JPI$_RMS_DFMBFSMT	537
#define		JPI$_RMS_DFMBFSUR	538
#define		JPI$_RMS_DFMBFREL	539
#define		JPI$_RMS_DFMBFIDX	540
#define		JPI$_RMS_PROLOGUE	541
#define		JPI$_RMS_EXTEND_SIZE	542
#define		JPI$_RMS_DFNBC	        543
#define		JPI$_RMS_DFLRL	        544
#define		JPI$_RMS_HEURISTIC	545
#define		JPI$_PARSE_STYLE_PERM	546
#define		JPI$_PARSE_STYLE_IMAGE	547
#define		JPI$_RMS_VCC_DFW	548
#define		JPI$_RMS_QUERY_LOCK	549
#define		JPI$_CASE_LOOKUP_PERM	550
#define		JPI$_CASE_LOOKUP_IMAGE	551

#define		JPI$_LASTCTL		552

#define		JPI$_EXCVEC		256
#define		JPI$_FINALEXC		257

#define		JPI$_LASTADR		258

#define		JPI$_LASTPCBFLD		1280

#define		JPI$_LASTPHDFLD		1536
 
#endif
 
