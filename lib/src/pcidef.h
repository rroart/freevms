#ifndef PCIDEF_H
#define PCIDEF_H

#include <vms_types.h>

#define PCI$K_VENDOR_ID 0
#define PCI$K_DEVICE_ID 2
#define PCI$K_COMMAND 4
#define PCI$M_IO_ENABLE 0x1
#define PCI$M_MEM_ENABLE 0x2
#define PCI$M_BUS_MASTER_ENABLE 0x4
#define PCI$M_SPECIAL_CYCLE_ENABLE 0x8
#define PCI$M_INVAL_ENABLE 0x10
#define PCI$M_PALETTE_SNOOP_ENABLE 0x20
#define PCI$M_PARITY_ENABLE 0x40
#define PCI$M_WAIT_CYCLE_ENABLE 0x80
#define PCI$M_SERR_ENABLE 0x100
#define PCI$M_BACK_TO_BACK_ENABLE 0x200
#define PCI$K_STATUS 6
#define PCI$M_FAST_BB_CAPABLE 0x80
#define PCI$M_DATA_PARITY_DETECT 0x100
#define PCI$M_DEVSEL_TIMING 0x600
#define PCI$M_SIGNAL_TARGET_ABORT 0x800
#define PCI$M_RCV_TARGET_ABORT 0x1000
#define PCI$M_RCV_MASTER_ABORT 0x2000
#define PCI$M_SIGNAL_SERR 0x4000
#define PCI$M_DETECT_PE 0x8000
#define PCI$K_REVISION_ID 8
#define PCI$K_PROGRAMMING_IF 9
#define PCI$K_SUB_CLASS 10
#define PCI$K_BASE_CLASS 11
#define PCI$K_CACHE_LINE_SIZE 12
#define PCI$K_LATENCY_TIMER 13
#define PCI$K_HEADER_TYPE 14
#define PCI$K_BIST 15
#define PCI$K_BASE_ADDRESS_0 16
#define PCI$K_BASE_ADDRESS_1 20
#define PCI$K_BASE_ADDRESS_2 24
#define PCI$K_BASE_ADDRESS_3 28
#define PCI$K_BASE_ADDRESS_4 32
#define PCI$K_BASE_ADDRESS_5 36
#define PCI$K_CARDBUS_CIS 40
#define PCI$K_SUB_VNDR 44
#define PCI$K_SUB_ID 46
#define PCI$K_EXP_ROM_BASE 48
#define PCI$K_INTR_LINE 60
#define PCI$K_INTR_PIN 61
#define PCI$K_MIN_GNT 62
#define PCI$K_MAX_LAT 63
#define PCI$S_PCIDEF 64
#define PCI$K_DEVSEL_FAST 0
#define PCI$K_DEVSEL_MEDIUM 1
#define PCI$K_DEVSEL_SLOW 2
#define PCI$K_NOT_IMPLEMENTED 0
#define PCI$K_MASS_STORAGE_CTRLR 1
#define PCI$K_NETWORK_CTRLR 2
#define PCI$K_DISPLAY_CTRLR 3
#define PCI$K_MULTIMEDIA_DEVICE 4
#define PCI$K_MEMORY_CTRLR 5
#define PCI$K_BRIDGE_DEVICE 6
#define PCI$K_UNDEFINED 255
#define PCI$K_SCSI_CTRLR 0
#define PCI$K_IDE_CTRLR 1
#define PCI$K_FLOPPY_CTRLR 2
#define PCI$K_IPI_CTRLR 3
#define PCI$K_OTHER_MASS_STORAGE 80
#define PCI$K_NI_CTRLR 0
#define PCI$K_TOKEN_CTRLR 1
#define PCI$K_FDDI_CTRLR 2
#define PCI$K_OTHER_NETWORK 80
#define PCI$K_VGA_CTRLR 0
#define PCI$K_XGA_CTRLR 1
#define PCI$K_OTHER_DISPLAY 80
#define PCI$K_VIDEO_CTRLR 0
#define PCI$K_AUDIO_CTRLR 1
#define PCI$K_OTHER_MULTIMEDIA 80
#define PCI$K_RAM 0
#define PCI$K_FLASH 1
#define PCI$K_OTHER_MEMORY 80
#define PCI$K_HOST_BRIDGE 0
#define PCI$K_ISA_BRIDGE 1
#define PCI$K_EISA_BRIDGE 2
#define PCI$K_MC_BRIDGE 3
#define PCI$K_PCI_PCI_BRIDGE 4
#define PCI$K_PCMCIA_BRIDGE 5
#define PCI$K_OTHER_BRIDGE 80
#define PCI$K_INTR_PIN_NOT_USED 0
#define PCI$K_INTR_PIN_INTA 1
#define PCI$K_INTR_PIN_INTB 2
#define PCI$K_INTR_PIN_INTC 3
#define PCI$K_INTR_PIN_INTD 4
#define PCI$K_MAX_DEVICES 32
#define PCI$K_LENGTH 64

#define PCI$M_BASE_ADDRESS_MEM_IO 0x1
#define PCI$M_BASE_ADDRESS_TYPE 0x6
#define PCI$M_BASE_ADDRESS_PREFETCHABLE 0x8
#define PCI$M_BASE_ADDRESS_BITS_31_4 0xFFFFFFF0

