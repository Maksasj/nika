#include <SDL2/SDL.h>
#include <printf.h>

#include "nika.h"


typedef nika_color_t color_t; 

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

ray_result_t nika_trace_ray(ray_t ray, NikaSphere* objects, unsigned int count) {
    float depth = FLT_MAX;

    ray_result_t result;
    result.type = Miss;

    for(unsigned int i = 0; i < count; ++i) {
        NikaSphere sphere = objects[i]; 
        const v2_t t = sphere_intersect(ray.origin, ray.dir, sphere.origin, sphere.radius); 

        if (t.y < 0.0) { 
            continue;
           //  return (color_t){ 0.0f, 0.0f, 0.0f, 0.0f }; 
        } else if (t.x < 0.0) {
            continue;
            // return (color_t){ 0.0f, 1.0f, 0.0f, 0.0f }; 
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

typedef struct {
    v3_t origin;
} NikaCamera;

color_t nika_per_pixel(NikaCamera camera, float x, float y, float width, float height, NikaSphere* objects, unsigned int count) {
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

void nika_clear_canvas(NikaCanvas* canvas) {
    const unsigned int width = canvas->width;
    const unsigned int height = canvas->height;
    
    for(int x = 0; x < width; ++x) {
        for(int y = 0; y < height; ++y) {
            canvas->data[x + width * y] = (nika_color_t){ 0.0f, 0.0f, 0.0f, 1.0f };
        }
    }
}

void nika_render_scene(NikaCanvas* canvas, NikaCamera camera, NikaSphere* objects, unsigned int count) {
    const unsigned int width = canvas->width;
    const unsigned int height = canvas->height;
    
    for(int i = 0; i < 1; ++i) {
        for(int x = 0; x < width; ++x) {
            for(int y = 0; y < height; ++y) {
                color_t color = nika_per_pixel(camera, x, y, width, height, objects, count);

                canvas->data[x + width * y].r += color.r;
                canvas->data[x + width * y].g += color.g;
                canvas->data[x + width * y].b += color.b;
                canvas->data[x + width * y].a += color.a;
            }
        }
    }

    for(int x = 0; x < width; ++x) {
        for(int y = 0; y < height; ++y) {
            canvas->data[x + width * y].r /= 1.0f;
            canvas->data[x + width * y].g /= 1.0f;
            canvas->data[x + width * y].b /= 1.0f;
            canvas->data[x + width * y].a /= 1.0f;
        }
    }
};

int main(int argc, char *argv[]) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize the SDL2 library\n");
        printf("SDL2 Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("Nika", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);

    if(!window) {
        printf("Failed to create window\n");
        printf("SDL2 Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Surface *window_surface = SDL_GetWindowSurface(window);

    if(!window_surface) {
        printf("Failed to get the surface from the window\n");
        printf("SDL2 Error: %s\n", SDL_GetError());
        return -1;
    }
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_SOFTWARE);
    SDL_Texture *t = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 800, 600);

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

    printf("Started application\n");

    char keep_window_open = 1;
    while(keep_window_open) {
        SDL_Event e;

        while(SDL_PollEvent(&e) > 0) {
            switch(e.type) {
                case SDL_QUIT:
                    keep_window_open = 0;
                    break;
                case SDL_KEYDOWN:
                    if(e.key.keysym.sym == 97) {
                        camera.origin.z += 0.2f;
                    }

                    if(e.key.keysym.sym == 100) {
                        camera.origin.z -= 0.2f;
                    }

                    if(e.key.keysym.sym == 119) {
                        camera.origin.x += 0.2f;
                    }
                    
                    if(e.key.keysym.sym == 115) {
                        camera.origin.x -= 0.2f;
                    }

                    break;
                case SDL_KEYUP:
                    break;
                default:
                    break;
            }

            SDL_UpdateWindowSurface(window);
        }

        {   
            nika_clear_canvas(&canvas);
            nika_render_scene(&canvas, camera, objects, 4);

            int pitch, w, h;
            void* pixels;

            SDL_LockTexture(t, NULL, &pixels, &pitch);
            unsigned int* upixels = (Uint32*) pixels;

            for(int y = 0; y < 600; ++y) {
                for(int x = 0; x < 800; ++x) {
                    color_t pixel = ((color_t*) canvas.data)[x + 800 * y];

                    unsigned char r = (unsigned char) (pixel.r * 255.0f);
                    unsigned char g = (unsigned char) (pixel.g * 255.0f);
                    unsigned char b = (unsigned char) (pixel.b * 255.0f);

                    upixels[x + 800 * y] = r | g << 8 | b << 16;
                }
            }

            SDL_UnlockTexture(t);
        }

        {
            int pitch, w, h;
            void *pixels;

            SDL_LockTexture(t, NULL, &pixels, &pitch);
            memcpy(pixels, pixels, (pitch / 4) * h);
            SDL_UnlockTexture(t);

            SDL_Rect texture_rect;
            texture_rect.x = 0; //the x coordinate
            texture_rect.y = 0; //the y coordinate
            texture_rect.w = 800; //the width of the texture
            texture_rect.h = 600; //the height of the texture

            SDL_RenderClear(renderer); //clears the renderer
            SDL_RenderCopy(renderer, t, NULL, &texture_rect); 
            SDL_RenderPresent(renderer); //updates the renderer
        }
    }

    free(canvas.data);

    return 0;
}