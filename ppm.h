#ifndef PPM_H
#define PPM_H

#include <stdio.h>

#define PPM_INLINE static inline

typedef enum {
    R8G8B8Uint,
    R8G8B8A8Uint,
    R32G32B32A32Float,

    RGB24 = R8G8B8Uint,
    RGBA32 = R8G8B8A8Uint,
    RGBA32F = R32G32B32A32Float
} color_format_t;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} ppm_color_rgb24_t;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} ppm_color_rgba32_t;

typedef struct {
    float r;
    float g;
    float b;
    float a;
} ppm_color_rgba32f_t;

PPM_INLINE void ppm_export_image(const char* file_name, int width, int height, color_format_t format, void* data) {
    FILE* fptr = fopen(file_name, "w");

    if (fptr == NULL) {
        printf("The file is not opened. The program will now exit.");
        exit(0);
    }

    fprintf(fptr, "P6\n");
    fprintf(fptr, "# created by Eric R. Weeks using a C program\n");
    fprintf(fptr, "%d %d\n", width, height);
    fprintf(fptr, "255\n");
    
    if(format == R8G8B8Uint) {
        for(int y = 0; y < height; ++y) {
            for(int x = 0; x < width; ++x) {
                ppm_color_rgb24_t pixel = ((ppm_color_rgb24_t*) data)[x + width * y];

                fputc((unsigned char) pixel.r, fptr);
                fputc((unsigned char) pixel.g, fptr);
                fputc((unsigned char) pixel.b, fptr);
            }
        }
    } else if(format == R8G8B8A8Uint) {
        for(int y = 0; y < height; ++y) {
            for(int x = 0; x < width; ++x) {
                ppm_color_rgba32_t pixel = ((ppm_color_rgba32_t*) data)[x + width * y];
                
                fputc((unsigned char) pixel.r, fptr);
            }
        }
    } else if(format == R32G32B32A32Float) {
        for(int y = 0; y < height; ++y) {
            for(int x = 0; x < width; ++x) {
                ppm_color_rgba32f_t pixel = ((ppm_color_rgba32f_t*) data)[x + width * y];

                unsigned char r = (unsigned char) (pixel.r * 255.0f);
                unsigned char g = (unsigned char) (pixel.g * 255.0f);
                unsigned char b = (unsigned char) (pixel.b * 255.0f);
                
                fputc(r, fptr);
                fputc(g, fptr);
                fputc(b, fptr);
            }
        }
    }


    fclose(fptr);
}


#endif