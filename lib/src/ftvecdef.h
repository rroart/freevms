#ifndef ftvecdef_h
#define ftvecdef_h

#include <ttyvecdef.h>

#define PORT_FT_LENGTH 100
#define PORTS_FTVECDEF 100

struct _ft_vec
{
    struct _tt_port portr_ft_tt_port;
    int (*port_ft_create)();
    int (*port_ft_read)();
    int (*port_ft_write)();
    int (*port_ft_set_event)();
    int (*port_ft_cancel)();
    int (*port_ft_decterm_set)();
};

#endif

