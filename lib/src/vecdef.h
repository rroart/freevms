#ifndef VECDEF_H
#define VECDEF_H

#define VEC$K_LENGTH 16
#define VEC$C_LENGTH 16
#define S_VECDEF     16

struct _vec
{
    void *vec$ps_isr_code;
    void (*vec$ps_isr_pd)(void);
    struct _idb *vec$l_idb;
    struct _adp *vec$ps_adp;
};

#endif

