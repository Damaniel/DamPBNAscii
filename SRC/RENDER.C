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

// Draw the bars, the title, the background, etc

void draw_all(void) {
    g_globals.render.background = 1;
    g_globals.render.puzzle_box_area = 1;
    g_globals.render.legend_area = 1;
    g_globals.render.button_area = 1;
    g_globals.render.puzzle = 1;
    g_globals.render.palette = 1;
    g_globals.render.puzzle_cursor = 1;
    g_globals.render.palette_cursor = 1;
    g_globals.render.cursor_pos_area = 1;
    g_globals.render.timer_area = 1;
    g_globals.render.mistake_area = 1;
    g_globals.render.progress_area = 1;
    g_globals.render.information_area = 1;
    render_screen();
}

void render_screen(void) {
    switch(g_globals.current_state) {
        case STATE_GAME:
            render_game_state();
            break;
    }
}

void draw_drawn_square(int cursor_x, int cursor_y, int viewport_x, int viewport_y) {
    char standard = make_attr(COLOR_WHITE, COLOR_BLACK);
    char dimmer = make_attr(COLOR_DARK_GRAY, COLOR_BLACK);
    char wrong = make_attr(COLOR_WHITE, COLOR_RED);
    char cur_char, attr;
    ColorSquare *cs;

    cs = get_color_square(g_globals.current_picture, cursor_x + viewport_x, cursor_y + viewport_y);

    if (!is_transparent(cs)) {
        if(is_filled_in(cs)) {
            if(is_correct(cs)) {
                attr = make_attr(g_globals.current_picture->pal[cs->pal_entry][1], g_globals.current_picture->pal[cs->pal_entry][2]);
                char_at(DRAW_AREA_X1 + (cursor_x * 2), DRAW_AREA_Y1 + cursor_y, g_globals.current_picture->pal[cs->pal_entry][0], attr);
                char_at(DRAW_AREA_X1 + (cursor_x * 2) + 1, DRAW_AREA_Y1 + cursor_y, g_globals.current_picture->pal[cs->pal_entry][0], attr);   
            }
            else {
                char_at(DRAW_AREA_X1 + (cursor_x * 2), DRAW_AREA_Y1 + cursor_y, 'X', wrong);
                char_at(DRAW_AREA_X1 + (cursor_x * 2) + 1, DRAW_AREA_Y1 + cursor_y, 'X', wrong);                        
            }
        }
        else {
            cur_char = get_picture_color_at(g_globals.current_picture, cursor_x  + viewport_x, cursor_y + viewport_y);
            char_at(DRAW_AREA_X1 + (cursor_x * 2) + 1, DRAW_AREA_Y1 + cursor_y, palette_chars[cur_char], dimmer);
        }
    }
    else {
        char_at(DRAW_AREA_X1 + (cursor_x * 2), DRAW_AREA_Y1 + cursor_y, '.', dimmer);
        char_at(DRAW_AREA_X1 + (cursor_x * 2) + 1, DRAW_AREA_Y1 + cursor_y, '.', dimmer);                
    }
}

void draw_ui_base(void) {
    char title_bar = make_attr(COLOR_WHITE, COLOR_CYAN);
    char background = make_attr(COLOR_BLUE, COLOR_LIGHT_BLUE);
    
    hline_at(0, 0, 80, ' ', title_bar);
    fill_box_at(0, 1, 79, 24, 177, background);
    string_at(2, 0, "DamPBN (in ASCII!)", title_bar);
    string_at(61, 0, SW_VERSION, title_bar);
}

void draw_puzzle_box_area(void) {
    char standard = make_attr(COLOR_WHITE, COLOR_BLACK);

    // Draw the boxes
    box_at(DRAW_AREA_BOX_X1, DRAW_AREA_BOX_Y1, DRAW_AREA_BOX_X2, DRAW_AREA_BOX_Y2, BORDER_SINGLE, standard);
    box_at(STATUS_AREA_BOX_X1, STATUS_AREA_BOX_Y1, STATUS_AREA_BOX_X2, STATUS_AREA_BOX_Y2, BORDER_SINGLE, standard);
    // fix the characters
    char_at(STATUS_AREA_BOX_X1, STATUS_AREA_BOX_Y1, 195, standard);
    char_at(STATUS_AREA_BOX_X2, STATUS_AREA_BOX_Y1, 180, standard);
}

