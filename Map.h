#pragma once

#include <graphics.h>
#include <conio.h>
#include <cstdint>
#include <vector>

#ifndef _Map_h_
#define _Map_h_

class loc {
public:
	int16_t x;
	int16_t y;
	loc();
	loc(int lx, int ly);
};

class locN {
public:
	int16_t x;
	int16_t y;
	locN* nxN;
	locN();
	locN(int locx, int locy, locN* nextNode);
	bool end();
	locN operator++(int);
	void operator*=(locN& nextNode);
};

class locVal {
public:
	int16_t x;
	int16_t y;
	float val;
	locVal();
	locVal(int lx, int ly, float lval);
};

class locP {
public:
	int16_t x;
	int16_t y;
	int *pt;
	locP();
	locP(int lx, int ly, int* lp);
	bool operator<(const locP& locp)const;
	bool operator==(const locP& locp)const;
	bool operator>(const locP& locp)const;
};

void map8Dir(uint8_t dir, int* lx, int* ly);

void map16Dir(uint8_t dir, int* lx, int* ly);

void initmap();

void clearMapFP();

void clearMapBD();

void clearMapRD();

void clearMapVal1();

void clearMapVal2();

void writeMapForce();

void RenderMap();

void getMapOct(std::vector<loc>& vec, int x, int y, int r);

int getMapOct_BD(int x, int y, int r);

int getMapOctVH(int x, int y, int r);

float getMapOctVal1(int x, int y, int r);

#endif