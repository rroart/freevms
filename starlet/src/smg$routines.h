/* <smg$routines.h>
 *
 *	Screen management run-time library routines.
 */
#ifndef _SMG$ROUTINES_H
#define _SMG$ROUTINES_H
# ifdef __cplusplus
extern "C" {
# endif


unsigned long smg$add_key_def();

unsigned long smg$allow_escape();

unsigned long smg$begin_display_update();

unsigned long smg$begin_pasteboard_update();

unsigned long smg$cancel_input();

unsigned long smg$change_menu();

unsigned long smg$change_pbd_characteristics();

unsigned long smg$change_rendition();

unsigned long smg$change_viewport();

unsigned long smg$change_virtual_display();

unsigned long smg$check_for_occlusion();

unsigned long smg$control_mode();

unsigned long smg$copy_virtual_display();

int smg$cursor_column();

int smg$cursor_row();

unsigned long smg$create_key_table();

unsigned long smg$create_menu();

unsigned long smg$create_pasteboard();

unsigned long smg$create_subprocess();

unsigned long smg$create_viewport();

unsigned long smg$create_virtual_display();

unsigned long smg$create_virtual_keyboard();

unsigned long smg$del_term_table();

unsigned long smg$define_key();

unsigned long smg$delete_chars();

unsigned long smg$delete_key_def();

unsigned long smg$delete_line();

unsigned long smg$delete_menu();

unsigned long smg$delete_pasteboard();

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

unsigned long smg$erase_display();

unsigned long smg$erase_line();

unsigned long smg$erase_pasteboard();

unsigned long smg$execute_command();

unsigned long smg$find_cursor_display();

unsigned long smg$flush_buffer();

unsigned long smg$get_broadcast_message();

unsigned long smg$get_char_at_physical_cursor();

unsigned long smg$get_display_attr();

unsigned long smg$get_key_def();

unsigned long smg$get_keyboard_attributes();

unsigned long smg$get_numeric_data();

unsigned long smg$get_pasteboard_attributes();

unsigned long smg$get_pasting_info();

unsigned long smg$get_term_data();

unsigned long smg$get_viewport_char();

unsigned long smg$home_cursor();

unsigned long smg$init_term_table();

unsigned long smg$init_term_table_by_type();

unsigned long smg$insert_chars();

unsigned long smg$insert_line();

unsigned long smg$invalidate_display();

unsigned long smg$keycode_to_name();

unsigned long smg$label_border();

unsigned long smg$list_key_defs();

unsigned long smg$list_pasteboard_order();

unsigned long smg$list_pasting_order();

unsigned long smg$load_key_defs();

unsigned long smg$load_virtual_display();

unsigned long smg$move_virtual_display();

unsigned long smg$move_text();

unsigned long smg$name_to_keycode();

unsigned long smg$paste_virtual_display();

unsigned long smg$put_chars();

unsigned long smg$put_chars_highwide();

unsigned long smg$put_chars_multi();

unsigned long smg$put_chars_wide();

unsigned long smg$put_help_text();

unsigned long smg$put_line();

unsigned long smg$put_line_highwide();

unsigned long smg$put_line_multi();

unsigned long smg$put_line_wide();

unsigned long smg$pop_virtual_display();

unsigned long smg$print_pasteboard();

unsigned long smg$put_pasteboard();

unsigned long smg$put_status_line();

unsigned long smg$put_virtual_display_encoded();

unsigned long smg$put_with_scroll();

unsigned long smg$read_composed_line();

unsigned long smg$read_from_display();

unsigned long smg$read_keystroke();

unsigned long smg$read_locator();

unsigned long smg$read_string();

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

unsigned long smg$save_physical_screen();

unsigned long smg$scroll_display_area();

unsigned long smg$scroll_viewport();

unsigned long smg$select_from_menu();

unsigned long smg$set_broadcast_trapping();

unsigned long smg$set_cursor_abs();

unsigned long smg$set_cursor_mode();

unsigned long smg$set_cursor_rel();

unsigned long smg$set_default_state();

unsigned long smg$set_display_scroll_region();

unsigned long smg$set_keypad_mode();

unsigned long smg$set_out_of_band_asts();

unsigned long smg$set_physical_cursor();

unsigned long smg$set_term_characteristics();

unsigned long smg$snapshot();

unsigned long smg$snapshot_to_printer();

unsigned long smg$unpaste_virtual_display();

# ifdef __cplusplus
}
# endif
#endif	
