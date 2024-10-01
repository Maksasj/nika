#ifndef PPM_H
#define PPM_H

#include <stdio.h>

#define PPM_INLINE static inline

typedef struct {
    unsigned char a;
    unsigned char b;
    unsigned char g;
    unsigned char r;
} color_rgba_t;

#define EUCLIB_PLOT_AT(PLOT, X, Y) ((PLOT)->value[X + (((PLOT)->height - 1) - Y) * (PLOT)->width])

PPM_INLINE void ppm_export_image(const char* file_name, int width, int height, color_rgba_t* data) {
    FILE* fptr = fopen(file_name, "w");

    if (fptr == NULL) {
        printf("The file is not opened. The program will now exit.");
        exit(0);
    }

    fprintf(fptr, "P6\n");
    fprintf(fptr, "# created by Eric R. Weeks using a C program\n");
    fprintf(fptr, "%d %d\n", width, height);
    fprintf(fptr, "255\n");

    for(int x = 0; x < width; ++x) {
        for(int y = 0; y < height; ++y) {
            color_rgba_t pixel = data[x + width * y];

            fputc((unsigned char) pixel.r, fptr);
            fputc((unsigned char) pixel.g, fptr);
            fputc((unsigned char) pixel.b, fptr);
        }
    }

    fclose(fptr);
}


#endif