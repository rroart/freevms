#ifndef SMGDEF2_H
#define SMGDEF2_H

#undef SMG$M_CURSOR_OFF
static int SMG$M_CURSOR_OFF = 1;
#undef SMG$M_CURSOR_ON
static int SMG$M_CURSOR_ON = 2;
#undef SMG$M_UP
static int SMG$M_UP = 1;
#undef SMG$M_DOWN
static int SMG$M_DOWN = 2;
#undef SMG$M_ERASE_LINE
static int SMG$M_ERASE_LINE = 1;
#undef SMG$M_WRAP_CHAR
static int SMG$M_WRAP_CHAR = 1;
#undef SMG$M_BOLD
static int SMG$M_BOLD = 1;
#undef SMG$M_KEEP_CONTENTS
static int SMG$M_KEEP_CONTENTS = 1;
#undef SMG$M_BORDER
static int SMG$M_BORDER = 1;

#define SMG$SET_CURSOR_MODE smg$set_cursor_mode
#define SMG$END_PASTEBOARD_UPDATE smg$end_pasteboard_update
#define SMG$READ_COMPOSED_LINE smg$read_composed_line
#define SMG$ERASE_DISPLAY smg$erase_display
#define SMG$CHANGE_VIEWPORT smg$change_viewport
#define SMG$SET_CURSOR_ABS smg$set_cursor_abs
#define SMG$DELETE_PASTEBOARD smg$delete_pasteboard
#define SMG$SAVE_PHYSICAL_SCREEN smg$save_physical_screen
#define SMG$DISABLE_BROADCAST_TRAPPING smg$disable_broadcast_trapping
#define SMG$RESTORE_PHYSICAL_SCREEN smg$restore_physical_screen
#define SMG$GET_VIEWPORT_CHAR smg$get_viewport_char
#define SMG$RING_BELL smg$ring_bell
#define SMG$ERASE_LINE smg$erase_line
#define SMG$SET_BROADCAST_TRAPPING smg$set_broadcast_trapping
#define SMG$SCROLL_VIEWPORT smg$scroll_viewport
#define SMG$CHANGE_PBD_CHARACTERISTICS smg$change_pbd_characteristics
#define SMG$CHANGE_VIRTUAL_DISPLAY smg$change_virtual_display
#define SMG$CANCEL_INPUT smg$cancel_input
#define SMG$REPAINT_SCREEN smg$repaint_screen
#define SMG$PUT_CHARS smg$put_chars
#define SMG$PUT_LINE smg$put_line
#define SMG$GET_BROADCAST_MESSAGE smg$get_broadcast_message
#define SMG$GET_BROADCAST_MESSAGE smg$get_broadcast_message
#define SMG$CREATE_VIEWPORT smg$create_viewport
#define SMG$CREATE_VIRTUAL_DISPLAY smg$create_virtual_display
#define SMG$LABEL_BORDER smg$label_border
#define SMG$PASTE_VIRTUAL_DISPLAY smg$paste_virtual_display
#define SMG$BEGIN_PASTEBOARD_UPDATE smg$begin_pasteboard_update
#define SMG$ADD_KEY_DEF smg$add_key_def
#define SMG$ERASE_PASTEBOARD smg$erase_pasteboard
#define SMG$CREATE_VIRTUAL_KEYBOARD smg$create_virtual_keyboard
#define SMG$CREATE_KEY_TABLE smg$create_key_table
#define SMG$CREATE_PASTEBOARD smg$create_pasteboard
#define SMG$PUT_PASTEBOARD smg$put_pasteboard

static int i0 = 0;
static int i1 = 1;
static int i2 = 2;
static int i80 = 80;
static int i132 = 132;
static int i500 = 500;

#endif /* SMGDEF2_H */
