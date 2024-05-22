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
#include "includes.h"

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