#include <graphics.h>
#include <conio.h>
#include "Road.h"
#include "Map.h"
#include "mathd.h"
#include "EXgraphic.h"
#include <cstdint>
#include "SetList.h"
#include <queue>
using namespace std;

extern int map[MAPX][MAPY];
extern int mapRoad[MAPX][MAPY];
extern int8_t mapBD[MAPX][MAPY];

#define FR_ISWRITE 0b10000000
#define FR_ISDIFF1 0b1000000
#define FR_ISDIFF2 0b100000
#define FR_IS2 0b10000

//int8_t roadDir[MAPX][MAPY];
//int roadDist[MAPX][MAPY];
/*
struct roadDistCompare {
	bool operator()(loc* a, loc* b) {
		return roadDist[a->x][a->y] > roadDist[b->x][b->y];
	}
};*/

void findRoad(int x1, int y1, int x2, int y2)
{
	uint8_t** roadDir = NULL;
	int** roadDist = NULL;
	roadDir = new uint8_t *[MAPX];
	roadDist = new int *[MAPX];
	for (int i = 0; i < MAPX; i++) {
		roadDir[i] = new uint8_t[MAPY];
		roadDist[i] = new int[MAPY];
	}
	for (int i = 0; i < MAPX; i++) {
		for (int j = 0; j < MAPY; j++) {
			roadDir[i][j] = 0;
			roadDist[i][j] = INT_MAX;
		}
	}
	putSplt(540, 20, YELLOW); FlushBatchDraw();
	if (roadDir == NULL || roadDist == NULL)return;
	if (x1 == x2 && y1 == y2)return;
	if (x1<0 || x2<0 || y1<0 || y2<0 || x1>MAPX || x2>MAPX || y1>MAPY || y2>MAPY)return;
	//clearFindRoad();
	roadDist[x1][y1] = 0;
	roadDist[x2][y2] = 0;
	int met1X = -1; int met1Y = -1;
	int met2X = -1; int met2Y = -1;
	bool isMet = false;
	priority_queue<locP, vector<locP>, greater<locP>> q1;
	priority_queue<locP, vector<locP>, greater<locP>> q2;
	locP locp1 = locP(x1, y1, &roadDist[x1][y1]);
	locP locp2 = locP(x2, y2, &roadDist[x2][y2]);
	q1.push(locp1);
	q2.push(locp2);
	while ((!q1.empty())&& (!q2.empty())) {
		//ROADDIR(int8_t):  isWrite_  isDiff1_  isDiff2_  1or2_(0or1)  direction_ _ _ _
		int xx1 = q1.top().x;
		int yy1 = q1.top().y;
		q1.pop();
		if (xx1 < 0 || xx1 >= MAPX || yy1 < 0 || yy1 >= MAPY)continue;
		//if (isDiff[xx1][yy1]==1)continue;
		if ((roadDir[xx1][yy1] & FR_ISDIFF1) == FR_ISDIFF1)continue;
		roadDir[xx1][yy1] += FR_ISDIFF1;
		int prevDist = roadDist[xx1][yy1];
		for (uint8_t dir = 0; dir < 16; dir++) {
			uint8_t rdir = FR_ISWRITE + dir;
			int d1 = 12; int d2 = 17; int d3 = 27;
			int dist = d1;
			if ((dir & 0b10) == 0b10)dist = d2;
			if ((dir & 0b1) == 1)dist = d3;
			int lx = 0; int ly = 0;
			map16Dir(dir, &lx, &ly);
			lx *= 4; ly *= 4;
			lx += xx1; ly += yy1;
			if (lx < 0 || lx >= MAPX || ly < 0 || ly >= MAPY)continue;
			if ((roadDir[lx][ly] & FR_IS2) == FR_IS2 &&
				(roadDir[lx][ly] & FR_ISWRITE) == FR_ISWRITE) {
				met1X = xx1; met1Y = yy1;
				met2X = lx; met2Y = ly;
				isMet = true;
				break;
			}
			//if (isDiff[lx][ly]==1)continue;
			if ((roadDir[lx][ly] & FR_ISDIFF1) == FR_ISDIFF1)continue;
			//if (OctDist(lx, ly, x1, y1) + OctDist(lx, ly, x2, y2) > 1.4* OctDist(x1, y1, x2, y2))continue;
			if (OctDist(lx, ly, x1, y1) + OctDist(lx, ly, x2, y2) > 1.5* OctDist(x1, y1, x2, y2)+50)continue;
			int dh = abs(map[xx1][yy1] - map[lx][ly])* abs(map[xx1][yy1] - map[lx][ly]);
			//int dh = 100/(map[lx][ly]+1);
			dist = (dist * 2 + dh) / (1 + mapRoad[lx][ly]) + dh;
			if (mapBD[lx][ly] > 0) { dist += 1000; }
			dist += prevDist;
			if(roadDist[lx][ly]>dist){
				roadDir[lx][ly] = rdir;
				roadDist[lx][ly] = dist;
				q1.push(locP(lx, ly, &roadDist[lx][ly]));
			}
		}
		if (isMet)break;
		//-------------------------------------------------------------------------------------
		int xx2 = q2.top().x;
		int yy2 = q2.top().y;
		q2.pop();
		if (xx2 < 0 || xx2 >= MAPX || yy2 < 0 || yy2 >= MAPY)continue;
		//if (isDiff[xx2][yy2]==3)continue;
		if ((roadDir[xx2][yy2] & FR_ISDIFF2) == FR_ISDIFF2)continue;
		roadDir[xx2][yy2] += FR_ISDIFF2;
		int prevDist2 = roadDist[xx2][yy2];
		for (uint8_t dir = 0; dir < 16; dir++) {
			uint8_t rdir = FR_ISWRITE + FR_IS2 + dir;
			int d1 = 12; int d2 = 17; int d3 = 27;
			int dist = d1;
			if ((dir & 0b10) == 0b10)dist = d2;
			if ((dir & 0b1) == 1)dist = d3;
			int lx = 0; int ly = 0;
			map16Dir(dir, &lx, &ly);
			lx *= 4; ly *= 4;
			lx += xx2; ly += yy2;
			if (lx < 0 || lx >= MAPX || ly < 0 || ly >= MAPY)continue;
			if ((roadDir[lx][ly] & FR_IS2) == 0 &&
				(roadDir[lx][ly] & FR_ISWRITE) == FR_ISWRITE) {
				met1X = lx; met1Y = ly;
				met2X = xx2; met2Y = yy2;
				isMet = true;
				break;
			}
			//if (isDiff[lx][ly]==3)continue;
			if ((roadDir[lx][ly] & FR_ISDIFF2) == FR_ISDIFF2)continue;
			//if (OctDist(lx, ly, x1, y1) + OctDist(lx, ly, x2, y2) > 1.5 * OctDist(x1, y1, x2, y2) + 50)continue;
			int dh = abs(map[xx2][yy2] - map[lx][ly]) * abs(map[xx2][yy2] - map[lx][ly]);
			//int dh = 100/(map[lx][ly]+1);
			dist = (dist * 2 + dh) / (1 + mapRoad[lx][ly]) + dh;
			if (mapBD[lx][ly] > 0) { dist += 1000; }
			dist += prevDist2;
			if (roadDist[lx][ly] > dist) {
				roadDir[lx][ly] = rdir;
				roadDist[lx][ly] = dist;
				q2.push(locP(lx, ly, &roadDist[lx][ly]));
			}
		}
		if (isMet)break;
		/*
		DWORD* dst = GetImageBuffer(NULL);
		for (int iy = 0; iy < MAPX; iy++) {
			for (int ix = 0; ix < MAPY; ix++) {
				if(roadDist[ix][iy]<INT_MAX)
				dst[ix] = roadDist[ix][iy];
			}
			dst += SCRX;
		}
		//putpixel(520, q.size()/4, RED);
		//putpixel(521, q.size()/4, RED);
		//putpixel(522, q.size()/4, RED);
		FlushBatchDraw();
		//*/
		//if (xx1 == x2 && yy1 == y2)break;
	}
	if (met1X == -1 || met1Y == -1 || met2X == -1 || met2Y == -1)return;
	int lx1m = met1X; int ly1m = met1Y;
	int lx2m = met2X; int ly2m = met2Y;
	while (true) {
		if (lx1m != x1 || ly1m != y1) {
			uint8_t dir = roadDir[lx1m][ly1m];
			mapRoad[lx1m][ly1m]++;
			mapRoad[lx1m + 1][ly1m + 1]++; mapRoad[lx1m - 1][ly1m - 1]++;
			mapRoad[lx1m - 1][ly1m + 1]++; mapRoad[lx1m + 1][ly1m - 1]++;
			int lx = 0; int ly = 0;
			map16Dir(dir, &lx, &ly);
			lx *= 4; ly *= 4;
			lx1m -= lx; ly1m -= ly;
		}
		if (lx2m != x2 || ly2m != y2) {
			uint8_t dir = roadDir[lx2m][ly2m];
			mapRoad[lx2m][ly2m]++;
			mapRoad[lx2m + 1][ly2m + 1]++; mapRoad[lx2m - 1][ly2m - 1]++;
			mapRoad[lx2m - 1][ly2m + 1]++; mapRoad[lx2m + 1][ly2m - 1]++;
			int lx = 0; int ly = 0;
			map16Dir(dir, &lx, &ly);
			lx *= 4; ly *= 4;
			lx2m -= lx; ly2m -= ly;
		}
		if (lx1m == x1 && ly1m == y1 && lx2m == x2 && ly2m == y2)break;
		/*
		RenderMap();
		putSpot(x1, y1, GREEN);
		putSpot(x2, y2, BLUE);
		putSpot(lx1m, ly1m, RED);
		putSpot(lx2m, ly2m, RED);
		FlushBatchDraw();
		//Sleep(10/(1+mapRoad[lx2][ly2]));
		//*/
	}
	
	for (int i = 0; i < MAPX; i++) {
		delete[] roadDir[i];
		delete[] roadDist[i];
	}
	delete[] roadDir;
	delete[] roadDist;
}
/*
void clearFindRoad(){
	for (int x = 0; x < MAPX; x++) {
		for (int y = 0; y < MAPY; y++) {
			roadDir[x][y] = -1;
			roadDist[x][y] = INT_MAX;
		}
	}
}*/
