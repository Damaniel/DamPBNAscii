#include "textdraw.h"
#include "util.h"

int main(void) {
    Picture *p;
    int i, j, c;
    char border, bg, title, highlight;

    set_text_mode(MODE_80X25);
    set_bg_intensity(1);
    clear_screen();

    p = load_picture_file("test2.pic");

    border = make_attr(15, 0);
    bg = make_attr(1, 9);
    title = make_attr(15, 3);
    highlight = make_attr(0, 14);

    fill_box_at(0, 0, 79, 42, 177, bg);
    hline_at(0, 0, 80, ' ', title);

    box_at(4, 4, 37, 21, BORDER_DOUBLE, border);
    fill_box_at(5, 5, 36, 20, ' ', border);
    char_at(60, 5, '1', highlight);

    for (j=0;j<16;j++) {
        for(i=0;i<16;i++) {
            if(is_transparent(&(p->pic_squares[j*p->w + i]))) {
                char_at(5+i*2, 5+j, 219, make_attr(15, 0));
                char_at(5+i*2+1, 5+j, 219, make_attr(15, 0));

            }
            else {
                c = get_picture_color_at(p, i, j) - 1;
                char_at(5+i*2, 5+j, p->pal[c][0], make_attr(p->pal[c][1], p->pal[c][2]));
                char_at(5+i*2+1, 5+j, p->pal[c][0], make_attr(p->pal[c][1], p->pal[c][2]));
            }
        }
    }

    getchar();

    clear_screen();
    free_picture_file(p);

    return 0;
}