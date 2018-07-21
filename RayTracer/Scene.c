#include "Scene.h"

#include <stdlib.h>
#include <math.h>

#include "Mat4.h"

typedef struct PointLight
{
    Vec3 pos;
    Vec3 col;
    float distSq;
} PointLight;

static float PointLight_distanceBrightness(Vec3 point, PointLight *light)
{
    float rSq = Vec3_lenSq(Vec3_sub(light->pos, point));
    if (rSq < light->distSq)
    {
        float rSqInv = 1.0f - (rSq / light->distSq);
        return rSqInv * rSqInv;
    }

    return 0.0f;
}

static float PointLight_angleBrightness(Vec3 toLightNorm, Vec3 normal, PointLight *light)
{
    float br = Vec3_dot(normal, toLightNorm);
    if (br < 0.0f)
    {
        br = 0.0f;
    }
    return br;
}

typedef enum ObjectType
{
    OBJECT_NULL,
    OBJECT_PLANE,
    OBJECT_SPHERE
} ObjectType;

typedef struct Plane
{
    Vec3 center;
    float halfWidth;
    float halfHeight;
    float yaw;
    float pitch;
    Mat4 rotate;
    Mat4 translate;
    Mat4 rotateInverse;
    Mat4 translateInverse;
    Material material;
} Plane;

typedef struct Sphere
{
    Vec3 center;
    float radius;
    Mat4 translate;
    Mat4 translateInverse;
    Material material;
} Sphere;

typedef struct TraceInfo
{
    float t;
    Vec3 hitPoint;
    Vec3 normal;
    Material material;
} TraceInfo;

struct Scene
{
    PointLight *pointLights;
    int pointLightsPtr;
    int pointLightsSize;

    Plane *planes;
    int planesPtr;
    int planesSize;

    Sphere *spheres;
    int spheresPtr;
    int spheresSize;
};

Scene *Scene_create()
{
    Scene *scene = malloc(sizeof *scene);
    if (scene)
    {
        scene->pointLightsPtr = 0;
        scene->pointLightsSize = 1;

        scene->planesPtr = 0;
        scene->planesSize = 1;

        scene->spheresPtr = 0;
        scene->spheresSize = 1;

        scene->pointLights = malloc(sizeof *scene->pointLights);
        scene->planes = malloc(sizeof *scene->planes);
        scene->spheres = malloc(sizeof *scene->spheres);
        if (!scene->pointLights || !scene->planes || !scene->spheres)
        {
            Scene_destroy(scene);
            scene = NULL;
        }
    }
    return scene;
}

void Scene_addPointLight(Scene *scene, Vec3 pos, Vec3 col, float dist)
{
    int canAdd = 1;
    if (scene->pointLightsPtr == scene->pointLightsSize)
    {
        int newSize = scene->pointLightsSize * 2;
        PointLight *newArr = realloc(scene->pointLights, sizeof *newArr * newSize);
        if (newArr)
        {
            scene->pointLights = newArr;
            scene->pointLightsSize = newSize;
        }
        else
        {
            canAdd = 0;
        }
    }

    if (canAdd)
    {
        PointLight *pointLight = &scene->pointLights[scene->pointLightsPtr++];
        pointLight->pos = pos;
        pointLight->col = col;
        pointLight->distSq = dist * dist;
    }
}

void Scene_addPlane(Scene *scene, Vec3 center, float width, float height, float yaw, float pitch, Material material)
{
    int canAdd = 1;
    if (scene->planesPtr == scene->planesSize)
    {
        int newSize = scene->planesSize * 2;
        Plane *newArr = realloc(scene->planes, sizeof *newArr * newSize);

        if (newArr)
        {
            scene->planes = newArr;
            scene->planesSize = newSize;
        }
        else
        {
            canAdd = 0;
        }
    }

    if (canAdd)
    {
        Plane *plane = &scene->planes[scene->planesPtr++];
        plane->center = center;
        plane->halfWidth = width * 0.5f;
        plane->halfHeight = height * 0.5f;
        plane->yaw = yaw;
        plane->pitch = pitch;
        plane->rotate = Mat4_mul(Mat4_rotateY(plane->yaw), Mat4_rotateX(plane->pitch));
        plane->translate = Mat4_translate(plane->center);
        plane->rotateInverse = Mat4_inverse(plane->rotate);
        plane->translateInverse = Mat4_inverse(plane->translate);
        plane->material = material;
    }
}

void Scene_addSphere(Scene *scene, Vec3 center, float radius, Material material)
{
    int canAdd = 1;
    if (scene->spheresPtr == scene->spheresSize)
    {
        int newSize = scene->spheresSize * 2;
        Sphere *newArr = realloc(scene->spheres, sizeof *newArr * newSize);
        if (newArr)
        {
            scene->spheres = newArr;
            scene->spheresSize = newSize;
        }
        else
        {
            canAdd = 0;
        }
    }

    if (canAdd)
    {
        Sphere *sphere = &scene->spheres[scene->spheresPtr++];
        sphere->center = center;
        sphere->radius = radius;
        sphere->translate = Mat4_translate(sphere->center);
        sphere->translateInverse = Mat4_inverse(sphere->translate);
        sphere->material = material;
    }
}

