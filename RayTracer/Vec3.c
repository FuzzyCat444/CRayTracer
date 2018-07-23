#include "Vec3.h"

#include <math.h>

Vec3 Vec3_add(Vec3 a, Vec3 b)
{
    Vec3 ret;
    ret.x = a.x + b.x;
    ret.y = a.y + b.y;
    ret.z = a.z + b.z;
    return ret;
}

Vec3 Vec3_sub(Vec3 a, Vec3 b)
{
    Vec3 ret;
    ret.x = a.x - b.x;
    ret.y = a.y - b.y;
    ret.z = a.z - b.z;
    return ret;
}

Vec3 Vec3_mul(Vec3 a, Vec3 b)
{
    Vec3 ret;
    ret.x = a.x * b.x;
    ret.y = a.y * b.y;
    ret.z = a.z * b.z;
    return ret;
}

Vec3 Vec3_div(Vec3 a, Vec3 b)
{
    Vec3 ret;
    ret.x = a.x / b.x;
    ret.y = a.y / b.y;
    ret.z = a.z / b.z;
    return ret;
}

Vec3 Vec3_mulScalar(Vec3 a, float scalar)
{
    a.x *= scalar;
    a.y *= scalar;
    a.z *= scalar;
    return a;
}

float Vec3_dot(Vec3 a, Vec3 b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vec3 Vec3_cross(Vec3 a, Vec3 b)
{
    Vec3 ret;
    ret.x = a.y*b.z - a.z*b.y;
    ret.y = a.z*b.x - a.x*b.z;
    ret.z = a.x*b.y - a.y*b.x;
    return ret;
}

float Vec3_len(Vec3 a)
{
    return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

float Vec3_lenSq(Vec3 a)
{
    return a.x*a.x + a.y*a.y + a.z*a.z;
}

Vec3 Vec3_norm(Vec3 a)
{
    Vec3 ret = a;
    float len = sqrt(ret.x*ret.x + ret.y*ret.y + ret.z*ret.z);
    ret.x /= len;
    ret.y /= len;
    ret.z /= len;
    return ret;
}

Vec3 Vec3_normComponents(Vec3 a)
{
    float max = a.x;
    if (a.y > max) max = a.y;
    if (a.z > max) max = a.z;

    return (Vec3)
    {
        a.x / max,
        a.y / max,
        a.z / max
    };
}
