#ifndef atrdef_h
#define atrdef_h

#define ATR$C_UCHAR 3
#define ATR$C_RECATTR 4
#define ATR$C_FILNAM 5
#define ATR$C_FILTYP 6
#define ATR$C_FILVER 7
#define ATR$C_EXPDAT 8
#define ATR$C_STATBLK 9
#define ATR$C_HEADER 10
#define ATR$C_BLOCKSIZE 11
#define ATR$C_USERLABEL 12
#define ATR$C_ASCDATES 13
#define ATR$C_ALCONTROL 14
#define ATR$C_ENDLBLAST 15
#define ATR$C_ASCNAME 16
#define ATR$C_CREDATE 17
#define ATR$C_REVDATE 18
#define ATR$C_EXPDATE 19
#define ATR$C_BAKDATE 20
#define ATR$C_UIC 21
#define ATR$C_FPRO 22
#define ATR$C_RPRO 23
#define ATR$C_ACLEVEL 24
#define ATR$C_SEMASK 25
#define ATR$C_UIC_RO 26
#define ATR$C_DIRSEQ 27
#define ATR$C_BACKLINK 28
#define ATR$C_JOURNAL 29
#define ATR$C_HDR1_ACC 30

#define ATR$C_ADDACLENT 31
#define ATR$C_DELACLENT 32
#define ATR$C_MODACLENT 33
#define ATR$C_FNDACLENT 34
#define ATR$C_FNDACLTYP 35
#define ATR$C_DELETEACL 36
#define ATR$C_READACL 37
#define ATR$C_ACLLENGTH 38
#define ATR$C_READACE 39
#define ATR$C_RESERVED 40
#define ATR$C_HIGHWATER 41
#define ATR$C_DUMMY_0 42
#define ATR$C_PRIVS_USED 43
#define ATR$C_MATCHING_ACE 44
#define ATR$C_ACCESS_MODE 45
#define ATR$C_FILE_SPEC 46
#define ATR$C_CLASS_MASK 47
#define ATR$C_BUFFER_OFFSET 48
#define ATR$C_RU_ACTIVE 49

#define ATR$C_GRANT_ACE 50
#define ATR$C_NEXT_ACE 51
#define ATR$C_DELETE_ALL 52
#define ATR$C_BACKUP_DONE 53

#define ATR$C_EFFDATE 54
#define ATR$C_RCDDATE 55
#define ATR$C_VD 56
#define ATR$C_PVD 57
#define ATR$C_SVD 58
#define ATR$C_VPD 59
#define ATR$C_VDST 60
#define ATR$C_BOOT 61
#define ATR$C_PTR 62
#define ATR$C_DIR 63
#define ATR$C_XAR 64

#define ATR$C_VOLUME_ID 65
#define ATR$C_VOLUME_FLAGS 66
#define ATR$C_ESCAPE_SEQUENCES 67
#define ATR$C_VOLUME_SET_ID 68
#define ATR$C_COPYRIGHT_FID 69
#define ATR$C_ABSTRACT_FID 70
#define ATR$C_BIBLIOGRAPHIC_FID 71
#define ATR$C_CACHE_SEQ 72
#define ATR$C_FILE_SYSTEM_INFO 73
#define ATR$C_FILE_ATTR_CACHING_POLICY 74
#define ATR$C_FILE_DATA_CACHING_POLICY 75
#define ATR$C_CACHING_OPTIONS 76
#define ATR$C_POSIX_DATA 77
#define ATR$C_ACCDATE 78
#define ATR$C_ATTDATE 79
#define ATR$C_LENGTH_HINT 80
#define ATR$C_EX_RECATTR 81

#define ATR$C_EXTFLAGS 82


#define ATR$C_MAX_PLUS1 83
#define ATR$C_MAX_CODE 82
#define ATR$C_FNDACETYP 35

#define ATR$C_CACHE_POLICY_WRITETHRU 1
#define ATR$C_CACHE_POLICY_WRITEBACK 2
#define ATR$C_CACHE_POLICY_DEFAULT 3

