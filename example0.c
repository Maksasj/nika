#include <stdlib.h>
#include <math.h>

#include "ppm.h"

#include "nika.h"

int main() {
    nika_canvas_t canvas = (nika_canvas_t) {
        .data = (nika_color_t*) malloc(sizeof(nika_color_t) * 800 * 600),
        .width = 800,
        .height = 600
    };

    nika_material_t red = (nika_material_t) {
        .albedo = (nika_color_t){ 0.8f, 0.4f, 0.4f, 1.0f },
        .metallic = 0.1f
    };

    nika_material_t green = (nika_material_t) {
        .albedo = (nika_color_t){ 0.4f, 0.8f, 0.4f, 1.0f },
        .metallic = 0.02f
    };
    
    nika_material_t blue = (nika_material_t) {
        .albedo = (nika_color_t){ 0.4f, 0.4f, 0.8f, 1.0f },
        .metallic = 0.02f
    };

    nika_material_t floor = (nika_material_t) {
        .albedo = (nika_color_t){ 0.2f, 0.2f, 0.2f, 1.0f },
        .metallic = 0.99f
    };

    nika_object_t objects[] = {
        nika_sphere((v3_t){ 0.0f, 0.0f, -7.0f }, 1.0f, &red),
        nika_sphere((v3_t){ 1.0f, 0.0f, -5.0f }, 0.8f, &green),
        nika_sphere((v3_t){ -1.0f, 0.0f, -5.0f }, 0.8f, &blue),
        nika_sphere((v3_t){ 0.0f, 400.5f, -5.0f }, 400.0f, &floor)
    };

    nika_camera_t camera = (nika_camera_t) {
        .origin = (v3_t){ 0.0f, 0.0f, 0.0f }
    };

    nika_render_scene(&canvas, camera, objects, 4);
        
    ppm_export_image("render.ppm", 800, 600, RGBA32F, canvas.data);

    free(canvas.data);

    return 0;
}


