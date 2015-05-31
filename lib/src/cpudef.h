#ifndef CPUDEF_H
#define CPUDEF_H

#define     CPU$C_MAX_CPUS      32
#define     CPU$C_RESERVED      0
#define     CPU$C_INIT      1
#define     CPU$C_RUN       2
#define     CPU$C_STOPPING      3
#define     CPU$C_STOPPED       4
#define     CPU$C_TIMOUT        5
#define     CPU$C_BOOT_REJECTED 6
#define     CPU$C_BOOTED        7
#define     CPU$C_NOT_CONFIGURED    8
#define     CPU$C_POWERED_DOWN  9
#define     CPU$M_INV_TBS       0x1
#define     CPU$M_INV_TBA       0x2
#define     CPU$M_BUGCHK        0x4
#define     CPU$M_BUGCHKACK     0x8
#define     CPU$M_RECALSCHD     0x10
#define     CPU$M_UPDASTSR      0x20
#define     CPU$M_UPDATE_HWCLOCK    0x40
#define     CPU$M_WORK_FQP      0x80
#define     CPU$M_QLOST     0x100
#define     CPU$M_RESCHED       0x200
#define     CPU$M_VIRTCONS      0x400
#define     CPU$M_IOPOST        0x800
#define     CPU$M_INV_ISTREAM   0x1000
#define     CPU$M_INV_TBSD      0x2000
#define     CPU$M_INV_TBS_MMG   0x4000
#define     CPU$M_INV_TBSD_MMG  0x8000
#define     CPU$M_IO_INT_AFF    0x10000
#define     CPU$M_IO_START_AFF  0x20000
#define     CPU$M_CPUFILL_1     0xFFFFFFF
#define     CPU$M_CPUSPEC1      0x10000000
#define     CPU$M_CPUSPEC2      0x20000000
#define     CPU$M_CPUSPEC3      0x40000000
#define     CPU$M_CPUSPEC4      0x80000000
#define     CPU$K_NUM_SWIQS     6
#define     CPU$M_SYS_ASTEN     0xF
#define     CPU$M_SYS_ASTSR     0xF0
#define     CPU$M_SYS_FEN       0x1
#define     CPU$M_SYS_PME       0x4000000000000000
#define     CPU$M_SYS_DATFX     0x8000000000000000
#define     CPU$M_TERM_ASTEN    0xF
#define     CPU$M_TERM_ASTSR    0xF0
#define     CPU$M_TERM_FEN      0x1
#define     CPU$M_TERM_PME      0x4000000000000000
#define     CPU$M_TERM_DATFX    0x8000000000000000
#define     CPU$M_SCHED     0x1
#define     CPU$M_FOREVER       0x2
#define     CPU$M_NEWPRIM       0x4
#define     CPU$M_PSWITCH       0x8
#define     CPU$M_BC_STACK      0x10
#define     CPU$M_BC_CONTEXT    0x20
#define     CPU$M_USER_CAPABILITIES_SET 0x40
#define     CPU$M_STOPPING      0x1
#define     CPU$M_PCSAMPLE_ACTIVE       0x1
#define     CPU$M_IO_AFF_FKB_INUSE      0x1
#define     CPU$M_PORT_ASSIGNED     0x2
#define     CPU$M_DISTRIBUTED_INTS      0x4
#define     CPU$M_LASTPAGE_TESTED       0x20000000
#define     CPU$M_MCHECK        0x40000000
#define     CPU$M_MEMORY_WRITE      0x80000000
#define     CPU$M_AUTO_START        0x1
#define     CPU$M_NOBINDINGS        0x2

#define     CPU$K_SCHED_LENGTH      40

#define     CPU$K_LENGTH        2528
#define     CPU$C_LENGTH        2528
#define     CPU$M_AGE_DATA      0x1

struct _cpu
{
    struct _pcb *cpu$l_curpcb;
    // struct _ktb *cpu$l_curktb;
    unsigned char cpu$b_ipl; /* some cpus lack something. what is the connnection to PSL IPL? */
    unsigned short cpu$w_sisr; /* some cpus lacks... pending interrupts */
    unsigned char iplnr;
    unsigned char previpl[32];
    struct
    {
        unsigned char interrupt;
        unsigned char at_level;
        void * address;
    } cpu$t_ipending[256];

    //  unsigned char cpu$b_astlvl; /* some cpus lack something. pcb stuff. */
    unsigned char cpu$b_intstk; /* PSL IS. not yet used */
    unsigned char cpu$b_cur_mod; /* PSL CUR_MOD */
    unsigned char cpu$b_prv_mod; /* PSL PRV_MOD */

    unsigned long cpu$l_realstack;
    unsigned short int cpu$w_size;
    unsigned char cpu$b_type;
    unsigned char cpu$b_subtype;
    unsigned char cpu$b_busywait;
    unsigned char cpu$b_state;
    unsigned char cpu$b_cpumtx;
    unsigned char cpu$b_cur_pri;
    unsigned long cpu$l_work_req;
    unsigned long cpu$l_percpuva;
    unsigned long cpu$l_saved_ap;
    unsigned long cpu$l_haltpc;
    unsigned long cpu$l_haltpsl;
    unsigned long cpu$l_saved_isp;
    unsigned long cpu$l_pcbb;
    unsigned long cpu$l_scbb;
    unsigned long cpu$l_sisr;
    unsigned long cpu$l_p0br;
    unsigned long cpu$l_p0lr;
    unsigned long cpu$l_p1br;
    unsigned long cpu$l_p1lr;
    unsigned long cpu$l_bugcode;
    unsigned long cpu$l_mchk_mask;
    unsigned long cpu$l_mchk_sp;
    unsigned long cpu$l_p0pt_page;
    unsigned long long cpu$q_swiqfl [6];
    unsigned long cpu$l_psfl;
    unsigned long cpu$l_psbl;
    unsigned long cpu$l_boot_time;
    unsigned long cpu$l_cpuid_mask;
    unsigned long cpu$l_phy_cpuid;
    unsigned long cpu$l_capability;
    unsigned long cpu$l_tenusec;
    unsigned long cpu$l_ubdelay;
    unsigned long cpu$l_nullcpu;
    long cpu$ps_pda;
};

struct _sched_ds
{
    unsigned long long cpu$q_acc_run;
    unsigned long long cpu$q_proc_count;
    unsigned long long cpu$q_acc_interrupt;
    unsigned long long cpu$q_acc_waitime;
    unsigned long long cpu$q_sched_flags;
};

#endif

