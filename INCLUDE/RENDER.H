#pragma once
#include "uiconsts.h"
#include "textdraw.h"
#include "globals.h"
#include "util.h"
#include <stdio.h>
#include <string.h>

extern void draw_all(void);

extern void draw_ui_base(void);
extern void draw_puzzle_area(void);
extern void draw_puzzle_box_area(void);
extern void draw_legend_area(void);
extern void draw_legend(void);
extern void draw_information_area(void);
extern void draw_button_area(void);
extern void draw_cursor_pos_text(void);