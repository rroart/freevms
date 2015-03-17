#ifndef keydef_h
#define keydef_h

#define		KEY$M_DUPKEYS		0x1
#define		KEY$M_CHGKEYS		0x2
#define		KEY$M_NULKEYS		0x4
#define		KEY$M_IDX_COMPR		0x8
#define		KEY$M_INITIDX		0x10
#define		KEY$M_KEY_COMPR		0x40
#define		KEY$M_REC_COMPR		0x80
#define		KEY$C_MAX_DAT		10

#define		KEY$C_MAX_PRIMARY	6

#define		KEY$C_MAX_INDEX		6

#define		KEY$C_STRING		0
#define		KEY$C_SGNWORD		1
#define		KEY$C_UNSGNWORD		2
#define		KEY$C_SGNLONG		3
#define		KEY$C_UNSGNLONG		4
#define		KEY$C_PACKED		5
#define		KEY$C_SGNQUAD		6
#define		KEY$C_UNSGNQUAD		7
#define		KEY$C_COLLATED		8
#define		KEY$C_MAX_ASCEND	8
#define		KEY$C_DSTRING		32
#define		KEY$C_DSGNWORD		33
#define		KEY$C_DUNSGNWORD	34
#define		KEY$C_DSGNLONG		35
#define		KEY$C_DUNSGNLONG	36
#define		KEY$C_DPACKED		37
#define		KEY$C_DSGNQUAD		38
#define		KEY$C_DUNSGNQUAD	39
#define		KEY$C_DCOLLATED		40
#define		KEY$C_MAX_DATA		40
#define		KEY$K_BLN		96
#define		KEY$C_BLN		96
#define		KEY$C_SPARE		6
#define		KEY$S_KEYDEF		96

// like Indexed File Prologue Block 1 in rmsint2.doc?

struct _prologue_key
{
    unsigned int key$l_idxfl;
    unsigned short key$w_noff;
    unsigned char key$b_ianum;
    unsigned char key$b_lanum;
    unsigned char key$b_danum;
    unsigned char key$b_rootlev;
    unsigned char key$b_idxbktsz;
    unsigned char key$b_datbktsz;
    unsigned int key$l_rootvbn;
    union
    {
        unsigned char key$b_flags;
#if 0
        // gcc bitfield problems
        struct
        {
            unsigned key$v_dupkeys		: 1;
            unsigned key$v_chgkeys		: 1;
            unsigned key$v_nulkeys		: 1;
            unsigned key$v_idx_compr		: 1;
            unsigned key$v_initidx		: 1;
            unsigned key$$_fill_1		: 1;
            unsigned key$v_key_compr		: 1;
            unsigned key$v_fill_8_		: 1;
        };
        struct
        {
            unsigned key$$_fill_2		: 1;
            unsigned key$$_fill_3		: 2;
            unsigned key$$_fill_4		: 1;
            unsigned key$$_fill_5		: 1;
            unsigned key$$_fill_6		: 1;
            unsigned key$$_fill_7		: 1;
            unsigned key$v_rec_compr		: 1;
        };
#endif
    };
    unsigned char key$b_datatype;
    unsigned char key$b_segments;
    unsigned char key$b_nullchar;
    unsigned char key$b_keysz;
    unsigned char key$b_keyref;
    unsigned short key$w_minrecsz;
    unsigned short key$w_idxfill;
    unsigned short key$w_datfill;
    union
    {
        unsigned short key$w_position;
        unsigned short key$w_position0;
    };
    unsigned short key$w_position1;
    unsigned short key$w_position2;
    unsigned short key$w_position3;
    unsigned short key$w_position4;
    unsigned short key$w_position5;
    unsigned short key$w_position6;
    unsigned short key$w_position7;
    union
    {
        unsigned char key$b_size;
        unsigned char key$b_size0;
    };
    unsigned char key$b_size1;
    unsigned char key$b_size2;
    unsigned char key$b_size3;
    unsigned char key$b_size4;
    unsigned char key$b_size5;
    unsigned char key$b_size6;
    unsigned char key$b_size7;
    char key$t_keynam [32];
    unsigned int key$l_ldvbn;
    union
    {
        unsigned char key$b_type;
        unsigned char key$b_type0;
    };
    unsigned char key$b_type1;
    unsigned char key$b_type2;
    unsigned char key$b_type3;
    unsigned char key$b_type4;
    unsigned char key$b_type5;
    unsigned char key$b_type6;
    unsigned char key$b_type7;
};

#endif