#define BASE_ADDR_32 0
#define BASE_ADDR_BELOW_1MB 1
#define BASE_ADDR_64 2
#define BASE_ADDR_RESERVED 3
#define PCI$M_PCI_NODE_NUMBER_FUNCTION 0x7
#define PCI$M_PCI_NODE_NUMBER_DEVICE 0xF8
#define PCI$M_PCI_NODE_NUMBER_BUS 0xFF00
#define PCI$M_PCI_NODE_NUMBER_OFFSET 0xFFFF0000

#define PCIERR$K_LENGTH 72

#define PCIFLAGS$M_FILL1 0xFFFFFF00
#define PCIFLAGS$M_DATA_PARITY_DETECT 0x1
#define PCIFLAGS$M_SIGNAL_TARGET_ABORT 0x2
#define PCIFLAGS$M_RCV_TARGET_ABORT 0x4
#define PCIFLAGS$M_RCV_MASTER_ABORT 0x8
#define PCIFLAGS$M_SIGNAL_SERR 0x10
#define PCIFLAGS$M_DETECT_PE 0x20
#define PCIFLAGS$M_FILL2 0xC0
#define PCIFLAGS$M_FILL3 0xFFFFFF00

#define ERRTAG$K_PCIERR 16

struct _pci
{
    INT16 pci$w_vendor_id;
    INT16 pci$w_device_id;
    union
    {
        INT16 pci$w_command;
        struct
        {
            unsigned pci$v_io_enable : 1;
            unsigned pci$v_mem_enable : 1;
            unsigned pci$v_bus_master_enable : 1;
            unsigned pci$v_special_cycle_enable : 1;
            unsigned pci$v_inval_enable : 1;
            unsigned pci$v_palette_snoop_enable : 1;
            unsigned pci$v_parity_enable : 1;
            unsigned pci$v_wait_cycle_enable : 1;
            unsigned pci$v_serr_enable : 1;
            unsigned pci$v_back_to_back_enable : 1;
            unsigned pci$v_command_fill : 6;
        };
    };
    union
    {
        INT16 pci$w_status;
        struct
        {
            unsigned pci$v_status_fill : 7;
            unsigned pci$v_fast_bb_capable : 1;
            unsigned pci$v_data_parity_detect : 1;
            unsigned pci$v_devsel_timing : 2;
            unsigned pci$v_signal_target_abort : 1;
            unsigned pci$v_rcv_target_abort : 1;
            unsigned pci$v_rcv_master_abort : 1;
            unsigned pci$v_signal_serr : 1;
            unsigned pci$v_detect_pe : 1;
        };
    };
    INT8 pci$b_revision_id;
    INT8 pci$b_programming_if;
    INT8 pci$b_sub_class;
    INT8 pci$b_base_class;
    INT8 pci$b_cache_line_size;
    INT8 pci$b_latency_timer;
    INT8 pci$b_header_type;
    INT8 pci$b_bist;
    INT32 pci$l_base_address_0;
    INT32 pci$l_base_address_1;
    INT32 pci$l_base_address_2;
    INT32 pci$l_base_address_3;
    INT32 pci$l_base_address_4;
    INT32 pci$l_base_address_5;
    INT32 pci$l_cardbus_cis;
    INT16 pci$w_sub_vndr;
    INT16 pci$w_sub_id;
    INT32 pci$l_exp_rom_base;
    INT32 pci$l_reserved_3;
    INT32 pci$l_reserved_4;
    INT8 pci$b_intr_line;
    INT8 pci$b_intr_pin;
    INT8 pci$b_min_gnt;
    INT8 pci$b_max_lat;
};

struct _base_address
{
    union
    {
        INT32 pci$l_base_address;
        struct
        {
            unsigned pci$v_base_address_mem_io : 1;
            unsigned pci$v_base_address_type : 2;
            unsigned pci$v_base_address_prefetchable : 1;
            unsigned pci$v_base_address_bits_31_4 : 28;
        };
    };
};

struct _pci_node_number
{
    union
    {
        INT32 pci$l_pci_node_number;
        struct
        {
            unsigned pci$v_pci_node_number_function : 3;
            unsigned pci$v_pci_node_number_device : 5;
            unsigned pci$v_pci_node_number_bus : 8;
            unsigned pci$v_pci_node_number_offset : 16;
        };
    };
};

struct _pci_busarray_hardware_id
{
    union
    {
        INT64 pci$q_pci_hardware_id;
        struct
        {
            INT16 pci$w_pci_hardware_id_vendor;
            INT16 pci$w_pci_hardware_id_device;
            union
            {
                INT32 pci$l_pci_hardware_id_fill;
                struct
                {
                    INT16 pci$w_pci_hardware_id_sub_vndr;
                    INT16 pci$w_pci_hardware_id_sub_id;
                };
            };
        };
    };
};

struct _pcierr
{
    unsigned pcierr$v_fill1 : 11;
    unsigned pcierr$v_device_number : 5;
    UINT8 pcierr$b_bus_number;
    UINT8 pcierr$b_fill2;
    UINT32 pcierr$l_frame_size;
    struct _pci pcierr$r_pci;
};

struct _pciflags
{
    union
    {
        struct
        {
            UINT8 pciflags$b_pciflags;
            unsigned pciflags$v_fill1 : 24;
        };
        struct
        {
            unsigned pciflags$v_data_parity_detect : 1;
            unsigned pciflags$v_signal_target_abort : 1;
            unsigned pciflags$v_rcv_target_abort : 1;
            unsigned pciflags$v_rcv_master_abort : 1;
            unsigned pciflags$v_signal_serr : 1;
            unsigned pciflags$v_detect_pe : 1;
            unsigned pciflags$v_fill2 : 2;
            unsigned pciflags$v_fill3 : 24;
        };
    };
};

#endif

