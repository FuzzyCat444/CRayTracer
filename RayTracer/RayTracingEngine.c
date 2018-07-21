#include "RayTracingEngine.h"

#include <stdlib.h>
#include <math.h>

struct RayTracingEngine
{
    int width;
    int height;
    int blockWidth;
    int blockSize;
    int *blockOrder;
    int blockOrderIndex;
    int blockOrderVal;
    Framebuffer *renderBuffer;

    Scene *scene;
    Camera *camera;
};

RayTracingEngine *RayTracingEngine_create(int width, int height, int blockWidth, float fov)
{
    RayTracingEngine *engine = malloc(sizeof *engine);
    if (engine)
    {
        engine->width = width;
        engine->height = height;

        blockWidth = blockWidth < 1 ? 1 : blockWidth > height ? height : blockWidth;
        engine->blockWidth = blockWidth;
        engine->blockSize = blockWidth * blockWidth;
        engine->blockOrder = malloc(sizeof *engine->blockOrder * engine->blockSize);
        engine->blockOrderIndex = 0;
        engine->blockOrderVal = 0;

        engine->renderBuffer = Framebuffer_create(width, height);

        engine->scene = Scene_create();
        engine->camera = Camera_create(width, height, fov);
        if (!engine->renderBuffer || !engine->scene || !engine->camera || !engine->blockOrder)
        {
            RayTracingEngine_destroy(engine);
            engine = NULL;
        }
        else
        {
            for (int i = 0; i < engine->blockSize; i++)
            {
                engine->blockOrder[i] = i;
            }
            for (int i = 0; i < engine->blockSize; i++)
            {
                int temp = engine->blockOrder[i];
                int randIndex = rand() % engine->blockSize;

                engine->blockOrder[i] = engine->blockOrder[randIndex];
                engine->blockOrder[randIndex] = temp;
            }
        }
    }

    return engine;
}

int RayTracingEngine_getWidth(RayTracingEngine *engine)
{
    return engine->width;
}

int RayTracingEngine_getHeight(RayTracingEngine *engine)
{
    return engine->height;
}

void RayTracingEngine_simulate(RayTracingEngine *engine)
{
    uint8_t *pixels = Framebuffer_getPixels(engine->renderBuffer);
    Vec3 camPos = Camera_getPos(engine->camera);

    if (engine->blockOrderIndex < engine->blockSize)
    {
        engine->blockOrderVal = engine->blockOrder[engine->blockOrderIndex++];
        int blockPxOffset = engine->blockOrderVal % engine->blockWidth;
        int blockPyOffset = engine->blockOrderVal / engine->blockWidth;

        int pLocIncColumn = engine->blockWidth * 3;

        for (int y = blockPyOffset; y < engine->height; y += engine->blockWidth)
        {
            int pLoc = (y * engine->width + blockPxOffset) * 3;
            for (int x = blockPxOffset; x < engine->width; x += engine->blockWidth, pLoc += pLocIncColumn)
            {
                Vec3 rayDir = Camera_vectorAt(engine->camera, x, y);

                Vec3 color = Scene_trace(engine->scene, camPos, rayDir);
                uint8_t r = (uint8_t) floor(color.x * 255.0f + 0.5f);
                uint8_t g = (uint8_t) floor(color.y * 255.0f + 0.5f);
                uint8_t b = (uint8_t) floor(color.z * 255.0f + 0.5f);

                pixels[pLoc    ] = r;
                pixels[pLoc + 1] = g;
                pixels[pLoc + 2] = b;
            }
        }
    }
}

Framebuffer *RayTracingEngine_getRenderBuffer(RayTracingEngine *engine)
{
    return engine->renderBuffer;
}

Scene *RayTracingEngine_getScene(RayTracingEngine *engine)
{
    return engine->scene;
}

void RayTracingEngine_moveCamera(RayTracingEngine *engine, Vec3 v, float yaw, float pitch)
{
    Camera_move(engine->camera, v, yaw, pitch);
    Framebuffer_clear(engine->renderBuffer, 0, 0, 0);
    engine->blockOrderIndex = 0;
}

void RayTracingEngine_moveCameraForward(RayTracingEngine *engine, float amt)
{
    Camera_moveForward(engine->camera, amt);
    Framebuffer_clear(engine->renderBuffer, 0, 0, 0);
    engine->blockOrderIndex = 0;
}

void RayTracingEngine_moveCameraUp(RayTracingEngine *engine, float amt)
{
    Camera_moveUp(engine->camera, amt);
    Framebuffer_clear(engine->renderBuffer, 0, 0, 0);
    engine->blockOrderIndex = 0;
}

void RayTracingEngine_moveCameraRight(RayTracingEngine *engine, float amt)
{

    Camera_moveRight(engine->camera, amt);
    Framebuffer_clear(engine->renderBuffer, 0, 0, 0);
    engine->blockOrderIndex = 0;
}

void RayTracingEngine_destroy(RayTracingEngine *engine)
{
    Framebuffer_destroy(engine->renderBuffer);
    Scene_destroy(engine->scene);
    Camera_destroy(engine->camera);
    free(engine->blockOrder);

    free(engine);
}
