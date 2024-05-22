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

void change_state(State new_state) {
    g_globals.previous_state = g_globals.current_state;
    g_globals.current_state = new_state;

    // process state transition stuff here
    switch(g_globals.current_state) {
        case STATE_EXIT:
            g_globals.exit_game = 1;
            break;
    }
}

void print_mem_stats() {
    printf("Available memory: %u bytes\n", _memavl());
    printf("Maximum allocatable chunk: %u bytes\n", _memmax());
}

int main(void) {
    int done = 0;
    unsigned short key;
    int i, j, c;

    set_text_mode(MODE_80X25);
    set_bg_intensity(1);
    clear_screen();

    clear_global_game_state(&g_globals);
    change_state(STATE_GAME);

    g_globals.current_picture = load_picture_file("test1.pic");

    draw_all();

    while (!g_globals.exit_game) {
        key = get_input_key();
        if (key) {
            // Process input for the current state
            process_input(key);
        }
        if (g_globals.update_screen) {
            update_screen();
            g_globals.update_screen = 0;
        }
    }

    clear_screen();
    free_picture_file(g_globals.current_picture);

    return 0;
}