#ifndef FDTDEF_H
#define FDTDEF_H

#define FDT$K_LENGTH 528

struct __fdt
{
    unsigned long long fdt$q_valid;
    //    unsigned long long fdt$q_ok64bit;
    unsigned long long fdt$q_buffered;
    struct
    {
        unsigned long long fdt$q_mask;
        int (*fdt$ps_func_rtn)(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);
    } f[64];
    //    int (*fdt$ps_func_rtn [64])(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);
    //    unsigned long fdt$l_qsrv_sts2 [64];
};

struct _fdt
{
    unsigned long long fdt$q_valid;
    unsigned long long fdt$q_buffered;
    int (*fdt$ps_func_rtn [64])(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);
};

#endif

