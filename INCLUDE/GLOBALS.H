#pragma once

// This file contains all struct definitions, including the GameGlobals object, which contains all of the 
// variables specific to the (global) instance of the game.

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
    unsigned int palette_index;
    unsigned int total_picture_squares;
    unsigned int progress;
    unsigned int mistakes;
    Picture *current_picture;
} GameGlobals;

#define PALETTE_ENTRIES     64
extern char palette_chars[PALETTE_ENTRIES + 1];

extern GameGlobals g_globals;
