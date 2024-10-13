#include <stdlib.h>
#include <math.h>

#include "ppm.h"

#include "nika.h"

int main() {
    NikaCanvas canvas = (NikaCanvas) {
        .data = (nika_color_t*) malloc(sizeof(nika_color_t) * 800 * 600),
        .width = 800,
        .height = 600
    };

    NikaMaterial red = (NikaMaterial) {
        .albedo = (nika_color_t){ 0.8f, 0.4f, 0.4f, 1.0f },
        .metallic = 0.1f
    };

    NikaMaterial green = (NikaMaterial) {
        .albedo = (nika_color_t){ 0.4f, 0.8f, 0.4f, 1.0f },
        .metallic = 0.02f
    };
    
    NikaMaterial blue = (NikaMaterial) {
        .albedo = (nika_color_t){ 0.4f, 0.4f, 0.8f, 1.0f },
        .metallic = 0.02f
    };

    NikaMaterial floor = (NikaMaterial) {
        .albedo = (nika_color_t){ 0.2f, 0.2f, 0.2f, 1.0f },
        .metallic = 0.99f
    };

    NikaSphere objects[] = {
        (NikaSphere){ (v3_t){ 0.0f, 0.0f, -7.0f }, 1.0f, &red},
        (NikaSphere){ (v3_t){ 1.0f, 0.0f, -5.0f }, 0.8f, &green},
        (NikaSphere){ (v3_t){ -1.0f, 0.0f, -5.0f }, 0.8f, &blue},
        (NikaSphere){ (v3_t){ 0.0f, 400.5f, -5.0f }, 400.0f, &floor},
    }; 

    NikaCamera camera = (NikaCamera) {
        .origin = (v3_t){ 0.0f, 0.0f, 0.0f }
    };

    nika_render_scene(&canvas, camera, objects, 4);
        
    ppm_export_image("render.ppm", 800, 600, RGBA32F, canvas.data);

    free(canvas.data);

    return 0;
}


