
#include <math.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

#include "tui.h"

/* ================= GEOMETRY STUFF ================= */

const float FPS = 60;

typedef struct {
    float x, y;
} point2d;

typedef struct {
    float x, y, z;
} point3d;

point2d
new_p2(float x, float y) {
    return (point2d) {x, y};
}
point3d
new_p3(float x, float y, float z) {
    return (point3d) {x, y, z};
}

point2d
normalize(point3d p) {
    if (p.z == 0) p.z = 0.001f;
    return new_p2( 
        p.x/p.z,
        p.y/p.z
    );
}



void
rotate_y(point3d *p, double delta) {
    float x = p->x, z = p->z;
    p->x = x*cos(delta) - z*sin(delta);
    p->z = x*sin(delta) + z*cos(delta);
} 
void
rotate_x(point3d *p, double delta) {
    float y = p->y, z = p->z;
    p->y = y*cos(delta) - z*sin(delta);
    p->z = y*sin(delta) + z*cos(delta);
} 
void
rotate_z(point3d *p, double delta) {
    float x = p->x, y = p->y;
    p->x = x*cos(delta) - y*sin(delta);
    p->y = x*sin(delta) + y*cos(delta);
} 

point2d
coords_to_pix(screen *s, point2d p) {
    //               -1..1 -> 0..2 -> 0..1 -> 0..cols 
    int screen_x = (int)((p.x + 1.0f) * 0.5f * s->cols);

    //               -1..1 -> 2..0 -> 1..0 -> rows..0 
    int screen_y = (int)((1.0f - p.y) * 0.5f * s->rows);

    return new_p2(screen_x, screen_y);
}

void
s_plot(screen *s, point2d p, char c) {
    point2d screen = coords_to_pix(s, p);
    s_put(s, screen.x, screen.y, c);
}


int main(void) {
    enableRawMode();
    
    int rows, cols;
    get_terminal_size(&rows, &cols);

    screen *s = init_screen(rows, cols);

    float time = 0.0f;

    char c;
    clear_screen();

    #define CUBE_R 0.5
    point2d p2d;

    float dt = 1/FPS;
    double delta = 0.1;

    while (1) {
        // listeners
        if (read(STDIN_FILENO, &c, 1) == 1)
            if (c == 17 || c == 'q')
                break; 

        // --- DRAW ---
        s_clear(s);

        for (float i = -CUBE_R; i <= CUBE_R; i += 0.02f) {
          for (float j = -CUBE_R; j <= CUBE_R; j += 0.02f) {

            // Definisco i 6 punti grezzi
            point3d punti[6] = {
                {i, j, CUBE_R},  // Fronte
                {i, j, -CUBE_R}, // Retro
                {CUBE_R, j, i},  // Destra
                {-CUBE_R, j, i}, // Sinistra
                {i, CUBE_R, j},  // Sopra
                {i, -CUBE_R, j}  // Sotto
            };

            // Caratteri diversi per ogni faccia (per distinguerle)
            char chars[6] = {'@', '.', '$', '~', '#', '+'};

            // Processo tutti e 6 i punti in un colpo solo
            for (int k = 0; k < 6; k++) {
              point3d p = punti[k];

              // 1. Ruota
              rotate_x(&p, time);
              rotate_y(&p, time);
              rotate_z(&p, time); // Opzionale
              // 2. Sposta
              p.z += 2.0f;

              float ooz = 1.0f / p.z;
              p2d = normalize(p);

              point2d coords = coords_to_pix(s, p2d);
              int idx = coords.y * cols + coords.x;
              if (ooz > s->zbuf[idx]) {
                s->zbuf[idx] = ooz;
                s_put(s, coords.x, coords.y, chars[k]);
              }
            }
          }
        }

        time += 0.05;
        // end
        printf("\x1b[H"); 
        fputs(s->buff, stdout); 
        fflush(stdout); 
        usleep(33000); 
    }

    free_screen(s);
    return 0;
}
