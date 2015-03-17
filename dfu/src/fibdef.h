/********************************************************************************************************************************/
/* Created: 10-AUG-1994 09:41:30 by OpenVMS SDL EV1-33     */
/* Source:  10-AUG-1994 09:38:21 SSOVCS$DKA300:[SYS0.SYSUPD.CC040]STARDEFFL.SDI; */
/********************************************************************************************************************************/
/*** MODULE $FIBDEF ***/
#ifndef __FIBDEF_LOADED
#define __FIBDEF_LOADED 1

#pragma nostandard

#ifdef __cplusplus
extern "C" {
#define __unknown_params ...
#else
#define __unknown_params
#endif

#if !defined(__VAXC) && !defined(VAXC)
#define __struct struct
#define __union union
#else
#define __struct variant_struct
#define __union variant_union
#endif

    /*----                                                                      */
    /*                                                                          */
    /* LAYOUT OF THE FILE IDENTIFICATION BLOCK (FIB)                            */
    /*                                                                          */
    /*****                                                                      */
    /*                                                                          */
    /*        NOTE:        If the size of the FIB is changed the following must be changed */
    /*                to reflect the change:                                    */
    /*                                                                          */
    /*                In Module:        [RMS.SRC]RMSFWADEF.SDL                  */
    /*                                                                          */
    /*                Field:                FWA$T_FIBBUF                        */
    /*                                                                          */
    /*                Constant:        FWA$C_FIBLEN                             */
    /*                                                                          */
    /*                Both the field and constant must be GEQ to the size of    */
    /*                the FIB, i.e. FIB$C_LENGTH. FIB length is currently 80.   */
    /*                                                                          */
    /*****                                                                      */
    /*                                                                          */
#define FIB$M_NOWRITE 0x1
#define FIB$M_DLOCK 0x2
#define FIB$M_BLK_LOCK 0x4
#define FIB$M_FASTRDSQNOS 0x8
#define FIB$M_SPOOL 0x10
#define FIB$M_WRITECK 0x20
#define FIB$M_SEQONLY 0x40
#define FIB$M_SEQNO 0x80
#define FIB$M_WRITE 0x100
#define FIB$M_READCK 0x200
#define FIB$M_NOREAD 0x400
#define FIB$M_NOTRUNC 0x800
#define FIB$M_CONTROL 0x1000
#define FIB$M_NO_READ_DATA 0x2000
#define FIB$M_EXECUTE 0x10000
#define FIB$M_PRSRV_ATR 0x20000
#define FIB$M_RMSLOCK 0x40000
#define FIB$M_WRITETHRU 0x80000
#define FIB$M_NOLOCK 0x100000
#define FIB$M_NORECORD 0x200000
#define FIB$M_NOVERIFY 0x400000
#define FIB$M_CHANGE_VOL 0x800000
#define FIB$M_REWIND 0x8
#define FIB$M_CURPOS 0x10
#define FIB$M_UPDATE 0x40
#define FIB$K_ACCDATA 10                /* ABOVE DATA NECESSARY FOR ACCESS  */
#define FIB$C_ACCDATA 10                /* ABOVE DATA NECESSARY FOR ACCESS  */
#define FIB$K_DIRDATA 22                /* ABOVE DATA NECESSARY FOR DIRECTORY OPS  */
#define FIB$C_DIRDATA 22                /* ABOVE DATA NECESSARY FOR DIRECTORY OPS  */
#define FIB$M_ALLVER 0x8
#define FIB$M_ALLTYP 0x10
#define FIB$M_ALLNAM 0x20
#define FIB$M_WILD 0x100
#define FIB$M_NEWVER 0x200
#define FIB$M_SUPERSEDE 0x400
#define FIB$M_FINDFID 0x800
#define FIB$M_LOWVER 0x4000
#define FIB$M_HIGHVER 0x8000
#define FIB$M_ALCON 0x1
#define FIB$M_ALCONB 0x2
#define FIB$M_FILCON 0x4
#define FIB$M_ALDEF 0x8
#define FIB$M_ALLOCATR 0x10
#define FIB$M_EXTEND 0x80
#define FIB$M_TRUNC 0x100
#define FIB$M_NOHDREXT 0x200
#define FIB$M_MARKBAD 0x400
#define FIB$M_NOPLACE 0x4000
#define FIB$M_NOCHARGE 0x8000
#define FIB$K_EXTDATA 32                /* ABOVE NECESSARY FOR BASIC FILE EXTENSION  */
#define FIB$C_EXTDATA 32                /* ABOVE NECESSARY FOR BASIC FILE EXTENSION  */
#define FIB$M_EXACT 0x1
#define FIB$M_ONCYL 0x2
#define FIB$C_CYL 1                     /* CYLINDER ADDRESS SPECIFIED       */
#define FIB$C_LBN 2                     /* LBN SPECIFIED                    */
#define FIB$C_VBN 3                     /* PROXIMATE VBN SPECIFIED          */
#define FIB$C_RFI 4                     /* RELATED FILE ID SPECIFIED        */
#define FIB$K_ALCDATA 44                /* ABOVE DATA NECESSARY FOR PLACEMENT  */
#define FIB$C_ALCDATA 44                /* ABOVE DATA NECESSARY FOR PLACEMENT  */
#define FIB$M_ALT_REQ 0x1
#define FIB$M_ALT_GRANTED 0x2
#define FIB$M_DIRACL 0x4
#define FIB$M_PROPAGATE 0x8
#define FIB$M_NOCOPYACL 0x10
#define FIB$M_NOCOPYOWNER 0x20
#define FIB$M_NOCOPYPROT 0x40
#define FIB$M_EXCLPREVIOUS 0x80
#define FIB$M_ALIAS_ENTRY 0x100
#define FIB$M_WILDFID_FLAG 0x200
#define FIB$K_MOVEFILE 72               /* end of FIB required by MOVEFILE  */
#define FIB$C_MOVEFILE 72               /* end of FIB required by MOVEFILE  */
#define FIB$K_LENGTH 80
#define FIB$C_LENGTH 80
    struct fibdef
    {
        __union  {
            unsigned int fib$l_acctl;       /* ACCESS CONTROL BITS              */
            __struct
            {
                unsigned fib$v_nowrite : 1; /* NO OTHER WRITERS                 */
                unsigned fib$v_dlock : 1;   /* ENABLE DEACCESS LOCK             */
                unsigned fib$v_blk_lock : 1; /* ENABLE RMS-11 BLOCK LOCKING     */
                unsigned fib$v_fastrdsqnos : 1; /* READ HDRSEQ AND DATASEQ VALS */
                unsigned fib$v_spool : 1;   /* SPOOL FILE ON CLOSE              */
                unsigned fib$v_writeck : 1; /* ENABLE WRITE CHECK               */
                unsigned fib$v_seqonly : 1; /* SEQUENTIAL ONLY ACCESS           */
                unsigned fib$v_seqno : 1;   /* STORE SEQNOS FOR THIS FILE       */
                unsigned fib$v_write : 1;   /* WRITE ACCESS                     */
                unsigned fib$v_readck : 1;  /* ENABLE READ CHECK                */
                unsigned fib$v_noread : 1;  /* NO OTHER READERS                 */
                unsigned fib$v_notrunc : 1; /* FILE MAY NOT BE TRUNCATED        */
                unsigned fib$v_control : 1; /* CONTROL ACCESS TO FILE           */
                unsigned fib$v_no_read_data : 1; /* NO READ ACCESS TO FILE DATA */
                unsigned fibdef$$_fill_3 : 2; /* SPARE                          */
                /* THE HIGH 8 BITS CANNOT BE COPIED                                         */
                /* INTO THE ACCESS MODE WORD IN THE WINDOW                                  */
                unsigned fib$v_execute : 1; /* ACCESS FOR EXECUTE (USE EXECUTE PROTECTION)  */
                unsigned fib$v_prsrv_atr : 1; /* PRESERVE ORIGINAL ATTRIBUTES OF FILE  */
                unsigned fib$v_rmslock : 1; /* OPEN WITH RMS RECORD LOCKING     */
                unsigned fib$v_writethru : 1; /* FORCE CACHE WRITE-THROUGH ON OPERATION  */
                unsigned fib$v_nolock : 1;  /* OVERRIDE ACCESS INTERLOCKS       */
                unsigned fib$v_norecord : 1; /* DO NOT RECORD FILE ACCESS       */
                unsigned fib$v_noverify : 1; /* Do not perform compare on move. */
                unsigned fib$v_change_vol : 1; /* Allow Movefile move between volumes in a set. */
            } fib$r_acctl_bits0;
            __struct
            {
                unsigned fibdef$$_fill_5 : 3;
                unsigned fib$v_rewind : 1;  /* REWIND TAPE                      */
                unsigned fib$v_curpos : 1;  /* CREATE AT CURRENT TAPE POSITION  */
                unsigned fibdef$$_fill_6 : 1;
                unsigned fib$v_update : 1;  /* UPDATE MODE (POSITION TO START OF FILE)  */
                unsigned fib$v_fill_1 : 1;
            } fib$r_acctl_bits1;
            __struct
            {
                char fibdef$$_fill_13 [3];
                char fib$b_wsize;           /* WINDOW SIZE                      */
            } fib$r_acctl_fields2;
        } fib$r_acctl_overlay;
        __union
        {
            unsigned short int fib$w_fid [3]; /* FILE ID                        */
            __struct  {
                unsigned short int fib$w_fid_num; /* FILE NUMBER                */
                unsigned short int fib$w_fid_seq; /* FILE SEQUENCE NUMBER       */
                __union  {
                    unsigned short int fib$w_fid_rvn; /* RELATIVE VOLUME NUMBER  */
                    __struct  {
                        unsigned char fib$b_fid_rvn; /* SHORT FORM RVN          */
                        unsigned char fib$b_fid_nmx; /* EXTENDED FILE NUMBER    */
                    } fib$r_fid_rvn_fields;
                } fib$r_fid_rvn_overlay;
            } fib$r_fid_fields;
#if 0
            // gave offset problem
            __struct  {                     /* File ID fields for ISO 9660      */
                unsigned short int fib$w_fid_dirnum; /*	Directory number of File-Id */
                unsigned int fib$l_fid_recnum; /*	Record number of File-ID    */
            } fib$r_fid_iso_9660_fields;
#endif
        } fib$r_fid_overlay;
        __union
        {
            unsigned short int fib$w_did [3]; /* DIRECTORY ID                   */
            __struct  {
                unsigned short int fib$w_did_num; /* FILE NUMBER                */
                unsigned short int fib$w_did_seq; /* FILE SEQUENCE NUMBER       */
                __union  {
                    unsigned short int fib$w_did_rvn; /* RELATIVE VOLUME NUMBER  */
                    __struct  {
                        unsigned char fib$b_did_rvn; /* SHORT FORM RVN          */
                        unsigned char fib$b_did_nmx; /* EXTENDED FILE NUMBER    */
                    } fib$r_did_rvn_fields;
                } fib$r_did_rvn_overlay;
            } fib$r_did_fields;
#if 0
            // gave offset problem
            __struct  {                     /* Directory ID fields for ISO 9660 */
                unsigned short int fib$w_did_dirnum; /*	Directory number of File-Id */
                unsigned int fib$l_did_recnum; /*	Record number of File-ID    */
            } fib$r_did_iso_9660_fields;
#endif
        } fib$r_did_overlay;
        unsigned int fib$l_wcc;             /* WILD CARD CONTEXT                */
        __union
        {
            unsigned short int fib$w_nmctl; /* NAME CONTROL BITS                */
            __struct  {
                unsigned short fibdef$$_fill_7 : 3;
                unsigned short fib$v_allver : 1;  /* MATCH ALL VERSIONS               */
                unsigned short fib$v_alltyp : 1;  /* MATCH ALL TYPES                  */
                unsigned short fib$v_allnam : 1;  /* MATCH ALL NAMES                  */
                unsigned short fibdef$$_fill_8 : 2;
                unsigned short fib$v_wild : 1;    /* WILD CARDS IN FILE NAME          */
                unsigned short fib$v_newver : 1;  /* MAXIMIZE VERSION NUMBER          */
                unsigned short fib$v_supersede : 1; /* SUPERSEDE EXISTING FILE        */
                unsigned short fib$v_findfid : 1; /* SEARCH FOR FILE ID               */
                unsigned short fibdef$$_fill_9 : 2;
                unsigned short fib$v_lowver : 1;  /* LOWER VERSION OF FILE EXISTS     */
                unsigned short fib$v_highver : 1; /* HIGHER VERSION OF FILE EXISTS    */
            } fib$r_nmctl_bits;
        } fib$r_nmctl_overlay;
        __union
        {
            unsigned short int fib$w_exctl; /* EXTEND CONTROL                   */
            __struct  {
                unsigned short fib$v_alcon : 1;   /* ALLOCATE CONTIGUOUS              */
                unsigned short fib$v_alconb : 1;  /* CONTIGUOUS BEST EFFORT           */
                unsigned short fib$v_filcon : 1;  /* MARK FILE CONTIGUOUS             */
                unsigned short fib$v_aldef : 1;   /* ALLOCATE DEFAULT AMOUNT          */
                unsigned short fib$v_allocatr : 1; /* PLACEMENT DATA PRESENT IN ATTRIBUTE LIST  */
                unsigned short fibdef$$_fill_10 : 2;
                unsigned short fib$v_extend : 1;  /* ENABLE EXTENSION                 */
                unsigned short fib$v_trunc : 1;   /* ENABLE TRUNCATION                */
                unsigned short fib$v_nohdrext : 1; /* INHIBIT EXTENSION HEADERS       */
                unsigned short fib$v_markbad : 1; /* MARK BLOCKS BAD                  */
                unsigned short fibdef$$_fill_11 : 3;
                unsigned short fib$v_noplace : 1; /* DON'T ADD PLACEMENT POINTERS IN MOVEFILE */
                unsigned short fib$v_nocharge : 1; /* DON'T CHARGE DISKQUOTA          */
            } fib$r_exctl_bits;
        } fib$r_exctl_overlay;
        unsigned int fib$l_exsz;            /* EXTEND SIZE                      */
        unsigned int fib$l_exvbn;           /* EXTENSION VBN                    */
        __union
        {
            unsigned char fib$b_alopts;     /* ALLOCATION OPTIONS               */
            __struct  {
                unsigned char fib$v_exact : 1;   /* EXACT PLACEMENT REQUIRED         */
                unsigned char fib$v_oncyl : 1;   /* PUT ALLOCATION ON ONE CYLINDER   */
                unsigned char fib$v_fill_2 : 6;
            } fib$r_alopts_bits;
        } fib$r_alopts_overlay;
        unsigned char fib$b_alalign;        /* ALLOCATION ALIGNMENT             */
        __union
        {
            unsigned short int fib$w_alloc [5]; /* ALLOCATION LOCATION          */
            __struct  {
                __union  {
                    unsigned short int fib$w_loc_fid [3]; /* RELATED FILE ID    */
                    __struct  {
                        unsigned short int fib$w_loc_num; /* RELATED FILE NUMBER  */
                        unsigned short int fib$w_loc_seq; /* FILE SEQUENCE NUMBER  */
                        __union  {
                            unsigned short int fib$w_loc_rvn; /* RELATED RVN    */
                            __struct  {
                                unsigned char fib$b_loc_rvn; /* SHORT FORM RVN  */
                                unsigned char fib$b_loc_nmx; /* EXTENDED FILE NUMBER  */
                            } fib$r_loc_rvn_fields;
                        } fib$r_loc_rvn_overlay;
                    } fib$r_loc_fid_fields;
                } fib$r_loc_fid_overlay;
                unsigned int fib$l_loc_addr; /* LOCATION ADDRESS (VBN, LBN, CYL)  */
            } fib$r_alloc_fields;
        } fib$r_alloc_overlay;
        unsigned short int fib$w_verlimit;  /* DIRECTORY ENTRY VERSION LIMIT    */
        unsigned char fib$b_agent_mode;     /* AGENTS ACCESS MODE               */
        unsigned char fib$b_ru_facility;    /* RECOVERABLE-FACILITY CODE        */
        unsigned int fib$l_aclctx;          /* ACL CONTEXT FOR READ             */
        unsigned int fib$l_acl_status;      /* RETURN STATUS FROM ACL OPERATION */
        __union
        {
            unsigned int fib$l_status;      /* GENERAL STATUS LONGWORD          */
            __struct  {
                unsigned fib$v_alt_req : 1; /* ALTERNATE ACCESS REQUIRED        */
                unsigned fib$v_alt_granted : 1; /* ALTERNATE ACCESS GRANTED     */
                unsigned fib$v_diracl : 1;  /* DO DIRECTORY ACL PROPAGATION     */
                unsigned fib$v_propagate : 1; /* DO PROPAGATION ON ENTER/MODIFY */
                unsigned fib$v_nocopyacl : 1; /* DON'T PROPAGATE THE ACL        */
                unsigned fib$v_nocopyowner : 1; /* DON'T PROPAGATE THE OWNER UIC */
                unsigned fib$v_nocopyprot : 1; /* DON'T PROPAGATE THE SOGW PROTECTION */
                unsigned fib$v_exclprevious : 1; /* EXCLUDE THE PREVIOUS VERSION FROM PROPAGATION */
                unsigned fib$v_alias_entry : 1; /* FILE ACCESSED VIA AN ALIAS DIRECTORY ENTRY */
                unsigned fib$v_wildfid_flag : 1; /* WILDCARD FID FLAG           */
                unsigned fib$v_fill_3 : 6;
            } fib$r_status_bits;
        } fib$r_status_overlay;
        unsigned int fib$l_alt_access;      /* ALTERNATE ACCESS MASK            */
        __union
        {
            __struct  {
                unsigned int fib$l_mov_svbn; /* starting VBN (MOVEFILE)         */
                unsigned int fib$l_mov_vbncnt; /* count of VBNs (MOVEFILE)      */
            } fib$r_movfildp;
        } fib$r_funcdepend;
        unsigned short int fib$w_file_hdrseq_incr; /*header seq no increment for indiv file */
        unsigned short int fib$w_dir_hdrseq_incr; /*header seq no increment for directory file */
        unsigned short int fib$w_file_dataseq_incr; /*data seq no increment for indiv file */
        unsigned short int fib$w_dir_dataseq_incr; /*data seq no increment for driectory file */
    } ;

#if !defined(__VAXC) && !defined(VAXC)
#define fib$l_acctl fib$r_acctl_overlay.fib$l_acctl
#define fib$v_nowrite fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_nowrite
#define fib$v_dlock fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_dlock
#define fib$v_blk_lock fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_blk_lock
#define fib$v_fastrdsqnos fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_fastrdsqnos
#define fib$v_spool fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_spool
#define fib$v_writeck fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_writeck
#define fib$v_seqonly fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_seqonly
#define fib$v_seqno fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_seqno
#define fib$v_write fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_write
#define fib$v_readck fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_readck
#define fib$v_noread fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_noread
#define fib$v_notrunc fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_notrunc
#define fib$v_control fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_control
#define fib$v_no_read_data fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_no_read_data
#define fib$v_execute fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_execute
#define fib$v_prsrv_atr fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_prsrv_atr
#define fib$v_rmslock fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_rmslock
#define fib$v_writethru fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_writethru
#define fib$v_nolock fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_nolock
#define fib$v_norecord fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_norecord
#define fib$v_noverify fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_noverify
#define fib$v_change_vol fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_change_vol
#define fib$v_rewind fib$r_acctl_overlay.fib$r_acctl_bits1.fib$v_rewind
#define fib$v_curpos fib$r_acctl_overlay.fib$r_acctl_bits1.fib$v_curpos
#define fib$v_update fib$r_acctl_overlay.fib$r_acctl_bits1.fib$v_update
#define fib$b_wsize fib$r_acctl_overlay.fib$r_acctl_fields2.fib$b_wsize
#define fib$w_fid fib$r_fid_overlay.fib$w_fid
#define fib$w_fid_num fib$r_fid_overlay.fib$r_fid_fields.fib$w_fid_num
#define fib$w_fid_seq fib$r_fid_overlay.fib$r_fid_fields.fib$w_fid_seq
#define fib$w_fid_rvn fib$r_fid_overlay.fib$r_fid_fields.fib$r_fid_rvn_overlay.fib$w_fid_rvn
#define fib$b_fid_rvn fib$r_fid_overlay.fib$r_fid_fields.fib$r_fid_rvn_overlay.fib$r_fid_rvn_fields.fib$b_fid_rvn
#define fib$b_fid_nmx fib$r_fid_overlay.fib$r_fid_fields.fib$r_fid_rvn_overlay.fib$r_fid_rvn_fields.fib$b_fid_nmx
#define fib$w_fid_dirnum fib$r_fid_overlay.fib$r_fid_iso_9660_fields.fib$w_fid_dirnum
#define fib$l_fid_recnum fib$r_fid_overlay.fib$r_fid_iso_9660_fields.fib$l_fid_recnum
#define fib$w_did fib$r_did_overlay.fib$w_did
#define fib$w_did_num fib$r_did_overlay.fib$r_did_fields.fib$w_did_num
#define fib$w_did_seq fib$r_did_overlay.fib$r_did_fields.fib$w_did_seq
#define fib$w_did_rvn fib$r_did_overlay.fib$r_did_fields.fib$r_did_rvn_overlay.fib$w_did_rvn
#define fib$b_did_rvn fib$r_did_overlay.fib$r_did_fields.fib$r_did_rvn_overlay.fib$r_did_rvn_fields.fib$b_did_rvn
#define fib$b_did_nmx fib$r_did_overlay.fib$r_did_fields.fib$r_did_rvn_overlay.fib$r_did_rvn_fields.fib$b_did_nmx
#define fib$w_did_dirnum fib$r_did_overlay.fib$r_did_iso_9660_fields.fib$w_did_dirnum
#define fib$l_did_recnum fib$r_did_overlay.fib$r_did_iso_9660_fields.fib$l_did_recnum
#define fib$w_nmctl fib$r_nmctl_overlay.fib$w_nmctl
#define fib$v_allver fib$r_nmctl_overlay.fib$r_nmctl_bits.fib$v_allver
#define fib$v_alltyp fib$r_nmctl_overlay.fib$r_nmctl_bits.fib$v_alltyp
#define fib$v_allnam fib$r_nmctl_overlay.fib$r_nmctl_bits.fib$v_allnam
#define fib$v_wild fib$r_nmctl_overlay.fib$r_nmctl_bits.fib$v_wild
#define fib$v_newver fib$r_nmctl_overlay.fib$r_nmctl_bits.fib$v_newver
#define fib$v_supersede fib$r_nmctl_overlay.fib$r_nmctl_bits.fib$v_supersede
#define fib$v_findfid fib$r_nmctl_overlay.fib$r_nmctl_bits.fib$v_findfid
#define fib$v_lowver fib$r_nmctl_overlay.fib$r_nmctl_bits.fib$v_lowver
#define fib$v_highver fib$r_nmctl_overlay.fib$r_nmctl_bits.fib$v_highver
#define fib$w_exctl fib$r_exctl_overlay.fib$w_exctl
#define fib$v_alcon fib$r_exctl_overlay.fib$r_exctl_bits.fib$v_alcon
#define fib$v_alconb fib$r_exctl_overlay.fib$r_exctl_bits.fib$v_alconb
#define fib$v_filcon fib$r_exctl_overlay.fib$r_exctl_bits.fib$v_filcon
#define fib$v_aldef fib$r_exctl_overlay.fib$r_exctl_bits.fib$v_aldef
#define fib$v_allocatr fib$r_exctl_overlay.fib$r_exctl_bits.fib$v_allocatr
#define fib$v_extend fib$r_exctl_overlay.fib$r_exctl_bits.fib$v_extend
#define fib$v_trunc fib$r_exctl_overlay.fib$r_exctl_bits.fib$v_trunc
#define fib$v_nohdrext fib$r_exctl_overlay.fib$r_exctl_bits.fib$v_nohdrext
#define fib$v_markbad fib$r_exctl_overlay.fib$r_exctl_bits.fib$v_markbad
#define fib$v_noplace fib$r_exctl_overlay.fib$r_exctl_bits.fib$v_noplace
#define fib$v_nocharge fib$r_exctl_overlay.fib$r_exctl_bits.fib$v_nocharge
#define fib$b_alopts fib$r_alopts_overlay.fib$b_alopts
#define fib$v_exact fib$r_alopts_overlay.fib$r_alopts_bits.fib$v_exact
#define fib$v_oncyl fib$r_alopts_overlay.fib$r_alopts_bits.fib$v_oncyl
#define fib$w_alloc fib$r_alloc_overlay.fib$w_alloc
#define fib$w_loc_fid fib$r_alloc_overlay.fib$r_alloc_fields.fib$r_loc_fid_overlay.fib$w_loc_fid
#define fib$w_loc_num fib$r_alloc_overlay.fib$r_alloc_fields.fib$r_loc_fid_overlay.fib$r_loc_fid_fields.fib$w_loc_num
#define fib$w_loc_seq fib$r_alloc_overlay.fib$r_alloc_fields.fib$r_loc_fid_overlay.fib$r_loc_fid_fields.fib$w_loc_seq
#define fib$w_loc_rvn fib$r_alloc_overlay.fib$r_alloc_fields.fib$r_loc_fid_overlay.fib$r_loc_fid_fields.fib$r_loc_rvn_overlay.fib$w\
_loc_rvn
#define fib$b_loc_rvn fib$r_alloc_overlay.fib$r_alloc_fields.fib$r_loc_fid_overlay.fib$r_loc_fid_fields.fib$r_loc_rvn_overlay.fib$r\
_loc_rvn_fields.fib$b_loc_rvn
#define fib$b_loc_nmx fib$r_alloc_overlay.fib$r_alloc_fields.fib$r_loc_fid_overlay.fib$r_loc_fid_fields.fib$r_loc_rvn_overlay.fib$r\
_loc_rvn_fields.fib$b_loc_nmx
#define fib$l_loc_addr fib$r_alloc_overlay.fib$r_alloc_fields.fib$l_loc_addr
#define fib$l_status fib$r_status_overlay.fib$l_status
#define fib$v_alt_req fib$r_status_overlay.fib$r_status_bits.fib$v_alt_req
#define fib$v_alt_granted fib$r_status_overlay.fib$r_status_bits.fib$v_alt_granted
#define fib$v_diracl fib$r_status_overlay.fib$r_status_bits.fib$v_diracl
#define fib$v_propagate fib$r_status_overlay.fib$r_status_bits.fib$v_propagate
#define fib$v_nocopyacl fib$r_status_overlay.fib$r_status_bits.fib$v_nocopyacl
#define fib$v_nocopyowner fib$r_status_overlay.fib$r_status_bits.fib$v_nocopyowner
#define fib$v_nocopyprot fib$r_status_overlay.fib$r_status_bits.fib$v_nocopyprot
#define fib$v_exclprevious fib$r_status_overlay.fib$r_status_bits.fib$v_exclprevious
#define fib$v_alias_entry fib$r_status_overlay.fib$r_status_bits.fib$v_alias_entry
#define fib$v_wildfid_flag fib$r_status_overlay.fib$r_status_bits.fib$v_wildfid_flag
#define fib$l_mov_svbn fib$r_funcdepend.fib$r_movfildp.fib$l_mov_svbn
#define fib$l_mov_vbncnt fib$r_funcdepend.fib$r_movfildp.fib$l_mov_vbncnt
#endif		/* #if !defined(__VAXC) && !defined(VAXC) */

#define FIB$C_REWINDVOL 1               /* REWIND VOLUME SET                */
#define FIB$C_POSEND 2                  /* POSITION TO END OF VOLUME SET    */
#define FIB$C_NEXTVOL 3                 /* FORCE NEXT VOLUME                */
#define FIB$C_SPACE 4                   /* SPACE MAGNETIC TAPE              */
#define FIB$C_ILLEGAL 5                 /*                                  */
#define FIB$C_REWINDFIL 6               /* REWIND FILE                      */
#define FIB$C_LOCK_VOL 7                /* LOCK VOLUME AGAINST ALLOCATION   */
#define FIB$C_UNLK_VOL 8                /* UNLOCK VOLUME                    */
    /* QUOTA FILE OPERATIONS                                                    */
#define FIB$C_ENA_QUOTA 9               /* ENABLE QUOTA FILE                */
#define FIB$C_DSA_QUOTA 10              /* DISABLE QUOTA FILE               */
#define FIB$C_ADD_QUOTA 11              /* ADD QUOTA FILE ENTRY             */
#define FIB$C_EXA_QUOTA 12              /* EXAMINE QUOTA FILE ENTRY         */
#define FIB$C_MOD_QUOTA 13              /* MODIFY QUOTA FILE ENTRY          */
#define FIB$C_REM_QUOTA 14              /* REMOVE QUOTA FILE ENTRY          */
#define FIB$C_USEREOT 15                /* ENABLE USER END OF TAPE HANDLING  */
#define FIB$C_REMAP 16                  /* REMAP FILE WINDOW                */
#define FIB$C_CLSEREXCP 17              /* ALLOW THE USER TO CLEAR A SERIOUS EXCP FROM A TAPE DRIVE */
#define FIB$C_FLUSH_CACHE 18            /* FLUSH SELECTED CACHE             */
#define FIB$C_FORCE_MV 19               /* FORCE MOUNT VERIFICATION TO OCCUR */
#define FIB$C_VALIDATE_VOLUME 20        /* VALIDATE VOLUME SET FOLLOWING SNAPSHOT FILE BOOT */
#define FIB$C_VALIDATE_FILE 21          /* VALIDATE FILE FOLLOWING SNAPSHOT FILE BOOT */
#define FIB$C_UPDATE_VOL_PROFILE 22     /* UPDATE A VOLUME'S (PERMANENT) SECURITY PROFILE */
#define FIB$C_UPDATE_FIL_PROFILE 23     /* UPDATE A FILE'S (PERMANENT) SECURITY PROFILE */
#define FIB$C_CHECK_PROTECTION 24       /* PERFORM FILE OR VOLUME PROTECTION CHECK */
#define FIB$C_ADD_THREAD 25             /* ADD A NEW THREAD                 */
#define FIB$C_REM_THREAD 26             /* REMOVE A THREAD                  */
#define FIB$C_EXA_THREAD 27             /* INQUIRE ABOUT THREADS            */
#define FIB$C_CHECK_PATH 28             /* FIND DIRSEQ VAL AND ARM BLKAST   */
#define FIB$C_ENA_DW 29                 /* ENABLE DEFERRED WRITE            */
#define FIB$C_DSA_DW 30                 /* DISABLE DEFERRED WRITE           */
#define FIB$K_MTALEN 28                 /* LENGTH OF MTAACP DATA            */
#define FIB$C_MTALEN 28                 /* LENGTH OF MTAACP DATA            */
    /*                                                                          */
#define FIB$C_FID_CACHE 1               /* FLUSH THE FID CACHE              */
#define FIB$C_EXTENT_CACHE 2            /* FLUSH THE EXTENT CACHE           */
#define FIB$C_QUOTA_CACHE 3             /* FLUSH THE QUOTA CACHE            */
#define FIB$C_BFRD_CACHE 4              /* FLUSH THE BFRD CACHE             */
#define FIB$C_VBN_CACHE 5               /* FLUSH THE VBN CACHE              */
    /*                                                                          */
#define FIB$M_ALL_MEM 0x1
#define FIB$M_ALL_GRP 0x2
#define FIB$M_MOD_USE 0x4
#define FIB$M_MOD_PERM 0x8
#define FIB$M_MOD_OVER 0x10
    struct fibdef1
    {
        char fibdef$$_fill_14 [22];
        unsigned short int fib$w_cntrlfunc; /* ACP CONTROL FUNCTION             */
        /* DEFINE ACP CONTROL FUNCTION CODES                                        */
        /*                                                                          */
        __union
        {
            unsigned int fib$l_cntrlval;    /* ACP CONTROL FUNCTION VALUE PARAMETER  */
            /* CACHE IDENTIFIER CODES FOR FLUSH_CACHE                                   */
            /*                                                                          */
            /* CONTROL BITS FOR QUOTA FILE OPERATIONS                                   */
            /*                                                                          */
            __struct  {
                unsigned fib$v_all_mem : 1; /* MATCH ALL MEMBER NUMBERS         */
                unsigned fib$v_all_grp : 1; /* MATCH ALL GROUP NUMBERS          */
                unsigned fib$v_mod_use : 1; /* MODIFY USAGE DATA                */
                unsigned fib$v_mod_perm : 1; /* MODIFY PERMANENT QUOTA          */
                unsigned fib$v_mod_over : 1; /* MODIFY OVERDRAFT LIMIT          */
                unsigned fib$v_fill_4 : 3;
            } fib$r_cntrlval_bits;
        } fib$r_cntrlval_overlay;
    } ;

#if !defined(__VAXC) && !defined(VAXC)
#define fib$l_cntrlval fib$r_cntrlval_overlay.fib$l_cntrlval
#define fib$v_all_mem fib$r_cntrlval_overlay.fib$r_cntrlval_bits.fib$v_all_mem
#define fib$v_all_grp fib$r_cntrlval_overlay.fib$r_cntrlval_bits.fib$v_all_grp
#define fib$v_mod_use fib$r_cntrlval_overlay.fib$r_cntrlval_bits.fib$v_mod_use
#define fib$v_mod_perm fib$r_cntrlval_overlay.fib$r_cntrlval_bits.fib$v_mod_perm
#define fib$v_mod_over fib$r_cntrlval_overlay.fib$r_cntrlval_bits.fib$v_mod_over
#endif		/* #if !defined(__VAXC) && !defined(VAXC) */


#ifdef __cplusplus
}
#endif
#pragma standard

#endif /* __FIBDEF_LOADED */

