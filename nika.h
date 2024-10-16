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

#define ITERATIONS 200.0f

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

void nika_swap_f(float *a, float *b);

float nika_length_v3(v3_t a);
float nika_dot_v3(v3_t a, v3_t b);
float nika_distance_v3(v3_t a, v3_t b);

v3_t nika_sum_v3(v3_t a, v3_t b);
v3_t nika_sub_v3(v3_t a, v3_t b);
v3_t nika_mul_v3(v3_t a, v3_t b);
v3_t nika_div_v3(v3_t a, v3_t b);

v3_t nika_sum_v3_scalar(v3_t a, float scalar);
v3_t nika_sub_v_scalar3(v3_t a, float scalar);
v3_t nika_mul_v3_scalar(v3_t a, float scalar);
v3_t nika_div_v3_scalar(v3_t a, float scalar);

v3_t nika_v3_from_color(nika_color_t color);
v3_t nika_v3_normalize(v3_t a);

static float random_float();
static v3_t random_in_unit_sphere();

typedef struct {
    v3_t origin;
    v3_t dir;
} ray_t;

typedef struct {
    nika_color_t albedo;

    float metallic;

    v3_t emission_color;
    float emission_strength;
} nika_material_t;

typedef enum {
    Hit,
    Miss
} ray_result_type_t;

typedef struct {
    ray_result_type_t type;

    v2_t distance;

    v3_t hit_point;
    v3_t hit_normal;
} ray_intersect_result_t;

struct nika_object_t;

typedef struct {
    void* data;
    nika_material_t* material;
    ray_intersect_result_t (*intersect_callback)(ray_t ray, void*);
} nika_object_t;

typedef struct {
    ray_intersect_result_t result;
    nika_object_t* object;
} ray_result_t;

typedef struct {
    v3_t origin;
} nika_camera_t;

typedef struct {
    v3_t origin;
    float radius;
} nika_sphere_data_t;

ray_intersect_result_t nika_sphere_intersect(ray_t ray, void* object);

nika_object_t nika_sphere(v3_t origin, float radius, nika_material_t* material);

void nika_free_object(nika_object_t* object);

typedef struct {
    nika_color_t* data;
    unsigned int width;
    unsigned int height;
} nika_canvas_t;

void nika_clear_canvas(nika_canvas_t* canvas);

ray_result_t nika_trace_ray(ray_t ray, nika_object_t* objects, unsigned int count);

v3_t nika_pow_v3(v3_t a, v3_t b);

v3_t nika_pow_v3_scalar(v3_t a, float b);

nika_color_t nika_per_pixel(nika_camera_t camera, float x, float y, float width, float height, nika_object_t* objects, unsigned int count);

void nika_render_scene(nika_canvas_t* canvas, nika_camera_t camera, nika_object_t* objects, unsigned int count);

#ifdef NIKA_IMPLEMENTATION

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

void nika_swap_f(float *a, float *b) {
    float temp = *a;
    *a = *b;
    *b = temp;
}

ray_intersect_result_t nika_sphere_intersect(ray_t ray, void* object) {
    nika_sphere_data_t* data = (nika_sphere_data_t*) ((nika_object_t*) object)->data; 

    v3_t oc = nika_sub_v3(ray.origin, data->origin);
    
    float a = nika_dot_v3(ray.dir, ray.dir);
    float b = 2.0f * nika_dot_v3(ray.dir, oc);
    float c = nika_dot_v3(oc, oc) - data->radius*data->radius;
    float d = b*b - 4*a*c;

    if(d >= 0.0f) {
        float root = sqrt(d);

        float d0 = (-b + root) / (2.0f * a);
        float d1 = (-b - root) / (2.0f * a);

        if(d0 > d1) nika_swap_f(&d0, &d1);

        if(d0 < 0.0f && d1 < 0.0f)
            return (ray_intersect_result_t){ Miss };
            
        v3_t hit_point = nika_sum_v3(ray.origin, nika_mul_v3_scalar(ray.dir, d0));
        v3_t hit_normal = nika_v3_normalize(nika_sub_v3(hit_point, data->origin));

        return (ray_intersect_result_t) { 
            Hit, 
            (v2_t){ d0, d1 },
            hit_point,
            hit_normal
        };
    }

    return (ray_intersect_result_t){ Miss };
}

nika_object_t nika_sphere(v3_t origin, float radius, nika_material_t* material) {
    nika_sphere_data_t* data = (nika_sphere_data_t*) malloc(sizeof(nika_sphere_data_t));
    data->origin = origin;
    data->radius = radius;

    return (nika_object_t) {
        data,
        material,
        nika_sphere_intersect
    };
}

