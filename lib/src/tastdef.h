#ifndef tastdef_h
#define tastdef_h

#define TAST$M_MASK_DSBL 0x1
#define TAST$M_INCLUDE 0x2
#define TAST$M_ONE_SHOT 0x4
#define TAST$M_BUSY 0x8
#define TAST$M_LOST 0x10
#define TAST$M_ABORT 0x20
#define TAST$K_LENGTH 60
#define TAST$C_LENGTH 60
#define TAST$M_ABO 0x4000
#define TAST$M_INC 0x8000

#define TAST$S_TASTDEF 64

struct _tast
{
    int tastdef$$filler1 [9];
    struct _tast *tast$l_flink;
    int (*tast$l_ast)();
    unsigned int tast$l_astprm;
    unsigned int tast$l_pid;
    unsigned char tast$b_rmod;
    union
    {
        unsigned char tast$b_ctrl;
        struct
        {
            unsigned tast$v_mask_dsbl		: 1;
            unsigned tast$v_include		: 1;
            unsigned tast$v_one_shot		: 1;
            unsigned tast$v_busy		: 1;
            unsigned tast$v_lost		: 1;
            unsigned tast$v_abort		: 1;
            unsigned tast$v_fill_0_		: 2;
        };
    };
    unsigned short int tast$w_chan;
    unsigned int tast$l_mask;
    struct
    {
        unsigned tast$v_fill	: 14;
        unsigned tast$v_abo		: 1;
        unsigned tast$v_inc		: 1;
    };
    char tast$b_filler1[2];
};

#endif

