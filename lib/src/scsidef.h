#ifndef SCSIDEF_H
#define SCSIDEF_H

#include <vms_types.h>

#define INQ_CMD$K_INQ_OPCODE 18
#define INQ_CMD$M_ENABLE_VPD 0x1
#define INQ_CMD$M_COMMAND_DATA 0x2
#define INQ_CMD$K_LENGTH 6

struct _inq_cmd
{
    UINT8 inq_cmd$b_opcode;
    union
    {
        UINT8 inq_cmd$b_flags;
        struct
        {
            unsigned inq_cmd$v_enable_vpd : 1;
            unsigned inq_cmd$v_command_data : 1;
            unsigned inq_cmd$v_reserved1 : 6;
        };
    };
    UINT8 inq_cmd$b_vpd_page;
    UINT8 inq_cmd$b_reserved2;
    UINT8 inq_cmd$b_alloc_length;
    UINT8 inq_cmd$b_control;
    INT8 inq_cmd$b_fill_0_ [2];
};

#define SCSI$INQ$M_DEVICE_TYPE 0x1F
#define SCSI$C_DISK 0
#define SCSI$C_TAPE 1
#define SCSI$C_PRINTER 2
#define SCSI$C_CPU 3
#define SCSI$C_WORM 4
#define SCSI$C_CDROM 5
#define SCSI$C_SCANNER 6
#define SCSI$C_OPTICAL 7
#define SCSI$C_JUKEBOX 8
#define SCSI$C_INTERCONNECT 9


#define SCSI$C_SCC 12
#define SCSI$C_ENC_SVCS 13
#define SCSI$C_RBC 14


#define SCSI$C_UNKNOWN 31
#define SCSI$INQ$M_QUALIFIER 0xE0
#define SCSI$C_LUN_CONNECTED 0
#define SCSI$C_LUN_NOCONNECT 1
#define SCSI$C_LUN_UNAVAILABLE 3
#define SCSI$INQ$M_MODIFIER 0x7F
#define SCSI$INQ$M_RMB 0x80
#define SCSI$INQ$M_ANSI_VERSION 0x7
#define SCSI$C_ANSI_UNKNOWN 0
#define SCSI$C_ANSI_SCSI_1 1
#define SCSI$C_ANSI_SCSI_2 2
#define SCSI$C_ANSI_SCSI_3_SPC1 3
#define SCSI$C_ANSI_SCSI_3_SPC2 4
#define SCSI$C_ANSI_SCSI_3 3
#define SCSI$INQ$M_ECMA_VERSION 0x38
#define SCSI$INQ$M_ISO_VERSION 0xC0
#define SCSI$INQ$M_RESP_DATA_FORMAT 0xF
#define SCSI$C_SCSI_1 0
#define SCSI$C_OTHER 1
#define SCSI$C_SCSI_2 2
#define SCSI$INQ$M_HIERARCH_SUPPORT 0x10
#define SCSI$INQ$M_NORMAL_ACA 0x20
#define SCSI$INQ$M_TRMIOP 0x40
#define SCSI$INQ$M_AENC 0x80
#define SCSI$INQ$M_SSC_SUPPORT 0x80
#define SCSI$INQ$M_ADDRESS_16BIT 0x1
#define SCSI$INQ$M_ADDRESS_32BIT 0x2
#define SCSI$INQ$M_REQ_ACK_XFERS 0x4
#define SCSI$INQ$M_MEDIUM_CHANGER 0x8
#define SCSI$INQ$M_MULTI_PORT 0x10
#define SCSI$INQ$M_VENDOR_SPEC1 0x20
#define SCSI$INQ$M_ENCLOSURE_SVCS 0x40
#define SCSI$INQ$M_BASIC_QUEUING 0x80
#define SCSI$INQ$M_SFTRE 0x1
#define SCSI$INQ$M_CMDQUE 0x2
#define SCSI$INQ$M_XFER_DISABLE 0x4
#define SCSI$INQ$M_LINKED 0x8
#define SCSI$INQ$M_SYNC 0x10
#define SCSI$INQ$M_WBUS16 0x20
#define SCSI$INQ$M_WBUS32 0x40
#define SCSI$INQ$M_RELADR 0x80
#define INQ_DATA$K_STANDARD_LENGTH 36
#define SCSI$INQ$M_RESERVED3 0xF
#define SCSI$INQ$M_OTHER_STATUS 0x10
#define SCSI$INQ$M_MULTIBUS 0x20
#define SCSI$INQ$M_BOOT_PREFERENCE 0x40
#define SCSI$INQ$M_EXT_LUN_SUPPORT 0x80
#define INQ_DATA$K_LENGTH 255

