#include "Building.h"
#include <graphics.h>
#include <conio.h>
#include "Map.h"
#include <cstdint>
#include <cmath>
#include <forward_list>
#include "SetList.h"
#include "mathd.h"
#include "EXgraphic.h"
#include "PhySim.h"
#include <queue>

using namespace std;

extern int map[MAPX][MAPY];
extern int mapRoad[MAPX][MAPY];
extern int8_t mapBD[MAPX][MAPY];
extern float mapVal1[MAPX][MAPY];
extern float mapVal2[MAPX][MAPY];

extern vecForce mapForce[MAPX][MAPY];
extern Building buildings[BD_MUN];

int8_t diffDir[MAPX][MAPY];
int8_t dJump[MAPX][MAPY];
bool isDiff[MAPX][MAPY];
int diffDist[MAPX][MAPY];
loc elm[DSTS][ELMS];

void clearBDdiffuse()
{
    for (int i = 0; i < MAPX; i++) {
        for (int j = 0; j < MAPY; j++) {
            //diffDir[i][j] = -1;
            dJump[i][j] = -1;
            isDiff[i][j] = false;
            diffDist[i][j] = INT_MAX;
        }
    }
    /*
    for (int i = 0; i < DSTS; i++) {
        for (int j = 0; j < ELMS; j++) {
            elm[i][j] = loc();
        }
    }*/
}

Building::Building()
{
    x = 0; y = 0; r = 0; w = 0; l = 0; decks = 0; islife = false; score = 0; type = 0;
    F0 = vecForce(); Fr = 0.0; Fw = 0.0; Fl = 0.0; v0 = vecForce(); vr = 0.0; vw = 0.0; vl = 0.0;
}

Building::Building(int16_t x0, int16_t y0, float r0, int8_t w0, int8_t l0, int8_t d0, int8_t ty0)
{
    x = x0; y = y0; r = r0; w = w0; l = l0; decks = d0; islife = true; score = 0; type = ty0;
    F0 = vecForce(); Fr = 0.0; Fw = 0.0; Fl = 0.0; v0 = vecForce(); vr = 0.0; vw = 0.0; vl = 0.0;
    getList();
}

void Building::aloc(int iw, int il)
{
    islife = true;
    w = iw; l = il; r = 0.89;
    getList();
    int Area = getArea();
    float maxVal1 = -1.0;
    float maxVal = -1.0;
    int maxX1 = 0; int maxY1 = 0;
    for (int ir = 0; ir < 22; ir++) {
        float fr = ir / 7.0;
        r = fr;
        getList2x();
        int ts1 = 0;
        while (ts1 < 400) {
            ts1++;
            int ix = random(11, 999);
            int iy = random(11, 999);
            x = ix; y = iy;
            getScore();
            if (score > maxVal1) {
                maxVal1 = score; maxX1 = ix; maxY1 = iy;
            }
        }
    }
    int maxX = 0; int maxY = 0; float maxR = 0;
    for (int ir = 0; ir < 22; ir++) {
        float fr = ir / 7.0;
        r = fr;
        getList();
        for (int iy = maxY1 - 21; iy < maxY1 + 21; iy += 2) {
            for (int ix = maxX1 + 21; ix > maxX1 - 21; ix -= 2) {
                x = ix; y = iy;
                getScore();
                /*
                write();
                RenderMap();*/
                if (score > maxVal) {
                    maxVal = score; maxX = ix; maxY = iy; maxR = fr;
                }
            }
        }
    }
    x = maxX; y = maxY; r = maxR;
    score = maxVal;
    getList();
}

