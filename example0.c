#include <stdlib.h>
#include <math.h>

#include "ppm.h"

#include "nika.h"

typedef struct {
    float x;
    float y;
    float z;
} v3_t;

typedef struct {
    float x;
    float y;
} v2_t;

typedef struct {
    v3_t origin;
    v3_t dir;
} ray_t;

float dot(v3_t a, v3_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

v2_t sphere_intersect(v3_t ro, v3_t rd, v3_t ce, float ra) {
    const v3_t oc = { ro.x - ce.x, ro.y - ce.y, ro.z - ce.z };
    float b = dot(oc, rd);

    v3_t qc = (v3_t) {
        oc.x - b*rd.x,
        oc.y - b*rd.y,
        oc.z - b*rd.z
    };

    float h = ra*ra - dot( qc, qc );
    
    if( h<0.0 ) return (v2_t){-1.0, -1.0}; // no intersection
    
    h = sqrt( h );
    
    return (v2_t){ -b-h, -b+h };

}

unsigned int nika_per_pixel(float x, float y, float width, float height) {
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

    v3_t orig = (v3_t) {
        0.0f,
        0.0f,
        -5.0f
    }; 

    const v2_t t = sphere_intersect(ray_orig, ray_dir, orig, 1.0f); 

    if(t.y < 0.0) { 
        return 0x00000000;
    } else if(t.x < 0.0) {
        return 0xff000000; 
    } else { 
        return 0x00ff0000; 
    }
}

int main() {
    color_rgba_t* canvas = malloc(sizeof(color_rgba_t) * 800 * 600);

    for(int x = 0; x < 800; ++x) {
        for(int y = 0; y < 600; ++y) {
            *((unsigned int*)&(canvas[x + 800 * y])) = nika_per_pixel(x, y, 800.0f, 600.0f);

            // if(x % 2 == 0) {
            //     *((unsigned int*)&(canvas[x + 800 * y])) = 0xFFFFFFFF; 
            // } else 
            //     *((unsigned int*)&(canvas[x + 800 * y])) = 0x0;
        }
    }
        
    ppm_export_image("render.ppm", 800, 600, canvas);

    free(canvas);

    return 0;
}


