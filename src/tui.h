#ifndef __TUI_H 
#define __TUI_H

#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include <sys/ioctl.h> 
#include <unistd.h>

#include "geometry.h"
#include "tools.h"

/* ================= TUI STUFF ================= */

typedef struct {
    size_t  rows;
    size_t  cols;
    size_t  size; 
    char   *buff;
    double *zbuf;
} screen;

typedef enum {
    BLACK = 30, RED = 31, GREEN = 32, YELLOW = 33,
    BLUE = 34, MAGENTA = 35, CYAN = 36, WHITE = 37
} Color;

screen*
init_screen(
    int rows, int cols
);

void
free_screen(screen *s);

void s_clear(screen *s);

void
s_plot(screen *s, vec3 v, char c);

void
s_put(screen *s, int x, int y, char c);

void
s_setcursor(screen *s, int i, int j);

void
s_write(screen *s, int x, int y, const char* str);

void
s_draw_text(screen *s, int x, int y, const char* fmt, ...);


static inline void
set_color(Color c) {
    printf("\x1b[%dm", c);
}

static inline void
reset_color(void) {
    printf("\x1b[0m");
}


static inline void
clear_screen(void) {
    // clear the screen
    printf("\x1b[2J");
    // move cursor to 0, 0
    printf("\x1b[H");
}


void disableRawMode();
void enableRawMode();
void get_terminal_size(int *rows, int *cols);


#endif
