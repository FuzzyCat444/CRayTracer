#include "Mat4.h"

#include <math.h>
#include <stdio.h>

Mat4 Mat4_identity()
{
    return (Mat4) {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

/*
    References used to implement Mat4_inverse:
        - https://stackoverflow.com/questions/2624422/efficient-4x4-matrix-inverse-affine-transform
        - http://mathworld.wolfram.com/MatrixInverse.html
        - https://www.mathsisfun.com/algebra/matrix-determinant.html
*/
Mat4 Mat4_inverse(Mat4 m)
{
    // Row 1
    float det11 = m.a22*m.a33 - m.a23*m.a32;
    float det12 = m.a13*m.a32 - m.a12*m.a33;
    float det13 = m.a12*m.a23 - m.a13*m.a22;

    // Row 2
    float det21 = m.a23*m.a31 - m.a21*m.a33;
    float det22 = m.a11*m.a33 - m.a13*m.a31;
    float det23 = m.a13*m.a21 - m.a11*m.a23;

    // Row 3
    float det31 = m.a21*m.a32 - m.a22*m.a31;
    float det32 = m.a12*m.a31 - m.a11*m.a32;
    float det33 = m.a11*m.a22 - m.a12*m.a21;

    Mat4 invMat3 = {
        .a11=det11, .a12=det12, .a13=det13, .a14=0,
        .a21=det21, .a22=det22, .a23=det23, .a24=0,
        .a31=det31, .a32=det32, .a33=det33, .a34=0,
            .a41=0,     .a42=0,     .a43=0, .a44=1
    };
    float oneOverDetMat3 = 1.0f / (m.a11*det11 + m.a12*det21 + m.a13*det31);
    invMat3 = Mat4_mulScalar(invMat3, oneOverDetMat3);
    invMat3.a44 = 1.0f;

    Mat4 negInvMat3 = Mat4_mulScalar(invMat3, -1.0f);
    negInvMat3.a44 = 1.0f;

    Vec3 right = {.x = m.a14, .y = m.a24, .z = m.a34};
    right = Mat4_mulVec3(negInvMat3, right);

    invMat3.a14 = right.x;
    invMat3.a24 = right.y;
    invMat3.a34 = right.z;

    return invMat3;
}

Mat4 Mat4_mul(Mat4 a, Mat4 b)
{
    Mat4 ret;

    // Row 1
    ret.a11 = a.a11*b.a11 + a.a12*b.a21 + a.a13*b.a31 + a.a14*b.a41;
    ret.a12 = a.a11*b.a12 + a.a12*b.a22 + a.a13*b.a32 + a.a14*b.a42;
    ret.a13 = a.a11*b.a13 + a.a12*b.a23 + a.a13*b.a33 + a.a14*b.a43;
    ret.a14 = a.a11*b.a14 + a.a12*b.a24 + a.a13*b.a34 + a.a14*b.a44;

    // Row 2
    ret.a21 = a.a21*b.a11 + a.a22*b.a21 + a.a23*b.a31 + a.a24*b.a41;
    ret.a22 = a.a21*b.a12 + a.a22*b.a22 + a.a23*b.a32 + a.a24*b.a42;
    ret.a23 = a.a21*b.a13 + a.a22*b.a23 + a.a23*b.a33 + a.a24*b.a43;
    ret.a24 = a.a21*b.a14 + a.a22*b.a24 + a.a23*b.a34 + a.a24*b.a44;

    // Row 3
    ret.a31 = a.a31*b.a11 + a.a32*b.a21 + a.a33*b.a31 + a.a34*b.a41;
    ret.a32 = a.a31*b.a12 + a.a32*b.a22 + a.a33*b.a32 + a.a34*b.a42;
    ret.a33 = a.a31*b.a13 + a.a32*b.a23 + a.a33*b.a33 + a.a34*b.a43;
    ret.a34 = a.a31*b.a14 + a.a32*b.a24 + a.a33*b.a34 + a.a34*b.a44;

    // Row 4
    ret.a41 = a.a41*b.a11 + a.a42*b.a21 + a.a43*b.a31 + a.a44*b.a41;
    ret.a42 = a.a41*b.a12 + a.a42*b.a22 + a.a43*b.a32 + a.a44*b.a42;
    ret.a43 = a.a41*b.a13 + a.a42*b.a23 + a.a43*b.a33 + a.a44*b.a43;
    ret.a44 = a.a41*b.a14 + a.a42*b.a24 + a.a43*b.a34 + a.a44*b.a44;

    return ret;
}

Mat4 Mat4_mulScalar(Mat4 m, float scalar)
{
    // Row 1
    m.a11 *= scalar;
    m.a12 *= scalar;
    m.a13 *= scalar;
    m.a14 *= scalar;

    // Row 2
    m.a21 *= scalar;
    m.a22 *= scalar;
    m.a23 *= scalar;
    m.a24 *= scalar;

    // Row 3
    m.a31 *= scalar;
    m.a32 *= scalar;
    m.a33 *= scalar;
    m.a34 *= scalar;

    // Row 4
    m.a41 *= scalar;
    m.a42 *= scalar;
    m.a43 *= scalar;
    m.a44 *= scalar;

    return m;
}

Vec3 Mat4_mulVec3(Mat4 m, Vec3 v)
{
    return (Vec3) {
        m.a11*v.x + m.a12*v.y + m.a13*v.z + m.a14,
        m.a21*v.x + m.a22*v.y + m.a23*v.z + m.a24,
        m.a31*v.x + m.a32*v.y + m.a33*v.z + m.a34
    };
}

Mat4 Mat4_translate(Vec3 trans)
{
    float x = trans.x;
    float y = trans.y;
    float z = trans.z;
    return (Mat4) {
        1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1
    };
}

Mat4 Mat4_rotateX(float rot)
{
    float s = sin(rot);
    float c = cos(rot);
    return (Mat4) {
        1, 0,  0, 0,
        0, c, -s, 0,
        0, s,  c, 0,
        0, 0,  0, 1
    };
}

Mat4 Mat4_rotateY(float rot)
{
    float s = sin(rot);
    float c = cos(rot);
    return (Mat4) {
        c, 0,  s, 0,
        0, 1,  0, 0,
       -s, 0,  c, 0,
        0, 0,  0, 1
    };
}

Mat4 Mat4_rotateZ(float rot)
{
    float s = sin(rot);
    float c = cos(rot);
    return (Mat4) {
        c, -s, 0, 0,
        s,  c, 0, 0,
        0,  0, 1, 0,
        0,  0, 0, 1
    };
}

Mat4 Mat4_scale(Vec3 scale)
{
    float x = scale.x;
    float y = scale.y;
    float z = scale.z;
    return (Mat4) {
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1
    };
}

void Mat4_print(Mat4 m)
{
    printf("%.3f, %.3f, %.3f, %.3f\n",   m.a11, m.a12, m.a13, m.a14);
    printf("%.3f, %.3f, %.3f, %.3f\n",   m.a21, m.a22, m.a23, m.a24);
    printf("%.3f, %.3f, %.3f, %.3f\n",   m.a31, m.a32, m.a33, m.a34);
    printf("%.3f, %.3f, %.3f, %.3f\n\n", m.a41, m.a42, m.a43, m.a44);
}
