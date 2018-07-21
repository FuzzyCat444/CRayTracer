#ifndef MAT4_H_INCLUDED
#define MAT4_H_INCLUDED

#include "Vec3.h"

typedef struct Mat4
{
    float a11, a12, a13, a14,
          a21, a22, a23, a24,
          a31, a32, a33, a34,
          a41, a42, a43, a44;
} Mat4;

Mat4 Mat4_identity();

Mat4 Mat4_inverse(Mat4 m);

Mat4 Mat4_mul(Mat4 a, Mat4 b);

Mat4 Mat4_mulScalar(Mat4 m, float scalar);

Vec3 Mat4_mulVec3(Mat4 m, Vec3 v);

Mat4 Mat4_translate(Vec3 trans);

Mat4 Mat4_rotateX(float rot);
Mat4 Mat4_rotateY(float rot);
Mat4 Mat4_rotateZ(float rot);

Mat4 Mat4_scale(Vec3 scale);

void Mat4_print(Mat4 m);

#endif // MAT4_H_INCLUDED
