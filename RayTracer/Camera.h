#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "Vec3.h"

typedef struct Camera Camera;

Camera *Camera_create(int width, int height, float fov);

Vec3 Camera_vectorAt(Camera *cam, int x, int y);

void Camera_set(Camera *camera, Vec3 pos, float yaw, float pitch);

void Camera_move(Camera *camera, Vec3 pos, float yaw, float pitch);

void Camera_moveForward(Camera *camera, float amt);
void Camera_moveUp(Camera *camera, float amt);
void Camera_moveRight(Camera *camera, float amt);

Vec3 Camera_getPos(Camera *camera);

void Camera_destroy(Camera *camera);

#endif // CAMERA_H_INCLUDED
