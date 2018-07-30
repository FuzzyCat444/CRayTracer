#include "Scene.h"

#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#define _1_PI 1.0/M_PI
#define _1_2PI 1.0/(M_PI*2)

#include "Mat4.h"
#include "MathFunctions.h"

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
    OBJECT_SPHERE,
    OBJECT_TORUS
} ObjectType;

typedef struct Plane
{
    Vec3 center;
    float halfWidth;
    float halfHeight;
    float yaw;
    float pitch;
    Mat4 translate;
    Mat4 translateInverse;
    Mat4 rotate;
    Mat4 rotateInverse;
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

typedef struct Torus
{
    Vec3 center;
    float radius;
    float tubeRadius;
    float yaw;
    float pitch;
    Mat4 translate;
    Mat4 translateInverse;
    Mat4 rotate;
    Mat4 rotateInverse;
    Material material;
} Torus;

typedef struct TraceInfo
{
    float t;
    Vec3 hitPoint;
    Vec3 normal;
    Material material;
} TraceInfo;

typedef struct Sky
{
    uint8_t *pixels;
    int pixelsWidth;
    int pixelsHeight;
    int enabled;
    int reflectionsEnabled;
} Sky;

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

    Torus *tori;
    int toriPtr;
    int toriSize;

    Sky sky;
};

Scene *Scene_create()
{
    Scene *scene = malloc(sizeof *scene);
    if (scene)
    {
        scene->pointLights = malloc(sizeof *scene->pointLights);
        scene->planes = malloc(sizeof *scene->planes);
        scene->spheres = malloc(sizeof *scene->spheres);
        scene->tori = malloc(sizeof *scene->tori);
        if (!scene->pointLights || !scene->planes || !scene->spheres || !scene->tori)
        {
            Scene_destroy(scene);
            scene = NULL;
        }
        else
        {
            scene->pointLightsPtr = 0;
            scene->pointLightsSize = 1;

            scene->planesPtr = 0;
            scene->planesSize = 1;

            scene->spheresPtr = 0;
            scene->spheresSize = 1;

            scene->toriPtr = 0;
            scene->toriSize = 1;

            Scene_setSky(scene, NULL, 0, 0, 0, 0);
        }
    }
    return scene;
}

