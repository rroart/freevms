#ifndef FM2DEF_H
#define FM2DEF_H

/*
 *  Mapping area structures and definitions
 */

/* Mapping pointer type codes */
#define FM2$C_PLACEMENT 0
#define FM2$C_FORMAT1   1
#define FM2$C_FORMAT2   2
#define FM2$C_FORMAT3   3

/* Map pointer FM2$C_PLACEMENT bit definitions */
#define FM2$V_EXACT  0
#define FM2$V_ONCYL  1
#define FM2$V_LBN    12
#define FM2$V_RVN    13
#define FM2$V_FORMAT 14

#define FM2$K_LENGTH0 2
#define FM2$C_LENGTH0 2
#define FM2$K_LENGTH1 4
#define FM2$C_LENGTH1 4
#define FM2$S_FM2DEF  4

struct _fm2
{
    union
    {
        UINT16 fm2$w_map_type;
        struct
        {
            UINT16 fm2$v_exact :1; /* Placement is exact */
            UINT16 fm2$v_oncyl :1; /* Space allocated on a single cylinder */
            UINT16 fm2$v_fill :10; /* Unused bits */
            UINT16 fm2$v_lbn :1; /* Allocate space using LBN in next ptr */
            UINT16 fm2$v_rvn :1; /* Allocate space on this rel. volume */
            UINT16 fm2$v_format0 :2; /* Must be FM2$C_PLACEMENT (0) */
        };
        struct
        {
            UINT16 fm2$v_count1 :8; /* Number of blocks pointed to (< 256) */
            UINT16 fm2$v_highlbn :6; /* High order 6 bits of LBN */
            UINT16 fm2$v_format1 :2; /* Must be FM2$C_FORMAT1 (1) */
        };
        struct
        {
            UINT16 fm2$v_count2 :14; /* Number of blocks pointed to (< 16384)*/
            UINT16 fm2$v_format2 :2; /* Must be FM2$C_FORMAT2 (2) */
        };
        struct
        {
            UINT16 fm2$v_hicount :14; /* High order part of the block count */
            UINT16 fm2$v_format3 :2; /* Must be FM2$C_FORMAT3 (3) */
        };
    };
    UINT16 fm2$w_lowlbn; /* Low order 16 bit of LBN, only for FM2$C_FORMAT1 (1) */
};

#endif

