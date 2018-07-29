#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED

#include <stdint.h>

#include "Vec3.h"
#include "Material.h"

typedef struct Scene Scene;

Scene *Scene_create();

void Scene_setSky(Scene *scene, uint8_t *pixels, int width, int height, int skyEnabled, int reflectionsEnabled);

void Scene_addPointLight(Scene *scene, Vec3 pos, Vec3 col, float dist);

void Scene_addPlane(Scene *scene, Vec3 center, float width, float height, float yaw, float pitch, Material material);

void Scene_addSphere(Scene *scene, Vec3 center, float radius, Material material);

void Scene_addTorus(Scene *scene, Vec3 center, float radius, float tubeRadius, float yaw, float pitch, Material material);

Vec3 Scene_trace(Scene *scene, Vec3 start, Vec3 rayDir);

void Scene_destroy(Scene *scene);

#endif // SCENE_H_INCLUDED
