#include "PhySim.h"
#include <cmath>

vecForce::vecForce()
{
	x = 0; y = 0;
}

vecForce::vecForce(float Fx, float Fy)
{
	x = Fx; y = Fy;
}

vecForce vecForce::operator+(const vecForce& F2)
{
	return vecForce(this->x + F2.x, this->y + F2.y);
}

vecForce vecForce::operator*(const float& a)
{
	return vecForce(this->x * a, this->y * a);
}

void vecForce::operator+=(const vecForce& F2)
{
	x += F2.x; y += F2.y;
}

void vecForce::operator=(const vecForce& F2)
{
	x = F2.x; y = F2.y;
}

float vecForce::mol()
{
	return sqrt(x * x + y * y);
}

void vecForce::routate(float r)
{
	float xx = x; float yy = y;
	x = xx * cos(r) - yy * sin(r);
	y = xx * sin(r) + yy * cos(r);
}

void vecForce::sqrize()
{
	if (x > 0)x = sqrt(x);
	if (x < 0)x = -sqrt(-x);
	if (y > 0)y = sqrt(y);
	if (y < 0)y = -sqrt(-y);
}

void vecForce::sqrmol()
{
	float m = mol();
	if (m == 0.0)return;
	float ratio = sqrt(m) / m;
	x *= ratio; y *= ratio;
}

void vecForce::limit(float a)
{
	float ratio = a / mol();
	if (ratio > 1.0)return;
	x *= ratio; y *= ratio;
}

void vecForce::putForce(int x1, int y1)
{
	line(x1, y1, 0.1*x + x1, 0.1*y + y1);
}

polForce::polForce()
{
	r = 0; l = 0;
}

polForce::polForce(float Fr, float Fl)
{
	r = Fr; l = Fl;
}

polForce polForce::operator+(const polForce& F2)
{
	float mixR = (this->r + F2.r) / 2;
	return polForce();
}
