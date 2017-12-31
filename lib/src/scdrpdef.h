#ifndef scdrpdef_h
#define scdrpdef_h

#define SCDRP$C_VERSION 6
#define SCDRP$K_SCDRPBASE 0
#define SCDRP$C_SCDRPBASE 0
#define SCDRP$M_FLAG_S0BUF 0x1
#define SCDRP$M_FLAG_BUFFER_MAPPED 0x2
#define SCDRP$M_FLAG_DISK_SPUN_UP 0x4
#define SCDRP$M_FLAG_LOCK 0x8
#define SCDRP$M_FLAG_QUEUED_IO 0x10
#define SCDRP$M_FLAG_ACA_IO 0x20
#define SCDRP$M_FLAG_CLEAR_ACA_MSG 0x40
#define SCDRP$M_FLAG_ASENSE_VALID 0x80
#define SCDRP$M_FLAG_ON_PORT_QUEUE 0x100
#define SCDRP$M_FLAG_ON_DEV_QUEUE 0x200
#define SCDRP$M_FLAG_ABORT_THIS_IO 0x400
#define SCDRP$M_FLAG_QUEUE_FULL_INIT 0x800
#define SCDRP$M_FLAG_QUEUE_FULL_SEEN 0x1000
#define SCDRP$M_FLAG_WAIT_FOR_IO 0x2000
#define SCDRP$M_FLAG_INTERNAL_REQUEST 0x4000
#define SCDRP$M_FLAG_SEND_MESSAGE_ONLY 0x8000
#define SCDRP$M_FLAG_SEND_DEVICE_RESET 0x10000
#define SCDRP$M_FLAG_MODE_SENSE 0x60000
#define SCDRP$M_FLAG_CL_PRIVATE_BUFF 0x80000
#define SCDRP$M_FLAG_TENBYTE 0x100000
#define SCDRP$M_FLAG_BUS_RESET 0x200000
#define SCDRP$M_FLAG_ON_SYS_QUEUE 0x400000
#define SCDRP$M_FLAG_ON_FP_QUEUE 0x800000
#define SCDRP$M_FLAG_RBUN_WANTED 0x1000000
#define SCDRP$M_DSF_NOWAIT 0x1
#define SCDRP$M_DSF_RELEASE_SPDT_LOCK 0x2
#define SCDRP$M_DSF_DEVICE_WAS_RESET 0x4
#define SCDRP$M_DSF_REQUEST_ABORTED 0x8
#define SCDRP$M_DSF_REQUEST_FLUSHED 0x10
#define SCDRP$M_DSF_STALL_WFIKPCH_DIPL 0x20
#define SCDRP$M_MSGO_IDENTIFY 0x1
#define SCDRP$M_MSGO_QUEUE_TAG 0x2
#define SCDRP$M_MSGO_SYNC_OUT 0x4
#define SCDRP$M_MSGO_BUS_DEVICE_RESET 0x8
#define SCDRP$M_MSGO_MSG_PARITY_ERROR 0x10
#define SCDRP$M_MSGO_ID_ERROR 0x20
#define SCDRP$M_MSGO_ABORT 0x40
#define SCDRP$M_MSGO_NOP 0x80
#define SCDRP$M_MSGO_MESSAGE_REJECT 0x100
#define SCDRP$M_MSGO_CLEAR_ACA 0x200
#define SCDRP$M_MSGO_LAST_BIT 0x400
#define SCDRP$M_MSGI_SYNC_IN 0x1
#define SCDRP$K_QCHAR_UNORDERED 0
#define SCDRP$K_QCHAR_ORDERED 1
#define SCDRP$K_QCHAR_HEAD 2
#define SCDRP$K_QCHAR_NOT_QUEUED 3
#define SCDRP$K_QCHAR_ACA 4
#define SCDRP$M_EVENT_PARERR 0x1
#define SCDRP$M_EVENT_BSYERR 0x2
#define SCDRP$M_EVENT_MISPHS 0x4
#define SCDRP$M_EVENT_BADPHS 0x8
#define SCDRP$M_EVENT_RST 0x10
#define SCDRP$M_EVENT_CTLERR 0x20
#define SCDRP$M_EVENT_BUSERR 0x40
#define SCDRP$M_EVENT_ABORT 0x80
#define SCDRP$M_EVENT_MSGERR 0x100
#define SCDRP$M_CNX_ABORT_PND 0x1
#define SCDRP$M_CNX_ABORT_CMPL 0x2
#define SCDRP$M_CNX_ABORT_INPROG 0x4
#define SCDRP$M_CNX_ABORT_RESEL 0x8
#define SCDRP$M_CNX_PND_RESEL 0x10
#define SCDRP$M_CNX_DSCN 0x20
#define SCDRP$M_CNX_TMODSCN 0x40
#define SCDRP$M_PHASE_DATAOUT 0x1
#define SCDRP$M_PHASE_DATAIN 0x2
#define SCDRP$M_PHASE_CMD 0x4
#define SCDRP$M_PHASE_STS 0x8
#define SCDRP$M_PHASE_INV1 0x10
#define SCDRP$M_PHASE_INV2 0x20
#define SCDRP$M_PHASE_MSGOUT 0x40
#define SCDRP$M_PHASE_MSGIN 0x80
#define SCDRP$M_PHASE_ARB 0x100
#define SCDRP$M_PHASE_SEL 0x200
#define SCDRP$M_PHASE_RESEL 0x400
#define SCDRP$M_PHASE_DISCON 0x800
#define SCDRP$M_PHASE_CMD_CMPL 0x1000
#define SCDRP$M_PHASE_TMODISCON 0x2000
#define SCDRP$M_PHASE_FREE 0x4000
#define SCDRP$K_SCSI_LEN 440
#define SCDRP$C_SCSI_LEN 440
#define SCDRP$K_SCSI_V73 1
#define SCDRP$C_SCSI_V73 1
#define SCDRP$K_LENGTH 504
#define SCDRP$C_LENGTH 504

