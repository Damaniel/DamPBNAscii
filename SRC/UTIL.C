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

PictureItem g_pictures[MAX_PICTURES];
CollectionItem g_collections[MAX_COLLECTIONS];

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

void dump_picture_file(void) {
    int i, j;
    ColorSquare *cs;
    printf("--------------------------------------\n");
    for(j=0;j<g_globals.current_picture->h;j++) {
        for(i=0;i<g_globals.current_picture->w;i++) {
            cs = get_color_square(g_globals.current_picture, i, j);
            printf("%c ", palette_chars[cs->pal_entry]);
        }
        printf("\n");
    }
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

    if(pic->version == 2) {
        g_globals.total_picture_squares = total_trans_picture_squares;
    }
    else {
        g_globals.total_picture_squares = pic->w * pic->h;
    }

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

void reset_image_state(GameGlobals *g) {
    g->cursor_x = 0;
    g->cursor_y = 0;
    g->old_cursor_x = 0;
    g->old_cursor_y = 0;
    g->viewport_x = 0;
    g->viewport_y = 0;
    g->old_viewport_x = 0;
    g->old_viewport_y = 0;
    g->palette_index = 0;
    g->mark_enabled = 0;
    g->map_x = 0;
    g->map_y = 0;
    g->map_hide_legend = 0;
    g->selected_option = 0;
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
    g->top_collection = 0;
    g->top_picture = 0;
    g->old_top_collection = 0;
    g->old_top_picture = 0;
    g->selected_collection = 0;
    g->selected_picture = 0;
    g->old_selected_collection = 0;
    g->old_selected_picture = 0;
    g->saving_in_progress = 0;
    g->loading_in_progress = 0;
    g->save_message_start_ticks = 0;
    g->load_message_start_ticks = 0;
    g->map_x = 0;
    g->map_y = 0;
    g->map_hide_legend = 0;
    g->use_high_res_text_mode = 0;
    g->selected_option = 0;
}

int save_options_file(void) {
    FILE *fp;

    fp = fopen(OPTIONS_FILE, "wb");
    if (fp == NULL) {
        return -1;
    }
    fputc(g_globals.option_high_res, fp);
    fputc(g_globals.option_mark_default, fp);
    fputc(g_globals.option_auto_save, fp);
    fputc(g_globals.option_sound, fp);

    fclose(fp);
    return 0;
}

int load_options_file(void) {
    FILE *fp;

    // Set some sane defaults
    g_globals.option_high_res = 1;
    g_globals.option_mark_default = 1;
    g_globals.option_auto_save = 0;
    g_globals.option_sound = 0;

    // Now get the actual options
    fp = fopen(OPTIONS_FILE, "rb");
    if (fp == NULL) {
        return -1;
    }
    g_globals.option_high_res = fgetc(fp);
    g_globals.option_mark_default = fgetc(fp);
    g_globals.option_auto_save = fgetc(fp);
    g_globals.option_sound = fgetc(fp);

    fclose(fp);

    return 0;
}

/*=============================================================================
 * save_progress_file
 *============================================================================*/
int save_progress_file(Picture *p) {
  FILE *fp;
  int i, j;
  char progress_file[80];
  ColorSquare *cs;

  sprintf(progress_file, "%s/%s/%s.pro",  PROGRESS_FILE_DIR, 
          g_globals.collection_name,
          g_globals.picture_file_basename);

  fp = fopen(progress_file, "wb");
  if (fp == NULL)
    return -1;

  fputc('P', fp);
  fputc('R', fp);
  fputc('A', fp);
  fputc('S', fp);
  
  /* Write the image size */
  fwrite(&p->w, 1, sizeof(short), fp);
  fwrite(&p->h, 1, sizeof(short), fp);

  /* Write total elapsed time */
  fwrite(&(g_globals.elapsed_seconds), 1, sizeof(unsigned long), fp);

  /* Write correct and wrong count */
  fwrite(&g_globals.mistakes, 1, sizeof(int), fp);
  fwrite(&g_globals.progress, 1, sizeof(int), fp);

  /* Write out flag data */
  for (j = 0; j < p->h; j++) {
    for(i = 0; i < p->w; i++) {
        cs = get_color_square(p, i, j);
        fwrite(&(cs->flags), 1, sizeof(char), fp);
    }
  }

  fclose(fp);

  return 0;
}

int load_progress_file(Picture *p) {
    FILE *fp;
    char progress_file[128];
    char magic[4];
    short width, height;
    char flag;
    int i, j;
    ColorSquare *cs;

    // Clear some settings in case no progress is actually found
    g_globals.elapsed_seconds = 0;
    g_globals.progress = 0;
    g_globals.mistakes = 0;

    sprintf(progress_file, "%s/%s/%s.pro",  PROGRESS_FILE_DIR, 
            g_globals.collection_name, 
            g_globals.picture_file_basename);

     fp = fopen(progress_file, "rb");
     if (fp == NULL) {
       /* No progress file.  That's fine. */
       return 0;
     }

    /* If the first four bytes aren't PRAS', then return */
    magic[0] = fgetc(fp);
    magic[1] = fgetc(fp);
    magic[2] = fgetc(fp);
    magic[3] = fgetc(fp);
    if(magic[0] != 'P' || magic[1] != 'R' || magic[2] != 'A' || magic[3] != 'S') {
        fclose(fp);
        return -1;
    }

    /* Load the width and the height.  If they don't match the provided picture,
         return an error */
    fread(&width, 1, sizeof(short), fp);
    fread(&height, 1, sizeof(short), fp);
    if(width != p->w || height != p->h) {
        return -1;
    }

    /* Load and set elapsed time */
    fread(&(g_globals.elapsed_seconds), 1, sizeof(unsigned long), fp);

    /* Load and set mistake count */
    fread(&(g_globals.mistakes), 1, sizeof(int), fp);

    /* Load and set progress counter */
    fread(&(g_globals.progress), 1, sizeof(int), fp);

    for(j=0; j<p->h; j++) {
        for (i=0;i<p->w; i++) {
            fread(&flag, 1, sizeof(char), fp);
            cs = get_color_square(p, i, j);
            cs->flags = flag;
        }
    }

    fclose(fp);
    return 0;

}

// File browser
//
// Puzzle files are grouped into 'collections', which are just a folder containing one or more puzzle files.
// In DamPBN (non-ASCII), the game supported up to 1000 collections with 1000 pictures each.  To keep 
// memory usage down, this version will only support up to 100 collections with 100 pictures each.  
// Later, I'll probably write a program that lets the user swap collections in and out (along with any
// progress data).
// 
// All we really need to store is the names of the collections and the filenames (minus extension) for 
// the current collection, plus some basic metadata.  That's roughly 1k for the collections, and a 
// little over 2k for the filenames of the collection.

/*=============================================================================
 * get_picture_metadata
 *============================================================================*/
void get_picture_metadata(char *basepath, char *filename, PictureItem *p) {
    char full_file[128];
    FILE *fp;
    int i;
    char v2;

    sprintf(full_file, "%s/%s.pic", basepath, filename);
    fp = fopen(full_file, "rb");

    /* Get the relevant fields from this file */
    /* Category, dimensions, colors */
    fgetc(fp);
    fgetc(fp);
    fgetc(fp);
    fgetc(fp);
    fread(&p->width, 1, sizeof(short), fp);
    fread(&p->height, 1, sizeof(short), fp);
    fread(&p->category, 1, sizeof(char), fp);
    for(i=0;i<32;i++)
       fgetc(fp);
    fread(&p->colors, 1, sizeof(char), fp);
    for(i=0;i<193;i++)
       fgetc(fp);
    fread(&v2, 1, sizeof(char), fp);
    if (v2) {
      fread(&p->total, 1, sizeof(short), fp);
    } else {
      p->total = p->width * p->height;
    }
    fclose(fp);    
}

/*=============================================================================
 * get_progress_metadata
 *============================================================================*/
void get_progress_metadata(char *basepath, char *filename, PictureItem *p) {
    char full_file[128];
    FILE *fp;
    int i;

    sprintf(full_file, "%s/%s.pro", basepath, filename);
    fp = fopen(full_file, "rb");
    if (fp == NULL) {
        p->progress = 0;
        return;
    }

    for (i=0;i<14;i++)
        fgetc(fp);

    fread(&p->progress, 1, sizeof(int), fp);
    fclose(fp);
}

void get_collections(void) {
    struct find_t fileinfo;
    unsigned int rc;
    int i;

    g_globals.num_collections = 0;

    rc = _dos_findfirst(COLLECTION_PATHSPEC, _A_SUBDIR, &fileinfo);
    while (rc == 0 && g_globals.num_collections < MAX_COLLECTIONS) {
        if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
            strncpy(g_collections[g_globals.num_collections].name, fileinfo.name, 8);
            ++g_globals.num_collections;
        }
        rc = _dos_findnext(&fileinfo);
    }

}

void get_pictures(int collection_idx) {
    struct find_t fileinfo;
    char pic_pathspec[64];
    char pic_dir[64];
    char progress_dir[64];
    int total_files;
    unsigned int rc;
    char *filename;

    sprintf(pic_pathspec, "%s/%s/*.pic", PIC_FILE_DIR, g_collections[collection_idx].name);
    sprintf(pic_dir, "%s/%s", PIC_FILE_DIR, g_collections[collection_idx].name);
    sprintf(progress_dir, "%s/%s", PROGRESS_FILE_DIR, g_collections[collection_idx].name);

    total_files = 0;
    rc = _dos_findfirst(pic_pathspec, _A_NORMAL, &fileinfo);
    while (rc == 0 && total_files < MAX_PICTURES) {
        filename = strtok(fileinfo.name, ".");
        strncpy(g_pictures[total_files].name, filename, 8);
        get_picture_metadata(pic_dir, g_pictures[total_files].name, &g_pictures[total_files]);
        get_progress_metadata(progress_dir, g_pictures[total_files].name, &g_pictures[total_files]);
        ++total_files;
        rc = _dos_findnext(&fileinfo);
    }

    g_globals.num_pictures = total_files;
}
