#pragma once
#include<math.h>
#include<iostream>
using namespace std;
typedef unsigned int Color;
#define PI 3.141592653589793238462643383279502884197169399375
struct Vector
{
	float x, y, z;
	Vector(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
	Vector() {};
	Vector(const Vector& r) : x(r.x), y(r.y), z(r.z) {}

	float sqrLength() const;
	float length() const;
	bool operator == (const Vector&r)const;
	bool operator != (const Vector&r)const;
	Vector operator+(const Vector& r) const;
	Vector operator-(const Vector& r) const;
	Vector operator*(float v) const;
	Vector operator/(float v) const;
	Vector normalize() const;
	float dot(const Vector& r) const;
	Vector cross(const Vector& r) const;
	static Vector zero()
	{
		return Vector(0, 0, 0);
	}
	static Vector VNULL()
	{
		return Vector(INT_MIN, INT_MIN, INT_MIN);
	}
};
inline Vector operator*(float l, const Vector& r) { return r * l; }