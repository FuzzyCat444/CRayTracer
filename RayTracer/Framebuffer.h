#ifndef FRAMEBUFFER_H_INCLUDED
#define FRAMEBUFFER_H_INCLUDED

#include <stdint.h>

typedef struct Framebuffer Framebuffer;

Framebuffer *Framebuffer_create(int width, int height);

int Framebuffer_getWidth(Framebuffer *buffer);
int Framebuffer_getHeight(Framebuffer *buffer);

uint8_t *Framebuffer_getPixels(Framebuffer *buffer);

void Framebuffer_clear(Framebuffer *buffer, uint8_t r, uint8_t g, uint8_t b);

void Framebuffer_destroy(Framebuffer *buffer);

#endif // FRAMEBUFFER_H_INCLUDED