void Building::getForce()
{
    //getList();
    F0 = vecForce();
    Fr = 0.0;
    Fw = 0.0;
    Fl = 0.0;
    vecForce netF = vecForce();
    if (type == BD_NULL)return;
    if (x < 21) { F0.x += 2100.0 / x; v0.x = 0; }
    if (x > MAPX - 21) { F0.x -= 2100.0 / (MAPX - x); v0.x = 0; }
    if (y < 21) { F0.y += 2100.0 / y; v0.y = 0; }
    if (y > MAPY - 21) { F0.y -= 2100.0 / (MAPY - y); v0.y = 0; }
    F0 += mapForce[x][y];
    //getList();
    for (loc k : listMap) {
        if (k.x + x<0 || k.x + x>=MAPX || k.y + y<0 || k.y + y>=MAPY)continue;
        netF += mapForce[k.x + x][k.y + y];
        Fr += k.x * mapForce[k.x + x][k.y + y].y - k.y * mapForce[k.x + x][k.y + y].x;
        vecForce o1 = vecForce(k.x, k.y); o1.routate(-r);
        vecForce o2 = mapForce[k.x + x][k.y + y]; o2.routate(-r);
        if (o1.x * o2.x > 0) { Fw++; }
        if (o1.x * o2.x < 0) { Fw--; }
        if (o1.y * o2.y > 0) { Fl++; }
        if (o1.y * o2.y < 0) { Fl--; }
    }
    F0 += netF * 0.01;
    float ew = 1;
    switch (type) {
    case 0x10:ew = 5; break;
    case 0x20:ew = 4; break;
    case 0x30:ew = 3; break;
    case 0x40:ew = 6; break;
    }
    Fw += 1.25 * (ew - w) * (ew - w) * (ew - w);
    Fl += 1.25 * (ew - l) * (ew - l) * (ew - l);
    for (int i = 0; i < BD_MUN; i++) {
        if (&buildings[i] == this)continue;
        float dist = EucDist(x, y, buildings[i].x, buildings[i].y) + 1.0;
        float dist2 = 11.0 / (dist * dist);
        float dist3 = 111.0 / (dist * dist * dist);
        float dist4 = 111.0 / (dist * dist * dist * dist);
        int dx = buildings[i].x - x;
        int dy = buildings[i].y - y;
        int IItype = (((int)type) << 8) + buildings[i].type;
        float FK = 0.0;
        switch (IItype) {
        case 0x1010: FK = -0.1; break;
        case 0x1020: FK = 5.1; break;
        case 0x1030: FK = 1.5; break;
        case 0x2010: FK = 0.2; break;
        case 0x2020: FK = 8.1; break;
        case 0x3010: FK = 4.5; break;
        case 0x3020: FK = 0.7; break;
        case 0x3030: FK = -1.1; break;
        case 0x4010: FK = 1.1; break;
        case 0x4020: FK = 1.1; break;
        case 0x4030: FK = 1.1; break;
        case 0x4040: FK = -2.5; break;
        }
        if (dist > 8) {
            if(FK > 0){
                F0.x += FK * dx * dist2;
                F0.y += FK * dy * dist2;
            }
            if (FK < 0) {
                F0.x += FK * dx * dist3;
                F0.y += FK * dy * dist3;
            }
        }
        if (dist < 8) {
            F0.x -= dx * dist4;
            F0.y -= dy * dist4;
        }
    }
}

void Building::getVelocity()
{
    v0 += F0 * 0.8;
    vr += Fr * 0.001;
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
        vr = 0.8*sqrt(vr);
    else
        vr = -0.8*sqrt(-vr);

    vw += 0.05 * Fw;
    vl += 0.05 * Fl;
}

void Building::forceMove()
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
    }
}

void Building::gradianMove()
{
    getList();
    getScore();
    double prevScore = score;
    int dx = random(-4, 4)*2;
    int dy = random(-4, 4)*2;
    int dw = random(-1, 1);
    int dl = random(-1, 1);
    //float dr = random(-3, 3) / 7.0;
    x += dx; y += dy; //r += dr; 
    //w += dw; l += dl;
    getScore();
    if (score <= prevScore) {
        x -= dx; y -= dy; //r -= dr; 
        //w -= dw; l -= dl;
        return;
    }
}

