#ifndef SHM_IDDEF_H
#define SHM_IDDEF_H

#include <vms_types.h>

struct _shm_id
{
    union
    {
        struct
        {
            union
            {
                UINT32 shm_id$l_short_id;
                struct
                {
                    UINT16 shm_id$w_index;
                    UINT16 shm_id$w_extent;
                };
            };
            UINT32 shm_id$l_seq_num;
        };
        UINT64 shm_id$q_ident;
    };
};

typedef struct _shm_id * SHM_ID_PQ;

#endif

