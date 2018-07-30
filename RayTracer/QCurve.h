#ifndef QCURVE_H_INCLUDED
#define QCURVE_H_INCLUDED

#include "Vec3.h"

typedef struct QCurve
{
    Vec3 _a;
    Vec3 _b;
    Vec3 _c;
} QCurve;

QCurve QCurve_create(Vec3 point0, Vec3 point1, Vec3 deriv, int derivPoint);

Vec3 QCurve_interpolate(QCurve curve, float t);

Vec3 QCurve_getDerivative(QCurve curve, int derivPoint);

#endif // QCURVE_H_INCLUDED