static const float FAR_T = 1000.0f;
static const float EPSILON = 0.001f;

// Calculates one intersection of the ray with the closest object and returns information about the hit
static void Scene_traceHit(Scene *scene, Vec3 start, Vec3 rayDir, TraceInfo *info)
{
    float closestT = FAR_T;
    void *closestObject = NULL;
    ObjectType closestObjectType = OBJECT_NULL;

    // Planes
    for (int i = 0; i < scene->planesPtr; i++)
    {
        Plane *plane = &scene->planes[i];

        Vec3 st = Mat4_mulVec3(Mat4_mul(plane->rotateInverse, plane->translateInverse), start);
        Vec3 dr = Mat4_mulVec3(plane->rotateInverse, rayDir);

        float t = -st.y / dr.y;
        Vec3 hitPoint = (Vec3) {st.x + t * dr.x, 0.0f, st.z + t * dr.z};
        if (t < closestT && t > EPSILON && abs(hitPoint.x) < plane->halfWidth && abs(hitPoint.z) < plane->halfHeight)
        {
            closestT = t;
            closestObject = plane;
            closestObjectType = OBJECT_PLANE;
        }
    }

    // Spheres
    for (int i = 0; i < scene->spheresPtr; i++)
    {
        Sphere *sphere = &scene->spheres[i];

        Vec3 st = Mat4_mulVec3(sphere->translateInverse, start);
        Vec3 dr = rayDir;

        float A = st.x;
        float B = st.y;
        float C = st.z;
        float a = dr.x*dr.x + dr.y*dr.y + dr.z*dr.z;
        float b = 2 * (A*dr.x + B*dr.y + C*dr.z);
        float c = A*A + B*B + C*C - sphere->radius * sphere->radius;
        float disc = b*b - 4*a*c;
        if (disc >= 0.0f)
        {
            float sqrtDisc = sqrt(disc);
            float a2 = 1 / (a * 2);
            float t1 = (-b + sqrtDisc) * a2;
            float t2 = (-b - sqrtDisc) * a2;
            if (t1 < closestT && t1 > EPSILON)
            {
                closestT = t1;
                closestObject = sphere;
                closestObjectType = OBJECT_SPHERE;
            }
            if (t2 < closestT && t2 > EPSILON)
            {
                closestT = t2;
                closestObject = sphere;
                closestObjectType = OBJECT_SPHERE;
            }
        }
    }

    info->t = closestT;
    info->hitPoint = Vec3_add(start, Vec3_mulScalar(rayDir, closestT));

    if (closestObjectType == OBJECT_PLANE)
    {
        Plane *plane = (Plane*) closestObject;

        info->normal = Mat4_mulVec3(plane->rotate, (Vec3) {0.0f, 1.0f, 0.0f});
        info->material = plane->material;
    }
    else if (closestObjectType == OBJECT_SPHERE)
    {
        Sphere *sphere = (Sphere*) closestObject;

        info->normal = Vec3_mulScalar(Vec3_sub(info->hitPoint, sphere->center), 1.0f / sphere->radius);
        info->material = sphere->material;
    }
}

// Traces a ray through a scene, including reflections, and returns the color 'seen' by the ray
Vec3 Scene_trace(Scene *scene, Vec3 start, Vec3 rayDir)
{
    TraceInfo infoInitial;
    Scene_traceHit(scene, start, rayDir, &infoInitial);

    Vec3 color = {0.0f};
    if (infoInitial.t < FAR_T)
    {
        const float amb = 0.05f;
        color = (Vec3) {amb, amb, amb};
        for (int i = 0; i < scene->pointLightsPtr; i++)
        {
            PointLight *light = &scene->pointLights[i];

            Vec3 toLight = Vec3_sub(light->pos, infoInitial.hitPoint);
            Vec3 toLightNorm = Vec3_norm(toLight);
            TraceInfo infoShadow;
            Scene_traceHit(scene, infoInitial.hitPoint, toLightNorm, &infoShadow);
            float tL = Vec3_len(toLight);

            if (tL < infoShadow.t)
            {
                float brightness = PointLight_distanceBrightness(infoInitial.hitPoint, light) * PointLight_angleBrightness(toLightNorm, infoInitial.normal, light);
                color = Vec3_add(color, Vec3_mulScalar(light->col, brightness));
            }
        }
        color = Vec3_mul(color, infoInitial.material.color);
    }

    if (color.x > 1.0f) color.x = 1.0f;
    if (color.y > 1.0f) color.y = 1.0f;
    if (color.z > 1.0f) color.z = 1.0f;
    return color;
}

void Scene_destroy(Scene *scene)
{
    free(scene->pointLights);
    free(scene->planes);
    free(scene->spheres);

    free(scene);
}
