#ifndef mntdef_h
#define mntdef_h

#define MNT$M_FOREIGN 0x1
#define MNT$M_GROUP 0x2
#define MNT$M_NOASSIST 0x4
#define MNT$M_NODISKQ 0x8
#define MNT$M_NOHDR3 0x10
#define MNT$M_NOLABEL 0x20
#define MNT$M_NOWRITE 0x40
#define MNT$M_OVR_ACCESS 0x80
#define MNT$M_OVR_EXP 0x100
#define MNT$M_OVR_IDENT 0x200
#define MNT$M_OVR_SETID 0x400
#define MNT$M_READCHECK 0x800
#define MNT$M_SHARE 0x1000
#define MNT$M_MESSAGE 0x2000
#define MNT$M_SYSTEM 0x4000
#define MNT$M_WRITECHECK 0x8000
#define MNT$M_WRITETHRU 0x10000
#define MNT$M_NOCACHE 0x20000
#define MNT$M_OVR_LOCK 0x40000
#define MNT$M_NOMNTVER 0x80000
#define MNT$M_NOUNLOAD 0x100000
#define MNT$M_TAPE_DATA_WRITE 0x200000
#define MNT$M_NOCOPY 0x400000
#define MNT$M_NOAUTO 0x800000
#define MNT$M_INIT_ALL 0x1000000
#define MNT$M_INIT_CONT 0x2000000
#define MNT$M_OVR_VOLO 0x4000000
#define MNT$M_INTERCHG 0x8000000
#define MNT$M_CLUSTER 0x10000000
#define MNT$M_NOREBUILD 0x20000000
#define MNT$M_OVR_SHAMEM 0x40000000
#define MNT$M_MULTI_VOL 0x80000000
#define MNT2$M_DISKQ 0x1
#define MNT2$M_COMPACTION 0x2
#define MNT2$M_INCLUDE 0x4
#define MNT2$M_PASS2 0x8
#define MNT2$M_OVR_NOFE 0x10
#define MNT2$M_SCRATCH 0x20
#define MNT2$M_CDROM 0x40
#define MNT2$M_XAR 0x80
#define MNT2$M_DSI 0x100
#define MNT2$M_SUBSYSTEM 0x200
#define MNT2$M_NOCOMPACTION 0x400
#define MNT2$M_OVR_SECURITY 0x800
#define MNT2$M_OVR_LIMITED_SEARCH 0x1000
#define MNT2$M_POOL 0x2000
#define MNT2$M_WLG_ENABLE 0x4000
#define MNT2$M_WLG_DISABLE 0x8000
#define MNT2$M_REQUIRE_MEMBERS 0x10000
#define MNT2$M_VERIFY_LABEL 0x20000
#define MNT2$M_FULL_MERGE 0x40000
#define MNT2$M_WRITE_FIRST 0x80000
#define MNT2$M_DCD 0x100000
#define MNT2$M_NODCD 0x200000
#define MNT2$M_LOCAL_HOST 0x400000
#define MNT2$M_FACTOR 0x800000
#define MNT2$M_PRIORITY 0x1000000
#define MNT$_DEVNAM 1
#define MNT$_VOLNAM 2
#define MNT$_LOGNAM 3
#define MNT$_FLAGS 4
#define MNT$_ACCESSED 5
#define MNT$_PROCESSOR 6
#define MNT$_VOLSET 7
#define MNT$_BLOCKSIZE 8
#define MNT$_DENSITY 9
#define MNT$_EXTENT 10
#define MNT$_FILEID 11
#define MNT$_LIMIT 12
#define MNT$_OWNER 13
#define MNT$_VPROT 14
#define MNT$_QUOTA 15
#define MNT$_RECORDSIZ 16
#define MNT$_WINDOW 17
#define MNT$_EXTENSION 18
#define MNT$_VISUAL_ID 19
#define MNT$_COMMENT 20
#define MNT$_CLASS 21
#define MNT$_UNUSED2 22
#define MNT$_ACCPTNAM 23
#define MNT$_SHACOPY_BUF 24
#define MNT$_SHANAM 25
#define MNT$_SHAMEM 26
#define MNT$_SHAMEM_MGCOPY 27
#define MNT$_SHAMEM_COPY 28
#define MNT$_PRFD_PATH 29
#define MNT$_ASSIGNMENT_UNIT 30
#define MNT$_CART_MEDIA_NAME 31
#define MNT$_CARTRIDGE_NAME 32
#define MNT$_CARTRIDGE_SIDE 33
#define MNT$_COLLECTION 34
#define MNT$_DEVICE_TYPE 35
#define MNT$_DISPOSITION 36
#define MNT$_LOCATION 37
#define MNT$_MEDIA_NAME 38
#define MNT$_UNUSED4 39
#define MNT$_UNDEFINED_FAT 40
#define MNT$_UCS 41
#define MNT$_TAPE_EXPIRATION 42
#define MNT$_PRIORITY 43
#define MNT$_FACTOR 44
#define MNT$_WBM_SIZE 45