void draw_puzzle_area(void) {
    char standard = make_attr(COLOR_WHITE, COLOR_BLACK);
    char dimmer = make_attr(COLOR_DARK_GRAY, COLOR_BLACK);
    char wrong = make_attr(COLOR_WHITE, COLOR_RED);
    int i, j;
    char cur_char, attr;
    ColorSquare *cs;

    fill_box_at(DRAW_AREA_X1, DRAW_AREA_Y1, DRAW_AREA_X2, DRAW_AREA_Y2, ' ', standard);
    for (j=0; j < DRAW_VISIBLE_HEIGHT; j++) {
        for (i=0; i < DRAW_VISIBLE_WIDTH; i++) {
            draw_drawn_square(i, j, g_globals.viewport_x, g_globals.viewport_y);
            // cs = get_color_square(g_globals.current_picture, i + g_globals.viewport_x, j + g_globals.viewport_y);
            // if (!is_transparent(cs)) {
            //     if(is_filled_in(cs)) {
            //         if(is_correct(cs)) {
            //             attr = make_attr(g_globals.current_picture->pal[cs->pal_entry][1], g_globals.current_picture->pal[cs->pal_entry][2]);
            //             char_at(DRAW_AREA_X1 + (i * 2), DRAW_AREA_Y1 + j, g_globals.current_picture->pal[cs->pal_entry][0], attr);
            //             char_at(DRAW_AREA_X1 + (i * 2) + 1, DRAW_AREA_Y1 + j, g_globals.current_picture->pal[cs->pal_entry][0], attr);   
            //         }
            //         else {
            //             char_at(DRAW_AREA_X1 + (i * 2), DRAW_AREA_Y1 + j, 'X', wrong);
            //             char_at(DRAW_AREA_X1 + (i * 2) + 1, DRAW_AREA_Y1 + j, 'X', wrong);                        
            //         }
            //     }
            //     else {
            //         cur_char = get_picture_color_at(g_globals.current_picture, i + g_globals.viewport_x, j + g_globals.viewport_y);
            //         char_at(DRAW_AREA_X1 + (i * 2) + 1, DRAW_AREA_Y1 + j, palette_chars[cur_char], dimmer);
            //     }
            // }
            // else {
            //         char_at(DRAW_AREA_X1 + (i * 2), DRAW_AREA_Y1 + j, '.', dimmer);
            //         char_at(DRAW_AREA_X1 + (i * 2) + 1, DRAW_AREA_Y1 + j, '.', dimmer);                
            // }
        }
    }
}

void draw_puzzle_cursor() {
    char yellow = make_attr(COLOR_YELLOW, COLOR_BLACK);
    char dimmer = make_attr(COLOR_DARK_GRAY, COLOR_BLACK);
    char wrong = make_attr(COLOR_WHITE, COLOR_RED);
    char invert_wrong = make_attr(COLOR_WHITE, COLOR_RED);

    char color;
    ColorSquare *old_cs, *cs;

    old_cs = get_color_square(g_globals.current_picture, g_globals.old_cursor_x + g_globals.old_viewport_x, g_globals.old_cursor_y + g_globals.old_viewport_y);
    cs = get_color_square(g_globals.current_picture, g_globals.cursor_x + g_globals.viewport_x, g_globals.cursor_y + g_globals.viewport_y);
    color = old_cs->pal_entry;

    // Redraw the underlying color at the old cursor location
    // TODO: do the right thing for filled in squares
    if (is_filled_in(old_cs)) {
        // Draw the filled in color if correct, the incorrect square if not correct
        if (is_correct(old_cs)) {
            char_at(DRAW_AREA_X1 + (g_globals.old_cursor_x * 2), DRAW_AREA_Y1 + g_globals.old_cursor_y, g_globals.current_picture->pal[color][0], 
                    make_attr(g_globals.current_picture->pal[color][1], g_globals.current_picture->pal[color][2]));
        }
        else {
            char_at(DRAW_AREA_X1 + (g_globals.old_cursor_x * 2), DRAW_AREA_Y1 + g_globals.old_cursor_y, 'X', wrong);
        }
    }
    else {
        if (is_transparent(old_cs)) {
            char_at(DRAW_AREA_X1 + (g_globals.old_cursor_x * 2), DRAW_AREA_Y1 + g_globals.old_cursor_y, '.', dimmer);
        }
        else {
            char_at(DRAW_AREA_X1 + (g_globals.old_cursor_x * 2), DRAW_AREA_Y1 + g_globals.old_cursor_y, ' ', dimmer);   
        }
    }

    // Draw the cursor at the new location (held in cursor_x and cursor_y)
    if(is_filled_in(cs)) {
        if (is_correct(cs)) {
            char_at(DRAW_AREA_X1 + (g_globals.cursor_x * 2), DRAW_AREA_Y1 + (g_globals.cursor_y), 16, yellow);
        }
        else {
            char_at(DRAW_AREA_X1 + (g_globals.cursor_x * 2), DRAW_AREA_Y1 + (g_globals.cursor_y), 16, invert_wrong);
        }
    }
    else {
        char_at(DRAW_AREA_X1 + (g_globals.cursor_x * 2), DRAW_AREA_Y1 + (g_globals.cursor_y), 16, yellow);
    }   
}