void Building::diffuse(int trd)
{
    clearBDdiffuse();
    int elmHead[DSTS];
    int elmSize = 0;
    for (int i = 0; i < DSTS; i++)elmHead[i] = 0;
    //roadDist[x1][y1] = 0;
    //priority_queue<locP, vector<locP>, greater<locP>> q;
    //locP locp1 = locP(x1, y1, &roadDist[x1][y1]);
    int nowDist = 0;
    for (loc k : listMap) {
        if (k.x + x < 0 || k.x + x >= MAPX || k.y + y < 0 || k.y + y >= MAPY)continue;
        //if (((k.x + x) & 1) != 0 || ((k.y + y) & 1) != 0)continue;
        diffDist[k.x + x][k.y + y] = 0;
        dJump[k.x + x][k.y + y] = 0;
        //-------------PUSH-------------
        int elmPt = min(elmHead[0], (ELMS-1));
        elm[0][elmPt] = loc(k.x + x, k.y + y);
        if (elmHead[0] < ELMS) {
            elmHead[0]++;
            elmSize++;
        }
        //-----^^^-----PUSH-----^^^-----
        //q.push(locp1);
    }
    queue<loc> bdQ;
    int maxDist = 0;
    while (elmSize > 0) {
        //-------------POP-------------
        int CnowDist = nowDist & (DSTS-1);
        if(elmHead[CnowDist] <= 0) {
            nowDist++;
            continue;
        }
        loc popLoc = elm[CnowDist][elmHead[CnowDist] - 1];
        int thisX = popLoc.x;
        int thisY = popLoc.y;
        elmHead[CnowDist]--;
        elmSize--;
        //-----^^^-----POP-----^^^-----
        //q.pop();
        int jumpDist = 1;
        if (isDiff[thisX][thisY])continue;
        if (thisX < 0 || thisX >= MAPX || thisY < 0 || thisY >= MAPY)continue;
        isDiff[thisX][thisY] = true;
        int prevDist = diffDist[thisX][thisY];
        if (prevDist >= 20) {
            jumpDist = 2;
            if ((thisX & 1) != 0 || (thisY & 1) != 0)continue;
        }
        if (prevDist >= 80) {
            jumpDist = 4;
            if ((thisX & 3) != 0 || (thisY & 3) != 0)continue;
        }
        if (prevDist >= 200) {
            jumpDist = 8;
            if ((thisX & 7) != 0 || (thisY & 7) != 0)continue;
        }
        if (prevDist >= 400) {
            jumpDist = 16;
            if ((thisX & 15) != 0 || (thisY & 15) != 0)continue;
        }
        if (prevDist >= 800) {
            jumpDist = 32;
            if ((thisX & 31) != 0 || (thisY & 31) != 0)continue;
        }
        if (mapBD[thisX][thisY] > 0)bdQ.push(loc(thisX, thisY));
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
            if (isDiff[nextX][nextY])continue;
            int dh = abs(map[thisX][thisY] - map[nextX][nextY]) * abs(map[thisX][thisY] - map[nextX][nextY]);
            //int dh = 100/(map[lx][ly]+1);
            dist = dist + dh;
            if (mapBD[nextX][nextY] > 0) { dist = 512; }
            //dist *= jumpDist;
            dist = min(dist, DSTS-1);
            int ddist = dist;
            dist += prevDist;
            if (diffDist[nextX][nextY] > dist) {
                diffDir[nextX][nextY] = dir;
                diffDist[nextX][nextY] = dist;
                dJump[nextX][nextY] = jumpDist;
                //-------------PUSH-------------
                int nextDist = nowDist + ddist;
                int CnextDist = nextDist & (DSTS-1);
                int elmPt = min(elmHead[CnextDist], (ELMS-1));
                elm[CnextDist][elmPt] = loc(nextX, nextY);
                if (elmHead[CnextDist] < ELMS) {
                    elmHead[CnextDist]++;
                    elmSize++;
                }
                //-----^^^-----PUSH-----^^^-----
                //q.push(locP(nextX, nextY, &diffDist[nextX][nextY]));
            }
        }
        /*
        DWORD* dst = GetImageBuffer(NULL);
        for (int iy = 0; iy < MAPX; iy++) {
            for (int ix = 0; ix < MAPY; ix++) {
                if (diffDist[ix][iy] < INT_MAX)
                    dst[ix] = diffDist[ix][iy];
                //if (dJump[ix][iy] > 0)dst[ix] = GREEN;
            }
            dst += SCRX;
        }
        putSplt(1030, bdQ.size(), GREEN);
        FlushBatchDraw();
        //*/
    }
    while (!bdQ.empty()) {
        int distX = bdQ.front().x;
        int distY = bdQ.front().y;
        bdQ.pop();
        while (dJump[distX][distY] != 0) {
            int nextX = 0; int nextY = 0;
            mapRoad[distX][distY]++;
            map16Dir(diffDir[distX][distY], &nextX, &nextY);
            nextX = -nextX; nextY = -nextY;
            nextX *= dJump[distX][distY]; nextY *= dJump[distX][distY];
            distX += nextX; distY += nextY;
        }
    }
}

void Building::getScore(){
    int Area = getArea();
    score = 1000.0 * (Area - 0.001 * Area * Area);
    int bdXX = 0;
    int dd = 0;
    for (int i = 0; i < BD_MUN; i++) {
        if (&buildings[i] == this)continue;
        bdXX += getIntersect(buildings[i]);
        int ddd = EucDist(x, y, buildings[i].x, buildings[i].y);
        dd += ddd;
    }/*
    getList2x();
    for (loc k : listMap) {
        if (k.x + x<0 || k.x + x>MAPX || k.y + y<0 || k.y + y>MAPY) { continue; }
        score += map[k.x + x][k.y + y];
    }*/
    score /= 1.0 + bdXX/11;
    score /= 1 + dd;
}

void Building::getList()
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

