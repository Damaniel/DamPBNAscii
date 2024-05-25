/* Copyright 2024 Shaun Brandt
   
   Permission is hereby granted, free of charge, to any person obtaining a 
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
   DEALINGS IN THE SOFTWARE.
 */
#include "includes.h"

// Returns the scan code of the key in the lower byte, and the shift state in the upper byte
unsigned short get_input_key (void) {
    unsigned short key_state, shift_state, output;
    if (!_bios_keybrd(_KEYBRD_READY)) {
        return 0;
    }
    key_state = _bios_keybrd(_KEYBRD_READ);
    shift_state = _bios_keybrd(_KEYBRD_SHIFTSTATUS);

    output = shift_state << 8;
    output |= (key_state >> 8);

    return output;
}

_inline unsigned char get_scan_code(short key) {
    return (key & 0xFF);
}

_inline unsigned char get_shift_state(short key) {
    return (key >> 8);
}

void process_title_input(short key) {
    switch (get_scan_code(key)) {
        case KEY_ESC:
            change_state(STATE_EXIT);
            break;
        case KEY_ENTER:
            change_state(STATE_LOAD_DIALOG);
            break;
    }
}

void process_load_dialog_input(short key) {
    char path[64];

    switch(get_scan_code(key)){
        case KEY_UP:
            if(g_globals.active_load_window == COLLECTION_TAB) {
                g_globals.old_selected_collection = g_globals.selected_collection;
                g_globals.old_top_collection = g_globals.top_collection;
                if (g_globals.selected_collection > 0) {
                    if (g_globals.top_collection != 0 ) {
                        --g_globals.top_collection;
                    }
                    --g_globals.selected_collection;
                }
                get_pictures(g_globals.selected_collection);
                g_globals.current_picture = 0;
                g_globals.render.load_collections_list = 1;
                g_globals.render.load_pictures_list = 1;
                g_globals.render.load_collection_cursor = 1;
                g_globals.render.load_picture_cursor = 1;
                g_globals.render.load_metadata_text = 1;
            }
            else {
                g_globals.old_selected_picture = g_globals.selected_picture;
                g_globals.old_top_picture = g_globals.top_picture;
                if (g_globals.selected_picture > 0) {
                    if (g_globals.top_picture != 0 ) {
                        --g_globals.top_picture;
                        g_globals.render.load_pictures_list = 1;
                    }
                    --g_globals.selected_picture;
                }
                g_globals.render.load_picture_cursor = 1;
                g_globals.render.load_metadata_text = 1;
            }
            break;
        case KEY_DOWN:
            if(g_globals.active_load_window == COLLECTION_TAB) {
                g_globals.old_selected_collection = g_globals.selected_collection;
                g_globals.old_top_collection = g_globals.top_collection;
                if (g_globals.selected_collection - g_globals.top_collection >= LOAD_SCREEN_MAX_LIST_ROWS - 1) {
                    if (g_globals.selected_collection < g_globals.num_collections - 1 ) {
                        ++g_globals.selected_collection;
                        ++g_globals.top_collection;
                        g_globals.render.load_collections_list = 1;
                    }
                }
                else {
                    if(g_globals.selected_collection < g_globals.num_collections - 1) {
                        ++g_globals.selected_collection;
                    }
                }
                get_pictures(g_globals.selected_collection);
                g_globals.current_picture = 0;
                g_globals.render.load_pictures_list = 1;
                g_globals.render.load_collection_cursor = 1;
                g_globals.render.load_picture_cursor = 1;
                g_globals.render.load_metadata_text = 1;
            }
            else {
                g_globals.old_selected_picture = g_globals.selected_picture;
                g_globals.old_top_picture = g_globals.top_picture;
                if (g_globals.selected_picture - g_globals.top_picture >= LOAD_SCREEN_MAX_LIST_ROWS - 1) {
                    if (g_globals.selected_picture < g_globals.num_pictures - 1 ) {
                        ++g_globals.selected_picture;
                        ++g_globals.top_picture;
                    }
                }
                else {
                    if(g_globals.selected_picture < g_globals.num_pictures - 1) {
                        ++g_globals.selected_picture;
                    }
                }
                g_globals.render.load_pictures_list = 1;
                g_globals.render.load_picture_cursor = 1;
                g_globals.render.load_metadata_text = 1;
            }
            break;
        case KEY_TAB:
            if (g_globals.active_load_window == COLLECTION_TAB) {
                g_globals.active_load_window = PICTURE_TAB;
            } 
            else {
                g_globals.active_load_window = COLLECTION_TAB;
            }
            g_globals.render.load_pictures = 1;
            g_globals.render.load_collections = 1;
            g_globals.render.load_collections_list = 1;
            g_globals.render.load_pictures_list = 1;
            g_globals.render.load_picture_cursor = 1;
            g_globals.render.load_collection_cursor = 1;
            break;
        case KEY_R:
            sprintf(path, "%s/%s/%s.pro",  PROGRESS_FILE_DIR, g_collections[g_globals.selected_collection].name, g_pictures[g_globals.selected_picture].name);
            if (access(path, F_OK) == 0) {
                unlink(path);
                g_pictures[g_globals.selected_picture].progress = 0;
                g_globals.render.load_metadata_text = 1;
            }
            break;
        case KEY_ESC:
            change_state(STATE_TITLE);
            break;
        case KEY_ENTER:
            change_state(STATE_GAME);
            break;
    }
}

