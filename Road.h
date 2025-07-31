#pragma once

#include <graphics.h>
#include <conio.h>
#include <cstdint>
#include <forward_list>
#include "Map.h"
#include "PhySim.h"

#ifndef _Road_h_
#define _Road_h_

using namespace std;

void initRDdiffuse();

void clearRDdiffuse(int trd);

class Road {
public:
	int16_t x;
	int16_t y;
	float r;
	int8_t w;
	int8_t l;
	int8_t type;
	bool islife;
	vecForce F0;
	float Fr;
	float Fw;
	float Fl;
	vecForce v0;
	float vr;
	float vw;
	float vl;
	forward_list<loc> listMap;
	Road();
	Road(int16_t x0, int16_t y0, float r0, int8_t w0, int8_t l0, int8_t ty0);
	void getList();
	void clearForce();
	void getForce();
	void getVelocity();
	void forceMove();
	void write();
	void writeForce();
	void diffuse(int trd);
};

#endif