struct _scdrp
{
    struct _fkb *scdrp$l_fqfl;
    struct _fkb *scdrp$l_fqbl;
    unsigned short int scdrp$w_scdrpsize;
    unsigned char scdrp$b_cd_type;
    unsigned char scdrp$b_flck;
    void (*scdrp$l_fpc)();
    unsigned long scdrp$l_fr3;
    unsigned long scdrp$l_fr4;
    struct _ucb *scdrp$l_port_ucb;
    struct _ucb *scdrp$l_ucb;
    unsigned int scdrp$l_func;
    unsigned int scdrp$l_boff;
    unsigned int scdrp$is_sts;
    struct _pte *scdrp$l_svapte;
    unsigned int scdrp$l_bcnt;
    void *scdrp$l_media;
    unsigned int scdrp$l_abcnt;
    int (*scdrp$l_savd_rtn)();
    void *scdrp$l_msg_buf;
    unsigned int scdrp$l_rspid;
    struct _cdt *scdrp$l_cdt;
    short int *scdrp$l_rwcptr;
    union
    {
        struct
        {
            struct _irp *scdrp$l_irp;
            struct _kpb *scdrp$ps_kpb;
            struct _scdrp *scdrp$ps_prev_scdrp;
            struct _spdt *scdrp$ps_spdt;
            void *scdrp$l_sva_user;
            void *scdrp$l_cmd_buf;
            unsigned int scdrp$l_cmd_buf_len;
            char *scdrp$l_cmd_ptr;
            int *scdrp$l_sts_ptr;
            union
            {
                unsigned int scdrp$l_scsi_flags;
                struct
                {
                    unsigned scdrp$v_flag_s0buf : 1;
                    unsigned scdrp$v_flag_buffer_mapped : 1;
                    unsigned scdrp$v_flag_disk_spun_up : 1;
                    unsigned scdrp$v_flag_lock : 1;
                    unsigned scdrp$v_flag_queued_io : 1;
                    unsigned scdrp$v_flag_aca_io : 1;
                    unsigned scdrp$v_flag_clear_aca_msg : 1;
                    unsigned scdrp$v_flag_asense_valid : 1;
                    unsigned scdrp$v_flag_on_port_queue : 1;
                    unsigned scdrp$v_flag_on_dev_queue : 1;
                    unsigned scdrp$v_flag_abort_this_io : 1;
                    unsigned scdrp$v_flag_queue_full_init : 1;
                    unsigned scdrp$v_flag_queue_full_seen : 1;
                    unsigned scdrp$v_flag_wait_for_io : 1;
                    unsigned scdrp$v_flag_internal_request : 1;
                    unsigned scdrp$v_flag_send_message_only : 1;
                    unsigned scdrp$v_flag_send_device_reset : 1;
                    unsigned scdrp$v_flag_mode_sense : 2;
                    unsigned scdrp$v_flag_cl_private_buff : 1;
                    unsigned scdrp$v_flag_tenbyte : 1;
                    unsigned scdrp$v_flag_bus_reset : 1;
                    unsigned scdrp$v_flag_on_sys_queue : 1;
                    unsigned scdrp$v_flag_on_fp_queue : 1;
                    unsigned scdrp$v_flag_rbun_wanted : 1;
                    unsigned scdrp$v_fill_14_ : 7;
                };
            };
            union
            {
                unsigned int scdrp$is_dipl_scsi_flags;
                struct
                {
                    unsigned scdrp$v_dsf_nowait : 1;
                    unsigned scdrp$v_dsf_release_spdt_lock : 1;
                    unsigned scdrp$v_dsf_device_was_reset : 1;
                    unsigned scdrp$v_dsf_request_aborted : 1;
                    unsigned scdrp$v_dsf_request_flushed : 1;
                    unsigned scdrp$v_dsf_stall_wfikpch_dipl : 1;
                    unsigned scdrp$v_fill_15_ : 2;
                };
            };
            void *scdrp$l_datacheck;
            unsigned int scdrp$l_cl_retry;
            unsigned int scdrp$l_dma_timeout;
            unsigned int scdrp$l_discon_timeout;
            unsigned int scdrp$l_addnl_info;
            unsigned char scdrp$b_sense_key;
            char scdrp$t_scdrp_align_0 [3];
            unsigned int scdrp$l_pad_bcnt;
            void *scdrp$l_sva_dma;
            int scdrp$is_cmd_slot;
            struct _pte *scdrp$l_sva_spte;
            void *scdrp$ps_port_dma_va;
            struct _pte *scdrp$l_port_svapte;
            unsigned int scdrp$l_port_boff;
            void *scdrp$ps_mode_args;
            void *scdrp$l_scsimsgo_ptr;
            void *scdrp$l_scsimsgi_ptr;
            char scdrp$b_scsimsgo_buf [8];
            char scdrp$b_scsimsgi_buf [8];
            union
            {
                unsigned int scdrp$l_msgo_pending;
                struct
                {
                    unsigned scdrp$v_msgo_identify : 1;
                    unsigned scdrp$v_msgo_queue_tag : 1;
                    unsigned scdrp$v_msgo_sync_out : 1;
                    unsigned scdrp$v_msgo_bus_device_reset : 1;
                    unsigned scdrp$v_msgo_msg_parity_error : 1;
                    unsigned scdrp$v_msgo_id_error : 1;
                    unsigned scdrp$v_msgo_abort : 1;
                    unsigned scdrp$v_msgo_nop : 1;
                    unsigned scdrp$v_msgo_message_reject : 1;
                    unsigned scdrp$v_msgo_clear_aca : 1;
                    unsigned scdrp$v_msgo_last_bit : 1;
                    unsigned scdrp$v_fill_16_ : 5;
                };
            };
            union
            {
                unsigned int scdrp$l_msgi_pending;
                struct
                {
                    unsigned scdrp$v_msgi_sync_in : 1;
                    unsigned scdrp$v_fill_17_ : 7;
                };
            };
            unsigned char scdrp$b_last_msgo;
            char scdrp$t_scdrp_align_1 [3];
            void *scdrp$l_data_ptr;
            unsigned int scdrp$l_trans_cnt;
            unsigned int scdrp$l_save_data_cnt;
            unsigned int scdrp$l_save_data_ptr;
            unsigned int scdrp$l_sdp_data_cnt;
            void *scdrp$l_sdp_data_ptr;
            unsigned int scdrp$l_duetime;
            unsigned int scdrp$is_cmd_bcnt;
            unsigned int scdrp$is_busy_retry_cnt;
            unsigned int scdrp$is_arb_retry_cnt;
            unsigned int scdrp$is_sel_retry_cnt;
            unsigned int scdrp$is_cmd_retry_cnt;
            unsigned int scdrp$is_sel_tqe_retry_cnt;
            unsigned int scdrp$is_dma_long;
            long long scdrp$q_time_stamp;
            union
            {
                unsigned int scdrp$is_queue_tag;
                unsigned long long scdrp$q_queue_tag;
            };
            unsigned int scdrp$is_queue_char;
            void *scdrp$ps_pqfl;
            void *scdrp$ps_pqbl;
            void *scdrp$ps_sense_buffer;
            unsigned int scdrp$is_sense_buffer_len;
            union
            {
                unsigned int scdrp$is_events_seen;
                struct
                {
                    unsigned scdrp$v_event_parerr : 1;
                    unsigned scdrp$v_event_bsyerr : 1;
                    unsigned scdrp$v_event_misphs : 1;
                    unsigned scdrp$v_event_badphs : 1;
                    unsigned scdrp$v_event_rst : 1;
                    unsigned scdrp$v_event_ctlerr : 1;
                    unsigned scdrp$v_event_buserr : 1;
                    unsigned scdrp$v_event_abort : 1;
                    unsigned scdrp$v_event_msgerr : 1;
                    unsigned scdrp$v_fill_18_ : 7;
                };
            };
            union
            {
                unsigned int scdrp$is_cnx_sts;
                struct
                {
                    unsigned scdrp$v_cnx_abort_pnd : 1;
                    unsigned scdrp$v_cnx_abort_cmpl : 1;
                    unsigned scdrp$v_cnx_abort_inprog : 1;
                    unsigned scdrp$v_cnx_abort_resel : 1;
                    unsigned scdrp$v_cnx_pnd_resel : 1;
                    unsigned scdrp$v_cnx_dscn : 1;
                    unsigned scdrp$v_cnx_tmodscn : 1;
                    unsigned scdrp$v_fill_19_ : 1;
                };
            };
            unsigned int scdrp$is_sequence;
            void *scdrp$ps_class_kpb;
            union
            {
                unsigned int scdrp$is_bus_phase;
                struct
                {
                    unsigned scdrp$v_phase_dataout : 1;
                    unsigned scdrp$v_phase_datain : 1;
                    unsigned scdrp$v_phase_cmd : 1;
                    unsigned scdrp$v_phase_sts : 1;
                    unsigned scdrp$v_phase_inv1 : 1;
                    unsigned scdrp$v_phase_inv2 : 1;
                    unsigned scdrp$v_phase_msgout : 1;
                    unsigned scdrp$v_phase_msgin : 1;
                    unsigned scdrp$v_phase_arb : 1;
                    unsigned scdrp$v_phase_sel : 1;
                    unsigned scdrp$v_phase_resel : 1;
                    unsigned scdrp$v_phase_discon : 1;
                    unsigned scdrp$v_phase_cmd_cmpl : 1;
                    unsigned scdrp$v_phase_tmodiscon : 1;
                    unsigned scdrp$v_phase_free : 1;
                    unsigned scdrp$v_fill_20_ : 1;
                };
            };
            unsigned int scdrp$is_old_phases;
            unsigned int scdrp$is_request_status;

            union
            {
                char scdrp$t_crctx_filler [96];
                struct
                {
                    char scdrp$t_crctx_ffill1 [56];
                    int scdrp$is_item_cnt;
                    int scdrp$is_item_num;
                };
            };
        };
    };
    void *scdrp$ps_qio_p6;
    unsigned long long scdrp$q_port_specific;
    int scdrp$l_rsvd_long [11];
    int scdrp$l_qfull_sts_cnt;
    union
    {
        void *scdrp$ps_scsi_rbun_p;
        struct _fcp_rbun *scdrp$ps_fcp_rbun;
    };
};

#endif

