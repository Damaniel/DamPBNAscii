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

// Scancode definitions
#define KEY_ESC              1
#define KEY_1                2
#define KEY_2                3
#define KEY_3                4
#define KEY_4                5
#define KEY_5                6
#define KEY_6                7
#define KEY_7                8
#define KEY_8                9
#define KEY_9               10
#define KEY_0               11
#define KEY_MINUS           12
#define KEY_EQUALS          13
#define KEY_BACKSPACE       14
#define KEY_TAB             15
#define KEY_Q               16
#define KEY_W               17
#define KEY_E               18
#define KEY_R               19
#define KEY_T               20
#define KEY_Y               21
#define KEY_U               22
#define KEY_I               23
#define KEY_O               24
#define KEY_P               25
#define KEY_LEFT_BRACKET    26
#define KEY_RIGHT_BRACKET   27
#define KEY_ENTER           28
#define KEY_CTRL            29
#define KEY_A               30
#define KEY_S               31
#define KEY_D               32
#define KEY_F               33
#define KEY_G               34
#define KEY_H               35
#define KEY_J               36
#define KEY_K               37
#define KEY_L               38
#define KEY_SEMICOLON       39
#define KEY_APOSTROPHE      40
#define KEY_TILDE           41
#define KEY_LEFT_SHIFT      42
#define KEY_BACKSLASH       43
#define KEY_Z               44
#define KEY_X               45
#define KEY_C               46
#define KEY_V               47
#define KEY_B               48
#define KEY_N               49
#define KEY_M               50
#define KEY_COMMA           51
#define KEY_PERIOD          52
#define KEY_FORWARD_SLASH   53
#define KEY_RIGHT_SHIFT     54
#define KEY_PRINT_SCREEN    55
#define KEY_ALT             56
#define KEY_SPACE           57
#define KEY_CAPS_LOCK       58
#define KEY_F1              59
#define KEY_F2              60
#define KEY_F3              61
#define KEY_F4              62
#define KEY_F5              63
#define KEY_F6              64
#define KEY_F7              65
#define KEY_F8              66
#define KEY_F9              67
#define KEY_F10             68
#define KEY_F11             133
#define KEY_F12             134
#define KEY_NUM_LOCK        69
#define KEY_SCROLL_LOCK     70
#define KEY_HOME            71
#define KEY_UP              72
#define KEY_PAGE_UP         73
#define KEY_NUM_MINUS       74
#define KEY_LEFT            75
#define KEY_CENTER          76
#define KEY_RIGHT           77
#define KEY_NUM_PLUS        78
#define KEY_END             79
#define KEY_DOWN            80
#define KEY_PAGE_DOWN       81
#define KEY_INSERT          82
#define KEY_DELETE          83

// Shift state definitions
#define STATE_RIGHT_SHIFT      0x01
#define STATE_LEFT_SHIFT       0x02
#define STATE_CTRL             0x04
#define STATE_ALT              0x08

// Returns the scan code of the key in the lower byte, and the shift state in the upper byte
extern unsigned short get_input_key(void);

// process input for the current state
extern void process_input(short key);