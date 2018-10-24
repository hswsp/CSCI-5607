#pragma once
#include"Vector.h"
class texture
{
public:
	int type;
	texture() {}
	texture(int atype):type(atype) {}
	virtual Vector value(float u, float v, const Vector&p)const = 0;//(u,v) is image pixel and P is hit position
};

class constant_texture :public texture
{
public:
	Vector color;
	constant_texture() {}
	constant_texture(Vector aColor):color(aColor) {}
	virtual Vector value(float u, float v, const Vector&p)const
	{
		return color;
	}
};
class checker_texture :public texture
{
public:
	texture *odd;
	texture *even;
	checker_texture():texture(1) {};
	checker_texture(texture *t0, texture*t1) :even(t0), odd(t1), texture(1)
	{

	}
	virtual Vector value(float u, float v, const Vector&p)const;
};

class image_texture :public texture
{
public:
	int nx, ny;//image resolution
	unsigned char*data;
	image_texture():texture(2){}
	image_texture(unsigned char * pixels, int a, int b) :data(pixels), nx(a), ny(b), texture(2) {}
	virtual Vector value(float u, float v, const Vector&p)const;
};

void get_sphere_uv(const Vector&p, float& u, float&v);