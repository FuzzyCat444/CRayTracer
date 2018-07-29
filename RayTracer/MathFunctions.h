#ifndef MATHFUNCTIONS_H_INCLUDED
#define MATHFUNCTIONS_H_INCLUDED

float MathFunctions_randomF(float minRange, float maxRange);

int MathFunctions_sameSignF(float a, float b);

float MathFunctions_bisectionMethodF(float (*f)(float x, void *data), void *data, float sx, float ex, float sy, int iter);

int MathFunctions_findRootsF(float (*f)(float x, void *data), void *data, float sx, float ex, float roots[], int rootCount, int tries, int iter);

#endif // MATHFUNCTIONS_H_INCLUDED
