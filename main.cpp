#include <graphics.h>
#include <conio.h>
#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <thread>
#include <atomic>
#include <forward_list>
#include "EXgraphic.h"
#include "Map.h"
#include "Road.h"
#include "mathd.h"
#include "SetList.h"
#include "Building.h"
#include "PhySim.h"
#include <thread>

using namespace std;

int map[MAPX][MAPY];
int mapRoad[MAPX][MAPY];
int8_t mapBD[MAPX][MAPY];
float mapVal1[MAPX][MAPY];
float mapVal2[MAPX][MAPY];

vecForce mapForce[MAPX][MAPY];

Building buildings[BD_MUN];

void initItems() {
	for (int i = 0; i < BD_MUN; i++) {
		Building* bd = new Building();
		int x = random(55, MAPX-55);
		int y = random(55, MAPY-55);
		float r = random(0, 88);
		int type = random(1, 4);
		//x *= 2; y *= 2;
		r /= 7;
		int ww = 16;
		if (i > 32 && type == 4)type = 3;
		if (type == 3)ww = 3;
		*bd = Building(x, y, r, ww+1+type, ww+type, 1, type*16);
		buildings[i] = *bd;
	}
}

void bddf(int i) {
	buildings[i].diffuse();
}

int main() {
	initgraph(SCRX, SCRY);
	BeginBatchDraw();
	srand((unsigned)time(NULL));
	initmap();
	initItems();
	//buildings[0]= Building(500, 500, 0, 11, 11, 1, 0x10);
	//buildings[1]= Building(520, 500, 0, 11, 11, 1, 0x20);
	int count = 0;
	for (int i = 0; i < BD_MUN; i++) {
		buildings[i].write();
	}
	while (true) {
		//cleardevice();
		putSpot(SCRX - 16, count, GREEN);
		clearMapBD();
		clearMapRoad();
		writeMapForce();
		for (int i = 0; i < BD_MUN; i++) {
			buildings[i].getList();
			buildings[i].write();
		}
		/*
		thread th[BD_MUN];
		for (int i = 0; i < BD_MUN; i++) {
			th[i] = thread(bddf, i);
		}
		for (int i = 0; i < BD_MUN; i++) {
			th[i].join();
		}*/
		//*
		for (int i = 0; i < BD_MUN; i++) {
			buildings[i].diffuse();
		}//*/
		for (int i = 0; i < BD_MUN; i++) {
			buildings[i].getForce();
		}
		for (int i = 0; i < BD_MUN; i++) {
			buildings[i].forceMove();
		}
		RenderMap();
		FlushBatchDraw();
		//Sleep(100);
		count++;
	}
	return 0;
}