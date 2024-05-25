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

char palette_chars[PALETTE_ENTRIES + 1] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ$#";
GameGlobals g_globals;

void (__interrupt __far *old_isr)();
volatile long g_clock_ticks;
char g_timer_overflow;

void __interrupt __far timer_func(void) {
    g_clock_ticks++;
}

void set_high_res_mode(void) {
    g_globals.use_high_res_text_mode = 1;
    if (g_globals.use_high_res_text_mode && g_globals.text_mode != CARD_CGA) {
        set_text_mode(MODE_80X50);
        hide_cursor();
        if (g_globals.text_mode == CARD_EGA) {
            g_globals.text_lines = 43;
        } else {
            g_globals.text_lines = 50;
        }
    }
    else {
        g_globals.text_lines = 25;
    }
}
void change_state(State new_state) {
    char filename[80];

    g_globals.previous_state = g_globals.current_state;
    g_globals.current_state = new_state;

    // process state transition stuff here
    switch(g_globals.current_state) {
        case STATE_TITLE:
            g_globals.render.title = 1;
            break;
        case STATE_LOAD_DIALOG:
            g_globals.selected_collection = 0;
            g_globals.selected_picture = 0;
            g_globals.old_selected_collection = 0;
            g_globals.old_selected_picture = 0;
            g_globals.top_collection = 0;
            g_globals.top_picture = 0;
            g_globals.old_top_collection = 0;
            g_globals.old_top_picture = 0;
            get_collections();
            if (g_globals.num_collections > 0) {
                get_pictures(0);
            }
            g_globals.active_load_window = COLLECTION_TAB;
            g_globals.selected_collection = 0;
            g_globals.selected_picture = 0;
            g_globals.render.load_background = 1;
            g_globals.render.load_collections = 1;
            g_globals.render.load_pictures = 1;
            g_globals.render.load_collections_list = 1;
            g_globals.render.load_pictures_list = 1;
            g_globals.render.load_collection_cursor = 1;
            g_globals.render.load_metadata_text = 1;
            g_globals.render.load_picture_cursor = 1;
            break;
        case STATE_GAME:
            if (g_globals.previous_state != STATE_MAP) {
                strncpy(g_globals.collection_name,  g_collections[g_globals.selected_collection].name, 8);
                strncpy(g_globals.picture_file_basename, g_pictures[g_globals.selected_picture].name, 8);   
                sprintf(filename, "RES/PICS/%s/%s.PIC", g_globals.collection_name, g_globals.picture_file_basename);
                g_globals.current_picture = load_picture_file(filename);
                load_progress_file(g_globals.current_picture);
                clear_render_components(&(g_globals.render));
                reset_image_state(&(g_globals));
                if (g_pictures[g_globals.selected_picture].progress == g_pictures[g_globals.selected_picture].total) {
                    change_state(STATE_FINISHED);
                }
                else {
                    draw_all();
                    draw_puzzle_cursor();
                }
                start_game_timer();
            }
            else {
                draw_all();
                draw_puzzle_cursor();
                start_game_timer();
            }
            break;
        case STATE_MAP:
            pause_game_timer();
            set_high_res_mode();
            g_globals.render.map = 1;
            break;
        case STATE_FINISHED:
            if (!g_globals.saving_in_progress) {
                save_progress_file(g_globals.current_picture);
            }
            set_high_res_mode();
            g_globals.render.map = 1;
            break;
        case STATE_EXIT:
            g_globals.exit_game = 1;
            break;
    }
}

void start_game_timer(void) {
    g_globals.timer_running = 1;
    g_globals.start_ticks = g_clock_ticks;
}

void pause_game_timer(void) {
    g_globals.timer_running = 0;
}

