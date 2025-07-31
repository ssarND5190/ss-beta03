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
int mapFP[MAPX][MAPY];
//mapFootPrints
int8_t mapBD[MAPX][MAPY];
int8_t mapRD[MAPX][MAPY];
float mapVal1[MAPX][MAPY];
float mapVal2[MAPX][MAPY];

vecForce mapForceBD[MAPX][MAPY];
vecForce mapForceRD[MAPX][MAPY];

//globals
Building GBuildings[BD_MUN];
Road GRoads[RD_MUN];

void initItems() {
	for (int i = 0; i < BD_MUN; i++) {
		Building* bd = new Building();
		int x = random(55, MAPX-55);
		int y = random(55, MAPY-55);
		float r = random(0, 88);
		int type = random(1, 4);
		//x *= 2; y *= 2;
		r /= 7;
		int ww = 8;
		if (i > 32 && type == 4)type = 3;
		if (type == 3)ww = 3;
		if (type > 5)type = 5;
		if (type == 5)
			*bd = Building(x, y, r, 4, 12, 1, type*16);
		else
			*bd = Building(x, y, r, ww+5+type, ww+3+type, 1, type*16);
		GBuildings[i] = *bd;
	}
	for (int i = 0; i < RD_MUN; i++) {
		Road* rd = new Road();
		int x = random(55, MAPX - 55);
		int y = random(55, MAPY - 55);
		float r = random(0, 22)/7.0;
		*rd = Road(x, y, r, 3, 16, 1);
		GRoads[i] = *rd;
	}
}

void bddf(int i, int trd) {
	GBuildings[i].diffuse(trd);
}

void rddf(int i, int trd) {
	GRoads[i].diffuse(trd);
}

int main() {
	initgraph(SCRX, SCRY);
	BeginBatchDraw();
	srand((unsigned)time(NULL));
	initmap();
	initItems();
	initBDdiffuse();
	initRDdiffuse();
	//buildings[0]= Building(500, 500, 0, 11, 11, 1, 0x10);
	//buildings[1]= Building(520, 500, 0, 11, 11, 1, 0x20);
	int count = 0;
	for (int i = 0; i < BD_MUN; i++) {
		GBuildings[i].write();
	}
	for (int i = 0; i < RD_MUN; i++) {
		GRoads[i].write();
	}
	while (true) {
		//cleardevice();
		putSpot(SCRX - 16, count, GREEN);
		clearMapBD();
		clearMapRD();
		clearMapFP();
		writeMapForce();
		for (int i = 0; i < BD_MUN; i++) {
			GBuildings[i].getList();
			GBuildings[i].write();
			GBuildings[i].clearForce();
		}
		for (int i = 0; i < RD_MUN; i++) {
			GRoads[i].getList();
			GRoads[i].write();
			GRoads[i].clearForce();
		}
		//-----------------------------------------------------------
		for (int i = 0; i < 1; i++) {
			thread thBD[8];
			for (int t = 0; t < 8; t++) {
				thBD[t] = thread(bddf, i * 8 + t, t);
			}
			for (int t = 0; t < 8; t++) {
				thBD[t].join();
			}
		}
		for (int i = 0; i < 2; i++) {
			thread thRD[8];
			for (int t = 0; t < 8; t++) {
				thRD[t] = thread(rddf, i * 8 + t, t);
			}
			for (int t = 0; t < 8; t++) {
				thRD[t].join();
			}
		}
		//-----------------------------------------------------------
		for (int i = 0; i < BD_MUN; i++) {
			GBuildings[i].getForce();
		}
		for (int i = 0; i < BD_MUN; i++) {
			GBuildings[i].forceMove();
		}
		for (int i = 0; i < RD_MUN; i++) {
			GRoads[i].getForce();
		}
		for (int i = 0; i < RD_MUN; i++) {
			GRoads[i].forceMove();
		}
		RenderMap();
		/*
		for (int i = 0; i < BD_MUN; i++) {
			line(buildings[i].x, buildings[i].y, buildings[i].x + 100*buildings[i].F0.x, buildings[i].y + 100*buildings[i].F0.y);
		}*/
		FlushBatchDraw();
		//Sleep(100);
		count++;
	}
	return 0;
}