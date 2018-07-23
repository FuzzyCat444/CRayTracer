#ifndef VEC3_H_INCLUDED
#define VEC3_H_INCLUDED

typedef struct Vec3
{
    float x;
    float y;
    float z;
} Vec3;

Vec3 Vec3_add(Vec3 a, Vec3 b);

Vec3 Vec3_sub(Vec3 a, Vec3 b);

Vec3 Vec3_mul(Vec3 a, Vec3 b);

Vec3 Vec3_div(Vec3 a, Vec3 b);

Vec3 Vec3_mulScalar(Vec3 a, float scalar);

float Vec3_dot(Vec3 a, Vec3 b);

Vec3 Vec3_cross(Vec3 a, Vec3 b);

float Vec3_len(Vec3 a);

float Vec3_lenSq(Vec3 a);

Vec3 Vec3_norm(Vec3 a);

Vec3 Vec3_normComponents(Vec3 a);

#endif // VEC3_H_INCLUDED