void process_map_navigation(unsigned char direction) {
    switch (direction) {
        case KEY_UP:
            if(g_globals.current_picture->h <= g_globals.text_lines) {
                g_globals.map_y = 0;
            }
            else {
                g_globals.map_y -= g_globals.text_lines;
                if(g_globals.map_y < 0) {
                    g_globals.map_y = 0;
                }
            }
            g_globals.render.map = 1;
            break;
        case KEY_DOWN:
            if(g_globals.current_picture->h <= g_globals.text_lines) {
                g_globals.map_y = 0;
            } 
            else {
                g_globals.map_y += g_globals.text_lines;
                if(g_globals.map_y + g_globals.text_lines >= g_globals.current_picture->h) {
                    g_globals.map_y = (g_globals.current_picture->h - g_globals.text_lines);
                }
            }
            g_globals.render.map = 1;            
            break;
        case KEY_LEFT:
            if(g_globals.current_picture->w <= 80) {
                g_globals.map_x = 0;
            } 
            else {
                g_globals.map_x -= 80;
                if(g_globals.map_x < 0) {
                    g_globals.map_x = 0;
                }
            }    
            g_globals.render.map = 1;
            break;
        case KEY_RIGHT:
            if (g_globals.current_picture->w <= 80) {
                g_globals.map_x = 0;
            }
            else {
                g_globals.map_x += 80;
                if(g_globals.map_x + 80 >= g_globals.current_picture->w) {
                    g_globals.map_x = (g_globals.current_picture->w - 80);
                }
            }
            g_globals.render.map = 1;
            break;
    }
}
void process_map_input(short key) {
    switch(get_scan_code(key)) {
        case KEY_ESC:
        case KEY_ENTER:
        case KEY_M:
            if (g_globals.use_high_res_text_mode) {
                set_text_mode(MODE_80X25);
                hide_cursor();
            }
            change_state(STATE_GAME);
            break;
        case KEY_H:
            g_globals.map_hide_legend = ~(g_globals.map_hide_legend);
            g_globals.render.map = 1;
            break;
        case KEY_UP:
        case KEY_DOWN:
        case KEY_LEFT:
        case KEY_RIGHT:
            process_map_navigation(get_scan_code(key));
            break;
    }
}

