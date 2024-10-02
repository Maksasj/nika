#include <stdlib.h>
#include <math.h>

#include "ppm.h"

#include "nika.h"

typedef nika_color_t color_t; 

typedef enum {
    Hit,
    Miss
} ray_result_type_t;

typedef struct {
    ray_result_type_t type;
    v3_t point;
    nika_color_t color;
} ray_hit_result_t;

typedef union {
    ray_result_type_t type;
    ray_hit_result_t hit;
} ray_result_t;

ray_result_t nika_trace_ray(ray_t ray, NikaSphere* objects, unsigned int count) {
    float depth = FLT_MAX;

    ray_result_t result;
    result.type = Miss;

    for(unsigned int i = 0; i < count; ++i) {
        NikaSphere sphere = objects[i]; 
        const v2_t t = sphere_intersect(ray.origin, ray.dir, sphere.origin, sphere.radius); 

        if(t.y < 0.0) { 
            continue;
           //  return (color_t){ 0.0f, 0.0f, 0.0f, 0.0f }; 
        } else if(t.x < 0.0) {
            continue;
            // return (color_t){ 0.0f, 1.0f, 0.0f, 0.0f }; 
        }

        if(t.x > depth)
            continue;

        result.hit = (ray_hit_result_t) {
            .type = Hit,
            .point = (v3_t){ 0.0f, 0.0f, 0.0f },
            .color = sphere.material->color
        };

        depth = t.x;
    }

    return result;
}

color_t nika_per_pixel(float x, float y, float width, float height, NikaSphere* objects, unsigned int count) {
    const float aspect_ratio = width / height;

    v3_t ray_dir = (v3_t) {
        (x / width - 0.5f), 
        (y / height - 0.5f) / aspect_ratio, 
        -1.0f
    };

    v3_t ray_orig = (v3_t) {
        0.0f,
        0.0f,
        0.0f
    };

    ray_result_t result = nika_trace_ray((ray_t) {
        ray_orig,
        ray_dir
    }, objects, count);

    if(!result.type == Miss) {
        return result.hit.color;
    }

    return (nika_color_t){ 0.0f, 0.0f, 0.0f, 0.0f };
}

void nika_render_scene(NikaCanvas* canvas, NikaSphere* objects, unsigned int count) {
    const unsigned int width = canvas->width;
    const unsigned int height = canvas->height;
    
    for(int x = 0; x < width; ++x) {
        for(int y = 0; y < height; ++y) {
            canvas->data[x + width * y] = nika_per_pixel(x, y, width, height, objects, count);
        }
    }
};

int main() {
    NikaCanvas canvas = (NikaCanvas) {
        .data = (nika_color_t*) malloc(sizeof(nika_color_t) * 800 * 600),
        .width = 800,
        .height = 600
    };

    NikaMaterial red = (NikaMaterial) {
        .color = (nika_color_t){ 1.0f, 0.0f, 0.0f, 1.0f }
    };

    NikaMaterial green = (NikaMaterial) {
        .color = (nika_color_t){ 0.0f, 1.0f, 0.0f, 1.0f }
    };
    
    NikaMaterial blue = (NikaMaterial) {
        .color = (nika_color_t){ 0.0f, 0.0f, 1.0f, 1.0f }
    };

    NikaSphere objects[] = {
        (NikaSphere){ (v3_t){ 0.0f, 0.0f, -5.0f }, 1.0f, &red},
        (NikaSphere){ (v3_t){ 1.0f, 0.0f, -3.0f }, 0.8f, &green},
        (NikaSphere){ (v3_t){ -1.0f, 0.0f, -3.0f }, 0.8f, &blue},
    }; 

    nika_render_scene(&canvas, objects, 3);
        
    ppm_export_image("render.ppm", 800, 600, RGBA32F, canvas.data);

    free(canvas.data);

    return 0;
}