void draw_palette_cursor() {
    unsigned int p_x, p_y, op_x, op_y;
    char yellow = make_attr(COLOR_YELLOW, COLOR_BLACK);
    char standard = make_attr(COLOR_WHITE, COLOR_BLACK);

    p_x = (g_globals.palette_index  / LEGEND_NUM_ROWS) * LEGEND_COL_WIDTH + LEGEND_COLS_X - 1;
    p_y = g_globals.palette_index - ((g_globals.palette_index / LEGEND_NUM_ROWS) * LEGEND_NUM_ROWS) + LEGEND_COLS_Y;
    op_x = (g_globals.old_palette_index / LEGEND_NUM_ROWS) * LEGEND_COL_WIDTH + LEGEND_COLS_X - 1;
    op_y = g_globals.old_palette_index - ((g_globals.old_palette_index / LEGEND_NUM_ROWS) * LEGEND_NUM_ROWS) + LEGEND_COLS_Y;

    char_at(op_x, op_y, ' ', standard);
    char_at(p_x, p_y, 16, yellow);

}

void draw_legend_area(void) {
    char standard = make_attr(COLOR_WHITE, COLOR_BLACK);
    box_at(LEGEND_AREA_BOX_X1, LEGEND_AREA_BOX_Y1, LEGEND_AREA_BOX_X2, LEGEND_AREA_BOX_Y2, BORDER_SINGLE, standard);
    box_at(BUTTON_AREA_BOX_X1, BUTTON_AREA_BOX_Y1, BUTTON_AREA_BOX_X2, BUTTON_AREA_BOX_Y2, BORDER_SINGLE, standard);
    // fix the characters
    char_at(BUTTON_AREA_BOX_X1, BUTTON_AREA_BOX_Y1, 195, standard);
    char_at(BUTTON_AREA_BOX_X2, BUTTON_AREA_BOX_Y1, 180, standard);
    // Draw the legend text
    string_at(LEGEND_TEXT_X, LEGEND_TEXT_Y, " Legend ", standard);
}

void draw_legend(void) {
    char standard = make_attr(COLOR_WHITE, COLOR_BLACK);
    int i, j, idx, pal_idx;
    
    fill_box_at(LEGEND_AREA_X1, LEGEND_AREA_Y1, LEGEND_AREA_X2, LEGEND_AREA_Y2, ' ', standard);

    // For now, draw the entire legend.  In the actual game, we'll only draw the characters
    // that are actually part of the palette
    for (i=0; i < LEGEND_NUM_COLS; i++) {
        for (j=0; j < LEGEND_NUM_ROWS; j++) {
            idx = LEGEND_COLS_X + (LEGEND_COL_WIDTH * i);
            pal_idx = i * LEGEND_NUM_ROWS + j;
            if (pal_idx < g_globals.current_picture->num_colors) {
                char_at(idx, LEGEND_COLS_Y + j, palette_chars[i * LEGEND_NUM_ROWS + j], standard);
                char_at(idx + 2, LEGEND_COLS_Y + j, '-', standard);
                char_at(idx + 4, LEGEND_COLS_Y + j, g_globals.current_picture->pal[pal_idx][0], make_attr(g_globals.current_picture->pal[pal_idx][1], g_globals.current_picture->pal[pal_idx][2]));
            }
        }
    }
}

