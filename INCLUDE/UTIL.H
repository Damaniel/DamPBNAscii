#pragma once
#include "globals.h"

/* The flags are as follows:

   - bit 0: transparency (0 = opaque, 1 = transparent)
   - bit 1: filled in (0 = no, 1 = yes)
   - bit 2: correct (0 = no, 1 = yes)
*/
#define F_TRANSPARENT   0
#define F_FILLED_IN     1
#define F_CORRECT       2

#define COMPRESSION_NONE   0
#define COMPRESSION_RLE    1

extern void clear_global_game_state(GameGlobals *g);

extern void set_transparency_flag(ColorSquare *c, unsigned char flag);

extern void set_filled_flag(ColorSquare *c, unsigned char flag);

extern void set_correct_flag(ColorSquare *c, unsigned char flag);

extern int is_transparent(ColorSquare *c);

extern int is_filled_in(ColorSquare *c);

extern int is_correct(ColorSquare *c);

extern Picture * load_picture_file(char *filename);

extern void free_picture_file(Picture *p);

extern int get_picture_color_at(Picture *p, int x, int y);

extern ColorSquare *get_color_square(Picture *p, int x, int y);