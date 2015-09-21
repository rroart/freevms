#ifndef SBNBDEF_H
#define SBNBDEF_H

#define SBNB$K_LENGTH 36
#define SBNB$C_LENGTH 36
#define SBNB$S_SBNBDEF 36

struct _sbnb
{
    struct _sbnb *sbnb$l_flink;
    struct _sbnb *sbnb$l_blink;
    UINT16 sbnb$w_size;
    UINT8 sbnb$b_type;
    UINT8 sbnb$b_subtyp;
    UINT32 sbnb$l_discon_count;
    UINT8 sbnb$b_procnam [16];
    UINT16 sbnb$w_local_index;
    UINT16 sbnb$w_unused_1;
};

#endif

