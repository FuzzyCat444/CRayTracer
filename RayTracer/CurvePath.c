#include "CurvePath.h"

#include <stdlib.h>
#include <math.h>

struct CurvePath
{
    Vec3 prevPoint;
    Vec3 prevDeriv;

    QCurve *curves;
    int curvesPtr;
    int curvesSize;
};

CurvePath *CurvePath_create(Vec3 startPoint, Vec3 startDeriv)
{
    CurvePath *path = malloc(sizeof *path);
   if (path)
   {
        path->curves = malloc(sizeof *path->curves);
        if (!path->curves)
        {
            CurvePath_destroy(path);
            path = NULL;
        }
        else
        {
            path->prevPoint = startPoint;
            path->prevDeriv = startDeriv;

            path->curvesPtr = 0;
            path->curvesSize = 1;
        }
    }

    return path;
}

void CurvePath_addWaypoint(CurvePath *path, Vec3 point)
{
    int canAdd = 1;
    if (path->curvesPtr >= path->curvesSize)
    {
        int newSize = path->curvesSize * 2;
        QCurve *newArr = realloc(path->curves, sizeof *path->curves * newSize);
        if (newArr)
        {
            path->curvesSize = newSize;
            path->curves = newArr;
        }
        else
        {
            canAdd = 0;
        }
    }

    if (canAdd)
    {
        QCurve curve = QCurve_create(path->prevPoint, point, path->prevDeriv, 0);
        path->curves[path->curvesPtr++] = curve;
        path->prevPoint = point;
        path->prevDeriv = QCurve_getDerivative(curve, 1);
    }
}

// t ranges from 0-1
Vec3 CurvePath_interpolate(CurvePath *path, float t)
{
    float totalT = t * path->curvesSize;
    int curveIndex = (int) floor(totalT);
    float curveT = fmod(totalT, 1.0f);
    if (curveIndex >= path->curvesPtr)
    {
        curveIndex = path->curvesPtr - 1;
        curveT = 1.0f;
    }
    if (curveIndex < 0)
    {
        return (Vec3) {0.0f};
    }
    return QCurve_interpolate(path->curves[curveIndex], curveT);
}

void CurvePath_destroy(CurvePath *path)
{
    free(path->curves);
    free(path);
}