void process_game_input(short key) {
    int proposed_cursor, proposed_viewport, offset, scroll_page = 0;
    ColorSquare *cs;

    switch (get_scan_code(key)) {
        case KEY_ESC:
        case KEY_E:
            if (!g_globals.saving_in_progress) {
                save_progress_file(g_globals.current_picture);
            }
            change_state(STATE_TITLE);
            break;
        case KEY_S:
            if(!g_globals.saving_in_progress) {
                g_globals.saving_in_progress = 1;
                g_globals.save_message_start_ticks = g_clock_ticks;
                g_globals.render.save_message = 1;
                save_progress_file(g_globals.current_picture);
            }
            break;
        case KEY_L:
            if(!g_globals.loading_in_progress) {
                load_progress_file(g_globals.current_picture);
                g_globals.render.puzzle = 1;
                g_globals.render.cursor_pos_area = 1;
                g_globals.render.puzzle_cursor = 1;
                g_globals.render.timer_area = 1;
                g_globals.render.mistake_area = 1;
                g_globals.render.progress_area = 1;
                g_globals.render.information_area = 1;
                if(g_globals.mark_enabled) {
                    g_globals.render.marks = 1;
                }
                g_globals.loading_in_progress = 1;
                g_globals.load_message_start_ticks = g_clock_ticks;
                g_globals.render.load_message = 1;
            }
            break;
        case KEY_M:
            change_state(STATE_MAP);
            break;
        case KEY_K:
            g_globals.mark_enabled = ~(g_globals.mark_enabled);
            g_globals.render.marks = 1;
            g_globals.render.puzzle_cursor = 1;
            g_globals.render.button_area = 1;
            break;
        case KEY_LEFT_BRACKET:
            g_globals.old_palette_index = g_globals.palette_index;
            g_globals.palette_index -= 1;
            if (g_globals.palette_index < 0) {
                g_globals.palette_index = g_globals.current_picture->num_colors - 1;
            }
            g_globals.render.palette_cursor = 1;
            if (g_globals.mark_enabled) {
                g_globals.render.puzzle = 1;
                g_globals.render.puzzle_cursor = 1;
            }
            break;
        case KEY_RIGHT_BRACKET:
            g_globals.old_palette_index = g_globals.palette_index;
            g_globals.palette_index += 1;
            if (g_globals.palette_index >= g_globals.current_picture->num_colors) {
                g_globals.palette_index = 0;
            }
            g_globals.render.palette_cursor = 1;
            if (g_globals.mark_enabled) {
                g_globals.render.puzzle = 1;
                g_globals.render.puzzle_cursor = 1;
            }
            break;
        case KEY_LEFT:
            if (get_shift_state(key) & STATE_RIGHT_SHIFT || get_shift_state(key) & STATE_LEFT_SHIFT) {
                proposed_cursor = g_globals.cursor_x;
                proposed_viewport = g_globals.viewport_x - DRAW_VISIBLE_WIDTH;
                scroll_page = 1;
            } else {
                proposed_cursor = g_globals.cursor_x - 1;
                proposed_viewport = g_globals.viewport_x;
            }
            if (proposed_cursor < 0) {
                proposed_viewport = g_globals.viewport_x - 1;
                proposed_cursor = 0;
            }
            if (proposed_viewport < 0) {
                proposed_viewport = 0;
                // If we were already on the left edge then move the cursor all the way over
                // if (g_globals.viewport_x == proposed_viewport && scroll_page) {
                //     proposed_cursor = 0;
                // }
            }
            g_globals.old_cursor_y = g_globals.cursor_y;
            g_globals.old_cursor_x = g_globals.cursor_x;
            g_globals.cursor_x = proposed_cursor;
            if (g_globals.viewport_x != proposed_viewport) {
                g_globals.x_viewport_changed = 1;
            }
            else {
                g_globals.x_viewport_changed = 0;
            }
                g_globals.old_viewport_x = g_globals.viewport_x;
            g_globals.viewport_x = proposed_viewport;
            g_globals.render.cursor_pos_area = 1;
            g_globals.render.puzzle_cursor = 1;
            // If the viewport moved, we have to redraw the puzzle
            if (proposed_viewport != g_globals.old_viewport_x) {
                g_globals.render.puzzle = 1;
            }
            break;        
        case KEY_RIGHT:
            if (get_shift_state(key) & STATE_RIGHT_SHIFT || get_shift_state(key) & STATE_LEFT_SHIFT) {
                proposed_cursor = g_globals.cursor_x;
                proposed_viewport = g_globals.viewport_x + DRAW_VISIBLE_WIDTH;

            } else {
                proposed_cursor = g_globals.cursor_x + 1;
                proposed_viewport = g_globals.viewport_x;
            }
            if (proposed_cursor >= DRAW_VISIBLE_WIDTH) {
                proposed_viewport = g_globals.viewport_x + 1;
                proposed_cursor = DRAW_VISIBLE_WIDTH - 1;
            }
            if (proposed_viewport >= g_globals.current_picture->w - DRAW_VISIBLE_WIDTH) {
                proposed_viewport = g_globals.current_picture->w - DRAW_VISIBLE_WIDTH;
                // // If we were already on the right edge then move the cursor all the way over
                // if (g_globals.viewport_x == proposed_viewport && scroll_page) {
                //     proposed_cursor = DRAW_VISIBLE_WIDTH - 1;
                // }
            }
            g_globals.old_cursor_y = g_globals.cursor_y;
            g_globals.old_cursor_x = g_globals.cursor_x;
            g_globals.cursor_x = proposed_cursor;
            // If the viewport changed, set the old_cursor to the new_cursor
            if (g_globals.viewport_x != proposed_viewport) {
                g_globals.x_viewport_changed = 1;
            }
            else {
                g_globals.x_viewport_changed = 0;
            }
            g_globals.old_viewport_x = g_globals.viewport_x;
            g_globals.viewport_x = proposed_viewport;
            g_globals.render.cursor_pos_area = 1;
            g_globals.render.puzzle_cursor = 1;
            // If the viewport moved, we have to redraw the puzzle
            if (proposed_viewport != g_globals.old_viewport_x) {
                g_globals.render.puzzle = 1;
            }
            break;
        case KEY_UP:
            if (get_shift_state(key) & STATE_RIGHT_SHIFT || get_shift_state(key) & STATE_LEFT_SHIFT) {
                proposed_cursor = g_globals.cursor_y;
                proposed_viewport = g_globals.viewport_y - DRAW_VISIBLE_HEIGHT;
                scroll_page = 1;
            } else {
                proposed_cursor = g_globals.cursor_y - 1;
                proposed_viewport = g_globals.viewport_y;
            }
            if (proposed_cursor < 0) {
                proposed_viewport = g_globals.viewport_y - 1;
                proposed_cursor = 0;
            }
            if (proposed_viewport < 0) {
                proposed_viewport = 0;
                // If we were already on the top edge then move the cursor all the way over
                // if (g_globals.viewport_y == proposed_viewport && scroll_page) {
                //     proposed_cursor = 0;
                // }
            }
            g_globals.old_cursor_x = g_globals.cursor_x;
            g_globals.old_cursor_y = g_globals.cursor_y;
            g_globals.cursor_y = proposed_cursor;
            if (g_globals.viewport_y != proposed_viewport) {
                g_globals.y_viewport_changed = 1;
            }
            else {
                g_globals.y_viewport_changed = 0;
            }
            g_globals.old_viewport_y = g_globals.viewport_y;
            g_globals.viewport_y = proposed_viewport;
            g_globals.render.cursor_pos_area = 1;
            g_globals.render.puzzle_cursor = 1;
            // If the viewport moved, we have to redraw the puzzle
            if (proposed_viewport != g_globals.old_viewport_y) {
                g_globals.render.puzzle = 1;
            }
            break;  
        case KEY_DOWN:
            if (get_shift_state(key) & STATE_RIGHT_SHIFT || get_shift_state(key) & STATE_LEFT_SHIFT) {
                proposed_cursor = g_globals.cursor_y;
                proposed_viewport = g_globals.viewport_y + DRAW_VISIBLE_HEIGHT;
            } else {
                proposed_cursor = g_globals.cursor_y + 1;
                proposed_viewport = g_globals.viewport_y;
            }
            if (proposed_cursor >= DRAW_VISIBLE_HEIGHT) {
                proposed_viewport = g_globals.viewport_y + 1;
                proposed_cursor = DRAW_VISIBLE_HEIGHT - 1;
            }
            if (proposed_viewport >= g_globals.current_picture->h - DRAW_VISIBLE_HEIGHT) {
                proposed_viewport = g_globals.current_picture->h - DRAW_VISIBLE_HEIGHT;
                // If we were already on the bottom edge then move the cursor all the way over
                // if (g_globals.viewport_y == proposed_viewport && scroll_page ) {
                //     proposed_cursor = DRAW_VISIBLE_HEIGHT - 1;
                // }
            }
            g_globals.old_cursor_x = g_globals.cursor_x;
            g_globals.old_cursor_y = g_globals.cursor_y;
            g_globals.cursor_y = proposed_cursor;
            if (g_globals.viewport_y != proposed_viewport) {
                g_globals.y_viewport_changed = 1;
            }
            else {
                g_globals.y_viewport_changed = 0;
            }
            g_globals.old_viewport_y = g_globals.viewport_y;
            g_globals.viewport_y = proposed_viewport;
            g_globals.render.cursor_pos_area = 1;
            g_globals.render.puzzle_cursor = 1;
            // If the viewport moved, we have to redraw the puzzle
            if (proposed_viewport != g_globals.old_viewport_y) {
                g_globals.render.puzzle = 1;
            }
            //dump_picture_file();
            break;
        case KEY_SPACE:
            // If already correct, do nothing
            cs = get_color_square(g_globals.current_picture, g_globals.cursor_x + g_globals.viewport_x, g_globals.cursor_y + g_globals.viewport_y);
            if (!is_transparent(cs)) {
                // If not filled, set the color to the current palette color
                if (!is_filled_in(cs)) {
                    set_filled_flag(cs, 1);
                    if (cs->pal_entry == g_globals.palette_index) {
                        set_correct_flag(cs, 1);
                        ++g_globals.progress;
                        g_globals.render.progress_area = 1;
                        if (g_globals.progress >= g_globals.total_picture_squares) {
                            change_state(STATE_FINISHED);
                        }
                    } 
                    else {
                        set_correct_flag(cs, 0);
                        ++g_globals.mistakes;
                        g_globals.render.mistake_area = 1;
                    }
                }
                else {
                    // If filled and incorrect, clear the color
                    if(!is_correct(cs)) {
                        set_filled_flag(cs, 0);
                        --g_globals.mistakes;
                        g_globals.render.mistake_area = 1;
                    }
                }
                g_globals.render.drawn_square = 1;
                g_globals.render.puzzle_cursor = 1;
            }
            break;
            
    }
}

// y = before - 0 (0-24), after - 0 + 25
// y = before - 25 (25-31), after 7, +25
void process_finished_input(short key) {
    switch(get_scan_code(key)) {
        case KEY_ENTER:
        case KEY_ESC:
            if (g_globals.use_high_res_text_mode) {
                set_text_mode(MODE_80X25);
                hide_cursor();
            }
            change_state(STATE_TITLE);
            break;
        case KEY_UP:
        case KEY_DOWN:
        case KEY_LEFT:
        case KEY_RIGHT:
            process_map_navigation(get_scan_code(key));
            break;
        case KEY_H:
            g_globals.map_hide_legend = ~(g_globals.map_hide_legend);
            g_globals.render.map = 1;
            break;
    }
}

void process_input(short key) {
    switch(g_globals.current_state) {
        case STATE_TITLE:
            process_title_input(key);
            break;
        case STATE_LOAD_DIALOG:
            process_load_dialog_input(key);
            break;
        case STATE_GAME:
            process_game_input(key);
            break;
        case STATE_MAP:
            process_map_input(key);
            break;
        case STATE_FINISHED:
            process_finished_input(key);
            break;
    }
}