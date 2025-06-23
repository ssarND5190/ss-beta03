#include <graphics.h>
#include <conio.h>
#include "Map.h"
#include <cstdint>
#include "SetList.h"
#include "mathd.h"
#include <vector>
#include "PhySim.h"
#include "Building.h"

extern int map[MAPX][MAPY];
extern int mapRoad[MAPX][MAPY];
extern int8_t mapBD[MAPX][MAPY];
extern float mapVal1[MAPX][MAPY];
extern float mapVal2[MAPX][MAPY];

extern vecForce mapForce[MAPX][MAPY];

extern Building buildings[BD_MUN];

IMAGE IMGmap;

void map8Dir(uint8_t dir, int* lx, int* ly)
{
	dir = dir & 0b111;
	switch (dir) {
	case 0:(*lx)++; break;
	case 1:(*lx)++; (*ly)--; break;
	case 2:(*ly)--; break;
	case 3:(*lx)--; (*ly)--; break;
	case 4:(*lx)--; break;
	case 5:(*lx)--; (*ly)++; break;
	case 6:(*ly)++; break;
	case 7:(*lx)++; (*ly)++; break;
	}
}

void map16Dir(uint8_t dir, int* lx, int* ly)
{
	dir = dir & 0b1111;
	switch (dir) {
	case 0:(*lx)++; break;
	case 1:(*lx) += 2; (*ly)--; break;
	case 2:(*lx)++; (*ly)--; break;
	case 3:(*lx)++; (*ly) -= 2; break;
	case 4:(*ly)--; break;
	case 5:(*lx)--; (*ly) -= 2; break;
	case 6:(*lx)--; (*ly)--; break;
	case 7:(*lx) -= 2; (*ly)--; break;
	case 8:(*lx)--; break;
	case 9:(*lx) -= 2; (*ly)++; break;
	case 10:(*lx)--; (*ly)++; break;
	case 11:(*lx)--; (*ly) += 2; break;
	case 12:(*ly)++; break;
	case 13:(*lx)++; (*ly) += 2; break;
	case 14:(*lx)++; (*ly)++; break;
	case 15:(*lx) += 2; (*ly)++; break;
	}
}

void initmap() {
	loadimage(&IMGmap, _T("tex\\map0-2x.png"));
	DWORD* imb = GetImageBuffer(&IMGmap);
	int k = 0;
	for (int x = 0; x < MAPX; x++) {
		for (int y = 0; y < MAPY; y++) {
			map[y][x] = imb[k] & 0xff;
			mapRoad[y][x] = 0;
			mapBD[y][x] = 0;
			mapVal1[y][x] = 0;
			mapVal2[y][x] = 0;
			k++;
		}
	}
}

void clearMapRoad() {
	for (int ix = 0; ix < MAPX; ix++) {
		for (int iy = 0; iy < MAPY; iy++) {
			mapRoad[ix][iy] = 0;
		}
	}
}

void clearMapBD() {
	for (int ix = 0; ix < MAPX; ix++) {
		for (int iy = 0; iy < MAPY; iy++) {
			mapBD[ix][iy] = 0;
		}
	}
}

void clearMapVal1(){
	for (int ix = 0; ix < MAPX; ix++) {
		for (int iy = 0; iy < MAPY; iy++) {
			mapVal1[ix][iy] = 0;
		}
	}
}

void clearMapVal2() {
	for (int ix = 0; ix < MAPX; ix++) {
		for (int iy = 0; iy < MAPY; iy++) {
			mapVal2[ix][iy] = 0;
		}
	}
}

void writeMapForce()
{
	for (int ix = 0; ix < MAPX; ix++) {
		for (int iy = 0; iy < MAPY; iy++) {
			//mapForce[ix][iy] = vecForce(0,512-iy)*55.0;
			mapForce[ix][iy] = vecForce();
		}
	}
	for (int bd = 0; bd < BD_MUN; bd++) {
		buildings[bd].writeForce();
	}
}

void genrMapVal1() {
	for (int ix = 1; ix < MAPX-1; ix++) {
		for (int iy = 1; iy < MAPY-1; iy++) {
			int b = mapBD[ix][iy];
			if (b <= 0)continue;
			if (mapBD[ix + 1][iy] > 0 && mapBD[ix][iy + 1] > 0 && mapBD[ix - 1][iy] > 0 && mapBD[ix][iy - 1] > 0 &&
				mapBD[ix + 1][iy + 1] > 0 && mapBD[ix - 1][iy - 1] > 0 && mapBD[ix + 1][iy - 1] > 0 && mapBD[ix - 1][iy + 1] > 0)
				continue;
			int w = b + 32;
			for (int iix = ix - w; iix <= ix + w; iix++) {
				for (int iiy = iy - w; iiy <= iy + w; iiy++) {
					if (iix < 0 || iix >= MAPX || iiy < 0 || iiy >= MAPY)continue;
					if (iix == ix && iiy == iy)continue;
					float d = (1.0) / (0.1 + EucDist(ix, iy, iix, iiy));
					mapVal1[iix][iiy] += d;
				}
			}
		}
	}
}

