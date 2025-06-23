#include <graphics.h>
#include <conio.h>
#include <iostream>
#include <cmath>
#include "mathd.h"
#include "SetList.h"

int random(int min, int max)
{
	int rg = max - min;
	if (rg <= 0)return max;
	int ran = rand() % (rg + 1);
	ran += min;
	return ran;
}

int EucDist(int x1, int y1, int x2, int y2)
{
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

int OctDist(int x1, int y1, int x2, int y2)
{
	int xx = abs(x1 - x2);
	int yy = abs(y1 - y2);
	return (xx + yy + max(xx, yy)) >> 1;
}
