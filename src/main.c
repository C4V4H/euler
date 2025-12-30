
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

static inline float
dot_product(point3d p1, point3d p2) {
    return p1.x*p2.x + p1.y*p2.y + p1.z*p2.z;
}
static inline point3d
vec3_sum(point3d p1, point3d p2) {
    return new_p3(p1.x+p2.x, p1.y+p2.y, p1.z+p2.z);
}

void normalize_vec(point3d *v) {
    float len = sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
    if (len == 0) return; // Evita divisione per zero
    v->x /= len;
    v->y /= len;
    v->z /= len;
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

void draw_cube(
    screen *s,
    float   side,
    float   time,
    point3d offset
);

void draw_donut(   
    screen *s,
    float   maxrad,
    float   minrad,
    float   time,
    point3d offset
);

int main(void) {
    enableRawMode();
    
    int rows, cols;
    get_terminal_size(&rows, &cols);

    screen *s = init_screen(rows, cols);

    float time = 0.0f;

    char c;
    clear_screen();


    while (1) {
        // listeners
        if (read(STDIN_FILENO, &c, 1) == 1){
            if (c == 17 || c == 'q')
                break; 
        }

        // --- DRAW ---
        s_clear(s);

        // draw_cube (s, 0.2f, time, new_p3(0.5, 0, 1));
        // draw_donut(s, 0.08f, 0.25f, time, new_p3(-0.5, 0, 1));

        draw_donut(s, 0.1f, 0.5f, time, new_p3(0, 0, 1));
        
        // draw_cube (s, 0.3f, time, new_p3(0, 0, 1));
        
        // end
        time += 0.05f;
        printf("\x1b[H"); 
        fputs(s->buff, stdout); 
        fflush(stdout); 
        usleep(33000); 
    }

    clear_screen();
    free_screen(s);
    return 0;
}



void draw_donut(   
    screen *s,
    float   r1,
    float   r2,
    float   time,
    point3d offset
) {
    point3d p, normal, lum = new_p3(0, 1, -1);

    normalize_vec(&lum);
    
    char chars[12] = ".,-~:;=!*#$@";
    float ooz;
    point2d coords;
    int idx, charidx;
    
    for (double phi = 0; phi < 2*M_PI; phi+=0.02) {
        for (double theta = 0; theta < 2*M_PI; theta+=0.02) {
            // calcola l'inclinazione del punto
            // poi lo sposta dal centro di r2
            float width = r1*cos(theta) + r2;
            
            p = new_p3(
                width*cos(phi),
                   r1*sin(theta),
                width*sin(phi)
            );
            normal = new_p3(
                cos(theta)*cos(phi),
                sin(theta),
                cos(theta)*sin(phi)
            );

            

            rotate_x(&p, time);
            rotate_x(&normal, time);
            // rotate_y(&p, time);            
            // rotate_y(&normal, time);            
            rotate_z(&p, time);
            rotate_z(&normal, time);

            p = vec3_sum(p, offset);

            float prod = dot_product(normal, lum);

            if (prod > 0) {
                charidx = (int)(prod * 12);
                
                if (charidx > 12) charidx = 12;
                if (charidx < 0)  charidx = 0;
                
                ooz = 1.0f / p.z;
                coords = coords_to_pix(s, normalize(p));
                idx = coords.y * s->cols + coords.x;

                if (ooz > s->zbuf[idx]) {
                    s->zbuf[idx] = ooz;
                    s_put(s, coords.x, coords.y, chars[charidx]);
                }
            }
        }
    }
}

void draw_cube(
    screen *s,
    float   side,
    float   time,
    point3d offset
) {
    char chars[6] = {'@', '.', '$', '~', '#', '+'};
    float ooz;
    point2d p2d, coords;
    int idx;

    float sizeop = -1*side;

    for (float i = sizeop; i <= side; i += 0.02f) {
      for (float j = sizeop; j <= side; j += 0.02f) {

        // Definisco i 6 punti grezzi
        point3d punti[6] = {
            {i, j, side},  // Fronte
            {i, j, sizeop}, // Retro
            {side, j, i},  // Destra
            {sizeop, j, i}, // Sinistra
            {i, side, j},  // Sopra
            {i, sizeop, j}  // Sotto
        };

        // for each face draws a point.
        for (int k = 0; k < 6; k++) {
          point3d p = punti[k];

          rotate_x(&p, time);
          rotate_y(&p, time);
          rotate_z(&p, time);

          p = vec3_sum(p, offset);

          ooz = 1.0f / p.z;
          p2d = normalize(p);

          coords = coords_to_pix(s, p2d);
          idx = coords.y * s->cols + coords.x;

          if (ooz > s->zbuf[idx]) {
            s->zbuf[idx] = ooz;
            s_put(s, coords.x, coords.y, chars[k]);
          }
        }
      }
    }
}
