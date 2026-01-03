
#include <math.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

#include "geometry.h"
#include "tui.h"


void draw_cube(
    screen *s,
    float   side,
    vec3    offset,
    vec3    rotation
);

void draw_donut(   
    screen *s,
    double  r1,
    double  r2,
    vec3    offset,
    vec3    rotation
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

        draw_cube (s, 0.3,
                   (vec3){ 0.6, 0, 1}, (vec3){time, 0, time});
        draw_donut(s, 0.08, 0.4,
                   (vec3){-0.6, 0, 1}, (vec3){time, 0, time});
        
        // draw_donut(s, 0.1, 0.5, (vec3){0, 0, 1}, (vec3){time, 0, time});
        
        // draw_cube (s, 0.3f,     (vec3){0, 0, 1}, (vec3){time, 0, time});

        // end
        time += 0.1f;
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
    double  r1,
    double  r2,
    vec3    offset,
    vec3    rotation
) {
    vec3 lum = vec3_normalized((vec3){0, 1, -1});
    
    char  chars[12] = ".,-~:;=!*#$@";
    int   charidx;
    
    for (double phi = 0; phi < 2*M_PI; phi+=0.02) {
        for (double theta = 0; theta < 2*M_PI; theta+=0.02) {
            // calcola l'inclinazione del punto
            // poi lo sposta dal centro di r2
            float width = r1*cos(theta) + r2;
            
            vec3 v = (vec3) {
                width*cos(phi),
                   r1*sin(theta),
                width*sin(phi)
            };
            vec3 normal = (vec3) {
                cos(theta)*cos(phi),
                sin(theta),
                cos(theta)*sin(phi)
            };

            // apply given rotation
            v      = vec3_rotated(v, rotation);
            normal = vec3_rotated(normal, rotation);

            // add given offsett
            v = vec3_add(v, offset);

            double prod = vec3_dot_product(normal, lum);

            int charidx = 0;
            if (prod > 0)
                charidx = (int)(prod * 11.0);

            if (charidx > 11) charidx = 11;
            if (charidx < 0)  charidx = 0;

            s_plot(s, v, chars[charidx]);
        }
    }
}

void draw_cube(
    screen *s,
    float   side,
    vec3    offset,
    vec3    rotation
) {
    char chars[6] = "@.=~#+";
    float sizeop = -1*side;

    for (float i = sizeop; i <= side; i += 0.02f) {
        for (float j = sizeop; j <= side; j += 0.02f) {

            vec3 punti[6] = {
                {i, j, side},   // Fronte
                {i, j, sizeop}, // Retro
                {side, j, i},   // Destra
                {sizeop, j, i}, // Sinistra
                {i, side, j},   // Sopra
                {i, sizeop, j}  // Sotto
            };

            // for each face draws a point.
            for (int k = 0; k < 6; k++) {
                vec3 v = punti[k];

                v = vec3_rotated(v, rotation);
                v = vec3_add(v, offset);

                s_plot(s, v, chars[k]);
            }
        }
    }
}