struct _inquiry_data
{
    struct
    {
        unsigned scsi$inq$v_device_type : 5;
        unsigned scsi$inq$v_qualifier : 3;
    };
    struct
    {
        unsigned scsi$inq$v_modifier : 7;
        unsigned scsi$inq$v_rmb : 1;
    };
    struct
    {
        unsigned scsi$inq$v_ansi_version : 3;
        unsigned scsi$inq$v_ecma_version : 3;
        unsigned scsi$inq$v_iso_version : 2;
    };
    struct
    {
        unsigned scsi$inq$v_resp_data_format : 4;
        unsigned scsi$inq$v_hierarch_support : 1;
        unsigned scsi$inq$v_normal_aca : 1;
        unsigned scsi$inq$v_trmiop : 1;
        unsigned scsi$inq$v_aenc : 1;
    };
    UINT8 scsi$inq$b_add_length;
    struct
    {
        unsigned scsi$inq$v_reserved2 : 7;
        unsigned scsi$inq$v_ssc_support : 1;
    };
    struct
    {
        unsigned scsi$inq$v_address_16bit : 1;
        unsigned scsi$inq$v_address_32bit : 1;
        unsigned scsi$inq$v_req_ack_xfers : 1;
        unsigned scsi$inq$v_medium_changer : 1;
        unsigned scsi$inq$v_multi_port : 1;
        unsigned scsi$inq$v_vendor_spec1 : 1;
        unsigned scsi$inq$v_enclosure_svcs : 1;
        unsigned scsi$inq$v_basic_queuing : 1;
    };
    struct
    {
        unsigned scsi$inq$v_sftre : 1;
        unsigned scsi$inq$v_cmdque : 1;
        unsigned scsi$inq$v_xfer_disable : 1;
        unsigned scsi$inq$v_linked : 1;
        unsigned scsi$inq$v_sync : 1;
        unsigned scsi$inq$v_wbus16 : 1;
        unsigned scsi$inq$v_wbus32 : 1;
        unsigned scsi$inq$v_reladr : 1;
    };
    INT8 scsi$inq$b_vendor_id [8];
    INT8 scsi$inq$b_product_id [16];
    INT8 scsi$inq$b_product_revision [4];
    UINT8 scsi$inq$b_this_serial_no [10];
    UINT8 scsi$inq$b_other_serial_no [10];
    UINT8 scsi$inq$b_reserved2 [40];
    struct
    {
        unsigned scsi$inq$v_reserved3 : 4;
        unsigned scsi$inq$v_other_status : 1;
        unsigned scsi$inq$v_multibus : 1;
        unsigned scsi$inq$v_boot_preference : 1;
        unsigned scsi$inq$v_ext_lun_support : 1;
    };
    UINT8 scsi$inq$b_preferred_luns [32];
    UINT8 scsi$inq$b_reserved4;
    UINT8 scsi$inq$b_node_id [8];
    UINT64 scsi$inq$q_reserved5 [13];
    UINT8 scsi$inq$b_allocation_class [4];
    UINT8 scsi$inq$b_reserved6 [9];
    UINT8 scsi$inq$b_fill1;
};

#define inquiry_data _inquiry_data

struct mode_param_hdr_6
{
    UINT8 scsi$mph6$b_data_length;
    UINT8 scsi$mph6$b_medium_type;
    UINT8 scsi$mph6$b_device_param;
    UINT8 scsi$mph6$b_block_length;
};

struct mode_param_hdr_10
{
    UINT16 scsi$mph10$w_data_length;
    UINT8 scsi$mph10$b_medium_type;
    UINT8 scsi$mph10$b_device_param;
    UINT8 scsi$mph10$b_reserved1 [2];
    UINT16 scsi$mph10$w_block_length;
};

#define SCSI$DK$C_DEFAULT 0
#define SCSI$DK$C_SS 1
#define SCSI$DK$C_DS 2
#define SCSI$DK$C_DD 3

#define SCSI$DK$M_DPOFUA 0x10
#define SCSI$DK$M_WP 0x80

struct disk_specific_param
{
    unsigned scsi$dk$v_reserved1 : 4;
    unsigned scsi$dk$v_dpofua : 1;
    unsigned scsi$dk$v_reserved2 : 2;
    unsigned scsi$dk$v_wp : 1;
};

#define SCSI$CD$M_EBC 0x1
#define SCSI$CD$M_CACHE 0x10

struct cdrom_specific_param
{
    unsigned scsi$cd$v_ebc : 1;
    unsigned scsi$cd$v_reserved1 : 3;
    unsigned scsi$cd$v_cache : 1;
    unsigned scsi$cd$v_reserved2 : 3;
};




struct mode_parameter
{
    UINT8 scsi$mpbd$b_density;
    UINT8 scsi$mpbd$b_block [3];
    INT8 scsi$mpbd$b_reserved1;
    UINT8 scsi$mpbd$b_length [3];
};

#define SCSI$IDENT$M_LUNTRN 0x7
#define SCSI$IDENT$M_LUNTAR 0x20
#define SCSI$IDENT$M_DISC_PRIV 0x40
#define SCSI$IDENT$M_IDENTIFY 0x80

struct identify_message
{
    struct
    {
        unsigned scsi$ident$v_luntrn : 3;
        unsigned scsi$ident$v_reserved_1 : 1;
        unsigned scsi$ident$v_reserved_2 : 1;
        unsigned scsi$ident$v_luntar : 1;
        unsigned scsi$ident$v_disc_priv : 1;
        unsigned scsi$ident$v_identify : 1;
    };
};

#define SCSI$C_GOOD 0
#define SCSI$C_CHECK_CONDITION 2
#define SCSI$C_CONDITION_MET 4
#define SCSI$C_BUSY 8
#define SCSI$C_INTERMEDIATE 16
#define SCSI$C_INTERMEDIATE_COND_MET 20
#define SCSI$C_RESERVATION_CONFLICT 24
#define SCSI$C_COMMAND_TERMINATED 34
#define SCSI$C_QUEUE_FULL 40
#define SCSI$M_STATUS_BYTE_RESERVED 193



#define SCSI$C_COMMAND_COMPLETE 0
#define SCSI$C_EXTENDED_MESSAGE 1
#define SCSI$C_SAVE_DATA_POINTERS 2
#define SCSI$C_RESTORE_POINTERS 3
#define SCSI$C_DISCONNECT 4
#define SCSI$C_INITIATOR_DETECT_ERROR 5
#define SCSI$C_ABORT 6
#define SCSI$C_MESSAGE_REJECT 7
#define SCSI$C_NO_OPERATION 8
#define SCSI$C_MESSAGE_PARITY_ERROR 9
#define SCSI$C_LINKED_COMMAND 10
#define SCSI$C_LINKED_COMMAND_FLAGED 11
#define SCSI$C_BUS_DEVICE_RESET 12
#define SCSI$C_ABORT_TAG 13
#define SCSI$C_CLEAR_QUEUE 14
#define SCSI$C_INITIATE_RECOVERY 15
#define SCSI$C_RELEASE_RECOVERY 16
#define SCSI$C_TERMINATE_IO_PROCESS 17
#define SCSI$C_CLEAR_ACA 22
#define SCSI$C_SIMPLE_QUEUE_TAG 32
#define SCSI$C_HEAD_OF_QUEUE_TAG 33
#define SCSI$C_ORDERED_QUEUE_TAG 34
#define SCSI$C_IGNORE_WIDE_RESIDUE 35