void genrMapVal2(){
	for (int ix = 0; ix < MAPX; ix++) {
		for (int iy = 0; iy < MAPY; iy++) {
			int h = mapRoad[ix][iy];
			if (h <= 0)continue;
			int w = h + 24;
			for (int iix = ix - w; iix <= ix + w; iix++) {
				for (int iiy = iy - w; iiy <= iy + w; iiy++) {
					if (iix < 0 || iix >= MAPX || iiy < 0 || iiy >= MAPY)continue;
					if (iix == ix && iiy == iy)continue;
					float d = (sqrt(h) + 1.0) / (0.1 + OctDist(ix, iy, iix, iiy));
					mapVal2[iix][iiy] += d*d;
				}
			}
		}
	}
}

void RenderMap() {
	DWORD* dst = GetImageBuffer(NULL);
	for (int iy = 0; iy < MAPX; iy++) {
		for (int ix = 0; ix < MAPY; ix++) {
			dst[ix] = (map[ix][iy] << 16) + (map[ix][iy] << 8) + map[ix][iy];
			//dst[ix] = 0;
			//if (mapVal1[ix][iy] > 0)dst[ix] = 65793-65793.0 / (1.0 + 0.2*mapVal1[ix][iy]);
			if (mapBD[ix][iy] > 0) {
				switch (mapBD[ix][iy]) {
				case 1:dst[ix] = 0x111111; break;
				case BD_RESD:dst[ix] = 0xcccc88; break;
				case BD_PROD:dst[ix] = 0x6666cc; break;
				case BD_COMR:dst[ix] = 0xff6644; break;
				case BD_SERV:dst[ix] = 0x66ff44; break;
				}
			}
			if(mapRoad[ix][iy]>0)dst[ix] = HSVtoRGB(229-220.0/mapRoad[ix][iy],0.88,0.70);
		}
		dst += SCRX;
	}
	/*
	for (int iy = 0; iy < MAPX; iy+=4) {
		for (int ix = 0; ix < MAPY; ix+=4) {
			mapForce[ix][iy].putForce(ix, iy);
		}
	}*/
	FlushBatchDraw();
}

void getMapOct(std::vector<loc>& vec, int x, int y, int r)
{
	for (int ix = x - r; ix <= x + r; ix++) {
		for (int iy = y - r; iy <= y + r; iy++) {
			if (ix < 0 || iy < 0 || ix >= MAPX || iy >= MAPY)continue;
			if (OctDist(ix, iy, x, y) > r)continue;
			vec.push_back(loc(ix, iy));
		}
	}
}

int getMapOct_BD(int x, int y, int r) {
	int bd = 0;
	int Area = 0;
	for (int ix = x - r; ix <= x + r; ix++) {
		for (int iy = y - r; iy <= y + r; iy++) {
			if (ix < 0 || iy < 0 || ix >= MAPX || iy >= MAPY)continue;
			if (OctDist(ix, iy, x, y) > r)continue;
			if (mapBD[ix][iy] > 0) {
				bd++;
			}
			Area++;
		}
	}
	return bd;
}

int getMapOctVH(int x, int y, int r) {
	int varienceH = 0;
	int area = 0;
	int averageH = 0;
	for (int ix = x - r; ix <= x + r; ix++) {
		for (int iy = y - r; iy <= y + r; iy++) {
			if (ix < 0 || iy < 0 || ix >= MAPX || iy >= MAPY)continue;
			if (OctDist(ix, iy, x, y) > r)continue;
			averageH += map[ix][iy]; area++;
		}
	}
	for (int ix = x - r; ix <= x + r; ix++) {
		for (int iy = y - r; iy <= y + r; iy++) {
			if (ix < 0 || iy < 0 || ix >= MAPX || iy >= MAPY)continue;
			if (OctDist(ix, iy, x, y) > r)continue;
			varienceH = (averageH - map[ix][iy]) * (averageH - map[ix][iy]);
		}
	}
	return varienceH;
}

float getMapOctVal1(int x, int y, int r) {
	float val = 0;
	for (int ix = x - r; ix <= x + r; ix++) {
		for (int iy = y - r; iy <= y + r; iy++) {
			if (ix < 0 || iy < 0 || ix >= MAPX || iy >= MAPY)continue;
			if (OctDist(ix, iy, x, y) > r)continue;
			val += mapVal1[ix][iy];
		}
	}
	return val;
}

loc::loc(){
	x = 0; y = 0;
}

loc::loc(int lx, int ly){
	x = lx; y = ly;
}

locVal::locVal()
{
	x = 0; y = 0; val = -1.0;
}

locVal::locVal(int lx, int ly, float lval)
{
	x = lx; y = ly; val = lval;
}

locP::locP()
{
	x = 0; y = 0; pt = NULL;
}

locP::locP(int lx, int ly, int* lp)
{
	x = lx; y = ly; pt = lp;
}

bool locP::operator <(const locP& locp)const {
	return *pt < *(locp.pt);
}
bool locP::operator==(const locP& locp)const {
	return *pt == *(locp.pt);
}
bool locP::operator >(const locP& locp)const {
	return *pt > *(locp.pt);
}

locN::locN()
{
	x = 0; y = 0; nxN = NULL;
}

locN::locN(int locx, int locy, locN* nextNode)
{
	x = locx; y = locy; nxN = nextNode;
}

bool locN::end()
{
	return nxN == NULL;
}

locN locN::operator++(int)
{
	return *nxN;
}

void locN::operator*=(locN& nextNode)
{
	nxN = &nextNode;
}