void process_timing(void) {
    long elapsed_ticks = g_clock_ticks - g_globals.start_ticks;
    long elapsed_save_ticks = g_clock_ticks - g_globals.save_message_start_ticks;
    long elapsed_load_ticks = g_clock_ticks - g_globals.load_message_start_ticks;
    
    // The timer ticks at 18.2 ticks per second.  Every 5 seconds,
    // we wait 19 ticks instead of 18, to keep the underlying timer
    // as reasonably accurate as possible.
    if(g_globals.timer_running) {
        if (elapsed_ticks >= 19 && g_timer_overflow >= 5) {
            g_globals.elapsed_seconds++;
            g_timer_overflow = 0;
            g_globals.start_ticks = g_clock_ticks;
            g_globals.render.timer_area = 1;
        }
        else if (elapsed_ticks >= 18) {
            g_globals.elapsed_seconds++;
            g_timer_overflow++;
            g_globals.start_ticks = g_clock_ticks;
            g_globals.render.timer_area = 1;
        }
    }
    if(g_globals.saving_in_progress) {
        if (elapsed_save_ticks >= SAVE_MESSAGE_DURATION) {
            g_globals.save_message_start_ticks = 0;
            g_globals.saving_in_progress = 0;
            g_globals.render.save_message = 1;
        }
    }
    if (g_globals.loading_in_progress) {
        if (elapsed_load_ticks >= LOAD_MESSAGE_DURATION) {
            g_globals.load_message_start_ticks = 0;
            g_globals.loading_in_progress = 0;
            g_globals.render.load_message = 1;
        }
    }
}

void print_mem_stats() {
    printf("Available memory: %u bytes\n", _memavl());
    printf("Maximum allocatable chunk: %u bytes\n", _memmax());
}

// Check for and create progress directory tree if needed
// Mainly needed because git won't commit empty directories, and the player shouldn't have to 
// manually do this.
void check_for_progress_directories() {
    int result;
    struct find_t fileinfo;
    char progress_path[40];

    // If progress directory doesn't exist, create it
    if ( access(PROGRESS_FILE_DIR, F_OK) != 0) {
        printf("Progress base directory doesn't exist, creating...\n");
        mkdir(PROGRESS_FILE_DIR);
    }
    // For each directory in the collections folder
    // If a directory by that name doesn't exist in the progress directory, create it
    result = _dos_findfirst(COLLECTION_PATHSPEC, _A_SUBDIR, &fileinfo);
    while (result == 0) {
        if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
            sprintf(progress_path, "%s/%s", PROGRESS_FILE_DIR, fileinfo.name);
            if(access(progress_path, F_OK) != 0) {
                printf("Progress directory for collection %s doesn't exist, creating...\n", fileinfo.name);
                mkdir(progress_path);
            }
        }
        result = _dos_findnext(&fileinfo);
    }
}

void game_init() {  
    // Reset the tick counter
    g_clock_ticks = 0;
    // Prep our pre-written timer interrupt function
    old_isr = _dos_getvect(0x1C);
    _dos_setvect(0x1C, timer_func);

    // Set up the screen
    set_text_mode(MODE_80X25);
    set_bg_intensity(1);
    clear_screen();
    hide_cursor();

    // Check for progress directories and create if needed
    check_for_progress_directories();

    clear_global_game_state(&g_globals);

    g_globals.text_mode = detect_adapter();

    change_state(STATE_TITLE);
}

void game_cleanup(void) {
    // Reset the display
    clear_screen();
    show_cursor();

    // todo - remove this once we get the puzzle loader finished
    free_picture_file(g_globals.current_picture);

    // restore the timer handler
    _dos_setvect(0x1C, old_isr);
}

int main(void) {
    unsigned short key;

    game_init();

    while (!g_globals.exit_game) {
        key = get_input_key();
        if (key) {
            // Process input for the current state
            process_input(key);
        }
        process_timing();
        render_screen();
        // Add a very short delay (2ms).  This shouldn't affect timing, even on slower systems.
        // This can be removed if it causes issues.
        delay(2);
    }

    game_cleanup();

    return 0;
}