#define SCSI$C_MODIFY_DATA_POINTER 0
#define SCSI$C_SYNCH_DATA_TRANSFER 1
#define SCSI$C_WIDE_DATA_TRANSFER 3



#define SCSI$SNS$M_ERROR_CODE 0x7F
#define SCSI$SC1$C_CURRENT 112
#define SCSI$SC1$C_DEFERRED 113
#define SCSI$SNS$M_VALID 0x80
#define SCSI$SNS$M_SENSE_KEY 0xF
#define SCSI$SNS$M_ILI 0x20
#define SCSI$SNS$M_EOM 0x40
#define SCSI$SNS$M_FILEMARK 0x80
#define SCSI$C_RECOVERED_DATA 23

#define SCSI$SNS$M_BIT_POINTER 0x7
#define SCSI$SNS$M_BPV 0x8
#define SCSI$SNS$M_C_D 0x40
#define SCSI$SNS$M_SKSV 0x80

struct sense_data
{
    struct
    {
        unsigned scsi$sns$v_error_code : 7;
        unsigned scsi$sns$v_valid : 1;
    };
    UINT8 scsi$sns$b_segement;
    struct
    {
        unsigned scsi$sns$v_sense_key : 4;
        unsigned scsi$sns$v_reserved1 : 1;
        unsigned scsi$sns$v_ili : 1;
        unsigned scsi$sns$v_eom : 1;
        unsigned scsi$sns$v_filemark : 1;
    };
    union
    {
        UINT32 scsi$sns$l_lba_address;
        UINT32 scsi$sns$l_residual_length;
        UINT32 scsi$sns$l_residual_blocks;
        UINT32 scsi$sns$l_data_blocks;
        UINT32 scsi$sns$l_file_marks;
        UINT32 scsi$sns$l_set_marks;
    };
    UINT8 scsi$sns$b_add_sense_len;
    union
    {
        UINT8 scsi$sns$b_sense_information [4];
    };
    UINT8 scsi$sns$b_add_sense_code;







    UINT8 scsi$sns$b_add_sense_qual;
    UINT8 scsi$sns$b_fru_code;
    struct
    {
        unsigned scsi$sns$v_bit_pointer : 3;
        unsigned scsi$sns$v_bpv : 1;
        unsigned scsi$sns$v_reserved2 : 2;
        unsigned scsi$sns$v_c_d : 1;
        unsigned scsi$sns$v_sksv : 1;
        UINT16 scsi$sns$w_field_pointer;
    };
    struct
    {
        UINT8 scsi$sns$b_add_sense_bytes [1];
    };
};

#define SCSI$C_NO_SENSE 0
#define SCSI$C_RECOVERED_ERROR 1
#define SCSI$C_NOT_READY 2
#define SCSI$C_MEDIUM_ERROR 3
#define SCSI$C_HARDWARE_ERROR 4
#define SCSI$C_ILLEGAL_REQUEST 5
#define SCSI$C_UNIT_ATTENTION 6
#define SCSI$C_DATA_PROTECT 7
#define SCSI$C_BLANK_CHECK 8
#define SCSI$C_VENDOR_SPECIFIC 9
#define SCSI$C_COPY_ABORTED 10
#define SCSI$C_ABORTED_COMMAND 11
#define SCSI$C_EQUAL 12
#define SCSI$C_VOLUME_OVERFLOW 13
#define SCSI$C_MISCOMPARE 14
#define SCSI$C_RESERVED 15



#define SCSI$C_AUDIO_CONTROL_PAGE 14
#define SCSI$C_MEDIA_CHANGE 40

#define SCSI$C_MODE_CHANGE 42



#define SCSI$C_INCOMPATIBLE_MEDIA 48




#define SCSI$C_DRIVE_NOT_READY 4





#define SCSI$C_MEDIUM_NOT_PRESENT 58




#define SCSI$OPT$C_MEDIA_DEFAULT 0
#define SCSI$OPT$C_MEDIA_RO 1
#define SCSI$OPT$C_MEDIA_WORM 2
#define SCSI$OPT$C_MEDIA_ERASE 3
#define SCSI$OPT$C_MEDIA_RO_WORM 4
#define SCSI$OPT$C_MEDIA_RO_ERASE 5
#define SCSI$OPT$C_MEDIA_WORM_ERASE 6



#define SCSI$PGCD$C_VENDOR_SPECIFIC 0
#define SCSI$PGCD$C_READ_WRITE_ERR 1
#define SCSI$PGCD$C_DISCONNECT_REC 2
#define SCSI$PGCD$C_FORMAT_DEVICE 3
#define SCSI$PGCD$C_RIGID_DISK 4
#define SCSI$PGCD$C_FLEXIBLE_DISK 5
#define SCSI$PGCD$C_RESERVED 6
#define SCSI$PGCD$C_VERIFY_ERROR 7
#define SCSI$PGCD$C_CACHING 8
#define SCSI$PGCD$C_PERIPHERAL 9
#define SCSI$PGCD$C_CONTROL_MODE 10
#define SCSI$PGCD$C_MEDIUM_TYPES 11
#define SCSI$PGCD$C_NOTCH_PARTION 12
#define SCSI$PGCD$C_ALL_PAGES 63
#define SCSI$PGCD$C_MAX_PAGE_CODE 63
#define SCSI$PGCD$M_CURRENT 0
#define SCSI$PGCD$M_CHANGEABLE 64
#define SCSI$PGCD$M_DEFAULT 128
#define SCSI$PGCD$M_SAVED 192
#define SCSI$PGCD$C_PAGE_SIZE 512



