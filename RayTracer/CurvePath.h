#ifndef CURVEPATH_H_INCLUDED
#define CURVEPATH_H_INCLUDED

#include "QCurve.h"

typedef struct CurvePath CurvePath;

CurvePath *CurvePath_create(Vec3 startPoint, Vec3 startDeriv);

void CurvePath_addWaypoint(CurvePath *path, Vec3 point);

Vec3 CurvePath_interpolate(CurvePath *path, float t);

void CurvePath_destroy(CurvePath *path);

#endif // CURVEPATH_H_INCLUDED
