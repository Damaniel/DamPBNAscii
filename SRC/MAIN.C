#include <malloc.h>
#include "textdraw.h"
#include "util.h"
#include "uiconsts.h"
#include "render.h"
#include "globals.h"

char palette_chars[PALETTE_ENTRIES + 1] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ$#";

GameGlobals g_globals;

void print_mem_stats() {
    printf("Available memory: %u bytes\n", _memavl());
    printf("Maximum allocatable chunk: %u bytes\n", _memmax());
}

int main(void) {
    int i, j, c;
    char border, bg, title, highlight;

    set_text_mode(MODE_80X25);
    set_bg_intensity(1);
    clear_screen();

    clear_global_game_state(&g_globals);

    g_globals.current_picture = load_picture_file("test1.pic");

    for(i=0;i<g_globals.current_picture->w; i++) {
        for(j=0;j<g_globals.current_picture->h; j++) {
            set_filled_flag(&(g_globals.current_picture->pic_squares[j * g_globals.current_picture->w + i]), 1);
        }
    }
    draw_all();

    getchar();

    clear_screen();
    free_picture_file(g_globals.current_picture);

    return 0;
}