#define SCSI$ACP$M_PAGE_CODE 0x3F
#define SCSI$ACP$C_PAGE_CODE 14
#define SCSI$ACP$M_PS 0x80
#define SCSI$S_HEADER 2
#define SCSI$ACP$C_PAGE_LENGTH 14
#define SCSI$ACP$M_SOTC 0x2
#define SCSI$ACP$M_IMMED 0x4
#define SCSI$ACP$M_LBA_FORMAT 0xF000000
#define SCSI$ACP$M_CHANNEL_0 0x1
#define SCSI$ACP$M_CHANNEL_1 0x2
#define SCSI$ACP$M_CHANNEL_2 0x4
#define SCSI$ACP$M_CHANNEL_3 0x8

struct audio_control
{
    struct
    {
        unsigned scsi$acp$v_page_code : 6;
        unsigned scsi$acp$v_reserved1 : 1;
        unsigned scsi$acp$v_ps : 1;
    };
    UINT8 scsi$acp$b_page_length;
    struct
    {
        unsigned scsi$acp$v_reserved2 : 1;
        unsigned scsi$acp$v_sotc : 1;
        unsigned scsi$acp$v_immed : 1;
        unsigned scsi$acp$v_reserved3 : 21;
        unsigned scsi$acp$v_lba_format : 4;
        unsigned scsi$acp$v_fill_3_ : 4;
    };
    UINT16 scsi$acp$w_lbs_per_sec;
    union
    {
        UINT16 scsi$acp$w_channel_volume [4];
        struct
        {
            unsigned scsi$acp$v_channel_0 : 1;
            unsigned scsi$acp$v_channel_1 : 1;
            unsigned scsi$acp$v_channel_2 : 1;
            unsigned scsi$acp$v_channel_3 : 1;
            unsigned scsi$acp$v_reserved4 : 4;
            UINT8 scsi$acp$b_volume;
        };
    };
};

#define SCSI$CMP$M_PAGE_CODE 0x3F
#define SCSI$CMP$C_PAGE_CODE 10
#define SCSI$CMP$M_PS 0x80
#define SCSI$CMP$C_PAGE_LENGTH 6
#define SCSI$CMP$M_RLEC 0x1
#define SCSI$CMP$M_DQUE 0x100
#define SCSI$CMP$M_QERR 0x200
#define SCSI$CMP$M_QAM 0x7000
#define SCSI$QAM$C_RESTRICTED 0
#define SCSI$QAM$C_UNRESTRICTED 1
#define SCSI$CMP$M_EAENP 0x8000
#define SCSI$CMP$M_UAAENP 0x10000
#define SCSI$CMP$M_RAENP 0x20000
#define SCSI$CMP$M_EECA 0x400000

struct control_mode
{
    struct
    {
        unsigned scsi$cmp$v_page_code : 6;
        unsigned scsi$cmp$v_reserved1 : 1;
        unsigned scsi$cmp$v_ps : 1;
    };
    UINT8 scsi$cmp$b_page_length;
    struct
    {
        unsigned scsi$cmp$v_rlec : 1;
        unsigned scsi$cmp$v_reserved2 : 7;
        unsigned scsi$cmp$v_dque : 1;
        unsigned scsi$cmp$v_qerr : 1;
        unsigned scsi$cmp$v_reserved3 : 2;
        unsigned scsi$cmp$v_qam : 3;
        unsigned scsi$cmp$v_eaenp : 1;
        unsigned scsi$cmp$v_uaaenp : 1;
        unsigned scsi$cmp$v_raenp : 1;
        unsigned scsi$cmp$v_reserved4 : 4;
        unsigned scsi$cmp$v_eeca : 1;
        unsigned scsi$cmp$v_fill_4_ : 1;
    };
    INT8 scsi$cmp$b_reserved5;
    UINT16 scsi$cmp$w_ready_aen;
};

#define SCSI$ERP$M_PAGE_CODE 0x3F
#define SCSI$ERP$C_PAGE_CODE 1
#define SCSI$ERP$M_PS 0x80
#define SCSI$ERP$C_PAGE_LENGTH 10
#define SCSI$ERP$M_DCR 0x1
#define SCSI$ERP$M_DTE 0x2
#define SCSI$ERP$M_PER 0x4
#define SCSI$ERP$M_EER 0x8
#define SCSI$ERP$M_RC 0x10
#define SCSI$ERP$M_TB 0x20
#define SCSI$ERP$M_ARRE 0x40
#define SCSI$ERP$M_AWRE 0x80

struct error_recovery
{
    struct
    {
        unsigned scsi$erp$v_page_code : 6;
        unsigned scsi$erp$v_reserved1 : 1;
        unsigned scsi$erp$v_ps : 1;
    };
    UINT8 scsi$erp$b_page_length;
    struct
    {
        unsigned scsi$erp$v_dcr : 1;
        unsigned scsi$erp$v_dte : 1;
        unsigned scsi$erp$v_per : 1;
        unsigned scsi$erp$v_eer : 1;
        unsigned scsi$erp$v_rc : 1;
        unsigned scsi$erp$v_tb : 1;
        unsigned scsi$erp$v_arre : 1;
        unsigned scsi$erp$v_awre : 1;
    };
    UINT8 scsi$erp$b_read_rerty_count;
    UINT8 scsi$erp$b_correction_span;
    UINT8 scsi$erp$b_head_offset_count;
    UINT8 scsi$erp$b_data_strobe_offset;
    UINT8 scsi$erp$b_reserved2;
    UINT8 scsi$erp$b_write_retry_count;
    UINT8 scsi$erp$b_reserved3;
    UINT16 scsi$erp$w_recovery_time_limit;
};

