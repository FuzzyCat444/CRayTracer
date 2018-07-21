#include "MathFunctions.h"

#include <stdlib.h>

float randomF(float minRange, float maxRange)
{
    return rand() / (float) RAND_MAX * (maxRange - minRange) + minRange;
}