void Building::getList2x()
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
    b1 = (b1 | 1) + 1;
    t1 = (t1 | 1) + 1;
    for (int i = b1; i < b2; i+=2) {
        for (int j = t1; j < t2; j+=2) {
            if (j * (x2 - x1) + x1 * y2 > i * (y2 - y1) + x2 * y1
                && j * (x4 - x3) + x3 * y4 > i * (y4 - y3) + x4 * y3
                && j * (x2 - x3) + x3 * y2 < i * (y2 - y3) + x2 * y3
                && j * (x4 - x1) + x1 * y4 < i * (y4 - y1) + x4 * y1) {
                listMap.push_front(loc(i, j));
            }
        }
    }
}

int Building::getArea(){
    return (w + w + 1) * (l + l + 1);
}

int Building::getIntersect(Building bd)
{
    if (EucDist(x, y, bd.x, bd.y) > 5)return 0;
    //else return 30 - EucDist(x, y, bd.x, bd.y);
    ///*
    getList2x();
    bd.getList2x();
    int intersect = 0;
    for (loc k1 : listMap) {
        if (k1.x + x<0 || k1.x + x>MAPX || k1.y + y<0 || k1.y + y>MAPY) { continue; }
        for (loc k2 : bd.listMap) {
            if (k2.x + x<0 || k2.x + x>MAPX || k2.y + y<0 || k2.y + y>MAPY) { continue; }
            if (k1.x + x == k2.x + bd.x && k1.y + y == k2.y + bd.y)
                intersect++;
        }
    }
    return intersect;//*/
}

int Building::getDH()
{
    int dh = 0;
    int avh = 0;
    int areal = 0;
    for (loc k : listMap) {
        if (k.x + x<0 || k.x + x>MAPX || k.y + y<0 || k.y + y>MAPY) { areal++; continue; }
        areal++;
        avh += map[k.x + x][k.y + y];
    }
    if (areal == 0) { return 0; }
    avh /= areal;
    for (loc k : listMap) {
        if (k.x + x<0 || k.x + x>MAPX || k.y + y<0 || k.y + y>MAPY)continue;
        dh += abs(map[k.x + x][k.y + y] - avh);
    }
	return dh;
}

int Building::getVH()
{
    int dh = 0;
    int avh = 0;
    int areal = 0;
    for (loc k : listMap) {
        if (k.x + x<0 || k.x + x>MAPX || k.y + y<0 || k.y + y>MAPY)continue;
        areal++;
        avh += map[k.x + x][k.y + y];
    }
    if (areal == 0) { return 0; }
    avh /= areal;
    for (loc k : listMap) {
        if (k.x + x<0 || k.x + x>MAPX || k.y + y<0 || k.y + y>MAPY)continue;
        dh += (map[k.x + x][k.y + y] - avh) * (map[k.x + x][k.y + y] - avh);
    }
    return dh;
}

void Building::write(){
    for (loc k : listMap) {
        if (k.x + x<0 || k.x + x>=MAPX || k.y + y<0 || k.y + y>=MAPY)continue;
        mapBD[k.x + x][k.y + y] = type;
        if (type == 0) {
            mapBD[k.x + x][k.y + y] = 1;
        }
    }
    mapBD[x][y] = 1;
}

void Building::writeVal(){
    for (loc k : listMap) {
        if (k.x + x<=0 || k.x + x>=MAPX || k.y + y<=0 || k.y + y>=MAPY)continue;
        int ix = k.x + x; int iy = k.y + y;
        int b = mapBD[ix][iy];
        if (b <= 0)continue;
        if (mapBD[ix + 1][iy] > 0 && mapBD[ix][iy + 1] > 0 && mapBD[ix - 1][iy] > 0 && mapBD[ix][iy - 1] > 0 &&
            mapBD[ix + 1][iy + 1] > 0 && mapBD[ix - 1][iy - 1] > 0 && mapBD[ix + 1][iy - 1] > 0 && mapBD[ix - 1][iy + 1] > 0)
            continue;
        int w = 32;
        for (int iix = ix - w; iix <= ix + w; iix++) {
            for (int iiy = iy - w; iiy <= iy + w; iiy++) {
                if (iix < 0 || iix >= MAPX || iiy < 0 || iiy >= MAPY)continue;
                float d = (4.0) / (0.1 + OctDist(ix, iy, iix, iiy));
                mapVal1[iix][iiy] += d*d;
            }
        }
    }
}

void Building::writeForce()
{
    //getList();
    for (loc k : listMap) {
        if (k.x + x<0 || k.x + x>=MAPX || k.y + y<0 || k.y + y>=MAPY)continue;
        vecForce f = vecForce(k.x, k.y); 
        //f.sqrmol();
        mapForce[k.x + x][k.y + y] += f * 115.7;
    }
}
