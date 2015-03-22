#ifndef SCQDEF_H
#define SCQDEF_H

#define SCQ$C_VERSION 1

#define SCQ$K_CLSPOT_SP1 1
#define SCQ$K_CLSPOT_SP2 2

struct _scq
{
    unsigned int scq$is_clspot_protocol;
    union
    {
        unsigned int scq$is_version_nos;
        struct
        {
            unsigned char scq$ib_scdrp_vno;
            unsigned char scq$ib_spdt_vno;
            unsigned char scq$ib_scdt_vno;
            unsigned char scq$ib_stdt_vno;
        };
    };
    union
    {
        struct _scsipath *scq$ps_scsipath;
        union
        {
            unsigned int scq$is_scsi_ids;
            struct
            {
                unsigned short int scq$iw_scsi_port_id;
                unsigned short int scq$iw_scsi_bus_id;
            };
        };
    };
    union
    {
        unsigned int scq$is_scsi_lun;
        struct
        {
            short int scq$w_lun_reserved;
            unsigned short int scq$iw_scsi_lun;
        };
    };
    int (*scq$ps_sel_callback)(void);
    unsigned int scq$is_sel_context;
    int (*scq$ps_aen_callback)(void);
    unsigned int scq$is_aen_context;
    int (*scq$ps_port_state_callback)(void);
    unsigned int scq$is_port_state_context;

    void *scq$ps_spdt;
    void *scq$ps_scdt;
    void *scq$ps_stdt;
    unsigned int scq$is_max_bcnt;
    unsigned int scq$is_port_serv_flags;
};

#define SCQ$K_LENGTH 60
#define SCQ$S_SCQDEF 60

#endif

