#include "QCurve.h"

QCurve QCurve_create(Vec3 point0, Vec3 point1, Vec3 deriv, int derivPoint)
{
    QCurve curve;

    if (derivPoint == 0)
    {
        curve._a.x = point1.x - point0.x - deriv.x;
        curve._b.x = deriv.x;
        curve._c.x = point0.x;

        curve._a.y = point1.y - point0.y - deriv.y;
        curve._b.y = deriv.y;
        curve._c.y = point0.y;

        curve._a.z = point1.z - point0.z - deriv.z;
        curve._b.z = deriv.z;
        curve._c.z = point0.z;
    }
    else
    {
        curve._a.x = deriv.x + point0.x - point1.x;
        curve._b.x = 2 * (point1.x - point0.x) - deriv.x;
        curve._c.x = point0.x;

        curve._a.y = deriv.y + point0.y - point1.y;
        curve._b.y = 2 * (point1.y - point0.y) - deriv.y;
        curve._c.y = point0.y;

        curve._a.z = deriv.z + point0.z - point1.z;
        curve._b.z = 2 * (point1.z - point0.z) - deriv.z;
        curve._c.z = point0.z;
    }

    return curve;
}

Vec3 QCurve_interpolate(QCurve curve, float t)
{
    float t2 = t * t;
    return (Vec3)
    {
        curve._a.x * t2 + curve._b.x * t + curve._c.x,
        curve._a.y * t2 + curve._b.y * t + curve._c.y,
        curve._a.z * t2 + curve._b.z * t + curve._c.z
    };
}

Vec3 QCurve_getDerivative(QCurve curve, int derivPoint)
{
    float t = derivPoint == 0 ? 0.0f : 2.0f;
    return (Vec3)
    {
        curve._a.x * t + curve._b.x,
        curve._a.y * t + curve._b.y,
        curve._a.z * t + curve._b.z,
    };
}