#define SCSI$FMT$M_PAGE_CODE 0x3F
#define SCSI$FMT$C_PAGE_CODE 3
#define SCSI$FMT$M_PS 0x80
#define SCSI$FMT$C_PAGE_LENGTH 22
#define SCSI$FMT$M_SURF 0x10
#define SCSI$FMT$M_RMB 0x20
#define SCSI$FMT$M_HSEC 0x40
#define SCSI$FMT$M_SSEC 0x80

struct format_device
{
    struct
    {
        unsigned scsi$fmt$v_page_code : 6;
        unsigned scsi$fmt$v_reserved1 : 1;
        unsigned scsi$fmt$v_ps : 1;
    };
    UINT8 scsi$fmt$b_page_length;
    UINT16 scsi$fmt$w_tracks;
    UINT16 scsi$fmt$w_alt_sectors;
    UINT16 scsi$fmt$w_alt_tracks;
    UINT16 scsi$fmt$w_alt_tracks_unit;
    UINT16 scsi$fmt$w_sectors;
    UINT16 scsi$fmt$w_sector_size;
    UINT16 scsi$fmt$w_interleave;
    UINT16 scsi$fmt$w_track_skew;
    UINT16 scsi$fmt$w_cyl_skew;
    struct
    {
        unsigned scsi$fmt$v_reserved2 : 4;
        unsigned scsi$fmt$v_surf : 1;
        unsigned scsi$fmt$v_rmb : 1;
        unsigned scsi$fmt$v_hsec : 1;
        unsigned scsi$fmt$v_ssec : 1;
    };
    INT8 scsi$fmt$b_reserved3 [3];
};

#define SCSI$RGD$M_PAGE_CODE 0x3F
#define SCSI$RGD$C_PAGE_CODE 4
#define SCSI$RGD$M_PS 0x80
#define SCSI$RGD$C_PAGE_LENGTH 22
#define SCSI$RGD$M_RPL 0x3
#define SCSI$RGD$C_DISABLED 0
#define SCSI$RGD$C_SLAVE 1
#define SCSI$RGD$C_MASTER 2
#define SCSI$RGD$C_CONTROL 3

struct rigid_disk
{
    struct
    {
        unsigned scsi$rgd$v_page_code : 6;
        unsigned scsi$rgd$v_reserved1 : 1;
        unsigned scsi$rgd$v_ps : 1;
    };
    UINT8 scsi$rgd$b_page_length;
    INT8 scsi$rgd$b_cylinders [3];
    UINT8 scsi$rgd$b_heads;
    UINT16 scsi$rgd$w_cylinder_write;
    UINT16 scsi$rgd$w_cylinder_reduced;
    UINT16 scsi$rgd$w_step_rate;
    UINT16 scsi$rgd$w_landing_zone;
    struct
    {
        unsigned scsi$rgd$v_rpl : 2;
        unsigned scsi$rgd$v_reserved2 : 6;
    };
    UINT8 scsi$rgd$b_rotational_offset;
    UINT8 scsi$rgd$b_reserved3;
    UINT16 scsi$rgd$w_medium_rotation;
    UINT16 scsi$rgd$w_rotational_offset;
};

#define SCSI$FLX$M_PAGE_CODE 0x3F
#define SCSI$FLX$C_PAGE_CODE 5
#define SCSI$FLX$M_PS 0x80
#define SCSI$FLX$C_PAGE_LENGTH 30
#define SCSI$FLX$C_XFR_250KHZ 64000
#define SCSI$FLX$C_XFR_300KHZ 11265
#define SCSI$FLX$C_XFR_500KHZ 62465
#define SCSI$FLX$C_XFR_1MHZ 59395
#define SCSI$FLX$C_XFR_2MHZ 53255
#define SCSI$FLX$C_XFR_5MHZ 34835
#define SCSI$FLX$M_MO 0x20
#define SCSI$FLX$M_SSN 0x40
#define SCSI$FLX$M_TRDY 0x80
#define SCSI$FLX$M_SPC 0xF00
#define SCSI$FLX$M_PIN2 0x1
#define SCSI$FLX$M_PIN34 0x2
#define SCSI$FLX$M_PIN1 0x4
#define SCSI$FLX$M_PIN4 0x8

struct flexible_disk
{
    struct
    {
        unsigned scsi$flx$v_page_code : 6;
        unsigned scsi$flx$v_reserved1 : 1;
        unsigned scsi$flx$v_ps : 1;
    };
    UINT8 scsi$flx$b_page_length;
    UINT16 scsi$flx$w_transfer_rate;

    UINT8 scsi$flx$b_heads;
    UINT8 scsi$flx$b_sectors_track;
    UINT16 scsi$flx$w_sector_size;
    UINT16 scsi$flx$w_cylinders;
    UINT16 scsi$flx$w_cylinder_write;
    UINT16 scsi$flx$w_cylinder_reduced;
    UINT16 scsi$flx$w_step_rate;
    UINT8 scsi$flx$b_step_pulse;
    UINT16 scsi$flx$w_head_settle;
    UINT8 scsi$flx$b_motor_on;
    UINT8 scsi$flx$b_motor_off;
    struct
    {
        unsigned scsi$flx$v_reserved2 : 5;
        unsigned scsi$flx$v_mo : 1;
        unsigned scsi$flx$v_ssn : 1;
        unsigned scsi$flx$v_trdy : 1;
        unsigned scsi$flx$v_spc : 4;
        unsigned scsi$flx$v_reserved3 : 4;
    };
    UINT8 scsi$flx$b_write_comp;
    UINT8 scsi$flx$b_head_load;
    UINT8 scsi$flx$b_head_unload;
    struct
    {
        unsigned scsi$flx$v_pin2 : 1;
        unsigned scsi$flx$v_pin34 : 1;
        unsigned scsi$flx$v_pin1 : 1;
        unsigned scsi$flx$v_pin4 : 1;
        unsigned scsi$flx$v_fill_5_ : 4;
    };
    UINT16 scsi$flx$w_rotation;
    UINT16 scsi$flx$w_reserved4;
};

