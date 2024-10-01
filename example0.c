#include <stdlib.h>

#include "ppm.h"

#include "nika.h"

int main() {
    color_rgba_t* canvas = malloc(sizeof(color_rgba_t) * 800 * 600);
    
    for(int y = 0; y < 600; ++y) {
        for(int x = 0; x < 800; ++x) {
            *((unsigned int*)&(canvas[x + 800 * y])) = 0xFFFF00FF;
        }
    }

    ppm_export_image("render.ppm", 800, 600, canvas);

    free(canvas);

    return 0;
}


