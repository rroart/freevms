#ifndef iocdef_h
#define iocdef_h

#define IOC$M_PHY 0x1
#define IOC$M_TYPE 0x2
#define IOC$M_CLASS 0x4
#define IOC$M_LOCAL 0x8
#define IOC$M_EXISTS 0x10
#define IOC$M_2P 0x20
#define IOC$M_ANY 0x40
#define IOC$M_MOUNT 0x80
#define IOC$M_ALT 0x100
#define IOC$M_NO_TRANS 0x200
#define IOC$M_ALLOC 0x400
#define IOC$M_DTN 0x800
#define IOC$M_NOLOCK 0x1000
#define IOC$M_PAC 0x2000
#define IOC$M_B4CREATE 0x4000
#define IOC$S_IOCDEF 2

#define IOC$K_BYTE_LANED 1
#define IOC$K_WORD_LANED 2
#define IOC$K_LONGWORD 4
#define IOC$K_QUADWORD 8
#define IOC$K_BYTE 256
#define IOC$K_WORD 512
#define IOC$K_ENABLE_INTR 1
#define IOC$K_DISABLE_INTR 2
#define IOC$K_ENABLE_SG 3
#define IOC$K_DISABLE_SG 4
#define IOC$K_ENABLE_PAR 5
#define IOC$K_DISABLE_PAR 6
#define IOC$K_ENABLE_BLKM 7
#define IOC$K_DISABLE_BLKM 8
#define IOC$K_ISSUE_EOI 9
#define IOC$K_ENABLE_DISTRIB_INTR 10
#define IOC$K_DISABLE_DISTRIB_INTR 11
#define IOC$K_TURBO_SLOT_DENSE_PA 1
#define IOC$K_TURBO_SLOT_SPARSE_PA 2
#define IOC$K_FBUS_INT_LOC 3
#define IOC$K_EISA_IRQ 4
#define IOC$K_EISA_DMA_CHAN 5
#define IOC$K_EISA_CONFIG_BLOCK 6
#define IOC$K_EISA_MEM_CONFIG 7
#define IOC$K_LBUS_DEV_BLK_PTR 8
#define IOC$K_EISA_IO_PORT 9
#define IOC$K_SCSI_CLK_PERIOD 10
#define IOC$K_CPU_INT_MASK 11

#define IOC$K_IO_CSR_BYTE_ACCESS 11
#define IOC$K_BUS_IO_BYTE_GRAN 11
#define IOC$K_IO_CSR_LONG_ACCESS 12
#define IOC$K_IO_MEM_BYTE_ACCESS 13
#define IOC$K_BUS_MEM_BYTE_GRAN 13
#define IOC$K_IO_MEM_LONG_ACCESS 14
#define IOC$K_BUS_MEM_DENSE 14
#define IOC$K_IO_ADDRESS_SWIZZLE 15
#define IOC$K_DIRECT_DMA_BASE 16
#define IOC$K_DDMA_BASE_BA 16

#define IOC$K_DIRECT_DMA_SIZE 17

#define IOC$K_ISA_USER_PARAM 18
#define IOC$K_DEVICE_IPL 19
#define IOC$K_MONSTER_WINDOW 20

#define IOC$K_DDMA_WIN_SIZE 21
#define IOC$K_DDMA_BASE_PA 22

struct _ioc
{
    unsigned ioc$v_phy : 1;
    unsigned ioc$v_type : 1;
    unsigned ioc$v_class : 1;
    unsigned ioc$v_local : 1;
    unsigned ioc$v_exists : 1;
    unsigned ioc$v_2p : 1;
    unsigned ioc$v_any : 1;
    unsigned ioc$v_mount : 1;
    unsigned ioc$v_alt : 1;
    unsigned ioc$v_no_trans : 1;
    unsigned ioc$v_alloc : 1;
    unsigned ioc$v_dtn : 1;
    unsigned ioc$v_nolock : 1;
    unsigned ioc$v_pac : 1;
    unsigned ioc$v_b4create : 1;
    unsigned ioc$v_fill_0_ : 1;
};

#endif

