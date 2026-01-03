#include "tui.h"
#include "geometry.h"
#include <stddef.h>

void
s_clear(screen *s) {
    // sets all the buffer to ' '
    memset(s->buff, ' ', s->size);
    
    memset(s->zbuf, 0, s->rows * s->cols * sizeof(double));
    
    // sets the last col to be '\n'
    for (size_t y = 0; y < s->rows; y++) {
        s->buff[y * (s->cols + 1) + s->cols] = '\n';
    }
    s->buff[s->size] = '\0'; 
}

screen*
init_screen(
    size_t rows,
    size_t cols
) {
    screen *s = (screen*)xmalloc(sizeof(screen));
    s->rows   = rows;
    s->cols   = cols;
    s->size   = rows * (cols + 1); 
    s->buff   =   (char*)xmalloc(s->size + 1);
    s->zbuf   = (double*)xmalloc(sizeof(double)*rows*cols);
    
    s_clear(s);
    
    return s;
}

void free_screen(screen *s) {
    if (s) {
        if (s->buff) free(s->buff);
        if (s->zbuf) free(s->zbuf);
        free(s);
    }
}


vec2u coords_to_pix(screen *s, vec2 p) {
    float char_aspect = 2.3f; 

    float scale_y = (float)s->rows * 0.5f;
    float scale_x = scale_y * char_aspect;

    int center_x = (int)(s->cols / 2);
    int center_y = (int)(s->rows / 2);

    size_t screen_x = (size_t)( p.x * scale_x + center_x);
    size_t screen_y = (size_t)(-p.y * scale_y + center_y);

    return (vec2u){screen_x, screen_y};
}

void
s_plot(screen *s, vec3 v, char c) {
    double ooz    = 1 / v.z;
    vec2u  coords = coords_to_pix(s, vec3_project(v));

    
    if (coords.x >= s->cols ||
        coords.y >= s->rows)
        return;

    size_t idx = coords.y * (size_t)(s->cols + 1) + coords.x;

    if (ooz > s->zbuf[idx]) {
        s->zbuf[idx] = ooz;
        s_put(s, coords.x, coords.y, c);
    }
}

void
s_put(
    screen *s,
    size_t  x,
    size_t  y,
    char    c
) {
    if (x >= s->cols || y >= s->rows)
        return;
    
    size_t idx = (size_t)y * (s->cols + 1) + (size_t)x;
    s->buff[idx] = c;
}

void
s_write(
    screen *s,
    size_t  x,
    size_t  y,
    const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        s_put(s, x + i, y, str[i]);
    }
}

void
s_draw_text(
    screen *s,
    size_t  x,
    size_t  y,
    const char* fmt, ...) {
    char temp_buff[256]; 

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(temp_buff, sizeof(temp_buff), fmt, ap);
    va_end(ap);

    s_write(s, x, y, temp_buff);
}



// ================= TERMINAL SETUP =================

struct termios orig_termios;

void disableRawMode(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    printf("\x1b[?25h"); // Mostra cursore
}

void enableRawMode(void) {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    
    raw.c_lflag &= ~((tcflag_t)(ECHO | ICANON));
    raw.c_iflag &= ~((tcflag_t)(IXON)); 
    
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    
    printf("\x1b[?25l");
}

void get_terminal_size(size_t *rows, size_t *cols) {
    struct winsize w;
    
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        *rows = 24;
        *cols = 80;
    } else {
        *rows = w.ws_row;
        *cols = w.ws_col;
    }
}
