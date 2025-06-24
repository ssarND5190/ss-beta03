#pragma once

#include <cstdint>
#include <forward_list>
#include "Map.h"
#include "PhySim.h"

#ifndef _Building_h_
#define _Building_h_

using namespace std;

void clearBDdiffuse();

class Building {
public:
	int16_t x;
	int16_t y;
	float r;
	int8_t w;
	int8_t l;
	int8_t decks;
	int8_t type;
	bool islife;
	double score;
	vecForce F0;
	float Fr;
	float Fw;
	float Fl;
	vecForce v0;
	float vr;
	float vw;
	float vl;
	forward_list<loc> listMap;
	Building();
	Building(int16_t x0, int16_t y0, float r0, int8_t w0, int8_t l0, int8_t d0, int8_t ty0);
	void aloc(int iw, int il);
	void getForce();
	void getVelocity();
	void forceMove();
	void gradianMove();
	void diffuse(int trd);
	void getScore();
	void getList();
	void getList2x();
	int getArea();
	int getIntersect(Building bd);
	int getDH();
	int getVH();
	void write();
	void writeVal();
	void writeForce();
};

#endif