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

#define MAX_COLLECTIONS   100
#define MAX_PICTURES      100

#define COLLECTION_PATHSPEC "res/pics/*"
#define PIC_FILE_DIR "res/pics"
#define PROGRESS_FILE_DIR "progress"

#define SAVE_MESSAGE_DURATION   18
#define LOAD_MESSAGE_DURATION   18

static char *g_categories[] = {
    "Misc.",
    "Other",
    "Landscape",
    "Animals",
    "Fantasy",
    "Abstract",
    "Pattern"
};

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


typedef enum {
  COLLECTION_TAB,
  PICTURE_TAB
} LoadTab;

typedef struct {
  unsigned char title;
  unsigned char load_background;
  unsigned char load_collections;
  unsigned char load_pictures;
  unsigned char load_collections_list;
  unsigned char load_pictures_list;
  unsigned char load_metadata_text;
  unsigned char load_collection_cursor;
  unsigned char load_picture_cursor;
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
  unsigned char drawn_square;
  unsigned char marks;
  unsigned char save_message;
  unsigned char load_message;
  unsigned char map;
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
    int cursor_x, cursor_y;
    int old_cursor_x, old_cursor_y;
    int viewport_x, viewport_y;
    int old_viewport_x, old_viewport_y;
    int palette_index, old_palette_index;
    unsigned int total_picture_squares;
    unsigned int progress;
    unsigned int mistakes;
    unsigned char update_screen;
    unsigned char exit_game;
    unsigned char mark_enabled;
    State current_state;
    State previous_state;
    Picture *current_picture;
    RenderComponents render;
    unsigned char timer_running;
    long start_ticks;
    long elapsed_seconds;
    unsigned short num_collections;
    unsigned short num_pictures;
    char collection_name[9];
    char picture_file_basename[9];
    LoadTab active_load_window;
    char top_collection;    // The item at the top of the collection list
    char top_picture;       // The item at the top of the picture list
    char old_top_collection;
    char old_top_picture;
    char selected_collection;
    char selected_picture;
    char old_selected_collection;
    char old_selected_picture;
    char saving_in_progress;
    char loading_in_progress;
    long save_message_start_ticks;
    long load_message_start_ticks;
    char map_hide_legend;
    int map_x, map_y;
} GameGlobals;

typedef struct {
    char name[9];
    char category;
    short width;
    short height;
    char colors;
    int progress;
    int total;
} PictureItem;

typedef struct {
    char name[9];
    int items;
} CollectionItem;

extern void (__interrupt __far *old_isr)();
extern volatile long g_clock_ticks;
extern unsigned char g_timer_overflow;

#define PALETTE_ENTRIES     64
extern char palette_chars[PALETTE_ENTRIES + 1];

extern GameGlobals g_globals;
