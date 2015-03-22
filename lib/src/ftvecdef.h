#ifndef FTVECDEF_H
#define FTVECDEF_H

#include <ttyvecdef.h>

#define PORT_FT_LENGTH 100
#define PORTS_FTVECDEF 100

struct _ft_vec
{
    struct _tt_port portr_ft_tt_port;
    int (*port_ft_create)(void);
    int (*port_ft_read)(void);
    int (*port_ft_write)(void);
    int (*port_ft_set_event)(void);
    int (*port_ft_cancel)(void);
    int (*port_ft_decterm_set)(void);
};

#endif