void Scene_setSky(Scene *scene, uint8_t *pixels, int width, int height, int skyEnabled, int reflectionsEnabled)
{
    scene->sky = (Sky)
    {
        .pixels = pixels,
        .pixelsWidth = width,
        .pixelsHeight = height,
        .enabled = skyEnabled,
        .reflectionsEnabled = reflectionsEnabled
    };
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

void Scene_addTorus(Scene *scene, Vec3 center, float radius, float tubeRadius, float yaw, float pitch, Material material)
{
    int canAdd = 1;
    if (scene->toriPtr == scene->toriSize)
    {
        int newSize = scene->toriSize * 2;
        Torus *newArr = realloc(scene->tori, sizeof *newArr * newSize);

        if (newArr)
        {
            scene->tori = newArr;
            scene->toriSize = newSize;
        }
        else
        {
            canAdd = 0;
        }
    }

    if (canAdd)
    {
        if (tubeRadius > radius)
        {
            tubeRadius = radius;
        }
        Torus *torus = &scene->tori[scene->toriPtr++];
        torus->center = center;
        torus->radius = radius;
        torus->tubeRadius = tubeRadius;
        torus->yaw = yaw;
        torus->pitch = pitch;
        torus->translate = Mat4_translate(torus->center);
        torus->translateInverse = Mat4_inverse(torus->translate);
        torus->rotate = Mat4_mul(Mat4_rotateY(torus->yaw), Mat4_rotateX(torus->pitch));
        torus->rotateInverse = Mat4_inverse(torus->rotate);
        torus->material = material;
    }
}

typedef struct TorusConstants
{
    float R2_minus_r2;
    float _4R2;
    float xs, xd;
    float ys, yd;
    float zs, zd;
} TorusConstants;

static float torusFunction(float t, void *constants)
{
    TorusConstants *tc = (TorusConstants*) constants;

    float x = tc->xs + t * tc->xd;
    float y = tc->ys + t * tc->yd;
    float z = tc->zs + t * tc->zd;
    float x2 = x * x;
    float y2 = y * y;
    float z2 = z * z;

    float sum_x2_z2 = x2 + z2;
    float part1 = sum_x2_z2 + y2 + tc->R2_minus_r2;
    part1 *= part1;
    float part2 = -tc->_4R2 * sum_x2_z2;

    return part1 + part2;
}

static const float FAR_T = 1000.0f;
static const float EPSILON = 0.001f;

// Calculates one intersection of the ray with the closest object and returns information about the hit
static void Scene_traceHit(Scene *scene, Vec3 start, Vec3 rayDir, TraceInfo *info)
{
    float closestT = FAR_T;
    void *closestObject = NULL;
    ObjectType closestObjectType = OBJECT_NULL;
    Vec3 localHitPoint;

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
            localHitPoint = hitPoint;
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
                localHitPoint = Vec3_add(st, Vec3_mulScalar(dr, closestT));
            }
            if (t2 < closestT && t2 > EPSILON)
            {
                closestT = t2;
                closestObject = sphere;
                closestObjectType = OBJECT_SPHERE;
                localHitPoint = Vec3_add(st, Vec3_mulScalar(dr, closestT));
            }
        }
    }

    // Tori
    for (int i = 0; i < scene->toriPtr; i++)
    {
        Torus *torus = &scene->tori[i];

        Vec3 st = Mat4_mulVec3(Mat4_mul(torus->rotateInverse, torus->translateInverse), start);
        Vec3 dr = Mat4_mulVec3(torus->rotateInverse, rayDir);

        float R2 = torus->radius * torus->radius;
        TorusConstants tc =
        {
            .R2_minus_r2 = R2 - torus->tubeRadius * torus->tubeRadius,
            ._4R2 = 4 * R2,
            .xs = st.x, .xd = dr.x,
            .ys = st.y, .yd = dr.y,
            .zs = st.z, .zd = dr.z
        };

        float dist = Vec3_len(st);
        float outerRadius = (torus->radius + torus->tubeRadius) * 1.3f;
        float tMin = dist - outerRadius;
        tMin = tMin < EPSILON ? EPSILON : tMin;
        float tMax = dist + outerRadius;
        float t = -1.0f;
        MathFunctions_findRootsF(torusFunction, &tc, tMin, tMax, &t, 1, 50, 25);

        if (t < closestT && t > EPSILON)
        {
            closestT = t;
            closestObject = torus;
            closestObjectType = OBJECT_TORUS;
            localHitPoint = Vec3_add(st, Vec3_mulScalar(dr, closestT));
        }
    }

    info->t = closestT;
    info->hitPoint = Vec3_add(start, Vec3_mulScalar(rayDir, closestT));

    switch (closestObjectType)
    {
    case OBJECT_PLANE:
    {
        Plane *plane = (Plane*) closestObject;

        info->normal = Mat4_mulVec3(plane->rotate, (Vec3) {0.0f, 1.0f, 0.0f});
        info->material = plane->material;
        break;
    }
    case OBJECT_SPHERE:
    {
        Sphere *sphere = (Sphere*) closestObject;

        info->normal = Vec3_mulScalar(localHitPoint, 1.0f / sphere->radius);
        info->material = sphere->material;
        break;
    }
    case OBJECT_TORUS:
    {
        Torus *torus = (Torus*) closestObject;

        Vec3 toHitXZ = {localHitPoint.x, 0.0f, localHitPoint.z};
        float len = Vec3_len(toHitXZ);
        toHitXZ = Vec3_mulScalar(toHitXZ, 1.0f / len);
        float xComp = len - torus->radius;
        float yComp = localHitPoint.y;
        Vec3 localNormal = Vec3_mulScalar(Vec3_add(Vec3_mulScalar(toHitXZ, xComp), (Vec3) {0.0f, yComp, 0.0f}), 1.0f / torus->tubeRadius);

        info->normal = Mat4_mulVec3(torus->rotate, localNormal);
        info->material = torus->material;
    }
    case OBJECT_NULL:
        break;
    }
}

