/* <smg$routines.h>
 *
 *	Screen management run-time library routines.
 */
#ifndef _SMG$ROUTINES_H
#define _SMG$ROUTINES_H
# ifdef __cplusplus
extern "C" {
# endif

#include <zarg.h>

unsigned long smg$add_key_def();

unsigned long smg$allow_escape();

unsigned long smg$begin_display_update();

unsigned long smg$begin_pasteboard_update();

unsigned long smg$cancel_input();

unsigned long smg$change_menu();

unsigned long smg$change_pbd_characteristics(long * pasteboard_id, ...);
#define smg$change_pbd_characteristics(...) smg$change_pbd_characteristics(_buildargz7(__VA_ARGS__))

unsigned long smg$change_rendition();

unsigned long smg$change_viewport(long * display_id, ...);
#define smg$change_viewport(...) smg$change_viewport(_buildargz5(__VA_ARGS__))

unsigned long smg$change_virtual_display(long * display_id, ...);
#define smg$change_virtual_display(...) smg$change_virtual_display(_buildargz6(__VA_ARGS__))

unsigned long smg$check_for_occlusion();

unsigned long smg$control_mode();

unsigned long smg$copy_virtual_display();

int smg$cursor_column();

int smg$cursor_row();

unsigned long smg$create_key_table();

unsigned long smg$create_menu();

unsigned long smg$create_pasteboard(long * pasteboard_id, ...);
#define smg$create_pasteboard(...) smg$create_pasteboard(_buildargz7(__VA_ARGS__))

unsigned long smg$create_subprocess();

unsigned long smg$create_viewport();

unsigned long smg$create_virtual_display(int * number_of_rows, int * number_of_columns, long * display_id, ...);
#define smg$create_virtual_display(...) smg$create_virtual_display(_buildargz6(__VA_ARGS__))

unsigned long smg$create_virtual_keyboard();

unsigned long smg$del_term_table();

unsigned long smg$define_key();

unsigned long smg$delete_chars();

unsigned long smg$delete_key_def();

unsigned long smg$delete_line(long * display_id, int * start_row, ...);
#define smg$delete_line(...) smg$delete_line(_buildargz3(__VA_ARGS__))

unsigned long smg$delete_menu();

unsigned long smg$delete_pasteboard(long * pasteboard_id, ...);
#define smg$delete_pasteboard(...) smg$delete_pasteboard(buildargz2(__VA_ARGS__))

unsigned long smg$delete_subprocess();

unsigned long smg$delete_virtual_display();

unsigned long smg$delete_virtual_keyboard();

unsigned long smg$delete_viewport();

unsigned long smg$disable_broadcast_trapping();

unsigned long smg$disable_unsolicited_input();

unsigned long smg$draw_char();

unsigned long smg$draw_line();

unsigned long smg$draw_rectangle();

unsigned long smg$end_display_update();

unsigned long smg$end_pasteboard_update();

unsigned long smg$enable_unsolicited_input();

unsigned long smg$erase_chars();

unsigned long smg$erase_column();

unsigned long smg$erase_display(long * display_id);
#define smg$erase_display(...) smg$erase_display(_buildargz5(__VA_ARGS__))

unsigned long smg$erase_line(long * display_id);
#define smg$erase_line(...) smg$erase_line(_buildargz3(__VA_ARGS__))

unsigned long smg$erase_pasteboard(long * pasteboard_id, ...);
#define smg$erase_pasteboard(...) smg$erase_pasteboard(_buildargz7(__VA_ARGS__))

unsigned long smg$execute_command();

unsigned long smg$find_cursor_display();

unsigned long smg$flush_buffer();

unsigned long smg$get_broadcast_message(long * pasteboard_id, ...);
#define smg$get_broadcast_message(...) smg$get_broadcast_message(_buildargz4(__VA_ARGS__))

unsigned long smg$get_char_at_physical_cursor();

unsigned long smg$get_display_attr();

unsigned long smg$get_key_def();

unsigned long smg$get_keyboard_attributes();

unsigned long smg$get_numeric_data();

unsigned long smg$get_pasteboard_attributes();

unsigned long smg$get_pasting_info();

unsigned long smg$get_term_data();

unsigned long smg$get_viewport_char(long * display_id, ...);
#define smg$get_viewport_char(...) smg$get_viewport_char(_buildargz5(__VA_ARGS__))

unsigned long smg$home_cursor();

unsigned long smg$init_term_table();

unsigned long smg$init_term_table_by_type();

unsigned long smg$insert_chars(long * display_id, void * character_string, int * start_row, int * start_column, ...);
#define smg$insert_chars(...) smg$insert_chars(_buildargz7(__VA_ARGS__))

unsigned long smg$insert_line(long * display_id, int * start_row, ...);
#define smg$insert_line(...) smg$insert_line(_buildargz8(__VA_ARGS__))

unsigned long smg$invalidate_display();

unsigned long smg$keycode_to_name();

unsigned long smg$label_border(long * display_id, ...);
#define smg$label_border(...) smg$label_border(_buildargz7(__VA_ARGS__))

unsigned long smg$list_key_defs();

unsigned long smg$list_pasteboard_order();

unsigned long smg$list_pasting_order();

unsigned long smg$load_key_defs();

unsigned long smg$load_virtual_display();

unsigned long smg$move_virtual_display();

unsigned long smg$move_text();

unsigned long smg$name_to_keycode();

unsigned long smg$paste_virtual_display(long * display_id, long * pasteboard_id, ...);
#define smg$paste_virtual_display(...) smg$paste_virtual_display(_buildargz5(__VA_ARGS__))

unsigned long smg$put_chars(long * display_id, void * text, ...);
#define smg$put_chars(...) smg$put_chars(_buildargz8(__VA_ARGS__))

unsigned long smg$put_chars_highwide();

unsigned long smg$put_chars_multi();

unsigned long smg$put_chars_wide();

unsigned long smg$put_help_text(long * display_id, ...);
#define smg$put_help_text(...) smg$put_help_text(_buildargz6(__VA_ARGS__))

unsigned long smg$put_line(long * display_id, void * text, ...);
#define smg$put_line(...) smg$put_line(_buildargz7(__VA_ARGS__))

unsigned long smg$put_line_highwide();

unsigned long smg$put_line_multi();

unsigned long smg$put_line_wide();

unsigned long smg$pop_virtual_display();

unsigned long smg$print_pasteboard();

unsigned long smg$put_pasteboard(long * pasteboard_id, long action_routine, ...);
#define smg$put_pasteboard(...) smg$put_pasteboard(_buildargz4(__VA_ARGS__))

unsigned long smg$put_status_line();

unsigned long smg$put_virtual_display_encoded();

unsigned long smg$put_with_scroll();

unsigned long smg$read_composed_line(long * keyboard_id, long * key_table_id ,void * resultant_string, ...);
#define smg$read_composed_line(...) smg$read_composed_line(_buildargz12(__VA_ARGS__))

unsigned long smg$read_from_display();

unsigned long smg$read_keystroke();

unsigned long smg$read_locator();

unsigned long smg$read_string(long * keyboard_id, void * resultant_string, ...);
#define smg$read_string(...) smg$read_string(_buildargz14(__VA_ARGS__))

unsigned long smg$read_verify();

unsigned long smg$remove_line();

unsigned long smg$repaint_line();

unsigned long smg$repaint_screen();

unsigned long smg$repaste_virtual_display();

unsigned long smg$replace_input_line();

unsigned long smg$restore_physical_screen();

unsigned long smg$return_cursor_pos();

unsigned long smg$return_input_line();

unsigned long smg$ring_bell();

unsigned long smg$save_virtual_display();

unsigned long smg$save_physical_screen(long * pasteboard_id, long * display_id, ...);
#define smg$save_physical_screen(...) smg$save_physical_screen(_buildargz4(__VA_ARGS__))

unsigned long smg$scroll_display_area();

unsigned long smg$scroll_viewport(long * display_id, ...);
#define smg$scroll_viewport(...) smg$scroll_viewport(_buildargz3(__VA_ARGS__))

unsigned long smg$select_from_menu();

unsigned long smg$set_broadcast_trapping(long * pasteboard_id, ...);
#define smg$set_broadcast_trapping(...) smg$set_broadcast_trapping(_buildargz3(__VA_ARGS__))

unsigned long smg$set_cursor_abs(long * display_id, ...);
#define smg$set_cursor_abs(...) smg$set_cursor_abs(_buildargz3(__VA_ARGS__))

unsigned long smg$set_cursor_mode();

unsigned long smg$set_cursor_rel(long * display_id, ...);
#define smg$set_cursor_rel(...) smg$set_cursor_rel(_buildargz3(__VA_ARGS__))

unsigned long smg$set_default_state();

unsigned long smg$set_display_scroll_region();

unsigned long smg$set_keypad_mode();

unsigned long smg$set_out_of_band_asts();

unsigned long smg$set_physical_cursor();

unsigned long smg$set_term_characteristics(long * pasteboard_id, ...);
#define smg$set_term_characteristics(...) smg$set_term_characteristics(_buildargz10(__VA_ARGS__))

unsigned long smg$snapshot();

unsigned long smg$snapshot_to_printer();

unsigned long smg$unpaste_virtual_display();

# ifdef __cplusplus
}
# endif
#endif	
