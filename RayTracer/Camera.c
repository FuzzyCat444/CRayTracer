#include "Camera.h"

#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "Vec3.h"
#include "Mat4.h"

struct Camera
{
    int width;
    int height;
    Vec3 *rays;

    Vec3 pos;
    float yaw;
    float pitch;

    Vec3 forward;
    Vec3 right;
    Vec3 up;
};

Camera *Camera_create(int width, int height, float fov)
{
    Camera *cam = malloc(sizeof *cam);
    if (cam)
    {
        cam->width = width;
        cam->height = height;
        Camera_set(cam, (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f}, 0.0f, 0.0f);
        cam->rays = malloc(sizeof *cam->rays * width * height);
        if (!cam->rays || width % 2 == 1 || height % 2 == 1)
        {
            Camera_destroy(cam);
            cam = NULL;
        }
        else
        {
            float su = tan(fov * 0.5f * M_PI / 180.0f) * 2.0f / width;
            int halfWidth = width / 2;
            int halfHeight = height / 2;

            Vec3 v;
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    v.x = (x - halfWidth + 0.5f) * su;
                    v.y = (y - halfHeight + 0.5f) * su;
                    v.z = 1.0f;
                    cam->rays[x + y * width] = Vec3_norm(v);
                }
            }
        }
    }
    return cam;
}

Vec3 Camera_vectorAt(Camera *cam, int x, int y)
{
    Vec3 v = cam->rays[x + y * cam->width];
    v = Vec3_add(Vec3_add(Vec3_mulScalar(cam->right, v.x), Vec3_mulScalar(cam->up, v.y)), Vec3_mulScalar(cam->forward, v.z));
    return v;
}

void Camera_set(Camera *camera, Vec3 pos, float yaw, float pitch)
{
    camera->pos = pos;
    camera->yaw = yaw;
    camera->pitch = pitch;

    Mat4 rot = Mat4_mul(Mat4_rotateY(yaw), Mat4_rotateX(pitch));

    camera->forward = Mat4_mulVec3(rot, (Vec3) {0.0f, 0.0f, 1.0f});
    camera->up = Mat4_mulVec3(rot, (Vec3) {0.0f, 1.0f, 0.0f});
    camera->right = Vec3_cross(camera->up, camera->forward);
}

void Camera_move(Camera *camera, Vec3 pos, float yaw, float pitch)
{
    Camera_set(camera, Vec3_add(camera->pos, pos), camera->yaw + yaw, camera->pitch + pitch);
}

void Camera_moveForward(Camera *camera, float amt)
{
    Vec3 mv = Vec3_mulScalar(camera->forward, amt);
    Camera_move(camera, mv, 0.0f, 0.0f);
}


void Camera_moveUp(Camera *camera, float amt)
{
    Vec3 mv = Vec3_mulScalar(camera->up, amt);
    Camera_move(camera, mv, 0.0f, 0.0f);
}

void Camera_moveRight(Camera *camera, float amt)
{
    Vec3 mv = Vec3_mulScalar(camera->right, amt);
    Camera_move(camera, mv, 0.0f, 0.0f);
}

Vec3 Camera_getPos(Camera *camera)
{
    return camera->pos;
}

void Camera_destroy(Camera *camera)
{
    free(camera->rays);

    free(camera);
}
