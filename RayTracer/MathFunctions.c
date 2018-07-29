#include "MathFunctions.h"

#include <stdlib.h>

float MathFunctions_randomF(float minRange, float maxRange)
{
    return rand() / (float) RAND_MAX * (maxRange - minRange) + minRange;
}

int MathFunctions_sameSignF(float a, float b)
{
	return (a >= 0.0f) ^ (b < 0.0f);
}

float MathFunctions_bisectionMethodF(float (*f)(float x, void *data), void *data, float sx, float ex, float sy, int iter)
{
	float mx = 0.0f;
	float my = 0.0f;
	for (int i = 0; i < iter; i++)
	{
		mx = (sx + ex) * 0.5f;
		my = f(mx, data);
		if (MathFunctions_sameSignF(sy, my))
		{
			sx = mx;
			sy = my;
		}
		else
		{
			ex = mx;
		}
	}
	return mx;
}

int MathFunctions_findRootsF(float (*f)(float x, void *data), void *data, float sx, float ex, float roots[], int rootCount, int tries, int iter)
{
	float subInterval = (ex - sx) / (tries + 1);
	float prevX = sx + subInterval;
	float prevY = f(prevX, data);

	int currentRoot = 0;

	while (tries > 0 && currentRoot < rootCount)
	{
		float curX = prevX + subInterval;
		float curY = f(curX, data);
		if (!MathFunctions_sameSignF(prevY, curY))
		{
			roots[currentRoot++] = MathFunctions_bisectionMethodF(f, data, prevX, curX, prevY, iter);
		}

		prevX = curX;
		prevY = curY;
		tries--;
	}

	return currentRoot; // Return number of roots found
}
