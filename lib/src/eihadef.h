#ifndef EIHADEF_H
#define EIHADEF_H

#define EIHA$K_LENGTH 48
#define EIHA$C_LENGTH 48
#define EIHA$S_EIHADEF 48

struct _eiha
{
    unsigned int eiha$l_size;
    unsigned int eiha$l_spare;
    union
    {
        unsigned long long eiha$q_tfradr1;
        struct
        {
            unsigned int eiha$l_tfradr1;
            unsigned int eiha$l_tfradr1_h;
        };
    };
    union
    {
        unsigned long long eiha$q_tfradr2;
        struct
        {
            unsigned int eiha$l_tfradr2;
            unsigned int eiha$l_tfradr2_h;
        };
    };
    union
    {
        unsigned long long eiha$q_tfradr3;
        struct
        {
            unsigned int eiha$l_tfradr3;
            unsigned int eiha$l_tfradr3_h;
        };
    };
    union
    {
        unsigned long long eiha$q_tfradr4;
        struct
        {
            unsigned int eiha$l_tfradr4;
            unsigned int eiha$l_tfradr4_h;
        };
    };
    union
    {
        unsigned int eiha$l_inishr;
        unsigned long long eiha$q_inishr;
    };
};

#endif

