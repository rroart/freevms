#ifndef BUSARRAYDEF_H
#define BUSARRAYDEF_H

#include <vms_types.h>

#define BUSARRAY$M_NO_RECONNECT 0x1

struct _busarrayentry
{
    UINT64 busarray$q_hw_id;
    UINT64 busarray$q_csr;
    UINT32 busarray$l_node_number;
    union
    {
        UINT32 busarray$l_flags;
        struct
        {
            unsigned busarray$v_no_reconnect : 1;
            unsigned busarray$v_fill_0_ : 7;
        };
    };
    struct _crb *busarray$ps_crb;
    struct _adp *busarray$ps_adp;
    union
    {
        UINT32 busarray$l_autoconfig;
        void *busarray$ps_autoconfig;
    };
    union
    {
        UINT32 busarray$l_ctrlltr;
        UINT8 busarray$b_ctrlltr;
    };

    union
    {
        UINT64 busarray$q_bus_specific;
        struct
        {
            union
            {
                UINT32 busarray$l_bus_specific_l;
                void *busarray$ps_bus_specific_l;
                UINT32 busarray$l_int_vec;
                UINT32 busarray$l_sys_irq;
            };
            union
            {
                UINT32 busarray$l_bus_specific_h;
                void *busarray$ps_bus_specific_h;
                UINT32 busarray$l_bus_specific_1;
                void *busarray$ps_bus_specific_1;
            };
        };
    };
    UINT32 busarray$l_cpu_affinity;
    union
    {
        UINT32 busarray$l_bus_specific_2;
        void *busarray$ps_bus_specific_2;
    };
};

#define BUSARRAYENTRY$K_LENGTH 56

#define BUSARRAYHEADER$K_LENGTH 24

#define BUSARRAY$S_BUSARRAYHEADER 24

struct _busarray_header
{
    struct _adp *busarray$ps_parent_adp;
    UINT32 busarray$l_fill1;
    UINT16 busarray$w_size;
    UINT8 busarray$b_type;
    UINT8 busarray$b_subtype;
    UINT32 busarray$l_bus_type;
    UINT32 busarray$l_bus_node_cnt;
    UINT32 busarray$l_fill2;
    union
    {
        INT64 busarray$q_entry_list [1];
        struct _busarrayentry busarray$r_bus_array_entry;
    };
};

#endif