void draw_information_text(void) {
    char standard = make_attr(COLOR_WHITE, COLOR_BLACK);
    char header = make_attr(COLOR_CYAN, COLOR_BLACK);
    fill_box_at(STATUS_AREA_X1, STATUS_AREA_Y1, STATUS_AREA_X2, STATUS_AREA_Y2, ' ', standard);

    // fill in the headers
    string_at(CATEGORY_TEXT_X, CATEGORY_TEXT_Y, "Category:", header);
    string_at(PROGRESS_TEXT_X, PROGRESS_TEXT_Y, "Progress:", header);
    string_at(MISTAKES_TEXT_X, MISTAKES_TEXT_Y, "Mistakes:", header);
    string_at(TIME_TEXT_X, TIME_TEXT_Y, "Time:", header);
}

void draw_timer(void) {
    char standard = make_attr(COLOR_WHITE, COLOR_BLACK);
    char time_string[10];
    int hours, minutes, seconds;

    hours = g_globals.elapsed_seconds / 3600;
    minutes = (g_globals.elapsed_seconds - (hours * 3600)) / 60;
    seconds = (g_globals.elapsed_seconds - (hours * 3600)) % 60;
    if (hours > 99) {
        sprintf(time_string, "99:59:59");
    } 
    else {
        sprintf(time_string, "%.2d:%.2d:%.2d", hours, minutes, seconds);
    }
    string_at(TIME_VALUE_X, TIME_VALUE_Y, time_string, standard);
}

void draw_progress(void) {
    char standard = make_attr(COLOR_WHITE, COLOR_BLACK);
    char progress[16];

    string_at(PROGRESS_VALUE_X, PROGRESS_VALUE_Y, "           ", standard);
    sprintf(progress, "%d/%d", g_globals.progress, g_globals.current_picture->pic_squares);
    string_at(PROGRESS_VALUE_X, PROGRESS_VALUE_Y, progress, standard);
}

void draw_mistakes(void) {
    char standard = make_attr(COLOR_WHITE, COLOR_BLACK);
    char mistakes[10];

    sprintf(mistakes, "%d", g_globals.mistakes);
    string_at(MISTAKES_VALUE_X, MISTAKES_VALUE_Y, mistakes, standard);
}

void draw_cursor_pos_text(void) {
    char standard = make_attr(COLOR_WHITE, COLOR_BLACK);
    char cursor_pos[10];

    sprintf(cursor_pos, "(%3d,%3d)", g_globals.cursor_x + g_globals.viewport_x, g_globals.cursor_y + g_globals.viewport_y);
    string_at(CURSOR_LOC_X, CURSOR_LOC_Y, cursor_pos, standard);
}