#define AMBIENT_LIGHT 0.05f
static Vec3 Scene_diffuse(Scene *scene, TraceInfo *traceInfo)
{
    Vec3 color = (Vec3) {AMBIENT_LIGHT, AMBIENT_LIGHT, AMBIENT_LIGHT};
    for (int i = 0; i < scene->pointLightsPtr; i++)
    {
        PointLight *light = &scene->pointLights[i];

        Vec3 toLight = Vec3_sub(light->pos, traceInfo->hitPoint);
        Vec3 toLightNorm = Vec3_norm(toLight);
        TraceInfo infoShadow;
        Scene_traceHit(scene, traceInfo->hitPoint, toLightNorm, &infoShadow);
        float tL = Vec3_len(toLight);

        if (tL < infoShadow.t)
        {
            float brightness = PointLight_distanceBrightness(traceInfo->hitPoint, light) * PointLight_angleBrightness(toLightNorm, traceInfo->normal, light);
            color = Vec3_add(color, Vec3_mulScalar(light->col, brightness));
        }
    }
    return color;
}

#include "MathFunctions.h"

#define NUM_REFLECTIONS 5
// Traces a ray through a scene, including reflections, and returns the color 'seen' by the ray
Vec3 Scene_trace(Scene *scene, Vec3 start, Vec3 rayDir)
{
    TraceInfo traceInfo;

    Vec3 from = start;
    Vec3 to = rayDir;
    int reflectCount = 0;
    Vec3 materialInfo[2][NUM_REFLECTIONS + 1];
    while (1)
    {
        Scene_traceHit(scene, from, to, &traceInfo);
        if (traceInfo.t >= FAR_T)
        {
            if (scene->sky.pixels != NULL && ((reflectCount > 0 && scene->sky.reflectionsEnabled) || (reflectCount == 0 && scene->sky.enabled)))
            {
                float u = 0.5f + atan2(to.z, to.x) * _1_2PI;
                float v = 0.5f - asin(to.y) * _1_PI;
                int x = (int) floor(u * (scene->sky.pixelsWidth - 1) + 0.5f);
                int y = (int) floor(v * (scene->sky.pixelsHeight - 1) + 0.5f);
                int loc = (x + y * scene->sky.pixelsWidth) * 3;
                materialInfo[0][reflectCount] = (Vec3) {scene->sky.pixels[loc] / 255.0f, scene->sky.pixels[loc + 1] / 255.0f, scene->sky.pixels[loc + 2] / 255.0f};
            }
            else
            {
                materialInfo[0][reflectCount] = (Vec3) {0.0f, 0.0f, 0.0f};
            }
            break;
        }
        else
        {
            materialInfo[0][reflectCount] = Vec3_mul(Scene_diffuse(scene, &traceInfo), traceInfo.material.diffuse);
            materialInfo[1][reflectCount] = traceInfo.material.specular;
        }

        if (!(traceInfo.material.hasSpecular && reflectCount < NUM_REFLECTIONS))
            break;

        from = traceInfo.hitPoint;
        to = Vec3_sub(to, Vec3_mulScalar(traceInfo.normal, 2.0f * Vec3_dot(to, traceInfo.normal)));
        reflectCount++;
    }

    Vec3 color = materialInfo[0][reflectCount]; // Normalize?
    reflectCount--;
    while (reflectCount >= 0)
    {
        Vec3 diff = materialInfo[0][reflectCount];
        Vec3 spec = materialInfo[1][reflectCount];

        color = Vec3_add(diff, Vec3_mul(spec, color));

        reflectCount--;
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
    free(scene->tori);

    free(scene);
}
