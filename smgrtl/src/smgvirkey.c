// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <descrip.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ssdef.h>
#include <starlet.h>

struct virtual_keyboard
{
    short int smg$w_chan;
};

#if 0
int
smg$delete_virtual_keyboard(void * keyboard_id)
{
    if (keyboard_id)
        free(keyboard_id);
    return 1;
}
#endif

int
smg$create_key_table(unsigned int * key_table_id)
{
    *key_table_id=0;
    return 1;
}

int
smg$create_virtual_keyboard(unsigned long * keyboard_id , void * input_device , void * default_filespec , void * resultant_filespec , unsigned char * recall_size)
{
    int status;
    $DESCRIPTOR(dsc, "sys$input");
    struct virtual_keyboard * smg = malloc (sizeof (struct virtual_keyboard));
    *keyboard_id = smg;
    if (input_device == 0)
        input_device = &dsc;
    extern int smgunix;
    if (!smgunix)
        status = sys$assign (input_device, &smg->smg$w_chan, 0, 0, 0);
    else
    {
        smg->smg$w_chan = 0;
        status = 1;
    }
    return 1;
}

int
smg$delete_virtual_keyboard(unsigned long * keyboard_id)
{
    free (*keyboard_id);
    return 1;
}

int
smg$cancel_input(unsigned long * keyboard_id)
{
    struct virtual_keyboard * smg = * keyboard_id;
    sys$cancel (smg->smg$w_chan);
    return 1;
}

int smg$$virtual_kbd_chan(struct virtual_keyboard * v, short int * chan)
{
    *chan = v->smg$w_chan;
    return 1;
}

int smg$add_key_def ( long * key_table_id, void * key_name, int * if_state, int * attributes, void * equivalence_string, void * state_string)
{
    return SS$_NORMAL;
}