void draw_button_area(void) {
    char standard = make_attr(COLOR_WHITE, COLOR_BLACK);
    char highlight = make_attr(COLOR_CYAN, COLOR_BLACK);

    fill_box_at(BUTTON_AREA_X1, BUTTON_AREA_Y1, BUTTON_AREA_X2, BUTTON_AREA_Y2, ' ', standard);
    // draw the lines
    hline_at(BUTTON_AREA_HLINE_X, BUTTON_AREA_HLINE_Y, BUTTON_AREA_HLINE_LEN, 196, standard);
    vline_at(BUTTON_AREA_VLINE1_X, BUTTON_AREA_VLINE1_Y, BUTTON_AREA_VLINE1_LEN, 179, standard);
    vline_at(BUTTON_AREA_VLINE2_X, BUTTON_AREA_VLINE2_Y, BUTTON_AREA_VLINE2_LEN, 179, standard);

    // fix the corners
    char_at(BUTTON_AREA_BOX_X1, BUTTON_AREA_BOX_X1, 195, standard);
    char_at(BUTTON_AREA_BOX_X2, BUTTON_AREA_BOX_Y1, 180, standard);
    char_at(BUTTON_AREA_HLINE_X, BUTTON_AREA_HLINE_Y, 195, standard);
    char_at(BUTTON_AREA_HLINE_X + BUTTON_AREA_HLINE_LEN - 1, BUTTON_AREA_HLINE_Y, 180, standard);
    char_at(BUTTON_AREA_VLINE1_X, BUTTON_AREA_VLINE1_Y, 194, standard);
    char_at(BUTTON_AREA_VLINE2_X, BUTTON_AREA_VLINE2_Y, 194, standard);
    char_at(BUTTON_AREA_VLINE1_X, BUTTON_AREA_VLINE1_Y + BUTTON_AREA_VLINE1_LEN - 1, 193, standard);
    char_at(BUTTON_AREA_VLINE2_X, BUTTON_AREA_VLINE2_Y + BUTTON_AREA_VLINE2_LEN - 1, 193, standard);
    char_at(BUTTON_AREA_VLINE1_X, BUTTON_AREA_HLINE_Y, 197, standard);
    char_at(BUTTON_AREA_VLINE2_X, BUTTON_AREA_HLINE_Y, 197, standard);

    // draw the text
    string_at(SAVE_TEXT_X, SAVE_TEXT_Y, "( )ave", standard);
    string_at(LOAD_TEXT_X, LOAD_TEXT_Y, "( )oad", standard);
    string_at(MARK_TEXT_X, MARK_TEXT_Y, "Mar( )", standard);
    string_at(MAP_TEXT_X, MAP_TEXT_Y, "( )ap", standard);
    string_at(OPTS_TEXT_X, OPTS_TEXT_Y, "( )pts", standard);
    string_at(EXIT_TEXT_X, EXIT_TEXT_Y, "( )xit", standard);
    char_at(SAVE_TEXT_X + 1, SAVE_TEXT_Y, 'S', highlight);
    char_at(LOAD_TEXT_X + 1, LOAD_TEXT_Y, 'L', highlight);
    char_at(MARK_TEXT_X + 4, MARK_TEXT_Y, 'k', highlight);
    char_at(MAP_TEXT_X + 1, MAP_TEXT_Y, 'M', highlight);
    char_at(OPTS_TEXT_X + 1, OPTS_TEXT_Y, 'O', highlight);
    char_at(EXIT_TEXT_X + 1, EXIT_TEXT_Y, 'E', highlight);

}

// Conditionally update the screen for the current state
void render_game_state(void) {
    if (g_globals.render.background) {
        draw_ui_base();
        g_globals.render.background = 0;
    }
    if (g_globals.render.puzzle_box_area) {
        draw_puzzle_box_area();
        g_globals.render.puzzle_box_area = 0;
    }
    if (g_globals.render.legend_area) {
        draw_legend_area();
        g_globals.render.legend_area = 0;
    }
    if (g_globals.render.button_area) {
        draw_button_area();
        g_globals.render.button_area = 0;
    }
    if (g_globals.render.puzzle) {
        draw_puzzle_area();
        g_globals.render.puzzle = 0;
    }
    if (g_globals.render.drawn_square) {
        draw_drawn_square(g_globals.cursor_x, g_globals.cursor_y, g_globals.viewport_x, g_globals.viewport_y);
        g_globals.render.drawn_square = 0;
    }
    if (g_globals.render.palette) {
        draw_legend();
        g_globals.render.palette = 0;
    }
    if (g_globals.render.puzzle_cursor) {
        draw_puzzle_cursor();
        g_globals.render.puzzle_cursor = 0;
    }
    if (g_globals.render.palette_cursor) {
        draw_palette_cursor();
        g_globals.render.palette_cursor = 0;
    }
    if (g_globals.render.cursor_pos_area) {
        draw_cursor_pos_text();
        g_globals.render.cursor_pos_area = 0;
    }
    if (g_globals.render.information_area) {
        draw_information_text();
        g_globals.render.information_area = 0;
    }
    if (g_globals.render.timer_area) {
        draw_timer();
        g_globals.render.timer_area = 0;
    }
    if (g_globals.render.mistake_area) {
        draw_mistakes();
        g_globals.render.mistake_area = 0;
    }
    if (g_globals.render.progress_area) {
        draw_progress();
        g_globals.render.progress_area = 0;
    }
}

void clear_render_components(RenderComponents *r) {
    r->background = 0;
    r->puzzle_box_area = 0;
    r->legend_area = 0;
    r->button_area = 0;
    r->cursor_pos_area = 0;
    r->puzzle = 0;
    r->palette = 0;
    r->puzzle_cursor = 0;
    r->palette_cursor = 0;
    r->timer_area = 0;
    r->mistake_area = 0;
    r->progress_area = 0;
    r->information_area = 0;
    r->drawn_square = 0;
}