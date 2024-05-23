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
#pragma once

extern void draw_all(void);

extern void draw_ui_base(void);
extern void draw_puzzle_area(void);
extern void draw_puzzle_box_area(void);
extern void draw_legend_area(void);
extern void draw_legend(void);
extern void draw_information_area(void);
extern void draw_button_area(void);
extern void draw_cursor_pos_text(void);
extern void draw_puzzle_cursor(void);
extern void draw_palette_cursor(void);
extern void draw_timer(void);
extern void draw_progress(void);
extern void draw_mistakes(void);
extern void draw_marks(void);

extern void render_screen(void);
extern void render_game_state(void);
extern void clear_render_components(RenderComponents *r);
