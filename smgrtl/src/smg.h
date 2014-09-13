#ifndef smg_h
#define smg_h

struct virtual_display;

int smg$$get_pasteboard_chan (void * pasteboard_id, short * chan);
int smg$$display (long * display_id, int row, int column);
int smg$$set_display_pasteboard(long * display_id, long * pasteboard_id);
int smg$$cursor (long * display_id);
int smg$$virtual_kbd_chan(struct virtual_keyboard * v, short int * chan);
int smg$$insert_viewport(struct virtual_display * s, long ** v);
int smg$$get_viewport(struct virtual_display * s, long ** v);
int smg$$pasteboard_update (long * pasteboard_id);

#endif
