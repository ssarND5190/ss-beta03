#include <graphics.h>
#include <conio.h>
#include "EXgraphic.h"

void putAlpixel(int x, int y, COLORREF cl)
{
	if (x < 0 || x >= 610 || y < 0 || y >= 160)return;
	DWORD* gbuf = GetImageBuffer();
	COLORREF cl1 = gbuf[y * 610 + x];
	int r0 = GetRValue(cl);
	int r1 = GetRValue(cl1);
	int g0 = GetGValue(cl);
	int g1 = GetGValue(cl1);
	int b0 = GetBValue(cl);
	int b1 = GetBValue(cl1);
	r0 = (r0 + r1) / 2; g0 = (g0 + g1) / 2; b0 = (b0 + b1) / 2;
	cl1 = RGB(r0, g0, b0);
	putpixel(x, y, cl1);
}

void putSpot(int x, int y, COLORREF cl)
{
	putpixel(x, y+1, cl); putpixel(x+1, y, cl);
	putpixel(x, y+2, cl); putpixel(x+2, y, cl);
	putpixel(x, y-1, cl); putpixel(x-1, y, cl);
	putpixel(x, y-2, cl); putpixel(x-2, y, cl);
	putpixel(x+1, y-1, cl); putpixel(x-1, y+1, cl);
	putpixel(x+1, y+1, cl); putpixel(x-1, y-1, cl);
}

void putSplt(int x, int y, COLORREF cl)
{
	putpixel(x, y, cl);
	putpixel(x, y + 3, cl); putpixel(x + 3, y, cl);
	putpixel(x, y + 2, cl); putpixel(x + 2, y, cl);
	putpixel(x, y - 3, cl); putpixel(x - 3, y, cl);
	putpixel(x, y - 2, cl); putpixel(x - 2, y, cl);
	putpixel(x + 1, y - 1, cl); putpixel(x - 1, y + 1, cl);
	putpixel(x + 1, y + 1, cl); putpixel(x - 1, y - 1, cl);
}