#define SCSI$SUB$C_SUBQ_CHANNEL_DATA 0
#define SCSI$SUB$C_CD_ROM_POSITION 1
#define SCSI$SUB$C_MCN 2
#define SCSI$SUB$C_ISRC 3



#define SCSI$SUBQ$C_NOT_VALID 0
#define SCSI$SUBQ$C_PLAY 1
#define SCSI$SUBQ$C_PAUSED 2
#define SCSI$SUBQ$C_COMPLETE 3
#define SCSI$SUBQ$C_ERROR 4
#define SCSI$SUBQ$C_ACTIVE 5
#define SCSI$SUBQ$M_CONTROL 0xF
#define SCSI$SUBQ$C_PRE_EMPHASIS 0
#define SCSI$SUBQ$C_COPY_PERMITTED 1
#define SCSI$SUBQ$C_DATA_TRACK 2
#define SCSI$SUBQ$C_FOUR_CHANNEL 3
#define SCSI$SUBQ$M_ADR 0xF0
#define SCSI$SUBQ$C_UNKNOWN 0
#define SCSI$SUBQ$C_CURRENT_POS 1
#define SCSI$SUBQ$C_MCN 2
#define SCSI$SUBQ$C_ISRC 3
#define SCSI$SUBQ$M_MC_VAL 0x80
#define SCSI$SUBQ$C_MCN_LENGTH 32
#define SCSI$SUBQ$M_TC_VAL 0x80

struct subq_channel
{
    INT8 scsi$subq$b_reserved1;
    UINT8 scsi$subq$b_audio_status;
    UINT16 scsi$subq$w_data_length;
    UINT8 scsi$subq$b_format_code;
    struct
    {
        unsigned scsi$subq$v_control : 4;
        unsigned scsi$subq$v_adr : 4;
    };
    UINT8 scsi$subq$b_track;
    UINT8 scsi$subq$b_index;
    UINT32 scsi$subq$l_abs_address;
    UINT32 scsi$subq$l_rel_address;
    struct
    {
        unsigned scsi$subq$v_reserved2 : 7;
        unsigned scsi$subq$v_mc_val : 1;
    };
    UINT8 scsi$subq$b_mcn [15];
    struct
    {
        unsigned scsi$subq$v_reserved3 : 7;
        unsigned scsi$subq$v_tc_val : 1;
    };
    UINT8 scsi$subq$b_irrc [15];
};

#define SCSIPATH$K_VMS 1
#define SCSIPATH$K_CONSOLE 2
#define SCSIPATH$K_QIOSERVER 3

struct _scsipath
{
    UINT8 scsipath$b_type;
    UINT8 scsipath$b_reserved1 [3];
    struct _ucb *scsipath$ps_port_ucb;
    INT64 scsipath$q_target_id;
    INT64 scsipath$q_lun;
};



#define DEVID_CMD$K_WR_DEVID_OPCODE 235
#define DEVID_CMD$K_RD_DEVID_OPCODE 236
#define DEVID_CMD$K_LENGTH 10

struct _devid_cmd
{
    UINT8 devid_cmd$b_opcode;
    UINT8 devid_cmd$b_reserved1 [6];
    UINT8 devid_cmd$b_alloc_length [2];
    UINT8 devid_cmd$b_control;
    INT8 devid_cmd$b_fill_6_ [2];
};



#define DEVID_HDR$K_LENGTH 4
#define DEVID$K_LENGTH 255

struct _devid
{
    UINT8 devid$b_reserved1;
    UINT8 devid$b_reserved2;
    struct
    {
        unsigned devid$v_valid : 1;
        unsigned devid$v_reserved3 : 7;
    };
    UINT8 devid$b_ident_len;
    UINT8 devid$b_ident [251];
    INT8 devid$b_fill1;
};

#define RPTLUN_CMD$K_RPTLUN_OPCODE 160
#define RPTLUN_CMD$K_LENGTH 12

struct _rptlun_cmd
{
    UINT8 rptlun_cmd$b_opcode;
    UINT8 rptlun_cmd$b_reserved1 [5];
    UINT8 rptlun_cmd$b_alloc_length [4];
    UINT8 rptlun_cmd$b_reserved2;
    UINT8 rptlun_cmd$b_control;
};

struct _lunlist_entry
{
    UINT64 lunlist_entry$q_lun [1];
};

#define RPTLUN_HDR$K_LENGTH 8

struct _rptlun_data
{
    UINT32 rptlun_data$l_list_length;
    UINT32 rptlun_data$l_reserved1;
    struct _lunlist_entry rptlun_data$r_lun_list;
};




#define PG83_IDENT$K_RESERVED 0
#define PG83_IDENT$K_BINARY 1
#define PG83_IDENT$K_ASCII 2
#define PG83_IDENT$K_VENDOR_SPEC 0
#define PG83_IDENT$K_VENDOR_ID 1
#define PG83_IDENT$K_IEEE_EUI64 2
#define PG83_IDENT$K_FCPH_NAME_ID 3
#define PG83_IDENT$K_USER_SUPPLIED 4
#define PG83_IDENT$K_DEVICE_ASSOC 0
#define PG83_IDENT$K_PORT_ASSOC 1
#define IDENT_HDR$K_LENGTH 4
#define FCPH$C_STD_ID 1
#define FCPH$C_EXT_ID 2
#define FCPH$C_REG_ID 5
#define FCPH$C_REGEXT_ID 6

