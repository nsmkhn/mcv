#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#define HEIGHT 512
#define WIDTH 512
#define NUM_PIXELS (WIDTH)*(HEIGHT)
#define BLACK 0x000000
#define INDIGO 0x4B0082
#define HONEYDEW 0xF0FFF0
#define TILE_SIZE 8
#define RADIUS (WIDTH)/3

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB;

void
fill_pixels(RGB *pixels, size_t num_pixels, RGB color)
{
    for(size_t i = 0; i < num_pixels; ++i)
        pixels[i] = color;
}

void
save_as_ppm(char *filepath, RGB *pixels, size_t width, size_t height)
{
    FILE *file;
    if((file = fopen(filepath, "w")) == NULL)
    {
        fprintf(stderr, "Failed to open file %s: %s", filepath, strerror(errno));
        exit(EXIT_FAILURE);
    }

    fprintf(file, "P6\n%zu %zu 255\n", width, height);
    fwrite(pixels, sizeof(RGB), width*height, file);

    if(fclose(file) == EOF)
    {
        fprintf(stderr, "Failed to close file %s: %s", filepath, strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("Generated %s\n", filepath);
}

void
stripes_pattern(RGB *pixels,
                size_t width,
                size_t height,
                size_t tile_size,
                RGB background,
                RGB foreground)
{
    for(size_t y = 0; y < height; ++y)
    {
        for(size_t x = 0; x < width; ++x)
        {
            pixels[y * width + x] =
                ((x + y)/tile_size) % 2 == 0 ? foreground : background;
        }
    } 
}

void
checker_pattern(RGB *pixels,
                size_t width,
                size_t height,
                size_t tile_size,
                RGB background,
                RGB foreground)
{
    for(size_t y = 0; y < height; ++y)
    {
        for(size_t x = 0; x < width; ++x)
        {
            pixels[y * height + x] =
                (x/tile_size + y/tile_size) % 2 == 0 ? foreground : background;
        }
    } 
}

void
solid_circle_pattern(RGB *pixels,
                     size_t width,
                     size_t height,
                     size_t radius,
                     RGB background,
                     RGB foreground)
{
    int64_t cx = width;
    int64_t cy = height;
    int64_t r = radius * 2;
    for(size_t y = 0; y < height; ++y)
    {
        for(size_t x = 0; x < width; ++x)
        {
            int64_t dx = cx - x*2 - 1;
            int64_t dy = cy - y*2 - 1;
            pixels[y * width + x] =
                (dx*dx + dy*dy) <= r*r ? foreground : background;
        }
    }
}

void
hollow_circle_pattern(RGB *pixels,
                      size_t width,
                      size_t height,
                      size_t radius,
                      RGB color)
{
    assert(width == height);
    float w = width, h = height;
    float r = radius;
    float x = 0.0f, y = r - 0.5;
    float cx = w / 2, cy = h / 2;
    while(x <= y)
    {
        size_t px = x + cx;
        size_t py = y + cy;

        pixels[py * width + px] = color;
        pixels[px * width + py] = color;

        pixels[(height - py) * width + px] = color;
        pixels[px * width + (height - py)] = color;

        pixels[py * width + (width - px)] = color;
        pixels[(width - px) * width + py] = color;

        pixels[(width - px) * width + (height - py)] = color;
        pixels[(height - py) * width + (width - px)] = color;

        x += 1.0f;
        if(x*x + y*y > r*r)
            y -= 1.0f;
    }
}

int
main()
{
    static RGB pixels[NUM_PIXELS];
    RGB honeydew = *(RGB *) &(uint32_t){HONEYDEW};
    RGB indigo = *(RGB *) &(uint32_t){INDIGO};
    RGB black = *(RGB *) &(uint32_t){BLACK};

    stripes_pattern(pixels, WIDTH, HEIGHT, TILE_SIZE, honeydew, indigo);
    save_as_ppm("stripes.ppm", pixels, WIDTH, HEIGHT);

    fill_pixels(pixels, WIDTH*HEIGHT, black);
    checker_pattern(pixels, WIDTH, HEIGHT, TILE_SIZE, honeydew, indigo);
    save_as_ppm("checker.ppm", pixels, WIDTH, HEIGHT);

    fill_pixels(pixels, WIDTH*HEIGHT, black);
    solid_circle_pattern(pixels, WIDTH, HEIGHT, RADIUS, honeydew, indigo);
    save_as_ppm("solid_circle.ppm", pixels, WIDTH, HEIGHT);

    fill_pixels(pixels, WIDTH*HEIGHT, honeydew);
    hollow_circle_pattern(pixels, WIDTH, HEIGHT, RADIUS, indigo);
    save_as_ppm("hollow_circle.ppm", pixels, WIDTH, HEIGHT);

    return 0;
}
