#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#define HEIGHT 16
#define WIDTH 16
#define BLACK 0x000000
#define MAGENTA 0xFF00FF
#define GREEN 0x00FF00
#define TILE_SIZE 8

void
reset_pixels(uint32_t *pixels, size_t num_pixels, uint32_t color)
{
    for(size_t i = 0; i < num_pixels; ++i)
        pixels[i] = color;
}

void
save_as_ppm(char *filepath, uint32_t *pixels, size_t width, size_t height)
{
    FILE *file;
    if((file = fopen(filepath, "w")) == NULL)
    {
        fprintf(stderr, "Failed to open file %s: %s", filepath, strerror(errno));
        exit(EXIT_FAILURE);
    }

    uint8_t colors[width*height*3], *pcolors = colors;
    fprintf(file, "P6\n%zu %zu 255\n", width, height);
    for(size_t y = 0; y < height; ++y)
    {
        for(size_t x = 0; x < width; ++x)
        {
            uint32_t pixel = pixels[y * width + x];
            *pcolors++ = (pixel >> 8 * 2) & 0xFF;
            *pcolors++ = (pixel >> 8 * 1) & 0xFF;
            *pcolors++ = (pixel >> 8 * 0) & 0xFF;
        }
    }

    fwrite(colors, sizeof(uint8_t), sizeof(colors), file);
    if(fclose(file) == EOF)
    {
        fprintf(stderr, "Failed to close file %s: %s", filepath, strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("Generated %s\n", filepath);
}

void
stripes_pattern(uint32_t *pixels,
                size_t width,
                size_t height,
                size_t tile_size,
                uint32_t background,
                uint32_t foreground)
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
checker_pattern(uint32_t *pixels,
                size_t width,
                size_t height,
                size_t tile_size,
                uint32_t background,
                uint32_t foreground)
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
solid_circle_pattern(uint32_t *pixels,
                    size_t width,
                    size_t height,
                    size_t radius,
                    uint32_t background,
                    uint32_t foreground)
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
hollow_circle_pattern(uint32_t *pixels,
                      size_t width,
                      size_t height,
                      size_t radius,
                      uint32_t color)
{
    assert(width == height);
    int32_t w = width, h = height;
    int32_t r = radius;
    int32_t x = 0, y = r;
    int32_t cx = w / 2, cy = h / 2;
    while(x <= y)
    {
        int32_t px = x + cx;
        int32_t py = y + cy;

        pixels[py * width + px] = color;
        pixels[px * width + py] = color;

        pixels[(height - py) * width + px] = color;
        pixels[px * width + (height - py)] = color;

        pixels[py * width + (width - px)] = color;
        pixels[(width - px) * width + py] = color;

        pixels[(width - px) * width + (height - py)] = color;
        pixels[(height - py) * width + (width - px)] = color;

        ++x;
        if(x*x + y*y > r*r)
            --y;
    }
}

int
main()
{
    uint32_t pixels[WIDTH*HEIGHT] = {0};

    stripes_pattern(pixels, WIDTH, HEIGHT, TILE_SIZE, BLACK, MAGENTA);
    save_as_ppm("stripes.ppm", pixels, WIDTH, HEIGHT);

    reset_pixels(pixels, WIDTH*HEIGHT, BLACK);
    checker_pattern(pixels, WIDTH, HEIGHT, TILE_SIZE, BLACK, MAGENTA);
    save_as_ppm("checker.ppm", pixels, WIDTH, HEIGHT);

    size_t radius = WIDTH/2;
    reset_pixels(pixels, WIDTH*HEIGHT, BLACK);
    solid_circle_pattern(pixels, WIDTH, HEIGHT, radius, BLACK, MAGENTA);
    save_as_ppm("solid_circle.ppm", pixels, WIDTH, HEIGHT);

    reset_pixels(pixels, WIDTH*HEIGHT, GREEN);
    hollow_circle_pattern(pixels, WIDTH, HEIGHT, radius, MAGENTA);
    save_as_ppm("hollow_circle.ppm", pixels, WIDTH, HEIGHT);

    return 0;
}
