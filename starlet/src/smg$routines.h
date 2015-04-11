/* <smg$routines.h>
 *
 *  Screen management run-time library routines.
 */
#ifndef _SMG$ROUTINES_H
#define _SMG$ROUTINES_H
# ifdef __cplusplus
extern "C"
{
# endif

#include <zarg.h>

    int smg$add_key_def(long * key_table_id, void * key_name, int * if_state,
                        int * attributes, void * equivalence_string, void * state_string);

    int smg$allow_escape(void);

    int smg$begin_display_update(long * display_id);

    int smg$begin_pasteboard_update(long * pasteboard_id);

    int smg$cancel_input(unsigned long * keyboard_id);

    int smg$change_menu(void);

#if 0
// not yet
    int smg$change_pbd_characteristics(long * pasteboard_id, ...);
#define smg$change_pbd_characteristics(...) smg$change_pbd_characteristics(_buildargz7(__VA_ARGS__))
#endif
    int smg$change_pbd_characteristics(long * pasteboard_id, int * desired_width,
                                       int * width, int * desired_height, int * height,
                                       int * desired_background_color, int * background_color);

    int smg$change_rendition(void);

    int smg$change_viewport(long * display_id, ...);
#define smg$change_viewport(...) smg$change_viewport(_buildargz5(__VA_ARGS__))

#if 0
// not yet
    int smg$change_virtual_display(long * display_id, ...);
#define smg$change_virtual_display(...) smg$change_virtual_display(_buildargz6(__VA_ARGS__))
#endif
    int smg$change_virtual_display(long * display_id, int * number_of_rows,
                                   int * number_of_columns, int * display_attributes,
                                   int * video_attributes, int * character_set);

    int smg$check_for_occlusion(void);

    int smg$control_mode(void);

    int smg$copy_virtual_display(long * current_display_id, long * new_display_id);

    int smg$cursor_column(long * display_id);

    int smg$cursor_row(long * display_id);

    int smg$create_key_table(unsigned int * key_table_id);

    int smg$create_menu(void);

#if 0
// not yet
    int smg$create_pasteboard(long * pasteboard_id, ...);
#define smg$create_pasteboard(...) smg$create_pasteboard(_buildargz7(__VA_ARGS__))
#endif
    int smg$create_pasteboard(long * pasteboard_id, void * output_device,
                              signed int * number_of_pasteboard_rows,
                              signed int * number_of_pasteboard_columns, unsigned int * flags,
                              unsigned int * type_of_terminal, void * device_name);

    int smg$create_subprocess(void);

    int smg$create_viewport(long * display_id, int * viewport_row_start,
                            int * viewport_column_start, int * viewport_number_rows,
                            int * viewport_number_columns);

#if 0
// not yet
    int smg$create_virtual_display(int * number_of_rows, int * number_of_columns, long * display_id, ...);
#define smg$create_virtual_display(...) smg$create_virtual_display(_buildargz6(__VA_ARGS__))
#endif
    int smg$create_virtual_display(int * number_of_rows, int * number_of_columns,
                                   long * display_id, int * display_attributes, int * video_attributes,
                                   int * character_set);

    int smg$create_virtual_keyboard(unsigned long * keyboard_id,
                                    void * input_device, void * default_filespec, void * resultant_filespec,
                                    unsigned char * recall_size);

    int smg$del_term_table(void);

    int smg$define_key(void);

    int smg$delete_chars(long * display_id, int * number_of_characters,
                         int * start_row, int * start_column);

    int smg$delete_key_def(void);

#if 0
// not yet
    int smg$delete_line(long * display_id, int * start_row, ...);
#define smg$delete_line(...) smg$delete_line(_buildargz3(__VA_ARGS__))
#endif
    int smg$delete_line(long * display_id, int * start_row, int * number_of_rows);

    int smg$delete_menu(void);

#if 0
// not yet
    int smg$delete_pasteboard(long * pasteboard_id, ...);
#define smg$delete_pasteboard(...) smg$delete_pasteboard(_buildargz2(__VA_ARGS__))
#endif
    int smg$delete_pasteboard(long * pasteboard_id, int * flags);

    int smg$delete_subprocess(void);

    int smg$delete_virtual_display(long * display_id);

    int smg$delete_virtual_keyboard(void);

    int smg$delete_viewport(long * display_id);

    int smg$disable_broadcast_trapping(long * pasteboard_id);

    int smg$disable_unsolicited_input(void);

    int smg$draw_char(void);

    int smg$draw_line(void);

    int smg$draw_rectangle(void);

    int smg$end_display_update(long * display_id);

    int smg$end_pasteboard_update(long * pasteboard_id);

    int smg$enable_unsolicited_input(void);

    int smg$erase_chars(long * display_id, int * number_of_characters,
                        int * start_row, int * start_column);

    int smg$erase_column(void);

#if 0
// not yet
    int smg$erase_display(long * display_id, ...);
#define smg$erase_display(...) smg$erase_display(_buildargz5(__VA_ARGS__))
#endif
    int smg$erase_display(long * display_id, int * start_row, int * start_column,
                          int * end_row, int * end_column);

#if 0
// not yet
    int smg$erase_line(long * display_id, ...);
#define smg$erase_line(...) smg$erase_line(_buildargz3(__VA_ARGS__))
#endif
    int smg$erase_line(long * display_id, int * start_row, int * number_of_rows);

#if 0
// not yet
    int smg$erase_pasteboard(long * pasteboard_id, ...);
#define smg$erase_pasteboard(...) smg$erase_pasteboard(_buildargz7(__VA_ARGS__))
#endif
    int smg$erase_pasteboard(long * pasteboard_id, void * output_device,
                             signed int * number_of_pasteboard_rows,
                             signed int * number_of_pasteboard_columns, unsigned int * flags,
                             unsigned int * type_of_terminal, void * device_name);

    int smg$execute_command(void);

    int smg$find_cursor_display(void);

    int smg$flush_buffer(long * pasteboard_id);

#if 0
// not yet
    int smg$get_broadcast_message(long * pasteboard_id, ...);
#define smg$get_broadcast_message(...) smg$get_broadcast_message(_buildargz4(__VA_ARGS__))
#endif
    int smg$get_broadcast_message(long * pasteboard_id, void * message,
                                  short * message_length, short * message_type);

    int smg$get_char_at_physical_cursor(void);

    int smg$get_display_attr(void);

    int smg$get_key_def(void);

    int smg$get_keyboard_attributes(void);

    int smg$get_numeric_data(void);

    int smg$get_pasteboard_attributes(void);

    int smg$get_pasting_info(void);

    int smg$get_term_data(void);

#if 0
// not yet
    int smg$get_viewport_char(long * display_id, ...);
#define smg$get_viewport_char(...) smg$get_viewport_char(_buildargz5(__VA_ARGS__))
#endif
    int smg$get_viewport_char(long * display_id, int * viewport_row_start,
                              int * viewport_column_start, int * viewport_number_rows,
                              int * viewport_number_columns);

    int smg$home_cursor(void);

    int smg$init_term_table(void);

    int smg$init_term_table_by_type(void);

#if 0
// not yet
    int smg$insert_chars(long * display_id, void * character_string, int * start_row, int * start_column, ...);
#define smg$insert_chars(...) smg$insert_chars(_buildargz7(__VA_ARGS__))
#endif
    int smg$insert_chars(long * display_id, void * character_string,
                         int * start_row, int * start_column, int * rendition_set,
                         int * rendition_complement, int * character_set);

#if 0
// not yet
    int smg$insert_line(long * display_id, int * start_row, ...);
#define smg$insert_line(...) smg$insert_line(_buildargz8(__VA_ARGS__))
#endif
    int smg$insert_line(long * display_id, int * start_row, void * character_string,
                        int * direction, int * rendition_set, int * rendition_complement,
                        int * flags, int * character_set);

    int smg$invalidate_display(void);

    int smg$keycode_to_name(void);

#if 0
// not yet
    int smg$label_border(long * display_id, ...);
#define smg$label_border(...) smg$label_border(_buildargz7(__VA_ARGS__))
#endif
    int smg$label_border(long * display_id, void * text, int * position_code,
                         int * units, int * rendition_set, int * rendition_complement,
                         int * character_set);

    int smg$list_key_defs(void);

    int smg$list_pasteboard_order(void);

    int smg$list_pasting_order(void);

    int smg$load_key_defs(void);

    int smg$load_virtual_display(void);

    int smg$move_virtual_display(void);

    int smg$move_text(void);

    int smg$name_to_keycode(void);

#if 0
// not yet
    int smg$paste_virtual_display(long * display_id, long * pasteboard_id, ...);
#define smg$paste_virtual_display(...) smg$paste_virtual_display(_buildargz5(__VA_ARGS__))
#endif
    int smg$paste_virtual_display(long * display_id, long * pasteboard_id,
                                  int * pasteboard_row, int * pasteboard_column, long * top_display_id);

#if 0
// not yet;
    int smg$put_chars(long * display_id, void * text, ...);
#define smg$put_chars(...) smg$put_chars(_buildargz8(__VA_ARGS__))
#endif
    int smg$put_chars(long * display_id, void * text, int * start_row,
                      int * start_column, int * flags, int * rendition_set,
                      int * rendition_complement, int * character_set);

    int smg$put_chars_highwide(void);

    int smg$put_chars_multi(void);

    int smg$put_chars_wide(void);

#if 0
// not yet
    int smg$put_help_text(long * display_id, ...);
#define smg$put_help_text(...) smg$put_help_text(_buildargz6(__VA_ARGS__))
#endif
    int smg$put_help_text(long * display_id, long * keyboard_id, void * help_topic,
                          int * help_library, int * rendition_set, int * rendition_complement);

#if 0
// not yet
    int smg$put_line(long * display_id, void * text, ...);
#define smg$put_line(...) smg$put_line(_buildargz7(__VA_ARGS__))
#endif
    int smg$put_line(long * display_id, void * text, int * line_advance,
                     int * rendition_set, int * rendition_complement, int * flags,
                     int * direction);

    int smg$put_line_highwide(void);

    int smg$put_line_multi(void);

    int smg$put_line_wide(void);

    int smg$pop_virtual_display(void);

    int smg$print_pasteboard(void);

#if 0
// not yet
    int smg$put_pasteboard(long * pasteboard_id, long action_routine, ...);
#define smg$put_pasteboard(...) smg$put_pasteboard(_buildargz4(__VA_ARGS__))
#endif
    int smg$put_pasteboard(long * pasteboard_id, long action_routine,
                           long user_argument, int * flags);

    int smg$put_status_line(void);

    int smg$put_virtual_display_encoded(void);

    int smg$put_with_scroll(void);

    int smg$read_composed_line(long * keyboard_id, long * key_table_id,
                               void * resultant_string, ...);
#define smg$read_composed_line(...) smg$read_composed_line(_buildargz12(__VA_ARGS__))

    int smg$read_from_display(void);

    int smg$read_keystroke(void);

    int smg$read_locator(void);

    int smg$read_string(long * keyboard_id, void * resultant_string, ...);
#define smg$read_string(...) smg$read_string(_buildargz14(__VA_ARGS__))

    int smg$read_verify(void);

    int smg$remove_line(void);

    int smg$repaint_line(void);

    int smg$repaint_screen(long * pasteboard_id);

    int smg$repaste_virtual_display(void);

    int smg$replace_input_line(void);

    int smg$restore_physical_screen(long * pasteboard_id, long * display_id);

    int smg$return_cursor_pos(void);

    int smg$return_input_line(void);

    int smg$ring_bell(long * display_id);

    int smg$save_virtual_display(void);

#if 0
// not yet
    int smg$save_physical_screen(long * pasteboard_id, long * display_id, ...);
#define smg$save_physical_screen(...) smg$save_physical_screen(_buildargz4(__VA_ARGS__))
#endif
    int smg$save_physical_screen(long * pasteboard_id, long * display_id,
                                 int * desired_start_row, int * desired_end_row);

    int smg$scroll_display_area(void);

    int smg$scroll_viewport(long * display_id, ...);
#define smg$scroll_viewport(...) smg$scroll_viewport(_buildargz3(__VA_ARGS__))

    int smg$select_from_menu(void);

#if 0
// not yet
    int smg$set_broadcast_trapping(long * pasteboard_id, ...);
#define smg$set_broadcast_trapping(...) smg$set_broadcast_trapping(_buildargz3(__VA_ARGS__))
#endif
    int smg$set_broadcast_trapping(long * pasteboard_id, long AST_routine,
                                   long AST_argument);

#if 0
// not yet
    int smg$set_cursor_abs(long * display_id, ...);
#define smg$set_cursor_abs(...) smg$set_cursor_abs(_buildargz3(__VA_ARGS__))
#endif
    int smg$set_cursor_abs(long * display_id, int * start_row, int * start_column);

    int smg$set_cursor_mode(long * pasteboard_id, int * flags);

#if 0
// not yet
    int smg$set_cursor_rel(long * display_id, ...);
#define smg$set_cursor_rel(...) smg$set_cursor_rel(_buildargz3(__VA_ARGS__))
#endif
    int smg$set_cursor_rel(long * display_id, int * delta_row, int * delta_column);

    int smg$set_default_state(void);

    int smg$set_display_scroll_region(void);

    int smg$set_keypad_mode(void);

    int smg$set_out_of_band_asts(void);

    int smg$set_physical_cursor(void);

#if 0
// not yet
    int smg$set_term_characteristics(long * pasteboard_id, ...);
#define smg$set_term_characteristics(...) smg$set_term_characteristics(_buildargz10(__VA_ARGS__))
#endif
    int smg$set_term_characteristics(long * pasteboard_id,
                                     int * on_characteristics1, int * on_characteristics2,
                                     int * off_characteristics1, int * off_characteristics2,
                                     int * old_characteristics1, int * old_characteristics2,
                                     int * on_characteristics3, int * off_characteristics3,
                                     int * old_characteristics3);

    int smg$snapshot(void);

    int smg$snapshot_to_printer(void);

    int smg$unpaste_virtual_display(long * display_id, long * pasteboard_id);

# ifdef __cplusplus
}
# endif
#endif
