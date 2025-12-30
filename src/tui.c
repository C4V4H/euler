#include "tui.h"




void
s_clear(screen *s) {
    // sets all the buffer to ' '
    memset(s->buff, ' ', s->size);
    
    memset(s->zbuf, 0, s->rows * s->cols * sizeof(float));
    
    // sets the last col to be '\n'
    for (int y = 0; y < s->rows; y++) {
        s->buff[y * (s->cols + 1) + s->cols] = '\n';
    }
    s->buff[s->size] = '\0'; 
}

screen*
init_screen(
    int rows,
    int cols
) {
    screen *s = (screen*)xmalloc(sizeof(screen));
    s->rows   = rows;
    s->cols   = cols;
    s->size   = rows * (cols + 1); 
    s->buff   = (char*)xmalloc(s->size + 1);
    s->zbuf   = (float*)xmalloc(sizeof(float)*rows*cols);
    
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

void
s_put(screen *s, int x, int y, char c) {
    if (x < 0 || x >= s->cols || y < 0 || y >= s->rows) return;
    
    s->buff[y * (s->cols + 1) + x] = c;
}

void
s_write(screen *s, int x, int y, const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        s_put(s, x + i, y, str[i]);
    }
}

void
s_draw_text(screen *s, int x, int y, const char* fmt, ...) {
    char temp_buff[256]; 

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(temp_buff, sizeof(temp_buff), fmt, ap);
    va_end(ap);

    s_write(s, x, y, temp_buff);
}



// ================= TERMINAL SETUP =================

struct termios orig_termios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    printf("\x1b[?25h"); // Mostra cursore
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_iflag &= ~(IXON); 
    
    // === AGGIUNGI QUESTE DUE RIGHE ===
    raw.c_cc[VMIN] = 0;  // Numero minimo di byte da leggere: 0
    raw.c_cc[VTIME] = 0; // Timeout: 0 decimi di secondo (ritorna subito)
    // =================================

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    
    printf("\x1b[?25l"); // Nascondi cursore
}
void get_terminal_size(int *rows, int *cols) {
    struct winsize w;
    
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        *rows = 24;
        *cols = 80;
    } else {
        *rows = w.ws_row;
        // *cols = w.ws_col;
        *cols = w.ws_row*2.3;
    }
}
