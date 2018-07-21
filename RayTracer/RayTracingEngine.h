#ifndef RAYTRACINGENGINE_H_INCLUDED
#define RAYTRACINGENGINE_H_INCLUDED

#include "Framebuffer.h"

#include "Scene.h"
#include "Camera.h"

typedef struct RayTracingEngine RayTracingEngine;

RayTracingEngine *RayTracingEngine_create(int width, int height, int blockWidth, float fov);

int RayTracingEngine_getWidth(RayTracingEngine *engine);
int RayTracingEngine_getHeight(RayTracingEngine *engine);

void RayTracingEngine_simulate(RayTracingEngine *engine);

Framebuffer *RayTracingEngine_getRenderBuffer(RayTracingEngine *engine);

Scene *RayTracingEngine_getScene(RayTracingEngine *engine);

void RayTracingEngine_moveCamera(RayTracingEngine *engine, Vec3 v, float yaw, float pitch);
void RayTracingEngine_moveCameraForward(RayTracingEngine *engine, float amt);
void RayTracingEngine_moveCameraUp(RayTracingEngine *engine, float amt);
void RayTracingEngine_moveCameraRight(RayTracingEngine *engine, float amt);

void RayTracingEngine_destroy(RayTracingEngine *engine);

#endif