void nika_free_object(nika_object_t* object) {
    free(object->data);
    free(object);
}

void nika_clear_canvas(nika_canvas_t* canvas) {
    const unsigned int width = canvas->width;
    const unsigned int height = canvas->height;
    
    for(int x = 0; x < width; ++x) {
        for(int y = 0; y < height; ++y) {
            canvas->data[x + width * y] = (nika_color_t){ 0.0f, 0.0f, 0.0f, 1.0f };
        }
    }
}

ray_result_t nika_trace_ray(ray_t ray, nika_object_t* objects, unsigned int count) {
    ray_result_t result;
    result.result.type = Miss;
    result.result.distance.x = FLT_MAX;

    for (unsigned int i = 0; i < count; ++i) {
        nika_object_t* object = &objects[i]; 

        ray_intersect_result_t res = (*object->intersect_callback)(ray, object);

        if (res.type == Miss)
            continue;

        if (res.distance.x > result.result.distance.x)
            continue;

        result.result = res;
        result.object = object;
    }

    return result;
}

v3_t nika_pow_v3(v3_t a, v3_t b) {
    return (v3_t) {
        powf(a.x, b.x),
        powf(a.y, b.y),
        powf(a.z, b.z)
    };
}

v3_t nika_pow_v3_scalar(v3_t a, float b) {
    return (v3_t) {
        powf(a.x, b),
        powf(a.y, b),
        powf(a.z, b)
    };
}

nika_color_t nika_per_pixel(nika_camera_t camera, float x, float y, float width, float height, nika_object_t* objects, unsigned int count) {
    const float aspect_ratio = width / height;

    v3_t ray_dir = (v3_t) {
        (x / width - 0.5f), 
        (y / height - 0.5f) / aspect_ratio, 
        -1.0f
    };

    v3_t rotation = (v3_t) {
        0.0f,
        0.4f,
        0.0f
    };

    ray_dir = nika_v3_normalize(nika_sum_v3(ray_dir, rotation));

    v3_t ray_orig = camera.origin;

    v3_t light = (v3_t) { 0.0f, 0.0f, 0.0f };
    v3_t contribution = (v3_t) { 1.0f, 1.0f, 1.0f };

    for(unsigned int b = 0; b < 5; ++b) {
        ray_t ray = (ray_t) { ray_orig, ray_dir };

        ray_result_t result = nika_trace_ray(ray, objects, count);

        if(result.result.type == Miss) {
            v3_t sky_color = (v3_t) { 0.6f,  0.7f,  0.8f };
            light = nika_sum_v3(light, nika_mul_v3(sky_color, contribution));
            break;
        }

        nika_material_t* material = result.object->material;

        contribution = nika_mul_v3(contribution, nika_v3_from_color(material->albedo));
        light = nika_sum_v3(light, nika_mul_v3_scalar(material->emission_color, material->emission_strength));

        v3_t reflection = nika_sub_v3(ray_dir, nika_mul_v3_scalar(nika_mul_v3_scalar(result.result.hit_normal, nika_dot_v3(ray_dir, result.result.hit_normal)), 2.0f)); 
        ray_dir = nika_v3_normalize(nika_sum_v3(reflection, nika_mul_v3_scalar(random_in_unit_sphere(), material->metallic)));
        ray_orig = nika_sum_v3(result.result.hit_point, nika_mul_v3_scalar(result.result.hit_normal, 0.001f)); 
    }
    
    // reinhard tone mapping
    light = nika_div_v3(light, nika_sum_v3(light, (v3_t){ 1.0f, 1.0f, 1.0f}));
    // gamma correction 
    float gamma = 2.2f;
    light = nika_pow_v3_scalar(light, 1.0 / gamma);

    return (nika_color_t){ light.x, light.y, light.z, 1.0f };
}

void nika_render_scene(nika_canvas_t* canvas, nika_camera_t camera, nika_object_t* objects, unsigned int count) {
    const unsigned int width = canvas->width;
    const unsigned int height = canvas->height;

    for(int i = 0; i < ITERATIONS; ++i) {
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
            canvas->data[x + width * y].r /= ITERATIONS;
            canvas->data[x + width * y].g /= ITERATIONS;
            canvas->data[x + width * y].b /= ITERATIONS;
            canvas->data[x + width * y].a /= ITERATIONS;
        }
    }
};

#endif

#endif