#define ATR$S_UCHAR 4
#define ATR$S_RECATTR 32
#define ATR$S_FILNAM 6
#define ATR$S_FILTYP 2
#define ATR$S_FILVER 2
#define ATR$S_EXPDAT 7
#define ATR$S_STATBLK 32
#define ATR$S_HEADER 512
#define ATR$S_BLOCKSIZE 2
#define ATR$S_USERLABEL 80
#define ATR$S_ASCDATES 35
#define ATR$S_ALCONTROL 14
#define ATR$S_ENDLBLAST 4
#define ATR$S_ASCNAME 252
#define ATR$S_CREDATE 8
#define ATR$S_REVDATE 8
#define ATR$S_EXPDATE 8
#define ATR$S_BAKDATE 8
#define ATR$S_UIC 4
#define ATR$S_FPRO 2
#define ATR$S_RPRO 2
#define ATR$S_ACLEVEL 1
#define ATR$S_SEMASK 8
#define ATR$S_UIC_RO 4
#define ATR$S_DIRSEQ 2
#define ATR$S_BACKLINK 6
#define ATR$S_JOURNAL 1
#define ATR$S_HDR1_ACC 1

#define ATR$S_ADDACLENT 255
#define ATR$S_DELACLENT 255
#define ATR$S_MODACLENT 255
#define ATR$S_FNDACLENT 255
#define ATR$S_FNDACLTYP 255
#define ATR$S_FNDACETYP 255
#define ATR$S_DELETEACL 255
#define ATR$S_READACL 512
#define ATR$S_ACLLENGTH 4
#define ATR$S_READACE 255
#define ATR$S_RESERVED 380
#define ATR$S_HIGHWATER 4
#define ATR$S_DUMMY_0 4
#define ATR$S_PRIVS_USED 4
#define ATR$S_MATCHING_ACE 255
#define ATR$S_ACCESS_MODE 1
#define ATR$S_FILE_SPEC 4098
#define ATR$S_CLASS_MASK 20
#define ATR$S_BUFFER_OFFSET 2
#define ATR$S_RU_ACTIVE 1
#define ATR$S_GRANT_ACE 255
#define ATR$S_NEXT_ACE 4
#define ATR$S_DELETE_ALL 255
#define ATR$S_BACKUP_DONE 1

#define ATR$S_EFFDATE 8
#define ATR$S_RCDDATE 8
#define ATR$S_VD 7
#define ATR$S_PVD 1395
#define ATR$S_SVD 1395
#define ATR$S_VPD 88
#define ATR$S_VDST 7
#define ATR$S_BOOT 71
#define ATR$S_PTR 45
#define ATR$S_DIR 512
#define ATR$S_XAR 512

#define ATR$S_VOLUME_ID 32
#define ATR$S_VOLUME_FLAGS 1
#define ATR$S_ESCAPE_SEQUENCES 32
#define ATR$S_VOLUME_SET_ID 128
#define ATR$S_COPYRIGHT_FID 37
#define ATR$S_ABSTRACT_FID 37
#define ATR$S_BIBLIOGRAPHIC_FID 37
#define ATR$S_CACHE_SEQ 8
#define ATR$S_FILE_SYSTEM_INFO 2
#define ATR$S_FILE_ATTR_CACHING_POLICY 1
#define ATR$S_FILE_DATA_CACHING_POLICY 1
#define ATR$S_POSIX_DATA 64
#define ATR$S_CACHING_OPTIONS 4
#define ATR$S_ACCDATE 8
#define ATR$S_ATTDATE 8
#define ATR$S_LENGTH_HINT 16
#define ATR$S_EX_RECATTR 8
#define ATR$S_EXTFLAGS 2
	
struct _atrdef {
  unsigned short int atr$w_size;
  unsigned short int atr$w_type;
  void *atr$l_addr;
};

#define ATR$M_FILE_ATTRIBUTES 0xF
#define ATR$M_FILE_CONTENTS 0xF0
#define ATR$C_DEFAULT 0
#define ATR$C_WRITETHROUGH 1
#define ATR$C_WRITEBEHIND 2
#define ATR$C_NOCACHING 3
#define ATR$M_FLUSH_ON_CLOSE 0xF00
#define ATR$C_FLUSH 1
#define ATR$C_NOFLUSH 2
#define ATR$M_CACHING_OPTIONS_MBZ 0xFFFFF000
	
struct _atr_caching_options_flags {
  unsigned atr$v_file_attributes : 4;
  unsigned atr$v_file_contents : 4;
  unsigned atr$v_flush_on_close : 4;
  unsigned atr$v_caching_options_mbz : 20;
};

#define ATR$C_ODS1 1
#define ATR$C_ODS2 2
#define ATR$C_ODS64 64
	
struct _atr_filesystem_info_fields {
  unsigned char atr$b_strucver;
  unsigned char atr$b_struclev;
};

#endif
 
