#include <graphics.h>
#include <conio.h>
#include "Road.h"
#include "Map.h"
#include "mathd.h"
#include "EXgraphic.h"
#include <cstdint>
#include <cmath>
#include <forward_list>
#include "SetList.h"
#include <queue>
#include "PhySim.h"
#include "Building.h"

using namespace std;

extern int map[MAPX][MAPY];
extern int mapFP[MAPX][MAPY];
extern int8_t mapBD[MAPX][MAPY];
extern int8_t mapRD[MAPX][MAPY];

extern vecForce mapForceBD[MAPX][MAPY];
extern vecForce mapForceRD[MAPX][MAPY];
extern Building GBuildings[BD_MUN];
extern Road GRoads[RD_MUN];

int8_t RDdiffDir[8][MAPX][MAPY];
int8_t RDdJump[8][MAPX][MAPY];
bool RDisDiff[8][MAPX][MAPY];
int RDdiffDist[8][MAPX][MAPY];
loc RDelm[8][DSTS][ELMS];

void initRDdiffuse(){
    for (int t = 0; t < 8; t++) {
        for (int i = 0; i < MAPX; i++) {
            for (int j = 0; j < MAPY; j++) {
                RDdJump[t][i][j] = -1;
                RDisDiff[t][i][j] = false;
                RDdiffDist[t][i][j] = INT_MAX;
            }
        }
    }
    for (int t = 0; t < 8; t++) {
        for (int i = 0; i < DSTS; i++) {
            for (int j = 0; j < ELMS; j++) {
                RDelm[t][i][j] = loc();
            }
        }
    }
}

void clearRDdiffuse(int trd){
    for (int i = 0; i < MAPX; i++) {
        for (int j = 0; j < MAPY; j++) {
            RDdJump[trd][i][j] = -1;
            RDisDiff[trd][i][j] = false;
            RDdiffDist[trd][i][j] = INT_MAX;
        }
    }
}

Road::Road()
{
	x = 0; y = 0; r = 0; w = 0; l = 0; islife = false; type = 0;
	F0 = vecForce(); Fr = 0.0; Fw = 0.0; Fl = 0.0; v0 = vecForce(); vr = 0.0; vw = 0.0; vl = 0.0;
}

Road::Road(int16_t x0, int16_t y0, float r0, int8_t w0, int8_t l0, int8_t ty0)
{
	x = x0; y = y0; r = r0; w = w0; l = l0; islife = true; type = ty0;
	F0 = vecForce(); Fr = 0.0; Fw = 0.0; Fl = 0.0; v0 = vecForce(); vr = 0.0; vw = 0.0; vl = 0.0;
	getList();
}

void Road::getList()
{
	listMap.clear();
	float cosr = cos(r);
	float sinr = sin(r);
	int x1 = w * cosr - l * sinr;
	int x4 = w * cosr + l * sinr; int x3 = -x1; int x2 = -x4;
	int y1 = l * cosr + w * sinr;
	int y2 = l * cosr - w * sinr; int y3 = -y1; int y4 = -y2;
	int b1 = min(min(x1, x2), min(x3, x4)); int b2 = max(max(x1, x2), max(x3, x4));
	int t1 = min(min(y1, y2), min(y3, y4)); int t2 = max(max(y1, y2), max(y3, y4));
	for (int i = b1; i < b2; i++) {
		for (int j = t1; j < t2; j++) {
			if (j * (x2 - x1) + x1 * y2 > i * (y2 - y1) + x2 * y1
				&& j * (x4 - x3) + x3 * y4 > i * (y4 - y3) + x4 * y3
				&& j * (x2 - x3) + x3 * y2 < i * (y2 - y3) + x2 * y3
				&& j * (x4 - x1) + x1 * y4 < i * (y4 - y1) + x4 * y1) {
				listMap.push_front(loc(i, j));
			}
		}
	}
}


void Road::clearForce()
{
	F0 = vecForce();
	Fr = 0.0;
	Fw = 0.0;
	Fl = 0.0;
}

