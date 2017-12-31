#ifndef shm_iddef_h
#define shm_iddef_h

struct _shm_id
{
    union
    {
        struct
        {
            union
            {
                unsigned int shm_id$l_short_id;
                struct
                {
                    unsigned short int shm_id$w_index;
                    unsigned short int shm_id$w_extent;
                };
            };
            unsigned int shm_id$l_seq_num;
        };
        unsigned long long shm_id$q_ident;
    };
};

typedef unsigned long long SHM_ID_PQ;

#endif

