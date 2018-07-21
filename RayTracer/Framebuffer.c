#include "Framebuffer.h"
#include <stdlib.h>

struct Framebuffer
{
    int width;
    int height;
    uint8_t *pixels;
};

Framebuffer *Framebuffer_create(int width, int height)
{
    Framebuffer *buffer = malloc(sizeof *buffer);
    if (buffer)
    {
        buffer->width = width;
        buffer->height = height;
        buffer->pixels = malloc(3 * width * height);
        if (!buffer->pixels)
        {
            Framebuffer_destroy(buffer);
            buffer = NULL;
        }
    }
    return buffer;
}

int Framebuffer_getWidth(Framebuffer *buffer)
{
    return buffer->width;
}

int Framebuffer_getHeight(Framebuffer *buffer)
{
    return buffer->height;
}

uint8_t *Framebuffer_getPixels(Framebuffer *buffer)
{
    return buffer->pixels;
}

void Framebuffer_clear(Framebuffer *buffer, uint8_t r, uint8_t g, uint8_t b)
{
    int pixelCount = Framebuffer_getWidth(buffer) * Framebuffer_getHeight(buffer) * 3;
    for (int i = 0; i < pixelCount; i += 3)
    {
        buffer->pixels[i    ] = r;
        buffer->pixels[i + 1] = g;
        buffer->pixels[i + 2] = b;
    }
}

void Framebuffer_destroy(Framebuffer *buffer)
{
    free(buffer->pixels);

    free(buffer);
}
