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


void nika_clear_canvas(NikaCanvas* canvas) {
    const unsigned int width = canvas->width;
    const unsigned int height = canvas->height;
    
    for(int x = 0; x < width; ++x) {
        for(int y = 0; y < height; ++y) {
            canvas->data[x + width * y] = (nika_color_t){ 0.0f, 0.0f, 0.0f, 1.0f };
        }
    }
}

typedef enum {
    Hit,
    Miss
} ray_result_type_t;

typedef struct {
    ray_result_type_t type;
    v2_t point;
    NikaSphere sphere;
} ray_hit_result_t;

typedef union {
    ray_result_type_t type;
    ray_hit_result_t hit;
} ray_result_t;

typedef struct {
    v3_t origin;
} NikaCamera;

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

ray_result_t nika_trace_ray(ray_t ray, NikaSphere* objects, unsigned int count) {
    float depth = FLT_MAX;

    ray_result_t result;
    result.type = Miss;

    for(unsigned int i = 0; i < count; ++i) {
        NikaSphere sphere = objects[i]; 
        const v2_t t = sphere_intersect(ray.origin, ray.dir, sphere.origin, sphere.radius); 

        if (t.y < 0.0) { 
            continue;
           //  return (nika_color_t){ 0.0f, 0.0f, 0.0f, 0.0f }; 
        } else if (t.x < 0.0) {
            continue;
            // return (nika_color_t){ 0.0f, 1.0f, 0.0f, 0.0f }; 
        }

        if (t.x > depth)
            continue;

        result.hit = (ray_hit_result_t) {
            .type = Hit,
            .point = t,
            .sphere = sphere
        };

        depth = t.x;
    }

    return result;
}

nika_color_t nika_per_pixel(NikaCamera camera, float x, float y, float width, float height, NikaSphere* objects, unsigned int count) {
    const float aspect_ratio = width / height;

    v3_t ray_dir = (v3_t) {
        (x / width - 0.5f), 
        (y / height - 0.5f) / aspect_ratio, 
        -1.0f
    };

    v3_t ray_orig = camera.origin;

    v3_t light = (v3_t) { 0.0f, 0.0f, 0.0f };
    v3_t contribution = (v3_t) { 1.0f, 1.0f, 1.0f };

    for(int b = 0; b < 5; ++b) {
        ray_t ray = (ray_t) { ray_orig, ray_dir };

        ray_result_t result = nika_trace_ray(ray, objects, count);

        if(result.type == Miss) {
            v3_t sky_color = (v3_t) { 1.0f,  1.0f,  1.0f };
            light = nika_sum_v3(light, nika_mul_v3(sky_color, contribution));
            break;
        }

        NikaMaterial* material = result.hit.sphere.material;

        contribution = nika_mul_v3(contribution, nika_v3_from_color(material->albedo));

        float distance = result.hit.point.x;
        v3_t hit_point = nika_sum_v3(ray_orig, nika_mul_v3_scalar(ray_dir, distance));
        v3_t normal = nika_v3_normalize(nika_sub_v3(hit_point, result.hit.sphere.origin)); 

        ray_orig = nika_sum_v3(hit_point, nika_mul_v3_scalar(normal, 0.001f)); 

        v3_t reflection = nika_sub_v3(ray_dir, nika_mul_v3_scalar(nika_mul_v3_scalar(normal, nika_dot_v3(ray_dir, normal)), 2.0f)); 

        ray_dir = nika_v3_normalize(nika_sum_v3(reflection, nika_mul_v3_scalar(random_in_unit_sphere(), material->metallic)));
    }

    return (nika_color_t){ light.x, light.y, light.z, 1.0f };
}

void nika_render_scene(NikaCanvas* canvas, NikaCamera camera, NikaSphere* objects, unsigned int count) {
    const unsigned int width = canvas->width;
    const unsigned int height = canvas->height;
    
    for(int i = 0; i < 10; ++i) {
        for(int x = 0; x < width; ++x) {
            for(int y = 0; y < height; ++y) {
                nika_color_t color = nika_per_pixel(camera, x, y, width, height, objects, count);

                canvas->data[x + width * y].r += color.r;
                canvas->data[x + width * y].g += color.g;
                canvas->data[x + width * y].b += color.b;
                canvas->data[x + width * y].a += color.a;
            }
        }
    }

    for(int x = 0; x < width; ++x) {
        for(int y = 0; y < height; ++y) {
            canvas->data[x + width * y].r /= 10.0f;
            canvas->data[x + width * y].g /= 10.0f;
            canvas->data[x + width * y].b /= 10.0f;
            canvas->data[x + width * y].a /= 10.0f;
        }
    }
};

#endif