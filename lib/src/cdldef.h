#ifndef cdldef_h
#define cdldef_h

#define CDL$C_LENGTH 16
#define CDL$S_CDLDEF 20

struct _cdl
{
    unsigned short cdl$w_maxconidx;
    short cdldef$$_fill_1;
    unsigned long cdl$l_freecdt;
    unsigned short cdl$w_size;
    unsigned char cdl$b_type;
    unsigned char cdl$b_subtyp;
    unsigned long cdl$l_nocdt_cnt;
    void *cdl$l_base;
};

#endif

