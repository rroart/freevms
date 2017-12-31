#ifndef mtxdef_h
#define mtxdef_h

#define MTX$S_MTXDEF 4

struct _mtx
{
    union
    {
        int mtxdef$$_fill_1;
        struct
        {
            unsigned mtxdef$$_fill_2 : 16;
            unsigned mtx$v_wrt : 1;
            unsigned mtx$v_interlock : 1;
            unsigned mtx$v_fill_2_ : 6;
        };
        struct
        {
            unsigned short int mtx$w_owncnt;
            unsigned short int mtx$w_sts;
        };
    };
};

#endif