struct _pg83_ident
{
    struct
    {
        unsigned pg83_ident$v_codeset : 4;
        unsigned pg83_ident$v_reserved1 : 4;
    };
    struct
    {
        unsigned pg83_ident$v_id_type : 4;
        unsigned pg83_ident$v_assoc : 2;
        unsigned pg83_ident$v_reserved2 : 2;
    };
    UINT8 pg83_ident$b_reserved3;
    UINT8 pg83_ident$b_ident_len;
    union
    {
        UINT8 pg83_ident$b_ident;
        struct
        {
            unsigned pg83_ident$v_unused : 4;
            unsigned pg83_ident$v_naa : 4;
        };
    };
    INT8 pg83_ident$b_fill_7_ [3];
};

#define PAGE83_HDR$K_LENGTH 4
#define PAGE83$K_LENGTH 255

struct _page83
{
    struct
    {
        unsigned page83$v_device_type : 5;
        unsigned page83$v_qualifier : 3;
    };
    UINT8 page83$b_page_code;
    UINT8 page83$b_reserved1;
    UINT8 page83$b_page_len;
    UINT8 page83$b_ident_list [251];
    INT8 page83$b_fill1;
};

#define PAGE00_HDR$K_LENGTH 4
#define PAGE00$K_LENGTH 80

struct _page00
{
    struct
    {
        unsigned page00$v_device_type : 5;
        unsigned page00$v_qualifier : 3;
    };
    UINT8 page00$b_page_code;
    UINT8 page00$b_reserved1;
    UINT8 page00$b_page_len;
    UINT8 page00$b_supported_page [76];
};

#define PAGE80_HDR$K_LENGTH 4
#define PAGE80$K_LENGTH 255

struct _page80
{
    struct
    {
        unsigned page80$v_device_type : 5;
        unsigned page80$v_qualifier : 3;
    };
    UINT8 page80$b_page_code;
    UINT8 page80$b_reserved1;
    UINT8 page80$b_page_len;
    UINT8 page80$b_serial_no [251];
    INT8 page80$b_fill1;
};

#define SPTB$K_LENGTH 299

struct sptbdef
{
    UINT32 sptb$l_size;
    UINT32 sptb$l_lkid;
    UINT32 sptb$l_channel;
    INT32 sptb$l_retries;
    union
    {
        INT64 sptb$q_iosb;
        struct
        {
            UINT16 sptb$w_status;
            UINT16 sptb$w_retlen;
        };
    };
    UINT32 sptb$l_scsi_id;
    UINT32 sptb$l_lun;
    void *sptb$ps_busarray;
    UINT32 sptb$l_handle;
    UINT32 sptb$l_allocls;
    char sptb$t_inquirybuf [255];
};

#define RPTDENS_CMD$K_RPTDENS_OPCODE 68
#define RPTDENS_CMD$K_LENGTH 10

struct _rptdens_cmd
{
    UINT8 rptdens_cmd$b_opcode;
    UINT8 rptdens_cmd$b_reserved1 [6];
    UINT8 rptdens_cmd$b_alloc_length [2];
    UINT8 rptdens_cmd$b_control;
    INT8 rptdens_cmd$b_fill_8_ [2];
};








#define DENS_DESC$M_RESERVED1 0x1
#define DENS_DESC$M_DEFLT 0x20
#define DENS_DESC$M_DUP 0x40
#define DENS_DESC$M_WRTOK 0x80
#define DENS_DESC$K_LENGTH 52

struct _dens_descriptor
{
    UINT8 dens_desc$b_primary_dens_code;
    UINT8 dens_desc$b_secondary_dens_code;
    struct
    {
        unsigned dens_desc$v_reserved1 : 5 ;
        unsigned dens_desc$v_deflt : 1;
        unsigned dens_desc$v_dup : 1;
        unsigned dens_desc$v_wrtok : 1;
    };
    UINT8 dens_desc$b_reserved2;
    UINT8 dens_desc$b_reserved3;
    UINT8 dens_desc$b_bits_per_mm [3];
    UINT8 dens_desc$b_media_width [2];
    UINT8 dens_desc$b_tracks [2];
    UINT8 dens_desc$b_capacity [4];
    UINT8 dens_desc$b_assigning_org [8];
    UINT8 dens_desc$b_density_name [8];
    UINT8 dens_desc$b_description [20];
};

#define RPTDENS_HDR$K_LENGTH 4

struct _rptdens_data
{
    UINT8 rptdens$b_list_length_msb;
    UINT8 rptdens$b_list_length_lsb;
    UINT8 rptdens$b_reserved1 [2];
    struct _dens_descriptor rptdens$r_first_desc;
};



#define PAGE_0F$K_PAGE_CODE 15
#define PAGE_0F$K_PAGE_LENGTH 14
#define PAGE_0F$M_DCC 0x40
#define PAGE_0F$M_DCE 0x80
#define PAGE_0F$M_RED 0x60
#define PAGE_0F$M_DDE 0x80
#define PAGE_0F$K_NOCOMP 0
#define PAGE_0F$K_DEFAULT 1
#define PAGE_0F$K_IBM_ALDC_512 3
#define PAGE_0F$K_IBM_ALDC_1024 4
#define PAGE_0F$K_IBM_ALDC_2048 5
#define PAGE_0F$K_IBM_IDRC 16
#define PAGE_0F$K_DCLZ 32

