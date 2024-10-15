#include <SDL2/SDL.h>
#include <printf.h>

#include "nika.h"

typedef nika_color_t color_t; 

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

    nika_canvas_t canvas = (nika_canvas_t) {
        .data = (nika_color_t*) malloc(sizeof(nika_color_t) * 800 * 600),
        .width = 800,
        .height = 600
    };

    nika_material_t red = (nika_material_t) {
        .albedo = (nika_color_t){ 0.8f, 0.4f, 0.4f, 1.0f },
        .metallic = 0.1f,
        .emission_strength = 0.0f,
    };

    nika_material_t green = (nika_material_t) {
        .albedo = (nika_color_t){ 0.4f, 0.8f, 0.4f, 1.0f },
        .metallic = 0.02f,
        .emission_strength = 0.0f,
    };
    
    nika_material_t blue = (nika_material_t) {
        .albedo = (nika_color_t){ 0.4f, 0.4f, 0.8f, 1.0f },
        .metallic = 0.02f,
        .emission_strength = 0.0f
    };

    nika_material_t floor = (nika_material_t) {
        .albedo = (nika_color_t){ 0.2f, 0.2f, 0.2f, 1.0f },
        .metallic = 0.99f,
        .emission_strength = 0.0f
    };

    nika_material_t sun = (nika_material_t) {
        .albedo = (nika_color_t){ 1.0f, 0.6f, 0.2f, 1.0f },
        .metallic = 0.99f,

        .emission_color = (nika_color_t){ 1.0f, 0.6f, 0.2f, 1.0f },
        .emission_strength = 1.0f
    };

    nika_object_t objects[] = {
        nika_sphere((v3_t){ 0.0f, 0.0f, -7.0f }, 1.0f, &red),
        nika_sphere((v3_t){ 1.0f, 0.0f, -5.0f }, 0.8f, &green),
        nika_sphere((v3_t){ -1.0f, 0.0f, -5.0f }, 0.8f, &blue),
        nika_sphere((v3_t){ 0.0f, 400.5f, -5.0f }, 400.0f, &floor),
        nika_sphere((v3_t){ -0.8f, 0.3f, -3.5f }, 0.2f, &sun),
    };

    nika_camera_t camera = (nika_camera_t) {
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
            nika_render_scene(&canvas, camera, objects, 5);

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