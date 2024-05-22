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

// This file contains all struct definitions, including the GameGlobals object, which contains all of the 
// variables specific to the (global) instance of the game.

typedef enum {
  STATE_NONE,
  STATE_TITLE,
  STATE_GAME,
  STATE_LOAD,
  STATE_SAVE,
  STATE_HELP,
  STATE_OPTS,
  STATE_MAP,
  STATE_LOAD_DIALOG,
  STATE_FINISHED,
  STATE_EXIT
} State;

typedef struct {
  unsigned char background;
  unsigned char puzzle_box_area;
  unsigned char legend_area;
  unsigned char button_area;
  unsigned char puzzle;
  unsigned char palette;
  unsigned char puzzle_cursor;
  unsigned char palette_cursor;
  unsigned char cursor_pos_area;
  unsigned char timer_area;
  unsigned char mistake_area;
  unsigned char progress_area;
  unsigned char information_area;
} RenderComponents;

typedef struct {
    unsigned char pal_entry;
    unsigned char flags;
} ColorSquare;

typedef struct {
  unsigned char version;
  char image_name[32 + 1];
  unsigned short w;
  unsigned short h;
  unsigned char category;
  unsigned char num_colors;
  unsigned int num_mistakes;
  unsigned char pal[64][3];
  ColorSquare *pic_squares;
} Picture;

typedef struct {
    unsigned int cursor_x, cursor_y;
    unsigned int old_cursor_x, old_cursor_y;
    unsigned int viewport_x, viewport_y;
    unsigned int palette_index, old_palette_index;
    unsigned int total_picture_squares;
    unsigned int progress;
    unsigned int mistakes;
    unsigned char update_screen;
    unsigned char exit_game;
    State current_state;
    State previous_state;
    Picture *current_picture;
    RenderComponents render;
} GameGlobals;

#define PALETTE_ENTRIES     64
extern char palette_chars[PALETTE_ENTRIES + 1];

extern GameGlobals g_globals;
