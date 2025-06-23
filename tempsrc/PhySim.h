#pragma once

#include <graphics.h>
#include <conio.h>
#include <cmath>

#ifndef _PhySim_h_
#define _PhySim_h_

class vecForce {
public:
	float x;
	float y;
	vecForce();
	vecForce(float Fx, float Fy);
	vecForce operator+(const vecForce& F2);
	vecForce operator*(const float& a);
	void operator+=(const vecForce& F2);
	void operator=(const vecForce& F2);
	float mol();
	void routate(float r);
	void sqrize();
	void sqrmol();
	void limit(float a);
	void putForce(int x1, int y1);
};

class polForce {
public:
	float r;
	float l;
	polForce();
	polForce(float Fr, float Fl);
	polForce operator+(const polForce& F2);
};


#endif
