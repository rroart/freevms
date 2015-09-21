#ifndef SCQDEF_H
#define SCQDEF_H

#define SCQ$C_VERSION 1

#define SCQ$K_CLSPOT_SP1 1
#define SCQ$K_CLSPOT_SP2 2

struct _scq
{
    UINT32 scq$is_clspot_protocol;
    union
    {
        UINT32 scq$is_version_nos;
        struct
        {
            UINT8 scq$ib_scdrp_vno;
            UINT8 scq$ib_spdt_vno;
            UINT8 scq$ib_scdt_vno;
            UINT8 scq$ib_stdt_vno;
        };
    };
    union
    {
        struct _scsipath *scq$ps_scsipath;
        union
        {
            UINT32 scq$is_scsi_ids;
            struct
            {
                UINT16 scq$iw_scsi_port_id;
                UINT16 scq$iw_scsi_bus_id;
            };
        };
    };
    union
    {
        UINT32 scq$is_scsi_lun;
        struct
        {
            UINT16 scq$w_lun_reserved;
            UINT16 scq$iw_scsi_lun;
        };
    };
    int (*scq$ps_sel_callback)(void);
    UINT32 scq$is_sel_context;
    int (*scq$ps_aen_callback)(void);
    UINT32 scq$is_aen_context;
    int (*scq$ps_port_state_callback)(void);
    UINT32 scq$is_port_state_context;

    void *scq$ps_spdt;
    void *scq$ps_scdt;
    void *scq$ps_stdt;
    UINT32 scq$is_max_bcnt;
    UINT32 scq$is_port_serv_flags;
};

#define SCQ$K_LENGTH 60
#define SCQ$S_SCQDEF 60

#endif

