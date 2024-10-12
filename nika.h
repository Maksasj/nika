#ifndef NIKA_H
#define NIKA_H

#ifndef NIKA_SQRT
    #include <math.h>
    #define NIKA_SQRT sqrt
#endif

#ifndef FLT_MAX
    #define FLT_MAX 3.402823466e+38F
#endif

#ifndef FLT_MIN
    #define FLT_MIN 1.175494351e-38F
#endif

typedef struct {
    float r;
    float g;
    float b;
    float a;
} nika_color_t;

typedef struct {
    float x;
    float y;
} v2_t;

typedef struct {
    float x;
    float y;
    float z;
} v3_t;

float nika_length_v3(v3_t a) {
    return NIKA_SQRT(a.x * a.x + a.y * a.y + a.z * a.z);
}

float nika_dot_v3(v3_t a, v3_t b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

float nika_distance_v3(v3_t a, v3_t b) {
    const float x = a.x - b.x;
    const float y = a.y - b.y;
    const float z = a.z - b.z;

    return NIKA_SQRT((x*x)+(y*y)+(z*z));
}

v3_t nika_sum_v3(v3_t a, v3_t b) {
    return (v3_t) {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
}

v3_t nika_sub_v3(v3_t a, v3_t b) {
    return (v3_t) {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}

v3_t nika_mul_v3(v3_t a, v3_t b) {
    return (v3_t) {
        a.x * b.x,
        a.y * b.y,
        a.z * b.z
    };
}

v3_t nika_div_v3(v3_t a, v3_t b) {
    return (v3_t) {
        a.x / b.x,
        a.y / b.y,
        a.z / b.z
    };
}


v3_t nika_sum_v3_scalar(v3_t a, float scalar) {
    return (v3_t) {
        a.x + scalar,
        a.y + scalar,
        a.z + scalar
    };
}

v3_t nika_sub_v_scalar3(v3_t a, float scalar) {
    return (v3_t) {
        a.x - scalar,
        a.y - scalar,
        a.z - scalar
    };
}

v3_t nika_mul_v3_scalar(v3_t a, float scalar) {
    return (v3_t) {
        a.x * scalar,
        a.y * scalar,
        a.z * scalar
    };
}

v3_t nika_div_v3_scalar(v3_t a, float scalar) {
    return (v3_t) {
        a.x / scalar,
        a.y / scalar,
        a.z / scalar
    };
}

v3_t nika_v3_from_color(nika_color_t color) {
    return (v3_t) {
        color.r,
        color.g,
        color.b
    };
}

v3_t nika_v3_normalize(v3_t a) {
    const float length = nika_length_v3(a);

    return (v3_t) {
        a.x / length,
        a.y / length,
        a.z / length
    };
}


static float random_float() {
    return (float) (rand()) / (float) (RAND_MAX);
}

static v3_t random_in_unit_sphere() {
    const float x = random_float() * 2.0f - 1.0f;
    const float y = random_float() * 2.0f - 1.0f;
    const float z = random_float() * 2.0f - 1.0f;

    return nika_v3_normalize((v3_t){x, y, z});
}

typedef struct {
    v3_t origin;
    v3_t dir;
} ray_t;

typedef struct {
    nika_color_t albedo;
    float metallic;
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
    const v3_t oc = nika_sub_v3(ro, ce);
    float b = nika_dot_v3(oc, rd);

    v3_t qc = (v3_t) {
        oc.x - b*rd.x,
        oc.y - b*rd.y,
        oc.z - b*rd.z
    };

    float h = ra*ra - nika_dot_v3(qc, qc);
    
    if(h < 0.0) return (v2_t){ -1.0, -1.0 }; // no intersection
    
    h = NIKA_SQRT(h);
    
    return (v2_t){ -b-h, -b + h };

}

#endif