#ifndef bktdef_h
#define bktdef_h

#define BKT$K_OVERHDSZ 14
#define BKT$C_OVERHDSZ 14
#define BKT$M_LASTBKT 0x1
#define BKT$M_ROOTBKT 0x2
#define BKT$M_PTR_SZ 0x18
#define BKT$C_ENDOVHD 4
#define BKT$C_DATBKTOVH 2
#define BKT$C_DUPBKTOVH 4

#define BKT$C_MAXBKTSIZ 63
#define BKT$S_BKTDEF 14

// looks like General Bucket Header in rmsint2.doc?

struct _bkt
{
    unsigned char bkt$b_checkchar;
    union
    {
        unsigned char bkt$b_areano;
        unsigned char bkt$b_indexno;
    };
    unsigned short int bkt$w_adrsample;
    union
    {
        unsigned short int bkt$w_freespace;
        unsigned short int bkt$w_keyfrespc;
    };
    union
    {
        unsigned short int bkt$w_nxtrecid;
        struct
        {
            unsigned char bkt$b_nxtrecid;
            unsigned char bkt$b_lstrecid;
        };
    };
    unsigned int bkt$l_nxtbkt;
    unsigned char bkt$b_level;
    union
    {
        unsigned char bkt$b_bktcb;
#if 0
        // offset probs
        struct
        {
            unsigned bkt$v_lastbkt : 1;
            unsigned bkt$v_rootbkt : 1;
            unsigned bkt$$_fill_1  : 1;
            unsigned bkt$v_ptr_sz  : 2;
            unsigned bkt$v_fill_4_ : 3;
        };
#endif
    };
};

#endif

