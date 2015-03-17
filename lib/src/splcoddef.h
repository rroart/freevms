#ifndef splcoddef_h
#define splcoddef_h

#define SPL$C_EMB 32
#define SPL$C_MCHECK 33
#define SPL$C_MEGA 34
#define SPL$C_HWCLK 36
#define SPL$C_INVALIDATE 38
#define SPL$C_PERFMON 40
#define SPL$C_POOL 42
#define SPL$C_MAILBOX 44
#define SPL$C_IOLOCK11 46
#define SPL$C_IOLOCK10 47
#define SPL$C_IOLOCK9 48
#define SPL$C_SCHED 50
#define SPL$C_MMG 52
#define SPL$C_IO_MISC 54
#define SPL$C_PORT 55
#define SPL$C_TIMER 56
#define SPL$C_TX_SYNCH 57
#define SPL$C_IOLOCK8 58
#define SPL$C_LCKMGR 59
#define SPL$C_FILSYS 60
#define SPL$C_QUEUEAST 62
#define SPL$C_SCS 58
#define SPL$_MIN_INDEX 32
#define SPL$_MAX_INDEX 62
#define SPL$_NUM_LOCKS 31



#define SMP$M_ENABLED 0x1
#define SMP$M_START_CPU 0x2
#define SMP$M_CRASH_CPU 0x4
#define SMP$M_TODR 0x8
#define SMP$M_UNMOD_DRIVER 0x10
#define SMP$M_TODR_ACK 0x20
#define SMP$M_SYNCH 0x40
#define SMP$M_BENIGN 0x80
#define SMP$M_MINIMUM_ACQUIRE 0x100
#define SMP$M_READ_SCC 0x200
#define SMP$M_READ_SCC_ACK 0x400
#define SMP$M_CLOCKS_SYNCH 0x800
#define SMP$M_DISPLAY_TRANSITIONS 0x1000
#define SMP$M_OVERRIDE 0x1
#define SMP$M_FOREVER 0x2
#define SMP$M_FKB_FRU_CHANGE 0x1
#define SMP$M_FKB_DOORBELL 0x2
#define SMP$S_SMPDEF 4

struct _smp
{
    struct
    {
        unsigned smp$v_enabled : 1;
        unsigned smp$v_start_cpu : 1;
        unsigned smp$v_crash_cpu : 1;
        unsigned smp$v_todr : 1;
        unsigned smp$v_unmod_driver : 1;
        unsigned smp$v_todr_ack : 1;
        unsigned smp$v_synch : 1;
        unsigned smp$v_benign : 1;
        unsigned smp$v_minimum_acquire : 1;
        unsigned smp$v_read_scc : 1;
        unsigned smp$v_read_scc_ack : 1;
        unsigned smp$v_clocks_synch : 1;

        unsigned smp$v_display_transitions : 1;
        unsigned smp$v_fill_0_ : 3;
    };
    struct
    {
        unsigned smp$v_override : 1;
        unsigned smp$v_forever : 1;

        unsigned smp$v_fill_1_ : 6;
    };
    struct
    {
        unsigned smp$v_fkb_fru_change : 1;
        unsigned smp$v_fkb_doorbell : 1;
        unsigned smp$v_fill_2_ : 6;
    };
};

#endif