void Road::getForce()
{
	vecForce netF = vecForce();
	if (type == BD_NULL)return;
	if (x < 21) { F0.x += 2100.0 / x; v0.x = 0; }
	if (x > MAPX - 21) { F0.x -= 2100.0 / (MAPX - x); v0.x = 0; }
	if (y < 21) { F0.y += 2100.0 / y; v0.y = 0; }
	if (y > MAPY - 21) { F0.y -= 2100.0 / (MAPY - y); v0.y = 0; }
	F0 += mapForceRD[x][y];
	//getList();
	for (loc k : listMap) {
		if (k.x + x < 0 || k.x + x >= MAPX || k.y + y < 0 || k.y + y >= MAPY)continue;
		netF += mapForceRD[k.x + x][k.y + y];
		Fr += k.x * mapForceRD[k.x + x][k.y + y].y - k.y * mapForceRD[k.x + x][k.y + y].x;
		vecForce o1 = vecForce(k.x, k.y); o1.routate(-r);
		vecForce o2 = mapForceRD[k.x + x][k.y + y]; o2.routate(-r);
		if (o1.x * o2.x > 0) { Fw++; }
		if (o1.x * o2.x < 0) { Fw--; }
		if (o1.y * o2.y > 0) { Fl++; }
		if (o1.y * o2.y < 0) { Fl--; }
	}
	F0 += netF * 0.01;
	float ew = 1;
	Fw += 1.25 * (ew - w) * (ew - w) * (ew - w);
	Fl += 1.25 * (ew - l) * (ew - l) * (ew - l);
}

void Road::getVelocity()
{
	v0 += F0 * 1.8;
	vr += Fr * 0.01;
	/*
	if (v0.x > 1)
		v0.x = sqrt(v0.x);
	if (v0.x < -1)
		v0.x = -sqrt(-v0.x);
	if (v0.y > 1)
		v0.y = sqrt(v0.y);
	if (v0.y < -1)
		v0.y = -sqrt(-v0.y);*/
	v0.sqrmol();
	if (vr > 0)
		vr = 0.8 * sqrt(vr);
	else
		vr = -0.8 * sqrt(-vr);
	/*
	vw += 0.05 * Fw;
	vl += 0.05 * Fl;
	*/
}

void Road::forceMove()
{
	//x += F0.x; y += F0.y;
	/*
	if (F0.x > 0)x++;
	else if (F0.x < -0)x--;
	if (F0.y > 0)y++;
	else if (F0.y < -0)y--;
	if (Fr > 0)r += 0.05;
	else if (Fr < -0)r -= 0.05;*/
	getVelocity();
	float rs = 0.65;
	if (v0.x > 1) {
		x++; v0.x -= rs;
	}
	if (v0.x < -1) {
		x--; v0.x += rs;
	}
	if (v0.y > 1) {
		y++; v0.y -= rs;
	}
	if (v0.y < -1) {
		y--; v0.y += rs;
	}
	if (vr > 1) {
		r += 0.01; vr -= 0.9;
	}
	if (vr < -1) {
		r -= 0.01; vr += 0.9;
	}
	/*
	if (Fw > 0.0001) {
		w++; vw -= 0.9;
	}
	if (Fw < -0.0001) {
		w--;
	}
	if (Fl > 0.0001) {
		l++; vl -= 0.9;
	}
	if (Fl < -0.0001) {
		l--;
	}*/
}

void Road::write() {
	for (loc k : listMap) {
		if (k.x + x < 0 || k.x + x >= MAPX || k.y + y < 0 || k.y + y >= MAPY)continue;
		mapRD[k.x + x][k.y + y] = 1;
	}
	mapRD[x][y] = 1;
}


void Road::writeForce()
{
	//getList();
	for (loc k : listMap) {
		if (k.x + x < 0 || k.x + x >= MAPX || k.y + y < 0 || k.y + y >= MAPY)continue;
		vecForce f = vecForce(k.x, k.y);
		//f.sqrmol();
		mapForceRD[k.x + x][k.y + y] += f * 115.7;
	}
}

