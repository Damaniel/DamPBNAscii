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
    draw_ui_base();
    draw_puzzle_box_area();
    draw_puzzle_area();
    draw_legend_area();
    draw_legend();
    draw_information_text();
    draw_cursor_pos_text();
    draw_button_area();
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
    int i, j;
    char cur_char, attr;
    ColorSquare *cs;

    fill_box_at(DRAW_AREA_X1, DRAW_AREA_Y1, DRAW_AREA_X2, DRAW_AREA_Y2, ' ', standard);
    for (j=0; j < DRAW_VISIBLE_HEIGHT; j++) {
        for (i=0; i < DRAW_VISIBLE_WIDTH; i++) {
            cs = get_color_square(g_globals.current_picture, i + g_globals.cursor_x, j + g_globals.cursor_y);
            if (!is_transparent(cs)) {
                if(is_filled_in(cs)) {
                    attr = make_attr(g_globals.current_picture->pal[cs->pal_entry][1], g_globals.current_picture->pal[cs->pal_entry][2]);
                    char_at(DRAW_AREA_X1 + (i * 2), DRAW_AREA_Y1 + j, g_globals.current_picture->pal[cs->pal_entry][0], attr);
                    char_at(DRAW_AREA_X1 + (i * 2) + 1, DRAW_AREA_Y1 + j, g_globals.current_picture->pal[cs->pal_entry][0], attr);
                }
                else {
                    cur_char = get_picture_color_at(g_globals.current_picture, i + g_globals.cursor_x, j + g_globals.cursor_y);
                    char_at(DRAW_AREA_X1 + (i * 2) + 1, DRAW_AREA_Y1 + j, palette_chars[cur_char], dimmer);
                }
            }
            else {
                    char_at(DRAW_AREA_X1 + (i * 2), DRAW_AREA_Y1 + j, '.', dimmer);
                    char_at(DRAW_AREA_X1 + (i * 2) + 1, DRAW_AREA_Y1 + j, '.', dimmer);                
            }
        }
    }
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
    fill_box_at(STATUS_AREA_X1, STATUS_AREA_Y1, STATUS_AREA_X2, STATUS_AREA_Y2, ' ', standard);
}

void draw_cursor_pos_text(void) {
    char standard = make_attr(COLOR_WHITE, COLOR_BLACK);
    char cursor_pos[10];

    sprintf(cursor_pos, "(%3d,%3d)", g_globals.cursor_x, g_globals.cursor_y);
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

// Update the screen for the current state
void update_screen(void) {

}