#define MNT$_LAST_ITEM_CODE 46
#define MNT$C_BASE_CARTRIDGE 1
#define MNT$C_COMPOUND_CARTRIDGE 2
#define MNT$C_PREASSIGNED_SIDE 3
#define MNT$C_SIDE 4
#define MNT$C_KEEP 1
#define MNT$C_RELEASE 2
	
union _mntdef {
  struct  {
    unsigned mnt$v_foreign		: 1;
    unsigned mnt$v_group		: 1;
    unsigned mnt$v_noassist		: 1;
    unsigned mnt$v_nodiskq		: 1;
    unsigned mnt$v_nohdr3		: 1;
    unsigned mnt$v_nolabel		: 1;
    unsigned mnt$v_nowrite		: 1;
    unsigned mnt$v_ovr_access		: 1;
    unsigned mnt$v_ovr_exp		: 1;
    unsigned mnt$v_ovr_ident		: 1;
    unsigned mnt$v_ovr_setid		: 1;
    unsigned mnt$v_readcheck		: 1;
    unsigned mnt$v_share		: 1;
    unsigned mnt$v_message		: 1;
    unsigned mnt$v_system		: 1;
    unsigned mnt$v_writecheck		: 1;
    unsigned mnt$v_writethru		: 1;
    unsigned mnt$v_nocache		: 1;
    unsigned mnt$v_ovr_lock		: 1;
    unsigned mnt$v_nomntver		: 1;
    unsigned mnt$v_nounload		: 1;
    unsigned mnt$v_tape_data_write	: 1;
    unsigned mnt$v_nocopy		: 1;
    unsigned mnt$v_noauto		: 1;
    unsigned mnt$v_init_all		: 1;
    unsigned mnt$v_init_cont		: 1;
    unsigned mnt$v_ovr_volo		: 1;
    unsigned mnt$v_interchg		: 1;
    unsigned mnt$v_cluster		: 1;
    unsigned mnt$v_norebuild		: 1;
    unsigned mnt$v_ovr_shamem		: 1;
    unsigned mnt$v_multi_vol		: 1;
    unsigned mnt$v_diskq		: 1;
    unsigned mnt$v_compaction		: 1;
    unsigned mnt$v_include		: 1;
    unsigned mnt$v_pass2		: 1;
    unsigned mnt$v_ovr_nofe		: 1;
    unsigned mnt$v_scratch		: 1;
    unsigned mnt$v_cdrom		: 1;
    unsigned mnt$v_xar			: 1;
    unsigned mnt$v_dsi			: 1;
    unsigned mnt$v_subsystem		: 1;
    unsigned mnt$v_nocompaction		: 1;
    unsigned mnt$v_ovr_security		: 1;
    unsigned mnt$v_ovr_limited_search	: 1;
    unsigned mnt$v_pool			: 1;
    unsigned mnt$v_wlg_enable		: 1;
    unsigned mnt$v_wlg_disable		: 1;
    unsigned mnt$v_require_members	: 1;
    unsigned mnt$v_verify_label		: 1;
    unsigned mnt$v_full_merge		: 1;
    unsigned mnt$v_write_first		: 1;
    unsigned mnt$v_dcd			: 1;
    unsigned mnt$v_nodcd		: 1;
    unsigned mnt$v_local_host		: 1;
    unsigned mnt$v_factor		: 1;
    unsigned mnt$v_priority		: 1;
    unsigned mnt$v_minicopy_required	: 1;
    unsigned mnt$v_minicopy_optional	: 1;
    unsigned mnt$v_nominicopy		: 1;
    unsigned mnt$v_fill_0		: 4;
  };
  struct  {
    unsigned mnt2$v_diskq		: 1;
    unsigned mnt2$v_compaction		: 1;
    unsigned mnt2$v_include		: 1;
    unsigned mnt2$v_pass2		: 1;
    unsigned mnt2$v_ovr_nofe		: 1;
    unsigned mnt2$v_scratch		: 1;
    unsigned mnt2$v_cdrom		: 1;
    unsigned mnt2$v_xar			: 1;
    unsigned mnt2$v_dsi			: 1;
    unsigned mnt2$v_subsystem		: 1;
    unsigned mnt2$v_nocompaction	: 1;
    unsigned mnt2$v_ovr_security	: 1;
    unsigned mnt2$v_ovr_limited_search	: 1;
    unsigned mnt2$v_pool		: 1;
    unsigned mnt2$v_wlg_enable		: 1;
    unsigned mnt2$v_wlg_disable		: 1;
    unsigned mnt2$v_require_members	: 1;
    unsigned mnt2$v_verify_label	: 1;
    unsigned mnt2$v_full_merge		: 1;
    unsigned mnt2$v_write_first		: 1;
    unsigned mnt2$v_dcd			: 1;
    unsigned mnt2$v_nodcd		: 1;
    unsigned mnt2$v_local_host		: 1;
    unsigned mnt2$v_factor		: 1;
    unsigned mnt2$v_priority		: 1;
    unsigned mnt2$v_minicopy		: 1;
    unsigned mnt2$v_minicopy_optional	: 1;
    unsigned mnt2$v_nominicopy		: 1;
    unsigned mnt2$v_fill_1		: 4;
  };
};
	