void Road::diffuse(int trd)
{
    clearRDdiffuse(trd);
    int elmHead[DSTS]{};
    int elmSize = 0;
    int nowDist = 0;
    for (loc k : listMap) {
        if (k.x + x < 0 || k.x + x >= MAPX || k.y + y < 0 || k.y + y >= MAPY)continue;
        RDdiffDist[trd][k.x + x][k.y + y] = 0;
        RDdJump[trd][k.x + x][k.y + y] = 0;
        //-------------PUSH-------------
        int elmPt = min(elmHead[0], (ELMS - 1));
        RDelm[trd][0][elmPt] = loc(k.x + x, k.y + y);
        if (elmHead[0] < ELMS) {
            elmHead[0]++;
            elmSize++;
        }
        //-----^^^-----PUSH-----^^^-----
    }
    queue<loc> bdQ;
    int maxDist = 0;
    while (elmSize > 0) {
        //-------------POP-------------
        int CnowDist = nowDist & (DSTS - 1);
        if (elmHead[CnowDist] <= 0) {
            nowDist++;
            continue;
        }
        loc popLoc = RDelm[trd][CnowDist][elmHead[CnowDist] - 1];
        int thisX = popLoc.x;
        int thisY = popLoc.y;
        elmHead[CnowDist]--;
        elmSize--;
        //-----^^^-----POP-----^^^-----
        int jumpDist = 1;
        if (RDisDiff[trd][thisX][thisY])continue;
        if (thisX < 0 || thisX >= MAPX || thisY < 0 || thisY >= MAPY)continue;
        RDisDiff[trd][thisX][thisY] = true;
        int prevDist = RDdiffDist[trd][thisX][thisY];
        if (prevDist >= 80) {
            jumpDist = 2;
            if ((thisX & 1) != 0 || (thisY & 1) != 0)continue;
        }
        if (prevDist >= 160) {
            jumpDist = 4;
            if ((thisX & 3) != 0 || (thisY & 3) != 0)continue;
        }
        if (prevDist >= 320) {
            jumpDist = 8;
            if ((thisX & 7) != 0 || (thisY & 7) != 0)continue;
        }
        if (prevDist >= 640) {
            jumpDist = 16;
            if ((thisX & 15) != 0 || (thisY & 15) != 0)continue;
        }
        if (prevDist >= 1280) {
            jumpDist = 32;
            if ((thisX & 31) != 0 || (thisY & 31) != 0)continue;
        }
        if (mapBD[thisX][thisY] > 0)
            bdQ.push(loc(thisX, thisY));
        for (int8_t dir = 0; dir < 16; dir++) {
            int d1 = 12; int d2 = 17; int d3 = 27;
            int dist = d1;
            if ((dir & 0b10) == 0b10)dist = d2;
            if ((dir & 0b1) == 1)dist = d3;
            int nextX = 0; int nextY = 0;
            map16Dir(dir, &nextX, &nextY);
            nextX *= jumpDist; nextY *= jumpDist;
            nextX += thisX; nextY += thisY;
            if (nextX < 0 || nextX >= MAPX || nextY < 0 || nextY >= MAPY)continue;
            if (RDisDiff[trd][nextX][nextY])continue;
            int dh = abs(map[thisX][thisY] - map[nextX][nextY]) * abs(map[thisX][thisY] - map[nextX][nextY]);
            //int dh = 100/(map[lx][ly]+1);
            dist = dist + dh;
            if (mapRD[nextX][nextY] > 0) { dist = 1; }
            if (mapBD[nextX][nextY] > 0) { dist = 512; }
            //dist *= jumpDist;
            dist = min(dist, DSTS - 1);
            int ddist = dist;
            dist += prevDist;
            if (RDdiffDist[trd][nextX][nextY] > dist) {
                RDdiffDir[trd][nextX][nextY] = dir;
                RDdiffDist[trd][nextX][nextY] = dist;
                RDdJump[trd][nextX][nextY] = jumpDist;
                //-------------PUSH-------------
                int nextDist = nowDist + ddist;
                int CnextDist = nextDist & (DSTS - 1);
                int elmPt = min(elmHead[CnextDist], (ELMS - 1));
                RDelm[trd][CnextDist][elmPt] = loc(nextX, nextY);
                if (elmHead[CnextDist] < ELMS) {
                    elmHead[CnextDist]++;
                    elmSize++;
                }
                //-----^^^-----PUSH-----^^^-----
            }
        }
    }
    //Clear the queue
    vecForce pullF = vecForce();
    while (!bdQ.empty()) {
        int distX = bdQ.front().x;
        int distY = bdQ.front().y;
        bdQ.pop();
        bool change_in_BD = false;
        int8_t prev_BD = mapBD[distX][distY];
        //-----REVERSE-----
        while (RDdJump[trd][distX][distY] != 0) {
            int8_t curn_BD = mapBD[distX][distY];
            if (curn_BD != prev_BD)change_in_BD = true;
            //avoid reverse in one BD
            int nextX = 0; int nextY = 0;
            mapFP[distX][distY]++;
            map16Dir(RDdiffDir[trd][distX][distY], &nextX, &nextY);
            nextX = -nextX; nextY = -nextY;
            nextX *= RDdJump[trd][distX][distY]; nextY *= RDdJump[trd][distX][distY];
            distX += nextX; distY += nextY;
            if (RDdJump[trd][distX][distY] == 0 && change_in_BD) {
                int dfx = 0; int dfy = 0;
                map16Dir(RDdiffDir[trd][distX - nextX][distY - nextY], &dfx, &dfy);
                mapForceRD[distX][distY] += vecForce(dfx, dfy);
                pullF.x += dfx;
                pullF.y += dfy;
            }
        }
    }
    //F0 += pullF * 0.7;
}
