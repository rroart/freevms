#ifndef WSLDEF_H
#define WSLDEF_H

#define WSL$M_VALID   0x1
#define WSL$M_PAGTYP  0xE
#define WSL$M_PFNLOCK 0x10
#define WSL$M_WSLOCK  0x20
#define WSL$M_GOODPAGE 0x40
#define WSL$M_MODIFY   0x100
#define WSL$C_LENGTH   8

#define WSL$C_PROCESS 0
#define WSL$C_SYSTEM 2
#define WSL$C_GLOBAL 4
#define WSL$C_GBLWRT 6
#define WSL$C_PPGTBL 8
#define WSL$C_GPGTBL 10
#define WSL$C_RESERVED 12
#define WSL$C_UNKNOWN 14
#define WSL$S_WSLDEF 8
#define WSL$C_SHIFT_SIZE 3

struct _wsl
{
    union
    {
        void *wsl$pq_va;
        struct
        {
            unsigned wsl$v_valid     : 1;
            unsigned wsl$v_pagtyp    : 3;
            unsigned wsl$v_pfnlock   : 1;
            unsigned wsl$v_wslock    : 1;
            unsigned wsl$v_goodpage  : 1;
            unsigned wsldef$$_fill   : 1;
            unsigned wsl$v_modify    : 1;
            unsigned wsl$v_fillme    : 7;
        };
    };
};

#endif

