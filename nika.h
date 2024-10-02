#ifndef NIKA_H
#define NIKA_H

#ifndef NIKA_SQRT
    #include <math.h>
    #define NIKA_SQRT sqrt
#endif

#define FLT_MAX 3.402823466e+38F
#define FLT_MIN 1.175494351e-38F

typedef struct {
    float x;
    float y;
} v2_t;

typedef struct {
    float x;
    float y;
    float z;
} v3_t;

typedef struct {
    v3_t origin;
    v3_t dir;
} ray_t;

float nika_dot(v3_t a, v3_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

typedef struct {
    float r;
    float g;
    float b;
    float a;
} nika_color_t;

typedef struct {
    nika_color_t color;
} NikaMaterial;

typedef struct {
    v3_t origin;
    float radius;
    NikaMaterial* material;
} NikaSphere;

typedef struct {
    nika_color_t* data;
    unsigned int width;
    unsigned int height;
} NikaCanvas;

v2_t sphere_intersect(v3_t ro, v3_t rd, v3_t ce, float ra) {
    const v3_t oc = { ro.x - ce.x, ro.y - ce.y, ro.z - ce.z };
    float b = nika_dot(oc, rd);

    v3_t qc = (v3_t) {
        oc.x - b*rd.x,
        oc.y - b*rd.y,
        oc.z - b*rd.z
    };

    float h = ra*ra - nika_dot( qc, qc );
    
    if(h < 0.0) return (v2_t){-1.0, -1.0}; // no intersection
    
    h = NIKA_SQRT( h );
    
    return (v2_t){ -b-h, -b+h };

}

#endif