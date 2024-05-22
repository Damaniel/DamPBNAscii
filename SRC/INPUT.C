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

void process_game_input(short key) {
    if (get_scan_code(key) == KEY_ESC) {
        change_state(STATE_EXIT);
    }
}

void process_title_input(short key) {

}

void process_input(short key) {
    switch(g_globals.current_state) {
        case STATE_GAME:
            process_game_input(key);
            break;
    }
}