struct _datcomp_pg
{
    struct
    {
        unsigned page_0f$v_pagecode : 6;
        unsigned page_0f$v_rsvd1 : 1;
        unsigned page_0f$v_ps : 1;
    };
    UINT8 page_0f$b_page_len;
    struct
    {
        unsigned page_0f$v_rsvd2 : 6;
        unsigned page_0f$v_dcc : 1;
        unsigned page_0f$v_dce : 1;
    };
    struct
    {
        unsigned page_0f$v_rsvd3 : 5;
        unsigned page_0f$v_red : 2;
        unsigned page_0f$v_dde : 1;
    };
    union
    {
        UINT32 page_0f$l_comp_algorithm;
        UINT8 page_0f$b_comp_bytes [4];
    };
    union
    {
        UINT32 page_0f$l_decomp_algorithm;
        UINT8 page_0f$b_decomp_bytes [4];
    };
    UINT8 page_0f$b_rsvd4 [4];
};

#include <wwiddef.h>

struct _dev_wwid_duple
{
    struct _dev_wwid_duple *dev_wwid_duple$ps_flink;
    struct _dev_wwid_duple *dev_wwid_duple$ps_blink;
    UINT16 dev_wwid_duple$w_size;
    UINT8 dev_wwid_duple$b_type;
    UINT8 dev_wwid_duple$b_subtype;
    UINT32 dev_wwid_duple$l_flags;
    char dev_wwid_duple$t_device [16];
    struct _wwid dev_wwid_duple$r_wwid;
};

#define DEV_WWID_DUPLE$K_LENGTH 312
#define DEV_WWID_DUPLE$C_LENGTH 312




#define PROUT_CMD$K_PROUT_OPCODE 95
#define PROUT_CMD$M_SERVICE_ACTION 0x1F00
#define PROUT_CMD$C_REGISTER 0
#define PROUT_CMD$C_RESERVE 1
#define PROUT_CMD$C_RELEASE 2
#define PROUT_CMD$C_CLEAR 3
#define PROUT_CMD$C_PREEMPT 4
#define PROUT_CMD$C_PREEMPT_ABORT 5
#define PROUT_CMD$C_REGISTER_IGNORE 6
#define PROUT_CMD$M_RESERVED1 0xE000
#define PROUT_CMD$M_TYPE 0xF0000
#define PROUT_CMD$C_OBSELETE1 0
#define PROUT_CMD$C_WRITE_EX 1
#define PROUT_CMD$C_OBSELETE2 2
#define PROUT_CMD$C_EXCLUSIVE 3
#define PROUT_CMD$C_OBSELETE3 4
#define PROUT_CMD$C_WRITE_EX_RO 5
#define PROUT_CMD$C_EXCLUSIVE_RO 6
#define PROUT_CMD$M_SCOPE 0xF00000
#define PROUT_CMD$C_LOGICAL_UNIT 0
#define PROUT_CMD$C_OBSELETE4 1
#define PROUT_CMD$C_ELEMENT 2
#define PROUT_CMD$K_PROUT_PLL 24
#define PROUT_CMD$K_LENGTH 10

struct _prout_cmd
{
    UINT8 prout_cmd$b_opcode;
    unsigned prout_cmd$v_service_action : 5;
    unsigned prout_cmd$v_reserved1 : 3;
    unsigned prout_cmd$v_type : 4;
    unsigned prout_cmd$v_scope : 4;
    UINT32 prout_cmd$l_reserved2;
    UINT8 prout_cmd$b_param_list_len_msb;
    UINT8 prout_cmd$b_param_list_len_lsb;
    UINT8 prout_cmd$b_control;
};



#define PROUT_PL$M_APTPL 0x0
#define PROUT_PL$M_RESERVED 0x0
#define PROUT_PL$K_LENGTH 24

struct _prout_pl
{
    UINT64 prout_pl$q_reservation_key;
    UINT64 prout_pl$q_sa_reservation_key;
    UINT32 prout_pl$l_scope_address;
    unsigned prout_pl$v_aptpl : 1;
    unsigned prout_pl$v_reserved : 15;
    UINT16 prout_pl$w_obselete;
};



#define PRIN_CMD$K_PRIN_OPCODE 94
#define PRIN_CMD$M_SERVICE_ACTION 0x1F00
#define PRIN_CMD$C_READ_KEYS 0
#define PRIN_CMD$C_READ_RESERVATION 1
#define PRIN_CMD$M_RESERVED1 0xE000
#define PRIN_CMD$K_LENGTH 10

struct _prin_cmd
{
    UINT8 prin_cmd$b_opcode;
    unsigned prin_cmd$v_service_action : 5;
    unsigned prin_cmd$v_reserved1 : 3;
    UINT32 prin_cmd$l_reserved2;
    UINT8 prin_cmd$b_reserved3;
    UINT16 prin_cmd$w_allocation_len;
    UINT8 prin_cmd$b_control;
};



#define PRIN_KEYS$K_LENGTH 16

struct _prin_keys
{
    UINT32 prin_keys$l_generation;
    UINT32 prin_keys$l_additional_len;
    UINT64 prin_keys$q_key;
};



#define PRIN_RES$M_TYPE 0x0
#define PRIN_RES$C_OBSELETE1 0
#define PRIN_RES$C_WRITE_EX 1
#define PRIN_RES$C_OBSELETE2 2
#define PRIN_RES$C_EXCLUSIVE 3
#define PRIN_RES$C_OBSELETE3 4
#define PRIN_RES$C_WRITE_EX_RO 5
#define PRIN_RES$C_EXCLUSIVE_RO 6
#define PRIN_RES$M_SCOPE 0x0
#define PRIN_RES$C_LOGICAL_UNIT 0
#define PRIN_RES$C_OBSELETE4 1
#define PRIN_RES$C_ELEMENT 2
#define PRIN_RES$K_LENGTH 24

struct _prin_res
{
    UINT32 prin_res$l_generation;
    UINT32 prin_res$l_additional_len;
    UINT64 prin_res$q_key;
    UINT32 prin_res$l_scope_address;
    UINT8 prin_res$b_reserved1;
    unsigned prin_res$v_type : 4;
    unsigned prin_res$v_scope : 4;
    UINT16 prin_res$w_obselete;
};

#endif

