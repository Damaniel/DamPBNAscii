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

int is_transparent(ColorSquare *c) {
    return (c->flags >> F_TRANSPARENT) & 1;
}

int is_filled_in(ColorSquare *c) {
    return (c->flags >> F_FILLED_IN) & 1;
}

int is_correct(ColorSquare *c) {
    return (c->flags >> F_CORRECT) & 1;
}

void set_transparency_flag(ColorSquare *c, unsigned char value) {
    c->flags = (c->flags & ~(1 << F_TRANSPARENT)) | (value << F_TRANSPARENT);

}

void set_filled_flag(ColorSquare *c, unsigned char value) {
    c->flags =(c->flags & ~(1 << F_FILLED_IN)) | (value << F_FILLED_IN);
}

void set_correct_flag(ColorSquare *c, unsigned char value) {
    c->flags =(c->flags & ~(1 << F_CORRECT)) | (value << F_CORRECT);
}

Picture * load_picture_file(char *filename) {
    FILE *fp;
    Picture *pic;
    unsigned char magic[4];
    unsigned char compression;
    unsigned char transparent_flag;
    unsigned int bytes_processed;
    unsigned char transparent_val;
    unsigned char first_byte;
    unsigned int run_length;
    unsigned int i;
    unsigned int total_trans_picture_squares;

    fp = fopen(filename, "rb");
    if (fp == NULL) {
        return NULL;
    }

    /* Check for magic bytes */
    fscanf(fp, "%c%c%c%c", &magic[0], &magic[1], &magic[2], &magic[3]);
    if(magic[0] != 'D' || magic[1] != 'P' || magic[2] != 'T' || magic[3] != 'M') {
        fclose(fp);
        return NULL;
    }

    /* Set up the Picture object */
    pic = (Picture *)malloc(sizeof(Picture));

    /* Read in the header */
    fread(&(pic->w), 1, sizeof(short), fp);
    fread(&(pic->h), 1, sizeof(short), fp);
    fread(&(pic->category), 1, sizeof(unsigned char), fp);
    fread(pic->image_name, 32, sizeof(char), fp);
    fread(&(pic->num_colors), 1, sizeof(unsigned char), fp);
    fread(&compression, 1, sizeof(unsigned char), fp);
    for(i=0; i<64; i++) {
        pic->pal[i][0] = fgetc(fp);
        pic->pal[i][1] = fgetc(fp);
        pic->pal[i][2] = fgetc(fp);
    }

    transparent_flag = fgetc(fp);
    if (transparent_flag) {
        pic->version = 2;
        fread(&total_trans_picture_squares, 1, sizeof(short), fp);
        for(i=0;i<18;i++) {
            fgetc(fp);
        }
    }
    else {
        pic->version = 1;
        for(i=0;i<20;i++) {
            fgetc(fp);
        }
    }

    /* Create required Picture arrays */
    pic->pic_squares = (ColorSquare *)malloc(pic->w * pic->h *
                                             sizeof(ColorSquare));

    /* Check compression type and perform appropriate decompression */
    if(compression == COMPRESSION_NONE) {
        for(i=0; i< (pic->w*pic->h); i++) {
            /* Using '+ 1'  since palettes in the Picture go from 1-64, not 0-63 */
            set_transparency_flag(&(pic->pic_squares[i]), 0);
            set_filled_flag(&(pic->pic_squares[i]), 0);
            set_correct_flag(&(pic->pic_squares[i]), 0);
            (pic->pic_squares[i]).pal_entry = fgetc(fp);
        }
    } else {
        bytes_processed = 0;
        while (bytes_processed < (pic->w * pic->h)) {
            first_byte = fgetc(fp);
            if(first_byte & 0x80) {
                /* found a run.  Load the next byte and write the appropriate
                    number of copies to the buffer */
                run_length = fgetc(fp);
                for (i=0;i<run_length;i++) {
                    set_transparency_flag(&(pic->pic_squares[bytes_processed]), 0);
                    set_filled_flag(&(pic->pic_squares[bytes_processed]), 0);
                    set_correct_flag(&(pic->pic_squares[bytes_processed]), 0);
                    (pic->pic_squares[bytes_processed]).pal_entry = (first_byte & 0x7F);    
                    bytes_processed++;
                }
            } else {
                /* Found a single value */
                set_transparency_flag(&(pic->pic_squares[bytes_processed]), 0);
                set_filled_flag(&(pic->pic_squares[bytes_processed]), 0);
                set_correct_flag(&(pic->pic_squares[bytes_processed]), 0);
                (pic->pic_squares[bytes_processed]).pal_entry = first_byte;
                bytes_processed++;
            }
        }
    }

  /* Process the transparency data for the image*/
  if (transparent_flag) {
    if(compression == COMPRESSION_NONE) {
     for(i=0; i< (pic->w*pic->h); i++) {
       transparent_val = fgetc(fp);  
       set_transparency_flag(&(pic->pic_squares[i]), (transparent_val == 0) ? 1: 0);
     }
   } else {
     bytes_processed = 0;
     while (bytes_processed < (pic->w * pic->h)) {
       first_byte = fgetc(fp);
       if(first_byte & 0x80) {
         /* found a run.  Load the next byte and write the appropriate
            number of copies to the buffer */
         transparent_val = first_byte & 0x7F;
         run_length = fgetc(fp);
         for (i=0;i<run_length;i++) {
           set_transparency_flag(&(pic->pic_squares[bytes_processed]), (transparent_val == 0) ? 1: 0);
           bytes_processed++;
         }
       } else {
         /* Found a single value */
         transparent_val = first_byte;
         set_transparency_flag(&(pic->pic_squares[bytes_processed]), (transparent_val == 0) ? 1: 0);
         bytes_processed++;
       }
     } 
    }
  }

    g_globals.total_picture_squares = total_trans_picture_squares;

    fclose(fp);
    return pic;
}

ColorSquare *get_color_square(Picture *p, int x, int y) {
    if (x < 0 || y < 0 || x >= p->w || y >= p->h) {
        printf("Warning! Out of bounds!\n");
        return &(p->pic_squares[0]);
    }
    return &(p->pic_squares[y*p->w + x]);
}

int get_picture_color_at(Picture *p, int x, int y) {
    if (x < 0 || y < 0 || x >= p->w || y >= p->h) {
        return -1;
    }
    return p->pic_squares[y*p->w + x].pal_entry;
}

void free_picture_file(Picture *p) {
    if (p == NULL) {
        return;
    }

    if (p->pic_squares != NULL) {
        free(p->pic_squares);
    }
}

void clear_global_game_state(GameGlobals *g) {
    g->cursor_x = 0;
    g->cursor_y = 0;
    g->old_cursor_x = 0;
    g->old_cursor_y = 0;
    g->viewport_x = 0;
    g->viewport_y = 0;
    g->old_viewport_x = 0;
    g->old_viewport_y = 0;
    g->palette_index = 0;
    g->current_state = STATE_NONE;
    g->previous_state = STATE_NONE;
    g->exit_game = 0;
    g->elapsed_seconds = 0;
    g->timer_running = 0;
    g->start_ticks = 0;
    g->mark_enabled = 0;
}