#define MNT$S_MNTDEF 4
#define MNT$V_FOREIGN 0
#define MNT$V_GROUP 1
#define MNT$V_NOASSIST 2
#define MNT$V_NODISKQ 3
#define MNT$V_NOHDR3 4
#define MNT$V_NOLABEL 5
#define MNT$V_NOWRITE 6
#define MNT$V_OVR_ACCESS 7
#define MNT$V_OVR_EXP 8
#define MNT$V_OVR_IDENT 9
#define MNT$V_OVR_SETID 10
#define MNT$V_READCHECK 11
#define MNT$V_SHARE 12
#define MNT$V_MESSAGE 13
#define MNT$V_SYSTEM 14
#define MNT$V_WRITECHECK 15
#define MNT$V_WRITETHRU 16
#define MNT$V_NOCACHE 17
#define MNT$V_OVR_LOCK 18
#define MNT$V_NOMNTVER 19
#define MNT$V_NOUNLOAD 20
#define MNT$V_TAPE_DATA_WRITE 21
#define MNT$V_NOCOPY 22
#define MNT$V_NOAUTO 23
#define MNT$V_INIT_ALL 24
#define MNT$V_INIT_CONT 25
#define MNT$V_OVR_VOLO 26
#define MNT$V_INTERCHG 27
#define MNT$V_CLUSTER 28
#define MNT$V_NOREBUILD 29
#define MNT$V_OVR_SHAMEM 30
#define MNT$V_MULTI_VOL 31
#define MNT$V_DISKQ 32
#define MNT$V_COMPACTION 33
#define MNT$V_INCLUDE 34
#define MNT$V_PASS2 35
#define MNT$V_OVR_NOFE 36
#define MNT$V_SCRATCH 37
#define MNT$V_CDROM 38
#define MNT$V_XAR 39
#define MNT$V_DSI 40
#define MNT$V_SUBSYSTEM 41
#define MNT$V_NOCOMPACTION 42
#define MNT$V_OVR_SECURITY 43
#define MNT$V_OVR_LIMITED_SEARCH 44
#define MNT$V_POOL 45
#define MNT$V_WLG_ENABLE 46
#define MNT$V_WLG_DISABLE 47
#define MNT$V_REQUIRE_MEMBERS 48
#define MNT$V_VERIFY_LABEL 49
#define MNT$V_FULL_MERGE 50
#define MNT$V_WRITE_FIRST 51
#define MNT$V_DCD 52
#define MNT$V_NODCD 53
#define MNT$V_LOCAL_HOST 54
#define MNT$V_FACTOR 55
#define MNT$V_PRIORITY 56
#define MNT$V_MINICOPY 57
#define MNT$V_MINICOPY_OPTIONAL 58

#define MNT2$S_MNTDEF 4
#define MNT2$V_DISKQ 0
#define MNT2$V_COMPACTION 1
#define MNT2$V_INCLUDE 2
#define MNT2$V_PASS2 3
#define MNT2$V_OVR_NOFE 4
#define MNT$V_SCRATCH 5
#define MNT$V_CDROM 6
#define MNT$V_XAR 7
#define MNT$V_DSI 8
#define MNT$V_SUBSYSTEM 9
#define MNT$V_NOCOMPACTION 10
#define MNT$V_OVR_SECURITY 11
#define MNT$V_OVR_LIMITED_SEARCH 12
#define MNT$V_POOL 13
#define MNT$V_WLG_ENABLE 14
#define MNT$V_WLG_DISABLE 15
#define MNT$V_REQUIRE_MEMBERS 16
#define MNT$V_VERIFY_LABEL 17
#define MNT$V_FULL_MERGE 18
#define MNT$V_WRITE_FIRST 19
#define MNT$V_DCD 20
#define MNT$V_NODCD 21
#define MNT$V_LOCAL_HOST 22
#define MNT$V_FACTOR 23
#define MNT$V_PRIORITY 24
#define MNT$V_MINICOPY 25
#define MNT$V_MINICOPY_OPTIONAL 26

struct _undefined_fat {
  unsigned short int unfat$w_mrs;
  unsigned char unfat$b_rat;
  unsigned char unfat$b_rfm;
};
 
#endif
 
