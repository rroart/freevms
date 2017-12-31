#ifndef adpdef_h
#define adpdef_h

#define ADP$M_INDIRECT_VECTOR 0x1
#define ADP$M_ONLINE 0x2
#define ADP$M_BOOT_ADP 0x4
#define ADP$M_PCI_PCI_BRIDGE 0x8
#define ADP$M_EISA_PARENT 0x10
#define ADP$M_DEEP 0x20
#define ADP$M_PCI_MULTI 0x40
#define ADP$M_SHUTDOWN 0x1
#define ADP$M_PORTONLY 0x2
#define ADP$M_STRUCT_ALLOCATED 0x4

#define ADP$K_CIADPLEN 288
#define ADP$C_CIADPLEN 288
#define ADP$K_NIADPLEN 288
#define ADP$C_NIADPLEN 288
#define ADP$K_GBIADPLEN 288
#define ADP$C_GBIADPLEN 288
#define ADP$K_MINADPLEN 288
#define ADP$C_MINADPLEN 288
#define ADP$S_ADPDEF 288

struct _adp
{
    long long adp$q_csr;
    unsigned short int adp$w_size;
    unsigned char adp$b_type;
    unsigned char adp$b_number;
    struct _adp *adp$l_link;
    unsigned int adp$l_tr;
    unsigned int adp$l_adptype;
    int (*adp$ps_node_data)(void);
    void *adp$l_vector;
    struct _crb *adp$l_crb;
    void *adp$ps_mbpr;
    unsigned long long adp$q_queue_time;
    unsigned long long adp$q_wait_time;
    struct _adp *adp$ps_parent_adp;
    struct _adp *adp$ps_peer_adp;
    struct _adp *adp$ps_child_adp;
    unsigned int adp$l_probe_cmd;
    struct _busarray_header *adp$ps_bus_array;
    void *adp$ps_command_tbl;
    struct _spl *adp$ps_spinlock;
    union
    {
        unsigned short int adp$w_node_num;
        unsigned short int adp$w_prim_node_num;
    };
    unsigned short int adp$w_sec_node_num;
    unsigned short int adp$w_fill2 [1];
    unsigned char adp$b_hose_num;
    unsigned char adp$b_fill3 [1];
    union
    {
        void *adp$ps_adp_specific2;
        unsigned int adp$l_adp_specific2;
        unsigned int adp$l_hw_location;
        unsigned int adp$l_a32_free_items;
    };
    union
    {
        void *adp$ps_adp_specific3;
        unsigned int adp$l_adp_specific3;
        unsigned int adp$l_a64_item_num;
    };
    struct _crab *adp$l_crab;
    union
    {
        unsigned int adp$l_adapter_flags;
        struct
        {
            unsigned adp$v_indirect_vector : 1;
            unsigned adp$v_online : 1;
            unsigned adp$v_boot_adp : 1;
            unsigned adp$v_pci_pci_bridge : 1;
            unsigned adp$v_eisa_parent : 1;
            unsigned adp$v_deep : 1;
            unsigned adp$v_pci_multi : 1;
            unsigned adp$v_fill_0_ : 1;
        };
    };
    unsigned int adp$l_reserved1;
    union
    {
        long long adp$q_hw_id_mask;
        struct
        {
            int adp$l_hw_id_mask_lo;
            int adp$l_hw_id_mask_hi;
        };
    };
    unsigned int adp$l_cpu_affinity;
    int (*adp$ps_node_function)(void);
    union
    {
        unsigned int adp$l_vportsts;
        struct
        {
            unsigned adp$v_shutdown : 1;
            unsigned adp$v_portonly : 1;
            unsigned adp$v_struct_allocated : 1;
            unsigned adp$v_fill_1_ : 5;
        };
    };
    void *adp$l_avector;
    unsigned long long adp$q_scratch_buf_pa;
    void *adp$ps_scratch_buf_va;
    unsigned int adp$l_scratch_buf_len;
    void *adp$l_lsdump;
    int (*adp$ps_probe_csr)(void);
    int (*adp$ps_probe_csr_cleanup)(void);
    int (*adp$ps_load_map_reg)(void);
    int (*adp$ps_shutdown)(void);
    void *adp$ps_config_table;
    void *adp$ps_map_reg_base;
    union
    {
        void *adp$ps_adp_specific;
        void *adp$ps_adp_specific1;
        unsigned int adp$l_adp_specific1;
        unsigned int adp$l_a32_item_num;
        void *adp$ps_ablk;
    };
    int (*adp$ps_disable_interrupts)(void);
    int (*adp$ps_startup)(void);
    int (*adp$ps_init)(void);
    union
    {
        void *adp$ps_adp_specific4;
        unsigned int adp$l_adp_specific4;
        unsigned int adp$l_a64_free_items;
        unsigned int adp$l_total_sg_entries;
    };
    long long adp$q_hardware_type;
    long long adp$q_hardware_rev;
    int (*adp$ps_cram_cmd)(void);
    int (*adp$ps_read_pci_config)(void);
    int (*adp$ps_write_pci_config)(void);
    int (*adp$ps_map_io)(void);
    int (*adp$ps_read_io)(void);
    int (*adp$ps_write_io)(void);
    struct _iohandle *adp$ps_iohandle_flink;
    struct _iohandle *adp$ps_iohandle_blink;
    unsigned int adp$l_intd [4];
    unsigned int adp$l_bus_num;
    unsigned int adp$l_pci_node_num;
    unsigned int adp$l_sintd [4];
};

#endif

